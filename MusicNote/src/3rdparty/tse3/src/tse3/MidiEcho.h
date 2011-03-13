/*
 * @(#)MidiEcho.h 3.00 22 February 2000
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

#ifndef TSE3_MIDIECHO_H
#define TSE3_MIDIECHO_H

#include "tse3/listen/MidiEcho.h"

#include "tse3/Notifier.h"
#include "tse3/MidiFilter.h"

namespace TSE3
{
    class MidiScheduler;

    /**
     * The MidiEcho class provides a software equivalent to the MIDI Thru
     * port found on some MIDI equipment.
     *
     * MIDI Thru provides an exact copy of the MIDI data recieved at the MIDI
     * In port allowing a number of MIDI devices to be 'daisy-chained' together.
     *
     * Many MIDI interfaces, however, do not provide a MIDI Thru capability
     * so the MidiEcho class is used to emulate it in software.
     *
     * The MidiEcho class contains a @ref MidiFilter to provide some
     * software filtering of the incoming/outgoing data. You can disable
     * the MidiEcho functionality by setting the filter's status to false.
     *
     * The @ref Transport class creates and maintains an object of this class
     * to do good things.
     *
     * @short   Software MIDI Thru port
     * @author  Pete Goodliffe
     * @version 3.00
     */
    class MidiEcho : public Notifier<MidiEchoListener>
    {
        public:

            /**
             * MidiEcho defaults to being off.
             *
             * @param scheduler The MidiScheduler output is sent to.
             */
            MidiEcho(MidiScheduler *scheduler);

            /**
             * Returns the MIDI filter that effects MIDI echo.
             *
             * You can use this filter to force which channel and port the
             * MIDI echo output appears on.
             *
             * @return The MidiEcho object's @ref MidiFilter
             */
            MidiFilter *filter() { return &_filter; }

            /**
             * Returns the channel that MIDI echoing is accepted from.
             * A value of @ref MidiCommand::AllChannels means every channel.
             *
             * Note that this value doesn't set where MIDI echo output is
             * sent to, but from where it is received.
             *
             * @return Incoming MIDI channel
             * @see    setChannel
             */
            int channel() const { return _channel; }

            /**
             * Sets the channel that MIDI echoing is accepted from.
             *
             * Note that this value doesn't set where MIDI echo output is
             * sent to, but from where it is received.
             *
             * @param c New incoming MIDI channel
             * @see   channel
             */
            void setChannel(int c);

            /**
             * Returns the port that MIDI echoing is accepted from.
             * A value of @ref MidiScheduler::AllPorts means every port.
             *
             * Note that this value doesn't set where MIDI echo output is
             * sent to, but from where it is received.
             *
             * @return Incoming MIDI port
             * @see    setPort
             */
            int port() const { return _port; }

            /**
             * Sets the port that MIDI echoing is accepted from.
             *
             * Note that this value doesn't set where MIDI echo output is
             * sent to, but from where it is received.
             *
             * @param p New incoming MIDI port
             * @see   port
             */
            void setPort(int p);

            /**
             * Echo the specified command. This method should be called for
             * all recieved MIDI data, as soon as possible after it has
             * been recieved (this is done by the @ref Transport object).
             *
             * Although the input parameter is a @ref MidiEvent, only the
             * 'data' part, a @ref MidiCommand, is used.
             *
             * @param command The MIDI command to echo
             */
            void echo(MidiEvent command);

        private:

            MidiScheduler *scheduler;
            MidiFilter     _filter;
            int            _channel;
            int            _port;
    };
}

#endif
