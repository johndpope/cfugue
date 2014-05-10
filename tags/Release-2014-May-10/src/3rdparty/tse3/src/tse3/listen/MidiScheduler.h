/*
 * @(#)listen/MidiScheduler.h 3.00 14 May 1999
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

#ifndef TSE3_LISTEN_SCHEDULER_H
#define TSE3_LISTEN_SCHEDULER_H

#include <cstddef>

namespace TSE3
{
    class MidiScheduler;

    /**
     * @ref MidiScheduler listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class MidiSchedulerListener
    {
        public:
            typedef MidiScheduler notifier_type;

            virtual void MidiScheduler_Started(MidiScheduler *)             {}
            virtual void MidiScheduler_Stopped(MidiScheduler *)             {}
            virtual void MidiScheduler_Moved(MidiScheduler *)               {}
            virtual void MidiScheduler_TempoChanged(MidiScheduler *)        {}
            virtual void MidiScheduler_PortAdded(MidiScheduler *, size_t)   {}
            virtual void MidiScheduler_PortRemoved(MidiScheduler *, size_t) {}
    };
}

#endif
