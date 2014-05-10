/*
 * @(#)listen/Part.h 3.00 25 May 1999
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

#ifndef TSE3_LISTEN_PART_H
#define TSE3_LISTEN_PART_H

#include "tse3/Midi.h"

namespace TSE3
{
    class Part;
    class Phrase;

    /**
     * @ref Part listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class PartListener
    {
        public:
            typedef Part notifier_type;

            virtual void Part_StartAltered(Part *, Clock /*start*/)      {}
            virtual void Part_EndAltered(Part *, Clock /*end*/)          {}
            virtual void Part_RepeatAltered(Part *, Clock /*repeat*/)    {}
            virtual void Part_PhraseAltered(Part *, Phrase * /*phrase*/) {}
            virtual void Part_Reparented(Part *)                         {}

            virtual void Part_MidiFilterAltered(Part *, int /*what*/)    {}
            virtual void Part_MidiParamsAltered(Part *, int /*what*/)    {}
            virtual void Part_DisplayParamsAltered(Part *)               {}
    };
}

#endif
