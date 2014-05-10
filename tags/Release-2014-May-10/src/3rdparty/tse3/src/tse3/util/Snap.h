/*
 * @(#)util/Utilities.h 3.00 20 May 1999
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

#ifndef TSE3_UTIL_SNAP_H
#define TSE3_UTIL_SNAP_H

#include "tse3/Midi.h"

namespace TSE3
{
    class TimeSigTrack;

    namespace Util
    {
        /**
         * Time 'snapping' services: snapping a given time to the next lowest
         * point at a given resolution.
         *
         * This class uses the @ref TimeSigTrack class to work out where bars
         * start/stop.
         *
         * @short   Time 'snapping'
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Clock
         * @see     TimeSigTrack
         */
        class Snap
        {
            public:

                /**
                 * The Snap object defaults to snapping to the previous bar.
                 *
                 * @param t @ref TSE3::TimeSigTrack to use.
                 *          Specifying zero will provide no snap at all.
                 */
                Snap(TSE3::TimeSigTrack *t) : tst(t), _snap(-1) {}

                /**
                 * Changes the @ref TSE3::TimeSigTrack to use.
                 *
                 * @param t @ref TSE3::TimeSigTrack to use.
                 *          Specifying zero will provide no snap at all.
                 */
                void setTimeSigTrack(TSE3::TimeSigTrack *t) { tst = t; }

                /**
                 * Returns the current snap value. A value of -1 means snap to
                 * the previous bar.
                 *
                 * @see setSnap
                 */
                TSE3::Clock snap() const { return _snap; }

                /**
                 * Sets the snap value.
                 *
                 * @see snap
                 */
                void setSnap(TSE3::Clock s) { _snap = s; }

                /**
                 * Perform a 'snap': return the snapped value of the given
                 * @ref Clock.
                 */
                TSE3::Clock operator()(TSE3::Clock c) const;

            private:

                TSE3::TimeSigTrack *tst;
                TSE3::Clock         _snap;
        };
    }
}

#endif
