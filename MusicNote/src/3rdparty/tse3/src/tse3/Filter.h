/*
 * @(#)Filter.h 3.00 8 June 1999
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

#ifndef TSE3_MIDIEVENTFILTER_H
#define TSE3_MIDIEVENTFILTER_H

namespace TSE3
{
    struct MidiEvent;

    /**
     * The Filter is an interface that allows the use of MidiEvent
     * 'filters'. These filters accept a single @ref MidiEvent, and return the
     * @ref MidiEvent filtered in some way.
     *
     * @short   Interface for a @ref MidiEvent filter
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     MidiEvent
     */
    class Filter
    {
        public:

            Filter();

            virtual ~Filter() = 0;

            /**
             * Filter a @ref MidiEvent by doing something suitably marvellous
             * to it. If the event is removed altogether the filter will return
             * a @ref MidiEvent with a @ref MidiCommand_Invalid. These can
             * propagate up the @ref Playable hierarchy and be ignored by the
             * @ref MidiScheduler at the top.
             *
             * @param  e @ref MidiEvent to filter
             * @return @ref Filtered @ref MidiEvent (may be a @ref
             *         MidiCommand_Invalid)
             */
            virtual MidiEvent filter(const MidiEvent &e) const = 0;

        private:

            Filter &operator=(const Filter &);
            Filter(const Filter &);
    };
}

#endif
