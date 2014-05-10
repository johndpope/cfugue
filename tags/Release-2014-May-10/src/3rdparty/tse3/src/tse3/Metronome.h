/*
 * @(#)Metronome.h 3.00 25 May 1999
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

#ifndef TSE3_METRONOME_H
#define TSE3_METRONOME_H

#include "tse3/listen/Metronome.h"

#include "tse3/Notifier.h"
#include "tse3/Playable.h"

namespace TSE3
{
    /**
     * This class provides the functionality of a metronome.
     *
     * The metronome remembers whether it should be produced in playing and
     * recording modes, but it doesn't enforce this itself - it is the
     * responsibility of the @ref Transport playback routines.
     *
     * There are two different metronome ticks that are produced, for bars
     * and beats. They must have the same MIDI channel, port, and duration, but
     * can have different notes and velocities.
     *
     * To determine the point at which clocks bar ticks are produced, a bar
     * reference exists.
     *
     * @short   Metronome tick object
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Playable
     */
    class Metronome : public Playable,
                      public Notifier<MetronomeListener>
    {
        public:

            /**
             * Constructs a Metronome with default tick.
             */
            Metronome();
            virtual ~Metronome();

            /**
             * Returns the metronome MIDI channel.
             *
             * @return Metronome MIDI channel
             * @see    setChannel
             */
            int channel() const { return _channel; }

            /**
             * Sets the metronome MIDI channel.
             *
             * @param c New metronome MIDI channel
             * @see   channel
             */
            void setChannel(int c);

            /**
             * Returns the metronome MIDI port.
             *
             * @return Metronome MIDI port
             * @see    setPort
             */
            int port() const { return _port; }

            /**
             * Sets the metronome MIDI port.
             *
             * @param p New metronome MIDI port
             * @see   port
             */
            void setPort(int p);

            /**
             * Returns the metronome duration.
             *
             * @return Metronome tick duration
             * @see    setDuration
             */
            int duration() const { return _duration; }

            /**
             * Sets the metronome duration.
             *
             * @param p New metronome tick duration
             * @see   duration
             */
            void setDuration(int d);

            /**
             * Returns the metronome bar note. This is the note that is played
             * on a metronome tick that occurs at the start of a bar.
             *
             * @return Metronome bar note
             * @see    setBarNote
             * @see    setBeatNote
             * @see    setBarVelocity
             */
            int barNote() const { return _barNote; }

            /**
             * Sets the metronome bar note.
             *
             * @param n New metronome bar note
             * @see   barNote
             * @see   beatNote
             * @see   barVelocity
             */
            void setBarNote(int n);

            /**
             * Returns the metronome bar velocity.
             *
             * @return Metronome bar velocity
             * @see    setBarVelocity
             * @see    setBeatVelocity
             * @see    setBarNote
             */
            int barVelocity() const { return _barVelocity; }

            /**
             * Sets the metronome bar velocity.
             *
             * @param v New metronome bar velocity
             * @see   barVelocity
             * @see   beatVelocity
             * @see   barNote
             */
            void setBarVelocity(int v);

            /**
             * Returns the metronome beat note. This is the note that is played
             * on a metronome tick that does not occur at the start of a bar.
             *
             * @return Metronome beat note
             * @see    setBeatNote
             * @see    setBarNote
             * @see    setBeatVelocity
             */
            int beatNote() const { return _beatNote; }

            /**
             * Sets the metronome beat note.
             *
             * @param n New metronome beat note
             * @see   beatNote
             * @see   barNote
             * @see   beatVelocity
             */
            void setBeatNote(int n);

            /**
             * Returns the metronome beat velocity.
             *
             * @return Metronome beat velocity
             * @see    setBeatVelocity
             * @see    setBarVelocity
             * @see    setBeatNote
             */
            int beatVelocity() const { return _beatVelocity; }

            /**
             * Sets the metronome bar velocity.
             *
             * @param v New metronome bar velocity
             * @see   beatrVelocity
             * @see   barVelocity
             * @see   beatNote
             */
            void setBeatVelocity(int v);

            /**
             * Returns the metronome playing status for a given @ref Transport
             * object status mode.
             *
             * @param  mode @ref Transport object status mode
             * @return Whether metronome is produced in this mode
             * @see    setStatus
             */
            bool status(int mode) const
            {
                return _status & (1<<mode);
            }

            /**
             * Sets the metronome playing status for a given transport mode.
             *
             * @param mode @ref Transport object status mode
             * @param s    New metronome playing status
             * @see   status
             */
            void setStatus(int mode, bool s);

            /**
             * Returns the metronome beats per bar value.
             *
             * A normal TSE3 library user need not worry about this method.
             *
             * The @ref Transport object uses this value to tell the metronome
             * the current time signtaure information. The value represents
             * the number of quarter notes in a bar. It is used in conjuction
             * with the barReference to determine which metronome ticks are
             * 'bar' and which are 'beat'.
             *
             * @internal
             * @return Metronome beats per bar value
             * @see    setBeatsPerBar
             */
            int beatsPerBar() const { return _beatsPerBar; }

            /**
             * Sets the metronome beats per bar value.
             *
             * A normal TSE3 library user need not worry about this method.
             *
             * @internal
             * @param bpb New beats per bar value
             * @see   beatsPerBar
             */
            void setBeatsPerBar(int bpb);

            /**
             * Returns the metronome bar reference clock.
             *
             * A normal TSE3 library user need not worry about this method.
             *
             * This is the time value of some accented 'bar' note. This
             * tells the Metronome object when to create a 'bat' tick and
             * when to create a 'beat' tick when used in conjuction with the
             * @ref beatsPerBar method.
             *
             * @internal
             * @return A bar tick reference time
             * @see    setBarReference
             * @see    beatsPerBar
             *
             */
            Clock barReference() const { return _barReference; }

            /**
             * Sets the metronome bar reference clock.
             *
             * A normal TSE3 library user need not worry about this method.
             *
             * @internal
             * @param br New bar tick reference time
             * @see   barReferenceTime
             */
            void setBarReference(Clock br);

            /**
             * @reimplemented
             */
            virtual PlayableIterator *iterator(Clock index);

            /**
             * @reimplemented
             */
            virtual Clock lastClock() const;

            friend class MetronomeIterator;

        private:

            Metronome &operator=(const Metronome &);
            Metronome(const Metronome &);

            void calculateCommands();

            int         _channel, _port;
            int         _status;
            Clock       _duration;
            int         _barNote, _barVelocity;
            int         _beatNote, _beatVelocity;
            int         _beatsPerBar;
            Clock       _barReference;

            MidiCommand barNoteOn, barNoteOff;   // Cached commands calculated
            MidiCommand beatNoteOn, beatNoteOff; // by calculateCommands

            Clock       pos; // Playable support
    };
}

#endif
