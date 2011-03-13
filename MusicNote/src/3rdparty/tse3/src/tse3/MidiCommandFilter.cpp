/*
 * @(#)MidiCommandFilter.cpp 3.00 25 Dec 1999
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

#include "tse3/MidiCommandFilter.h"

#include "tse3/Midi.h"
#include "tse3/Mutex.h"

using namespace TSE3;

/******************************************************************************
 * MidiCommandFilter class
 *****************************************************************************/

bool MidiCommandFilter::filter(MidiCommand type) const
{
    Impl::CritSec cs;

    int index = type.status - MidiCommand_NoteOn;
    if (index < 0) index = 0;
    return 0x1 & (_filter >> index);
}


void MidiCommandFilter::setFilterStatus(MidiCommand type,
                                        const bool status)
{
    Impl::CritSec cs;

    int index = type.status - MidiCommand_NoteOn;
    if (index < 0) index = 0;
    _filter &= 0xff - (1 << index);
    if (status)
    {
        _filter |= (1 << index);
    }
    notify(&MidiCommandFilterListener::MidiCommandFilter_Altered);
}

