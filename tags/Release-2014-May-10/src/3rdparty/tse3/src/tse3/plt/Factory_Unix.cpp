/*
 * @(#)Factory_Unix.cpp 3.00 4 February 2002
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

#include "tse3/plt/Factory.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef TSE3_WITH_OSS
#include "tse3/plt/OSS.h"
#endif

#ifdef TSE3_WITH_ALSA
#include "tse3/plt/Alsa.h"
#endif

#ifdef TSE3_WITH_ARTS
#include "tse3/plt/Arts.h"
#endif

#include "tse3/util/MidiScheduler.h"

#include "tse3/Error.h"

using namespace TSE3::Plt;

namespace
{
    TSE3::Plt::UnixMidiSchedulerFactory::UnixPlatform plt
        = TSE3::Plt::UnixMidiSchedulerFactory::UnixPlatform_Alsa;
    TSE3::Plt::UnixMidiSchedulerFactory::UnixPlatform cplt
        = TSE3::Plt::UnixMidiSchedulerFactory::UnixPlatform_Null;

    /**
     * Try to create an @ref OSSMidiScheduler. Catch any
     * exceptions thrown, and return zero if didn't create.
     */
    TSE3::MidiScheduler *createOSS();

    /**
     * Try to create an @ref AlsaMidiScheduler. Catch any
     * exceptions thrown, and return zero if didn't create.
     */
    TSE3::MidiScheduler *createAlsa();

    /**
     * Try to create an @ref ArtsMidiScheduler. Catch any
     * exceptions thrown, and return zero if didn't create.
     */
    TSE3::MidiScheduler *createArts();
}

namespace TSE3
{
namespace Plt
{
namespace UnixMidiSchedulerFactory
{
    UnixPlatform preferredPlatform()
    {
        return plt;
    }
    void setPreferredPlatform(UnixPlatform p)
    {
        plt = p;
    }
    UnixPlatform createdPlatform()
    {
        return cplt;
    }
}
}
}


TSE3::MidiSchedulerFactory::MidiSchedulerFactory(bool c)
: _canReturnNull(c)
{
}


TSE3::MidiSchedulerFactory::~MidiSchedulerFactory()
{
}


TSE3::MidiScheduler *TSE3::MidiSchedulerFactory::createScheduler()
{
    TSE3::MidiScheduler *ms = 0;
    switch (plt)
    {
        case TSE3::Plt::UnixMidiSchedulerFactory::UnixPlatform_OSS:
        {
            ms = createOSS();
            if (!ms)
            {
                ms = createAlsa();
            }
            break;
        }
        case TSE3::Plt::UnixMidiSchedulerFactory::UnixPlatform_Alsa:
        {
            ms = createAlsa();
            if (!ms)
            {
                ms = createOSS();
            }
            break;
        }
        case TSE3::Plt::UnixMidiSchedulerFactory::UnixPlatform_Arts:
        {
            ms = createArts();
            if (!ms)
            {
                ms = createAlsa();
            }
            if (!ms)
            {
                ms = createOSS();
            }
            break;
        }
        default:
        {
            break;
        }
    }
    if (!ms)
    {
        if (_canReturnNull)
        {
            ms = new TSE3::Util::NullMidiScheduler();
        }
        else
        {
            throw MidiSchedulerError(MidiSchedulerCreateErr);
        }
    }
    return ms;
}


namespace
{

TSE3::MidiScheduler *createOSS()
{
    TSE3::MidiScheduler *ms = 0;
#ifdef TSE3_WITH_OSS
    try
    {
        // Create the scheduler
        ms = new OSSMidiScheduler();
        cplt = TSE3::Plt::UnixMidiSchedulerFactory::UnixPlatform_OSS;
    }
    catch (TSE3::Error)
    {
    }
#endif
    return ms;
}


TSE3::MidiScheduler *createAlsa()
{
    TSE3::MidiScheduler *ms = 0;
#ifdef TSE3_WITH_ALSA
    try
    {
        // Create the scheduler
        ms = new AlsaMidiScheduler();
        cplt = TSE3::Plt::UnixMidiSchedulerFactory::UnixPlatform_Alsa;
    }
    catch (TSE3::Error)
    {
    }
#endif
    return ms;
}


TSE3::MidiScheduler *createArts()
{
    TSE3::MidiScheduler *ms = 0;
#ifdef TSE3_WITH_ARTS
    try
    {
        // Create the scheduler
        ms = new ArtsMidiScheduler();
        cplt = TSE3::Plt::UnixMidiSchedulerFactory::UnixPlatform_Arts;
    }
    catch (TSE3::Error)
    {
    }
#endif
    return ms;
}

}
