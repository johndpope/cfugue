/*
 * @(#)MidiFilter.cpp 3.00 30 August 2000
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

#include "tse3/MidiFilter.h"

#include "tse3/FileBlockParser.h"
#include "tse3/Mutex.h"

using namespace TSE3;

/******************************************************************************
 * Compile time assertion that unsigned int is at least 16 bits.
 *****************************************************************************/

namespace
{
    template <bool expression>
    class compile_assertion;
    template <>
    class compile_assertion<true> {};

    // If this line generates an error then your ints aren't big enough
    compile_assertion<sizeof(unsigned int)*8 >= 16> uint_size_test;
}

/******************************************************************************
 * MidiFilter class
 *****************************************************************************/

MidiFilter::MidiFilter()
: _status(true),
  _channelFilter(0xffff),
  _channel(MidiCommand::SameChannel), _port(MidiCommand::SamePort),
  _offset(0), _timeScale(100), _quantise(0),
  _minLength(0), _maxLength(-1),
  _transpose(0),
  _minVelocity(0), _maxVelocity(127), _velocityScale(100)
{
}


MidiFilter:: MidiFilter(const MidiFilter &s)
: Filter(), Serializable(), Notifier<MidiFilterListener>(),
  _status(s._status),
  _channelFilter(s._channelFilter),
  _channel(s._channel), _port(s._port),
  _offset(s._offset), _timeScale(s._timeScale), _quantise(s._quantise),
  _minLength(s._minLength), _maxLength(s._maxLength),
  _transpose(s._transpose),
  _minVelocity(s._minVelocity), _maxVelocity(s._maxVelocity),
  _velocityScale(s._velocityScale)
{
}


MidiFilter &MidiFilter::operator=(const MidiFilter &s)
{
    Impl::CritSec cs;

    _status        = s._status;
    _channelFilter = s._channelFilter;
    _channel       = s._channel;
    _port          = s._port;
    _offset        = s._offset;
    _timeScale     = s._timeScale;
    _quantise      = s._quantise;
    _minLength     = s._minLength;
    _maxLength     = s._maxLength;
    _transpose     = s._transpose;
    _minVelocity   = s._minVelocity;
    _maxVelocity   = s._maxVelocity;
    _velocityScale = s._velocityScale;

    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::StatusChanged);
    return *this;
}


MidiFilter::~MidiFilter()
{
}


void MidiFilter::setStatus(bool s)
{
    Impl::CritSec cs;

    _status = s;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::StatusChanged);
}


void MidiFilter::setChannelFilter(int c, bool val)
{
    Impl::CritSec cs;

    if (c < 0 || c > 15) return;
    _channelFilter &= ~(1<<c);
    if (val) _channelFilter |= 1<<c;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::ChannelFilterChanged);
}


void MidiFilter::setChannel(int c)
{
    Impl::CritSec cs;

    _channel = c;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::ChannelChanged);
}


void MidiFilter::setPort(int p)
{
    Impl::CritSec cs;

    _port = p;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::PortChanged);
}


void MidiFilter::setOffset(Clock o)
{
    Impl::CritSec cs;

    _offset = o;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::OffsetChanged);
}

void MidiFilter::setTimeScale(int t)
{
    Impl::CritSec cs;

    if (t >= 1 && t <= 500) _timeScale = t;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::TimeScaleChanged);
}


void MidiFilter::setQuantise(Clock q)
{
    Impl::CritSec cs;

    if (q >= 0) _quantise = q;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::QuantiseChanged);
}


void MidiFilter::setMinLength(Clock m)
{
    Impl::CritSec cs;

    if (m >= 0) _minLength = m;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::MinLengthChanged);
}


void MidiFilter::setMaxLength(Clock m)
{
    Impl::CritSec cs;

    if (m >=-10) _maxLength = m;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::MaxLengthChanged);
}


void MidiFilter::setTranspose(int t)
{
    Impl::CritSec cs;

    if (t >= -127 && t <= 127) _transpose = t;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::TransposeChanged);
}


void MidiFilter::setMinVelocity(int v)
{
    Impl::CritSec cs;

    if (v >= 0 && v <= 127) _minVelocity = v;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::MinVelocityChanged);
}


void MidiFilter::setMaxVelocity(int v)
{
    Impl::CritSec cs;

    if (v >= 0 && v <= 127) _maxVelocity = v;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::MaxVelocityChanged);
}


void MidiFilter::setVelocityScale(int v)
{
    Impl::CritSec cs;

    if (v >= 1 && v <= 200) _velocityScale = v;
    notify(&MidiFilterListener::MidiFilter_Altered,
           MidiFilterListener::VelocityScaleChanged);
}


MidiEvent MidiFilter::filter(const MidiEvent &event) const
{
    Impl::CritSec cs;

    if (!_status
        || !(_channelFilter & (1<<event.data.channel)))
    {
        return MidiEvent();
    }

    MidiEvent e = event;

    // ATM we're presuing only channel events come through this routine
    // It wouldn't be hard to check with MidiCommand::isChannel()

    // Channel
    if (_channel != MidiCommand::SameChannel)
    {
        e.data.channel    = _channel;
        e.offData.channel = _channel;
    }

    // Port
    if (_port != MidiCommand::SamePort)
    {
        e.data.port    = _port;
        e.offData.port = _port;
    }

    // Offset
    e.time -= _offset;
    if (e.data.status == MidiCommand_NoteOn)
    {
        e.offTime -= _offset;
    }

    // Time scale
    if (_timeScale != 100)
    {
        e.time *= _timeScale;
        e.time /= 100;
    }
    if (e.data.status == MidiCommand_NoteOn && _timeScale != 100)
    {
        e.offTime *= _timeScale;
        e.offTime /= 100;
    }

    // Quantise
    if (_quantise != 0)
    {
        e.time += _quantise/2;
        e.time /= _quantise;
        e.time *= _quantise;
        if (e.data.status == MidiCommand_NoteOn)
        {
            e.offTime += _quantise/2;
            e.offTime /= _quantise;
            e.offTime *= _quantise;
        }
    }

    if (e.data.status == MidiCommand_NoteOn
        || e.data.status == MidiCommand_KeyPressure)
    {
        // Transpose
        {
            int note = e.data.data1 + _transpose;
            if (note >= 0 && note <= 127)
            {
                e.data.data1    = note;
                e.offData.data1 = note;
            } else
            {
                e.data.status = MidiCommand_Invalid;
            }
        }

    }

    if (e.data.status == MidiCommand_NoteOn)
    {
        // Min length
        if (e.offTime - e.time < _minLength)
        {
            e.offTime = e.time + _minLength;
        }

        // Max Length
        if (_maxLength >= 0 && e.offTime - e.time > _maxLength)
        {
            e.offTime = e.time + _maxLength;
        }

        // Velocity things
        {
            int velocity = e.data.data2;

            if (_velocityScale != 100)
            {
                velocity *= _velocityScale;
                velocity /= 100;
            }
            if (velocity < _minVelocity) velocity = _minVelocity;
            if (velocity > _maxVelocity) velocity = _maxVelocity;

            e.data.data2 = velocity;
        }

    }

    return e;
}


/******************************************************************************
 * MidiFilter Serializable interface
 *****************************************************************************/

void MidiFilter::save(std::ostream &o, int i) const
{
    o << indent(i)   << "{\n";
    o << indent(i+1) << "Status:";
    if (_status) o << "On\n"; else o << "Off\n";
    o << indent(i+1) << "ChannelFilter:" << _channelFilter << "\n";
    o << indent(i+1) << "Channel:"       << _channel       << "\n";
    o << indent(i+1) << "Port:"          << _port          << "\n";
    o << indent(i+1) << "Offset:"        << _offset        << "\n";
    o << indent(i+1) << "TimeScale:"     << _timeScale     << "\n";
    o << indent(i+1) << "Quantise:"      << _quantise      << "\n";
    o << indent(i+1) << "Transpose:"     << _transpose     << "\n";
    o << indent(i+1) << "MinVelocity:"   << _minVelocity   << "\n";
    o << indent(i+1) << "MaxVelocity:"   << _maxVelocity   << "\n";
    o << indent(i+1) << "VelocityScale:" << _velocityScale << "\n";
    o << indent(i)   << "}\n";
}


void MidiFilter::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_OnOff<MidiFilter>
        status(this, &MidiFilter::setStatus);
    FileItemParser_Number<MidiFilter>
        channelFilter(this, &MidiFilter::setChannelFilter);
    FileItemParser_Number<MidiFilter>
        channel(this, &MidiFilter::setChannel);
    FileItemParser_Number<MidiFilter>
        port(this, &MidiFilter::setPort);
    FileItemParser_Clock<MidiFilter>
        offset(this, &MidiFilter::setOffset);
    FileItemParser_Number<MidiFilter>
        timeScale(this, &MidiFilter::setTimeScale);
    FileItemParser_Clock<MidiFilter>
        quantise(this, &MidiFilter::setQuantise);
    FileItemParser_Number<MidiFilter>
        minVel(this, &MidiFilter::setMinVelocity);
    FileItemParser_Number<MidiFilter>
        maxVel(this, &MidiFilter::setMaxVelocity);
    FileItemParser_Number<MidiFilter>
        velScale(this, &MidiFilter::setVelocityScale);
    FileBlockParser parser;
    parser.add("Status",        &status);
    parser.add("ChannelFilter", &channelFilter);
    parser.add("Channel",       &channel);
    parser.add("Port",          &port);
    parser.add("Offset",        &offset);
    parser.add("TimeScale",     &timeScale);
    parser.add("Quantise",      &quantise);
    parser.add("MinVelocity",   &minVel);
    parser.add("MaxVelocity",   &maxVel);
    parser.add("VelocityScale", &velScale);
    parser.parse(in, info);
}
