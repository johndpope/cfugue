/*
 * @(#)Part.cpp 3.00 9 July 2000
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

#include "tse3/cmd/Part.h"

#include "tse3/Track.h"
#include "tse3/Part.h"
#include "tse3/Error.h"

#include <algorithm>

using namespace TSE3;
using namespace TSE3::Cmd;

/******************************************************************************
 * Part_SetInfo class
 *****************************************************************************/

Part_SetInfo::Part_SetInfo(TSE3::Part                *part,
                           TSE3::Phrase              *phrase,
                           TSE3::Clock                repeat,
                           const TSE3::MidiFilter    &smef,
                           const TSE3::MidiParams    &mp,
                           const TSE3::DisplayParams &dp)
: Command("set part info"), part(part),
  newPhrase(phrase), newRepeat(repeat), smef(smef), mp(mp), dp(dp)
{
}


void Part_SetInfo::executeImpl()
{
    oldPhrase = part->phrase();
    oldRepeat = part->repeat();

    part->setPhrase(newPhrase);
    part->setRepeat(newRepeat);
    std::swap(smef, *(part->filter()));
    std::swap(mp,   *(part->params()));
    std::swap(dp,   *(part->displayParams()));
}


void Part_SetInfo::undoImpl()
{
    part->setPhrase(oldPhrase);
    part->setRepeat(oldRepeat);
    std::swap(smef, *(part->filter()));
    std::swap(mp,   *(part->params()));
    std::swap(dp,   *(part->displayParams()));
}


/******************************************************************************
 * Part_SetPhrase class
 *****************************************************************************/

Part_SetPhrase::Part_SetPhrase(TSE3::Part *part, TSE3::Phrase *phrase)
: Command("set phrase"), part(part), newPhrase(phrase)
{
}


void Part_SetPhrase::executeImpl()
{
    oldPhrase = part->phrase();
    part->setPhrase(newPhrase);
}


void Part_SetPhrase::undoImpl()
{
    part->setPhrase(oldPhrase);
}


/******************************************************************************
 * Part_Move class
 *****************************************************************************/

Part_Move::Part_Move(int          action,
                     TSE3::Part  *part,
                     TSE3::Track *track,
                     TSE3::Clock  nStart,
                     TSE3::Clock  nEnd)
: Command(prvTitle(part->parent() != 0, newEnd != -1, track == part->parent())),
  part(part), newTrack(track), newStart(nStart), newEnd(nEnd), action(action),
  clippedStart(-1), clippedEnd(-1), newPart(0),
  valid(true)
{
    oldTrack = part->parent();
    oldStart = part->start();
    oldEnd   = part->end();

    if (newStart == -1) newStart = oldStart;
    if (newEnd   == -1) newEnd   = newStart + oldEnd - oldStart;

    if (!newTrack || newStart < 0) valid = false;
}


const char *Part_Move::prvTitle(bool hasParent, bool relativeEnd,
                                bool sameTrack)
{
    if (!hasParent)
    {
        return "insert part";
    }
    else
    {
        if (!relativeEnd || !sameTrack)
        {
            return "move part";
        }
        else
        {
            return "resize part";
        }
    }
}


Part_Move::~Part_Move()
{
    if (done())
    {
        while (removed.size())
        {
            delete removed.back();
            removed.pop_back();
        }
    }
    else
    {
        delete newPart;
    }
}


void Part_Move::removeAndSetPart()
{
    if (oldTrack) oldTrack->remove(part);
    part->setStartEnd(newStart, newEnd);
}


void Part_Move::unsetAndReinsertPart()
{
    part->setStartEnd(oldStart, oldEnd);
    if (oldTrack) oldTrack->insert(part);
}


void Part_Move::executeImpl()
{
    if (valid)
    {
        // Remove the Part from it's old position
        removeAndSetPart();

        switch (action)
        {
            case NoOverlap:
            {
                try
                {
                    newTrack->insert(part);
                }
                catch (const TSE3::TrackError &e)
                {
                    unsetAndReinsertPart();
                    valid = false;
                    throw;
                }
                break;
            }
            case Replace:
            {
                Util::Track_RemoveParts(newTrack, part->start(), part->end(),
                                        removed, clippedStart, clippedEnd,
                                        newPart);
                newTrack->insert(part);
                break;
            }
            case Under:
            {
                break;
            }
        }
    }
}


void Part_Move::undoImpl()
{
    if (valid)
    {
        switch (action)
        {
            case NoOverlap:
            {
                newTrack->remove(part);
                break;
            }
            case Replace:
            {
                newTrack->remove(part);
                Util::Track_UnremoveParts(newTrack, part->start(), part->end(),
                                          removed, clippedStart, clippedEnd);
                break;
            }
            case Under:
            {
                break;
            }
        }

        // The Part will just slot back into the old gap
        unsetAndReinsertPart();
    }
}
