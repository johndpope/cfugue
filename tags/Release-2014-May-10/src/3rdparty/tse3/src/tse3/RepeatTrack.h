/*
 * @(#)RepeatTrack.h 3.00 22 June 1999
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

#ifndef TSE3_REPEATTRACK_H
#define TSE3_REPEATTRACK_H

#include "tse3/listen/RepeatTrack.h"

#include "tse3/Notifier.h"
#include "tse3/Playable.h"
#include "tse3/Serializable.h"
#include "tse3/EventTrack.h"

namespace TSE3
{
    /**
     * Repeat objects hold the repeat time.
     *
     * This class defines the type that is used to create repeat events,
     * which are of type Event<Repeat>.
     *
     * When streamed from a @ref Playable interface (using the
     * @ref RepaetTrackIterator class) Repeat objects are represented by a
     * @ref MidiEvent containing a @ref MidiCommand_TSE_Meta @ref MidiCommand
     * with data 1 byte as @ref MidiCommand_TSE_Meta_MoveTo.
     *
     * Repeat is a value type.
     *
     * @short   A repeat point
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     RepeatTrack
     * @see     Event
     */
    class Repeat
    {
        public:

            /**
             * Create a Repeat.
             *
             * @param c The time to loop back to
             * @param s Whetner the Repeat event is enabled
             */
            Repeat(Clock c = 0, bool s = true) : repeat(c), status(s) {}

            /**
             * The time this repeat point will jump to.
             */
            Clock repeat;

            /**
             * Whether this repeat point is activated or not.
             */
            bool status;

            int operator==(const Repeat &r) const
            {
                return repeat == r.repeat && status == r.status;
            }
    };

    /**
     * The RepeatTrack provides a simple time ordered list of repeat points
     * in the Song.
     *
     * The RepeatTrack is no longer used by the @ref Song class - it has been
     * superceeded by a more simple mechanism. However it remains in the TSE3
     * library since it may find other uses.
     *
     * @short   A list of Repeat events
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Event
     * @see     Repeat
     */
    class RepeatTrack : public EventTrack<Repeat>,
                        public Serializable
    {
        public:

            /**
             * Creates an empty RepeatTrack.
             */
            RepeatTrack();
            virtual ~RepeatTrack();

            /**
             * Returns the status of the RepeatTrack.
             *
             * A value of true means repeat events are generated, a value of
             * false means no repeat events are generated
             *
             * @return RepeatTrack status
             * @see    setStatus
             */
            bool status() const { return _status; }

            /**
             * Set the status value.
             *
             * @param s New RepeatTrack status
             * @see   status
             */
            void setStatus(bool s) { _status = s; }

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

            /**
             * Used by load to read events
             */
            void loadEvents(std::istream &i, int filePPQN);

            bool _status;
    };
}

#endif
