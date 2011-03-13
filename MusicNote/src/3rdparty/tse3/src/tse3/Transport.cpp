/*
 * @(#)Transport.cpp 3.00 24 May 1999
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

#include "tse3/Transport.h"

#include "tse3/Metronome.h"
#include "tse3/MidiScheduler.h"
#include "tse3/FlagTrack.h"
#include "tse3/PhraseEdit.h"
#include "tse3/FlagTrack.h"

using namespace TSE3;

/******************************************************************************
 * TransportCallback class
 *****************************************************************************/

TransportCallback::~TransportCallback()
{
}


/******************************************************************************
 * Transport class
 *****************************************************************************/

const Clock Transport::_minimumLookAhead = 10;

Transport::Transport(Metronome *m, MidiScheduler *s)
: _playable(0), iterator(0), flagTrack(0), recPE(0),
  metronome(m), metronomeIterator(0), _scheduler(s),
  _midiMapper(), _midiEcho(s),
  _status(Resting), _synchro(false), _punchIn(false), _autoStop(true),
  lastScheduledClock(0), lastPollPlaybackClock(0),
  _adaptiveLookAhead(true), _lookAhead(Clock::PPQN),
  _breakUps(0), injectedMidiCommand(),
  _playLeadIn(0), _recLeadIn(Clock::PPQN*4),
  transportLeadIn(Clock::PPQN/4)
{
    metronomeIterator = metronome->iterator(0);
    Listener<MidiSchedulerListener>::attachTo(_scheduler);
}


Transport::~Transport()
{
    if (_status != Resting) stop();
    delete metronomeIterator;
}


void Transport::attachCallback(TransportCallback *c)
{
    callbacks.push_back(c);
}


void Transport::detachCallback(TransportCallback *c)
{
    callbacks.remove(c);
}


void Transport::setLookAhead(Clock c)
{
    if (c >= 0 && c != _lookAhead)
    {
        _lookAhead = (c > _minimumLookAhead) ? c : _minimumLookAhead;
        notify(&TransportListener::Transport_Altered,
               TransportListener::LookAheadChanged);
    }
}


void Transport::setSynchro(bool s)
{
    _synchro = s;
    notify(&TransportListener::Transport_Altered,
           TransportListener::SynchroChanged);
}


void Transport::setPunchIn(bool p)
{
    _punchIn = p;
    notify(&TransportListener::Transport_Altered,
           TransportListener::PunchInChanged);
}


void Transport::setAutoStop(bool s)
{
    _autoStop = s;
    notify(&TransportListener::Transport_Altered,
           TransportListener::AutoStopChanged);
}


void Transport::setPlayLeadIn(Clock c)
{
    if (c >= 0) _playLeadIn = c;
    notify(&TransportListener::Transport_Altered,
           TransportListener::PlayLeadInChanged);
}


void Transport::setRecordLeadIn(Clock c)
{
    if (c >= 0) _recLeadIn = c;
    notify(&TransportListener::Transport_Altered,
           TransportListener::RecordLeadInChanged);
}


void Transport::setAdaptiveLookAhead(bool ala)
{
    _adaptiveLookAhead = ala;
}


void Transport::play(Playable *p, Clock start)
{
    if (_status == Playing || _status == SynchroPlaying)
    {
        stop();
        return;
    }
    else if (_status != Resting)
    {
        return;
    }

    if (start < 0) start = 0;

    // Prepare
    start                 -= _playLeadIn;
    lastScheduledClock     = start;
    lastPollPlaybackClock  = start;
    _playable              = p;
    punchInFilter          = 0;
    _breakUps              = 0;

    if (_playable)
    {
        iterator = _playable->iterator((start<0) ? Clock(0) : start);
    }
    else
    {
        iterator = 0;
    }

    metronomeIterator->moveTo(start);

    // Play Panic object
    PlayableIterator *pi = _startPanic.iterator(0);
    while (pi->more())
    {
        _scheduler->tx(*(*pi));
        callback_MidiOut((*pi)->data);
        ++(*pi);
    }
    delete pi;

    // Set Tempo and TimeSig

    // Start playing
    if (_synchro)
    {
        _scheduler->moveTo(start);
        _status = SynchroPlaying;
    }
    else
    {
        _scheduler->start(start-transportLeadIn);
        _status = Playing;
    }

    notify(&TransportListener::Transport_Status, Playing);
}


void Transport::record(Playable *p, Clock start, PhraseEdit *pe,
                       MidiFilter *filter)
{
    if (_status == Recording)
    {
        stop();
        return;
    }
    else if (_status != Resting)
    {
        return;
    }

    if (start < 0) start = 0;

    // Ready ourself to the PhraseEdit
    recPE = pe;
    Listener<PhraseEditListener>::attachTo(pe);

    // Prepare
    start                 -= _recLeadIn;
    lastScheduledClock     = start;
    lastPollPlaybackClock  = start;
    _playable              = p;
    _breakUps              = 0;
    punchInFilter          = filter;
    if (filter)
    {
        punchInStatus = filter->status();
    }
    if (_playable)
    {
        iterator = _playable->iterator((start<0) ? Clock(0) : start);
    }
    else
    {
        iterator = 0;
    }

    metronomeIterator->moveTo(start);

    punchedInYet = false;
    if (!_punchIn && filter)
    {
        // mute a particular Track
        filter->setStatus(false);
    }

    // Play Panic object
    PlayableIterator *pi = _startPanic.iterator(0);
    while (pi->more())
    {
        _scheduler->tx(*(*pi));
        callback_MidiOut((*pi)->data);
        ++(*pi);
    }
    delete pi;

    // Set Tempo and TimeSig

    // Start recording
    if (_synchro)
    {
        _scheduler->moveTo(start);
        _status = SynchroRecording;
    }
    else
    {
        _scheduler->start(start-transportLeadIn);
        _status = Recording;
    }

    notify(&TransportListener::Transport_Status, Recording);
}


void Transport::stop()
{
    if (_status == Resting)
    {
        if (_scheduler->clock() > 0) _scheduler->moveTo(0);
        return;
    }

    if (_status == Recording && punchInFilter)
    {
        punchInFilter->setStatus(punchInStatus);
    }

    stopPlayback(lastScheduledClock);

    // Play Panic object
    PlayableIterator *pi = _endPanic.iterator(0);
    while (pi->more())
    {
        _scheduler->tx(*(*pi));
        callback_MidiOut((*pi)->data);
        ++(*pi);
    }
    delete pi;
}


void Transport::ff(bool strong)
{
    // We add 0.5 * PPQN to the shiftBy time since it will be rounded down.
    Clock shift = strong ? Clock::PPQN * 9/2/*4.5*/ : Clock::PPQN * 3/2/*1.5*/;

    shiftBy(shift);
}


void Transport::ffFlag()
{
    if (!flagTrack) return;
    Clock now = _scheduler->clock();
    size_t index = flagTrack->index(now);
    if ((*flagTrack)[index].time < now || index == flagTrack->size()) return;
    ++index;
    Clock moveTo = (*flagTrack)[index].time;
    shiftBy(now - moveTo);
}


void Transport::rew(bool strong)
{
    Clock shift = strong ? Clock::PPQN * -4 : Clock::PPQN * -1;

    shiftBy(shift);
}


void Transport::rewFlag()
{
    if (!flagTrack) return;
    Clock now = _scheduler->clock();
    int index = flagTrack->index(now);
    Clock moveTo;
    if (index > 0 || (*flagTrack)[0].time < now)
    {
        index--;
        moveTo = (*flagTrack)[index].time;
    }
    else
    {
        moveTo = 0;
    }
    shiftBy(now - moveTo);
}


void Transport::shiftBy(Clock shiftBy)
{
    // We can't move in recording or synchro modes

    if (_status == Playing)
    {
        // First, flush the note off buffer at the lastScheduledClock time,
        // or we may never send these note offs.
        while (!noteOffBuffer.empty())
        {
            _scheduler->tx(noteOffBuffer.top().data);
            // mixer_process
            // callback ?
            noteOffBuffer.pop();
        }

        Clock moveTo = lastScheduledClock + shiftBy;
        moveTo -= moveTo % Clock::PPQN;
        if (moveTo < 0) moveTo = 0;

        _scheduler->moveTo(lastScheduledClock, moveTo);
        lastScheduledClock = moveTo;
        if (iterator)
        {
            iterator->moveTo(moveTo);
        }
        metronomeIterator->moveTo(moveTo);
    }
    else if (_status == Resting)
    {
        Clock moveTo = _scheduler->clock() + shiftBy;
        moveTo -= moveTo % Clock::PPQN;
        if (moveTo < 0) moveTo = 0;

        _scheduler->moveTo(moveTo);
    }
}


void Transport::poll()
{
    // MIDI input
    while (_scheduler->eventWaiting() || injectedMidiCommand.status)
    {
        // If we're in a synchro mode, start transport
        if (_status == SynchroPlaying)
        {
            _scheduler->start(lastScheduledClock);
            _status = Playing;
        }
        else if (_status == SynchroRecording)
        {
            _scheduler->start(lastScheduledClock);
            _status = Recording;
        }

        MidiEvent e;
        if (injectedMidiCommand.status)
        {
            e = MidiEvent(injectedMidiCommand, _scheduler->clock());
            injectedMidiCommand = MidiCommand();
        }
        else
        {
            e = _scheduler->rx();
        }
        _midiEcho.echo(e);
        callback_MidiIn(e.data);
        //mixer->processCommand_in(e);

        // We're not implementing a basic channel/port as in TSE2
        if (_status == Recording)
        {
            if (_punchIn && !punchedInYet && punchInFilter)
            {
                // mute the track
                punchInFilter->setStatus(false);
                punchedInYet = true;
            }
            recPE->insert(e);
        }
    }

    // MIDI output
    if (_status == Playing || _status == Recording) pollPlayback();
}


void Transport::pollPlayback()
{
    /*
     * We will be entered in playing mode.
     *
     * We don't need to test whether _playable == 0 since to get into playing
     * mode _playable has to point to a Playable object.
     */

    Clock now = _scheduler->clock();
    if (_adaptiveLookAhead)
    {
        _lookAhead = (now - lastPollPlaybackClock)*4 + (_lookAhead/2);
        if (_lookAhead < _minimumLookAhead) _lookAhead = _minimumLookAhead;
    }
    lastPollPlaybackClock = now;
    if (now > lastScheduledClock)
    {
        ++_breakUps;
    }

    if (now + _lookAhead > lastScheduledClock)
    {
        lastScheduledClock = now + _lookAhead;

        bool cont = true;  // whether to keep scheduling

        do
        {
            /*
             * Get the next event, which may come from the Playable, from the
             * noteOffBuffer or from the metronome.
             *
             * The following enum allows us to remember where events came from.
             */
            enum
            {
                from_None,
                from_Playable,
                from_NOBuff,
                from_Metro
            } from = from_None;

            MidiEvent e;

            // Get the next Playable event, if there is one
            if (iterator && iterator->more())
            {
                e    = **iterator;
                from = from_Playable;
                e    = _midiMapper.filter(e);
            }

            // This was below nobuffer, but metronome events can
            // cause transport not to stop. Should there be a separate
            // nobuffer for metronome off events? XXX
            if (from == from_None) cont = false;

            // Get the next noteOffBuffer event, if there is one
            if (!noteOffBuffer.empty())
            {
                const MidiEvent &e2 = noteOffBuffer.top();
                if (from == from_None || e2.time < e.time)
                {
                    e    = e2;
                    from = from_NOBuff;
                }
            }

            // Get the next Metronome event, if there is one
            if (metronome->status(_status))
            {
                const MidiEvent &e2 = **metronomeIterator;
                if (from == from_None || e2.time < e.time)
                {
                    e    = e2;
                    from = from_Metro;
                }
            }

            if (e.time < lastScheduledClock)
            {
                if (e.data.status >= MidiCommand_NoteOff)
                {
                    /*
                     * We have a MidiEvent to send to the scheduler. We do
                     * this and then consume it from whichever source it came.
                     *
                     * If it is a MidiCommand_NoteOn, we put the
                     * MidiCommand_NoteOff in a buffer to be sent later.
                     */
                    e = _filter.filter(e);
                    _scheduler->tx(e);
                    callback_MidiOut(e.data);
                    if (e.data.status == MidiCommand_NoteOn)
                    {
                        MidiEvent offEvent(e.offData, e.offTime);
                        noteOffBuffer.push(offEvent);
                    }
                }
                else if (e.data.status == MidiCommand_TSE_Meta)
                {
                    /*
                     * We have a TSE MetaEvent. Depending on it's type, do
                     * something productive.
                     */
                    switch (e.data.data1)
                    {
                        case MidiCommand_TSE_Meta_MoveTo:
                        {
                            /*
                             * We're about to move, so we schedule all the
                             * note off events to happen now.
                             */
                            while (!noteOffBuffer.empty())
                            {
                                MidiEvent off = noteOffBuffer.top();
                                noteOffBuffer.pop();
                                off.time = e.time;
                                _scheduler->tx(off);
                            }

                            _scheduler->moveTo(e.time, e.offTime);
                            /*
                             * We return from here since we'll be reentered
                             * through update with a new time frame. We can
                             * safely ignore the rest of this method.
                             */
                            return;
                        }
                        case MidiCommand_TSE_Meta_Tempo:
                        {
                            _scheduler->setTempo(e.data.data2, e.time);
                            break;
                        }
                        case MidiCommand_TSE_Meta_TimeSig:
                        {
                            metronome->setBarReference(e.time);
                            metronome->setBeatsPerBar(e.data.data2 >> 4);
                            break;
                        }
                    }
                }

                switch (from)
                {
                    case from_None:                           break;
                    case from_Playable: ++*iterator;          break;
                    case from_NOBuff:   noteOffBuffer.pop();  break;
                    case from_Metro:    ++*metronomeIterator; break;
                }

            }
            else
            {

                /*
                 * Don't continue scheduling since this event is past the
                 * look ahead window.
                 */
                cont = false;

            }

        } while (cont);

        // Handle automatic stop
        if ((!iterator || !iterator->more())
            && _autoStop
            && noteOffBuffer.empty()
            && _status == Playing)
        {
            stop();
        }
    }
}


void Transport::stopPlayback(Clock stopTime)
{
    _status = Resting;
    _scheduler->stop(stopTime);

    // empty the tx buffer somehow?
    while (!noteOffBuffer.empty())
    {
        const MidiEvent &e = noteOffBuffer.top();
        _scheduler->tx(e.data);
        noteOffBuffer.pop();
    }
    delete iterator;
    iterator = 0;

    if (_status == Recording)
    {
        Listener<PhraseEditListener>::detachFrom(recPE);
        recPE = 0;
    }

    _status   = Resting;
    _playable = 0;
    notify(&TransportListener::Transport_Status, Resting);
}


void Transport::inject(MidiCommand c)
{
    injectedMidiCommand = c;
    poll();
}


namespace
{
    /**
     * Function object that sends Transport_MidiOut to a TransportCallback.
     */
    class NotifyMidiOut
    {
        public:
            NotifyMidiOut(MidiCommand command)
                : command(command) {}
            void operator()(TransportCallback *c)
            {
                c->Transport_MidiOut(command);
            }
        private:
            MidiCommand command;
    };

    /**
     * Function object that sends Transport_MidiIn to a TransportCallback.
     */
    class NotifyMidiIn
    {
        public:
            NotifyMidiIn(MidiCommand command)
                : command(command) {}
            void operator()(TransportCallback *c)
            {
                c->Transport_MidiIn(command);
            }
        private:
            MidiCommand command;
    };
}


void Transport::callback_MidiOut(MidiCommand c)
{
    for_each(callbacks.begin(), callbacks.end(), NotifyMidiOut(c));
}


void Transport::callback_MidiIn(MidiCommand c)
{
    for_each(callbacks.begin(), callbacks.end(), NotifyMidiIn(c));
}


void Transport::handleMidiSchedulerEvent()
{
    if (_status != Resting)
    {
        Clock newTime = _scheduler->clock();
        if (iterator) iterator->moveTo(newTime);
        metronomeIterator->moveTo(newTime);
    }
}


void Transport::MidiScheduler_Started(MidiScheduler *)
{
    handleMidiSchedulerEvent();
}


void Transport::MidiScheduler_Stopped(MidiScheduler *)
{
    handleMidiSchedulerEvent();
}


void Transport::MidiScheduler_Moved(MidiScheduler *)
{
    handleMidiSchedulerEvent();
}


void Transport::Notifier_Deleted(FlagTrack *)
{
    flagTrack = 0;
}


void Transport::Notifier_Deleted(Metronome *)
{
    metronome = 0;
    std::cerr << "TSE3: Metronome deleted whilst in use by Transport\n";
}


void Transport::Notifier_Deleted(MidiScheduler *)
{
    _scheduler = 0;
    std::cerr << "TSE3: MidiScheduler deleted whilst in use by Transport\n";
    // XXX very bad things will now happen
}


void Transport::Notifier_Deleted(PhraseEdit *pe)
{
    if (pe == recPE)
    {
        stop();
        recPE = 0;
    }
}


void Transport::Notifier_Deleted(Playable *)
{
    _playable = 0;
    stop();
}

