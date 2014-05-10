/*
 * @(#)listen/EventTrack.h 3.00 24 May 1999
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

#ifndef TSE3_LISTEN_EVENTTRACK_H
#define TSE3_LISTEN_EVENTTRACK_H

#include <cstddef>

namespace TSE3
{
    template <class etype> class EventTrack;

    /**
     * @ref EventTrack listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    template <class etype>
    class EventTrackListener
    {
        public:
            typedef EventTrack<etype> notifier_type;

            virtual void EventTrack_EventAltered(EventTrack<etype> *,
                                                 size_t /*index*/)        {}
            virtual void EventTrack_EventInserted(EventTrack<etype> *,
                                                  size_t /*index*/)       {}
            virtual void EventTrack_EventErased(EventTrack<etype> *,
                                                size_t /*index*/)         {}
    };
}

#endif
