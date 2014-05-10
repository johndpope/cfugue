/*
 * @(#)MidiFilter.h 3.00 8 June 1999
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

#ifndef TSE3_EVENTFILTER_H
#define TSE3_EVENTFILTER_H

#include "tse3/listen/MidiFilter.h"

#include "tse3/Filter.h"
#include "tse3/Serializable.h"
#include "tse3/Midi.h"
#include "tse3/Notifier.h"

namespace TSE3
{
    /**
     * This the a standard (and comprehensive) @ref Filter object type that is
     * used by the TSE3 library.
     *
     * There is provision for the following features:
     *     @li enable/disable all events
     *     @li incomming MIDI channel filter
     *     @li forcing to a MIDI channel/port
     *     @li time shift
     *     @li time scaling
     *     @li linear quantise
     *     @li transposition
     *     @li velocity window clipping
     *     @li velocity scaling
     *
     * @sect Command classes
     *
     * Use the following command classes to manipute this object in a undo/redo
     * environment.
     *
     *     @li @ref TSE3::Cmd::Part_SetInfo
     *     @li @ref TSE3::Cmd::Track_SetInfo
     *
     * @short   Simple @ref MidiEvent filter
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Filter
     */
    class MidiFilter : public Filter,
                       public Serializable,
                       public Notifier<MidiFilterListener>
    {
        public:

            /**
             * Creates a MidiFilter with the default parameters
             * which leave @ref MidiEvents unchanged.
             */
            MidiFilter();
            MidiFilter(const MidiFilter &);
            virtual ~MidiFilter();
            MidiFilter &operator=(const MidiFilter &);

            /**
             * Returns the status value.
             *
             * A value of false means the filter blocks all output, returning
             * @ref MidiCommand_Invalid instead of output
             *
             * @return The status value
             * @see    setStatus
             */
            bool status() const { return _status; }

            /**
             * Set the status value.
             *
             * @param s The new status value to set
             * @see   status
             */
            void setStatus(bool s);

            /**
             * Returns the value of a MIDI channel filter.
             *
             * If true then events on this MIDI channel are allowed to pass
             * through filter. If false, events are not passed through filter.
             *
             * @param  c MIDI channel to find channel filter status of
             * @return The status of the MIDI channel's filter
             * @see    setChannelFilter
             */
            bool channelFilter(int c) const { return _channelFilter & (1<<c); }

            /**
             * Sets the value of a channel filter.
             *
             * @param c   MIDI channel to set filter status of
             * @param val New MIDI channel filter status value
             * @see   channelFilter
             */
            void setChannelFilter(int c, bool val);

            /**
             * Returns the MIDI channel value.
             *
             * A value of @ref MidiCommand::SameChannel means do not force to a
             * particular channel.
             *
             * @return MIDI channel events will be forced to
             * @see    setChannel
             */
            int channel() const { return _channel; }

            /**
             * Set the channel value.
             *
             * @param c New MIDI channel events will be forced to
             * @see   channel
             */
            void setChannel(int c);

            /**
             * Returns the MIDI port value.
             *
             * A value of @ref MidiCommand::SamePort means do not force to a
             * particular port.
             *
             * @return MIDI port number events will be forced to
             * @see    setPort
             */
            int port() const { return _port; }

            /**
             * Set the port value.
             *
             * @param p New MIDI port number events will be forced to
             * @see   port
             */
            void setPort(int p);

            /**
             * Returns the time offset value.
             *
             * @return @ref Clock value added to each @ref MidiEvent
             * @see    setOffset
             */
            Clock offset() const { return _offset; }

            /**
             * Sets the time offset value.
             *
             * @param o New @ref Clock value added to each @ref MidiEvent
             * @see   offset
             */
            void setOffset(Clock o);

            /**
             * Returns the time scale value.
             *
             * A value of 100 will have no effect.
             * The value is a percentage that ranges from 1 to 500.
             *
             * @return The time scale value
             * @see    setTimeScale
             */
            int timeScale() const { return _timeScale; }

            /**
             * Set the time scale value.
             *
             * @param t New time scale value
             * @see   timeScale
             */
            void setTimeScale(int t);

            /**
             * Returns the quantise value. This is a linear quantise resolution
             * that @ref MidiEvent times will be 'snapped' to.
             *
             * A value of zero means do not quantise.
             *
             * @return The quantise resolution value
             * @see    setQuantise
             */
            Clock quantise() const { return _quantise; }

            /**
             * Set the quantise value.
             *
             * @param q New quantise resolution value
             * @see   quantise
             */
            void setQuantise(Clock q);

            /**
             * Returns the minimum note length. This is applied to @ref
             * MidiCommand_NoteOn events to ensure that they are of at least
             * a certain duration.
             *
             * A value of 0 represents no minimum length.
             *
             * @return The minimum note length
             * @see    setMinLength
             */
            Clock minLength() const { return _minLength; }

            /**
             * Set the minimum length value.
             *
             * @param q New minimum length value
             * @see   minLength
             */
            void setMinLength(Clock ml);

            /**
             * Returns the minimum note length. This is applied to @ref
             * MidiCommand_NoteOn events to ensure that they are of at least
             * a certain duration.
             *
             * A value of -1 represents to maximum length.
             *
             * @return The maximum note length
             * @see    setMaxLength
             */
            Clock maxLength() const { return _maxLength; }

            /**
             * Set the maximum length value.
             *
             * @param lm New maximum length value
             * @see   minLength
             */
            void setMaxLength(Clock ml);

            /**
             * Returns the transpose value. This is an integer number that is
             * added to the value of notes in any note-related @ref MidiEvent.
             *
             * This can range between -127 to +127
             *
             * @return The transpose value
             * @see    setTranspose
             */
            int transpose() const { return _transpose; }

            /**
             * Set the transport value.
             *
             * @param t New transport value
             * @see   transpose
             */
            void setTranspose(int t);

            /**
             * Returns the minimum velocity window value. Note @ref MidiEvents
             * with veloicties below this value will have their velocity set
             * to this value. This is useful to prevent quiet notes from
             * not being heard, for example.
             *
             * A value of 0 will have no effect, and is considered 'off'.
             *
             * @return Minimum velocity window value
             * @see    setMinVelocity
             */
            int minVelocity() const { return _minVelocity; }

            /**
             * Set the minimum velocity window value.
             *
             * @param v New minimum velocity window value
             * @see   minVelocity
             */
            void setMinVelocity(int v);

            /**
             * Returns the maximum velocity window value. Note @ref MidiEvents
             * with velocities above this value will have their veolcity set
             * to this value. This is useful to prevent loud notes from drowning
             * out other notes, for example.
             *
             * A value of 127 will have no effect, and is considered 'off'.
             *
             * @return Maximum velocity window value
             * @see    setMaxVelocity
             */
            int maxVelocity() const { return _maxVelocity; }

            /**
             * Set the maximum velocity window value.
             *
             * @param v New maximum velocity window value.
             * @see   maxVelocity
             */
            void setMaxVelocity(int v);

            /**
             * Returns the velocity scale value. This value scales the time
             * value of a @ref MidiEvent.
             *
             * A value of 100 will have no effect.
             * The value is a percentage that ranges from 1 to 200.
             *
             * @return Velocity scale value
             * @see    setVelocityScale
             */
            Clock velocityScale() const { return _velocityScale; }

            /**
             * Set the velocity scale value.
             *
             * @param v New velocity scale value
             * @see   velocityScale
             */
            void setVelocityScale(int v);

            /**
             * @reimplemented
             */
            virtual MidiEvent filter(const MidiEvent &e) const;

            /**
             * @reimplemented
             */
            virtual void save(std::ostream &o, int i) const;

            /**
             * @reimplemented
             */
            virtual void load(std::istream &i, SerializableLoadInfo &info);

        private:

            void setChannelFilter(int c) { _channelFilter = c; }

            bool         _status;
            unsigned int _channelFilter;
            int          _channel;
            int          _port;
            Clock        _offset;
            int          _timeScale;
            Clock        _quantise;
            Clock        _minLength;
            Clock        _maxLength;
            int          _transpose;
            int          _minVelocity;
            int          _maxVelocity;
            int          _velocityScale;
    };
}

#endif
