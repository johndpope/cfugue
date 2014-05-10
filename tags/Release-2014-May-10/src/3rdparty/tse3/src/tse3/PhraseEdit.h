/*
 * @(#)PhraseEdit.h 3.00 19 May 1999
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

#ifndef TSE3_PHRASEEDIT_H
#define TSE3_PHRASEEDIT_H

#include "tse3/listen/PhraseEdit.h"

#include "tse3/Notifier.h"
#include "tse3/MidiData.h"

#include <string>
#include <cstddef>

namespace TSE3
{
    class Phrase;
    class PhraseList;

    /**
     * This class provides an editable @ref MidiData area. It is the mechanism
     * used to generate @ref Phrase objects.
     *
     * The PhraseEdit object can be used as an area into which @ref MidiEvent
     * data is recorded in real time (it is used by the @ref Transport class),
     * or as the basic data structure for a GUI @ref Phrase editor, for example.
     *
     * For this latter mode of usage, the PhraseEdit class provides a number
     * of callbacks (see @ref PhraseEditListener) that can be used to keep the
     * display updated.
     *
     * @sect Tidiness
     *
     * The state of the PhraseEdit area can be one of:
     *
     * @li Tidy:   Sustain pedal usage has been converted into sustained notes
     *             and MidiCommand_NoteOffs are coalescened into their
     *             appropriate @ref MidiEvent. The data in the PhraseEdit
     *             object is suitable for being played through the
     *             @ref Playable interface.
     * @li Untidy: MidiCommand_NoteOffs may not be matched into MidiEvents,
     *             sustain pedal events may not have been cleaned up, events
     *             could be out of chronological order. Basically, the data is
     *             likely to me a big mess and all hell could break loose if
     *             you try to play this @ref MidiData in it's current state.
     *             (All hell breaking loose is likely by be 'hanging notes'.)
     *
     * It is up to the user to know if a PhraseEdit area may be untidy and to
     * deal with it appropriately (i.e. make sure it is never played). After
     * having tidied the data (by calling @ref tidy()) the only way to make
     * the data untidy is to write a @ref MidiEvent directly using
     * @ref operator[]().
     *
     * When the PhraseEdit is tidy, the safest way of modifying a
     * @ref MidiEvent is to remove it and the reinsert the modified
     * @ref MidiEvent.
     *
     * @sect Managing selected @ref MidiEvent contents
     *
     * The PhraseEdit object can manage the selection status of it's @ref
     * MidiEvent contents. This will be useful for GUI editors that use the
     * PhraseEdit as the underlying data buffer.
     *
     * Selections are denoted by the @p selected bit in the @ref MidiCommand
     * data type. You can use @ref select() and @ref deselect() to alter
     * selection states.
     *
     * For convenience the PhraseEdit class remembers the index of the first
     * and last selected @ref MidiEvent.
     *
     * Working with selections only really makes sense when the data area
     * is tidy. You can still use the selection facilities with an untidy
     * data area, however.
     *
     * @sect Modification status
     *
     * As an extra utility for GUI uses, the PhraseEdit class manages a
     * 'modification' status. It allows the user to find out whether the
     * data has been modified since the last @ref reset(), and also fires
     * an event off the first time it is modified.
     *
     * Changing the selection does not affect the modified status.
     *
     * @sect Command classes
     *
     * Use the following command classes to manipute this object in a undo/redo
     * environment.
     *
     *     @li @ref TSE3::Cmd::Phrase_Create
     *
     * @short   An editable Phrase data type
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     MidiData
     * @see     Phrase
     */
    class PhraseEdit : public MidiData,
                       public Notifier<PhraseEditListener>
    {
        public:

            /**
             * Create a PhraseEdit with the given number of events
             * space reserved.
             *
             * It is not really necessary to specify this size unless you know
             * at least how many events will be inserted and reserving them
             * will result in a worthwhile performance increase.
             *
             * The PhaseEdit is initially created in an unmodified state (see
             * @ref modified()).
             *
             * @param noEvents Ignorable PhraseEdit size
             */
            explicit PhraseEdit(int noEvents = defaultSize);

            virtual ~PhraseEdit();

            /******************************************************************
             * Accessing the MidiEvents
             *****************************************************************/

            /**
             * Resets the PhraseEdit area to be a copy of the given MidiData.
             * If the @p source pointer is zero, the PhraseEdit area will be
             * blank.
             *
             * The modified status value will be cleared (see @ref modified())
             * which may result in the modified event being raised.
             *
             * @param source @ref MidiData to copy
             */
            void reset(const MidiData *source = 0);

            /**
             * Tidies the @ref MidiData (usually used after recording has
             * stopped) This does a number of things, including adding any
             * missing MidiCommand_NoteOffs and the conversion of sustain pedal
             * usage into elongated notes. It removes all events before
             * time zero (with a small tollerance)
             *
             * The data in the PhraseEdit at this point could be in any
             * state. MidiCommand_NoteOffs may be collated into the correct
             * @ref MidiEvent, or they may be in their own event (which they
             * will be when just recorded). Events may be in time order, or
             * in some completely random order.
             *
             * It also collates all MidiCommand_NoteOffs into the
             * appropriate MidiCommand_NoteOns so that the PhraseEdit
             * area will contain valid data that can be streamed out through
             * a @ref Playable interface.
             *
             * Prior to using tidy, you may need to apply @ref timeShift().
             *
             * This may cause the modified event to be raised.
             *
             * @param stopTime The time of the end of this MidiData (e.g. the
             *                 time recording stopped). If no stopTime is
             *                 specified then it is inferred from the last
             *                 event in the MidiData.
             * @see   tollerance
             */
            void tidy(Clock stopTime = -1);

            /**
             * Shift the time of every event in the PhraseEdit by @p delta
             * (the new event times are originalTime + @p delta).
             *
             * This operation can be applied to @ref tidy() or untidy
             * PhraseEdit data.
             *
             * You might want to use this method when recording @ref MidiEvent
             * data from an external source (using the @ref Transport class)
             * and you haven't started recording at the time zero. Once
             * recording has finished you can "normalise" the recorded data to
             * a reference start time of zero by calling
             * <pre>
             *     timeShift(-recordingStartTime);
             * </pre>
             *
             * If you do this, you will want to timeShift BEFORE calling
             * @ref tidy(), so that you won't get a whole slew of events
             * with minus times (which would not be "tidy" data).
             *
             * In order for a @ref Phrase to play in a @ref Part correctly, you
             * must ensure that it has a reference start time of zero, so you
             * should take care to perform this operation. (Alternatively,
             * use the @ref App::Record class which does all this for you).
             *
             * If you apply this method before calling @ref tidy, don't forget
             * to add the delta time to the @ref tidy()'s @p stopTime parameter
             * as well.
             *
             * Note that this operation can leave data untidy.
             *
             * This will cause the modified event to be raised.
             *
             * @param delta @ref Clock value to add to every event
             */
            void timeShift(Clock delta);

            /**
             * Creates a @ref Phrase from this @ref MidiData and inserts it
             * into the specified @ref PhraseList. Make sure that you have
             * done a tidy() first.
             *
             * If you don't specify a title for the new @ref Phrase then it
             * will automatically be given a name that is guaranteed not to be
             * used by any other @ref Phrase in the @ref PhraseList (you
             * can find out what it is with @ref Phrase::title()).
             *
             * If a @ref Phrase with the supplied @p title already exists in
             * the @ref PhraseList then a @ref PhraseListError is thrown, and
             * no new @ref Phrase is created.
             *
             * @param  pl    @ref PhraseList to insert the new @ref Phrase into
             *               (and from which to source a new unique name, if
             *               required)
             * @param  title New @ref Phrase's title - leave blank to have one
             *               automatically generated
             * @return New @ref Phrase object spawned from the data in this
             *         PhraseEdit
             * @throws PhraseListError
             * @see    tidy
             * @see    Phrase
             * @see    PhraseList
             */
            Phrase *createPhrase(PhraseList *pl,
                                 const std::string &title = "") const;

            /**
             * Returns the nth @ref MidiEvent in this @ref MidiData object.
             *
             * Note that, unlike the base @ref MidiData class, this object
             * is modifiable.
             *
             * The value returned for an index that is out of range is
             * undefined. The @ref size() method describes the valid
             * values.
             *
             * If you set the event directly using this method you run
             * the risk of leaving the PhraseEdit object 'untidy'.
             *
             * Any data manipulation performed via this operator will not
             * affect the modified status, and so no modified event will
             * be raised.
             *
             * In simple terms, use this method at your own risk!
             *
             * @param  n Index
             * @return @ref MidiEvent at index n
             */
            MidiEvent &operator[](size_t n) { return data[n]; }

            /**
             * Insert a @ref MidiEvent into the @ref MidiData in time order.
             *
             * @sect Important note
             *
             * If this is a @ref MidiCommand_NoteOn then you really want the
             * @ref MidiEvent to also hold the ballancing
             * @ref MidiCommand_NoteOff in its 'second half'.
             *
             * If it does then you can be assured that adding the
             * @ref MidiEvent to a 'tidied' PhraseEdit object will leave the
             * object still 'tidy'. If you only add a @ref MidiCommand_NoteOn/
             * @ref MidiCommand_NoteOff without the ballancing MIDI command
             * then the PhraseEdit is left 'untidy' and any use of it to create
             * a @ref Phrase and/or have it's data streamed out of a
             * @ref Playable interface is invalid.
             *
             * This restriction is important in the use of the TSE3 library,
             * if you don't use 'tidy' @ref MidiData then playback will produce
             * 'undefined' (very probably wrong) output.
             *
             * This may cause the modified event to be raised.
             *
             * @param MidiEvent to insert
             * @see   tidy
             */
            void insert(MidiEvent event);

            /**
             * Remove a @ref MidiEvent from the @ref MidiData. The result
             * is undefined if the index @p n is invalid.
             *
             * This may cause the modified event to be raised.
             *
             * @param n Index of MidiEvent to remove
             */
            void erase(size_t n);

            /**
             * Remove a @ref MidiEvent from the @ref MidiData. If there isn't a
             * @ref MidiEvent * matching this one then no change occurs.
             *
             * This may cause the modified event to be raised.
             *
             * @param event MidiEvent to remove
             */
            void erase(MidiEvent event);

            /******************************************************************
             * Accessing the selection
             *****************************************************************/

            /**
             * Selects the @ref MidiEvent at the specified index.
             *
             * @see deselect
             * @see selectRange
             */
            void select(size_t index);

            /**
             * Selects every @ref MidiEvent between the specified indexes.
             *
             * @see select
             * @see deselect
             */
            void selectRange(size_t from, size_t to);

            /**
             * Deselects the @ref MidiEvent at the specified index.
             *
             * @see select
             */
            void deselect(size_t index);

            /**
             * Clears any selection.
             */
            void clearSelection();

            /**
             * Inverts the selection status of every @ref MidiEvent.
             */
            void invertSelection();

            /**
             * Returns true if any @ref MidiEvent is selected.
             *
             * @return Whether any @ref MidiEvent is selected
             */
            bool selection() const { return _selection; }

            /**
             * Returns the index of the first selected @ref MidiEvent. If
             * no @ref MidiEvent has been selected, returns the value of
             * @ref size().
             *
             * @return The index of the first selected @ref MidiEvent
             */
            size_t firstSelectionIndex() const
            {
                return _selection ? _firstSelectionIndex : size();
            }

            /**
             * Returns the index of the last selected @ref MidiEvent. If
             * no @ref MidiEvent has been selected, returns the value of
             * @ref size().
             *
             * @return The index of the last selected @ref MidiEvent
             */
            size_t lastSelectionIndex() const
            {
                return _selection ? _lastSelectionIndex : size();
            }

            /**
             * Erases any @ref MidiEvent that has been selected.
             *
             * This may cause the modified event to be raised.
             */
            void eraseSelection();

            /******************************************************************
             * Accessing modification state
             *****************************************************************/

            /**
             * Returns true if the PhraseEdit has been modified since
             * creation or the last @ref reset().
             *
             * @return Whether PhraseEdit has been modified
             */
            bool modified() const { return _modified; }

            /**
             * Allows you to manually set the modified status.
             *
             * This may cause the modified event to be raised.
             *
             * @param m New modified status.
             */
            void setModified(bool m) { if (m != _modified) modified(m); }

            /******************************************************************
             * Other definitions
             *****************************************************************/

            /**
             * The default maximum no events in @ref Phrase editor. These days
             * the editor automatically grows so this is fairly redundant.
             */
            static const int defaultSize = 1024;

            /**
             * The time allowance given for notes to be before start time when
             * tidying.
             *
             * @see tidy
             */
            static const int tollerance = Clock::PPQN/2;

        private:

            PhraseEdit &operator=(const PhraseEdit &);
            PhraseEdit(const PhraseEdit &);

            /**
             * Updates the data held on selected events.
             */
            void updateSelectionInfo();

            /**
             * Updates the selection information on the grounds that the
             * @ref MidiEvent with index @index has changed to selection
             * state sel.
             *
             * This causes a notification to be sent.
             */
            void selected(size_t index, bool sel);

            /**
             * Sets the modified status and causes a notification to be sent.
             */
            void modified(bool m);

            /**
             * @p hint is a cached index that refers to the last position of
             * intertion. Since most inserts are performed in time order this
             * cached information can increase performance drastically.
             *
             * It has been shown, for example, to reduce MIDI file import
             * by at least a third.
             *
             * This used to be an iterator, however the insertion of the
             * iterator made it invalid which could lead to some freaky
             * crashes now and again.
             */
            size_t hint;

            /*
             * Selection information
             */
            bool   _selection;
            size_t _firstSelectionIndex;
            size_t _lastSelectionIndex;

            bool   _modified;

    };
}

#endif
