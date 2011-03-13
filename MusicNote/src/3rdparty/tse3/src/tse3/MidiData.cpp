/*
 * @(#)MidiData.cpp 3.00 17 May 1999
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

#include "tse3/MidiData.h"

#include "tse3/Mutex.h"

using namespace TSE3;

/******************************************************************************
 * MidiDataIterator class
 *****************************************************************************/

namespace TSE3
{
    /**
     * The @ref PlayableIterator for the @ref MidiData class.
     *
     * @short   MidiData PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     MidiData
     */
    class MidiDataIterator : public PlayableIterator,
                             public Listener<MidiDataListener>
    {
        public:

            /**
             * Create a MidiDataIterator for the given @MidiData object
             * starting at the given @ref Clock.
             */
            MidiDataIterator(MidiData *md, Clock c);
            ~MidiDataIterator();
            virtual void moveTo(Clock c);

            virtual void Notifier_Deleted(MidiData *);

        protected:

            virtual void getNextEvent();

            size_t    _pos;
            MidiData *_data;

        private:

            MidiDataIterator &operator=(const MidiDataIterator &);
            MidiDataIterator(const MidiDataIterator &m);
    };
}


MidiDataIterator::MidiDataIterator(MidiData *md, Clock c)
: _pos(0), _data(md)
{
    moveTo(c);
    attachTo(_data);
}


MidiDataIterator::~MidiDataIterator()
{
}


void MidiDataIterator::moveTo(Clock c)
{
    if (_data) _pos = _data->index(c);
    if (!_data || _pos == _data->size())
    {
        _more = false;
        _next = MidiEvent();
    }
    else
    {
        _more = true;
        _next = (*_data)[_pos];
    }
}


void MidiDataIterator::getNextEvent()
{
    if (_more)
    {
        ++_pos;
        if (_pos == _data->size())
            _more = false;
        else
            _next = (*_data)[_pos];
    }
}


void MidiDataIterator::Notifier_Deleted(MidiData *)
{
    _data = 0;
    moveTo(0);
}


/******************************************************************************
 * MidiData class
 *****************************************************************************/

MidiData::MidiData(int noEvents)
{
    data.reserve(noEvents);
}


MidiData::~MidiData()
{
}


size_t MidiData::index(Clock c) const
{
    Impl::CritSec cs;

    std::vector<MidiEvent>::const_iterator i = data.begin();
    while (i != data.end() && !(c <= (*i).time))
    {
        ++i;
    }
    return (i == data.end()) ? size() : i - data.begin();
}


PlayableIterator *MidiData::iterator(Clock index)
{
    return new MidiDataIterator(this, index);
}


Clock MidiData::lastClock() const
{
    return data.size() > 0 ? data[data.size()-1].time : TSE3::Clock(0);
}

