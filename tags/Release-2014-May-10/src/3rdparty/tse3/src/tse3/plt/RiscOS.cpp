/*
 * @(#)RiscOS.cpp 3.00 20 July 1999
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

#include "tse3/platform/RiscOS.h"
#include "tse3/platform/midisswis.h"
#include "kernel.h"

using namespace TSE3;
using namespace TSE3::Plt;

/*
 * NB:
 *
 * This is all hypothetical code since I've not been able to test it on a
 * RISC OS platform. There isn't a decent C++ compiler, and the Acorn CFront
 * isn't quite good enough ;-)
 *
 * I include it here for completeness, and to exorcise the demons.
 *
 */


/******************************************************************************
 * RISC OS MidiSchedulerFactory class
 *****************************************************************************/

MidiSchedulerFactory::MidiSchedulerFactory(bool b)
{
}


MidiSchedulerFactory::~MidiSchedulerFactory()
{
}


MidiScheduler *MidiSchedulerFactory::createScheduler()
{
    return new RiscOsMidiScheduler();
}


/******************************************************************************
 * RiscOsMidiScheduler class
 *****************************************************************************/

RiscOsMidiScheduler::RiscOsMidiScheduler()
{
    std::cout << "RiscOsMidiScheduler created ("
              << implementationName() << ")\n";
    _swix(MIDI_Init, _IN(0) | _OUT(0), 0, &(maxPort));
    for (int p = 0; p < maxPort; ++p)
    {
        addPort(p, false, p);
    }
}


RiscOsMidiScheduler::~RiscOsMidiScheduler()
{
    std::cout << "** RiscOsMidiScheduler deleted.\n";
}


const char *RiscOSMidiScheduler::impl_implementationName() const
{
    return "RiscOsMidiScheduler version 0.00 [dev].";
}


const char *NullMidiScheduler::impl_portName(int /*port*/) const
{
    return "RISC OS MIDI port";
}


const char *NullMidiScheduler::impl_portType(int /*port*/) const
{
    return "RISC OS MIDI port";
}


bool NullMidiScheduler::impl_portReadable(int /*port*/) const
{
    return true;
}


bool NullMidiScheduler::impl_portWriteable(int /*port*/) const
{
    return true;
}


void RiscOsMidiScheduler::impl_tx(MidiCommand mc)
{
    std::cout << "RiscOsMidiScheduler::tx - (now): "
              << mc.status << " on (" << mc.channel
              << "," << mc.port << ") with " << mc.data1
              << ", " << mc.data2 << "\n";
    int command = mc.channel
                + (mc.status << 4)
                + (mc.data1  << 8)
                + (mc.data2  << 16)
                + (mc.port   << 28)
    _swix(MIDI_TxCommand, _INR(0,1), command, 0);
}


void RiscOsMidiScheduler::impl_start(Clock start)
{
    std::cout << "RiscOsMidiScheduler::start(" << start << ")\n";

    // start the fast clock
    if (_tempo == 0) _tempo = 120;
    _swix(MIDI_FastClock, _INR(0,1), 2500 / _tempo, 0);

    // start the interface sending timing clocks then send a midi stop
    //   so that the instrument doesn't play itself away!
    _swix(MIDI_TxStart, 0);
    _swix(MIDI_TxCommand, _INR(0,1), 0xfc, 0);

    clockStarted(start);
}


void RiscOsMidiScheduler::impl_stop(Clock stop)
{
    std::cout << "RiscOsMidiScheduler::stopped at " << stop << "\n";
    _swix(MIDI_TxStop, 0);
    clockStopped(stop);
}


void RiscOsMidiScheduler::impl_moveTo(Clock moveTime, Clock newTime)
{
    clockMoved(moveTime, newTime);
}


Clock RiscOsMidiScheduler::impl_clock()
{
    int fcval;
    _swix(MIDI_FastClock, _IN(0) | _OUT(1), -1, &fcval);
    return msToClock(fcval);
}


int RiscOsMidiScheduler::impl_msecs()
{
    int fcval;
    _swix(MIDI_FastClock, _IN(0) | _OUT(1), -1, &fcval);
    return fcval;
}


void RiscOsMidiScheduler::impl_setTempo(int newTempo, Clock changeTime)
{
    tempoChanged(newTempo, changeTime);
}


bool RiscOsMidiScheduler::impl_eventWaiting()
{
    // find input buffer size              // SHOULD THIS BE HELD IN VARIABLE?
    int buffer_size;
    _swix(MIDI_SetBufferSize, _INR(0,1) | _OUT(0), 0, 0, &buffer_size);

    for (int n = 0; n <= maxPort; ++n)
    {
        // find number of free bytes in input buffer
        int bufsize;
        _swix(MIDI_InqBufferSize, _IN(0) | _OUT(0), n<<1, &bufsize);

        // event waiting if the two aren't equal
        if (buffer_size != bufsize) return 1;
    }

    return 0; // all buffers empty
}


MidiEvent RiscOsMidiScheduler::impl_rx()
{
    int c /*command*/, t /*time*/;
    _swix(MIDI_RxCommand, _IN(0) | _OUTR(0,1), -1, &c, &t);

    // return false if no event is pending
    if (c == 0) return MidiEvent();

    return MidiEvent(MidiCommand((c>>4)&0xf0, c&0xf, (c>>28)&0xf,
                                 (c>>8)&0x7f, (c>>16)&0x7f),
                     msToClock(t));
    remoteControl(e);
    return e;
}


void RiscOsMidiScheduler::impl_tx(MidiEvent e)
{

    if (e.event.data.status == MidiCommand_Invalid) return;
    std::cout << "RiscOsMidiScheduler::tx - " << e.event.time << ": "
              << e.event.data.status << " on (" << e.event.data.channel
              << "," << e.event.data.port << ") with " << e.event.data.data1
              << ", " << e.event.data.data2 << "\n";
    int command = mc.channel
                + (mc.status << 4)
                + (mc.data1  << 8)
                + (mc.data2  << 16)
                + (mc.port   << 28)
    _swix(MIDI_TxCommand, _INR(0,1), command, clockToMs(time));
}


void RiscOSMidiScheduler::impl_txSysEx(int port,
                                       const unsigned char *data, size_t size)
{
    std::cout << "RiscOsMidiScheduler::txByte - (now) " << byte << "\n";
    _swix(MIDI_TxByte, _IN(0), MidiSystem_SysExStart); // XXX port
    for (size_t n = 0; n < size; n++)
    {
        _swix(MIDI_TxByte, _IN(0), data[n]);
    }
    _swix(MIDI_TxByte, _IN(0), MidiSystem_SysExStop);
}
