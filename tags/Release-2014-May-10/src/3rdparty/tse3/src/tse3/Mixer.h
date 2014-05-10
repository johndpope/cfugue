/*
 * @(#)Mixer.h 3.00 31 March 2000
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

#ifndef TSE3_MIXER_H
#define TSE3_MIXER_H

#include "tse3/listen/Mixer.h"

#include "tse3/Notifier.h"
#include "tse3/Midi.h"
#include "tse3/listen/Transport.h"

#include <cstddef>

namespace TSE3
{
    /**
     * The MixerChannel holds all @ref Mixer values for a particular MIDI
     * channel. It holds information on the current state of the following
     * MIDI parameters:
     * @li channel volume
     * @li pan
     * @li chorus
     * @li reverb
     * @li program
     * @li bank select MSB
     * @li bank select LSB
     *
     * @short   TSE3 MIDI port Mixing desk
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Mixer
     * @see     MixerChannel
     */
    class MixerChannel : public Notifier<MixerChannelListener>
    {
        public:

            /**
             * Returns this channel's volume value.
             *
             * @return Channel's volume value
             * @see    setVolume
             */
            unsigned int volume() const { return _volume; }

            /**
             * Sets the channel's volume value.
             *
             * @param v    New volume value
             * @param send Whether to send a MIDI command for this setting
             * @see   volume
             */
            void setVolume(unsigned int v, bool send = true);

            /**
             * Returns this channel's pan value.
             *
             * @return Channel's pan value
             * @see    setPan
             */
            unsigned int pan() const { return _volume; }

            /**
             * Sets the channel's pan value.
             *
             * @param p    New pan value
             * @param send Whether to send a MIDI command for this setting
             * @see   pan
             */
            void setPan(unsigned int p, bool send = true);

            /**
             * Returns this channel's chorus value.
             *
             * @return Channel's chorus value
             * @see    setChorus
             */
            unsigned int chorus() const { return _volume; }

            /**
             * Sets the channel's chorus value.
             *
             * @param c    New chorus value
             * @param send Whether to send a MIDI command for this setting
             * @see   chorus
             */
            void setChorus(unsigned int c, bool send = true);

            /**
             * Returns this channel's reverb value.
             *
             * @return Channel's reverb value
             * @see    setReverb
             */
            unsigned int reverb() const { return _volume; }

            /**
             * Sets the channel's reverb value.
             *
             * @param r    New reverb value
             * @param send Whether to send a MIDI command for this setting
             * @see   reverb
             */
            void setReverb(unsigned int r, bool send = true);

            /**
             * Returns this channel's program value.
             *
             * @return Channel's program value
             * @see    setProgram
             */
            unsigned int program() const { return _volume; }

            /**
             * Sets the channel's program value.
             *
             * @param p    New program value
             * @param send Whether to send a MIDI command for this setting
             * @see   program
             */
            void setProgram(unsigned int p, bool send = true);

            /**
             * Returns this channel's bank LSB value.
             *
             * @return Channel's bank select LSB value
             * @see    setBankLSB
             */
            unsigned int bankLSB() const { return _volume; }

            /**
             * Sets the channel's blank LSB value.
             *
             * @param b    New bank select LSB value
             * @param send Whether to send a MIDI command for this setting
             * @see   bankLSB
             */
            void setBankLSB(unsigned int b, bool send = true);

            /**
             * Returns this channel's bank MSB value.
             *
             * @return Channel's bank select MSB value
             * @see    setBankMSB
             */
            unsigned int bankMSB() const { return _volume; }

            /**
             * Sets the channel's bank MSB value.
             *
             * @param b    New bank select MSB value
             * @param send Whether to send a MIDI command for this setting
             * @see   bankMSB
             */
            void setBankMSB(unsigned int b, bool send = true);

            /**
             * Process a @ref MidiCommand for this MixerChannel.
             *
             * This will be called by the @ref MixerPort class if the
             * @ref MixerPort is given a @ref MidiCommand to process. (Indeed,
             * this will indirectly be called by sending an event to the
             * top-level @ref Mixer object).
             *
             * @param mc @ref MidiCommand to process
             */
            void command(MidiCommand mc);

            friend class MixerPort;

        private:

            /**
             * Creates a MixerChannel with default mixer parameters.
             */
            MixerChannel(MixerPort *mixerPort, unsigned int channel);
            virtual ~MixerChannel();

            MixerChannel(const MixerChannel &);
            MixerChannel &operator=(const MixerChannel &);

            MixerPort     *mixerPort;
            unsigned int   channel;
            unsigned char  _volume;
            unsigned char  _pan;
            unsigned char  _chorus;
            unsigned char  _reverb;
            unsigned char  _program;
            unsigned char  _bankLSB;
            unsigned char  _bankMSB;
    };

    /**
     * The MixerPort hols all @ref Mixer values for a particular MIDI port.
     * It contains 16 @ref MixerChannel objects which hold the information
     * for each of the port's MIDI channels.
     *
     * @short   TSE3 MIDI port Mixing desk
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Mixer
     * @see     MixerChannel
     */
    class MixerPort : public Notifier<MixerPortListener>
    {
        public:

            /**
             * Returns this MixerPort's @ref MixerChannel object for the
             * specified channel.
             *
             * The value returned for an index that is out of range is
             * undefined. The @ref size method describes the valid
             * values.
             *
             * @param n Index
             * @return @ref MixerChannel for this MIDI channel number
             */
            MixerChannel *operator[](size_t n) { return mixerChannels[n]; }

            /**
             * Returns this ports's master volume value.
             *
             * @return Port's volume value
             * @see setVolume
             */
            unsigned int volume() const { return _volume; }

            /**
             * Sets the ports's master volume value.
             *
             * @param v    New volume value
             * @param send Whether to send a MIDI command for this setting
             * @see volume
             */
            void setVolume(unsigned int v, bool send = true);

            /**
             * Process a @ref MidiCommand.
             *
             * This accepts the specified event (which is for this port) and
             * sends it to the appropriate MixerChannel.
             *
             * This will be called by the @ref Mixer class if the @ref Mixer
             * is given a @ref MidiCommand to process.
             *
             * @param mc @ref MidiCommand to process
             */
            void command(MidiCommand mc);

            friend class Mixer;
            friend class MixerChannel;

        private:

            MixerPort(Mixer *mixer, unsigned int port);
            virtual ~MixerPort();

            MixerPort(const MixerPort &);
            MixerPort &operator=(const MixerPort &);

            /**
             * This method sends the given @ref MidiCommand to the
             * @ref Mixer object, filling in the port information.
             */
            void txCommand(MidiCommand mc);

            MixerChannel  *mixerChannels[16];
            Mixer         *mixer;
            unsigned int   port;
            unsigned char  _volume;
    };

    /**
     * The Mixer class acts in a similar way to a studio mixing desk - it
     * represents the values of all the knobs and sliders you might find (in
     * the MIDI realm). The values it stores include main volume, channel
     * volume, channel pan, channel reverb, etc.
     *
     * The values can be updated in real time according to MIDI input, MIDI
     * output, or both. Values can be altered and sent back out through MIDI
     * out.
     *
     * It contains a number of @ref MixerPort objects which hold the state for
     * each MIDI port in the system.
     *
     * @short   TSE3 Mixing desk
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     MidiCommand
     * @see     MixerPort
     */
    class Mixer : public Notifier<MixerListener>,
                  public Listener<TransportListener>,
                  public TransportCallback
    {
        public:

            /**
             * The Mixer object is created with all values at GM default.
             *
             * @param noPorts The number of MIDI ports to hold mixing
             *                information for - you can get the appropriate
             *                value from the @ref MidiScheduler object
             */
            Mixer(size_t noPorts, Transport *transport);
            virtual ~Mixer();

            /**
             * Returns this Mixer's @ref MixerPort object for the specified
             * port.
             *
             * The value returned for an index that is out of range is
             * undefined. The @ref size method describes the valid
             * values.
             *
             * @param n Index
             * @return @ref MixerPort for this MIDI port number
             */
            MixerPort *operator[](size_t n) { return mixerPorts[n]; }

            /**
             * Process a @ref MidiCommand.
             *
             * This accepts the specified command, and sends it to the
             * appropriate MixerPort.
             *
             * @param mc @ref MidiCommand to process
             */
            void command(MidiCommand mc);

            /**
             * @reimplemented
             */
            virtual void Notifier_Deleted(Transport *);

            /**
             * @reimplemented
             */
            virtual void Transport_MidiIn(MidiCommand mc);

            /**
             * @reimplemented
             */
            virtual void Transport_MidiOut(MidiCommand mc);

            friend class MixerPort;

        private:

            Mixer(const Mixer &);
            Mixer &operator=(const Mixer &);

            /**
             * This method sends the given @ref MidiCommand to the
             * @ref Transport object.
             */
            void txCommand(MidiCommand mc);

            size_t      noPorts;
            MixerPort **mixerPorts;
            Transport  *transport;
            bool        _updateWithInput;
            bool        _updateWithOutput;
    };

}

#endif
