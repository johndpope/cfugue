/*
 * @(#)Part.h 3.00 25 May 1999
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

#ifndef TSE3_PART_H
#define TSE3_PART_H

#include "tse3/listen/Part.h"

#include "tse3/Playable.h"
#include "tse3/Serializable.h"
#include "tse3/listen/Phrase.h"
#include "tse3/listen/MidiFilter.h"
#include "tse3/listen/MidiParams.h"
#include "tse3/listen/DisplayParams.h"
#include "tse3/Notifier.h"

namespace TSE3
{
    class Part;
    class Track;

    /**
     * The Part class represents a placement of a @ref Phrase in a @ref Track.
     * It holds a reference to the @ref Phrase and the realtime parameters that
     * affect how the @ref Phrase sounds as it is played.
     *
     * A Part can only exist in one @ref Track at once.
     *
     * Each Part has a start and end time, which it manages. Before you insert
     * a Part in a @ref Track ensure that you have set the appropriate times
     * with @ref setStart() and @ref setEnd() or with the consolidated
     * @ref setStartEnd().
     *
     * The Part has a @ref DisplayParameters object associated with it which
     * may be used by an application to redraw the Part on screen (this use is
     * optional). If you do use it, the Part @ref DisplayParams should
     * superceed any @ref Phrase @ref DisplayParams.
     *
     * @sect Command classes
     *
     * Use the following command classes to manipute this object in a undo/redo
     * environment.
     *
     *     @li @ref TSE3::Cmd::Part_SetInfo
     *     @li @ref TSE3::Cmd::Part_SetPhrase
     *     @li @ref TSE3::Cmd::Part_Move
     *     @li @ref TSE3::Cmd::Track_Snip
     *     @li @ref TSE3::Cmd::Track_Glue
     *     @li @ref TSE3::Cmd::Track_RemovePart
     *
     * @short   A placement of a Phrase in a Track
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     MidiEvent
     * @see     Phrase
     * @see     PhraseEdit
     */
    class Part : public Playable,
                 public Listener<PhraseListener>,
                 public Listener<MidiFilterListener>,
                 public Listener<MidiParamsListener>,
                 public Listener<DisplayParamsListener>,
                 public Serializable,
                 public Notifier<PartListener>
    {
        public:

            /**
             * Creates a Part that uses no @ref Phrase. Set the @ref Phrase
             * with the @ref setPhrase method.
             *
             * The start and end points are set to a default. Set them with
             * @ref setStart and @ref setEnd.
             */
            Part();

            /**
             * Creates a Part that uses no @ref Phrase and the given start
             * and end points.
             */
            Part(Clock start, Clock end);

            Part(const Part &p);
            virtual ~Part();

            Part &operator=(const Part &);

            /**
             * @ref MidiFilter that affects how @ref MidiEvents are
             * produced by this Part.
             *
             * @return The Part's @ref MidiFilter object
             */
            MidiFilter *filter();

            /**
             * The Part parameters that affects how @ref MidiEvents are
             * produced by this Part.
             *
             * @return The Part's @ref MidiParams object
             */
            MidiParams *params();

            /**
             * The display parameters.
             *
             * @return The Part's @ref DisplayParams object
             */
            DisplayParams *displayParams();

            /**
             * Returns the @ref Phrase this Part uses (or 0 if there is no
             * current @ref Phrase).
             *
             * @return The Part's @ref Phrase
             * @see    setPhrase
             */
            Phrase *phrase() const;

            /**
             * Sets the @ref Phrase this Part uses.
             *
             * The @ref Phrase must be contained in the correct @ref PhraseList
             * for this operation to work. If the Phrase subequently is
             * removed from a @ref PhraseList the Part's @ref Phrase pointer
             * is reset.
             *
             * You may specify a value of 0 for "use no @ref Phrase".
             *
             * @see phrase
             * @throws PartError
             */
            void setPhrase(Phrase *p);

            /**
             * Returns a pointer to the parent @ref Track that this
             * Part has been inserted into. If this Part has not been
             * inserted in a @ref Track, this method returns zero.
             *
             * @return @ref Track this Part is inserted in
             */
            Track *parent() const;

            /**
             * Returns the start time of this Part.
             *
             * This is meerly held for the convenience of the parent and
             * is not used by the Part itself. In particular, it does not alter
             * the relative times of the MidiEvents produced through the
             * Playable interface: they remain relative to time zero.
             *
             * @return Part's start time
             * @see    end
             * @see    setStart
             */
            Clock start() const;

            /**
             * Sets the start time of this Part.
             *
             * If the Part is inserted in a @ref Track then the end time cannot
             * be changed to before the start time.
             *
             * Note that if the Part is in a @ref Track and the change would
             * cause an exception to be thrown by the @ref Track's insert
             * method then that exception will propagate from this method.
             * However you can be assured that if this happens the Part will be
             * left with times set as they were when you called the method.
             *
             * @param  c      New start time
             * @see    start
             * @see    setStartEnd
             * @throws PartError
             * @throws TrackError
             */
            void setStart(Clock c);

            /**
             * Returns the end time of this Part.
             *
             * This is meerly held for the convenience of the parent and
             * is not used by the Part itself.
             *
             * @return Part's end time
             * @see    start
             * @see    setEnd
             */
            Clock end() const;

            /**
             * Sets the end time of this Part.
             *
             * If the Part is inserted in a @ref Track then the end time cannot
             * be changed to before the start time.
             *
             * Note that if the Part is in a @ref Track and the change would
             * cause an exception to be thrown by the @ref Track's insert
             * method then that exception will propagate from this method.
             * However you can be assured that if this happens the Part will be
             * left with times set as they were when you called the method.
             *
             * @param  c      New end time
             * @see    end
             * @see    setStartEnd
             * @throws PartError
             * @throws TrackError
             */
            void setEnd(Clock c);

            /**
             * Sets the start and end times of this Part.
             *
             * You may wish to use this when the Part is inserted in a
             * @ref Track and you want to move the Part in such a way
             * that calling @ref setStart or @ref setEnd first would throw
             * an exception.
             *
             * Note that if the Part is in a @ref Track and the change would
             * cause an exception to be thrown by the @ref Track's insert
             * method then that exception will propagate from this method.
             * However you can be assured that if this happens the Part will be
             * left with times set as they were when you called the method.
             *
             * @param  start  New start time
             * @param  end    New end time
             * @see    setStart
             * @see    setEnd
             * @throws PartError
             * @throws TrackError
             */
            void setStartEnd(Clock start, Clock end);

            /**
             * Returns the repeat time of this Part.
             *
             * A repeat time of zero means no repeat.
             *
             * @return Repeat time value
             * @see    setRepeat
             */
            Clock repeat() const;

            /**
             * Sets the repeat time of this Part.
             *
             * @param r New repeat time value
             * @see   repeat
             */
            void setRepeat(Clock r);

            /**
             * @reimplemented
             */
            virtual void Notifier_Deleted(Phrase *phrase);

            /**
             * @reimplemented
             */
            virtual void Phrase_Reparented(Phrase *);

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
            virtual void MidiFilter_Altered(MidiFilter *, int what);

            /**
             * @reimplemented
             */
            virtual void MidiParams_Altered(MidiParams *, int what);

            /**
             * @reimplemented
             */
            virtual void DisplayParams_Altered(DisplayParams *);

            /**
             * The @ref Track class is a friend so that it can access the
             * @ref setParentTrack() method.
             */
            friend class Track;

        private:

            /**
             * This method is called by the @ref Track class alone. It is used
             * to set the parent @ref Track pointer, _track. This will be set
             * to the parent's pointer, or to zero if the Part is removed
             * from the @ref Track.
             *
             * This is the only private member the @ref Track class accesses.
             *
             * @param parent @ref Track object that this Part has been inserted
             *               into.
             */
            void setParentTrack(Track *parent);

            class PartImpl *pimpl;
    };
}

#endif
