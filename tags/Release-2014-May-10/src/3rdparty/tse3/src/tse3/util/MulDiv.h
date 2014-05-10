/*
 * @(#)util/MulDiv.h 3.00 4 February 2002
 *
 * Copyright (c) 2002 Pete Goodliffe (pete@cthree.org)
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

#ifndef TSE3_UTIL_MULDIV_H
#define TSE3_UTIL_MULDIV_H

namespace TSE3
{
    namespace Util
    {
        /**
         * A utility function for doing muldiv calculations very quickly on
         * platforms with no floating point support. It performs
         * a multiplication and a divison. It used because most systems have 32
         * bit integers and we want multiplication/division that can cope with
         * overflows.
         *
         * @param  val Value to perform muldiv on
         * @param  num Numerator
         * @param  div Divisor
         * @return val * num / div (which coped with integer overflows)
         */
        extern int muldiv(int val, int num, int div);
    }
}

#endif
