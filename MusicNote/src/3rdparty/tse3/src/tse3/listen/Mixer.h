/*
 * @(#)listen/Mixer.h 3.00 31 March 2000
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

#ifndef TSE3_LISTEN_MIXER_H
#define TSE3_LISTEN_MIXER_H

namespace TSE3
{
    class MixerChannel;
    class MixerPort;
    class Mixer;

    /**
     * @ref MixerChannel listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class MixerChannelListener
    {
        public:
            typedef MixerChannel notifier_type;

            virtual void MixerChannel_Volume(MixerChannel *)  {}
            virtual void MixerChannel_Pan(MixerChannel *)     {}
            virtual void MixerChannel_Chorus(MixerChannel *)  {}
            virtual void MixerChannel_Reverb(MixerChannel *)  {}
            virtual void MixerChannel_Program(MixerChannel *) {}
            virtual void MixerChannel_BankLSB(MixerChannel *) {}
            virtual void MixerChannel_BankMSB(MixerChannel *) {}
    };

    /**
     * @ref MixerPort listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class MixerPortListener
    {
        public:
            typedef MixerPort notifier_type;

            virtual void MixerPort_Volume(MixerPort *) {}
    };

    /**
     * @ref Mixer listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class MixerListener
    {
        public:
            typedef Mixer notifier_type;
    };
}

#endif
