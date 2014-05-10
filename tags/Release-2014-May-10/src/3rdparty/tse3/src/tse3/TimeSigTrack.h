/*
 * @(#)TimeSigTrack.h 3.00 24 May 1999
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

#ifndef TSE3_TIMESIGTRACK_H
#define TSE3_TIMESIGTRACK_H

#include "tse3/listen/TimeSigTrack.h"

#include "tse3/Notifier.h"
#include "tse3/Playable.h"
#include "tse3/Serializable.h"
#include "tse3/EventTrack.h"

namespace TSE3
{
    /**
     * TimeSig objects are representations of a time signature change that
     * occurs in the @ref Song. They are held in the @ref TimeSigTrack.
     *
     * This class defines the type that is used to create @ref TimeSigEvents,
     * which are of type @ref Event<@ref TimeSig>.
     *
     * When streamed from a @ref Playable interface (using the
     * @ref TimeSigTrackIterator class) TimeSig objects are represented by a
     * @ref MidiEvent containing a @ref MidiCommand_TSE_Meta @ref MidiCommand
     * with data 1 byte as @ref MidiCommand_TSE_Meta_TimeSig.
     *
     * TimeSig is a value type.
     *
     * @short   A time signature change
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     TimeSigTrack
     * @see     Event
     */
    class TimeSig
    {
        public:

            /**
             * Create a TimeSig.
             *
             * @param top    Time signature top value
             * @param bottom Time signature bottom value
             */
            TimeSig(int top = 4, int bottom = 4) : top(top), bottom(bottom) {}

            /**
             * The time signature top value.
             */
            int top;

            /**
             * The time signature bottom value.
             */
            int bottom;

            int operator==(const TimeSig &t) const
            {
                return (top == t.top) && (bottom == t.bottom);
            }
    };

    /**
     * The TimeSigTrack provides a simple time ordered list of @ref TimeSig
     * changes in the Song.
     *
     * There is one TimeSigTrack per @ref Song.
     *
     * @short   A list of TimeSig events
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Event
     * @see     TimeSig
     */
    class TimeSigTrack : public EventTrack<TimeSig>,
                         public Serializable
    {
        public:

            /**
             * Creates an empty TimeSigTrack.
             */
            TimeSigTrack();
            virtual ~TimeSigTrack();

            /**
             * Returns the status of the TimeSigTrack.
             *
             * A value of true means timesig events are generated, a value of
             * false means no timesig events are generated
             *
             * @return TimeSigTrack status
             * @see    setStatus
             */
            bool status() const { return _status; }

            /**
             * Set the TimeSigTrack status value.
             *
             * @param s New TimeSigTrack status
             * @see   status
             */
            void setStatus(bool s) { _status = s; }

            /**
             * Converts the given @p time value into a bar/beat/pulse
             * set according to the time signatures in the TimeSigTrack.
             *
             * @param time  (in)  Time at which to find bar/beat/pulse
             * @param bar   (out) Returns bar value at @p time
             * @param beat  (out) Returns beat value at @p time
             * @param pulse (out) Returns pulse value at @p time
             */
            void barBeatPulse(Clock time, int &bar, int &beat, int &pulse);

            /**
             * @reimplemented
             */
            virtual PlayableIterator *iterator(Clock index);

            /**
             * @reimplemented
             */
            virtual Clock lastClock() const;

            /**
             * @reimplemented
             */
            virtual void save(std::ostream &o, int i) const;

            /**
             * @reimplemented
             */
            virtual void load(std::istream &i, SerializableLoadInfo &info);

        private:

            TimeSigTrack &operator=(const TimeSigTrack &);
            TimeSigTrack(const TimeSigTrack &);

            /**
             * Used by load to read events
             */
            void loadEvents(std::istream &i, int filePPQN);

            bool _status;
    };
}

#endif
