/*
 * @(#)plt/OSS.h 3.00 20 July 1999
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

#ifndef TSE3_PLT_OSS_H
#define TSE3_PLT_OSS_H

#include "tse3/MidiScheduler.h"

#include <string>
#include <list>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef TSE3_WITH_OSS
#include <sys/soundcard.h>
#else
    struct synth_info;
    struct midi_info;
#endif

namespace TSE3
{
    namespace Plt
    {
        class OSSMidiScheduler_SynthDevice;

        /**
         * This is the Open Sound System @ref MidiScheduler implementation.
         * It requires OSS version 3.60.
         *
         * Since it opens /dev/sequencer (the OSS sequencer interface) there
         * can only ever be one of these objects instantiated at any given time.
         * Subsequent instanitations will throw an exception.
         *
         * Before you create an object of this type you may want to call the
         * following static member functions to set the location of patches
         * files:
         *
         * @li @ref OSSMidiScheduler_FMDevice::setPatchesDirectory
         * @li @ref OSSMidiScheduler_GUSDevice::setPatchesDirectory
         *
         * @short   OSS MidiScheduler implemtation
         * @author  Pete Goodliffe
         * @version 0.00
         * @see     MidiScheduler
         * @see     OSSMidiSchedulerFactory
         */
        class OSSMidiScheduler : public MidiScheduler
        {
            public:

                /**
                 * @throws TSE3::MidiSchedulerError
                 */
                OSSMidiScheduler();
                virtual ~OSSMidiScheduler();

                /**************************************************************
                 * OSS specific settings
                 *************************************************************/

                /**
                 * This returns in which directory the device will look for
                 * FM patch files. This must be set before the device is
                 * created.
                 *
                 * The patches directory path can contain multiple paths,
                 * separated by colons.
                 *
                 * @see setFmPatchesDirectory
                 */
                static std::string &fmPatchesDirectory();

                /**
                 * Call this before creating the @ref OSSMidiScheduler object
                 * to set up where to look for FM patches files.
                 *
                 * @see fmPatchesDirectory
                 */
                static void setFmPatchesDirectory(const std::string &dir);

                /**
                 * This returns in which directory the device will look for
                 * GUD patch files. This must be set before the device is
                 * created. (In fact, it must be set before each GUS voice is
                 * loaded - they are loaded on demand. However, it is safest to
                 * set this before creating the device.)
                 *
                 * The patches directory path can contain multiple paths,
                 * separated by colons.
                 *
                 * @see setGusPatchesDirectory
                 */
                static std::string &gusPatchesDirectory();

                /**
                 * Call this before creating the @ref OSSMidiScheduler object
                 * to set up where to look for GUS patches files.
                 *
                 * @see gusPatchesDirectory
                 */
                static void setGusPatchesDirectory(const std::string &dir);

            protected:

                /**
                 * @reimplemented
                 */
                virtual const char *impl_implementationName() const;
                /**
                 * @reimplemented
                 */
                virtual const char *impl_portName(int port) const;
                /**
                 * @reimplemented
                 */
                virtual const char *impl_portType(int port) const;
                /**
                 * @reimplemented
                 */
                virtual bool impl_portReadable(int port) const;
                /**
                 * @reimplemented
                 */
                virtual bool impl_portWriteable(int port) const;
                /**
                 * @reimplemented
                 */
                virtual void impl_start(Clock clock);
                /**
                 * @reimplemented
                 */
                virtual void impl_stop(Clock clock);
                /**
                 * @reimplemented
                 */
                virtual void impl_moveTo(Clock moveTime, Clock newTime);
                /**
                 * @reimplemented
                 */
                virtual Clock impl_clock();
                /**
                 * @reimplemented
                 */
                virtual int impl_msecs();
                /**
                 * @reimplemented
                 */
                virtual void impl_setTempo(int tempo, Clock changeTime);
                /**
                 * @reimplemented
                 */
                virtual bool impl_eventWaiting();
                /**
                 * @reimplemented
                 */
                virtual MidiEvent impl_rx();
                /**
                 * @reimplemented
                 */
                virtual void impl_tx(MidiCommand mc);
                /**
                 * @reimplemented
                 */
                virtual void impl_tx(MidiEvent mc);
                /**
                 * @reimplemented
                 */
                virtual void impl_txSysEx(int port,
                                          const unsigned char *data,
                                          size_t size);

            private:

                OSSMidiScheduler(const OSSMidiScheduler &);
                OSSMidiScheduler &operator=(const OSSMidiScheduler &);

                /**
                 * Does the tx. If outOfBand is false, sends data to the
                 * end of the playback queue. If outOfBand is true, it
                 * sends data immediately.
                 */
                void tx(const MidiCommand mc, bool outOfBand);

                unsigned char *running;     // running status values
                bool          *useRunning;  // does device support running sts?
                unsigned int   nosynths;    // no of internal synth devices
                unsigned int   nomidis;     // no of MIDI ports
                unsigned int   nodevices;   // nosynths + nomidis
                int            rate;        // sequencer timer rate (in ms)
                int            rateDivisor; // value to / timer by to get ms
                synth_info    *synthinfo;   // synth_info for each synth device
                midi_info     *midiinfo;    // midi_info for each MIDI device

                // In C we write SEQ_DEFINEBUF(1024); to set up some OSS magic
                // which creates a buffer that the OSS macros write data into.
                // However, this won't work in a class declaration. Sigh.
                // So this is how we do it.
                int            seqfd;          // file descfor /dev/sequencer
                unsigned char *_seqbuf;        // buffer for /dev/sequencer data
                int            _seqbuflen;     // size of _seqbuf
                int            _seqbufptr;     // pointer to end of pending data
                void           seqbuf_dump();  // wrts _seqbuf to /dev/sequencer
                void           seqbuf_clean(); // flushes the buffer

                // For a given port (TSE3 speak, same as OSS device number) is
                // it a synth or MIDI device?
                bool isSynth(unsigned int port) const
                {
                    return port < nosynths;
                }
                bool isMidi(unsigned int port) const
                {
                    return port >= nosynths;
                }

                // MIDI devices are easy to send data too, but the synths need
                // some special massaging. The classes held in this table do
                // just that.
                OSSMidiScheduler_SynthDevice **devices;

                // Output malarky
                Clock lastTxTime; // Time last MidiCommand was scheduled

                // Input malarky
                bool        input;   // true if there is an input command
                MidiCommand command; // if input is true, this holds the input
                Clock       time;    // time of the input MidiCommand

                /**
                 * Performs input scan of the /dev/sequencer device and
                 * interprets what it reads. If there is any input, then
                 * the input variable is set to true. When you've read the
                 * input, then set it to false.
                 */
                void readInput();
        };
    }
}

#endif
