/*
 * @(#)Arts.cpp 3.00 12 October 2000
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

#include "tse3/plt/Arts.h"

#include "tse3/Error.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// The prefix for this include is now worked out by configure script
#include <artsmidi.h>

#include <vector>
#include <utility>
#include <iostream>

using namespace TSE3;
using namespace TSE3::Plt;

namespace
{
    /**
     * this is a singleton shared by all aRts midi schedulers (if there might
     * be many?) to prevent the MCOP protocol from being initialized more than
     * once
     */
    class ArtsCommon
    {
        public:
            Arts::Dispatcher dispatcher;
            Arts::MidiManager manager;
            Arts::MidiClient client;
            Arts::MidiPort port;
            int offsetSec;

            bool alive;

            ArtsCommon()
            {
                alive = false;
                manager = Arts::Reference("global:Arts_MidiManager");

                if (manager.isNull())
                {
                    std::cerr << "TSE3: Arts MidiManager isNull\n";
                    throw TSE3::MidiSchedulerError(MidiSchedulerCreateErr);
                }

                std::string title = "TSE3";
                client            = manager.addClient(Arts::mcdPlay,
                                                      Arts::mctApplication,
                                                      title,"tse3");
                port              = client.addOutputPort();
                offsetSec         = port.time().sec;

                alive             = true;
            }
    };

    ArtsCommon *artsCommon     = 0;
    int         artsCommonInit = 0;
}


ArtsMidiScheduler::ArtsMidiScheduler()
{
    if(!artsCommonInit) { assert(!artsCommon); artsCommon = new ArtsCommon(); }
    artsCommonInit++;

    addPort(1, true); // XXX is internal?
}


ArtsMidiScheduler::~ArtsMidiScheduler()
{
    artsCommonInit--;
    if(!artsCommonInit) { delete artsCommon; artsCommon = 0; }
}


const char *ArtsMidiScheduler::impl_implementationName() const
{
    return "ArtsMidiScheduler version 1.00";
}


const char *ArtsMidiScheduler::impl_portName(int /*port*/) const
{
    return "Single aRts MIDI output port";
}


const char *ArtsMidiScheduler::impl_portType(int /*port*/) const
{
    return "aRts MIDI port";
}


bool ArtsMidiScheduler::impl_portReadable(int /*port*/) const
{
    return false;
}


bool ArtsMidiScheduler::impl_portWriteable(int /*port*/) const
{
    return true;
}


void ArtsMidiScheduler::impl_tx(MidiCommand mc)
{
    if(!artsCommon->alive) return;

    Arts::MidiCommand command(mc.status << 4 | mc.channel, mc.data1, mc.data2);
    artsCommon->port.processCommand(command);
}


void ArtsMidiScheduler::impl_start(Clock startTime)
{
    clockStarted(startTime);
}


void ArtsMidiScheduler::impl_stop(Clock stopTime)
{
    clockStopped(stopTime);
}


void ArtsMidiScheduler::impl_moveTo(Clock moveTime, Clock newTime)
{
    clockMoved(moveTime, newTime);
}


Clock ArtsMidiScheduler::impl_clock()
{
    return msToClock(impl_msecs());
}


int ArtsMidiScheduler::impl_msecs()
{
    Arts::TimeStamp time = artsCommon->port.time();
    time.sec -= artsCommon->offsetSec; /* to reduce the risk of overflows */

    return time.sec*1000 + time.usec/1000;
}


void ArtsMidiScheduler::impl_setTempo(int newTempo, Clock changeTime)
{
    tempoChanged(newTempo, changeTime);
}


bool ArtsMidiScheduler::impl_eventWaiting()
{
    return false;
}


MidiEvent ArtsMidiScheduler::impl_rx()
{
    return MidiEvent();
}


void ArtsMidiScheduler::impl_txSysEx(int /*port*/,
                                     const unsigned char * /*data*/,
                                     size_t /*size*/)
{
}


void ArtsMidiScheduler::impl_tx(MidiEvent e)
{
    if(!artsCommon->alive) return;

    Arts::TimeStamp time(clockToMs(e.time)/1000, (clockToMs(e.time)%1000)*1000);
    MidiCommand& mc = e.data;
    Arts::MidiCommand command(mc.status << 4 | mc.channel, mc.data1, mc.data2);
    artsCommon->port.processEvent(Arts::MidiEvent(time,command));
}

