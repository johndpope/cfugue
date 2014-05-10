/*
 * @(#)Demidify.cpp 3.00 3 April 2000
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

#include "tse3/util/Demidify.h"

#include "tse3/Song.h"
#include "tse3/Track.h"
#include "tse3/Part.h"
#include "tse3/Phrase.h"
#include "tse3/PhraseEdit.h"
#include "tse3/PhraseList.h"
#include "tse3/MidiParams.h"
#include "tse3/MidiFilter.h"
#include "tse3/Progress.h"

using namespace TSE3;
using namespace TSE3::Util;

#include <memory>

bool identical(Playable *first, Playable *second)
{
    std::auto_ptr<PlayableIterator> pi1(first->iterator(0));
    std::auto_ptr<PlayableIterator> pi2(second->iterator(0));

    while (pi1->more() && pi2->more())
    {
        if (**pi1 != **pi2) return false;
        ++(*pi1);
        ++(*pi2);
    }
    return (pi1->more() || pi2->more()) ? false : true;
}


void replacePhraseInParts(Song *song,
                          Phrase *phrase1, Phrase *phrase2)
{
    for (size_t track = 0; track < song->size(); ++track)
    {
        for (size_t part = 0; part < (*song)[track]->size(); ++part)
        {
            if ((*(*song)[track])[part]->phrase() == phrase1)
            {
                (*(*song)[track])[part]->setPhrase(phrase2);
            }
        }
    }
}


void replacePhrase(Song *song, Phrase *oldPhrase, Phrase *newPhrase)
{
    newPhrase->setTitle(oldPhrase->title());
    replacePhraseInParts(song, oldPhrase, newPhrase);
    song->phraseList()->erase(oldPhrase);
}


Demidify::Demidify(bool compactParts, bool pullTrackParameters,
                   Clock partSize, bool aggressive,
                   TSE3::Progress *progress,
                   int verbose, std::ostream &out)
: compactParts(compactParts), pullTrackParameters(pullTrackParameters),
  partSize(partSize), aggressive(aggressive), progress(progress),
  verbose(verbose), out(out)
{
    if (partSize < Clock::PPQN) partSize = Clock::PPQN;
    if (progress) progress->progressRange(0, 100);
}


void Demidify::go(Song *song)
{
    if (verbose)
        out << "Demidify\n"
            << "    |\n";
    if (verbose >= 2)
        out << "    +- Parameters:\n"
            << "    |     +- compactParts:    " << compactParts        << "\n"
            << "    |     +- pullTrackParams: " << pullTrackParameters << "\n"
            << "    |     +- partSize:        " << partSize            << "\n"
            << "    |     +- aggressive:      " << aggressive          << "\n"
            << "    |\n";

    int prog_step = 80/song->size();
    int prog      = 10 - prog_step;
    if (progress) progress->progress(0);

    // Disect Phrases

    for (size_t t = 0; t < song->size(); ++t)
    {
        prog += prog_step;
        if (progress) progress->progress(prog);
        if ((*song)[t]->size())
        {
            if (verbose)
                out << "    +- Disecting track " << t << "\n"
                    << "    |    |\n";
            disectPhrase(song, t, prog, prog_step);
            if (verbose) out << "    |\n";
        }
    }

    if (progress) progress->progress(90);

    // Look for identical Phrases

    if (verbose) out << "    +- Looking for identical Phrases\n";
    int noRemovedMatches = 0;
    for (size_t pos = 0; pos < song->phraseList()->size()-1; ++pos)
    {
        for (size_t subpos = pos+1;
             subpos < song->phraseList()->size();
             ++subpos)
        {
            Phrase *phrase1 = (*song->phraseList())[pos];
            Phrase *phrase2 = (*song->phraseList())[subpos];
            if (identical(phrase1, phrase2))
            {
                ++noRemovedMatches;
                replacePhraseInParts(song, phrase1, phrase2);
            }
        }
    }
    if (verbose >= 2)
        out << "    |    +- Removed " << noRemovedMatches << " Phrases\n";

    if (verbose)
        out << "    |\n"
            << "    +- Demidify complete\n\n";
}


void Demidify::disectPhrase(Song *song, size_t trackNo,
                            int prog_base, int max_prog_delta)
{
    if ((*song)[trackNo]->size() == 0) return;

    Phrase *phrase = (*(*song)[trackNo])[0]->phrase();
    if (!phrase) return;

    if (verbose >= 2)
    {
        out << "    |    +- Disecting Phrase '" << phrase->title()
            << "' which has "
            << phrase->size() << " events and is in Track " << trackNo
            << ".\n";
    }

    // STEP ZERO
    // Remove program changes and other fun from the Phrase.

    if (pullTrackParameters && phrase->size())
    {
        if (verbose >= 2) out << "    |    +- Pulling Track parameters.\n";
        PhraseEdit pe;
        pe.reset(phrase);
        size_t    pos = 0;
        bool      culled = false;
        MidiEvent e;
        do
        {
            if (pos < pe.size())
            {
                e = pe[pos];
                switch (e.data.status)
                {
                    case MidiCommand_ProgramChange:
                        (*song)[trackNo]->params()->setProgram(e.data.data1);
                        (*song)[trackNo]->filter()->setChannel(e.data.channel);
                        (*song)[trackNo]->filter()->setPort(e.data.port);
                        pe.erase(pos);
                        culled = true;
                        break;
                    case MidiCommand_ControlChange:
                        switch (e.data.data1)
                        {
                            case MidiControl_BankSelectMSB:
                                (*song)[trackNo]->params()->setBankMSB
                                    (e.data.data2);
                                pe.erase(pos);
                                culled = true;
                                break;
                            case MidiControl_BankSelectLSB:
                                (*song)[trackNo]->params()->setBankLSB
                                    (e.data.data2);
                                pe.erase(pos);
                                culled = true;
                                break;
                            case MidiControl_PanMSB:
                                (*song)[trackNo]->params()->setPan
                                    (e.data.data2);
                                pe.erase(pos);
                                culled = true;
                                break;
                            case MidiControl_ReverbDepth:
                                (*song)[trackNo]->params()->setReverb
                                    (e.data.data2);
                                pe.erase(pos);
                                culled = true;
                                break;
                            case MidiControl_ChorusDepth:
                                (*song)[trackNo]->params()->setChorus
                                    (e.data.data2);
                                pe.erase(pos);
                                culled = true;
                                break;
                            default:
                                ++pos;
                        }
                        break;
                    default:
                        ++pos;
                        break;
                }
            }
        }
        while (pos < pe.size() && e.data.status != MidiCommand_NoteOn);
        if (culled)
        {
            if (verbose >= 2)
                out << "    |    |    |\n"
                    << "    |    |    +- Culled some Track information\n";
            Phrase *newPhrase = pe.createPhrase(song->phraseList());
            replacePhrase(song, phrase, newPhrase);
            phrase = newPhrase;
        }
    }

    // STEP ONE:
    // Remove the original MidiImport Part from the Track.

    if (verbose >= 2) out << "    |    +- Removing original Part\n";
    Part *part = (*((*song)[trackNo]))[0];
    (*song)[trackNo]->remove(part);
    delete part;

    if (phrase->size() == 0) return;

    // STEP TWO:
    // Break the Phrase up into smaller Phrase chunks.

    if (verbose >= 2) out << "    |    +- Breaking up Phrase\n";

    Clock startTime = (*phrase)[0].time; // start time of Phrase
    Clock endTime;                       // end time of Phrase
    for (size_t pos = 0; pos < phrase->size(); ++pos)
    {
        if ((*phrase)[pos].time > endTime)
            endTime = (*phrase)[pos].time;
        if ((*phrase)[pos].data.status == MidiCommand_NoteOn
            && (*phrase)[pos].offTime > endTime)
            endTime = (*phrase)[pos].offTime;
    }

    size_t pos = 0;
    size_t noParts = 0;
    for (Clock partStart = (startTime / partSize) * partSize;
         partStart < endTime && pos < phrase->size();
         partStart += partSize)
    {
        if (progress)
            progress->progress(prog_base + pos*max_prog_delta/phrase->size());

        PhraseEdit pe;
        MidiEvent  e, lastE;
        do
        {
            e = (*phrase)[pos];
            if (e.time < partStart + partSize)
            {
                MidiEvent e2 = e;
                e2.time    -= partStart;
                if (e2.data.status == MidiCommand_NoteOn)
                    e2.offTime -= partStart;
                pe.insert(e2);
                ++pos;
                lastE = e;
            }
        }
        while (pos < phrase->size() && e.time < partStart + partSize);

        if (pe.size())
        {
            if (verbose >= 3)
                out << "    |    |    +- Created Phrase between "
                    << partStart << "-" << partStart+lastE.time << " with "
                    << pe.size() << " events\n";

            // If there is an identical Phrase already in this Track,
            // use that, otherwise create a new Phrase.

            Phrase *newPhrase = 0;

            for (size_t plpos = 0; plpos < song->phraseList()->size(); ++plpos)
            {
                if (identical(&pe, (*song->phraseList())[plpos]))
                {
                    newPhrase = (*song->phraseList())[plpos];
                    break;
                }
            }

            if (newPhrase == 0)
            {
                newPhrase = pe.createPhrase(song->phraseList());
            }

            Part *newPart = new Part;
            newPart->setStart(partStart);
            newPart->setEnd(partStart+partSize);
            newPart->setPhrase(newPhrase);

            (*song)[trackNo]->insert(newPart);

            ++noParts;
        }
        else
        {
            if (verbose >= 3)
                out << "    |    |    +- No Phrase at "
                    << partStart << "\n";
        }
    }
    if (verbose >= 2)
        out << "    |    |    |    +- split into " << noParts
            << " Parts\n";

    // STEP THREE:
    // Look at the Part we've instered. Can any be reduced to repeats in Parts?

    if (compactParts)
    {
        if (verbose >= 2) out << "    |    +- Reducing repeated Parts\n";
        reduceParts(song, trackNo);
    }

    // STEP FOUR:
    // Look at the pattern of Phrases (ignoring the repeated ones).
    // Can we make any bigger Phrases?

    if (aggressive)
    {
        if (verbose >= 2)
            out << "    |    +- Extending Parts/Phrases (there are "
                << (*song)[trackNo]->size() << " Parts)\n";
        size_t pos = 0;
        while (pos < (*song)[trackNo]->size() - 4)
        {
            if (!matchParts(song, trackNo, pos)) ++pos;
        }
        // Try the repeat thing again?
        if (compactParts) reduceParts(song, trackNo);
    }


    // STEP FIVE:
    // Now remove the original Phrase.

    if (verbose >= 2) out << "    |    +- Removing original Phrase\n";
    song->phraseList()->erase(phrase);


    if (verbose >= 2) out << "    |    +- Phrase disection done\n";
}


void Demidify::reduceParts(Song *song, size_t trackNo)
{
    if (verbose >= 2)
        out << "    |    |    +- Trying to compact Parts (there are "
            << (*song)[trackNo]->size() << ")...\n";

    size_t pos = 0;
    size_t partsCompacted = 0;
    while (pos < (*song)[trackNo]->size()-1)
    {
        Part *first  = (*(*song)[trackNo])[pos];
        Part *second = (*(*song)[trackNo])[pos+1];
        if (first->phrase()->title() == second->phrase()->title())
        {
            if (!first->repeat())
            {
                first->setRepeat(second->start() - first->start());
                (*song)[trackNo]->remove(second);
                first->setEnd(second->end());
                delete second;
                ++partsCompacted;
                continue;
            }
            else
            {
                Clock startTime = first->start();
                Clock repeat    = first->repeat();
                while(startTime+repeat <= second->start()) startTime += repeat;
                if (startTime != second->start())
                {
                    ++pos;
                    continue;
                }
                if (second->end() - second->start() > repeat)
                {
                    ++pos;
                    continue;
                }
                (*song)[trackNo]->remove(second);
                first->setEnd(second->end());
                delete second;
                ++partsCompacted;
            }
        }
        else
        {
            ++pos;
        }
    }

    if (verbose >= 2)
        out << "    |    |    |    +- compacted " << partsCompacted
            << " Parts\n";
}


int Demidify::matchParts(Song *song, size_t trackNo, size_t startPos)
{
    if ((*(*song)[trackNo])[startPos]->repeat()
        || (*(*song)[trackNo])[startPos+1]->repeat())
    {
        return 0;
    }

    Clock startTime1 = (*(*song)[trackNo])[startPos]->start();
    Clock startTime2 = (*(*song)[trackNo])[startPos+1]->start();

    // If not together, skip[ this match?

    bool matchFound = false;
    for (size_t n = startPos+2; n < (*song)[trackNo]->size()-1; ++n)
    {
        const bool firstPartsMatch
            = (*(*song)[trackNo])[startPos]->phrase()
            == (*(*song)[trackNo])[n]->phrase();
        const bool secondPartsMatch
            = (*(*song)[trackNo])[startPos+1]->phrase()
            == (*(*song)[trackNo])[n+1]->phrase();
        const Clock nStartTime1 = (*(*song)[trackNo])[n]->start();
        const Clock nStartTime2 = (*(*song)[trackNo])[n+1]->start();
        if (firstPartsMatch && secondPartsMatch
            && (*(*song)[trackNo])[n]->repeat() == 0
            && (*(*song)[trackNo])[n+1]->repeat() == 0
            && nStartTime2-nStartTime1 == startTime2-startTime1)
        {
            matchFound = true;
        }
    }
    if (!matchFound) return 0;

    // We've found some matching Parts, so let's reduce the Parts
    int noPartsRemoved = 0;
    Phrase *phrase1 = (*(*song)[trackNo])[startPos]->phrase();
    Phrase *phrase2 = (*(*song)[trackNo])[startPos+1]->phrase();

    // Create the composite Phrase
    PhraseEdit pe;
    pe.reset(phrase1);
    for (size_t pos = 0; pos < phrase2->size(); ++pos)
    {
        MidiEvent e = (*phrase2)[pos];
        e.time += startTime2-startTime1;
        if (e.data.status == MidiCommand_NoteOn)
            e.offTime += startTime2-startTime1;
        pe.insert(e);
    }
    Phrase *newPhrase = pe.createPhrase(song->phraseList());

    // Now scan the Track, looking for instances of the old Phrases, removing
    // them and placing Parts using the new Phrase

    bool removeOldPhr1 = true;
    bool removeOldPhr2 = true;
    for (size_t n = 0; n < (*song)[trackNo]->size()-1; ++n)
    {
        if ((*(*song)[trackNo])[n]->phrase() == phrase2) removeOldPhr2 = false;
        if ((*(*song)[trackNo])[n]->phrase() == phrase1)
        {
            if ((*(*song)[trackNo])[n+1]->phrase() == phrase2)
            {
                // not a matchm phr1 will be still used
                removeOldPhr1 = false;
            }
            else
            {
                if ((*(*song)[trackNo])[n]->repeat()
                    || (*(*song)[trackNo])[n+1]->repeat())
                {
                    removeOldPhr1 = false;
                    removeOldPhr2 = false;
                }
                else
                {
                    // it's a match! let's remove and replace
                    Part *part2 = (*(*song)[trackNo])[n+1];
                    (*song)[trackNo]->remove(part2);
                    (*(*song)[trackNo])[n]->setEnd(part2->end());
                    (*(*song)[trackNo])[n]->setPhrase(newPhrase);
                    ++noPartsRemoved;
                }
            }
        }
    }

    if (removeOldPhr1)
    {
        //song->phraseList()->remove(phrase1);
    }

    return noPartsRemoved;
}

