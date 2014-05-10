/*
 * @(#)app/TrackSelection.h 1.00 17 January 2001
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

#ifndef TSE3_APP_TRACKSELECTION_H
#define TSE3_APP_TRACKSELECTION_H

#include "tse3/listen/app/TrackSelection.h"

#include "tse3/Track.h"
#include "tse3/listen/app/PartSelection.h"
#include "tse3/Notifier.h"

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
         * The TrackSelection class allows an Application to provide the
         * facility to 'select' one or more @ref TSE3::Track in a
         * @ref TSE3::Song. These @ref TSE3::Track objects must be held in a
         * @ref TSE3::Song. As soon as a @ref TSE3::Track is removed from a
         * @ref TSE3::Song, it is removed from the selection.
         *
         * This behviour is useful when using the command history system in
         * the @ref TSE3::Cmd namespace where undoing a @ref TSE3::Track add,
         * for example, may cause a selected Track to be removed from the @ref
         * TSE3::Song.
         *
         * Internally, the Tracks are held in an STL vector of type <Track*>.
         * In order to get access to the contents of this vector, access
         * is provided to it's iterator.
         *
         * If you are using the iterators and performing any operations that
         * may affect the TrackSelection (i.e. removing a @ref TSE3::Track from
         * it's @ref TSE3::Track and reinserting it elsewhere) then you
         * will invalidate the iterator.
         *
         * A method of avoiding this would be to, rather than iterate over each
         * Track, to remove a Track from the head of the list, work on it,
         * and place it into a second TrackSelection. Do this for each Track
         * in the TrackSelection, and when finished, copy the new TrackSelection
         * into the old one.
         *
         * @sect Interaction with @ref PartSelection
         *
         * You may want to use this selection utility in connection with the
         * @ref PartSelection class to ensure that the user has only selected
         * one type of object at any time.
         *
         * To facilitate this, the TrackSelection is a
         * @ref PartSelectionListener. You can attach it to a
         * @ref PartSelection object, and whenever the @ref PartSelection
         * has a @ref TSE3::Part selected the TrackSelection will be cleared.
         *
         * You will need to use an attach like this:
         * <pre>
         *   TrackSelection  ts;
         *   PartSelection  *ps = somePartSelection();
         *   ts.TSE3::Listener<TSE3::App::PartSelectionListener>::attachTo(ps);
         * </pre>
         *
         * @short   Support class for selecting one or more @ref Track object
         * @author  Pete Goodliffe
         * @version 1.00
         * @see     TSE3
         */
        class TrackSelection : public TSE3::Listener<TSE3::TrackListener>,
                               public TSE3::Listener<PartSelectionListener>,
                               public TSE3::Notifier<TrackSelectionListener>
        {
            public:

                /**
                 * The TrackSelection begins with no @ref Track selected.
                 */
                TrackSelection();
                ~TrackSelection();
                TrackSelection(const TrackSelection &);

                /**************************************************************
                 * Setting the selection
                 *************************************************************/

                /**
                 * Selects a @ref TSE3::Track.
                 *
                 * If the @ref TSE3::Track is not in a @ref TSE3::Song, it
                 * will not be selected.
                 *
                 * @param track The @ref Track to select
                 * @param add   Whether to add the @ref TSE3::Track to the
                 *              selection (true) or to replace the current
                 *              selection (false).
                 */
                void select(TSE3::Track *track, bool add);

                /**
                 * Deselects a @ref Track. If the @ref Track is not selected,
                 * then nothing will happen.
                 *
                 * @param track The @ref Track to deselect
                 */
                void deselect(TSE3::Track *track);

                /**
                 * Clears the entire selection.
                 */
                void clear();

                /**
                 * Selects all the @ref TSE3::Track objects in the
                 * given @ref TSE3::Song (this 'should' include any Tracks
                 * alreadys selected!).
                 *
                 * @param song @ref TSE3::Song whose @ref TSE3::Track objects
                 *             are all to be selected
                 */
                void selectAll(TSE3::Song *song);

                /**
                 * Inverts the selection for all the @ref TSE3::Track objects in
                 * the given @ref TSE3::Song (i.e. all selected Tracks are
                 * deselected and all deselected Tracks are selected).
                 *
                 * @param song @ref TSE3::Song whose @ref TSE3::Track objects
                 *             are all to be inverted
                 */
                void invert(TSE3::Song *song);

                /**
                 * Copy TrackSelection contents.
                 */
                TrackSelection &operator=(const TrackSelection &);

                /**************************************************************
                 * Access to the selected Tracks
                 *************************************************************/

                /**
                 * Returns how many @ref TSE3::Track objects are selected.
                 *
                 * @return No of selected @ref TSE3::Track objects
                 */
                size_t size() const { return tracks.size(); }

                /**
                 * Returns whether a particular @ref TSE3::Track is selected.
                 *
                 * @param  track The @ref TSE3::Track to run selection test on
                 * @return Whether the @ref TSE3::Track is selected
                 */
                bool isSelected(TSE3::Track *track) const;

                typedef std::vector<TSE3::Track*>::const_iterator iterator_t;

                /**
                 * Returns the first item in the TrackSelection, or zero if
                 * there are no selected Tracks.
                 *
                 * @return First selected @ref TSE3::Track
                 */
                Track *front() const
                {
                    return tracks.size() ? tracks.front() : 0;
                }

                /**
                 * Returns the last item in the TrackSelection, or zero if
                 * there are no selected Tracks.
                 *
                 * @return Last selected @ref TSE3::Track
                 */
                Track *back() const
                {
                    return tracks.size() ? tracks.back() : 0;
                }

                /**
                 * Returns an iterator pointing to the first @ref TSE3::Track
                 * in this selection.
                 *
                 * @return Iterator pointing to first selected @ref TSE3::Track
                 */
                iterator_t begin() const { return tracks.begin(); }

                /**
                 * Returns an iterator pointing to the last @ref TSE3::Track in
                 * this selection.
                 *
                 * @return Iterator pointing to last selected @ref TSE3::Track
                 */
                iterator_t end() const { return tracks.end(); }

                /**
                 * @reimplemented
                 */
                virtual void Track_Reparented(Track *);

                /**
                 * @reimplemented
                 */
                virtual void Notifier_Deleted(Track *);

                /**
                 * @reimplemented
                 */
                virtual void PartSelection_Selected
                    (PartSelection *, TSE3::Part *, bool);

            private:

                /**
                 * Adds the @ref TSE3::Track to the vector and attaches the
                 * TrackSelection to it.
                 */
                void addTrack(TSE3::Track *track);

                /**
                 * Removes the @ref TSE3::Track from the vector and detaches the
                 * TrackSelection from it.
                 */
                void removeTrack(TSE3::Track *part);

                /**
                 * Recalculates minTrack and maxTrack;
                 */
                void recalculateEnds();

                std::vector<TSE3::Track*>  tracks;
                bool                       tracksValid;
                TSE3::Track               *minTrack;
                TSE3::Track               *maxTrack;
        };
    }
}

#endif
