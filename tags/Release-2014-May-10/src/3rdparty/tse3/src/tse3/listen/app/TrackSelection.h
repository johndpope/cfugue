/*
 * @(#)listen/app/TrackSelection.h 1.00 17 January 2001
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

#ifndef TSE3_LISTEN_APP_TRACKSELECTION_H
#define TSE3_LISTEN_APP_TRACKSELECTION_H

namespace TSE3
{
    class Track;

    namespace App
    {
        class TrackSelection;

        /**
         * @ref TrackSelection listener interface.
         *
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     TSE3::Listener
         */
        class TrackSelectionListener
        {
            public:
                typedef TrackSelection notifier_type;

                /**
                 * Take note that if a selected @ref TSE3::Track is removed
                 * from a @ref TSE3::Song the TrackSelection_Selected event
                 * will pass the @p track parent without a parent, so be
                 * careful when referencing @ref TSE3::Track::parent().
                 */
                virtual void TrackSelection_Selected
                    (TrackSelection *, TSE3::Track *, bool /*selected*/) {}
        };
    }
}

#endif
