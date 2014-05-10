/*
 * @(#)MidiMapper.cpp 3.00 24 May 1999
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

#include "tse3/MidiMapper.h"

#include "tse3/MidiScheduler.h"
#include "tse3/Midi.h"

#include <vector>

using namespace TSE3;


/******************************************************************************
 * MidiMapperImpl class
 *****************************************************************************/

class TSE3::MidiMapperImpl
{
    public:

        std::vector<int> map;
};


/******************************************************************************
 * MidiMapper class
 *****************************************************************************/

MidiMapper::MidiMapper()
: pimpl(new MidiMapperImpl)
{
    reset();
}


MidiMapper::~MidiMapper()
{
    delete pimpl;
}


const int MidiMapper::map(int fromPort) const
{
    if (fromPort >= (int)pimpl->map.size())
    {
        return fromPort;
    }
    else
    {
        return pimpl->map[fromPort];
    }
}


void MidiMapper::setMap(int fromPort, int toPort)
{
    if (fromPort == MidiCommand::NoPort
        || fromPort == MidiCommand::AllPorts)
    {
        return;
    }

    if (fromPort >= (int)pimpl->map.size())
    {
        if (fromPort == toPort) return; // no-op

        while ((int)pimpl->map.size() <= fromPort)
        {
            pimpl->map.push_back(pimpl->map.size());
        }
    }
    pimpl->map[fromPort] = toPort;
    notify(&MidiMapperListener::MidiMapper_Altered, fromPort);
}


int MidiMapper::maximumMap() const
{
    return pimpl->map.size()-1;
}


MidiEvent MidiMapper::filter(const MidiEvent &m) const
{
    MidiEvent me = m;
    int       p  = me.data.port;
    me.data.port = map(p);
    if (me.data.status == MidiCommand_NoteOn)
    {
        int p = me.offData.port;
        me.offData.port = map(p);
    }
    return me;
}


void MidiMapper::reset()
{
    pimpl->map.clear();
    pimpl->map.push_back(0); // MidiCommand::NoPort
    pimpl->map.push_back(1); // MidiCommand::AllPorts
    notify(&MidiMapperListener::MidiMapper_Altered, 0);
}
