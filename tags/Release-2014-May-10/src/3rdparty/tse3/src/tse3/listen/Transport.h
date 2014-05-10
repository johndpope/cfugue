/*
 * @(#)listen/Transport.h 3.00 25 May 1999
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

#ifndef TSE3_LISTEN_TRANSPORT_H
#define TSE3_LISTEN_TRANSPORT_H

#include "tse3/Midi.h"

namespace TSE3
{
    class Transport;

    /**
     * @ref Transport listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class TransportListener
    {
        public:
            typedef Transport notifier_type;

            enum WhatChanged
            {
                SynchroChanged           = 0x01,
                PunchInChanged           = 0x02,
                AutoStopChanged          = 0x04,
                PlayLeadInChanged        = 0x08,
                RecordLeadInChanged      = 0x10,
                LookAheadChanged         = 0x20,
                AdaptiveLookAheadChanged = 0x40
            };

            virtual void Transport_Altered(Transport *, int /*what*/)  {}
            virtual void Transport_Status(Transport *, int /*status*/) {}
    };

    /**
     * This class is used as a base for objects that want to be called back
     * from the @ref Transport object to be informed when MIDI activity occurs.
     * Uses of this may be a MIDI activity monitor, for example.
     *
     * @short   MIDI activity callback for Transport
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Transport
     */
    class TransportCallback
    {
        public:

            TransportCallback() {}

            virtual ~TransportCallback() = 0;

            /**
             * Called when some MIDI input has been recieved by the
             * @ref Transport object.
             */
            virtual void Transport_MidiIn(MidiCommand c) = 0;

            /**
             * Called when some MIDI output has been sent by the @ref Transport
             * object.
             */
            virtual void Transport_MidiOut(MidiCommand c) = 0;

        private:

            TransportCallback &operator=(const TransportCallback &);
            TransportCallback(const TransportCallback &);
    };
}

#endif
