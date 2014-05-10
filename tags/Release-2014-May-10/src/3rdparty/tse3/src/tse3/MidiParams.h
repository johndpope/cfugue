/*
 * @(#)MidiParams.h 3.00 1 June 1999
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

#ifndef TSE3_MIDIPARAMS_H
#define TSE3_MIDIPARAMS_H

#include "tse3/listen/MidiParams.h"

#include "tse3/Notifier.h"
#include "tse3/Playable.h"
#include "tse3/Serializable.h"
#include "tse3/Filter.h"

namespace TSE3
{
    /**
     * The MidiParams class holds values for MIDI parameters used by several of
     * the TSE3 classes.
     *
     * The parameters it defines can generally be assigned values, not assigned
     * values or forced to not allow any changes in these values at all.
     *
     * For this reason, MidiParams implements both the @ref Playable and
     * @ref Filter interfaces.
     *
     * @sect Command classes
     *
     * Use the following command classes to manipute this object in a undo/redo
     * environment.
     *
     *     @li @ref TSE3::Cmd::Track_SetInfo
     *     @li @ref TSE3::Cmd::Part_SetInfo
     *
     * @short   A collection of MIDI parameter values
     * @author  Pete Goodliffe
     * @version 3.00
     */
    class MidiParams : public Playable,
                       public Filter,
                       public Serializable,
                       public Notifier<MidiParamsListener>
    {
        public:

            /**
             * The default MidiParams has all parameters set to '@ref off'.
             */
            MidiParams();
            MidiParams(const MidiParams &);
            virtual ~MidiParams();
            MidiParams &operator=(const MidiParams &);

            /**
             * A parameter is set to this value to switch it's effect off.
             */
            static const int off = -1;

            /**
             * A parameter is set to this value to prevent any changes in it
             * occurring.
             */
            static const int forceNone = -2;

            /**
             * Read the bankLSB value.
             *
             * @return Bank select LSB value
             * @see setBankLSB
             */
            int bankLSB() const { return _bankLSB; }

            /**
             * Set the bankLSB value.
             *
             * @param b New bank select LSB value
             * @see bankLSB
             */
            void setBankLSB(int b);

            /**
             * Read the bankMSB value.
             *
             * @return Bank select MSB value
             * @see setBankMSB
             */
            int bankMSB() const { return _bankMSB; }

            /**
             * Set the bankMSB value.
             *
             * @param b New bank select MSB value
             * @see bankMSB
             */
            void setBankMSB(int b);

            /**
             * Read the program value.
             *
             * @return Program value
             * @see setProgram
             */
            int program() const { return _program; }

            /**
             * Set the program value.
             *
             * @param p New program value
             * @see program
             */
            void setProgram(int p);

            /**
             * Read the pan value.
             *
             * @return Pan value
             * @see setPan
             */
            int pan() const { return _pan; }

            /**
             * Set the pan value.
             *
             * @param p New pan value
             * @see pan
             */
            void setPan(int p);

            /**
             * Read the reverb value.
             *
             * @return Reverb value
             * @see setReverb
             */
            int reverb() const { return _reverb; }

            /**
             * Set the reverb value.
             *
             * @param r New reverb value
             * @see reverb
             */
            void setReverb(int r);

            /**
             * Read the chorus value.
             *
             * @return Chorus value
             * @see setChorus
             */
            int chorus() const { return _chorus; }

            /**
             * Set the chorus value.
             *
             * @param c New chorus value
             * @see chorus
             */
            void setChorus(int c);

            /**
             * Read the volume value.
             *
             * @return Volume value
             * @see setVolume
             */
            int volume() const { return _volume; }

            /**
             * Set the volume value.
             *
             * @param v New volume value
             * @see volume
             */
            void setVolume(int v);

            /**
             * @reimplemented
             */
            virtual PlayableIterator *iterator(Clock index);

            /**
             * @reimplemented
             */
            virtual Clock lastClock() const;

            /**
             * @reimplemented
             */
            virtual void save(std::ostream &o, int i) const;

            /**
             * @reimplemented
             */
            virtual void load(std::istream &in, SerializableLoadInfo &info);

            /**
             * @reimplemented
             */
            virtual MidiEvent filter(const MidiEvent &e) const;

        private:

            /*
             * Parameter values (0-127, -1 = none, -2 = force none)
             */

            int _bankLSB;
            int _bankMSB;
            int _program;
            int _pan;
            int _reverb;
            int _chorus;
            int _volume;
    };
}

#endif
