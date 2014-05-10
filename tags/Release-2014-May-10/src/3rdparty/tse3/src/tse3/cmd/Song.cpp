/*
 * @(#)Song.cpp 3.00 10 Nov 1999
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

#include "tse3/cmd/Song.h"
#include "tse3/Song.h"
#include "tse3/Track.h"
#include "tse3/Part.h"
#include "tse3/util/Song.h"

#include <utility>
#include <functional>

using namespace TSE3;
using namespace TSE3::Cmd;

/******************************************************************************
 * Song_SetInfo class
 *****************************************************************************/

Song_SetInfo::Song_SetInfo(TSE3::Song *s,
              const std::string &title,
              const std::string &author,
              const std::string &copyright,
              const std::string &date)
: Command("song info"), song(s),
  newTitle(title), newAuthor(author), newCopyright(copyright), newDate(date)
{
}


void Song_SetInfo::executeImpl()
{
    // Save old info
    oldTitle     = song->title();
    oldAuthor    = song->author();
    oldCopyright = song->copyright();
    oldDate      = song->date();

    // Set new info
    song->setTitle(newTitle);
    song->setAuthor(newAuthor);
    song->setCopyright(newCopyright);
    song->setDate(newDate);
}


void Song_SetInfo::undoImpl()
{
    song->setTitle(oldTitle);
    song->setAuthor(oldAuthor);
    song->setCopyright(oldCopyright);
    song->setDate(oldDate);
}


/******************************************************************************
 * Song_InsertTrack class
 *****************************************************************************/

Song_InsertTrack::Song_InsertTrack(Song *s, size_t t)
: Command("insert track"), song(s), track(t)
{
    if (t > song->size())
    {
        // Invalid parameter
        track = -1;
    }
}


void Song_InsertTrack::executeImpl()
{
    if (track != -1)
    {
        song->insert(track);
    }
}


void Song_InsertTrack::undoImpl()
{
    if (track != -1)
    {
        Track *t = (*song)[track];
        song->remove(track);
        delete t;
    }
}


/******************************************************************************
 * Song_RemoveTrack class
 *****************************************************************************/

Song_RemoveTrack::Song_RemoveTrack(Track *t)
: Command("remove track"), song(t->parent()), track(t), trackno(0)
{
    if (!song) track=0;
}


Song_RemoveTrack::Song_RemoveTrack(Song *s, size_t t)
: Command("remove track"), song(s), track(NULL), trackno(t)
{
}


Song_RemoveTrack::Song_RemoveTrack(Song *s, Track *t)
: Command("remove track"), song(s), track(t), trackno(0)
{
}


Song_RemoveTrack::~Song_RemoveTrack()
{
    if (done())
    {
        delete track;
    }
}


void Song_RemoveTrack::executeImpl()
{
    if (!track && trackno < (int)song->size())
    {
        track = (*song)[trackno];
    }
    else
    {
        trackno = song->index(track);
    }
    song->remove(track);
}


void Song_RemoveTrack::undoImpl()
{
    if (track)
    {
        song->insert(track, trackno);
    }
}


/******************************************************************************
 * Song_SoloTrack class
 *****************************************************************************/

Song_SoloTrack::Song_SoloTrack(Song *s, int track)
: Command("solo track"), song(s), track(track)
{
}


void Song_SoloTrack::executeImpl()
{
    old = song->soloTrack();
    song->setSoloTrack(track);
}


void Song_SoloTrack::undoImpl()
{
    song->setSoloTrack(old);
}
