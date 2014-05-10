/*
 * @(#)MidiFile.cpp 3.00 30 June 1999
 *
 * Copyright (c) 2000 Pete Goodliffe (pete@cthree.org)
 *
 * This file is part of TSE3 - the Trax Sequencer Engine version 3.00.
 *
 * This library is modifiable/redistributable under the terms of the GNU
 * General Public License.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; see the file COPYING. If not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include "tse3/MidiFile.h"

#include "tse3/Song.h"
#include "tse3/Track.h"
#include "tse3/Part.h"
#include "tse3/Phrase.h"
#include "tse3/TempoTrack.h"
#include "tse3/TimeSigTrack.h"
#include "tse3/KeySigTrack.h"
#include "tse3/PhraseList.h"
#include "tse3/PhraseEdit.h"
#include "tse3/Error.h"
#include "tse3/Progress.h"

#include <fstream>
#include <string>
#include <queue>
#include <math.h>

using namespace TSE3;

namespace
{
    const char *TSE3MESSAGE
        = "This file was created by TSE3 from Trax Software.";
}

/******************************************************************************
 * MidiFileImportIterator class
 *****************************************************************************/

namespace TSE3
{
    /**
     * The @ref PlayableIterator for the @ref MidiFileImport class.
     *
     * There are two ways to play a standard MIDI file. One is to create
     * a @ref MidiFileImport object, use it to generate a @ref Song and
     * then play that. You will also be able to edit the @ref Song and
     * save it again.
     *
     * The second way to play a MIDI file it to use the @ref MidiFileImport
     * class' @ref Playable interface - this will generate a
     * MidiFileImportIterator object. The advantage of this is that you do
     * not have to delay for the import process (which for large MIDI files
     * can be significant). On the other hand, the data will not be editable
     * or modifiable.
     *
     * @sect Important note
     *
     * Because of the nature of the ordering of events in standard MIDI files,
     * the MidiFileImportIterator does not generate completely valid
     * output. @ref MidiCommand_NoteOff events cannot be efficently matched
     * to their corresponding @ref MidiCommand_NoteOn so they are not
     * paired. This means that the @ref Transport class' note off buffering
     * will not work, and if you stop playback of the MIDI file before
     * it has reached the end you may be left with 'hanging notes'. The solution
     * to this is to send an all notes off message after the stop, using the
     * @ref Panic object.
     *
     * The you really want to ensure safe, proper playback you are encouraged
     * to use @ref MidiFileImport::import, which is a slower but safer
     * method of getting playback data.
     *
     * @short   MidiFileImport PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     MidiFileImport
     */
    class MidiFileImportIterator : public PlayableIterator,
                                   public Listener<MidiFileImportListener>
    {
        public:

            /**
             * Creates a @ref PlayableIterator for the @ref MidiFileImport
             * class.
             *
             * Setting the final parameter to true makes the iterator perform a
             * last clock value calculation, sending the information back to
             * it's @ref MidiFileImport parent. You do not need to use this
             * facility yourself, it is used internally by the
             * @ref MidiFileImport class.
             */
            MidiFileImportIterator(MidiFileImport *s, Clock c,
                                   bool lastClock = false);
            virtual ~MidiFileImportIterator();
            virtual void moveTo(Clock c);

            virtual void Notifier_Deleted(MidiFileImport *);

        protected:

            virtual void getNextEvent();

        private:

            void getNextChannelEvent(int channel);
            void importMeta(int channel);
            void calculateLastClock();

            /**
             * Reads a fixed length big endian value from position pos in
             * the file. Afterwards, pos is updated to point to the first byte
             * after the fixed value.
             *
             * @param pos    Position in file to read from.
             * @param length No bytes to read.
             */
            int readFixed(unsigned char *&pos, int length);

            /**
             * Reads a variable size value as defined by the MIDI file standard.
             * Afterwards, pos is updated to point to the first byte after the
             * variable value.
             *
             * @param pos Position in file to read from.
             */
            int readVariable(unsigned char *&pos);

            // File information
            MidiFileImport                 *mfi;
            unsigned char                 **mtrks;
            size_t                         *mtrksize;

            // Playback
            unsigned char                 **mtrkpos;
            Clock                          *mtrkclock;
            MidiCommand                    *mtrkcommand;
            int                            *mtrkstatus;
            int                            *mtrkchannel;
            int                            *mtrkport;
            int                             _source;  // where _next came from

        private:

            MidiFileImportIterator &operator=(const MidiFileImportIterator &);
            MidiFileImportIterator(const MidiFileImportIterator &);
    };
}

/*
 * Unfortunately, a lot of the stuff in this class looks pretty similar
 * to the MidiFileImport class, but not quite close enough that I can merge
 * the internal logic together. Sigh.
 */

MidiFileImportIterator::MidiFileImportIterator(MidiFileImport *mfi, Clock c,
                                               bool lastClock)
: mfi(mfi), _source(-1)
{
    // set up internal pointers
    mtrks       = new unsigned char*[mfi->fileNoMTrks];
    mtrksize    = new size_t[mfi->fileNoMTrks];
    mtrkpos     = new unsigned char*[mfi->fileNoMTrks];
    mtrkclock   = new Clock[mfi->fileNoMTrks];
    mtrkcommand = new MidiCommand[mfi->fileNoMTrks];
    mtrkstatus  = new int[mfi->fileNoMTrks];
    mtrkchannel = new int[mfi->fileNoMTrks];
    mtrkport    = new int[mfi->fileNoMTrks];

    size_t pos = mfi->fileHeaderSize;

    size_t mtrkNo = 0;
    while (pos < size_t(mfi->fileSize))
    {
        if (!strncmp((char *) mfi->file+pos, "MTrk", 4))
        {
            if (mtrkNo < mfi->fileNoMTrks)
            {
                mtrks[mtrkNo] = mfi->file+pos;
                pos += 4;
                mtrksize[mtrkNo] = mfi->readFixed(pos, 4);
                pos += mtrksize[mtrkNo];
            }
            ++mtrkNo;
        }
        else
        {
            pos += 4;
            int length = mfi->readFixed(pos, 4);
            pos += length;
        }
    }

    if (lastClock)
    {
        calculateLastClock();
    }

    moveTo(c);
    attachTo(mfi);
}


MidiFileImportIterator::~MidiFileImportIterator()
{
    mfi = 0;
    delete mtrks;
    delete mtrksize;
    delete mtrkpos;
    delete mtrkclock;
    delete mtrkcommand;
    delete mtrkstatus;
    delete mtrkchannel;
    delete mtrkport;
}


void MidiFileImportIterator::moveTo(Clock /*c*/)
{
    _more   = true;
    _source = -1;
    for (size_t c = 0; c < mfi->fileNoMTrks; ++c)
    {
        mtrkpos[c]     = mtrks[c]+8;
        mtrkclock[c]   = 0;
        mtrkstatus[c]  = MidiCommand_NoteOn;
        mtrkchannel[c] = 0;
        mtrkport[c]    = 0;
        getNextChannelEvent(c);
    }
    getNextEvent();
}


void MidiFileImportIterator::getNextEvent()
{
    if (_source != -1 && mtrkpos[_source] < mtrks[_source]+mtrksize[_source])
    {
        getNextChannelEvent(_source);
    }

    Clock c = -1;
    _source = -1;
    for (size_t n = 0; n < mfi->fileNoMTrks; ++n)
    {
        if (mtrkpos[n] < mtrks[n]+mtrksize[n])
        {
            if (c == -1 || mtrkclock[n] < c)
            {
                c = mtrkclock[n];
                _source = n;
            }
        }
    }

    if (_source == -1)
    {
        _more = false;
    }
    else
    {
        _more = true;
        _next = MidiEvent(mtrkcommand[_source],
                          Clock::convert(mtrkclock[_source], mfi->filePPQN));
    }
}


void MidiFileImportIterator::getNextChannelEvent(int c)
{
    if (mtrkpos[c] < mtrks[c]+mtrksize[c])
    {
        Clock deltaTime = readVariable(mtrkpos[c]);
        mtrkclock[c] += deltaTime;

        // Get first byte of event, and deal with possible running status
        if (*(mtrkpos[c]) & 0x80)
        {
            mtrkstatus[c]  = *(mtrkpos[c]) >> 4;
            mtrkchannel[c] = *(mtrkpos[c]) & 0xf;
            ++mtrkpos[c];
        }
        if (mtrkstatus[c] == MidiCommand_System
            && (mtrkchannel[c] == 0x0 || mtrkchannel[c] == 0x7))
        {
            // Sysex event
            int sysexLength;
            sysexLength = readVariable(mtrkpos[c]);
            mtrkpos[c] += sysexLength;
        }
        else if (mtrkstatus[c] == MidiCommand_System && mtrkchannel[c] == 0xf)
        {
            // Meta events
            importMeta(c);
        }
        else
        {
            // MIDI event
            int data1 = 0, data2 = 0;
            switch (mtrkstatus[c])
            {
                case MidiCommand_ProgramChange:
                case MidiCommand_ChannelPressure:
                    data1 = *(mtrkpos[c]++);
                    data2 = 0;
                    break;
                case MidiCommand_NoteOff:
                case MidiCommand_NoteOn:
                case MidiCommand_KeyPressure:
                case MidiCommand_ControlChange:
                case MidiCommand_PitchBend:
                    data1 = *(mtrkpos[c]++);
                    data2 = *(mtrkpos[c]++);
                    break;
            }
            mtrkcommand[c]
                = MidiCommand(mtrkstatus[c], mtrkchannel[c], mtrkport[c],
                              data1, data2);
        }
    }
    else
    {
        mtrkcommand[c] = MidiCommand();
    }
}


void MidiFileImportIterator::importMeta(int c)
{
    int metaType = *(mtrkpos[c]++);
    int length   = readVariable(mtrkpos[c]);

    switch (metaType)
    {
        case 0x21: // MIDI port
        {
            mtrkport[c] = *(mtrkpos[c]);
            break;
        }
        case 0x51: // Tempo
        {
            int tempo;
            tempo = readFixed(mtrkpos[c], 3);
            length -= 3;
            tempo = 60000000/tempo; // convert to BPM
            mtrkcommand[c] = MidiCommand(MidiCommand_TSE_Meta, 0, 0,
                                         MidiCommand_TSE_Meta_Tempo,
                                         tempo);
            break;
        }
        case 0x58: // Time signature
        {
            int nn; nn = *(mtrkpos[c]++);
            int dd; dd = *(mtrkpos[c]++);
            int cc; cc = *(mtrkpos[c]++);
            int bb; bb = *(mtrkpos[c]++);
            length -= 4;
            dd = (int) pow((float)2, dd);
            mtrkcommand[c] = MidiCommand(MidiCommand_TSE_Meta, 0, 0,
                                         MidiCommand_TSE_Meta_TimeSig,
                                         (nn << 4) | dd);
            break;
        }
        case 0x59: // Key signature
        {
            int sf; sf = *(mtrkpos[c]++);
            int mi; mi = *(mtrkpos[c]++);
            length -= 2;
            mtrkcommand[c] = MidiCommand(MidiCommand_TSE_Meta, 0, 0,
                                         MidiCommand_TSE_Meta_KeySig,
                                         (sf << 4) | mi);
            break;
        }
    }
    mtrkpos[c] += length;
}


void MidiFileImportIterator::Notifier_Deleted(MidiFileImport *)
{
    mfi = 0;
    moveTo(0);
}


int MidiFileImportIterator::readFixed(unsigned char *&pos, int length)
{
    int value = 0;
    while (length-- > 0 && pos < mfi->file+mfi->fileSize)
        value = (value << 8) + *(pos++);
    return value;
}


int MidiFileImportIterator::readVariable(unsigned char *&pos)
{
    char c;
    int value = 0;
    if ((value = *(pos++)) & 0x80)
    {
        value &= 0x7f;
        do
        {
            value = (value << 7) + ((c = *(pos++)) & 0x7f);
        } while (c & 0x80);
    }
    return value;
}


void MidiFileImportIterator::calculateLastClock()
{
    if (mfi->fileLastClock == -1)
    {
        moveTo(0);
        Clock lastClock = 0;
        for (size_t n = 0; n < mfi->fileNoMTrks; ++n)
        {
            while (mtrkpos[n] < mtrks[n]+mtrksize[n])
            {
                getNextChannelEvent(n);
            }
            if (mtrkclock[n] > lastClock) lastClock = mtrkclock[n];
        }
        mfi->fileLastClock = lastClock;
    }
}


/******************************************************************************
 * MidiFileImport class
 *****************************************************************************/

MidiFileImport::MidiFileImport(const std::string &fn, int v, std::ostream &o)
: filename(fn), verbose(v), out(o), file(0), fileLastClock(-1)
{
    std::ifstream in(filename.c_str(), std::ios::binary | std::ios::in);
    if (!in.good())
    {
        throw MidiFileImportError("Source MIDI file will not open.");
    }

    in.seekg(0, std::ios::end);
    fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    try
    {
        file = new unsigned char[fileSize];
    }
    catch (std::bad_alloc)
    {
        throw MidiFileImportError("Out of memory loading MIDI file.");
    }

    // OK, this is relatively evil, we're reading a char stream into
    // an unsigned char buffer. However, we opened the file binary and now
    // cross our fingers. We can't easily use a ifstream<unsigned char>
    // since it's not instantiated by default in existing standard C++
    in.read(reinterpret_cast<char*>(file), fileSize);
    if (in.gcount() != fileSize)
        throw MidiFileImportError("Error loading MIDI file.");

    if (verbose >= 1)
          out << "Loaded source MIDI file into memory.\n"
              << "  Filename: " << filename << "\n"
              << "  File size: " << fileSize << "\n"
              << "Reading header information\n";

    // Read the MIDI file header
    loadHeader();
}


MidiFileImport::~MidiFileImport()
{
    delete [] file;
}


Song *MidiFileImport::load(Progress *progress)
{
    Song *song = new Song(0);

    if (verbose >= 1) out << "Importing MIDI file...\n\n";

    // Read the MIDI file header
    loadHeader();

    size_t pos = fileHeaderSize;

    size_t mtrkNo  = 0;

    // Set the Tempo and TimeSig to the defaults
    // (already done in their constructors)

    // Read the chunks
    const size_t PROGRESS_OFFSET = 10;
    if (progress)
    {
        // We add PROGRESS_OFFSET to display the fact that the file
        // has already been loaded into memory, and this may have taken quite
        // a while already.

        progress->progressRange(0, size_t(fileSize) + PROGRESS_OFFSET);
    }
    while (pos < size_t(fileSize))
    {
        if (progress)
        {
            progress->progress(pos + PROGRESS_OFFSET);
        }

        if (!strncmp((char *) file+pos, "MTrk", 4))
        {
            if (mtrkNo >= fileNoMTrks)
            {
                static int message = 0;
                if (verbose >= 1 && !message)
                {
                    out << "\n*** More MTrks exist in the MIDI file than the "
                        << "file header says. Continuing... ***\n\n";
                    message = 1;
                }
            }
            loadMTrk(pos, song, mtrkNo);
            ++mtrkNo;
        }
        else
        {
            if (verbose >= 1)
            {
                out << "Unknown chunk type '"
                    << file[pos]
                    << file[pos+1]
                    << file[pos+2]
                    << file[pos+3];
            //pos += 4;
            //int length = readFixed(pos, 4);
            //if (verbose >= 1)
                //out << "' of length " << length
                //    << " at position " << pos-8 << "/" << fileSize
                out << "; skipping.\n";
            //pos += length;
            }

            // Fix: Gopalakrishna Palem
            // We are off course... seek the end of present track or Start of next track
            const unsigned char szTrk[] = "MTrk";
            const unsigned char szEOT[] = { 0xff, 0x2f, 0x00 };
            while(pos + 2 < (size_t)fileSize)
            {
                unsigned char* pChar = file + pos;
                if(pChar[0] == szEOT[0] && pChar[1] == szEOT[1] && pChar[2] == szEOT[2]) { pos += 3; break; }
                if(pChar[0] == szTrk[0] && pChar[1] == szTrk[1] && pChar[2] == szTrk[2]) { break ;}
                pos ++;
            }
        }
    }

    if (verbose >= 1)
        out << "\nImported MIDI file successfully."
            << "  Projected " << fileNoMTrks << " MTrks, got " << mtrkNo
            << ".\n\n";

    return song;
}


void MidiFileImport::loadHeader()
{
    size_t pos = 0;

    // Read the first 4 byte ID, and if RIFF, skip the extra header prelimaries
    if (strncmp((char *) file+pos, "RIFF", 4) == 0)
    {
        pos += 4;
        int value = readFixed(pos, 4);
        if (verbose >= 2)
            out << "RIFF file header: size " << value << "bytes\n";
        if (strncmp((char *) file+pos, "RMID", 4) != 0)
            throw MidiFileImportError("RIFF file is not of type RMID");
        pos += 12;
    }

    // Read the header chunk (MThd)
    if (strncmp((char *) file+pos, "MThd", 4) != 0)
        throw MidiFileImportError("No MThd chunk header (not a MIDI file)");
    pos +=4;
    if (verbose >= 1) out << "Reading MThd header chunk.\n";

    // Get the MThd chunk size, which should be 6
    int mthdSize = readFixed(pos, 4);
    if (mthdSize != 6)
        throw MidiFileImportError("Length of MThd chunk was not 6.");

    // Get the format number
    fileFormat = readFixed(pos, 2);
    if (fileFormat < 0 || fileFormat > 2)
        throw MidiFileImportError("Unknown MIDI file format (not 0, 1, or 2).");
    if (verbose >= 2) out << "  MIDI file format: " << fileFormat << "\n";

    // Get the number of tracks in the file
    fileNoMTrks = readFixed(pos, 2);
    if (verbose >= 2) out << "  No MTrks: " << fileNoMTrks << "\n";

    // Get the PPQN
    filePPQN = readFixed(pos, 2);
    if (verbose >= 2) out << "  PPQN: " << filePPQN << "\n";

    fileHeaderSize = pos;
}


void MidiFileImport::loadMTrk(size_t &pos, Song *song, int mtrkNo)
{
    // We are assured that the chunk is correct: Skip 'MTrk'
    pos += 4;

    // Get the chunk length
    int length = readFixed(pos, 4);
    if (verbose >= 1) out << "Reading MTrk, length " << length << "\n";
    size_t maxPos = pos+length;
    if (maxPos > size_t(fileSize))
    {
        throw MidiFileImportError("MTrk has invalid size.");
    }

    // Set up and get ready to parse the MIDI data
    PhraseEdit   phraseEdit;
    Track       *track      = new Track();
    Clock        time       = 0;                  // Times held in file's PPQN
    Clock        end        = 0;
    unsigned int status     = MidiCommand_NoteOn; // Running status byte
    int          channel    = 0;                  // Running status channel
    int          port       = 0;

    while (pos < maxPos)
    {
        Clock deltaTime = readVariable(pos);
        time += deltaTime;
        end = (time > end) ? time : end;
        // Get first byte of event, and deal with possible running status
        if (file[pos] & 0x80)
        {
            status  = file[pos] >> 4;
            channel = file[pos] & 0xf;
            ++pos;
        }
        if (status == MidiCommand_System && (channel == 0x0 || channel == 0x7))
        {
            // Sysex events
            if (verbose >= 2) out << "  sysex event: skipped\n";
            int sysexLength;
            sysexLength = readVariable(pos);
            pos += sysexLength;
        }
        else if (status == MidiCommand_System && channel == 0xf)
        {
            // Meta events
            loadMeta(pos, song, track, mtrkNo, time, port, end);
        }
        else
        {
            // MIDI event
            int data1 = 0, data2 = 0;
            switch (status)
            {
                case MidiCommand_ProgramChange:
                case MidiCommand_ChannelPressure:
                    data1 = file[pos++];
                    data2 = 0;
                    break;
                case MidiCommand_NoteOff:
                case MidiCommand_NoteOn:
                case MidiCommand_KeyPressure:
                case MidiCommand_ControlChange:
                case MidiCommand_PitchBend:
                    data1 = file[pos++];
                    data2 = file[pos++];
                    break;
            }
            if (verbose >= 3)
                out << "  MIDI command: 0x" << std::hex << status
                    << "(" << channel << "," << port
                    << "),0x" << data1
                    << ",0x" << data2 << std::dec
                    << " at " << time << "\n";
            phraseEdit.insert(MidiEvent(MidiCommand(status, channel, port,
                                                     data1, data2),
                                        Clock::convert(time, filePPQN)));
        }
    }

    if (phraseEdit.size() == 0)
    {
        if (verbose >= 1) out << "  No MIDI data in this MTrk\n";
        delete track;
        return;
    }

    if (verbose >= 1) out << "  Placing Phrase, Part, and Track into Song.\n";

    // Create a Phrase from this data
    phraseEdit.tidy();
    Phrase *phrase = phraseEdit.createPhrase(song->phraseList(),
        song->phraseList()->newPhraseTitle(PhraseList::importedString));
    if (verbose >= 2) out << "    Phrase title: " << phrase->title() << "\n";

    // Create a Part for this Phrase and place it in th Track
    Part *part = new Part();
    part->setStart(0);
    part->setEnd(Clock::convert(end, filePPQN));
    part->setPhrase(phrase);
    track->insert(part);
    if (verbose >= 2) out << "    Part between: 0 and " << part->end() << "\n";

    // Place the Track in the Song
    song->insert(track);

    if (verbose >= 1) out << "  MTrk loaded successfully\n";
}


void MidiFileImport::loadMeta(size_t &pos, Song *song, Track *track,
                              int mtrkNo, Clock time, int &port,
                              Clock &end)
{
    if (verbose >= 2) out << "  Meta event: ";

    int metaType = file[pos++];
    int length   = readVariable(pos);

    if (verbose >= 3)
        out << "(type: 0x" << std::hex << metaType << std::dec
            << ", length:" << length << ") ";

    switch (metaType)
    {
        case 0x21: // MIDI port
        {
            port = file[pos];
            if (verbose >= 2) out << "midi port (" << port << ")";
            break;
        }
        case 0x2f: // End of track
        {
            if (verbose >= 2) out << "end track marker at time " << time;
            end = time;
            break;
        }
        case 0x51: // Tempo
        {
            int tempo;
            tempo = readFixed(pos, 3);
            length -= 3;
            tempo = 60000000/tempo; // convert to BPM
            if (verbose >= 2) out << "tempo event (" << tempo << ")";
            if (time == 0)
                song->tempoTrack()->erase((*song->tempoTrack())[0]);
            song->tempoTrack()->insert(Event<Tempo>(Tempo(tempo),
                                       Clock::convert(time, filePPQN)));
            break;
        }
        case 0x58: // Time signature
        {
            int nn; nn = file[pos++];
            int dd; dd = file[pos++];
            int cc; cc = file[pos++];
            int bb; bb = file[pos++];
            length -= 4;
            dd = (int) pow((float)2, dd);
            if (verbose >= 2)
                out << "timesig event (" << nn << "/" << dd << ")";
            if (time == 0)
                song->timeSigTrack()->erase((*song->timeSigTrack())[0]);
            song->timeSigTrack()->insert(Event<TimeSig>(TimeSig(nn, dd),
                                         Clock::convert(time, filePPQN)));
            break;
        }
        case 0x59: // Key signature
        {
            if (verbose >= 2) out << "";
            int sf; sf = file[pos++];
            int mi; mi = file[pos++];
            length -= 2;
            if (verbose >= 2)
                out << "keysig event (" << sf << "-" << mi << ")";
            if (time == 0)
                song->keySigTrack()->erase((*song->keySigTrack())[0]);
            song->keySigTrack()->insert(Event<KeySig>(KeySig(sf, mi),
                                        Clock::convert(time, filePPQN)));
            break;
        }
        case 0x00: // Sequence number
        {
            if (verbose >= 2) out << "sequence number (skipping)\n";
            break;
        }
        case 0x01: // Text event
        {
            if (verbose >= 2) out << "text event (skipping)\n";
            break;
        }
        case 0x03: // Sequence/Track name
        {
            {
                std::string title;
                int n = 0;
                while (n < length)
                {
                    title += char(file[pos+n]);
                    ++n;
                }
                if (verbose >= 2)
                    out << "sequence/track name: (" << title << ")";
                if ((mtrkNo == 0 && fileFormat == 1) || fileFormat == 0)
                {
                    if (verbose >= 2) out << " (sequence name)";
                    song->setTitle(title);
                }
                else
                {
                    if (verbose >= 2) out << " (track name)";
                    track->setTitle(title);
                }
            }
            break;
        }
        case 0x02: // Copyright notice
        {
            if (verbose >= 2) out << "copyright notice";
            song->setCopyright((char *) file+pos);
            break;
        }
        case 0x04: // Instrument name
        {
            if (verbose >= 2) out << "instrument name (skipping)";
            break;
        }
        case 0x05: // Lyric
        {
            if (verbose >= 2) out << "lyric (skipping)";
            break;
        }
        case 0x06: // Marker
        {
            if (verbose >= 2) out << "marker (skipping)";
            break;
        }
        case 0x07: // Cue point
        {
            if (verbose >= 2) out << "cue point (skipping)";
            break;
        }
        case 0x54: // sMPTE offset
        {
            if (verbose >= 2) out << "SMPTE offset (skipping)";
            break;
        }
        case 0x7f: // Proprietary
        {
            if (verbose >= 2) out << "proprietary (skipping)";
            break;
        }
        default:
        {
            if (verbose >= 2) out << "unrecognised (skipping)";
            break;
        }
    }

    // Add on the remaining length of the meta event
    pos += length;

    if (verbose >= 2) out << "\n";
}


int MidiFileImport::readFixed(size_t &pos, int length)
{
    int value = 0;
    while (length-- > 0 && pos < size_t(fileSize))
        value = (value << 8) + file[pos++];
    return value;
}


int MidiFileImport::readVariable(size_t &pos)
{
    char c;
    int value = 0;
    if ((value = file[pos++]) & 0x80)
    {
        value &= 0x7f;
        do
        {
            value = (value << 7) + ((c = file[pos++]) & 0x7f);
        } while (c & 0x80);
    }
    return value;
}


PlayableIterator *MidiFileImport::iterator(Clock index)
{
    return new MidiFileImportIterator(this, index, true);
}


Clock MidiFileImport::lastClock() const
{
    if (fileLastClock == -1)
    {
        // Let the iterator do the calculation for us
        MidiFileImportIterator(const_cast<MidiFileImport*>(this), 0, true);
    }
    return fileLastClock;
}


/******************************************************************************
 * TempoKeyTimeSigTrackIterator class
 *****************************************************************************/

/**
 * This class is used internally by MidiFileExport. Since type 1 MIDI files
 * have their first MTrk as a special meta track containing tempo, key and
 * timesig events, we need this class which acts as an iterator over the group
 * of these tracks allowing us to insert them into an MTrk in time order.
 *
 * @short   TempoTrack, KeySigTrack and TimeSigTrack combined iterator.
 * @author  Pete Goodliffe
 * @version 3.00
 * @see     Song
 */
class TempoKeyTimeSigTrackIterator : public PlayableIterator
{
        public:

            TempoKeyTimeSigTrackIterator(Song *t, Clock c);
            virtual ~TempoKeyTimeSigTrackIterator();
            virtual void moveTo(Clock c);

        protected:

            virtual void getNextEvent();

            PlayableIterator *tempo;
            PlayableIterator *timesig;
            PlayableIterator *keysig;

            enum {fromNone, fromTempo, fromKeySig, fromTimeSig} from;

        private:

            TempoKeyTimeSigTrackIterator &operator=
                (const TempoKeyTimeSigTrackIterator &);
            TempoKeyTimeSigTrackIterator
                (const TempoKeyTimeSigTrackIterator &);
};



TempoKeyTimeSigTrackIterator::TempoKeyTimeSigTrackIterator(Song *s, Clock c)
{
    tempo   = s->tempoTrack()->iterator(c);
    timesig = s->timeSigTrack()->iterator(c);
    keysig  = s->keySigTrack()->iterator(c);
    from    = fromNone;
    moveTo(c);
}


TempoKeyTimeSigTrackIterator::~TempoKeyTimeSigTrackIterator()
{
    delete tempo;
    delete timesig;
    delete keysig;
}


void TempoKeyTimeSigTrackIterator::moveTo(Clock c)
{
    tempo->moveTo(c);
    timesig->moveTo(c);
    keysig->moveTo(c);
    _more = true;
    from  = fromNone;
    getNextEvent();
}


void TempoKeyTimeSigTrackIterator::getNextEvent()
{
    _more = true;

    switch (from)
    {
        case fromNone:                  break;
        case fromTempo:   ++(*tempo);   break;
        case fromKeySig : ++(*keysig);  break;
        case fromTimeSig: ++(*timesig); break;
    }

    _next = MidiEvent();

    if (tempo->more())
    {
        _next = **tempo;
        from = fromTempo;
    }

    if (timesig->more())
    {
        MidiEvent n = **timesig;
        if (_next.data.status == MidiCommand_Invalid || n.time < _next.time)
        {
            _next = n;
            from = fromTimeSig;
        }
    }

    if (keysig->more())
    {
        MidiEvent n = **keysig;
        if (_next.data.status == MidiCommand_Invalid || n.time < _next.time)
        {
            _next = n;
            from = fromKeySig;
        }
    }

    if (!tempo->more() && !timesig->more() && !keysig->more()) _more = false;
}


/******************************************************************************
 * MidiFileExport class
 *****************************************************************************/

MidiFileExport::MidiFileExport(int           f,
                               bool          compact,
                               int           v,
                               std::ostream &diag_out)
: verbose(v), diag_out(diag_out), _format(f),
  _compact(compact), _size(0)
{
}


MidiFileExport::~MidiFileExport()
{
}


void MidiFileExport::save(const std::string &filename, Song *s,
                          Progress *progress)
{

    std::ofstream out(filename.c_str(),
                      std::ios::binary | std::ios::out | std::ios::trunc);
    if (!out.good())
    {
        throw MidiFileExportError("Output MIDI file will not open.");
    }

    save(out, s, progress);

    out.close();
}


void MidiFileExport::save(std::ostream &out, Song *s, Progress *progress)
{
    if (verbose)
    {
        diag_out << "MIDI file export\n"
                 << "================\n\n";
    }

    song = s;

    if (progress)
    {
        progress->progressRange(0, song->size() + 2);
    }

    _size = 0;

    // Write MThd header chunk
    writeString(out, "MThd");                  // header chunk tag
    writeFixed(out, 6, 4);                     // header chunk length
    writeFixed(out, _format, 2);               // type of MIDI file
    writeFixed(out, 0, 2);                     // no MTrks in file
    writeFixed(out, Clock::PPQN, 2);           // file PPQN
    _size += 10;

    if (verbose) diag_out << "Writing MThd chunk\n";
    if (verbose >= 2)
    {
        diag_out << "  MThd chunk details are\n";
        diag_out << "      format   " << _format << "\n";
        diag_out << "      No MTrks <to be filled>\n";
        diag_out << "      PPQN     " << Clock::PPQN << "\n";
    }
    if (verbose) diag_out << "\n";

    _noMTrks = 0;

    // Save MTrks: different algorithm for the different MIDI file types

    if (_format == 0)
    {
        writeMTrk(out, song->iterator(0));
    }
    else if (_format == 1)
    {
        // Meta data MTrk
        {
            TempoKeyTimeSigTrackIterator meta(song, 0);
            if (verbose)
                diag_out << "Writing first MTrk with meta information\n";
            writeMTrk(out, &meta, "Tempo/TimSig info");
        }
        if (progress)
        {
            progress->progress(1);
        }

        // Ordinary Song Tracks
        for (size_t n = 0; n < song->size(); ++n)
        {
            if (verbose)
                diag_out << "Writing Song's Track " << n+1
                         << "/" << song->size() << "\n";
            PlayableIterator *i = (*song)[n]->iterator(0);
            writeMTrk(out, i, (*song)[n]->title());
            delete i;
            if (progress)
            {
                progress->progress(n+2);
            }
        }
    }

    if (progress)
    {
        progress->progress(song->size()+2);
    }

    // Finally, write the number of MTrks in the file header
    out.seekp(10, std::ios::beg);
    _size -= 2;
    writeFixed(out, _noMTrks, 2);
    if (verbose >= 2)
        diag_out << "No MTrks in this file = " << _noMTrks << "\n\n";

    if (verbose) diag_out << "Export done.\n\n\n";
}


void MidiFileExport::writeMTrk(std::ostream &out, PlayableIterator *iterator,
                               const std::string &title)
{
    ++_noMTrks;
    if (verbose >= 2) diag_out << "  (This is MTrk #" << _noMTrks << ")\n";

    // Write the MTrk header and remember the file position of it so that
    // the length can be written back in later
    MTrkPos = out.tellp();
    writeString(out, "MTrk");  // MTrk chunk tag
    writeString(out, "Arse");  // dummy length value (written later)
    _size += 8;

    // Set up some values ready for MTrk generation
    MTrkSize       = 0;
    runningStatus  = MidiCommand_Invalid;
    lastEventClock = 0;

    // Write the things at the front of the MTrk
    if (_noMTrks == 1)
    {
        // Sequence Name
        writeVariable(out, 0);    // delta time of zero
        writeFixed(out, 0xff, 1); // meta event
        writeFixed(out, 0x03, 1); // sequence name
        writeVariable(out, song->title().size()+1); // len
        writeString(out, song->title(), true);
        if (verbose >= 2)
            diag_out << "  Wrote sequence name: '" << song->title() << "'\n";

        // Copyright Notice
        writeVariable(out, 0);    // delta time of zero
        writeFixed(out, 0xff, 1); // meta event
        writeFixed(out, 0x02, 1); // copyright notice
        writeVariable(out, song->copyright().size()+1); // len
        writeString(out, song->copyright(), true);
        if (verbose >= 2)
            diag_out << "  Wrote copyright notice: '" << song->copyright()
                     << "'\n";

        // TSE3 text message
        writeVariable(out, 0);    // delta time of zero
        writeFixed(out, 0xff, 1); // meta event
        writeFixed(out, 0x01, 1); // text event
        writeVariable(out, std::strlen(TSE3MESSAGE)+1); // len
        writeString(out, TSE3MESSAGE, true);
    }
    else
    {
        // Sequence Name
        writeVariable(out, 0);    // delta time of zero
        writeFixed(out, 0xff, 1); // meta event
        writeFixed(out, 0x03, 1); // sequence name
        writeVariable(out, title.size()+1); // len
        writeString(out, title, true);
        if (verbose >= 2)
            diag_out << "  Wrote MTrk name: '" << title << "'\n";
    }

    writeMTrk_outputLoop(out, iterator);

    // write the End of Track command
    writeVariable(out, 0);
    writeFixed(out, 0xff,1); // meta event
    writeFixed(out, 0x2f,1); // End of Track
    writeVariable(out, 0);   // length of meta event
    if (verbose == 3) diag_out << "  Wrote end of track meta event\n";

    // Calculate the length of this MTrk and write it in the header
    std::streampos currentpos = out.tellp();
    out.seekp(MTrkPos+std::streampos(4), std::ios::beg);
    _size -= 4;
    writeFixed(out, MTrkSize, 4);
    out.seekp(currentpos, std::ios::beg);

    if (verbose >= 2) diag_out << "\n";
}


void MidiFileExport::writeMTrk_outputLoop(std::ostream &out,
                                          PlayableIterator *iterator)
{
    /*
     * This export algorithm is remarkably similar to the transport
     * output loop. The main difference is that there is no metronome
     * and we ignore looping requests etc.
     */

    // The noteOffBuffer that holds MidiCommand_NoteOffs that need to be
    // inserted at the appropriate time.
    std::priority_queue<MidiEvent,std::vector<MidiEvent>,
                        std::greater<MidiEvent> >
        noteOffBuffer;

        /*
         * Get the next event, which may come from the Playable or from the
         * noteOffBuffer.
         *
         * The following enum allows us to remember where events came from.
         */
        enum
        {
            from_None,
            from_Playable,
            from_NOBuff
        } from = from_None;

    do
    {

        from = from_None;

        MidiEvent e;

        // Get the next Playable event, if there is one
        if (iterator->more())
        {
            e    = **iterator;
            from = from_Playable;
        }

        // Get the next noteOffBuffer event, if there is one
        if (!noteOffBuffer.empty())
        {
            const MidiEvent &e2 = noteOffBuffer.top();
            if (from == from_None || e2.time < e.time)
            {
                e    = e2;
                from = from_NOBuff;
            }
        }

        if (e.data.status >= MidiCommand_NoteOff)
        {
            /*
             * We have a MidiEvent to send to the scheduler. We do
             * this and then consume it from whichever source it came.
             *
             * If it is a MidiCommand_NoteOn, we put the
             * MidiCommand_NoteOff in a buffer to be sent later.
             */
            writeMidiEvent(out, e);
            if (e.data.status == MidiCommand_NoteOn
                && e.data.data2 != 0)
            {
                MidiEvent offEvent(e.offData, e.offTime);
                if (_compact)
                {
                    offEvent.data.status = MidiCommand_NoteOn;
                    offEvent.data.data2  = 0;
                }
                noteOffBuffer.push(offEvent);
            }
        }
        else if (e.data.status == MidiCommand_TSE_Meta)
        {
            /*
             * We have a TSE MetaEvent. Depending on it's type, do
             * something productive.
             */
            switch (e.data.data1)
            {
                case MidiCommand_TSE_Meta_MoveTo:
                    break;
                case MidiCommand_TSE_Meta_Tempo:
                case MidiCommand_TSE_Meta_TimeSig:
                case MidiCommand_TSE_Meta_KeySig:
                    writeMetaEvent(out, e);
                    break;
            }
        }

        switch (from)
        {
            case from_None:                           break;
            case from_Playable: ++*iterator;          break;
            case from_NOBuff:   noteOffBuffer.pop();  break;
        }

    } while (from != from_None);
}


void MidiFileExport::writeMidiEvent(std::ostream &out, const MidiEvent &e)
{
    if (e.data.status < MidiCommand_NoteOff) return;

    if (verbose == 3)
        diag_out << "  Writing MIDI event time=" << e.time
                 << ", status="                  << e.data.status
                 << ", channel="                 << e.data.channel
                 << ", data1="                   << e.data.data1
                 << ", data2="                   << e.data.data2;

    // Delta time
    writeVariable(out, e.time-lastEventClock);
    lastEventClock = e.time;

    // Status byte (if needed - running status might obviate it)
    unsigned int statusByte = (e.data.status << 4) + e.data.channel;
    if (statusByte != runningStatus)
    {
        writeFixed(out, statusByte, 1);
        runningStatus = statusByte;
    }
    else if (verbose == 3)
    {
        diag_out << " (used running status)";
    }
    if (verbose == 3) diag_out << "\n";

    // Data bytes (if needed)
    switch (e.data.status)
    {
        case MidiCommand_NoteOff: // Two data bytes
        case MidiCommand_NoteOn:
        case MidiCommand_KeyPressure:
        case MidiCommand_ControlChange:
        case MidiCommand_PitchBend:
            writeFixed(out, e.data.data1, 1);
            writeFixed(out, e.data.data2, 1);
            break;

        case MidiCommand_ProgramChange: // One data byte
        case MidiCommand_ChannelPressure:
            writeFixed(out, e.data.data1, 1);
            break;

        // For the moment I'm ingoring system commands
    }
}


void MidiFileExport::writeMetaEvent(std::ostream &out, const MidiEvent &e)
{
    // Delta time - what if not recognied?
    writeVariable(out, e.time-lastEventClock);
    lastEventClock = e.time;

    switch (e.data.data1)
    {
        case MidiCommand_TSE_Meta_Tempo:
            {
                int tempo = e.data.data2;
                writeFixed(out, 0xff, 1); // meta event
                writeFixed(out, 0x51, 1); // set tempo
                writeVariable(out, 3);    // length
                writeFixed(out, 60000000/tempo, 3); // tempo
                if (verbose == 3)
                    diag_out << "  Writing tempo event time=" << e.time
                             << ", tempo=" << tempo << "\n";
            }
            break;

        case MidiCommand_TSE_Meta_TimeSig:
            {
                int top     = e.data.data2 >> 4;
                int bottom  = e.data.data2 & 0xf;
                int bottom2 = bottom;
                int dd = 1; while (bottom2 > 2) { ++dd; bottom2/=2; }
                writeFixed(out, 0xff, 1); // meta event
                writeFixed(out, 0x58, 1); // set time signature
                writeVariable(out, 4);    // length
                writeFixed(out, top, 1);  // nn
                writeFixed(out, dd, 1);   // dd
                writeFixed(out, 24, 1);   // cc
                writeFixed(out, 24, 1);   // bb
                if (verbose == 3)
                    diag_out << "  Writing timesig event time=" << e.time
                             << ", timesig=" << top << "/" << bottom << "\n";
            }
            break;

        case MidiCommand_TSE_Meta_KeySig:
            {
                int sf      = e.data.data2 >> 4;
                int mi      = e.data.data2 & 0xf;
                writeFixed(out, 0xff, 1); // meta event
                writeFixed(out, 0x59, 1); // set key signature
                writeVariable(out, 2);    // length
                writeFixed(out, sf, 1);   // sf
                writeFixed(out, mi, 1);   // mi
                if (verbose == 3)
                    diag_out << "  Writing keysig event time=" << e.time
                             << ", value=" << sf << "-" << mi << "\n";
            }
            break;
    }

    runningStatus = MidiCommand_Invalid;
}


void MidiFileExport::writeFixed(std::ostream &out, int value, int length)
{
    MTrkSize += length;
    _size    += length;
    while (length > 0)
    {
        out.put((value>>(8*(length-1))) & 0xff);
        --length;
    }
}


void MidiFileExport::writeVariable(std::ostream &out, int value)
{
    if (value < 0)
    {
        diag_out << "writeVariable < 0!";
        value = 0;
    }

     unsigned long buffer = value & 0x7f;
     while (value >>= 7)
     {
         buffer <<= 8;
         buffer |= ((value & 0x7f) | 0x80);
     }
     while (1)
     {
          out.put((char) (buffer & 0xff));
          ++MTrkSize;
          ++_size;
          if (buffer & 0x80)
          {
              buffer >>= 8;
          }
          else
          {
              break;
          }
     }
}


void MidiFileExport::writeString(std::ostream &out, const std::string &str,
                                 bool term)
{
    out << str;
    if (term)
    {
        out << (char) 0;
        ++MTrkSize;
        ++_size;
    }
    MTrkSize += str.size();
    _size    += str.size();
}

