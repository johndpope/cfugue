/*
 * @(#)listen/ins/Destination.h 3.00 21 August 2000
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

#ifndef TSE3_LISTEN_INS_DESTINATION_H
#define TSE3_LISTEN_INS_DESTINATION_H

#include <cstddef>

namespace TSE3
{
    namespace Ins
    {
        class Instrument;
        class Destination;

        /**
         * @ref Desintation listener interface.
         *
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Listener
         */
        class DestinationListener
        {
            public:
                typedef Destination notifier_type;

                virtual void Destination_Altered(Destination *,
                                                 size_t /*channel*/,
                                                 size_t /*port*/,
                                                 Instrument *)           {}
                virtual void Destination_InstrumentAdded(Destination*,
                                                         Instrument *)   {}
                virtual void Destination_InstrumentRemoved(Destination *,
                                                           Instrument *) {}
        };
    }
}

#endif
