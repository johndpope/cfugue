/*
 * @(#)plt/Unix.h 3.00 15 October 1999
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

#ifndef TSE3_PLT_UNIX_H
#define TSE3_PLT_UNIX_H

#include "tse3/MidiScheduler.h"

namespace TSE3
{
    /**
     * The Plt namespace contains classes that provide particular
     * platform @ref TSE3::MidiScheduler classes for different hardware
     * or software platforms.
     *
     * @short   @ref TSE3::MidiScheduler platform implementations
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     TSE3
     */
    namespace Plt
    {
        /**
         * This is the set of Unix platform additional APIs for the
         * @ref MidiSchedulerFactory.
         *
         * The MidiSchedulerFactory will (depending on availablity and
         * compile-time options) create either:
         *   @li An @ref OSSMidiScheduler
         *   @li An @ref AlsaMidiScheduler
         *   @li An @ref ArtsMidiScheduler
         *
         * @short   Unix platform MidiSchedulerFactory settings
         * @author  Pete Goodliffe
         * @version 1.00
         */
        namespace UnixMidiSchedulerFactory
        {
            /**
             * enum type describing a type of Unix @ref MidiScheduler
             * class.
             */
            enum UnixPlatform
            {
                UnixPlatform_OSS,
                UnixPlatform_Alsa,
                UnixPlatform_Arts,
                UnixPlatform_Null
            };

            /**
             * Returns the currently set preferred platform.
             *
             * The default is UnixPlatform_Alsa.
             *
             * @return Preferred platform
             * @see    setPreferredPlatform
             */
            UnixPlatform preferredPlatform();

            /**
             * Sets the preferred platform. You will need to call
             * this prior to the @ref createScheduler method being
             * called if you require behaviour different from the default.
             *
             * The default is UnixPlatform_Alsa.
             *
             * @param p Preferred platform type
             * @see   preferredPlatform
             */
            void setPreferredPlatform(UnixPlatform p);

            /**
             * Returns the type of platform that has been created by the
             * MidiScheduler.
             *
             * The result is only valid after @ref createScheduler has been
             * called, and if the MidiSchedulerFactory really is a Unix
             * version.
             */
            UnixPlatform createdPlatform();
        }
    }
}

#endif
