/*
 * @(#)Track.cpp 3.00 26 July 2000
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

#include "tse3/util/Track.h"

#include "tse3/Track.h"
#include "tse3/Part.h"
#include "tse3/MidiFilter.h"
#include "tse3/FileBlockParser.h"
#include "tse3/Error.h"

using namespace TSE3;
using namespace TSE3::Util;

/* ******** OLD STUFF TO BE REMOVED WHEN I'M SURE I DON'T WANT IT ********
         **
         * Inserts a @ref Part between the given times. The Track 'owns'
         * this @ref Part and will delete it when it is deleted.
         *
         * If the @ref Part cannot be inserted (due to @ref Part overlap)
         * a @ref TrackError will be thrown.
         *
         * @param  start  @ref Part's start time
         * @param  end    @ref Part's end time
         * @throws TrackError
         * @return New @ref Part. If several @ref Part objects are inserted
         *         (because @p action is @ref Part::Under) the pointer
         *         to the first @ref Part will be returned.
         *
        void Track_insert(TSE3::Track *track, TSE3::Part *part,
                          PartInsertAction action = NoOverlap);

Part *Track_Insert(Clock start, Clock end, Part::InsertAction action)
{
    // perhaps this should be more simply implemented:
    // create a Part, call the other insert - be done
    // (after dealing with exceptions cleanign up the Part)

    Part *part = 0;

    if (action == Part::Replace)
    {
        remove(start, end);
    }
    else if (action == Part::NoOverlap && partsBetween(start, end))
    {
        throw TrackError(PartOverlapErr);
    }

    if (action == Part::Under)
    {
        while (start < end)
        {
            // Work out times for the next Part
            Clock partStart = start;
            Clock partEnd   = end;
            size_t pos = indexAtTime(partStart);
            while (pos != _parts.size()
                   && _parts[pos]->start() <= partStart)
            {
                partStart = _parts[pos]->end();
                ++pos;
            }
            if (pos != _parts.size()
                && _parts[pos]->start() < partEnd)
            {
                partEnd = _parts[pos]->start();
            }

            // Insert the Part
            if (partStart < partEnd)
            {
                Part *newPart = new Part(partStart, partEnd);
                prvInsertPart(newPart);
                if (!part) part = newPart;
            }
            start = partEnd;
        }
        if (part)
        {
            notify(TrackListener::PartInserted);
        }
        else
        {
            throw TrackError(NoPartInsertedErr);
        }
    }
    else
    {
        // This is either a safe Part::NoOverlap mode or a pre-cleared out
        // Part::Replace. We can insert the one Part.

        part = new Part(start, end);
        prvInsertPart(part);
        notify(TrackListener::PartInserted);
    }

    return part;
}


void Track::insert(Part *part, Part::InsertAction action)
{
    if (part->parent())
    {
        throw TrackError(PartAlreadyInsertedErr);
    }
    if (part->start() > part->end())
    {
        throw TrackError(PartTimeErr);
    }

    if (action == Part::Replace)
    {
        remove(part->start(), part->end());
    }
    else if (action == Part::NoOverlap
             && partsBetween(part->start(), part->end()))
    {
        throw TrackError(PartOverlapErr);
    }

    if (action == Part::Under)
    {
        bool inserted = false;
        Clock originalStart = part->start();
        Clock start         = part->start();
        Clock end           = part->end();

        while (start < end)
        {
            // Work out times for the next Part
            Clock partStart = start;
            Clock partEnd   = end;
            size_t pos = indexAtTime(partStart);
            while (pos != _parts.size()
                   && _parts[pos]->start() <= partStart)
            {
                partStart = _parts[pos]->end();
                ++pos;
            }
            if (pos != _parts.size()
                && _parts[pos]->start() < partEnd)
            {
                partEnd = _parts[pos]->start();
            }

            // Insert the Part
            if (partStart < partEnd)
            {
                Part *newPart;
                if (!inserted)
                {
                    newPart = part;
                    part->setStartEnd(partStart, partEnd);
                }
                else
                {
                    newPart = new Part(*part);
                    newPart->setStartEnd(partStart, partEnd);
                    Clock offset = partStart = originalStart;
                    while (part->repeat() && offset > part->repeat())
                    {
                        offset -= part->repeat();
                    }
                    newPart->setRepeat(offset);
                }
                prvInsertPart(newPart);
                inserted = true;
            }
            start = partEnd;
        }
        if (inserted)
        {
            notify(TrackListener::PartInserted);
        }
        else
        {
            throw TrackError(NoPartInsertedErr);
        }
    }
    else
    {
        // This is either a safe Part::NoOverlap mode or a pre-cleared out
        // Part::Replace. We can insert the one Part.

        prvInsertPart(part);
        notify(TrackListener::PartInserted);
    }
}

******************* END OF OLD STUFF TO BE REMOVED **************************/


namespace
{
    /**
     * Internal utility function.
     *
     * Adjusts the start time of a Part, whilst also adjusting it's
     * offset value (taking into account the repeat setting) so that
     * the Part will play as if a chunk has been cut out of it, rather
     * than start from the beginning.
     *
     * @param part     @ref TSE3::Part to operate on
     * @param newStart New start time for the @ref TSE3::Part
     */
    void movePartStart(Part *part, Clock newStart)
    {
        Clock difference = newStart - part->start();
        while (part->repeat() && difference > part->repeat())
        {
            difference -= part->repeat();
        }
        part->filter()->setOffset(difference + part->filter()->offset());
        part->setStart(newStart);
    }


    /**
     * This internal function provides the implementation for both of the
     * public Track_RemoveParts methods.
     *
     * @param track        As public API
     * @param start        As public ARI
     * @param removed      Pointer to vector decsribed in public API, or zero
     *                     for no storage
     * @param clippedStart As publiC API
     * @param clippedEnd   As publiC API
     */
    void Track_RemovePartsPrv(Track               *track,
                              Clock                start,
                              Clock                end,
                              std::vector<Part*>  *removed,
                              Clock               &clippedStart,
                              Clock               &clippedEnd,
                              Part               *&newPart)
    {
        clippedStart = clippedEnd = -1;

        size_t pos = track->index(start);
        if (pos == track->size()) return;

        // Is there a Part that starts before 'start' and ends after 'end'?
        if ((*track)[pos]->start() < start && (*track)[pos]->end() > end)
        {
            clippedStart = (*track)[pos]->end();
            clippedEnd   = -2;
            if (!newPart)
            {
                newPart = new Part(*(*track)[pos]);
                movePartStart(newPart, end);
            }
            (*track)[pos]->setEnd(start);
            track->insert(newPart);
            return;
        }

        // Any Part that overlaps the start time
        if ((*track)[pos]->start() < start && (*track)[pos]->end() > start)
        {
            clippedStart = (*track)[pos]->end();
            (*track)[pos]->setEnd(start);
            ++pos;
        }

        // Any whole Parts between start and end
        while (pos < (*track).size() && (*track)[pos]->end() < end)
        {
            Part *part = (*track)[pos];
            track->remove(pos);
            if (removed)
            {
                removed->push_back(part);
            }
            else
            {
                delete part;
            }
        }

        // Any Part that overlaps the end time
        if (pos != track->size() && (*track)[pos]->start() < end)
        {
            clippedEnd = (*track)[pos]->start();
            movePartStart((*track)[pos], end);
        }
    }
}


void Util::Track_RemoveParts(Track *track, Clock start, Clock end)
{
    Clock temp1 = 0, temp2 = 0;
    Part *newPart;
    Track_RemovePartsPrv(track, start, end, 0, temp1, temp2, newPart);
}


void Util::Track_RemoveParts(Track              *track,
                             Clock               start,
                             Clock               end,
                             std::vector<Part*> &removed,
                             Clock              &clippedStart,
                             Clock              &clippedEnd,
                             Part               *&newPart)
{
    Track_RemovePartsPrv(track, start, end,
                         &removed, clippedStart, clippedEnd, newPart);
}


void Util::Track_UnremoveParts(Track              *track,
                               Clock               start,
                               Clock               end,
                               std::vector<Part*> &removed,
                               Clock               clippedStart,
                               Clock               clippedEnd)
{
    if (clippedEnd == -2)
    {
        // There was one big Part that we put a hole in. Restore it.
        // XXX could pose further undo/redo problems if we delete it here!

        size_t pos = track->index(start); // the Part we added
        //Part *part = (*track)[pos];
        track->remove(pos);
        //delete part; // We DON'T delete it, see API docs
        (*track)[pos-1]->setEnd(clippedStart);
        return;
    }

    // Restore any Parts that were clipped at the start or end of the
    // area
    if (clippedStart != -1)
    {
        size_t pos = track->index(start) - 1;
        (*track)[pos]->setEnd(clippedStart);
    }
    if (clippedEnd != -1)
    {
        size_t pos = track->index(end);
        (*track)[pos]->setStart(clippedEnd);
    }

    // Restore every deleted Part
    while (removed.size())
    {
        Part *part = removed.front();
        removed.pop_back();
        track->insert(part);
    }
}

