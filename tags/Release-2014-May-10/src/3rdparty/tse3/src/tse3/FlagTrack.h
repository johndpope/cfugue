/*
 * @(#)FlagTrack.h 3.00 19 May 1999
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

#ifndef TSE3_FLAGTRACK_H
#define TSE3_FLAGTRACK_H

#include "tse3/listen/FlagTrack.h"

#include "tse3/Notifier.h"
#include "tse3/Serializable.h"
#include "tse3/EventTrack.h"

#include <string>

namespace TSE3
{
    /**
     * Flags are simply named time points in the @ref Song, held in the
     * @ref FlagTrack. This class defines the type that is used to create
     * Flag events, which are of type @ref Event<@ref Flag>.
     *
     * Flag is a value type.
     *
     * @short   A flag
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     FlagTrack
     * @see     Event
     */
    class Flag
    {
        public:

            /**
             * Create a Flag with no title
             */
            Flag() {}

            /**
             * Create a Flag with the given title.
             *
             * @param title The Flag's title
             */
            Flag(const std::string &t) : _title(t) {}

            /**
             * Returns the 'title' of this Flag.
             */
            const std::string &title() const { return _title; }

            int operator==(const Flag &f) const
            {
                return _title == f._title;
            }

        private:

            std::string _title;
    };

    /**
     * The FlagTrack provides a simple time ordered list of @ref Flag
     * @ref Events in the @ref Song. These are just marked time positions.
     *
     * There is one FlagTrack per Song.
     *
     * @sect Command classes
     *
     * Use the following command classes to manipute this object in a undo/redo
     * environment.
     *
     *     @li @ref TSE3::Cmd::FlagTrack_Add
     *
     * @short   A list of Flag events
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Event
     * @see     Flag
     */
    class FlagTrack : public EventTrack<Flag>,
                      public Serializable
    {
        public:

            /**
             * Creates an empty FlagTrack.
             */
            FlagTrack();

            virtual ~FlagTrack();

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

            FlagTrack &operator=(const FlagTrack &);
            FlagTrack(const FlagTrack &);

            /**
             * Used by load to read events
             */
            void loadEvents(std::istream &i, int filePPQN);
    };
}

#endif
