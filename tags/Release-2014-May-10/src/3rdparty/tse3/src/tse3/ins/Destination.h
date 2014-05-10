/*
 * @(#)ins/Destination.h 3.00 21 August 2000
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

#ifndef TSE3_INS_DESTINATION_H
#define TSE3_INS_DESTINATION_H

#include "tse3/listen/ins/Destination.h"

#include "tse3/Notifier.h"

#include <string>
#include <cstddef>

namespace TSE3
{
    namespace Ins
    {
        /**
         * The Destination class is a simple utility class that can be used to
         * associate @ref Instrument definitions with MIDI outputs (channel/port
         * pairs).
         *
         * An application can use this to remember what sort of instrument
         * definition to use for which output; to present the correct program
         * change information to the user, for example.
         *
         * For each port, you may choose one @ref Instrument for every
         * channel, or may select a number of different @ref Instrument objects
         * for the different individual channels.
         *
         * You may also leave a particular definition unspecified (as zero).
         *
         * The Destination class has a secondary purpose of keeping track of
         * all the currently used @ref Instrument objects, keeping the
         * list of desinations up to date if any @ref Instrument is removed.
         *
         * Since this class operates independantly of a @ref TSE3::MidiScheduler
         * it cannot track what port numbers are currently valid, and
         * which are not.
         *
         * @short   MIDI destination information utility
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Instrument
         */
        class Destination : public TSE3::Notifier<DestinationListener>
        {
            public:

                /**
                 * Creates a Destination object.
                 *
                 * Initially no destinations are specified.
                 */
                Destination();
                ~Destination();

                /**************************************************************
                 * Managing the list of destinations
                 *************************************************************/

                /**
                 * Returns the default @ref Instrument.
                 *
                 * This is the instrument definition that is returned if
                 * no other mapping has been made.
                 *
                 * By default, this is set to zero.
                 *
                 * @return The default instrument definition
                 * @see    setDefaultInstrument
                 */
                Instrument *defaultInstrument() const;

                /**
                 * Sets the default @ref Instrument.
                 *
                 * @param instrument The default definition
                 * @see   defaultInstrument
                 */
                void setDefaultInstrument(Instrument *instrument);

                /**
                 * Returns whether there is one @ref Instrument selected for
                 * every channel on this port (true) or whether each channel
                 * is been assigned separately (false).
                 *
                 * @param  port Port number to enquire about
                 * @return Whether there is one @ref Instrument for every
                 *         channel on this port
                 * @see    setPort
                 * @see    setChannel
                 */
                bool allChannels(int port);

                /**
                 * Returns the @ref Instrument selected for this entire
                 * port. This instrument is used for every channel on this
                 * port.
                 *
                 * If no instrument has been specified for this @p port, then
                 * port() returns the @ref defaultInstrument.
                 *
                 * If @ref allChannels is false for this @p port then zero
                 * is returned (you have called this in error).
                 *
                 * @param  port Port number to enquire about
                 * @return The @ref Instrument selected for this port, or 0
                 *         if no @ref Instrument is specified
                 * @see    setPort
                 * @see    channel
                 */
                Instrument *port(int port);

                /**
                 * Sets which instrument is used by this port. This will
                 * have the side effect of making @ref allChannels return true.
                 *
                 * You may specify an @p instrument of zero to unspecify
                 * an @ref Instrument definition.
                 *
                 * @param port Port number to set @ref Instrument for
                 * @param instrument @ref Instrument to specify. Whilst it's
                 *                   not essential that this has been added
                 *                   with @ref addInstrument, it is advised
                 *                   to do so.
                 * @see   setChannel
                 */
                void setPort(int port, Instrument *instrument);

                /**
                 * Returns the @ref Instrument selected for this channel/port
                 * destination.
                 *
                 * If no instrument has been specified for this port/channel,
                 * then @p port returns @ref defaultInstrument.
                 *
                 * If @ref allChannels is true for this @p port then the
                 * @p channel value is ignored.
                 *
                 * @param  port    Port number to enquire about
                 * @param  channel Channel number to enquire about
                 * @return The @ref Instrument selected for this port, or 0
                 *         if no @ref Instrument is specified
                 * @see    setChannel
                 * @see    port
                 */
                Instrument *channel(int channel, int port);

                /**
                 * Sets which instrument is used by this channel/port pair.
                 * This will have the side effect of making @ref allChannels
                 * return false.
                 *
                 * You may specify an @p instrument of zero to unspecify
                 * an @ref Instrument definition.
                 *
                 * @param port    Port number to set @ref Instrument for
                 * @param channel Channel number to set @ref Instrument for
                 * @param instrument @ref Instrument to specify. Whilst it's
                 *                   not essential that this has been added
                 *                   with @ref addInstrument, it is advised
                 *                   to do so.
                 * @see   setChannel
                 */
                void setChannel(int channel, int port,
                                Instrument *instrument);

                /**************************************************************
                 * Managing the list of instruments
                 *************************************************************/

                /**
                 * Returns the number of @ref Instrument objects currently
                 * managed by the Destination object.
                 */
                size_t numInstruments() const;

                /**
                 * Returns the @ref Instrument at the given index.
                 * The list of @ref Instrument object is ordered
                 * alphabetically.
                 *
                 * @param  index Index into @ref Instrument list (between
                 *               0 and @ref noInstruments()).
                 * @return @ref Instrument object at @p index
                 * @see    noInstruments
                 */
                Instrument *instrument(size_t index);

                /**
                 * Returns the @ref Instrument with the given title,
                 * or zero if there is no such @ref Instrument.
                 *
                 * @param  title @ref Instrument title to search for
                 * @return @ref Instrument object or 0
                 */
                Instrument *instrument(const std::string &title);

                /**
                 * Adds the specified @ref Instrument to the list of
                 * @ref Instrument objects. You can only insert a given
                 * @ref Instrument once. The instrument is inserted into the
                 * list in alphabetical order of title.
                 *
                 * The @ref Instrument object is considered to be 'owned' by
                 * the Destination class, and will be deleted when the
                 * Destination is.
                 *
                 * @param instrument New @ref Instrument object to insert
                 * @see   removeInstrument
                 */
                void addInstrument(Instrument *instrument);

                /**
                 * Removed the specified @ref Instrument from the list of
                 * @ref Instrument objects. If @p instrument is being used
                 * as a destination, then the destination link is removed.
                 *
                 * Once removed it is your responsibility to delete the
                 * @ref Instrument.
                 *
                 * @param instrument @ref Instrument object to remove
                 * @see   addInstrument
                 */
                void removeInstrument(Instrument *instrument);

            private:

                Destination(const Destination &);
                Destination &operator=(const Destination &);

                class DestinationImpl *pimpl;

                size_t index(int port, int channel)
                {
                    return (port*16) + channel;
                }
        };
    }
}

#endif
