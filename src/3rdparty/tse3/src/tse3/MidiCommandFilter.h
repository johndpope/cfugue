/*
 * @(#)MidiCommandFilter.h 3.00 19 May 1999
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

#ifndef TSE3_MIDICOMMANDFILTER_H
#define TSE3_MIDICOMMANDFILTER_H

#include "tse3/listen/MidiCommandFilter.h"

#include "tse3/Notifier.h"

namespace TSE3
{
    class MidiCommand;

    /**
     * This class provides a configurable filter for the different kinds
     * of @ref MidiCommand. It tells it's users whether a given @ref MidiCommand
     * is to be used or not.
     *
     * The MidiCommandFilter considers MidiCommand_NoteOff and
     * MidiCommand_NoteOns to be the same (otherwise you could produce 'hanging
     * notes').
     *
     * @short   Filter for MidiCommands
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     MidiCommand
     */
    class MidiCommandFilter : public Notifier<MidiCommandFilterListener>
    {
        public:

            /**
             * By default, all the filters are enabled, that is all
             * MidiCommands are passed through.
             */
            MidiCommandFilter() : _filter(0xff) {}

            /**
             * Returns the status of a @ref MidiCommand filter.
             *
             * @param  type A MidiCommand to filter
             *         (only the status is inspected)
             * @return A boolean filter value:
             *         true for the MidiCommand being allowed through the
             *         filter,
             *         false for the MidiCommand being filtered out.
             */
            bool filter(MidiCommand type) const;

            /**
             * Sets the status of a @ref MidiCommand filter.
             *
             * @param type A MidiCommand to set a filter for
             *        (only the status is inspected)
             * @param A boolean filter value:
             *        true to allow the MidiCommand through the filter,
             *        false to prevent the MidiCommand passing the filter.
             */
            void setFilterStatus(MidiCommand type, const bool status);

        private:

            int _filter; // filter statuses held in a bitfield
    };
}

#endif
