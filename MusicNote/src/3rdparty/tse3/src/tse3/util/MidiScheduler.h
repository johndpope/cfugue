/*
 * @(#)util/MidiScheduler.h 3.00 2 August 2000
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

#ifndef TSE3_UTIL_MIDISCHEDULER_H
#define TSE3_UTIL_MIDISCHEDULER_H

#include "tse3/MidiScheduler.h"

#include <iostream>
#include <cstddef>

namespace TSE3
{
    namespace Util
    {
        /**
         * This is a very simple example @ref TSE3::MidiScheduler
         * implementation that merely produces diagnostic output to a given
         * ostream.
         *
         * @short   Diagnostic ostream MidiScheduler implementation
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     MidiScheduler
         * @see     MidiSchedulerFactory
         */
        class StreamMidiScheduler : public MidiScheduler
        {
            public:

                StreamMidiScheduler(std::ostream &stream = std::cout);
                virtual ~StreamMidiScheduler();

            protected:

                /**
                 * @reimplemented
                 */
                virtual OIL::StrUtils_Return_Type impl_implementationName() const;
                /**
                 * @reimplemented
                 */
                virtual OIL::StrUtils_Return_Type impl_portName(int port) const;
                /**
                 * @reimplemented
                 */
                virtual OIL::StrUtils_Return_Type impl_portType(int port) const;
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

                StreamMidiScheduler &operator=(const StreamMidiScheduler &);
                StreamMidiScheduler(const StreamMidiScheduler &);

                void outClock(Clock c);
                void outMidiCommand(MidiCommand mc);

                std::ostream &out;
                Clock         clock;
        };

        /**
         * If a MidiScheduler cannot be made for a given platform then this is
         * a placeholder 'null' implementation that does absolutely nothing.
         *
         * @short   Null MidiScheduler implementation (does nothing!)
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     MidiScheduler
         * @see     MidiSchedulerFactory
         */
        class NullMidiScheduler : public MidiScheduler
        {
            public:

                NullMidiScheduler();
                virtual ~NullMidiScheduler();

            protected:

                /**
                 * @reimplemented
                 */
                virtual OIL::StrUtils_Return_Type impl_implementationName() const;
                /**
                 * @reimplemented
                 */
                virtual OIL::StrUtils_Return_Type impl_portName(int port) const;
                /**
                 * @reimplemented
                 */
                virtual OIL::StrUtils_Return_Type impl_portType(int port) const;
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

                NullMidiScheduler &operator=(const NullMidiScheduler &);
                NullMidiScheduler(const NullMidiScheduler &);

                Clock clock;
        };
    }
}

#endif
