/*
 * @(#)TSE2MDL.cpp 3.00 28 May 1999
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

/******************************************************************************
 * DISCLAIMER
 *
 * This is Old Code from the TSE2 library hacked into the TSE3 environment.
 * I /will/ rewrite it one day ;-)
 *****************************************************************************/

#include "tse3/TSE2MDL.h"

#include "tse3/Song.h"
#include "tse3/Track.h"
#include "tse3/Part.h"
#include "tse3/Phrase.h"
#include "tse3/PhraseList.h"
#include "tse3/MidiParams.h"
#include "tse3/MidiFilter.h"
#include "tse3/TimeSigTrack.h"
#include "tse3/TempoTrack.h"
#include "tse3/KeySigTrack.h"
#include "tse3/FlagTrack.h"
#include "tse3/PhraseEdit.h"
#include "tse3/Progress.h"

#include <fstream>
#include <cstring>

using namespace TSE3;

/**
 * Inline function to convert between different PPQN time scales
 */
inline int convertPPQN(int time, int oldPPQN, int newPPQN)
{
  return (time * newPPQN) / oldPPQN;
}


TSE2MDL::TSE2MDL(const std::string &a, bool v, std::ostream &o)
: appname(a), verbose(v), out(o), prevTrack(0), prevPart(0)
{
}


Song *TSE2MDL::load(const std::string &filename, Progress *progress)
{
    if (verbose) out << "Loading TSEMDL file: " << filename << "\n";

    song = new Song(0);

    std::ifstream in(filename.c_str(), std::ios::binary | std::ios::in);
    if (!in.good())
    {
        throw std::exception();
    }
    in.seekg(0, std::ios::end);
    file_size = in.tellg();
    in.seekg(0, std::ios::beg);
    if (progress)
    {
        progress->progressRange(0, file_size);
    }

    // Read the file header
    if (!load_header(in))
    {
        return song;
    }

    // Get the right number of Tracks in the Song
    // (Remove Tracks?)
    while (file_noTracks > song->size()) song->insert(0);
    while (file_noTracks < song->size())
    {
        Track *t = (*song)[0];
        song->remove(size_t(0));
        delete t;
    }

    size_t noTracksLoaded = 0;

    // Read each object in the file
    while (!in.eof())
    {
        if (progress)
        {
            progress->progress(in.tellg());
        }

        int type   = freadInt(in, 4);
        int length = freadInt(in, 4)-8;

        if (verbose)
            out << "Read TSEMDL object of type:" << type << " length: "
                << length << "\n";

        switch (type)
        {

            case -1: // EOF we're done XXX HACK
                break;

            case 0: // song title object
                load_songTitle(in);
                break;

            case 1: // song author object
                load_songAuthor(in);
                break;

            case 2: // song copyright object
                load_songCopyright(in);
                break;

            case 3: // song date object
                load_songDate(in);
                break;

            case 7: // tempo track object
                load_TempoTrack(in, length);
                break;

            case 8: // timesig track object
                load_TimeSigTrack(in, length);
                break;

            case 10: // flag track object
                load_FlagTrack(in, length);
                break;

            case 14: // audio track object
                //loadAudioTrackTSEMDL(song);
                skip(in, length);
                if (verbose)
                   out << "  TSE2 AudioTrack objects are not handled by TSE3\n";
                break;

            case 4: // track object
                if (noTracksLoaded != song->size())
                {
                  load_Track(in);
                  ++noTracksLoaded;
                }
                else
                {
                  skip(in, length);
                }
                break;

            case 11: // extended Track information object
                load_ExtendedTrack(in, length);
                break;

            case 5: // phrase object
                load_Phrase(in, length);
                break;

            case 15: // audio phrase object
                if (verbose)
                    out << "  TSE2 AudioPhrase objects are not handled by TSE3"
                        << "\n";
                skip(in, length);
                //if (loadAudioPhraseTSEMDL(song, filename))
                //  status = loadPhraseOverflow;
                break;

            case 6: // part object
                load_Part(in);
                break;

            case 16: // audio part object
                if (verbose)
                    out << "  TSE2 AudioPart objects are not handled by TSE3\n";
                skip(in, length);
                //loadAudioPartTSEMDL(song, length, filePPQN);
                //break;

            case 12: // extended Part information object
                load_ExtendedPart(in, length);
                break;

            case 9: // choices object
                load_Choices(in, length);
                break;

            default: // undefined object
                skip(in, length);
                break;
        }

    }

    return song;
}


bool TSE2MDL::load_header(std::istream &in)
{
    char buffer[21];

    if (verbose) out << "Loading TSEMDL header\n";

    in.read(buffer, 8);
    if (strncmp(buffer, "TSEMDL  ", 8) != 0) throw std::exception();
    int majorVersion = freadInt(in, 4);
    int minorVersion = freadInt(in, 4);
    in.read(buffer, 20); // creator credit
    file_noTracks = freadInt(in, 2);
    file_PPQN     = freadInt(in, 2);

    if (verbose)
        out << "  Major:     " << majorVersion << "\n"
            << "  Minor:     " << minorVersion << "\n"
            << "  No Tracks: " << file_noTracks << "\n"
            << "  PPQN:      " << file_PPQN << "\n";


    return true;
}


bool TSE2MDL::load_songTitle(std::istream &in)
{
    char charBuffer[100]; // was SONG_STRLEN+1

    freadPString(in, charBuffer);
    song->setTitle(charBuffer);

    if (verbose) out << "  -- Song title: " << charBuffer << "\n";

    return true;
}


bool TSE2MDL::load_songAuthor(std::istream &in)
{
    char charBuffer[100]; // was SONG_STRLEN+1

    freadPString(in, charBuffer);
    song->setAuthor(charBuffer);

    if (verbose) out << "  -- Song author: " << charBuffer << "\n";

    return true;
}


bool TSE2MDL::load_songCopyright(std::istream &in)
{
    char charBuffer[100]; // was SONG_STRLEN+1

    freadPString(in, charBuffer);
    song->setCopyright(charBuffer);

    if (verbose) out << "  -- Song copyright: " << charBuffer << "\n";

    return true;
}


bool TSE2MDL::load_songDate(std::istream &in)
{
    char charBuffer[100]; // was SONG_STRLEN+1

    freadPString(in, charBuffer);
    song->setDate(charBuffer);

    if (verbose) out << "  -- Song date: " << charBuffer << "\n";

    return true;
}


bool TSE2MDL::load_TempoTrack(std::istream &in, int length)
{
    // Get the status
    int s = freadInt(in, 4);
    song->tempoTrack()->setStatus(s);

    // Get the events
    for (length -= 4; length > 0; length -= 8)
    {
        int time  = freadInt(in, 4);
        int tempo = freadInt(in, 4);
        time = convertPPQN(time, file_PPQN, Clock::PPQN);
        song->tempoTrack()->insert(Event<Tempo>(Tempo(tempo), time));
    }

    if (verbose) out << "  -- TempoTrack object\n";

    return true;
}


bool TSE2MDL::load_TimeSigTrack(std::istream &in, int length)
{
    // Get the status
    int s = freadInt(in, 4);
    song->timeSigTrack()->setStatus(s);

    // Get the events
    for (length -= 4; length > 0; length -= 8)
    {
        int time = freadInt(in, 4);
        int t    = freadInt(in, 1);
        int b    = freadInt(in, 1);
        freadInt(in, 2);
        time = convertPPQN(time, file_PPQN, Clock::PPQN);
        song->timeSigTrack()->insert(Event<TimeSig>(TimeSig(t,b), time));
    }

    if (verbose) out << "  -- TimeSigTrack object\n";

    return true;
}


bool TSE2MDL::load_FlagTrack(std::istream &in, int length)
{
    // Get the events
    for (; length > 0; length -= 4)
    {
        int time = freadInt(in, 4);
        time = convertPPQN(time, file_PPQN, Clock::PPQN);
        song->flagTrack()->insert(Event<Flag>(Flag(), time));
    }

    if (verbose) out << "  -- FlagTrack object\n";

    return true;
}


bool TSE2MDL::load_Track(std::istream &in)
{
    char charBuffer[100]; // was TRACK_STRLEN+1

    int trackNo = freadInt(in, 4);
    Track *t = (*song)[trackNo];
    freadPString(in, charBuffer);
    t->setTitle(charBuffer);
    t->filter()->setChannel(freadInt(in, 1));
    t->filter()->setPort(freadInt(in, 1));
    t->params()->setProgram(freadInt(in, 1));
    freadInt(in, 1);
    int bank = freadInt(in, 4);
    t->params()->setBankLSB(bank&0x7f);
    t->params()->setBankMSB(bank>>7);
    t->filter()->setStatus(freadInt(in, 4));
    prevTrack = t;

    if (verbose) out << "  -- Track object " << trackNo << "\n";

    return true;
}

bool TSE2MDL::load_ExtendedTrack(std::istream &in, int length)
{
    for (; length > 0; length -=8)
    {
        int param = freadInt(in, 4);
        int value = freadInt(in, 4);
        if (prevTrack)
            switch (param)
            {
                case 0x000000: prevTrack->params()->setPan(value);    break;
                case 0x000001: prevTrack->params()->setReverb(value); break;
                case 0x000002: prevTrack->params()->setChorus(value); break;
                case 0x000003: prevTrack->params()->setVolume(value); break;
            }
    }

    if (verbose) out << "  -- ExtendedTrack object\n";

    return true;
}


bool TSE2MDL::load_Phrase(std::istream &in, int length)
{
    PhraseEdit pe;
    char       buffer[100]; // was PHRASE_STRLEN+1
    int        titleLen = freadPString(in, buffer);

    size_t noEvents = (length-titleLen)/8;

    for (size_t n = 0; n < noEvents; ++n)
    {
        Clock       time    = freadInt(in, 4);
        int         c       = freadInt(in, 4);
        MidiCommand command((c&0xf0)>>4, c & 0xf, (c&0xf0000000)>>28,
                            (c&0xff00)>>8, (c&0xff0000)>>16);
        time = convertPPQN(time, file_PPQN, Clock::PPQN);
        if (command.status != MidiCommand_NoteOn)
        {
            pe.insert(MidiEvent(command, time));
            //if (verbose)
            //    out << "    -- Event at " << time << ": "
            //        << command.status << "\n";
        }
        else
        {
            Clock       offTime = freadInt(in, 4);
            int         oc      = freadInt(in, 4);
            MidiCommand offCommand((oc&0xf0)>>4, oc & 0xf, (oc&0xf0000000)>>28,
                            (oc&0xff00)>>8, (oc&0xff0000)>>16);
            offTime = convertPPQN(offTime, file_PPQN, Clock::PPQN);
            pe.insert(MidiEvent(command, time, offCommand, offTime));
            --noEvents;
            //if (verbose)
            //    out << "    -- Note event at " << time << ": "
            //        << command.status << "\n";
        }
    }
    pe.createPhrase(song->phraseList(), buffer);

    if (verbose)
        out << "  -- Phrase " << buffer << " with " << noEvents
            << " events\n";

    return true;
}


bool TSE2MDL::load_Part(std::istream &in)
{
    int       trackNo = freadInt(in, 4);
    int       s_i     = freadInt(in, 4);
    int       e_i     = freadInt(in, 4);
    Clock     start   = convertPPQN(s_i, file_PPQN, Clock::PPQN);
    Clock     end     = convertPPQN(e_i, file_PPQN, Clock::PPQN);

    char buffer[100]; // was PHRASE_STRLEN+1
    int phrLen = freadPString(in, buffer);
    phrLen = phrLen; // to shut up the complier

    int tmp;

    Part *part = (*song)[trackNo]->insert(start, end);
    part->setPhrase(song->phraseList()->phrase(buffer));
    tmp = freadInt(in, 4);
    part->setRepeat(convertPPQN(tmp, file_PPQN, Clock::PPQN));
    tmp = freadInt(in, 4);
    part->filter()->setOffset(convertPPQN(tmp, file_PPQN, Clock::PPQN));
    part->filter()->setStatus(freadInt(in, 1));
    part->filter()->setChannel(freadInt(in, 1));
    part->filter()->setPort(freadInt(in, 1));
    part->params()->setProgram(freadInt(in, 1));
    part->filter()->setTranspose(freadInt(in, 1)-127); // difference in 3 vs 2
    part->filter()->setMinVelocity(freadInt(in, 1));
    part->filter()->setMaxVelocity(freadInt(in, 1));
    freadInt(in, 1); // read padding byte
    tmp = freadInt(in, 4);
    part->filter()->setQuantise(convertPPQN(tmp, file_PPQN, Clock::PPQN));
    int bank = freadInt(in, 4);
    part->params()->setBankLSB(bank & 0x7f);
    part->params()->setBankMSB((bank>>7) & 0x7f);

    if (verbose)
        out << "  -- Part using Phrase " << buffer
            << " in track " << trackNo << "\n";

    return true;
}


bool TSE2MDL::load_ExtendedPart(std::istream &in, int length)
{
    for (; length > 0; length -=8)
    {
        int param = freadInt(in, 4);
        int value = freadInt(in, 4);
        if (prevPart)
            switch (param)
            {
                case 0x000000: prevPart->params()->setPan(value);    break;
                case 0x000001: prevPart->params()->setReverb(value); break;
                case 0x000002: prevPart->params()->setChorus(value); break;
                case 0x000003: prevPart->params()->setVolume(value); break;
            }
    }

    if (verbose) out << "  -- ExtendedPart object\n";

    return true;
}


bool TSE2MDL::load_Choices(std::istream &in, int length)
{
    for (; length > 0; length -= 8)
    {
        int id    = freadInt(in, 4);
        int value = freadInt(in, 4);
        switch (id)
        {
            case 0x00000005: // repeat status
                song->setRepeat(value);
                break;
            case 0x00000008: // left marker
                song->setFrom(value);
                break;
            case 0x00000009: // right marker
                song->setTo(value);
                break;
        }
    }

    if (verbose) out << "  -- Choices object\n";

    return true;
}


int TSE2MDL::freadInt(std::istream &in, int size)
{
  int value   = 0;
  int counter = 0;

  while (counter < size && in)
  {
    value += in.get() << (8*counter);
    ++counter;
  }

  return value;
}


int TSE2MDL::freadPString(std::istream &in, char *string)
{
  int length = -1;
  do
  {
    ++length;
    string[length] = in.get();
  }
  while (string[length] != 0);
  ++length;

  // read the word boundary padding
  int padLen = (4-(length%4))%4;
  for (int n = 0; n < padLen; ++n)
  {
      in.get();
  }
  length += padLen;

  return length;
}


bool TSE2MDL::skip(std::istream &in, int length)
{
    if (length < 0) length = 0;
    in.ignore(length);
    return true;
}
