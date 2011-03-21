/*
 * @(#)MidiScheduler.cpp 3.00 2 August 2000
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

#include "tse3/util/MidiScheduler.h"

#include "tse3/util/NoteNumber.h"
#include <iomanip>
#include <iostream>

using namespace TSE3;
using namespace TSE3::Util;

/******************************************************************************
 * StreamMidiScheduler class
 *****************************************************************************/

StreamMidiScheduler::StreamMidiScheduler(std::ostream &stream)
: out(stream)
{
    out << "[StreamMidiScheduler::ctor]     " << implementationName() << "\n";
    addPort(0, 0);
}


StreamMidiScheduler::~StreamMidiScheduler()
{
    out << "[StreamMidiScheduler::dtor]\n";
}


OIL::StrUtils_Return_Type StreamMidiScheduler::impl_implementationName() const
{
    return "StreamMidiScheduler version 0.10 [dev].";
}


OIL::StrUtils_Return_Type StreamMidiScheduler::impl_portName(int /*port*/) const
{
    return "StreamMidiScheduler port";
}


OIL::StrUtils_Return_Type StreamMidiScheduler::impl_portType(int /*port*/) const
{
    return "StreamMidiScheduler port";
}


bool StreamMidiScheduler::impl_portReadable(int /*port*/) const
{
    return false;
}


bool StreamMidiScheduler::impl_portWriteable(int /*port*/) const
{
    return true;
}


void StreamMidiScheduler::impl_tx(MidiCommand mc)
{
    out << "[StreamMidiScheduler::tx]       ";
    outMidiCommand(mc);
    out << "\n";
}


void StreamMidiScheduler::impl_start(Clock start)
{
    out << "[StreamMidiScheduler::start]    ";
    clockStarted(start);
    outClock(start);
    clock = start;
    out << "\n";
}


void StreamMidiScheduler::impl_stop(Clock stopTime)
{
    out << "[StreamMidiScheduler::stop]     ";
    outClock(stopTime);
    out << "\n";
    clockStopped(stopTime);
}


void StreamMidiScheduler::impl_moveTo(Clock moveTime, Clock newTime)
{
    out << "[StreamMidiScheduler::moveTo]   ";
    outClock(newTime);
    out << "\n";
    clockMoved(moveTime, newTime);
}


Clock StreamMidiScheduler::impl_clock()
{
    clock += Clock::PPQN;
    return clock;
}


int StreamMidiScheduler::impl_msecs()
{
    // XXX
    return 0;
}


void StreamMidiScheduler::impl_setTempo(int newTempo, Clock changeTime)
{
    out << "[StreamMidiScheduler::setTempo] ";
    outClock(changeTime);
    out << " - " << newTempo << "\n";
    tempoChanged(newTempo, changeTime);
}


bool StreamMidiScheduler::impl_eventWaiting()
{
    return false;
}


MidiEvent StreamMidiScheduler::impl_rx()
{
    return MidiEvent();
}


void StreamMidiScheduler::impl_tx(MidiEvent e)
{
    if (e.data.status == MidiCommand_Invalid) return;
    out << "[StreamMidiScheduler::tx]       ";
    outClock(e.time);
    out << " - ";
    outMidiCommand(e.data);
    out << "\n";
}


void StreamMidiScheduler::impl_txSysEx(int,
                                       const unsigned char *, size_t size)
{
    out << "[StreamMidiScheduler::txSysEx]  " << size << " bytes\n";
}


void StreamMidiScheduler::outClock(Clock c)
{
    const int WIDTH = Clock::PPQN < 10 ? 1 : (Clock::PPQN < 100 ? 2 : 3);
    out << std::setfill(' ') << std::setw(4) << c / Clock::PPQN
        << "."
        << std::setfill('0') << std::setw(WIDTH) << c % Clock::PPQN;
}


void StreamMidiScheduler::outMidiCommand(MidiCommand mc)
{
    out << std::hex;
    switch (mc.status)
    {
        case MidiCommand_Invalid:         out << "[Invalid]......."; break;
        case MidiCommand_TSE_Meta:        out << "[TSE Meta]......"; break;
        case MidiCommand_NoteOn:          out << "Note On........."; break;
        case MidiCommand_NoteOff:         out << "Note Off........"; break;
        case MidiCommand_KeyPressure:     out << "Key Pressure...."; break;
        case MidiCommand_ControlChange:   out << "Control Change.."; break;
        case MidiCommand_ProgramChange:   out << "Program Change.."; break;
        case MidiCommand_ChannelPressure: out << "Channel Pressure"; break;
        case MidiCommand_PitchBend:       out << "Pitch Bend......"; break;
        case MidiCommand_System:          out << "System.........."; break;
    }
    out << " c:" << mc.channel
        << " p:"  << mc.port
        << " d1:" << std::setw(2) << mc.data1;
    if (MidiCommand_NoDataBytes[mc.status] == 2)
        out << " d2:" << std::setw(2) << mc.data2;
    switch (mc.status)
    {
        case MidiCommand_NoteOn:
        case MidiCommand_NoteOff:
        case MidiCommand_KeyPressure:
            {
                std::string note = TSE3::Util::numberToNote(mc.data1);
                out << "  (" << note << ")";
            }
            break;
    }
    out << std::dec;
}


/******************************************************************************
 * NullMidiScheduler class
 *****************************************************************************/

NullMidiScheduler::NullMidiScheduler()
{
    addPort(0, 0);
}


NullMidiScheduler::~NullMidiScheduler()
{
}


OIL::StrUtils_Return_Type NullMidiScheduler::impl_implementationName() const
{
    return "NullMidiScheduler version 0.00 [dev].";
}


OIL::StrUtils_Return_Type NullMidiScheduler::impl_portName(int /*port*/) const
{
    return "NullMidiScheduler port";
}


OIL::StrUtils_Return_Type NullMidiScheduler::impl_portType(int /*port*/) const
{
    return "NullMidiScheduler port";
}


bool NullMidiScheduler::impl_portReadable(int /*port*/) const
{
    return false;
}


bool NullMidiScheduler::impl_portWriteable(int /*port*/) const
{
    return true;
}


void NullMidiScheduler::impl_tx(MidiCommand /*c*/)
{
}


void NullMidiScheduler::impl_start(Clock start)
{
    clock = start;
    clockStarted(start);
}


void NullMidiScheduler::impl_stop(Clock stopTime)
{
    clockStopped(stopTime);
}


void NullMidiScheduler::impl_moveTo(Clock moveTime, Clock newTime)
{
    clock = newTime;
    clockMoved(moveTime, newTime);
}


Clock NullMidiScheduler::impl_clock()
{
    return clock;
}


int NullMidiScheduler::impl_msecs()
{
    return 0;
}


void NullMidiScheduler::impl_setTempo(int newTempo, Clock changeTime)
{
    tempoChanged(newTempo, changeTime);
}


bool NullMidiScheduler::impl_eventWaiting()
{
    return false;
}


MidiEvent NullMidiScheduler::impl_rx()
{
    return MidiEvent();
}


void NullMidiScheduler::impl_tx(MidiEvent /*e*/)
{
}


void NullMidiScheduler::impl_txSysEx(int /*port*/,
                                     const unsigned char * /*data*/,
                                     size_t /*size*/)
{
}

