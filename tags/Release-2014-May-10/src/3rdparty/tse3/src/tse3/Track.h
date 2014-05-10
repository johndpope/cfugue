/*
 * @(#)Track.h 3.00 17 May 1999
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

#ifndef TSE3_TRACK_H
#define TSE3_TRACK_H

#include "tse3/listen/DisplayParams.h"
#include "tse3/listen/Track.h"

#include "tse3/Playable.h"
#include "tse3/Serializable.h"
#include "tse3/listen/Part.h"
#include "tse3/Notifier.h"

#include <string>
#include <cstddef>

namespace TSE3
{
    class Song;
    class MidiFilter;
    class MidiParams;

    /**
     * A Track is a subunit of a @ref Song. Many Tracks can exist in a @ref Song
     * and are played concurrently. You can imagine them to correspond to the
     * different members of a band or orchestra.
     *
     * Tracks contain a number of parameters that effect the way the musical
     * data they contain is produced.
     *
     * Tracks contain @ref Parts. They 'own' them, and so a @ref Part's lifetime
     * is bound to it's parent Track. @ref Parts are created by external
     * agents - the user of the TSE3 library. Each @ref Part has a well defined
     * start and end time, and the @ref Part objects in the Track may not
     * overlap.
     *
     * @sect Command classes
     *
     * Use the following command classes to manipute this object in a undo/redo
     * environment.
     *
     *     @li @ref TSE3::Cmd::Track_SetInfo
     *     @li @ref TSE3::Cmd::Track_Snip
     *     @li @ref TSE3::Cmd::Track_Glue
     *     @li @ref TSE3::Cmd::Track_RemovePart
     *     @li @ref TSE3::Cmd::Track_Sort
     *     @li @ref TSE3::Cmd::Song_InsertTrack
     *     @li @ref TSE3::Cmd::Song_RemoveTrack
     *     @li @ref TSE3::Cmd::Song_SoloTrack
     *     @li @ref TSE3::Cmd::Part_Move
     *
     * @short   Represents the concurrent musical sections of a Song
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Song
     * @see     Part
     */
    class Track : public Listener<PartListener>,
                  public Listener<DisplayParamsListener>,
                  public Playable,
                  public Serializable,
                  public Notifier<TrackListener>
    {

        public:

            Track();
            virtual ~Track();

            /**
             * Read the Track title.
             *
             * @return Track's title string
             * @see    setTitle
             */
            const std::string &title() const;

            /**
             * Set the Track title.
             *
             * @param s New title string
             * @see   title
             */
            void setTitle(const std::string &s);

            /**
             * @ref MidiFilter that affects how @ref MidiEvents are
             * produced by this Track.
             *
             * @return Track's @ref MidiFilter
             */
            MidiFilter *filter();

            /**
             * The Track parameters.
             *
             * @return Track's @ref MidiParams
             */
            MidiParams *params();

            /**
             * The display parameters.
             *
             * @return The Track's @ref DisplayParams object
             */
            DisplayParams *displayParams();

            /**
             * Returns a pointer to the parent @ref Song that this
             * Track has been inserted into. If this Track has not been
             * inserted in a @ref Song, this method returns zero.
             *
             * @return @ref Song this Track is inserted in
             */
            Song *parent() const;

            /**
             * The number of @ref Parts in this Track.
             *
             * @return The number of @ref Parts
             */
            size_t size() const;

            /**
             * Return the @ref Part at the given index
             *
             * The value returned for an index that is out of range is
             * undefined. The @ref size method describes the valid
             * values.
             *
             * @param  n Index
             * @return @ref Part at this index
             */
            Part *operator[](size_t n) const;

            /**
             * Inserts a @ref Part between the given times. The Track 'owns'
             * this @ref Part and will delete it when it is deleted.
             *
             * If the @ref Part cannot be inserted (due to @ref Part overlap)
             * a @ref TrackError will be thrown.
             *
             * @param  start  @ref Part's start time
             * @param  end    @ref Part's end time
             * @throws TrackError
             * @return New @ref Part. If several @ref Part objects are inserted
             *         (because @p action is @ref Part::Under) the pointer
             *         to the first @ref Part will be returned.
             */
            Part *insert(Clock start, Clock end);

            /**
             * Inserts the given @ref Part into the Track at the @ref Part's
             * times.
             *
             * This causes the @ref Part to be 'owned' by the Track, it will
             * be deleted when the Track is deleted.
             *
             * You may not pass a @ref Part that has already been inserted
             * into a different (or indeed the same) Track (the @ref TrackError
             * exception will be thrown).
             *
             * If the @ref Part cannot be inserted (due to @ref Part overlap)
             * a @ref TrackError will be thrown.
             *
             * If the @ref Part has invalid times (i.e. start after end)
             * then a @ref TrackError will be thrown.
             *
             * @param  New @ref Part to insert
             * @throws TrackError
             */
            void insert(Part *part);

            /**
             * Remove the given @ref Part.
             *
             * The @ref Part will not be deleted, it is no longer considered
             * to be 'owned' by the Track.
             *
             * If the @ref Part is not in the Track then nothing will
             * happen.
             *
             * @param part @ref Part to remove - you must now delete it
             */
            void remove(Part *part);

            /**
             * Remove the @ref Part with the given index.
             *
             * The @ref Part will not be deleted, it is no longer considered
             * to be 'owned' by the Track.
             *
             * If the index is invalid then nothing will happen.
             *
             * @param part @ref Part to remove - you must now delete it
             */
            void remove(size_t index);

            /**
             * Returns the number of @ref Part objects (or bits of @ref Part
             * objects) that exist in the Track between the given times.
             *
             * @param   start Start of time zone
             * @param   end   End of time zone
             * @returns Number of @ref Part objects between times
             */
            size_t numPartsBetween(Clock start, Clock end);

            /**
             * This less than operation compares the Tracks' title strings.
             */
            int operator<(const Track &t) const;

            /**
             * Returns the index of the first @ref Part that occurs after
             * the given @ref Clock.
             *
             * @param  c Clock value to search for
             * @return Index of first Part at or after this time.
             *         If past the end of the Track object then returns
             *         'size'.
             */
            size_t index(Clock c) const;

            /**
             * Returns the index of the specified @ref Part (or size if not
             * in this Track).
             *
             * @param  part Pointer to @refPartTrack
             * @return The index of the specified @ref Part, or @ref size()
             */
            size_t index(Part *part) const;

            /**
             * @reimplemented
             */
            virtual void Notifier_Deleted(Part *);

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
            virtual void load(std::istream &in, SerializableLoadInfo &info);

            /**
             * @reimplemented
             */
            virtual void DisplayParams_Altered(DisplayParams *);

            /**
             * The @ref Song class is a friend so that it can access the
             * @ref setParentSong() method.
             */
            friend class Song;

        private:

            Track &operator=(const Track &);
            Track(const Track &);

            /**
             * This method is called by the @ref Song class alone. It is used
             * to set the parent @ref Song pointer, song. This will be set
             * to the parent's pointer, or to zero if the Track is removed
             * from the @ref Song.
             *
             * This is the only private member the @ref Song class accesses.
             *
             * @ref parent @ref Song object that this Track has been inserted
             *             into.
             */
            void setParentSong(Song *parent);

            /**
             * A private method to insert a @ref Part into the @ref parts
             * vector. This will also set the @ref Part's parent Track and
             * attach the Track to this @ref Part.
             *
             * The @ref Part passed must be valid to be placed in the vector
             * (i.e. it must not cause an overlap, be in any other Track).
             *
             * This private method is called by other insert methods.
             *
             * The notification for Part insertion is not performed.
             *
             * @param part @ref Part to insert
             */
            void prvInsertPart(Part *part);

            class TrackImpl *pimpl;
    };
}

#endif
