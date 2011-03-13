/*
 * @(#)TrackSelection.cpp 3.00 17 January 2001
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

#include "tse3/app/TrackSelection.h"

#include "tse3/Song.h"
#include "tse3/Track.h"

#include <algorithm>

using namespace TSE3;
using namespace TSE3::App;

TrackSelection::TrackSelection()
: tracksValid(false), minTrack(0), maxTrack(0)
{
}


TrackSelection::TrackSelection(const TrackSelection &t)
: TSE3::Listener<TSE3::TrackListener>(),
  TSE3::Listener<TSE3::App::PartSelectionListener>(),
  TSE3::Notifier<TSE3::App::TrackSelectionListener>()
{
    // Copy data members
    tracks      = t.tracks;
    tracksValid = t.tracksValid;
    minTrack    = t.minTrack;
    maxTrack    = t.maxTrack;

    // Listen to each Track
    iterator_t i = tracks.begin();
    while (i != tracks.end())
    {
        Listener<TrackListener>::attachTo(*i);
        ++i;
    }
}


TrackSelection::~TrackSelection()
{
    // Detach from current Tracks
    while (tracks.size())
    {
        removeTrack(tracks.front());
    }
}


bool TrackSelection::isSelected(TSE3::Track *track) const
{
    std::vector<TSE3::Track*>::const_iterator i
        = find(tracks.begin(), tracks.end(), track);
    return i != tracks.end();
}


void TrackSelection::select(TSE3::Track *track, bool add)
{
    if (!add)
    {
        clear();
    }
    addTrack(track);
}


void TrackSelection::deselect(TSE3::Track *track)
{
    removeTrack(track);
}


void TrackSelection::clear()
{
    minTrack = maxTrack = 0;
    tracksValid = false;
    while (tracks.size())
    {
        Track *t = *tracks.begin();
        Listener<TrackListener>::detachFrom(t);
        tracks.erase(tracks.begin());
        notify(&TrackSelectionListener::TrackSelection_Selected, t, false);
    }
    recalculateEnds();
}


void TrackSelection::selectAll(TSE3::Song *song)
{
    for (size_t t = 0; t < song->size(); ++t)
    {
        addTrack((*song)[t]);
    }
}


void TrackSelection::invert(TSE3::Song *song)
{
    for (size_t t = 0; t < song->size(); ++t)
    {
        TSE3::Track * const track = (*song)[t];
        if (isSelected(track))
        {
            removeTrack(track);
        }
        else
        {
            addTrack(track);
        }
    }
}


TrackSelection &TrackSelection::operator=(const TrackSelection &p)
{
    // Detach from current Tracks
    while (tracks.size())
    {
        Track *track = tracks.front();
        removeTrack(track);
    }

    // Copy data members
    tracks      = p.tracks;
    tracksValid = p.tracksValid;
    minTrack    = p.minTrack;
    maxTrack    = p.maxTrack;

    // Listen to each Track
    iterator_t i = tracks.begin();
    while (i != tracks.end())
    {
        Listener<TrackListener>::attachTo(*i);
        notify(&TrackSelectionListener::TrackSelection_Selected, *i, true);
        ++i;
    }

    return *this;
}


void TrackSelection::addTrack(TSE3::Track *track)
{
    if (track->parent()
        && find(tracks.begin(), tracks.end(), track) == tracks.end())
    {
        tracks.push_back(track);
        Listener<TrackListener>::attachTo(track);
        size_t trackno = track->parent()->index(track);
        if (!tracksValid || trackno < minTrack->parent()->index(minTrack))
        {
            minTrack = track;
        }
        if (!tracksValid || trackno > maxTrack->parent()->index(maxTrack))
        {
            maxTrack = track;
            tracksValid = true;
        }
        notify(&TrackSelectionListener::TrackSelection_Selected, track, true);
    }
}


void TrackSelection::removeTrack(TSE3::Track *track)
{
    std::vector<TSE3::Track*>::iterator i
        = find(tracks.begin(), tracks.end(), track);
    if (i != tracks.end())
    {
        Listener<TrackListener>::detachFrom(track);
        tracks.erase(i);
        recalculateEnds();
        notify(&TrackSelectionListener::TrackSelection_Selected, track, false);
    }
}


void TrackSelection::recalculateEnds()
{
    minTrack = maxTrack = 0;
    tracksValid = false;
    std::vector<TSE3::Track*>::iterator i = tracks.begin();
    while (i != tracks.end())
    {
        if (!minTrack)
        {
            // Only happens 1st time round the loop
            minTrack = maxTrack = *i;
            tracksValid = true;
        }
        else
        {
            if ((*i)->parent()->index(*i) < minTrack->parent()->index(minTrack))
            {
                minTrack = *i;
            }
            if ((*i)->parent()->index(*i) > maxTrack->parent()->index(maxTrack))
            {
                maxTrack = *i;
            }
        }
        ++i;
    }
}


void TrackSelection::Track_Reparented(Track *track)
{
    if (!track->parent())
    {
        removeTrack(track);
    }
}


void TrackSelection::Notifier_Deleted(Track *track)
{
    removeTrack(track);
}


void TrackSelection::PartSelection_Selected(PartSelection *,
                                            TSE3::Part *, bool selected)
{
    if (selected)
    {
        clear();
    }
}
