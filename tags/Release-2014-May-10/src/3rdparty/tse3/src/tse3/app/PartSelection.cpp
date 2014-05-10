/*
 * @(#)PartSelection.cpp 3.00 25 May 1999
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

#include "tse3/app/PartSelection.h"

#include "tse3/Song.h"
#include "tse3/Track.h"
#include "tse3/Part.h"
#include "tse3/Phrase.h"

#include <algorithm>

using namespace TSE3;
using namespace TSE3::App;

PartSelection::PartSelection()
: timesValid(false), _earliest(-1), _latest(-1),
  tracksValid(false), _minTrack(0), _maxTrack(0)
{
}


PartSelection::PartSelection(const PartSelection &p)
: TSE3::Listener<TSE3::PartListener>(),
  TSE3::Listener<TSE3::App::TrackSelectionListener>(),
  TSE3::Notifier<TSE3::App::PartSelectionListener>()
{
    // Copy data members
    parts       = p.parts;
    timesValid  = p.timesValid;
    _earliest   = p._earliest;
    _latest     = p._latest;
    tracksValid = p.tracksValid;
    _minTrack   = p._minTrack;
    _maxTrack   = p._maxTrack;

    // Listen to each Part
    iterator_t i = parts.begin();
    while (i != parts.end())
    {
        Listener<PartListener>::attachTo(*i);
        ++i;
    }
}


PartSelection::~PartSelection()
{
    // Detach from current Parts
    while (parts.size())
    {
        removePart(parts.front());
    }
}


bool PartSelection::isSelected(TSE3::Part *part) const
{
    std::vector<TSE3::Part*>::const_iterator i
        = find(parts.begin(), parts.end(), part);
    return i != parts.end();
}


void PartSelection::select(TSE3::Part *part, bool add)
{
    if (!add)
    {
        clear();
    }
    addPart(part);
}


void PartSelection::deselect(TSE3::Part *part)
{
    removePart(part);
}


void PartSelection::clear()
{
    _earliest   = _latest     = -1;
    _minTrack   = _maxTrack   = 0;
    timesValid  = tracksValid = false;
    while (parts.size())
    {
        Part *p = *parts.begin();
        Listener<PartListener>::detachFrom(p);
        parts.erase(parts.begin());
        notify(&PartSelectionListener::PartSelection_Selected, p, false);
    }
    recalculateEnds();
}


void PartSelection::selectAll(TSE3::Song *song)
{
    for (size_t t = 0; t < song->size(); ++t)
    {
        selectAll((*song)[t]);
    }
}


void PartSelection::selectAll(TSE3::Track *track)
{
    for (size_t p = 0; p < track->size(); ++p)
    {
        addPart((*track)[p]);
    }
}


void PartSelection::invert(TSE3::Song *song)
{
    for (size_t t = 0; t < song->size(); ++t)
    {
        TSE3::Track * const track = (*song)[t];
        for (size_t p = 0; p < track->size(); ++p)
        {
            if (isSelected((*track)[p]))
            {
                removePart((*track)[p]);
            }
            else
            {
                addPart((*track)[p]);
            }
        }
    }
}

void PartSelection::selectBetween(TSE3::Song *song,
                                  TSE3::Clock start, TSE3::Clock end,
                                  bool inside)
{
    for (size_t t = 0; t < song->size(); ++t)
    {
        selectBetween((*song)[t], start, end, inside);
    }
}

void PartSelection::selectBetween(TSE3::Track *track,
                                  TSE3::Clock start, TSE3::Clock end,
                                  bool inside)
{
    for (size_t p = 0; p < track->size(); ++p)
    {
        Part *part   = (*track)[p];
        bool  within = false;
        if (part->start() < start && part->end() > start) within = true;
        if (part->start() < end   && part->end() > end)   within = true;

        if (within == inside)
        {
            addPart(part);
        }
    }
}


PartSelection &PartSelection::operator=(const PartSelection &p)
{
    // Detach from current Parts
    while (parts.size())
    {
        Part *part = parts.front();
        removePart(part);
    }

    // Copy data members
    parts       = p.parts;
    timesValid  = p.timesValid;
    _earliest   = p._earliest;
    _latest     = p._latest;
    tracksValid = p.tracksValid;
    _minTrack   = p._minTrack;
    _maxTrack   = p._maxTrack;

    // Listen to each Part
    iterator_t i = parts.begin();
    while (i != parts.end())
    {
        Listener<PartListener>::attachTo(*i);
        notify(&PartSelectionListener::PartSelection_Selected, *i, true);
        ++i;
    }

    return *this;
}


void PartSelection::addPart(TSE3::Part *part)
{
    if (part->parent() && find(parts.begin(), parts.end(), part) == parts.end())
    {
        parts.push_back(part);
        Listener<PartListener>::attachTo(part);
        if (!timesValid || part->start() < _earliest)
        {
            _earliest = part->start();
        }
        if (!timesValid || part->end() > _latest)
        {
            _latest = part->end();
            timesValid = true;
        }
        size_t track = part->parent()->parent()->index(part->parent());
        if (!tracksValid || track < _minTrack)
        {
            _minTrack = track;
        }
        if (!tracksValid || track > _maxTrack)
        {
            _maxTrack = track;
            tracksValid = true;
        }
        notify(&PartSelectionListener::PartSelection_Selected, part, true);
    }
}


void PartSelection::removePart(TSE3::Part *part)
{
    std::vector<TSE3::Part*>::iterator i
        = find(parts.begin(), parts.end(), part);
    if (i != parts.end())
    {
        Listener<PartListener>::detachFrom(part);
        parts.erase(i);
        recalculateEnds();
        notify(&PartSelectionListener::PartSelection_Selected, part, false);
    }
}


void PartSelection::recalculateEnds()
{
    if (parts.size())
    {
        std::vector<TSE3::Part*>::iterator i = parts.begin();
        _earliest   = (*i)->start();
        _latest     = (*i)->end();
        _minTrack   = (*i)->parent()->parent()->index((*i)->parent());
        _maxTrack   = _minTrack;
        timesValid  = true;
        tracksValid = true;
        while (++i != parts.end())
        {
            if ((*i)->start() < _earliest) _earliest = (*i)->start();
            if ((*i)->end()   < _latest)   _latest   = (*i)->end();
            size_t track = (*i)->parent()->parent()->index((*i)->parent());
            if (track < _minTrack) _minTrack = track;
            if (track > _maxTrack) _maxTrack = track;
        }
    }
    else
    {
        _earliest   = -1;
        _latest     = -1;
        _minTrack   = 0;
        _maxTrack   = 0;
        timesValid  = false;
        tracksValid = false;
    }
}


void PartSelection::Part_StartAltered(Part *, Clock)
{
    recalculateEnds();
}


void PartSelection::Part_EndAltered(Part *, Clock)
{
    recalculateEnds();
}


void PartSelection::Part_Reparented(Part *part)
{
    if (!part->parent())
    {
        removePart(part);
    }
}


void PartSelection::Notifier_Deleted(Part *part)
{
    removePart(part);
}


void PartSelection::TrackSelection_Selected(TrackSelection *,
                                            TSE3::Track *, bool selected)
{
    if (selected)
    {
        clear();
    }
}

