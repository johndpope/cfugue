/*
 * @(#)listen/MidiCommandFilter.h 3.00 19 May 1999
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

#ifndef TSE3_LISTEN_MIDICOMMANDFILTER_H
#define TSE3_LISTEN_MIDICOMMANDFILTER_H

namespace TSE3
{
    class MidiCommandFilter;

    /**
     * @ref MidiCommandFilter listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class MidiCommandFilterListener
    {
        public:
            typedef MidiCommandFilter notifier_type;

            virtual void MidiCommandFilter_Altered(MidiCommandFilter *) {}
    };
}

#endif
