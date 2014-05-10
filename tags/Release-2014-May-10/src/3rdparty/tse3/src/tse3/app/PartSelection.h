/*
 * @(#)app/PartSelection.h 1.00 18 June 2000
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

#ifndef TSE3_APP_PARTSELECTION_H
#define TSE3_APP_PARTSELECTION_H

#include "tse3/listen/app/PartSelection.h"

#include "tse3/Notifier.h"
#include "tse3/Part.h"
#include "tse3/Midi.h"
#include "tse3/listen/app/TrackSelection.h"

#include <vector>
#include <cstddef>

namespace TSE3
{
    class Phrase;
    class Song;
    class Track;

    namespace App
    {
        /**
         * The PartSelection class allows an Application to provide the facility
         * to 'select' one or more @ref TSE3::Part in a @ref TSE3::Song. These
         * @ref TSE3::Part objects must be held in a @ref TSE3::Track. As soon
         * as a @ref TSE3::Part is removed from a @ref TSE3::Track, it is
         * removed from the selection.
         *
         * This behviour is useful when using the command history system in
         * the @ref TSE3::Cmd namespace where undoing a @ref TSE3::Part add,
         * for example, may cause a selected Part to be removed from the @ref
         * TSE3::Song.
         *
         * Internally, the Parts are held in an STL vector of type <Part*>.
         * In order to get access to the contents of this vector, access
         * is provided to it's iterator.
         *
         * If you are using the iterators and performing any operations that
         * may affect the PartSelection (i.e. removing a @ref TSE3::Part from
         * it's @ref TSE3::Track and reinserting it elsewhere) then you
         * will invalidate the iterator.
         *
         * A method of avoiding this would be to, rather than iterate over each
         * Part, to remove a Part from the head of the list, work on it,
         * and place it into a second PartSelection. Do this for each Part
         * in the PartSelection, and when finished, copy the new PartSelection
         * into the old one.
         *
         * @sect Interaction with @ref TrackSelection
         *
         * You may want to use this selection utility in connection with the
         * @ref TrackSelection class to ensure that the user has only selected
         * one type of object at any time.
         *
         * To facilitate this, the PartSelection is a
         * @ref TrackSelectionListener. You can attach it to a
         * @ref TrackSelection object, and whenever the @ref TrackSelection
         * has a @ref TSE3::Track selected the PartSelection will be cleared.
         *
         * You will need to use an attach like this:
         * <pre>
         *   PartSelection   ps;
         *   TrackSelection *ts = someTrackSelection();
         *   ps.TSE3::Listener<TSE3::App::TrackSelectionListener>::attachTo(ts);
         * </pre>
         *
         * @short   Support class for selecting one or more @ref Part object
         * @author  Pete Goodliffe
         * @version 1.00
         * @see     TSE3
         */
        class PartSelection : public TSE3::Listener<TSE3::PartListener>,
                              public TSE3::Listener<TrackSelectionListener>,
                              public TSE3::Notifier<PartSelectionListener>
        {
            public:

                /**
                 * The PartSelection begins with no @ref Part selected.
                 */
                PartSelection();
                ~PartSelection();
                PartSelection(const PartSelection &);

                /**************************************************************
                 * Setting the selection
                 *************************************************************/

                /**
                 * Returns the earliest start time of any @ref TSE3::Part in the
                 * selection.
                 *
                 * If there is no selection, this will return -1.
                 *
                 * @return Earliest @ref TSE3::Part start time
                 */
                TSE3::Clock earliest() const { return _earliest; }

                /**
                 * Returns the latest end time of any @ref TSE3::Part in the
                 * selection.
                 *
                 * If there is no selection, this will return -1.
                 *
                 * @return Latest @ref TSE3::Part start time
                 */
                TSE3::Clock latest() const { return _latest; }

                /**
                 * Returns the lowest @ref TSE3::Track index that any selected
                 * @ref TSE3::Part is in.
                 *
                 * If there is no selection, this will return 0.
                 *
                 * @return Lowest @ref TSE3::Track index.
                 */
                size_t minTrack() const { return _minTrack; }

                /**
                 * Returns the greatest @ref TSE3::Track index that any selected
                 * @ref TSE3::Part is in.
                 *
                 * If there is no selection, this will return 0.
                 *
                 * @return Greatest @ref TSE3::Track index.
                 */
                size_t maxTrack() const { return _maxTrack; }

                /**
                 * Selects a @ref TSE3::Part.
                 *
                 * If the @ref TSE3::Part is not in a @ref TSE3::Track, it
                 * will not be selected.
                 *
                 * @param part The @ref Part to select
                 * @param add  Whether to add the @ref TSE3::Part to the
                 *             selection (true) or to replace the current
                 *             selection (false).
                 */
                void select(TSE3::Part *part, bool add);

                /**
                 * Deselects a @ref Part. If the @ref Part is not selected,
                 * then nothing will happen.
                 *
                 * @param part The @ref Part to deselect
                 */
                void deselect(TSE3::Part *part);

                /**
                 * Clears the entire selection.
                 */
                void clear();

                /**
                 * Selects all the @ref TSE3::Part objects in the
                 * given @ref TSE3::Song (this 'should' include any Parts
                 * alreadys selected!).
                 *
                 * @param song @ref TSE3::Song whose @ref TSE3::Part objects
                 *             are all to be selected
                 */
                void selectAll(TSE3::Song *song);

                /**
                 * Adds to the selection all the @ref TSE3::Part objects in the
                 * given @ref TSE3::Track.
                 *
                 * @param track @ref TSE3::Track whose @ref TSE3::Part objects
                 *              are all to be selected
                 */
                void selectAll(TSE3::Track *track);

                /**
                 * Inverts the selection for all the @ref TSE3::Part objects in
                 * the given @ref TSE3::Song (i.e. all selected Parts are
                 * deselected and all deselected Parts are selected).
                 *
                 * @param song @ref TSE3::Song whose @ref TSE3::Part objects
                 *             are all to be inverted
                 */
                void invert(TSE3::Song *song);

                /**
                 * Adds to the selection all the Parts which have a section
                 * lying either inside or outside the window between a
                 * @p start time and an @p end time.
                 *
                 * @param song   @ref TSE3::Song whose @ref TSE3::Part objects
                 *               are all to be operated on
                 * @param start  Start of selection window
                 * @param end    End of selection window
                 * @param inside If true, select @ref TSE3::Part objects between
                 *               @p start and @p end, else select Parts NOT
                 *               between these times
                 */
                void selectBetween(TSE3::Song *song,
                                   TSE3::Clock start, TSE3::Clock end,
                                   bool inside = true);

                /**
                 * Adds to the selection all the Parts which have a section
                 * lying either inside or outside the window between a
                 * @p start time and an @p end time.
                 *
                 * @param track  @ref TSE3::Track whose @ref TSE3::Part objects
                 *               are all to be operated on
                 * @param start  Start of selection window
                 * @param end    End of selection window
                 * @param inside If true, select @ref TSE3::Part objects between
                 *               @p start and @p end, else select Parts NOT
                 *               between these times
                 */
                void selectBetween(TSE3::Track *track,
                                   TSE3::Clock start, TSE3::Clock end,
                                   bool inside = true);

                /**
                 * Copy PartSelection contents.
                 */
                PartSelection &operator=(const PartSelection &);

                /**************************************************************
                 * Access to the selected Parts
                 *************************************************************/

                /**
                 * Returns how many @ref TSE3::Part objects are selected.
                 *
                 * @return No of selected @ref TSE3::Part objects
                 */
                size_t size() const { return parts.size(); }

                /**
                 * Returns whether a particular @ref TSE3::Part is selected.
                 *
                 * @param  part The @ref TSE3::Part to run selection test on
                 * @return Whether the @ref TSE3::Part is selected
                 */
                bool isSelected(TSE3::Part *part) const;

                typedef std::vector<TSE3::Part*>::const_iterator iterator_t;

                /**
                 * Returns the first item in the PartSelection, or zero if
                 * there are no selected Parts.
                 *
                 * @return First selected @ref TSE3::Part
                 */
                Part *front() const
                {
                    return parts.size() ? parts.front() : 0;
                }

                /**
                 * Returns the last item in the PartSelection, or zero if
                 * there are no selected Parts.
                 *
                 * @return Last selected @ref TSE3::Part
                 */
                Part *back() const
                {
                    return parts.size() ? parts.back() : 0;
                }

                /**
                 * Returns an iterator pointing to the first @ref TSE3::Part in
                 * this selection.
                 *
                 * @return Iterator pointing to first selected @ref TSE3::Part
                 */
                iterator_t begin() const { return parts.begin(); }

                /**
                 * Returns an iterator pointing to the last @ref TSE3::Part in
                 * this selection.
                 *
                 * @return Iterator pointing to last selected @ref TSE3::Part
                 */
                iterator_t end() const { return parts.end(); }

                /**
                 * @reimplemented
                 */
                virtual void Part_StartAltered(Part *, Clock start);

                /**
                 * @reimplemented
                 */
                virtual void Part_EndAltered(Part *, Clock end);

                /**
                 * @reimplemented
                 */
                virtual void Part_Reparented(Part *);

                /**
                 * @reimplemented
                 */
                virtual void Notifier_Deleted(Part *);

                /**
                 * @reimplemented
                 */
                virtual void TrackSelection_Selected
                    (TrackSelection *, TSE3::Track *, bool);

            private:

                /**
                 * Adds the @ref TSE3::Part to the vector and attaches the
                 * PartSelection to it, then updates the _earliest and _latest
                 * values.
                 */
                void addPart(TSE3::Part *part);

                /**
                 * Removes the @ref TSE3::Part from the vector and detaches the
                 * PartSelection from it, then updates the _earliest and _latest
                 * values.
                 */
                void removePart(TSE3::Part *part);

                /**
                 * Recalculates _earliest and _latest;
                 */
                void recalculateEnds();

                std::vector<TSE3::Part*> parts;
                bool        timesValid;
                TSE3::Clock _earliest;
                TSE3::Clock _latest;
                bool        tracksValid;
                size_t      _minTrack;
                size_t      _maxTrack;
        };
    }
}

#endif
