/*
 * @(#)Alsa-0.9.cpp 3.00 28 March 2002
 *
 * Copyright (c) 2002 Pete Goodliffe (pete@cthree.org)
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

#include "tse3/plt/Alsa.h"

#include "tse3/Error.h"
#include "tse3/util/MulDiv.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define HAVE_SYS_ASOUNDLIB_H
#endif

#define _GNU_SOURCE
#include <sys/stat.h>
#include <errno.h>
#include <alloca.h>
#if HAVE_ALSA_ASOUNDLIB_H
#include <sys/asoundlib.h>
#elif HAVE_SYS_ASOUNDLIB_H
#include <sys/asoundlib.h>
#endif

#include <vector>
#include <utility>
#include <cstdio>

using namespace TSE3;
using namespace TSE3::Plt;

namespace
{
    // Temporary data areas where we store return values
    char tmpPortNameBuffer[64 + 20]; // + 20 because I add " client:port"
}

/******************************************************************************
 * AlsaImpl class
 *****************************************************************************/

/**
 * Note that this class is a private implementation class, but it is not self
 * contained, the Alsa driving logic is split between this and the
 * AlsaMidiScheduler class. This class does all the good initialisation,
 * though.
 *
 * @short   Alsa MidiScheduler private implementation
 * @author  Pete Goodliffe
 * @version 0.00
 * @see     AlsaMidiScheduler
 */
class TSE3::Plt::AlsaImpl
{
    public:

        /*
         * Internal Alsa implementation state data
         */

        snd_seq_t             *handle;      // our handle onto Alsa
        snd_seq_client_info_t *client_info; // info about ourself
        snd_seq_port_info_t   *port_info;   // info about our port
        int my_port;
        int                    queue;       // the id of the queue we create

        // Alsa destinations are client/port pairs. Whilst the MidiScheduler
        // API supports ports numbered 0-X, in Alsa these numbers are
        // non-contiguous. Therefore this vector holds a lookup table for
        // destination info. It is a pair containing <client, port>.
        typedef std::pair<unsigned char, unsigned char> dest_type;
        std::vector<dest_type> dest;

        std::vector<unsigned char>               running;
        std::vector<std::vector<unsigned char> > sysex; // sysex buffers

        /*
         * Private APIs
         */

        AlsaImpl();
        ~AlsaImpl();

        /**
         * Sends the MidiCommand. If no queue is specified, it is sent
         * immediately, bypassing the queue.
         *
         * In this version of the driver we only support timing in msecs.
         * Later on we'll hook into Alsa's ticks and then be able to sync
         * to external timing sources.
         */
        void tx(MidiCommand         mc,
                int                 queue = SND_SEQ_QUEUE_DIRECT,
                long int            sec   = 0,
                long int            nsec  = 0);
};


TSE3::Plt::AlsaImpl::AlsaImpl()
: handle(0), client_info(0), port_info(0)
{

    // Check for ALSA support on machine
    struct stat buf;
    stat("/proc/asound", &buf);
    if ((stat("/proc/asound", &buf) != 0 ) || !(S_ISDIR(buf.st_mode)))
    {
        std::cerr << "TSE3: Alsa scheduler error.\n"
                  << "      Alsa is not running on this machine\n";
        throw TSE3::MidiSchedulerError(MidiSchedulerCreateErr);
    }

    int r = snd_seq_open(&handle, "hw", SND_SEQ_OPEN_DUPLEX, 0);
    if (r < 0)
    {
        std::cerr << "TSE3: Alsa scheduler error. Couldn't open sequencer\n"
                  << "      (" << snd_strerror(r) << ")\n";
        throw TSE3::MidiSchedulerError(MidiSchedulerCreateErr);
    }

    // Get info about ourself
    snd_seq_client_info_malloc(&client_info);
    r = snd_seq_get_client_info(handle, client_info);
    if (r < 0)
    {
        std::cerr << "TSE3: Alsa scheduler error. Couldn't get client info\n"
                  << "      (" << snd_strerror(r) << ")\n";
        throw TSE3::MidiSchedulerError(MidiSchedulerCreateErr);
    }

    // Create a port to connect to all output ports available
    snd_seq_port_info_malloc(&port_info);
    snd_seq_port_info_set_name(port_info, "TSE3");
    snd_seq_port_info_set_capability(port_info,
                     SND_SEQ_PORT_CAP_READ
                     | SND_SEQ_PORT_CAP_SUBS_READ
                     | SND_SEQ_PORT_CAP_WRITE
                     | SND_SEQ_PORT_CAP_SUBS_WRITE
                     | SND_SEQ_PORT_CAP_DUPLEX);
    snd_seq_port_info_set_type(port_info, SND_SEQ_PORT_TYPE_APPLICATION);
    r = snd_seq_create_port(handle, port_info);
    if (r < 0)
    {
        std::cerr << "TSE3: Alsa scheduler error. Couldn't create port\n"
                  << "      (" << snd_strerror(r) << ")\n";
        snd_seq_close(handle);
        throw TSE3::MidiSchedulerError(MidiSchedulerCreateErr);
    }
    my_port = snd_seq_port_info_get_port(port_info);

    // Create a queue to schedule data on
    queue = snd_seq_alloc_named_queue(handle, "TSE3 queue");
    if (queue < 0)
    {
        std::cerr << "TSE3: Alsa scheduler error. Couldn't allocate queue\n"
                  << "      (" << snd_strerror(r) << ")\n";
        snd_seq_close(handle);
        throw TSE3::MidiSchedulerError(MidiSchedulerCreateErr);
    }
}


TSE3::Plt::AlsaImpl::~AlsaImpl()
{
    if (handle)      snd_seq_close(handle);
    if (client_info) snd_seq_client_info_free(client_info);
    if (port_info)   snd_seq_port_info_free(port_info);
}


/*
 * This was in the AlsaImpl, an internal API. However, the need to all
 * addPort and removePort means that I have to make it a member of
 * the AlsaMidiScheduler
 */
void TSE3::Plt::AlsaMidiScheduler::getSystemInfo()
{
    // Remove all devices from the public API
    {
        int index = 0;
        std::vector<AlsaImpl::dest_type>::iterator i = pimpl->dest.begin();
        while (i != pimpl->dest.end())
        {
            removePort(index);
            ++index;
            ++i;
        }
    }

    pimpl->dest.clear();

    snd_seq_client_info_t *ci;
    snd_seq_client_info_alloca(&ci);
    snd_seq_client_info_set_client(ci, 0);
    while (snd_seq_query_next_client(pimpl->handle, ci) >= 0)
    {
        int c = snd_seq_client_info_get_client(ci);
        if (c == snd_seq_client_info_get_client(pimpl->client_info))
        {
            continue; // not ourselves
        }

        snd_seq_port_info_t *pi;
        snd_seq_port_info_alloca(&pi); // XXX free?
        snd_seq_port_info_set_client(pi, c);
        snd_seq_port_info_set_port(pi, -1);
        while (snd_seq_query_next_port(pimpl->handle, pi) >= 0)
        {
            int p = snd_seq_port_info_get_port(pi);
            unsigned int outcaps = SND_SEQ_PORT_CAP_SUBS_WRITE
                                 | SND_SEQ_PORT_CAP_WRITE;
            unsigned int incaps  = SND_SEQ_PORT_CAP_SUBS_READ
                                 | SND_SEQ_PORT_CAP_READ;
            // "&& pi.type" weeds out system devices
            if (snd_seq_port_info_get_capability(pi) & outcaps)
            {
                pimpl->dest.push_back(AlsaImpl::dest_type(c, p));
                snd_seq_connect_to(pimpl->handle, pimpl->my_port, c, p);
            }
            if (snd_seq_port_info_get_capability(pi) & incaps)
            {
                snd_seq_port_subscribe_t *subs;
                snd_seq_port_subscribe_alloca(&subs);
                snd_seq_port_subscribe_set_sender
                    (subs, snd_seq_port_info_get_addr(pi));
                snd_seq_port_subscribe_set_dest
                    (subs, snd_seq_port_info_get_addr(pimpl->port_info));
                snd_seq_port_subscribe_set_queue(subs, pimpl->queue);
                snd_seq_port_subscribe_set_time_update(subs, 1);
                int r3 = snd_seq_subscribe_port(pimpl->handle, subs);
                if (r3 < 0)
                {
                    std::cerr << "TSE3: Cannot subscribe to "
                              << c << ":" << p << "\n"
                              << "      (" << snd_strerror(r3) << ")\n";
                }
            }
        }
    }

    pimpl->running.clear();
    pimpl->running.insert(pimpl->running.begin(), pimpl->dest.size(), 0);

    pimpl->sysex.clear();
    pimpl->sysex.insert(pimpl->sysex.begin(), pimpl->dest.size(),
                               std::vector<unsigned char>());

    // Now let the public API know about these devices
    {
        int index = 0;
        std::vector<AlsaImpl::dest_type>::iterator i = pimpl->dest.begin();
        while (i != pimpl->dest.end())
        {
            addPort(index, i->first < 64, i->first); // XXX isInternal correct?
            ++index;
            ++i;
        }
    }
}


void TSE3::Plt::AlsaImpl::tx(MidiCommand mc, int queue,
                             long int sec, long int nsec)
{
    snd_seq_event_t ev;
    if (mc.port > (int)dest.size()-1)
    {
        mc.port = dest.size()-1;
    }
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_dest(&ev, dest[mc.port].first, dest[mc.port].second);
    snd_seq_ev_set_source(&ev, my_port);
    snd_seq_real_time_t time = { sec, nsec };
    snd_seq_ev_schedule_real(&ev, queue, 0 /*absolute*/, &time);
    switch (mc.status)
    {
        case MidiCommand_NoteOn:
        {
            if (mc.data2)
            {
                snd_seq_ev_set_noteon(&ev, mc.channel, mc.data1, mc.data2);
                break;
            }
            // Intentional fallthrough
        }
        case MidiCommand_NoteOff:
        {
            snd_seq_ev_set_noteoff(&ev, mc.channel, mc.data1, mc.data2);
            break;
        }
        case MidiCommand_KeyPressure:
        {
            snd_seq_ev_set_keypress(&ev, mc.channel, mc.data1, mc.data2);
            break;
        }
        case MidiCommand_ProgramChange:
        {
            snd_seq_ev_set_pgmchange(&ev, mc.channel, mc.data1);
            break;
        }
        case MidiCommand_ChannelPressure:
        {
            snd_seq_ev_set_chanpress(&ev, mc.channel, mc.data1);
            break;
        }
        case MidiCommand_PitchBend:
        {
            int pb = (mc.data1 << 7) | (mc.data2 & 0x7f);
            snd_seq_ev_set_pitchbend(&ev, mc.channel, pb);
            break;
        }
        case MidiCommand_ControlChange:
        {
            snd_seq_ev_set_controller(&ev, mc.channel, mc.data1, mc.data2);
            break;
        }
        case MidiCommand_System:
        {
            switch (mc.channel)
            {
                case MidiSystem_SysExStart:
                {
                    if (running[mc.port] != (mc.status<<4) + mc.channel)
                    {
                        sysex[mc.port].clear();
                        sysex[mc.port].push_back((mc.status<<4) + mc.channel);
                    }
                    sysex[mc.port].push_back(mc.data1);
                    break;
                }
                case MidiSystem_SysExEnd:
                {
                    sysex[mc.port].push_back((mc.status<<4) + mc.channel);
                    snd_seq_ev_set_sysex(&ev,
                                         sysex[mc.port].size(),
                                         &(sysex[mc.port][0]));
                    break;
                }
            }
            break;
        }
        default:
        {
            return;
        }
    }
    snd_seq_event_output(handle, &ev);
    snd_seq_drain_output(handle);

    running[mc.port] = (mc.status<<4) + mc.channel;
}


/******************************************************************************
 * AlsaMidiScheduler class (0.9 version)
 *****************************************************************************/

AlsaMidiScheduler::AlsaMidiScheduler()
: pimpl(new AlsaImpl())
{
    // Get system info
    getSystemInfo();
}


AlsaMidiScheduler::~AlsaMidiScheduler()
{
    // if playing, stop first!
    if (running()) stop(-1);

    delete pimpl;
}


const char *AlsaMidiScheduler::impl_implementationName() const
{
    return "AlsaMidiScheduler version [0.9] 1.02";
}


const char *AlsaMidiScheduler::impl_portName(int port) const
{
    if (port < (int)pimpl->dest.size())
    {
        snd_seq_port_info_t *port_info;
        snd_seq_port_info_alloca(&port_info);
        int r = snd_seq_get_any_port_info(pimpl->handle,
                                          pimpl->dest[port].first,
                                          pimpl->dest[port].second,
                                          port_info);
        if (r < 0)
        {
            std::cerr << "TSE3: Alsa scheduler error reading port name\n"
                      << "      (" << snd_strerror(r) << ")\n";
            return "TSE3: No port name";
        }
        std::sprintf(tmpPortNameBuffer, "%s %d:%d",
                     snd_seq_port_info_get_name(port_info),
                     pimpl->dest[port].first,
                     pimpl->dest[port].second);
        return tmpPortNameBuffer;
    }
    else
    {
        return "Invalid port";
    }
}


const char *AlsaMidiScheduler::impl_portType(int) const
{
    return "ALSA MIDI port";
}


bool AlsaMidiScheduler::impl_portReadable(int port) const
{
    if (port < (int)pimpl->dest.size())
    {
        snd_seq_port_info_t *port_info;
        snd_seq_port_info_alloca(&port_info);
        int r = snd_seq_get_any_port_info(pimpl->handle,
                                          pimpl->dest[port].first,
                                          pimpl->dest[port].second,
                                          port_info);
        if (r < 0)
        {
            std::cerr << "TSE3: Alsa scheduler error reading port readable\n"
                      << "      (" << snd_strerror(r) << ")\n";
            return false;
        }
        return snd_seq_port_info_get_capability(port_info)
               & SND_SEQ_PORT_CAP_READ;
    }
    return false;
}


bool AlsaMidiScheduler::impl_portWriteable(int port) const
{
    if (port < (int)pimpl->dest.size())
    {
        snd_seq_port_info_t *port_info;
        snd_seq_port_info_alloca(&port_info);
        int r = snd_seq_get_any_port_info(pimpl->handle,
                                          pimpl->dest[port].first,
                                          pimpl->dest[port].second,
                                          port_info);
        if (r < 0)
        {
            std::cerr << "TSE3: Alsa scheduler error reading port writeable\n"
                      << "      (" << snd_strerror(r) << ")\n";
            return false;
        }
        return snd_seq_port_info_get_capability(port_info)
               & SND_SEQ_PORT_CAP_WRITE;
    }
    return false;
}


void AlsaMidiScheduler::impl_tx(MidiCommand mc)
{
    pimpl->tx(mc);
}


void AlsaMidiScheduler::impl_start(const Clock s)
{
    if (!running())
    {
        startClock = s;
        snd_seq_queue_tempo_t *tempo;
        snd_seq_queue_tempo_alloca(&tempo);
        int r = snd_seq_get_queue_tempo(pimpl->handle, pimpl->queue, tempo);
        snd_seq_queue_tempo_set_tempo(tempo, 10);
        snd_seq_queue_tempo_set_ppq(tempo, Clock::PPQN);
        r = snd_seq_set_queue_tempo(pimpl->handle, pimpl->queue, tempo);

        // Start system clock
        snd_seq_event_t ev;
        ev.queue             = pimpl->queue;
        ev.dest.client       = SND_SEQ_CLIENT_SYSTEM;
        ev.dest.port         = SND_SEQ_PORT_SYSTEM_TIMER;
        ev.data.queue.queue  = pimpl->queue;
        ev.flags             = SND_SEQ_TIME_STAMP_REAL
                             | SND_SEQ_TIME_MODE_REL;
        ev.time.time.tv_sec  = 0;
        ev.time.time.tv_nsec = 0;
        ev.type              = SND_SEQ_EVENT_START;
        snd_seq_event_output(pimpl->handle, &ev);
        snd_seq_drain_output(pimpl->handle);

        r = snd_seq_start_queue(pimpl->handle, pimpl->queue, NULL);
        if (r < 0)
        {
            std::cerr << "TSE3: Alsa scheduler error starting queue\n"
                      << "      (" << snd_strerror(r) << ")\n";
        }

        clockStarted(s);
    }
}


void AlsaMidiScheduler::impl_stop(Clock t)
{
    if (!running()) return;

    int r = snd_seq_stop_queue(pimpl->handle, pimpl->queue, NULL);
    if (r < 0)
    {
        std::cerr << "TSE3: Alsa scheduler error stopping queue\n"
                  << "      (" << snd_strerror(r) << ")\n";
    }

    // Stop system clock
    snd_seq_event_t ev;
    ev.queue             = pimpl->queue;
    ev.dest.client       = SND_SEQ_CLIENT_SYSTEM;
    ev.dest.port         = SND_SEQ_PORT_SYSTEM_TIMER;
    ev.data.queue.queue  = pimpl->queue;
    ev.flags             = SND_SEQ_TIME_STAMP_REAL
                         | SND_SEQ_TIME_MODE_REL;
    ev.time.time.tv_sec  = 0;
    ev.time.time.tv_nsec = 0;
    ev.type              = SND_SEQ_EVENT_STOP;
    snd_seq_event_output(pimpl->handle, &ev);
    snd_seq_drain_output(pimpl->handle);

    clockStopped(t);
}


void AlsaMidiScheduler::impl_moveTo(const Clock moveTime, const Clock newTime)
{
    clockMoved(moveTime, newTime);
}


Clock AlsaMidiScheduler::impl_clock()
{
    snd_seq_queue_status_t *status;
    snd_seq_queue_status_alloca(&status);
    int r;

    r = snd_seq_get_queue_status(pimpl->handle, pimpl->queue, status);
    if (r < 0)
    {
        std::cerr << "TSE3: Alsa scheduler error getting queue status\n"
                  << "      (" << snd_strerror(r) << ")\n";
    }

    snd_seq_real_time_t rt = *snd_seq_queue_status_get_real_time(status);
    int milli = rt.tv_sec*1000 + rt.tv_nsec/1000000;
    return msToClock(milli);
}


int AlsaMidiScheduler::impl_msecs()
{
    snd_seq_queue_status_t *status;
    snd_seq_queue_status_alloca(&status);
    int r;

    r = snd_seq_get_queue_status(pimpl->handle, pimpl->queue, status);
    if (r < 0)
    {
        std::cerr << "TSE3: Alsa scheduler error getting queue status\n"
                  << "      (" << snd_strerror(r) << ")\n";
    }

    snd_seq_real_time_t rt = *snd_seq_queue_status_get_real_time(status);
    return rt.tv_sec*1000 + rt.tv_nsec/1000000;
}


void AlsaMidiScheduler::impl_setTempo(int newTempo, Clock changeTime)
{
    tempoChanged(newTempo, changeTime);
/*    int r = snd_seq_change_queue_tempo(pimpl->handle,
                                      pimpl->queue, newTempo, NULL);
    if (r < 0)
    {
        std::cerr << "TSE3: Alsa scheduler error getting queue tempo\n"
                  << "      (" << snd_strerror(r) << ")\n";
    }*/
}


bool AlsaMidiScheduler::impl_eventWaiting()
{
    return snd_seq_event_input_pending(pimpl->handle, 1);
}


TSE3::MidiEvent AlsaMidiScheduler::impl_rx()
{
    if (!impl_eventWaiting()) return MidiEvent();

    snd_seq_event_t *ev;
    int r = snd_seq_event_input(pimpl->handle, &ev);
    if (r < 0)
    {
        std::cerr << "TSE3: Alsa scheduler error getting input event\n"
                  << "      (" << snd_strerror(r) << ")\n";
        return MidiEvent();
    }

    MidiEvent e;
    switch (ev->type)
    {
        case SND_SEQ_EVENT_NOTE:
        case SND_SEQ_EVENT_KEYPRESS:
        case SND_SEQ_EVENT_SYSEX:
        {
            // Hopefully this can't be read from the sequencer XXX
            std::cerr << "TSE3: Alsa: Received invalid input type " << ev->type
                      << "\n";
            break;
        }
        case SND_SEQ_EVENT_NOTEON:
        {
            e = MidiEvent(MidiCommand(MidiCommand_NoteOn,
                                      ev->data.note.channel, 0 /*port*/,
                                      ev->data.note.note,
                                      ev->data.note.velocity),
                          msToClock(msecs()));
            break;
        }
        case SND_SEQ_EVENT_NOTEOFF:
        {
            e = MidiEvent(MidiCommand(MidiCommand_NoteOff,
                                      ev->data.note.channel, 0 /*port*/,
                                      ev->data.note.note,
                                      ev->data.note.velocity),
                          msToClock(msecs()));
            break;
        }
        case SND_SEQ_EVENT_CONTROLLER:
        {
            e = MidiEvent(MidiCommand(MidiCommand_ControlChange,
                                      ev->data.control.channel, 0 /*port*/,
                                      ev->data.control.param,
                                      ev->data.control.value),
                          msToClock(msecs()));
            break;
        }
        case SND_SEQ_EVENT_PGMCHANGE:
        {
            e = MidiEvent(MidiCommand(MidiCommand_ProgramChange,
                                      ev->data.control.channel, 0 /*port*/,
                                      ev->data.control.value),
                          msToClock(msecs()));
            break;
        }
        case SND_SEQ_EVENT_PITCHBEND:
        {
            e = MidiEvent(MidiCommand(MidiCommand_PitchBend,
                                      ev->data.control.channel, 0 /*port*/,
                                      ev->data.control.value >> 7,
                                      ev->data.control.value & 0x7f),
                          msToClock(msecs()));
            break;
        }
        case SND_SEQ_EVENT_CHANPRESS:
        {
            e = MidiEvent(MidiCommand(MidiCommand_ChannelPressure,
                                      ev->data.control.channel, 0 /*port*/,
                                      ev->data.control.value),
                          msToClock(msecs()));
            break;
        }
    }
    snd_seq_free_event(ev);

    return e;
}


void AlsaMidiScheduler::impl_tx(MidiEvent e)
{
    pimpl->tx(e.data, pimpl->queue,
              clockToMs(e.time)/1000, (clockToMs(e.time)%1000)*1000000);
}


void AlsaMidiScheduler::impl_txSysEx(int port,
                                     const unsigned char *data, size_t size)
{
    snd_seq_event_t event;
    snd_seq_ev_clear(&event);
    snd_seq_ev_set_sysex(&event, size, (void *)data);
    snd_seq_ev_set_dest(&event, pimpl->dest[port].first,
                        pimpl->dest[port].second);
    snd_seq_ev_set_source(&event, pimpl->my_port);
    snd_seq_real_time_t time = { 0, 0 };
    snd_seq_ev_schedule_real(&event, SND_SEQ_QUEUE_DIRECT, 0/*abslt*/, &time);
    snd_seq_event_output(pimpl->handle, &event);
    snd_seq_drain_output(pimpl->handle);
}


