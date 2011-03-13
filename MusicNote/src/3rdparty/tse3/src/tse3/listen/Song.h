/*
 * @(#)listen/Song.h 3.00 26 May 1999
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

#ifndef TSE3_LISTEN_SONG_H
#define TSE3_LISTEN_SONG_H

#include "tse3/Midi.h"

#include <cstddef>

namespace TSE3
{
    class Song;
    class Track;

    /**
     * @ref Song listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class SongListener
    {
        public:
            typedef Song notifier_type;

            virtual void Song_InfoAltered(Song *)                         {}
            virtual void Song_SoloTrackAltered(Song *, int /*soloTrack*/) {}
            virtual void Song_RepeatAltered(Song *, bool /*repeat*/)      {}
            virtual void Song_FromAltered(Song *, Clock /*from*/)         {}
            virtual void Song_ToAltered(Song *, Clock /*to*/)             {}
            virtual void Song_TrackInserted(Song *, Track * /*track*/)    {}
            virtual void Song_TrackRemoved(Song *, Track * /*track*/,
                                           size_t /*index*/)              {}
    };
}

#endif
