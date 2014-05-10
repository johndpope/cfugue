/*
 * @(#)listen/MidiParams.h 3.00 1 June 1999
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

#ifndef TSE3_LISTEN_MIDIPARAMS_H
#define TSE3_LISTEN_MIDIPARAMS_H

namespace TSE3
{
    class MidiParams;

    /**
     * @ref MidiParams listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class MidiParamsListener
    {
        public:
            typedef MidiParams notifier_type;

            enum WhatChanged
            {
                BankLsbChanged = 0x01,
                BankMsbChanged = 0x02,
                ProgramChanged = 0x04,
                PanChanged     = 0x08,
                ReverbChanged  = 0x10,
                ChorusChanged  = 0x11,
                VolumeChanged  = 0x12
            };

            virtual void MidiParams_Altered(MidiParams *, int /*what*/) {}
    };
}

#endif
