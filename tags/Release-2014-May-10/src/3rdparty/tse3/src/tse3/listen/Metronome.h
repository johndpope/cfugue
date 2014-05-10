/*
 * @(#)listen/Metronome.h 3.00 25 May 1999
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

#ifndef TSE3_LISTEN_METRONOME_H
#define TSE3_LISTEN_METRONOME_H

namespace TSE3
{
    class Metronome;

    /**
     * @ref Metronome listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class MetronomeListener
    {
        public:
            typedef Metronome notifier_type;

            enum WhatChanged
            {
                ChannelChanged      = 0x01,
                PortChanged         = 0x02,
                DurationChanged     = 0x04,
                BarNoteChanged      = 0x08,
                BarVelocityChanged  = 0x10,
                BeatNoteChanged     = 0x20,
                BeatVelocityChanged = 0x40,
                StatusChanged       = 0x80
            };

            virtual void Metronome_Altered(Metronome *, int /*what*/) {}
    };
}

#endif
