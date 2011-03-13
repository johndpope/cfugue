/*
 * @(#)Factry_Win32.cpp 3.00 5 February 2002
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

#ifdef CONFIG_H
#include "config.h"
#endif

#include "tse3/plt/Win32.h"

#include "tse3/util/MidiScheduler.h"

#include "tse3/Error.h"

using namespace TSE3::Plt;


TSE3::MidiSchedulerFactory::MidiSchedulerFactory(bool c)
: _canReturnNull(c)
{
}


TSE3::MidiSchedulerFactory::~MidiSchedulerFactory()
{
}


TSE3::MidiScheduler *TSE3::MidiSchedulerFactory::createScheduler()
{
    try
    {
        Win32MidiScheduler *ms = new Win32MidiScheduler();
        return ms;
    }
    catch (...)
    {
        std::cout << "Failed to create a Win32MidiScheduler\n";
        throw;
    }
}


