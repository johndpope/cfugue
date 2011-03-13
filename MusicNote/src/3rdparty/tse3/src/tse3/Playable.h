/*
 * @(#)Playable.h 3.00 20 May 1999
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

#ifndef TSE3_PLAYABLE_H
#define TSE3_PLAYABLE_H

#include "tse3/listen/Playable.h"

#include "tse3/Notifier.h"
#include "tse3/Midi.h"

namespace TSE3
{
    class PlayableIterator;

    /**
     * The interface that an object that can produce @ref MidiEvents must
     * implement. @ref Playable objects allow a 'player' to get an iterator
     * that will produce all the @ref MidiEvents in the object. Each
     * implementation of @ref Playable will therefore also supply an
     * implementation of @ref PlayableIterator which is accesed via the
     * @ref iterator() method.
     *
     * Since all @ref Song components implement the Playable interface, this is
     * an application of the 'Composite' pattern from the GoF book.
     *
     * @short   A 'playable' object interface
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     */
    class Playable : public Notifier<PlayableListener>
    {
        public:

            Playable() {}

            virtual ~Playable() {};

            /**
             * Creates a new @ref PlayableIterator object and passes it to you
             * ready to iterate over the @ref MidiEvents in this object.
             *
             * It is the user's responsibility to delete it.
             *
             * @param  index The time at which to position the iterator
             * @return New @ref PlayableIterator that emits @ref MidiEvent
             *         data contained in this object - you must delete the
             *         @ref PlayableIterator when you have finished with it.
             */
            virtual PlayableIterator *iterator(Clock index) = 0;

            /**
             * Returns the last @ref Clock value that will be returned by
             * this Playable object. Since all @ref MidiEvents are delivered
             * in strict order, this will also be the greatest @ref Clock
             * value returned by this Playable object.
             *
             * @return The last @ref Clock value in the object
             */
            virtual Clock lastClock() const = 0;

        protected:

            Playable &operator=(const Playable &);
            Playable(const Playable &);
    };

    /**
     * This is an iterator that moves over every @ref MidiEvent in a
     * @ref Playable object. Each @ref MidiEvent are delivered in time order.
     * This is a use of the 'Iterator' design pattern in the GoF book.
     *
     * Each kind of @ref Playable object will provide an implementation of the
     * PlayableIterator interface that knows how to iterate over that kind of
     * object.
     *
     * @short   Iterator over the MidiEvents in a Playable
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Playable
     * @see     MidiEvent
     */
    class PlayableIterator
    {
        public:

            /**
             * Create a PlayableIterator. Client code will never call this
             * member function.
             *
             * @sect Important note
             *
             * Derived classes must attach themselves to the parent Playable
             * object as listeners, and then implement @ref Listener::deleted
             * properly.
             *
             * @internal
             */
            PlayableIterator();
            virtual ~PlayableIterator();

            /**
             * Returns the next @ref MidiEvent in the iterator
             *
             * @return Next @ref MidiEvent
             * @see    operator->
             */
            const MidiEvent &operator*() const { return _next; }

            /**
             * Returns the next @ref MidiEvent in the iterator
             *
             * @return Next @ref MidiEvent
             * @see    operator*
             */
            const MidiEvent *operator->() const { return &_next; }

            /**
             * Returns whether there are any more events to come; i.e.
             * whether @ref operator++ is valid on this object.
             *
             * @return Whether there are any more events
             */
            bool more() const { return _more; }

            /**
             * Moves the iterator to the given time position.
             *
             * @param c Time to move iterator to.
             */
            virtual void moveTo(Clock /*c*/) { _more = false; }

            /**
             * Increments the iterator, so that it will return the next
             * @ref MidiEvent in the Playable.
             */
            PlayableIterator &operator++();

            //PlayableIterator operator++(int);
                // We can't implement this operator: it is impossible to
                // make a copy of a PlayableIterator: there is no
                // copy constructor.

        protected:

            virtual void getNextEvent() = 0;

            MidiEvent _next; // Implementations must put the next MidiEvent
                             // in here.
            bool      _more; // Implementations must define whether or not
                             // this is true.
        private:

            PlayableIterator &operator=(const PlayableIterator &);
            PlayableIterator(const PlayableIterator &);
    };
}

#endif
