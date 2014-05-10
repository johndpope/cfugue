/*
 * @(#)Transport.h 3.00 25 May 1999
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

#ifndef TSE3_TRANSPORT_H
#define TSE3_TRANSPORT_H

#include "tse3/listen/Transport.h"

#include "tse3/Notifier.h"
#include "tse3/Panic.h"
#include "tse3/MidiMapper.h"
#include "tse3/MidiEcho.h"
#include "tse3/listen/Playable.h"
#include "tse3/listen/PhraseEdit.h"
#include "tse3/listen/MidiScheduler.h"
#include "tse3/listen/FlagTrack.h"
#include "tse3/listen/Metronome.h"

#include <queue>
#include <list>

namespace TSE3
{
    /**
     * This object handles the playback/record mechanism. It is the central
     * control for musical input and output.
     *
     * It usually plays a @ref Song, but isn't restricted to @ref Songs since
     * it uses the abstract @ref Playable interface as the music source.
     *
     * When using this class it is important to call the @ref poll method
     * as often as possible: it does most of the work. It would be a good idea
     * to do this polling in a background thread (see @ref Impl::Mutex for
     * information on threading issues).
     *
     * Facilities provided by the Transport class include:
     * @li Play
     * @li Record into @ref PhraseEdit
     * @li Fast forward and rewind
     * @li Ability to attach callbacks which are kept informed of MIDI activity
     *     coming into or leaving the Transport object
     * @li Uses @ref Panic objects (for start and stop reset sequences), a
     *     @ref MidiEcho object
     * @li Synchro start (playback starts when MIDI input recieved)
     * @li Uses a @ref FlagTrack to move around the @ref Playable easily
     * @li Auto stop at the end of the @ref Playable object
     * @li Lead in values for play and record
     * @li Adaptive look ahead
     * @li Ability to 'inject' a @ref MidiCommand into the Transport stream.
     *
     * @short   Manager of transport facilities
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Song
     * @see     Playable
     * @see     TransportCallback
     */
    class Transport : public Listener<MidiSchedulerListener>,
                      public Listener<FlagTrackListener>,
                      public Listener<PhraseEditListener>,
                      public Listener<MetronomeListener>,
                      public Listener<PlayableListener>,
                      public Notifier<TransportListener>
    {
        public:

            /**
             * The Transport object starts up in resting mode.
             *
             * It is presumed that neither the metronome or scheduler objects
             * will be deleted whilst the Transport object exists.
             *
             * @param metronome @ref Metronome object to use
             * @param scheduler @ref MidiScheduler object to use
             * @see status
             */
            Transport(Metronome     *metronome,
                      MidiScheduler *scheduler);
            virtual ~Transport();

            /**
             * Register a callback object.
             *
             * @param c @ref TransportCallback object to attach
             */
            void attachCallback(TransportCallback *c);

            /**
             * Detach a callback object.
             *
             * @param c @ref TransportCallback object to detach
             */
            void detachCallback(TransportCallback *c);

            /**
             * Returns the @ref MidiScheduler that this Transport object
             * is using.
             *
             * @return MidiScheduler in use
             */
            MidiScheduler *scheduler() const { return _scheduler; }

            /**
             * @ref MidiFilter that affects how all @ref MidiEvents
             * are sent from the Transport object. You can use this to set
             * global overrides for some @ref Song setup; for example to
             * for every @ref MidiEvent to a specific port.
             *
             * @return The Part's @ref MidiFilter object
             */
            MidiFilter *filter() { return &_filter; };

            /**
             * The @ref Panic object used to send reset data at transport start.
             *
             * @return Transport's start @ref Panic object
             */
            Panic *startPanic() { return &_startPanic; }

            /**
             * The @ref Panic object used to send reset data at transport end.
             *
             * @return Transport's end @ref Panic object
             */
            Panic *endPanic() { return &_endPanic; }

            /**
             * This @ref MidiMapper object used to alter output destination.
             *
             * @return Transport's @ref MidiMapper object
             */
            MidiMapper *midiMapper() { return &_midiMapper; }

            /**
             * Returns the @ref MidiEcho object used to provide soft MIDI Thru.
             *
             * @return Transport's @ref MidiEcho object
             */
            MidiEcho *midiEcho() { return &_midiEcho; }

            /**
             * Set the @ref FlagTrack. You can specify the value 0 for no
             * @ref FlagTrack. The FlagTrack should be set if your playable
             * object is a @ref Song. If it is not, it's best to reset the
             * @ref FlagTrack value to 0.
             *
             * This is used when transport is resting and stop is called to
             * snap the stop position to a @ref Flag position.
             *
             * @param f @ref FlagTrack to use - it should tally with the
             *          current @ref Song
             */
            void setFlagTrack(FlagTrack *f) { flagTrack = f; }

            /**
             * Returns the current synchro mode.
             *
             * When enabled, transport will not start immediately when
             * play/record is called: it will only start when MIDI input is
             * recieved from the scheduler.
             *
             * @return Synchro mode status
             * @see    setSynchro
             */
            bool synchro() const { return _synchro; }

            /**
             * Set the synchro status.
             *
             * @param s New synchro mode status
             * @see   synchro
             */
            void setSynchro(bool s);

            /**
             * Returns the currrent punch-in record status.
             *
             * This effects how the recording is handled. When disabled, record
             * will mute the record channel. When enabled, muting only occurs
             * when the first MIDI input is recieved.
             *
             * @return Punch in status
             * @see    setPunchIn
             */
            bool punchIn() const { return _punchIn; }

            /**
             * Set the punch-in record status.
             *
             * @param p New punch in status
             * @see   punchIn
             */
            void setPunchIn(bool p);

            /**
             * Returns the currrent auto stop status.
             *
             * When enabled playback will automatically stop at the end of
             * the Playable object. This will not occur in recording mode.
             *
             * @return Auto stop mode status
             * @see    setAutoStop
             */
            bool autoStop() const { return _autoStop; }

            /**
             * Set the auto stop status.
             *
             * @param s New auto stop mode status
             * @see   setAutoStop
             */
            void setAutoStop(bool s);

            /**
             * Returns the playback lead in, the number of PPQN before
             * Song playback begins.
             *
             * @return The playback lead in
             * @see    setPlaybackLeadIn
             */
            Clock playLeadIn() const { return _playLeadIn; }

            /**
             * Sets the value of the playback lead in.
             *
             * @param c New playback lead in
             * @see   playbackLeadIn
             */
            void setPlayLeadIn(Clock c);

            /**
             * Returns the record lead in, the number of PPQN before
             * Song recording begins.
             *
             * @return The record lead in
             * @see    setRecordLeadIn
             */
            Clock recordLeadIn() const { return _recLeadIn; }

            /**
             * Sets the value of the record lead in.
             *
             * @param c New record lead in
             * @see   recordLeadIn
             */
            void setRecordLeadIn(Clock c);

            /**
             * Call this to start playback.
             *
             * If the request is successful, an event will be returned to all
             * @ref TransportListeners.
             *
             * @param p         The object to play.
             * @param startTime The time to start playback from.
             */
            void play(Playable *p, Clock startTime);

            /**
             * Call this to start recording.
             *
             * Whilst you are recording, a @ref Playable can be
             * played back (this will normally be the @ref Song you are working
             * on. You can optionally mute a @ref Track (well, it will
             * normally be a @ref Track) by specifying it's
             * @ref MidiFilter. (The 'setStatus' method is used to
             * perform the muting - it will be reset correctly afterwards).
             *
             * If course, you do not have to perform a mute, or do any playback
             * at all if you so wish.
             *
             * You would want to specify a mute @ref Track to silence the
             * @ref Track that the user thinks recording is taking place on
             * (when at the moment the @ref Song is not being altered by
             * the Transport's recording process at all).
             *
             * If the record request is successful, an event will be returned
             * to all @ref TransportListeners. Record may not do anything
             * (for example, you cannot enter Recording mode from Playing mode).
             *
             * If punch-in mode is enabled, the @ref Track the user thinks
             * recording is being performed on (when it's in fact going into
             * a @ref PhraseEdit object) will be muted when MIDI input is first
             * recieved. This muting is achieved by setting the @ref Track's
             * @ref MidiFilter status to false. It will be restored
             * when stop is called.
             *
             * If punch-in mode is disabled, then the filter will be muted
             * immediately, and restored when stop is called.
             *
             * If 0 is passed in filter then no muting will occur.
             *
             * @param p         The object to play.
             * @param startTime The time to start recording from.
             * @param pe        The PhraseEdit to record into.
             * @param filter    The filter for punch-in track (or 0).
             */
            void record(Playable *p, Clock startTime,
                        PhraseEdit *pe, MidiFilter *filter = 0);

            /**
             * Stop playback.
             *
             * If the request is successful, an event will be returned to all
             * @ref TransportListeners.
             *
             * If in Resting mode, then the 'resting clock' will be moved to
             * time zero.
             */
            void stop();

            /**
             * Fast forward.
             *
             * Makes the playback position jump forwards by
             * @ref Clock::PPQN.
             *
             * @param strong If true, fast forwards four times faster.
             */
            void ff(bool strong);

            /**
             * Fast forward to next flag position, if there is one.
             */
            void ffFlag();

            /**
             * Rewind.
             *
             * Makes the playback position jump backwards by
             * @ref Clock::PPQN.
             *
             * @param strong If true, rewinds four times faster.
             */
            void rew(bool strong);

            /**
             * Rewind to previous flag position, or the start if there is none.
             */
            void rewFlag();

            /**
             * The Transport object activity poll interface.
             *
             * This must be called often and rapidly (say every 10 usecs) if
             * you want to handle MIDI input properly. This includes recording
             * and MIDI echo.
             *
             * If you're only doing playback (and have disabled the MIDI echo
             * facility) then you can afford to call this only as often as you
             * need to (this is dependant on the @ref lookAhead()). This will
             * give your processor quite a rest.
             *
             * The poll method schedules an amount of musical data for playback
             * via the @ref MidiScheduler. The size of this 'window' of data
             * that is scheduled ahead of time is set by @ref lookAhead().
             */
            void poll();

            /**
             * Returns the current state of the Transport object
             *
             * @return Current Transport state
             */
            int status() const { return _status; }

            /**
             * If @ref status is not @ref Resting, then this will return
             * a pointer to the current @ref Playable object.
             *
             * When resting, this returns zero.
             *
             * @return Current playable
             * @see    play
             * @see    record
             */
            Playable *playable() const { return _playable; }

            /**
             * Transport status definitions for the current mode, as returned
             * by the @ref status method.
             */
            enum TransportMode
            {
                Resting,
                Playing,
                Recording,
                SynchroPlaying,
                SynchroRecording
            };

            /**
             * Returns the look ahead value. This is the amount of
             * data that the Transport object will schedule ahead of time
             * to ensure smooth and unbroken playback.
             *
             * @return Look ahead value
             * @see setLookAhead
             * @see minimumLookAhead
             * @see adaptiveLookAhead
             */
            Clock lookAhead() const { return _lookAhead; }

            /**
             * Sets the look ahead value.
             *
             * @param c New look ahead value
             * @see   lookAhead
             */
            void setLookAhead(Clock c);

            /**
             * Returns the minimum value @ref lookAhead can be set to. (There
             * has to be some kind of limit, setting it to zero is just asking
             * for trouble.)
             *
             * @return Minimum look ahead value
             * @see    lookAhead
             * @see    setLookAhead
             */
            Clock minimumLookAhead() const { return _minimumLookAhead; }

            /**
             * Returns whether the 'adaptive look ahead' facility is active.
             *
             * Adaptive look ahead causes the @ref poll method to calculate
             * the best value for @ref lookAhead. The calculation is based on
             * the frequency of calls to @ref poll and aims to reduce the
             * look ahead as much as possible (in order to increase system
             * responsivity) whilst ensuring that it is not so low as to
             * cause playback break-up.
             *
             * @return Adaptive look ahead status
             * @see    setAdaptiveLookAhead
             * @see    lookAhead
             * @see    setLookAhead
             */
            bool adaptiveLookAhead() const { return _adaptiveLookAhead; }

            /**
             * Set the value of the adaptive look ahead feature.
             *
             * @param ala New adaptive look ahead status
             * @see   adaptiveLookAhead
             */
            void setAdaptiveLookAhead(bool ala);

            /**
             * Returns the number of times playback has 'broken up' during the
             * current run of the Transport object. If this returns true you
             * may wish to increase the value of @ref lookAhead.
             *
             * @return The number of time playback has 'broken up'
             * @see    lookAhead
             * @see    setLookAhead
             */
            int breakUps() const { return _breakUps; }

            /**
             * Injects a @ref MidiCommand into the Transport object. This is a
             * facility used by components such as an on-screen keyboard.
             *
             * The Transport object acts as if this @ref MidiCommand was
             * recieved from the @ref MidiScheduler object, recording/echoing
             * it.
             *
             * The side effect of calling this member function is that a
             * call to @ref poll will be made (to process the injected @ref
             * MidiCommand.
             *
             * Take care: if you inject a @ref MidiCommand_NoteOn you must
             * ballance it with a @ref MidiCommand_NoteOff later on or
             * you will experience 'hanging notes'.
             *
             * @param c The @ref MidiCommand to inject
             */
            void inject(MidiCommand c);

            /**
             * @reimplemented
             */
            virtual void MidiScheduler_Started(MidiScheduler *);

            /**
             * @reimplemented
             */
            virtual void MidiScheduler_Stopped(MidiScheduler *);

            /**
             * @reimplemented
             */
            virtual void MidiScheduler_Moved(MidiScheduler *);

            /**
             * @reimplemented
             */
            virtual void Notifier_Deleted(FlagTrack *);

            /**
             * @reimplemented
             */
            virtual void Notifier_Deleted(Metronome *);

            /**
             * @reimplemented
             */
            virtual void Notifier_Deleted(MidiScheduler *);

            /**
             * @reimplemented
             */
            virtual void Notifier_Deleted(PhraseEdit *);

            /**
             * @reimplemented
             */
            virtual void Notifier_Deleted(Playable *);

        private:

            Transport &operator=(const Transport &);
            Transport(const Transport &);

            void startPlayback();

            /**
             * This method handles the playback of @ref MidiEvents read through
             * @ref Playable interfaces, sending them to the @ref MidiScheduler.
             */
            void pollPlayback();

            /**
             * Flushes the scheduler buffer and sends any pending
             * MidiCommand_NoteOffs immediately.
             */
            void stopPlayback(Clock stopTime);

            /**
             * Used by ff/rew methods.
             *
             * The c value is added, and then the final position is rounded
             * down to the previous whole @ref Clock::PPQN division.
             */
            void shiftBy(Clock c);

            /**
             * Sends Transport_MidiOut to all attached callbacks.
             */
            void callback_MidiOut(MidiCommand c);

            /**
             * Sends Transport_MidiIn to all attached callbacks.
             */
            void callback_MidiIn(MidiCommand c);

            /**
             * A single handler for every MidiSchedulerEvent except
             * the deleted event.
             */
            void handleMidiSchedulerEvent();

            std::list<TransportCallback *>  callbacks;
            Playable                       *_playable;
            PlayableIterator               *iterator;
            FlagTrack                      *flagTrack;
            PhraseEdit                     *recPE;
            std::priority_queue<MidiEvent,
                                std::vector<MidiEvent>,
                                std::greater<MidiEvent> >
                                            noteOffBuffer;
            Metronome                      *metronome;
            PlayableIterator               *metronomeIterator;
            MidiScheduler                  *_scheduler;
            MidiFilter                      _filter;
            Panic                           _startPanic;
            Panic                           _endPanic;
            MidiMapper                      _midiMapper;
            MidiEcho                        _midiEcho;

            int                             _status;
            bool                            _synchro;
            bool                            _punchIn;
            bool                            _autoStop;
            Clock                           lastScheduledClock;
            Clock                           lastPollPlaybackClock;
            bool                            _adaptiveLookAhead;
            Clock                           _lookAhead;
            static const Clock              _minimumLookAhead;
            int                             _breakUps;
            bool                            punchedInYet;
            MidiFilter                     *punchInFilter;
            bool                            punchInStatus;
            MidiCommand                     injectedMidiCommand;

            Clock                           _playLeadIn;
            Clock                           _recLeadIn;

            /**
             * The small amount of time which playback starts before the
             * specified time to give the transport object enough time to
             * get some MidiEvents into the scheduler.
             */
            Clock transportLeadIn;
    };
}

#endif
