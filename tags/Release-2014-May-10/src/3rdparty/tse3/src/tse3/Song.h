/*
 * @(#)Song.h 3.00 26 May 1999
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

#ifndef TSE3_SONG_H
#define TSE3_SONG_H

#include "tse3/listen/Song.h"

#include "tse3/Playable.h"
#include "tse3/Serializable.h"
#include "tse3/listen/Track.h"
#include "tse3/Notifier.h"

#include <string>
#include <cstddef>

namespace TSE3
{
    class TempoTrack;
    class TimeSigTrack;
    class KeySigTrack;
    class PhraseList;
    class FlagTrack;

    /**
     * The Song is the container of an entire piece of music. It contains
     * a number of @ref Track s, which play concurrently - they correspond
     * to the different members in a band playing together.
     *
     * The Song class also holds a @ref TempoTrack, @ref TimeSigTrack,
     * @ref KeySigTrack, and @ref FlagTrack. It conatins the
     * @ref PhraseList which holds the list of every @ref Phrase in the Song.
     * (The @ref Phrases are used by @ref Part objects held within each
     * @ref Track).
     *
     * @sect Command classes
     *
     * Use the following command classes to manipute this object in a undo/redo
     * environment.
     *
     *     @li @ref TSE3::Cmd::Song_SetInfo
     *     @li @ref TSE3::Cmd::Song_SetTitle
     *     @li @ref TSE3::Cmd::Song_SetAuthor
     *     @li @ref TSE3::Cmd::Song_SetCopyright
     *     @li @ref TSE3::Cmd::Song_SetDate
     *     @li @ref TSE3::Cmd::Song_InsertTrack
     *     @li @ref TSE3::Cmd::Song_RemoveTrack
     *     @li @ref TSE3::Cmd::Song_SoloTrack
     *     @li @ref TSE3::Cmd::Track_Sort
     *     @li @ref TSE3::Cmd::Phrase_Erase
     *     @li @ref TSE3::Cmd::Phrase_Replace
     *
     * @short   A piece of playable music
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Notifier
     * @see     Track
     */
    class Song : public Playable,
                 public Listener<TrackListener>,
                 public Serializable,
                 public Notifier<SongListener>
    {
        public:

            /**
             * The default number of @ref Tracks in a new Song.
             */
            static const int defaultNoTracks = 16;

            /**
             * Creates a Song containing the given number of @ref Tracks.
             * Each @ref Track will be empty.
             *
             * @param noTracks The initial number of (empty) @ref Tracks;
             *                 omit this value for a default (16)
             */
            Song(int noTracks = defaultNoTracks);
            virtual ~Song();

            /**
             * Returns the Song's @ref PhraseList.
             *
             * @return Song's @ref PhraseList
             */
            PhraseList *phraseList();

            /**
             * Returns the Song's @ref TempoTrack.
             *
             * @return Song's @ref TempoTrack
             */
            TempoTrack *tempoTrack();

            /**
             * Returns the Song's @ref TimeSigTrack.
             *
             * @return Song's @ref TimeSigTrack
             */
            TimeSigTrack *timeSigTrack();

            /**
             * Returns the Song's @ref KeySigTrack.
             *
             * @return Song's @ref KeySigTrack
             */
            KeySigTrack *keySigTrack();

            /**
             * Returns the Song's @ref FlagTrack.
             *
             * @return Song's @ref FlagTrack
             */
            FlagTrack *flagTrack();

            /**
             * Read the Song title.
             *
             * @return Song's title string
             * @see    setTitle
             */
            const std::string &title() const;

            /**
             * Set the Song title.
             *
             * @param s New Song title string
             * @see   title
             */
            void setTitle(const std::string &s);

            /**
             * Read the Song author.
             *
             * @return Song's author string
             * @see    setAuthor
             */
            const std::string &author() const;

            /**
             * Set the Song author.
             *
             * @param s New Song author string
             * @see   author
             */
            void setAuthor(const std::string &s);

            /**
             * Read the Song copyright message.
             *
             * @return Song's copyright message string
             * @see    setCopyright
             */
            const std::string &copyright() const;

            /**
             * Set the Song copyright message.
             *
             * @param s New Song copyright message string
             * @see   copyright
             */
            void setCopyright(const std::string &s);

            /**
             * Read the Song date.
             *
             * @return Song's date string
             * @see    setDate
             */
            const std::string &date() const;

            /**
             * Set the Song date.
             *
             * @param s New Song date string
             * @see   date
             */
            void setDate(const std::string &s);

            /**
             * Returns the number of @ref Track objects in the Song.
             *
             * @return Number of @ref Track objects in the Song
             */
            size_t size() const;

            /**
             * Returns the @ref Track at the given index.
             *
             * The value returned for an index that is out of range is
             * undefined. The @ref size method describes the valid
             * values.
             *
             * @param  n Index
             * @return @ref Track at this index
             */
            Track *operator[](size_t n) const;

            /**
             * Returns the index of the specified @ref Track (or size if not
             * in this Song).
             *
             * @param  track Pointer to @ref Track
             * @return The index of the specified @ref Track, or @ref size()
             */
            size_t index(Track *track) const;

            /**
             * Inserts a @ref Track at the given position. If you don't
             * specify a position, the the @ref Track will be inserted at the
             * end.
             *
             * This causes the Track to be 'owned' by the Song, it will
             * be deleted when the Song is deleted.
             *
             * @param  n Index to insert at, or -1 for 'at end'
             * @return Pointer to the new @ref Track (which will be deleted by
             *         the Song)
             */
            Track *insert(int n = -1);

            /**
             * Insert this @ref Track at the given position. If you don't
             * specify a position, the the @ref Track will be inserted at the
             * end.
             *
             * This causes the Track to be 'owned' by the Song, it will
             * be deleted when the Song is deleted.
             *
             * If the @ref Track has already been inserted in a Song, the
             * @ref SongError exception will be thrown.
             *
             * @param track @ref Track to insert
             * @param n Index to insert at
             * @throws SongError
             */
            void insert(Track *track, int n = -1);

            /**
             * Remove the @ref Track at the given position. This will not
             * delete it.
             *
             * @param n Index of @ref Track to remove - you must now delete it
             */
            void remove(size_t n);

            /**
             * Removes the given @ref Track. This will not delete it.
             *
             * @ref track @ref Track to remove - you must now delete it
             */
            void remove(Track *track);

            /**
             * Returns the currently soloed @ref Track.
             *
             * A value of -1 means that no @ref Track is soloed.
             *
             * By default, no @ref Track is soloed.
             *
             * @returns Current solo @ref Track (or -1 for none)
             * @see     setSoloTrack
             */
            int soloTrack() const;

            /**
             * Set the solo @ref Track.
             *
             * @param t New solo track (or -1 to disable solo)
             * @see   soloTrack
             */
            void setSoloTrack(int t);

            /**
             * Returns whether the repeat facility is enabled.
             *
             * When repeat is on, the @ref Transport object will loop
             * playback from the @ref to to the @ref from positions.
             *
             * By default, repeat is disabled.
             *
             * @return Whether repeat is enabled
             * @see    setRepeat
             * @see    from
             * @see    to
             */
            bool repeat() const;

            /**
             * Sets the repeat modes status.
             *
             * @param status Whether to repeat
             * @see   repeat
             */
            void setRepeat(bool status);

            /**
             * Return the 'from' value. This is the left repeat marker
             * position. When repeat is enabled in the @ref Transport object
             * time will loop back to here from the @ref to position.
             *
             * @return The 'from' value
             * @see    to
             * @see    setFrom
             */
            Clock from() const;

            /**
             * Sets the 'from' value.
             *
             * @param from New 'from' value
             * @see   from
             */
            void setFrom(Clock from);

            /**
             * Return the 'to' value. This is the right repeat marker
             * position. When repeat is enabled in the @ref Transport object
             * time will loop back from here from the @ref from position.
             *
             * @return The 'to' value
             * @see    from
             * @see    setTo
             */
            Clock to() const;

            /**
             * Sets the 'to' value.
             *
             * @param to New 'to' value
             * @see   to
             */
            void setTo(Clock to);

            /**
             * @reimplemented
             */
            virtual void Track_PartInserted(Track *, Part *);

            /**
             * @reimplemented
             */
            virtual void Track_PartRemoved(Track *, Part *);

            /**
             * @reimplemented
             */
            virtual void Notifier_Deleted(Track *);

            /**
             * @reimplemented
             */
            virtual PlayableIterator *iterator(Clock index);

            /**
             * @reimplemented
             *
             * This value is cached, not recalculated each time.
             */
            virtual Clock lastClock() const;

            /**
             * @reimplemented
             *
             * @sect Important note
             *
             * You do not have to use this method directly, instead use the
             * @ref TSE3MDL class which takes care of the entire load/save
             * process.
             */
            virtual void save(std::ostream &o, int i) const;

            /**
             * @reimplemented
             *
             * @sect Important note
             *
             * You do not have to use this method directly, instead use the
             * @ref TSE3MDL class which takes care of the entire load/save
             * process.
             */
            virtual void load(std::istream &in, SerializableLoadInfo &info);

        private:

            Song &operator=(const Song &);
            Song(const Song &);

            void recalculateLastClock() const;

            class SongImpl *pimpl;
    };
}

#endif
