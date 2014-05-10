/*
 * @(#)Mixer.cpp 3.00 31 March 2000
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

#include "tse3/Mixer.h"

#include "tse3/Transport.h"

using namespace TSE3;

/******************************************************************************
 * MixerChannel class
 *****************************************************************************/

MixerChannel::MixerChannel(MixerPort *mixerPort, unsigned int channel)
: mixerPort(mixerPort), channel(channel),
  _volume(127), _pan(0x40), _chorus(0), _reverb(0x28),
  _program(0), _bankLSB(0), _bankMSB(0)
{
}


MixerChannel::~MixerChannel()
{
}


void MixerChannel::setVolume(unsigned int v, bool send)
{
    if (v <= 127)
    {
        _volume = v;
        if (send)
        {
            mixerPort->txCommand(
                MidiCommand(MidiCommand_ControlChange, channel, 0,
                            MidiControl_ChannelVolumeMSB, _volume));
        }
        notify(&MixerChannelListener::MixerChannel_Volume);
    }
}


void MixerChannel::setPan(unsigned int p, bool send)
{
    if (p <= 127)
    {
        _pan = p;
        if (send)
        {
            mixerPort->txCommand(
                MidiCommand(MidiCommand_ControlChange, channel, 0,
                            MidiControl_PanMSB, _pan));
        }
        notify(&MixerChannelListener::MixerChannel_Pan);
    }
}


void MixerChannel::setChorus(unsigned int c, bool send)
{
    if (c <= 127)
    {
        _chorus = c;
        if (send)
        {
            mixerPort->txCommand(
                MidiCommand(MidiCommand_ControlChange, channel, 0,
                            MidiControl_ChorusDepth, _chorus));
        }
        notify(&MixerChannelListener::MixerChannel_Chorus);
    }
}


void MixerChannel::setReverb(unsigned int r, bool send)
{
    if (r <= 127)
    {
        _reverb = r;
        if (send)
        {
            mixerPort->txCommand(
                MidiCommand(MidiCommand_ControlChange, channel, 0,
                            MidiControl_ReverbDepth, _reverb));
        }
        notify(&MixerChannelListener::MixerChannel_Reverb);
    }
}


void MixerChannel::setProgram(unsigned int p, bool send)
{
    if (p <= 127)
    {
        _program = p;
        if (send)
        {
            mixerPort->txCommand(
                MidiCommand(MidiCommand_ProgramChange, channel, 0, _program));
        }
        notify(&MixerChannelListener::MixerChannel_Program);
    }
}


void MixerChannel::setBankLSB(unsigned int b, bool send)
{
    if (b <= 127)
    {
        _bankLSB = b;
        if (send)
        {
            mixerPort->txCommand(
                MidiCommand(MidiCommand_ControlChange, channel, 0,
                            MidiControl_BankSelectLSB, _bankLSB));
        }
        notify(&MixerChannelListener::MixerChannel_BankLSB);
    }
}


void MixerChannel::setBankMSB(unsigned int b, bool send)
{
    if ( b <= 127)
    {
        _bankMSB = b;
        if (send)
        {
            mixerPort->txCommand(
                MidiCommand(MidiCommand_ControlChange, channel, 0,
                            MidiControl_BankSelectMSB, _bankMSB));
        }
        notify(&MixerChannelListener::MixerChannel_BankMSB);
    }
}


void MixerChannel::command(MidiCommand mc)
{
    switch (mc.status)
    {
        case MidiCommand_ProgramChange:
            setProgram(mc.data1, false);
            break;
        case MidiCommand_ControlChange:
            switch (mc.data1)
            {
                case MidiControl_ChannelVolumeMSB:
                    setVolume(mc.data2, false);
                    break;
                case MidiControl_PanMSB:
                    setPan(mc.data2, false);
                    break;
                case MidiControl_ReverbDepth:
                    setReverb(mc.data2, false);
                    break;
                case MidiControl_ChorusDepth:
                    setChorus(mc.data2, false);
                    break;
                case MidiControl_BankSelectMSB:
                    setBankMSB(mc.data2, false);
                    break;
                case MidiControl_BankSelectLSB:
                    setBankLSB(mc.data2, false);
                    break;
            }
            break;
    }
}


/******************************************************************************
 * MixerPort class
 *****************************************************************************/

MixerPort::MixerPort(Mixer *mixer, unsigned int port)
: mixer(mixer), port(port), _volume(127)
{
    for (size_t n = 0; n < 16; ++n)
    {
        mixerChannels[n] = new MixerChannel(this, n);
    }
}


MixerPort::~MixerPort()
{
    for (size_t n = 0; n < 16; ++n)
    {
        delete mixerChannels[n];
    }
}


void MixerPort::command(MidiCommand mc)
{
    // XXX volume?
    mixerChannels[mc.channel]->command(mc);
}


void MixerPort::txCommand(MidiCommand mc)
{
    mc.port = port;
    mixer->txCommand(mc);
}


/******************************************************************************
 * Mixer class
 *****************************************************************************/

Mixer::Mixer(size_t noPorts, Transport *transport)
: noPorts(noPorts), transport(transport),
  _updateWithInput(true), _updateWithOutput(true)
{
    mixerPorts = new MixerPort*[noPorts];
    for (size_t n = 0; n < noPorts; ++n)
    {
        mixerPorts[n] = new MixerPort(this, n);
    }
    if (transport)
    {
        transport->attachCallback(this);
        attachTo(transport);
    }
}


Mixer::~Mixer()
{
    if (transport)
    {
        transport->detachCallback(this);
    }
    for (size_t n = 0; n < noPorts; ++n)
    {
        delete mixerPorts[n];
    }
    delete mixerPorts;
}


void Mixer::command(MidiCommand mc)
{
    if ((size_t)mc.port < noPorts)
    {
        mixerPorts[mc.port]->command(mc);
    }
}


void Mixer::Notifier_Deleted(Transport *)
{
    transport = 0;
}


void Mixer::txCommand(MidiCommand mc)
{
    if (transport) transport->inject(mc);
}


void Mixer::Transport_MidiIn(MidiCommand mc)
{
    if (_updateWithInput) command(mc);
}


void Mixer::Transport_MidiOut(MidiCommand mc)
{
    if (_updateWithOutput) command(mc);
}

