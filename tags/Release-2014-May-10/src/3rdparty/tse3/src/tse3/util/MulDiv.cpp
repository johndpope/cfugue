/*
 * @(#)Phrase.cpp 3.00 4 February 2002
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

#include "tse3/util/MulDiv.h"

using namespace TSE3::Util;

int TSE3::Util::muldiv(int val, int num, int div)
{
    int acc_d, acc_r, val_d, val_r;

    acc_d = acc_r = 0;
    val_d = num / div;
    val_r = num % div;
    while (val > 0)
    {
        if (val & 1)
        {
            acc_d += val_d;
            acc_r += val_r;
            if (acc_r >= div)
            {
                acc_r -= div;
                ++acc_d;
            }
        }
        val   >>= 1;
        val_d <<= 1;
        val_r <<= 1;
        if (val_r >= div)
        {
            val_r -= div;
            ++val_d;
        }
    }
    if (acc_r >= (div >> 1))
    {
        ++acc_d;
    }
    return acc_d;
}


