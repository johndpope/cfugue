/*
 * @(#)FlagTrack.cpp 3.00 26 April 2002
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

#include "tse3/cmd/FlagTrack.h"

using namespace TSE3;
using namespace TSE3::Cmd;

/******************************************************************************
 * FlagTrack_Add class
 *****************************************************************************/

FlagTrack_Add::FlagTrack_Add(TSE3::FlagTrack               *flagTrack,
                             const TSE3::Event<TSE3::Flag> &flag)
: Command("add flag"), flagTrack(flagTrack), flag(flag), insertIndex(0)
{
}


void FlagTrack_Add::executeImpl()
{
    insertIndex = flagTrack->insert(flag);
}


void FlagTrack_Add::undoImpl()
{
    flagTrack->erase(insertIndex);
}

