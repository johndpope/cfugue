/*
 * @(#)MidiData.h 3.00 14 May 1999
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

#ifndef TSE3_MIDIDATA_H
#define TSE3_MIDIDATA_H

#include "tse3/listen/MidiData.h"

#include "tse3/Notifier.h"
#include "tse3/Midi.h"
#include "tse3/Playable.h"

#include <vector>
#include <cstddef>

namespace TSE3
{
    /**
     * The MidiData class represents a list of @ref MidiEvents in time order.
     * It is an abstract base class, notably implemented by @ref Phrase and
     * @ref PhraseEdit.
     *
     * MidiData objects are not designed to be editable. Only the
     * @ref PhraseEdit class defines an editable MidiData object.
     *
     * MidiData does not implement the @ref Playable interface. This is for a
     * very good reason: more than one @ref Part may be using a given
     * @ref Phrase at the same time. The @ref Playable interface doesn't
     * provide an 'iterator' type so only one client can play a @ref Playable
     * at once. Therefore MidiData doesn't implement @ref Playable. It's a
     * piece of cake to use, anyway.
     *
     * @short   A list of MidiEvents
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     MidiEvent
     * @see     Phrase
     * @see     PhraseEdit
     * @see     MidiDataListener
     */
    class MidiData : public Playable,
                     public Notifier<MidiDataListener>
    {
        public:

            /**
             * Returns the number of @ref MidiEvents in this MidiData object.
             *
             * @return Number of @ref MidiEvents
             */
            size_t size() const { return data.size(); }

            /**
             * Returns the nth @ref MidiEvent in this MidiData object.
             *
             * The value returned for an index that is out of range is
             * undefined. The @ref size method describes the valid
             * values.
             *
             * @param  n Index
             * @return MidiEvent at index n
             */
            MidiEvent const &operator[](size_t n) const { return data[n]; }

            /**
             * Returns the index of the first event that occurs after
             * the given @ref Clock.
             *
             * @param  m  Clock value to search for.
             * @return Index of first MidiEvent at or after this time.
             *         If past the end of the MidiData object then returns
             *         'size'.
             */
            size_t index(Clock c) const;

            /**
             * @reimplemented
             */
            virtual PlayableIterator *iterator(Clock index);

            /**
             * @reimplemented
             */
            virtual Clock lastClock() const;

        protected:

            /**
             * MidiData objects can only be created by @ref PhraseEdit
             * objects.
             */
            MidiData(int noEvents);
            virtual ~MidiData() = 0;
            MidiData &operator=(const MidiData &);
            MidiData(const MidiData &);

            std::vector<MidiEvent> data;

            friend class PhraseEdit;
    };
}

#endif
