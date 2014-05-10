/*
 * @(#)listen/FlagTrack.h 3.00 19 May 1999
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

#ifndef TSE3_LISTEN_FLAGTRACK_H
#define TSE3_LISTEN_FLAGTRACK_H

#include "tse3/listen/EventTrack.h"

namespace TSE3
{
    class Flag;
    class FlagTrack;

    typedef EventTrackListener<Flag> FlagTrackListener;
}

#endif
