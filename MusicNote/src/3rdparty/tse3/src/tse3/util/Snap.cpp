/*
 * @(#)Snap.cpp 3.00 2 April 2000
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

#include "tse3/util/Snap.h"
#include "tse3/TimeSigTrack.h"

using namespace TSE3;
using namespace TSE3::Util;

Clock Snap::operator()(Clock c) const
{
    if (_snap == 1 || !tst) return c;

    // Get the relevant TimeSig information
    Event<TimeSig> ts = (*tst)[tst->index(c, false)];

    Clock tempSnap = (_snap == -1)
                   ? Clock(ts.data.top*4*Clock::PPQN/ts.data.bottom)
                   : _snap;
    c += tempSnap / 2;
    Clock offset = (c - ts.time)
                 % ((ts.data.top*4*Clock::PPQN) / ts.data.bottom);
    if (_snap != -1) offset %= _snap;
    return c-offset;
}
