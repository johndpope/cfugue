/*
 * @(#)Playable.cpp 3.00 1 June 1999
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

#include "tse3/Playable.h"

using namespace TSE3;

PlayableIterator::PlayableIterator()
: _next(MidiEvent()), _more(false)
{
}


PlayableIterator::~PlayableIterator()
{
}


PlayableIterator &PlayableIterator::operator++()
{
    if (_more) getNextEvent();
    return *this;
}

