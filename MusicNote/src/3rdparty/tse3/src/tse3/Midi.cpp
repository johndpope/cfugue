/*
 * @(#)Midi.cpp 3.00 9 September 1999
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

#include "tse3/Midi.h"

//const int TSE3::Clock::PPQN;

unsigned int TSE3::MidiCommand_NoDataBytes[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, // These are all meta events, which we'll ignore ATM
    2, 2, 2, 2, 1, 1, 2, 0
};
