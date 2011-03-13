/*
 * @(#)Track.cpp 3.00 8 July 2000
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

#include "tse3/cmd/Track.h"

#include "tse3/Track.h"
#include "tse3/Part.h"
#include "tse3/Song.h"
#include "tse3/MidiFilter.h"
#include "tse3/app/TrackSelection.h"

#include <algorithm>
#include <vector>
#include <iterator>

using namespace TSE3;
using namespace TSE3::Cmd;

/******************************************************************************
 * Track_SetInfo class
 *****************************************************************************/

Track_SetInfo::Track_SetInfo(TSE3::Track               *track,
                             const std::string         &title,
                             const TSE3::MidiFilter    &smef,
                             const TSE3::MidiParams    &mp,
                             const TSE3::DisplayParams &dp)
: Command("track info"), track(track),
  newTitle(title), smef(smef), mp(mp), dp(dp)
{
}


void Track_SetInfo::executeImpl()
{
    oldTitle = track->title();

    track->setTitle(newTitle);
    std::swap(smef, *(track->filter()));
    std::swap(mp,   *(track->params()));
    std::swap(dp,   *(track->displayParams()));
}


void Track_SetInfo::undoImpl()
{
    track->setTitle(oldTitle);
    std::swap(smef, *(track->filter()));
    std::swap(mp,   *(track->params()));
    std::swap(dp,   *(track->displayParams()));
}


/******************************************************************************
 * Track_Snip class
 *****************************************************************************/

Track_Snip::Track_Snip(TSE3::Track *track,
                       TSE3::Clock  snipTime)
: Command("snip part"), track(track), snipTime(snipTime), valid(false),
  shouldDelete(false)
{
    size_t pos = track->index(snipTime);
    if (pos < track->size() && (*track)[pos]->start() < snipTime)
    {
        oldPart      = (*track)[pos];
        oldEndTime   = oldPart->end();
        newPart      = new Part(*oldPart);
        valid        = true;
        shouldDelete = true;

        // Work out new Part's parameters
        newPart->setStart(snipTime);
        TSE3::Clock phraseStart = oldPart->start();
        if (oldPart->repeat())
        {
            while (phraseStart+oldPart->repeat() <= snipTime)
            {
                phraseStart += oldPart->repeat();
            }
        }
        newPart->filter()->setOffset(snipTime - phraseStart
                                     + oldPart->filter()->offset());
        if (oldPart->repeat())
        {
            while (newPart->filter()->offset() >= newPart->repeat())
            {
                newPart->filter()->setOffset(newPart->filter()->offset()
                                             - newPart->repeat());
            }
        }
    }
}


Track_Snip::~Track_Snip()
{
    if (shouldDelete)
    {
        delete newPart;
    }
}


void Track_Snip::executeImpl()
{
    if (valid)
    {
        oldPart->setEnd(snipTime);
        track->insert(newPart);
        shouldDelete = false;
    }
}


void Track_Snip::undoImpl()
{
    if (valid)
    {
        track->remove(newPart);
        oldPart->setEnd(oldEndTime);
        shouldDelete = true;
    }
}


/******************************************************************************
 * Track_Glue class
 *****************************************************************************/

bool Track_Glue::valid(TSE3::Track *track,
                       TSE3::Clock  glueTime)
{
    size_t pos = track->index(glueTime);
    if (pos != 0 && pos != track->size() && (*track)[pos]->start() <= glueTime)
    {
        if ((*track)[pos-1]->end() == (*track)[pos]->start())
        {
            return true;
        }
    }
    return false;
}


Track_Glue::Track_Glue(TSE3::Track *track,
                       TSE3::Clock  glueTime)
: Command("glue parts"), track(track), glueTime(glueTime),
  _valid(valid(track, glueTime)), oldPart(0)
{
    if (_valid)
    {
        pos = track->index(glueTime);
        oldEndTime = (*track)[pos]->start();
    }
}


Track_Glue::~Track_Glue()
{
    delete oldPart;
}


void Track_Glue::executeImpl()
{
    if (_valid)
    {
        oldPart = (*track)[pos];
        track->remove(pos);
        (*track)[pos-1]->setEnd(oldPart->end());
    }
}


void Track_Glue::undoImpl()
{
    if (_valid)
    {
        (*track)[pos-1]->setEnd(oldEndTime);
        track->insert(oldPart);
        oldPart = 0;
    }
}


/******************************************************************************
 * Track_RemovePart class
 *****************************************************************************/

Track_RemovePart::Track_RemovePart(Part *p)
: Command("remove part"), track(p->parent()), part(p), partno(0)
{
    if (!track) part=0;
}


Track_RemovePart::Track_RemovePart(Track *t, size_t p)
: Command("remove part"), track(t), part(NULL), partno(p)
{
}


Track_RemovePart::Track_RemovePart(Track *t, Part *p)
: Command("remove part"), track(t), part(p), partno(0)
{
}


Track_RemovePart::~Track_RemovePart()
{
    if (done())
    {
        delete part;
    }
}


void Track_RemovePart::executeImpl()
{
    if (!part && partno < (int)track->size())
    {
        part = (*track)[partno];
    }
    else
    {
        partno = track->index(part);
    }
    track->remove(part);
}


void Track_RemovePart::undoImpl()
{
    if (part)
    {
        track->insert(part);
    }
}



/******************************************************************************
 * Track_Sort class
 *****************************************************************************/

class TSE3::Cmd::Track_SortImpl
{
    public:

        Track_SortImpl(TSE3::Song                       *song,
                       TSE3::Cmd::Track_Sort::SortBy     by,
                       TSE3::Cmd::Track_Sort::SortOrder  order,
                       TSE3::App::TrackSelection        *selection)
            : song(song), by(by), order(order), selection(selection)
        {
            for (size_t trk = 0; trk < song->size(); ++trk)
            {
                original_order.push_back((*song)[trk]);
            }
            if (selection)
            {
                std::copy(selection->begin(), selection->end(),
                          std::back_inserter(selected_tracks));
            }

            comparator = &Track_SortImpl::compare_name;
            switch (by)
            {
                case Track_Sort::ByName:
                {
                    comparator = &Track_SortImpl::compare_name;
                    break;
                }
                case Track_Sort::ByMuted:
                {
                    comparator = &Track_SortImpl::compare_muted;
                    break;
                }
                case Track_Sort::BySelected:
                {
                    comparator = &Track_SortImpl::compare_selected;
                    break;
                }
                case Track_Sort::ByPort:
                {
                    comparator = &Track_SortImpl::compare_port;
                    break;
                }
                case Track_Sort::ByChannel:
                {
                    comparator = &Track_SortImpl::compare_channel;
                    break;
                }
                case Track_Sort::BySize:
                {
                    comparator = &Track_SortImpl::compare_size;
                    break;
                }
            }
        }

        void executeImpl();
        void undoImpl();

    private:

        typedef bool (Track_SortImpl::*comparator_t)(size_t,size_t);

        TSE3::Song                       *song;
        TSE3::Cmd::Track_Sort::SortBy     by;
        TSE3::Cmd::Track_Sort::SortOrder  order;
        TSE3::App::TrackSelection        *selection;
        std::vector<TSE3::Track *>        original_order;
        std::vector<TSE3::Track *>        selected_tracks;
        comparator_t                      comparator;

        void swap(size_t index1, size_t index2);
        bool compare(size_t index1, size_t index2);

        bool compare_name(size_t index1, size_t index2);
        bool compare_muted(size_t index1, size_t index2);
        bool compare_selected(size_t index1, size_t index2);
        bool compare_port(size_t index1, size_t index2);
        bool compare_channel(size_t index1, size_t index2);
        bool compare_size(size_t index1, size_t index2);

        void reselectTracks();
};


void TSE3::Cmd::Track_SortImpl::executeImpl()
{
    for (size_t trk = 0; trk < song->size(); ++trk)
    {
        size_t bestMatch = trk;
        for (size_t subtrk = trk+1; subtrk < song->size(); ++subtrk)
        {
            bool comparison = (this->*comparator)(bestMatch, subtrk);
            comparison ^= order;
            if (comparison)
            {
                bestMatch = subtrk;
            }
        }
        swap(trk, bestMatch);
    }
    reselectTracks();
}


void Track_SortImpl::undoImpl()
{
    while (song->size())
    {
        song->remove((size_t)0);
    }
    std::vector<TSE3::Track*>::iterator i = original_order.begin();
    while (i != original_order.end())
    {
        song->insert(*i);
        ++i;
    }
    reselectTracks();
}


void Track_SortImpl::reselectTracks()
{
    if (selection)
    {
        std::vector<TSE3::Track*>::iterator i = selected_tracks.begin();
        while (i != selected_tracks.end())
        {
            selection->select(*i, true);
            ++i;
        }
    }
}


void Track_SortImpl::swap(size_t index1, size_t index2)
{
    if (index1 == index2) return;

    if (index2 < index1)
    {
        std::swap(index1, index2);
    }
    TSE3::Track *track1 = (*song)[index1];
    TSE3::Track *track2 = (*song)[index2];
    song->remove(index2);
    song->insert(track2, index1);
    song->remove(index1+1);
    song->insert(track1, index2);
}


bool Track_SortImpl::compare_name(size_t index1, size_t index2)
{
    return (*song)[index1]->title().compare((*song)[index2]->title()) > 0;
}


bool Track_SortImpl::compare_muted(size_t index1, size_t index2)
{
    return (*song)[index1]->filter()->status()
               > (*song)[index2]->filter()->status();
}


bool Track_SortImpl::compare_selected(size_t index1, size_t index2)
{
    bool res1  = std::find(selected_tracks.begin(), selected_tracks.end(),
                           (*song)[index1])
              != selected_tracks.end();
    bool res2  = std::find(selected_tracks.begin(), selected_tracks.end(),
                          (*song)[index2])
              != selected_tracks.end();
    return res1 < res2;
}


bool Track_SortImpl::compare_port(size_t index1, size_t index2)
{
    return (*song)[index1]->filter()->port()
               > (*song)[index2]->filter()->port();
}


bool Track_SortImpl::compare_channel(size_t index1, size_t index2)
{
    return (*song)[index1]->filter()->channel()
               > (*song)[index2]->filter()->channel();
}


bool Track_SortImpl::compare_size(size_t index1, size_t index2)
{
    return (*song)[index1]->size() > (*song)[index2]->size();
}


Track_Sort::Track_Sort(Song *song, SortBy by, SortOrder order,
                       TSE3::App::TrackSelection *selection)
: Command("sort tracks"), pimpl(new Track_SortImpl(song, by, order, selection))
{
}


Track_Sort::~Track_Sort()
{
    delete pimpl;
}


void Track_Sort::executeImpl()
{
    pimpl->executeImpl();
}


void Track_Sort::undoImpl()
{
    pimpl->undoImpl();
}
