/*
 * @(#)listen/Panic.h 3.00 21 June 1999
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

#ifndef TSE3_LISTEN_PANIC_H
#define TSE3_LISTEN_PANIC_H

namespace TSE3
{
    class Panic;

    /**
     * @ref Panic listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class PanicListener
    {
        public:
            typedef Panic notifier_type;

            enum WhatChanged
            {
                StatusChanged              = 0x0001,
                PortChanged                = 0x2000,
                MidiResetChanged           = 0x0002,
                GmResetChanged             = 0x0004,
                GsResetChanged             = 0x0008,
                XgResetChanged             = 0x0010,
                GsIDMaskChanged            = 0x0020,
                XgIDMaskChanged            = 0x0040,
                AllNotesOffChanged         = 0x0080,
                AllNotesOffManuallyChanged = 0x0100,
                AllModsOffChanged          = 0x0200,
                AllPitchOffChanged         = 0x0400,
                AllCtrlOffChanged          = 0x0800,
                LiftSustainChanged         = 0x1000
            };

            virtual void Panic_Altered(Panic *, int /*what*/) {}
    };
}

#endif
