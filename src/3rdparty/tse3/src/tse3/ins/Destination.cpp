/*
 * @(#)Destination.h 3.00 21 August 2000
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

#include "tse3/ins/Destination.h"

#include "tse3/ins/Instrument.h"
#include "tse3/Midi.h"

#include <vector>
#include <algorithm>
#include <map>

using namespace TSE3::Ins;

/******************************************************************************
 * DestinationImpl class
 *****************************************************************************/

namespace
{
    struct DestinationInfo
    {
        DestinationInfo() : allChannels(false)
        {
            for (int n = 0; n < 16; ++n) instruments[n] = 0;
        }
        bool        allChannels;
        Instrument *instruments[16];
    };
}

class TSE3::Ins::DestinationImpl
{
    public:

        Instrument                 *defaultInstrument;
        std::vector<Instrument *>   ilist;

        typedef std::map<int, DestinationInfo> dmap_type;
        dmap_type dmap;
};


/******************************************************************************
 * Destination class
 *****************************************************************************/

Destination::Destination()
: pimpl(new DestinationImpl)
{
    pimpl->defaultInstrument = 0;
}


Destination::~Destination()
{
    delete pimpl;
}


Instrument *Destination::defaultInstrument() const
{
    return pimpl->defaultInstrument;
}


void Destination::setDefaultInstrument(Instrument *instrument)
{
    pimpl->defaultInstrument = instrument;
}


bool Destination::allChannels(int port)
{
    DestinationImpl::dmap_type::iterator i = pimpl->dmap.find(port);
    if (i != pimpl->dmap.end())
    {
        return i->second.allChannels;
    }
    else
    {
        return true;
    }
}


Instrument *Destination::port(int port)
{
    DestinationImpl::dmap_type::iterator i = pimpl->dmap.find(port);
    if (i != pimpl->dmap.end())
    {
        Instrument *instrument = i->second.allChannels
                               ? i->second.instruments[0]
                               : 0;
        return instrument ? instrument : pimpl->defaultInstrument;
    }
    else
    {
        return pimpl->defaultInstrument;
    }
}


void Destination::setPort(int port, Instrument *instrument)
{
    if (instrument)
    {
        pimpl->dmap[port].allChannels    = true;
        pimpl->dmap[port].instruments[0] = instrument;
    }
    else
    {
        pimpl->dmap.erase(port);
    }
    notify(&DestinationListener::Destination_Altered,
           TSE3::MidiCommand::AllChannels, port, instrument);
}


Instrument *Destination::channel(int channel, int port)
{
    DestinationImpl::dmap_type::iterator i = pimpl->dmap.find(port);
    if (i != pimpl->dmap.end() && channel >= 0 && channel < 16)
    {
        if (i->second.allChannels) channel = 0;
        Instrument *instrument = i->second.instruments[channel];
        return instrument ? instrument : pimpl->defaultInstrument;
    }
    else
    {
        return pimpl->defaultInstrument;
    }
}


void Destination::setChannel(int channel, int port,
                             Instrument *instrument)
{
    if (channel < 0 || channel >= 16) return;

    DestinationImpl::dmap_type::iterator i = pimpl->dmap.find(port);
    if (i != pimpl->dmap.end())
    {
        if (i->second.allChannels)
        {
            for (int n = 1; n < 16; n++)
            {
                i->second.instruments[n] = i->second.instruments[0];
                if (n != channel)
                {
                    notify(&DestinationListener::Destination_Altered, n, port,
                           i->second.instruments[0]);
                }
            }
        }
    }
    pimpl->dmap[port].allChannels          = false;
    pimpl->dmap[port].instruments[channel] = instrument;

    notify(&DestinationListener::Destination_Altered, channel, port,
           instrument);
}


/******************************************************************************
 * Destination class: managing the ilist
 *****************************************************************************/

size_t Destination::numInstruments() const
{
    return pimpl->ilist.size();
}


Instrument *Destination::instrument(size_t index)
{
    if (index < pimpl->ilist.size())
    {
        return pimpl->ilist[index];
    }
    else
    {
        return 0;
    }
}


Instrument *Destination::instrument(const std::string &title)
{
    std::vector<Instrument*>::iterator i = pimpl->ilist.begin();
    while (i != pimpl->ilist.end() && (*i)->title() != title)
    {
        ++i;
    }
    return (i == pimpl->ilist.end()) ? 0 : *i;
}


void Destination::addInstrument(Instrument *instrument)
{
    std::vector<Instrument*>::iterator i = pimpl->ilist.begin();
    while (i != pimpl->ilist.end() && (*i)->title() < instrument->title())
    {
        ++i;
    }

    if (i == pimpl->ilist.end() || instrument->title() != (*i)->title())
    {
        pimpl->ilist.insert(i, instrument);
        notify(&DestinationListener::Destination_InstrumentAdded, instrument);
    };
}


void Destination::removeInstrument(Instrument *instrument)
{
    // Check its in the managed list
    std::vector<Instrument*>::iterator i
        = find(pimpl->ilist.begin(), pimpl->ilist.end(), instrument);
    if (i == pimpl->ilist.end()) return;

    // Check every destination
    DestinationImpl::dmap_type::iterator di = pimpl->dmap.begin();
    while (di != pimpl->dmap.end())
    {
        int maxc = (di->second.allChannels) ? 1 : 16;
        for (int channel = 0; channel < maxc; channel++)
        {
            if (di->second.instruments[channel] == instrument)
            {
                di->second.instruments[channel] = 0;
                notify(&DestinationListener::Destination_Altered,
                       channel, di->first, (Instrument*)0);
            }
        }
        ++di;
    }
    if (pimpl->defaultInstrument == instrument)
    {
        pimpl->defaultInstrument = 0;
    }

    pimpl->ilist.erase(i);
    notify(&DestinationListener::Destination_InstrumentRemoved, instrument);
}

