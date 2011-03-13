/*
 * @(#)listen/MidiFilter.h 3.00 8 June 1999
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

#ifndef TSE3_LISTEN_EVENTFILTER_H
#define TSE3_LISTEN_EVENTFILTER_H

namespace TSE3
{
    class MidiFilter;

    /**
     * @ref MidiFilter listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class MidiFilterListener
    {
        public:
            typedef MidiFilter notifier_type;

            enum WhatChanged
            {
                StatusChanged        = 0x0001,
                ChannelFilterChanged = 0x0002,
                PortFilterChanged    = 0x0004,
                ChannelChanged       = 0x0008,
                PortChanged          = 0x0010,
                OffsetChanged        = 0x0020,
                TimeScaleChanged     = 0x0040,
                QuantiseChanged      = 0x0080,
                MinLengthChanged     = 0x0100,
                MaxLengthChanged     = 0x0200,
                TransposeChanged     = 0x0400,
                MinVelocityChanged   = 0x0800,
                MaxVelocityChanged   = 0x1000,
                VelocityScaleChanged = 0x2000
            };

            virtual void MidiFilter_Altered(MidiFilter *, int /*what*/) {}
    };
}

#endif
