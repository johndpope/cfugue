/*
 * @(#)MidiEcho.cpp 3.00 23 February 2000
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

#include "tse3/MidiEcho.h"

#include "tse3/MidiScheduler.h"

using namespace TSE3;

/******************************************************************************
 * MidiEcho class
 *****************************************************************************/

MidiEcho::MidiEcho(MidiScheduler *s)
: scheduler(s), _channel(MidiCommand::AllChannels), _port(MidiCommand::AllPorts)
{
    _filter.setStatus(true);
}


void MidiEcho::setChannel(int c)
{
    if (c != MidiCommand::AllChannels && (c < 0 || c > 15)) return;
    _channel = c;
    notify(&MidiEchoListener::MidiEcho_Altered,
           MidiEchoListener::ChannelChanged);
}


void MidiEcho::setPort(int p)
{
    _port = p;
    notify(&MidiEchoListener::MidiEcho_Altered, MidiEchoListener::PortChanged);
}


void MidiEcho::echo(MidiEvent event)
{
    if ((_channel == MidiCommand::AllChannels
         || event.data.channel == _channel)
        && (_port == MidiCommand::AllPorts
            || event.data.port == _port))
    {
        event = _filter.filter(event);
        scheduler->tx(event.data);
    }
}

