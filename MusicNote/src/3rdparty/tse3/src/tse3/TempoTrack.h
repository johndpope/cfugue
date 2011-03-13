/*
 * @(#)TempoTrack.h 3.00 24 May 1999
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

#ifndef TSE3_TEMPOTRACK_H
#define TSE3_TEMPOTRACK_H

#include "tse3/listen/TempoTrack.h"

#include "tse3/Notifier.h"
#include "tse3/Playable.h"
#include "tse3/Serializable.h"
#include "tse3/EventTrack.h"

namespace TSE3
{
    /**
     * Tempo objects are simply integer values of a tempo change that occur in
     * the Song, held in the @ref TempoTrack.
     *
     * This class defines the type that is used to create tempo events,
     * which are of type @ref Event<@ref Tempo>.
     *
     * When streamed from a @ref Playable interface (using the
     * @ref TempoTrackIterator class) Tempo objects are represented by a
     * @ref MidiEvent containing a @ref MidiCommand_TSE_Meta @ref MidiCommand
     * with data 1 byte as @ref MidiCommand_TSE_Meta_Tempo.
     *
     * Tempo is a value type.
     *
     * @short   A tempo change
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     TempoTrack
     * @see     Event
     */
    class Tempo
    {
        public:

            /**
             * Create a Tempo.
             *
             * @param t Tempo value
             */
            Tempo(int t = 120) : tempo(t) {}

            int tempo;

            int operator==(const Tempo &f) const { return tempo == f.tempo; }
    };

    /**
     * The TempoTrack provides a simple time ordered list of tempo changes
     * in the @ref Song.
     *
     * There is one TempoTrack per @ref Song.
     *
     * @short   A list of Tempo events
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Event
     * @see     Tempo
     */
    class TempoTrack : public EventTrack<Tempo>,
                       public Serializable
    {
        public:

            /**
             * Creates an empty TempoTrack.
             */
            TempoTrack();
            virtual ~TempoTrack();

            /**
             * Returns the status of the TempoTrack.
             *
             * A value of true means tempo events are generated, a value of
             * false means no tempo events are generated
             *
             * @return TempoTrack status
             * @see    setStatus
             */
            bool status() const { return _status; }

            /**
             * Set the TempoTrack status value.
             *
             * @param s New status
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

            TempoTrack &operator=(const TempoTrack &);
            TempoTrack(const TempoTrack &);

            /**
             * Used by load to read events.
             */
            void loadEvents(std::istream &i, int filePPQN);

            bool _status;
    };
}

#endif
