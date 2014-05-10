/*
 * @(#)plt/RiscOS.h 3.00 20 July 1999
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

#ifndef TSE3_PLT_RISCOS_H
#define TSE3_PLT_RISCOS_H

#include "tse3/MidiScheduler.h"

namespace TSE3
{
    namespace Plt
    {
        /**
         * This is the RISC OS standard MIDI @ref MidiScheduler implementation.
         *
         * @short   RISC OS MidiScheduler implementation
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     MidiScheduler
         * @see     MidiSchedulerFactory
         */
        class RiscOsMidiScheduler : public MidiScheduler
        {
            public:

                RiscOsMidiScheduler();
                virtual ~RiscOsMidiScheduler();

            protected:

                /**
                 * @reimplemented
                 */
                virtual const char *impl_implementationName() const;
                /**
                 * @reimplemented
                 */
                virtual const char *impl_portName(int port) const;
                /**
                 * @reimplemented
                 */
                virtual const char *impl_portType(int port) const;
                /**
                 * @reimplemented
                 */
                virtual bool impl_portReadable(int port) const;
                /**
                 * @reimplemented
                 */
                virtual bool impl_portWriteable(int port) const;
                /**
                 * @reimplemented
                 */
                virtual void impl_start(Clock clock);
                /**
                 * @reimplemented
                 */
                virtual void impl_stop(Clock clock);
                /**
                 * @reimplemented
                 */
                virtual void impl_moveTo(Clock moveTime, Clock newTime);
                /**
                 * @reimplemented
                 */
                virtual Clock impl_clock();
                /**
                 * @reimplemented
                 */
                virtual int impl_msecs();
                /**
                 * @reimplemented
                 */
                virtual void impl_setTempo(int tempo, Clock changeTime);
                /**
                 * @reimplemented
                 */
                virtual bool impl_eventWaiting();
                /**
                 * @reimplemented
                 */
                virtual MidiEvent impl_rx();
                /**
                 * @reimplemented
                 */
                virtual void impl_tx(MidiCommand mc);
                /**
                 * @reimplemented
                 */
                virtual void impl_tx(MidiEvent mc);
                /**
                 * @reimplemented
                 */
                virtual void impl_txSysEx(int port,
                                          const unsigned char *data,
                                          size_t size);

            private:

                RiscOSMidiScheduler(const RiscOSMidiScheduler &);
                RiscOSMidiScheduler &operator=(const RiscOSMidiScheduler &);

                int maxPort;
        };
    }
}

#endif
