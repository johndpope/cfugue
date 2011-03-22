/*
 * @(#)MidiScheduler.h 3.00 10 November 2001
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

#ifndef TSE3_SCHEDULER_H
#define TSE3_SCHEDULER_H

#include "Common/StrUtils.h"
#include "tse3/listen/MidiScheduler.h"

#include "tse3/Notifier.h"
#include "tse3/Midi.h"
#include "tse3/util/MulDiv.h"

#include <cstddef>
#include <vector>
#include <cstring>

namespace TSE3
{

    /**
     * This class will generate an appropriate @ref MidiScheduler for the
     * current platform. It provides a measure of platform indepenance in the
     * generation of @ref MidiSchedulers.
     *
     * To create a portable and maintainable TSE3 application, do not directly
     * instantiate a particular MidiScheduler, use the MidiSchedulerFactory.
     *
     * @short   MidiScheduler class factory
     * @author  Pete Goodliffe
     * @version 3.00
     */
    class MidiSchedulerFactory
    {
        public:

            /**
             * Creates a MidiSchedulerFactory object.
             *
             * You can specify whether or not the factory can return a
             * @ref NullMidiScheduler in the case of no appropriate platform
             * MidiScheduler being available.
             *
             * @param canReturnNull true if the factory can return a
             *                      NullMidiScheduler in the worst case
             */
            MidiSchedulerFactory(bool canReturnNull = false);
            virtual ~MidiSchedulerFactory();

            /**
             * Create the most appropriate (for some definition of appropriate
             * given by the version of the MidiSchedulerFactory class)
             * @ref MidiScheduler object.
             *
             * The @ref MidiScheduler is created with new; it is your
             * responsibility to ensure it is deleted.
             *
             * @return New appropriate @ref MidiScheduler object
             */
            MidiScheduler *createScheduler();

        private:

            MidiSchedulerFactory &operator=(const MidiSchedulerFactory &);
            MidiSchedulerFactory(const MidiSchedulerFactory &);

            bool _canReturnNull;
    };

    /**
     * The MidiScheduler provides the interface to the MIDI system (be it
     * a software or hardware driver). On top of this it implements a timed
     * stream for reception and transmission of MidiEvents.
     *
     * This is the TSE3 platform independant interface to the underlying
     * MIDI hardware. (In fact, it might not even be hardware, this could
     * be the interface to a software synth).
     *
     * To create a MidiScheduler you will need to use an object derived from
     * this class. The @ref MidiSchedulerFactory classes help you to create
     * this.
     *
     * @sect Ports
     *
     * The interface provides a number of MIDI "ports" (which may be added to
     * or removed at run time, depending on the type of system TSE3 is running
     * on).
     *
     * These ports are referenced by a unique number. You read or write data
     * from/to ports via the @ref MidiEvent data type. The MidiEvent has a
     * "port" number field which identifies which port the command is destined
     * for.
     *
     * As well as having numbers, the API allows you read a port name. You
     * can also set a "preferred" name for each port. This allows the user to
     * give a more useful name to the port (perhaps for use in a GUI).
     *
     * Port numbers are not necessarily contiguous and can have any value -
     * this will depend on the phyiscal MIDI device in use. In order to find
     * out what port numbers are in use use the following APIs:
     *   @li @ref MidiScheduler::numPorts()
     *   @li @ref MidiScheduler::portNumbers()
     *
     * Note that there are two reserved special port numbers:
     *   @li @ref MidiCommand::NoPort   - Commands addressed here disappear
     *   @li @ref MidiCommand::AllPorts - Commands addressed here go to every
     *                                    port in the system.
     *
     * @sect Time
     *
     * As well as managing the MIDI ports, the MidiScheduler looks after a
     * clock that it used to schedule delivery of these events. The API
     * allows you to start/stop/move the time line, as well as set the clock's
     * tempo.
     *
     * The MidiScheduler can notify back to its clients if the timestream has
     * moved, or been started/stopped by a device on the MIDI connection.
     *
     * If the timestream is 'stopped' the MidiScheduler class will still
     * remember a 'current' clock value, informally known as the 'resting
     * clock'. When you ask for the time (see @ref MidiScheduler::clock)
     * you will be given this 'resting clock'.
     *
     * @sect Remote control
     *
     * Additionally, the MidiScheduler provides a MIDI remote control facility,
     * where certain MIDI notes can trigger transport start/stop.
     *
     * The keys that are set to be "remote controls" will not be passed up
     * through the API in a @ref MidiEvent if remote control is enabled.
     *
     * If you switch this facility on, you have to bear in mind that the only
     * time the MidiScheduler gets a chance to investigate the input note
     * stream is as you consume events from it. (For simplicity it doesn't
     * have a clever background thread implementation).
     *
     * Therefore, read events from the MidiScheduler in a timely manner.
     *
     * @sect That's all folks
     *
     * The MidiScheduler class does not incorporate a lot of other fancy
     * utilities, like channel/port remapping, for example. Higher level TSE3
     * components do this, for example the @ref MidiMapper class. The
     * @ref Transport class ties this all together.
     *
     * @sect Implementing a MidiScheduler
     *
     * If you are porting TSE3 to another platform, you will need to
     * implement this MidiScheduler interface. It uses the template method
     * pattern (GoF book) - you have to implement a few member functions
     * and the base MidiScheduler class looks after most of the logic.
     *
     * These template methods are in the protected section near the end and
     * are all pretty easy to understand.
     *
     * @short   A MIDI interface providing scheduling facilities
     * @author  Pete Goodliffe
     * @version 3.01
     * @see     MidiEvent
     */
    class MidiScheduler : public Notifier<MidiSchedulerListener>
    {
        public:

            MidiScheduler();

            /**
             * If the MidiScheduler is running, it will be stopped.
             */
            virtual ~MidiScheduler();

            /**
             * Returns a string describing the particular implementation
             * of the MidiScheduler interface.
             *
             * @return Implementation name string
             */
            OIL::StrUtils_Return_Type implementationName() const
            {
                return this->impl_implementationName();
            }

            /******************************************************************
             * Finding out about the MIDI ports
             *****************************************************************/

            /**
             * Returns the number of port addressable by this MidiScheduler.
             * If this changes at any point the
             * @ref MidiSchedulerListener::MidiScheduler_Ports event
             * is raised.
             *
             * When a port is created it is assigned a "number" - this could
             * be any positive integer. This is its permanent reference ID.
             * It is the port number that is specified in the @ref MidiCommand
             * or @ref MidiEvent.
             *
             * @return Number of MIDI ports this MidiScheduler provides
             */
            size_t numPorts() const { return _portNumbers.size(); }

            /**
             * Returns whether the specified port number is valid.
             *
             * @param  port MIDI port number
             * @return true if there is currently a port with that number
             */
            bool validPort(int port) const;

            /**
             * Returns the "index"th port, if they were held in a list. So,
             * to get the first port number set index to 0, for the seonc
             * give index 1, and so on.
             *
             * If you specify a value of @ref numPorts or above, the result
             * is undefined.
             *
             * @param  index value
             * @return Port number with specified index
             * @see    numPorts
             * @see    portNumbers
             * @see    numberToIndex
             */
            int portNumber(size_t index) const
            {
                return _portNumbers[index].first;
            }

            /**
             * Converts a port number to an index. This is the reverse
             * operation of @ref portNumber.
             *
             * If the port number is not valid, this will return zero.
             *
             * @param  number Port number
             * @return "Index" of this port
             * @see    portNumber
             */
            size_t numberToIndex(int number) const;

            /**
             * Empties the contents of the supplied @ref std::vector,
             * and inserts into it every available port number.
             *
             * @param numbers std::vector which is given the current MIDI
             *                port numbers
             */
            void portNumbers(std::vector<int> &numbers) const;

            /**
             * Returns a string describing the port with the given number.
             *
             * If the port number is invalid the port name will be a
             * suitable "invalid" string, not a zero pointer.
             *
             * @param  port MIDI port number
             * @return Port name string
             */
            OIL::StrUtils_Return_Type portName(int port) const;

            /**
             * Returns a string describing the type of the port with the given
             * number.
             *
             * If the port number is invalid the port name will be a
             * suitable "invalid" string, not a zero pointer.
             *
             * @param  port MIDI port number
             * @return Port type string
             */
            OIL::StrUtils_Return_Type portType(int port) const;

            /**
             * Returns whether or not the port is readable. If this function
             * returns false, then you will never receive an event from
             * @ref rx from this port.
             *
             * If the port number is invalid, this will return false.
             *
             * @param  port MIDI port number
             * @return Whether @ref MidiEvent objects are readable from this
             *         port
             * @see    portWritable
             */
            bool portReadable(int port) const;

            /**
             * Returns whether or not the port is writeable. If this function
             * returns false, then you can't send @ref MidiEvent objects out
             * via this port (with @ref tx). If you specify this port number
             * in a @ref MidiEvent given to @ref tx, the @ref MidiEvent will
             * be ignored.
             *
             * If the port number is invalid, this will return false.
             *
             * @param  port MIDI port number
             * @return Whether @ref MidiEvent objects are readable from this
             *         port
             * @see    portReadable
             */
            bool portWriteable(int port) const;

            /**
             * Returns whether or not the port is "internal". If this function
             * returns false, then @ref MidiEvent objects sent here will be
             * transmitted vai MIDI to an external device.
             *
             * If the port number is invalid, this will return false.
             *
             * @param  port MIDI port number
             * @return Whether the port connects to an internal or external
             *         MIDI device.
             * @see    defaultInternalPort
             * @see    defaultExternalPort
             */
            bool portInternal(int port) const;

            /**
             * Returns a "default" internal port number. If you don't know
             * which internal port to use, choose this one! If there are no
             * internal ports, this returns @ref MidiCommand::NoPort.
             *
             * (The default internal port will in fact be the first registered
             * internal port from the platform implementation of MidiScheduler).
             *
             * @return Default internal port, or @ref MidiCommand::NoPort
             * @see    defaultExternalPort
             */
            int defaultInternalPort() const;

            /**
             * Returns a "default" external port number. If you don't know
             * which external port to use, choose this one! If there are no
             * external ports, this returns @ref MidiCommand::NoPort.
             *
             * (The default external port will in fact be the first registered
             * external port from the platform implementation of MidiScheduler).
             *
             * @return Default external port, or @ref MidiCommand::NoPort
             * @see    defaultInternalPort
             */
            int defaultExternalPort() const;

            /******************************************************************
             * Manipulating the clock
             *****************************************************************/

            /**
             * Start the scheduler clock running, set to the given time.
             *
             * @param startTime The time to start the scheduler at
             */
            void start(Clock startTime) { this->impl_start(startTime); }

            /**
             * Start the scheduler clock at the current 'resting time' (as
             * set by a call to @ref stop).
             */
            void start() { this->impl_start(_restingClock); }

            /**
             * Stop the scheduler clock and flush the Tx buffer instantaneously.
             * The stopTime is remembered as a 'resting time' and subsequent
             * calls to @ref clock will return this value.
             *
             * @param stopTime The time at which to stop (-1 means immediately)
             */
            void stop(Clock stopTime = -1);

            /**
             * Enquire whether the scheduler clock is running.
             *
             * @return Whether the clock is running
             */
            bool running() const { return _running; }

            /**
             * Without stopping, move the scheduler clock to the given time
             * newTime at time moveTime. Any further calls to clock()
             * after this will return times in the new time line.
             *
             * If the scheduler is not running, this just sets the
             * 'resting time'.
             *
             * @param moveTime Time at which to perform the move
             * @param newTime  Time to move to
             */
            void moveTo(Clock moveTime, Clock newTime);

            /**
             * Without stopping, move the scheduler clock immediately.
             *
             * @param moveTime Time to move to
             */
            void moveTo(Clock moveTime) { moveTo(clock(), moveTime); }

            /**
             * Read the scheduler clock.
             *
             * This works whether the scheduler is running or not. If it has
             * been stopped then it returns the time the scheduler was stopped
             * at (or has since been moved to) - the 'resting time'.
             *
             * @return MidiScheduler time value
             */
            Clock clock()
            {
                return _running ? this->impl_clock() : _restingClock; }

            /**
             * Read the scheduler clock in milliseconds.
             *
             * @return MidiScheduler time value in milliseconds
             */
            int msecs() { return this->impl_msecs(); }

            /**
             * Read the tempo.
             *
             * @return Current tempo
             * @see    setTempo
             */
            int tempo() const { return _tempo; }

            /**
             * Set the tempo.
             *
             * @param newTempo   The new tempo value in beats per
             *                   minute (1-256)
             * @param changeTime Only used if the scheduler is running to
             *                   indicate when the tempo change occurs. Any
             *                   further calls to clock() before changeTime
             *                   will return bogus values, as they will be in
             *                   the new tempo's time scale.
             * @see   clock
             * @see   tempo
             */
            void setTempo(int newTempo, Clock changeTime)
            {
                if (newTempo >= 0)
                {
                    this->impl_setTempo(newTempo, changeTime);
                    _tempo = newTempo;
                }
            }

            /******************************************************************
             * Transmitting and receiving commands
             *****************************************************************/

            /**
             * Enquire whether there is any data in the input buffer.
             *
             * This function will return true if there has been any input
             * from any of the readable MIDI ports that you haven't yet read.
             *
             * @return Whether there is any input data ready to be processed
             */
            bool eventWaiting() { return this->impl_eventWaiting(); }

            /**
             * Return and remove a @ref MidiEvent from scheduler receive
             * buffer.
             *
             * Note that it is possible for this function to return a
             * @ref MidiEvent with MidiCommand_Invalid status. All TSE3 classes
             * must be able to cope with these events flying around.
             *
             * If there is no event waiting (see @ref eventWaiting()) then
             * this API will return an invalid @ref MidiEvent.
             *
             * @return A MidiEvent containing the data. If the status of
             *         the MidiCommand is zero, there wasn't a whole
             *         MidiCommand in the buffer.
             */
            MidiEvent rx();

            /**
             * Transmit a @ref MidiCommand immediately.
             *
             * This bypasses every other scheduled @ref MidiEvent and
             * is sent to the hardware before them.
             *
             * @param mc The MidiCommand to transmit
             */
            void tx(MidiCommand mc);

            /**
             * Adds an event to scheduler transmit buffer, to be transmitted at
             * the appropriate time.
             *
             * Events must be given to this method <b>in time order</b>. This
             * is regardless of what port it is destined for.
             *
             * <b>Note:</b> if this is a @ref MidiCommand_NoteOn, then the
             * matching MidiCommand_NoteOff part of the @ref MidiEvent is
             * ignored, you have to schedule that separately.
             *
             * If the clock is not running, the event will not be scheduled
             * and will just be ignored.
             *
             * @param event @ref MidiEvent to schedule to transmission
             */
            void tx(MidiEvent event);

            /*
             * Transmit a MIDI "system exclusive" data section immeditately.
             * The data is prepended by a MidiSystem_SysExStart status byte,
             * and followed by a MidiSystem_SysExEnd status byte. You don't
             * have to include these in your parameters.
             *
             * Although you use this method to send sysex data, you receive
             * it through the normal rx mechanism; you will receive a
             * @ref MidiEvent with a @ref MidiSystem_SysExStart status byte,
             * and the first data byte (in data1). Subsequent data
             * bytes are received as MidiEvents with this same status byte. The
             * sysex data end is denoted by a @ref MidiEvent with the
             * @ref MidiSystem_SysExEnd status byte.
             *
             * @param port MIDI port number
             * @param data A buffer containing the sysex data to send
             * @param size The number of bytes in the @p data buffer
             */
            void txSysEx(int port, const unsigned char *data, size_t size);

            /******************************************************************
             * Remote control
             *****************************************************************/

            /**
             * Read the remote control status.
             *
             * @return true if remote control enabled, false if disabled
             * @see    setRemoteControl
             */
            bool remoteControl() const { return _remote; }

            /**
             * Sets the remote control status.
             *
             * @param s New remote control status
             * @see   remoteControl
             */
            void setRemoteControl(bool s) { _remote = s; }

            bool consumeRemoveEvents() const { return _consumeRemote; }

            /**
             * Returns the start note. When pressed this note will cause the
             * scheduler to start.
             *
             * @return Start note
             * @see    setStartNote
             */
            unsigned int startNote() const { return _startNote; }

            /**
             * Sets the start  note.
             *
             * @param n New start note
             * @see   startNote
             */
            void setStartNote(unsigned int n) { _startNote = n; }

            /**
             * Returns the stop note. When pushed with the shift note held down,
             * this note will cause the scheduler to stop.
             *
             * @return Stop note
             * @see    setStopNote
             */
            unsigned int stopNote() const { return _stopNote; }

            /**
             * Sets the stop note.
             *
             * @param n New stop note
             * @see   stopNote
             */
            void setStopNote(unsigned int n) { _stopNote = n; }

        protected:

            MidiScheduler(const MidiScheduler &);
            MidiScheduler &operator=(const MidiScheduler &);

            /******************************************************************
             * Implementation functions
             *
             * To implement a MidiScheduler for a particular platform you
             * implement the functions below. They are called by the
             * public MidiScheduler functions, using the Template Method
             * design pattern (GoF book).
             *
             * You can assume that you will never be called with an invalid
             * port number, and a number of other parameters are guaranteed
             * to be valid, as documented.
             *
             * Functions like impl_start and impl_stop are more requests
             * than commands. The MidiScheduler will not assume that the
             * clock has started until the implementation calls the
             * clockStarted function.
             *
             * You shouldn't need to perform any notifications, however, you
             * do need to take care to call the next block of protected APIs to
             * inform the MidiScheduler code what's going on. If you
             * don't do this, the MidiScheduler class will not behave
             * properly.
             *
             * Note that in your destructor you will also want to put the
             * following code first:
             *     // if playing, stop first!
             *     if (MidiScheduler::running()) stop();
             *
             * Your implementation of the MidiScheduler will have to deal
             * with timing. You will be sent events (via impl_tx) IN TIME
             * ORDER, for some time in the not-too distant future (as usually
             * specified by the Transport class's look-ahead). You have to
             * do the work of sending the event at the exact timer tick.
             *****************************************************************/

            virtual OIL::StrUtils_Return_Type impl_implementationName() const = 0;
            virtual OIL::StrUtils_Return_Type impl_portName(int port) const = 0;
            virtual OIL::StrUtils_Return_Type impl_portType(int port) const = 0;
            virtual bool            impl_portReadable(int port) const = 0;
            virtual bool            impl_portWriteable(int port) const = 0;
            /**
             * Current state is guaranteed to be stopped.
             *
             * Don't forget to call clockStarted if the start succeeds.
             */
            virtual void impl_start(Clock clock) = 0;
            /**
             * Current state is guaranteed to be started. The @p clock value
             * will not ever be -1.
             *
             * Don't forget to call clockStopped if the start succeeds.
             */
            virtual void impl_stop(Clock clock) = 0;
            /**
             * Guaranteed running.
             *
             * Don't forget to call clockMoved if the start succeeds.
             */
            virtual void impl_moveTo(Clock moveTime, Clock newTime) = 0;
            /**
             * Guaranteed running.
             */
            virtual Clock impl_clock() = 0;
            /**
             * Guaranteed running.
             */
            virtual int impl_msecs() = 0;
            /**
             * Guaranteed tempo > 0. For the duration of this method,
             * @ref tempo() will return the old tempo value.
             *
             * Don't forget to call clockMoved if the start succeeds.
             */
            virtual void impl_setTempo(int tempo, Clock changeTime) = 0;
            virtual bool impl_eventWaiting() = 0;
            /**
             * You'll buffer all MidiEvents recieved. Returns the top
             * buffered MidiEvent (or MidiEvent() if none are buffered).
             */
            virtual MidiEvent impl_rx() = 0;
            /**
             * Send this MidiCommand NOW. Bypass any queued events.
             *
             * Timer may be running or not.
             */
            virtual void impl_tx(MidiCommand mc) = 0;
            /**
             * Puts a MidiEvent on the queue ready for transmission.
             * You will be given MidiEvents in time order, so your buffer can
             * be a FIFO rather than a time-ordered queue.
             *
             * The MidiEvent will be for some time in the future, you will have
             * to arrange to transmit it at the appropriate point.
             *
             * Timer may be running or not.
             */
            virtual void impl_tx(MidiEvent mc) = 0;
            /**
             * Send a sysex package now, bypassing the transsion queue.
             */
            virtual void impl_txSysEx(int port,
                                      const unsigned char *data,
                                      size_t size) = 0;

            /******************************************************************
             * Implementations call these functions to let the MidiScheduler
             * class know whats going on.
             *
             * Don't forget to call these at appropriate times from the
             * impl_XXX functions above.
             *****************************************************************/

            /**
             * Tells the MidiScheduler that your implementation has an
             * available MIDI port. This is safe to call in your ctor.
             *
             * You can suggest a port number for the MidiScheduler, but you
             * are not necessarily guaranteed to get it.
             *
             * If you don't want to go to the effort of constructing a
             * port number, always specify zero.
             *
             * This function returns the actual port number you have been
             * assigned.
             *
             * @param portIndex     You implementation's port reference
             * @param isInternal    Specify true if this is an internal
             *                      sound generator, or false if it is a
             *                      MIDI link to an external device.
             * @param requestedPort The port number you'd like to present
             *                      to the user
             */
            int addPort(int portIndex, bool isInternal, int requestedPort = 0);

            /**
             * You don't need to call removePort in your implementation's
             * destructor.
             */
            void removePort(int portIndex);
            void clockStarted(Clock startTime);
            void clockStopped(Clock stopTime);
            void clockMoved(const Clock moveTime, Clock newTime);
            void tempoChanged(int tempo, Clock changeTime);

            /******************************************************************
             * Internal helper functions you can call
             *****************************************************************/

            /**
             * startClock holds the scheduler clock at hardware time '0'.
             * Many interfaces always start the timer at a reference count
             * of zero and use this variable to work out what MidiScheduler
             * time this is.
             */
            Clock startClock;
            bool   _running;
            Clock  _restingClock;

            // XXX use muldiv switch?

            /**
             * An internal method to convert @ref Clock values to
             * millisecond times.
             */
            int clockToMs(Clock time)
            {
                return Util::muldiv(time-startClock, 60000/Clock::PPQN, _tempo);
            }

            /**
             * An internal method to convert millisecond time values
             * to @ref Clocks.
             */
            Clock msToClock(int ms)
            {
                return startClock + Util::muldiv(ms, _tempo, 60000/Clock::PPQN);
            }

        private:

            /******************************************************************
             * Internal private functions
             *****************************************************************/

            /**
             * Processes the supplied event according to the remote control
             * setup. It also processes MIDI system messages.
             *
             * This utility is called by internal functions that have
             * called impl_rx.
             */
            MidiEvent doRemoteControl(MidiEvent e);

            /**
             * Returns whether the specified channel number is valid to
             * pass on to an implementation. The only valid channels are 0-15
             *
             * @param  port MIDI port number
             * @return true if there is currently a port with that number
             */
            bool validChannel(int channel) const
            {
                return channel >= 0 && channel <= 15;
            }
            bool validChannel(MidiCommand mc) const
            {
                return validChannel(mc.channel);
            }

            /**
             * Converts a port number (as visible to the public API) to
             * the port index understood by the platform implementation,
             * returning whether the lookup was sucessful or not.
             */
            bool lookUpPortNumber(MidiCommand &mc) const;
            bool lookUpPortNumber(int &port) const;

            /**
             * For a MidiCommand recieved from impl_rx we'll need to
             * convert the platform port index into a port number. This
             * is how to do it.
             */
            void setToPortNumber(MidiCommand &mc) const;

            /******************************************************************
             * Internal private data
             *****************************************************************/

            /**
             * The internal map of port numbers to implementation indexes.
             * This consists of a collection of pairs
             *    (public port number, implementation port index)
             */
            struct PortInfo
            {
                int  index;
                bool isInternal;
                PortInfo(int n, bool i) : index(n), isInternal(i) {}
            };
            typedef std::vector<std::pair<int,PortInfo> > port_vector;

            port_vector    _portNumbers;

            int            _tempo;
            bool           _remote;
            bool           _consumeRemote;
            unsigned int   _startNote;
            unsigned int   _stopNote;
            int            _defaultInternal;
            int            _defaultExternal;
    };
}

#endif

