/*
 * @(#)MidiMapper.h 3.00 25 May 1999
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

#ifndef TSE3_MIDIMAPPER_H
#define TSE3_MIDIMAPPER_h

#include "tse3/listen/MidiMapper.h"

#include "tse3/Notifier.h"
#include "tse3/Filter.h"

#include <utility>

namespace TSE3
{
    /**
     * The MidiMapper maintains a table mapping logical port
     * destinations with where @ref MidiEvents should /really/ be sent.
     *
     * This is used by the @ref Transport class prior to sending an
     * event to a MIDI interface.
     *
     * This interface allows you to establish arbitrary mappings. For
     * ports that aren't mapped, the mapper will let the event
     * through unchanged.
     *
     * @short   MidiEvent port destination mapper
     * @author  Pete Goodliffe
     * @version 3.00
     */
    class MidiMapper : public Filter,
                       public Notifier<MidiMapperListener>
    {
        public:

            /**
             * The default MidiMapper object performs no mapping whatsoever.
             */
            MidiMapper();
            virtual ~MidiMapper();

            /**
             * Returns the mapping for a particular port.
             *
             * If no mapping has been set for the specified port, the return
             * value will be the same as @p fromPort.
             *
             * @param  fromPort port to read mapping for
             * @return Port number this maps to
             * @see    setMap
             */
            const int map(int fromPort) const;

            /**
             * Set the mapping for a particular channel and port
             *
             * For @p fromPort, you cannot specify @ref MidiCommand::NoPorts
             * or @ref MidiCommand::AllPorts.
             *
             * To reset a mapping rule, you can just set @p fromPort equal
             * to @p toPort.
             *
             * @param fromPort port to set mapping for
             *                 (@ref MidiCommand::AllPorts to match all)
             * @param toPort   port to map to
             * @see   map
             */
            void setMap(int fromPort, int toPort);

            /**
             * The MidiMapper holds lists of port mappings. This method
             * returns the maximum port a mapping has been made for.
             *
             * @return Maximum port number a mapping has been made for
             */
            int maximumMap() const;

            /**
             * @reimplemented
             */
             virtual MidiEvent filter(const MidiEvent &me) const;

            /**
             * Reset the MidiMapper to a default state (i.e. no mappings).
             */
            void reset();

        private:

            MidiMapper &operator=(const MidiMapper &);
            MidiMapper(const MidiMapper &);

            class MidiMapperImpl *pimpl;
    };
}

#endif
