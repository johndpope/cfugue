/*
 * @(#)MidiScheduler.cpp 3.00 20 May 1999
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

// XXX mutex
// XXX no channel, same channel, all channels

#include "tse3/MidiScheduler.h"

using namespace TSE3;

namespace
{
#if defined UNICODE || defined _UNICODE
    const wchar_t *INVALID_PORT_STRING = L"<Invalid port>";
#else 
    const char *INVALID_PORT_STRING = "<Invalid port>";
#endif
}


/******************************************************************************
 * MidiScheduler class
 *****************************************************************************/

MidiScheduler::MidiScheduler()
: _running(false), _tempo(120), _restingClock(0),
  _remote(false), _consumeRemote(true),
  _startNote(24 /*C2*/), _stopNote(26 /*D2*/),
  _defaultInternal(MidiCommand::NoPort), _defaultExternal(MidiCommand::NoPort)
{
//    this->impl_setTempo(_tempo, 0);
}


MidiScheduler::~MidiScheduler()
{
}


bool MidiScheduler::validPort(int port) const
{
    return lookUpPortNumber(port);
}


size_t MidiScheduler::numberToIndex(int port) const
{
    if (lookUpPortNumber(port))
    {
        return port;
    }
    else
    {
        return 0;
    }
}


void MidiScheduler::portNumbers(std::vector<int> &numbers) const
{
    numbers.clear();
    port_vector::const_iterator i = _portNumbers.begin();
    while (i != _portNumbers.end())
    {
        numbers.push_back(i->first);
        ++i;
    }
}


OIL::StrUtils_Return_Type MidiScheduler::portName(int port) const
{
    if (lookUpPortNumber(port))
    {
        return impl_portName(port);
    }
    else
    {
        return INVALID_PORT_STRING;
    }
}


OIL::StrUtils_Return_Type MidiScheduler::portType(int port) const
{
    if (lookUpPortNumber(port))
    {
        return impl_portType(port);
    }
    else
    {
        return INVALID_PORT_STRING;
    }
}


bool MidiScheduler::portReadable(int port) const
{
    if (lookUpPortNumber(port))
    {
        return impl_portReadable(port);
    }
    else
    {
        return false;
    }
}
bool MidiScheduler::portWriteable(int port) const
{
    if (lookUpPortNumber(port))
    {
        return impl_portWriteable(port);
    }
    else
    {
        return false;
    }
}


bool MidiScheduler::portInternal(int port) const
{
    if (lookUpPortNumber(port))
    {
        return _portNumbers[port].second.isInternal;
    }
    else
    {
        return false;
    }
}


int MidiScheduler::defaultInternalPort() const
{
    return _defaultInternal;
}

int MidiScheduler::defaultExternalPort() const
{
    return _defaultExternal;
}


void MidiScheduler::stop(Clock stopTime)
{
    if (stopTime == -1)
    {
        _restingClock = impl_clock();
    }
    else
    {
        _restingClock = stopTime;
    }
    impl_stop(_restingClock);
}


void MidiScheduler::moveTo(Clock moveTime, Clock newTime)
{
    if (_running)
    {
        impl_moveTo(moveTime, newTime);
    }
    else
    {
        _restingClock = newTime;
        notify(&MidiSchedulerListener::MidiScheduler_Moved);
    }
}


MidiEvent MidiScheduler::rx()
{
    MidiEvent e = doRemoteControl(impl_rx());
    setToPortNumber(e.data);
    return e;
}


void MidiScheduler::tx(MidiCommand mc)
{
    if (mc.port != MidiCommand::AllPorts)
    {
        if (lookUpPortNumber(mc) && validChannel(mc))
        {
            impl_tx(mc);
        }
    }
    else
    {
        for (size_t port = 0; port < _portNumbers.size(); ++port)
        {
            mc.port = _portNumbers[port].second.index;
            impl_tx(mc);
        }
    }
}


void MidiScheduler::tx(MidiEvent me)
{
    if (me.data.port != MidiCommand::AllPorts)
    {
        if (lookUpPortNumber(me.data) && validChannel(me.data))
        {
            impl_tx(me);
        }
    }
    else
    {
        for (size_t port = 0; port < _portNumbers.size(); ++port)
        {
            me.data.port = _portNumbers[port].second.index;
            impl_tx(me);
        }
    }
}


void MidiScheduler::txSysEx(int port, const unsigned char *data, size_t size)
{
    if (port != MidiCommand::AllPorts)
    {
        if (lookUpPortNumber(port))
        {
            impl_txSysEx(port, data, size);
        }
    }
    else if (port != MidiCommand::NoPort)
    {
        for (size_t port = 0; port < _portNumbers.size(); ++port)
        {
            impl_txSysEx(_portNumbers[port].second.index, data, size);
        }
    }
    /*
    MidiCommand mc(MidiCommand_System, MidiSystem_SysExStart, port, *data);
    if (lookUpPortNumber(mc))
    {
        impl_tx(mc);
        impl_txBytes(mc.port, data+1, size-1);
        mc.channel = MidiSystem_SysExEnd;
        impl_tx(mc);
    }
    */
}


MidiEvent MidiScheduler::doRemoteControl(MidiEvent e)
{
    // TODO Handle MIDI start and stop and song pos messages
    if (_remote)
    {
        if (e.data.status == MidiCommand_NoteOn && e.data.data1 == _startNote)
        {
            impl_start(_restingClock);
            if (_consumeRemote) e = MidiEvent();
        }
        else if (e.data.status == MidiCommand_NoteOn
                 && e.data.data1 == _stopNote)
        {
            impl_stop(-1);
            if (_consumeRemote) e = MidiEvent();
        }
        else if (e.data.status == MidiCommand_NoteOff
                 && (e.data.data1 == _startNote || e.data.data1 == _stopNote))
        {
            if (_consumeRemote) e = MidiEvent();
        }
    }
    return e;
}


bool MidiScheduler::lookUpPortNumber(MidiCommand &mc) const
{
    port_vector::const_iterator i = _portNumbers.begin();
    while (i != _portNumbers.end())
    {
        if (i->first == mc.port)
        {
            mc.port = i->second.index;
            return true;
        }
        ++i;
    }
    return false;
}


bool MidiScheduler::lookUpPortNumber(int &port) const
{
    port_vector::const_iterator i = _portNumbers.begin();
    while (i != _portNumbers.end())
    {
        if (i->first == port)
        {
            port = i->second.index;
            return true;
        }
        ++i;
    }
    return false;
}


void MidiScheduler::setToPortNumber(MidiCommand &mc) const
{
    port_vector::const_iterator i = _portNumbers.begin();
    while (i != _portNumbers.end())
    {
        if (i->second.index == mc.port)
        {
            mc.port = i->first;
            return;
        }
        ++i;
    }
    // ERROR
}


int MidiScheduler::addPort(int portIndex, bool isInternal, int requestedPort)
{
    int newPort = MidiCommand::NoPort;
    if (requestedPort < 0)
    {
        requestedPort = 0;
    }

    while (newPort == MidiCommand::NoPort)
    {
        newPort = requestedPort;
        if (lookUpPortNumber(newPort))
        {
            newPort = MidiCommand::NoPort;
            ++requestedPort;
        }
    }

    _portNumbers.push_back(std::make_pair(newPort,
                           PortInfo(portIndex, isInternal)));
    if (isInternal && _defaultInternal == MidiCommand::NoPort)
    {
        _defaultInternal = newPort;
    }
    if (!isInternal && _defaultExternal == MidiCommand::NoPort)
    {
        _defaultExternal = newPort;
    }
    notify(&MidiSchedulerListener::MidiScheduler_PortAdded, newPort);

    return newPort;
}


void MidiScheduler::removePort(int portIndex)
{
    port_vector::iterator i = _portNumbers.begin();
    if (portIndex == _defaultInternal)
    {
        port_vector::const_iterator i = _portNumbers.begin();
        while (i != _portNumbers.end() && !i->second.isInternal) { ++i; }
        _defaultInternal = i == _portNumbers.end()
                         ? MidiCommand::NoPort
                         : i->first;
    }
    if (portIndex == _defaultExternal)
    {
        port_vector::const_iterator i = _portNumbers.begin();
        while (i != _portNumbers.end() && i->second.isInternal) { ++i; }
        _defaultExternal = i == _portNumbers.end()
                         ? MidiCommand::NoPort
                         : i->first;
    }
    while (i != _portNumbers.end())
    {
        if (i->second.index == portIndex)
        {
            size_t portNum = i->first;
            _portNumbers.erase(i);
            notify(&MidiSchedulerListener::MidiScheduler_PortRemoved, portNum);
            return;
        }
        ++i;
    }
    // Sigh. Passed an invalid index.
}


void MidiScheduler::clockStarted(Clock startTime)
{
    _running = true;
    startClock = startTime;
    notify(&MidiSchedulerListener::MidiScheduler_Started);
}


void MidiScheduler::clockStopped(Clock stopTime)
{
    _running      = false;
    _restingClock = stopTime;
    notify(&MidiSchedulerListener::MidiScheduler_Stopped);
}


void MidiScheduler::clockMoved(const Clock moveTime, const Clock newTime)
{
    startClock -= moveTime - newTime;
    notify(&MidiSchedulerListener::MidiScheduler_Moved);
}


void MidiScheduler::tempoChanged(int newTempo, Clock changeTime)
{
    /*
     * NOTE: startClock holds the scheduler clock at hardware time '0' -
     * by altering the timeline by changing tempo, we have to work out
     * a new value for startClock to make subseequent time calculations
     * consistent.
     */

    _tempo = newTempo;

    startClock = changeTime
               - Util::muldiv(newTempo, changeTime-startClock, tempo());
    notify(&MidiSchedulerListener::MidiScheduler_TempoChanged);

    // XXX send event?
}

