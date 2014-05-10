/*
 * @(#)PhraseEdit.cpp 3.00 24 May 1999
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

#include "tse3/PhraseEdit.h"

#include "tse3/Phrase.h"
#include "tse3/PhraseList.h"
#include "tse3/Error.h"
#include "tse3/Mutex.h"

#include <functional>
#include <algorithm>
#include <iterator>
using namespace TSE3;

/******************************************************************************
 * PhraseEdit class
 *****************************************************************************/

PhraseEdit::PhraseEdit(int noEvents)
: MidiData(noEvents), hint(0), _selection(false)
{
}


PhraseEdit::~PhraseEdit()
{
}


void PhraseEdit::reset(const MidiData *source)
{
    Impl::CritSec cs;

    data.clear();
    hint = 0;

    if (source)
    {
        copy(source->data.begin(), source->data.end(), back_inserter(data));
    }

    updateSelectionInfo();
    Notifier<PhraseEditListener>::notify(&PhraseEditListener::PhraseEdit_Reset);
    setModified(false);
}


void PhraseEdit::tidy(Clock endTime)
{
    Impl::CritSec cs;

    // Events are not necessarily already in time order: we do need to
    // reshuffle them just in case
    sort(data.begin(), data.end());

    // Find the last time in the data, we'll use it later
    if (endTime == -1) endTime = data[size()-1].time;

    // Deal with all events with a minus time
    while (size() && data[0].time < -tollerance)
    {
        data.erase(data.begin());
    }
    for (size_t n = 0; n < size() && data[n].time < 0; ++n)
    {
        data[0].time = 0;
        if (data[0].offTime < 0) data[0].offTime = 0;
    }

    // Convert all MidiCommand_NoteOns with velocity 0 to MidiCommand_NoteOffs
    bool altered = false;
    for (size_t n = 0; n < size(); ++n)
    {
        if (data[n].data.status == MidiCommand_NoteOn
            && data[n].data.data2 == 0)
        {
            data[n].data.status = MidiCommand_NoteOff;
        }
    }

    // Deal with the sustain pedal: strip it out and elongate notes accordingly
    for (int pos = size()-1; pos >= 0; --pos)
    {
        if (data[pos].data.status == MidiCommand_ControlChange
            && data[pos].data.data1 == MidiControl_SustainPedal
            && data[pos].data.data2 >= 0x40)
        {
            int susChannel = data[pos].data.channel;
            // We've found a sustain pedal down controller,
            // now find the related sustain pedal up controller
            size_t offPos = pos+1;
            while (offPos < size()
                   && !(data[offPos].data.status == MidiCommand_ControlChange
                       && data[offPos].data.data1 == MidiControl_SustainPedal
                       && data[offPos].data.data2 < 0x40))
            {
                ++offPos;
            }
            Clock susOffTime = (offPos < size()) ? data[offPos].time : endTime;
            if(offPos < size()) data.erase(data.begin()+offPos); // Fix: Gopalakrishna Palem
            data.erase(data.begin()+pos);
            --offPos;
            for (size_t n = pos; n < offPos; ++n)
            {
                if (data[n].data.status == MidiCommand_NoteOff
                    && data[n].data.channel == susChannel)
                {
                    data[n].time = susOffTime;
                    altered = true;
                }
            }
        }
    }
    if (altered) sort(data.begin(), data.end());

    // Remove any sustain pedal up events still hanging around
    for (int n = size()-1; n >= 0; --n)
    {
        if (data[n].data.status == MidiCommand_ControlChange
            && data[n].data.data1 == MidiControl_SustainPedal
            && data[n].data.data2 <  0x40)
        {
            data.erase(data.begin()+n);
        }
    }

    // Pull all MidiCommand_NoteOffs into their MidiCommand_NoteOn's MidiEvent
    for (size_t pos = 0; pos < size(); ++pos)
    {
        if (data[pos].data.status == MidiCommand_NoteOn
            && data[pos].offData.status == MidiCommand_Invalid)
        {
            size_t subPos = pos+1;
            while (subPos < size()
                   && !(data[subPos].data.status == MidiCommand_NoteOff
                        && data[subPos].data.data1 == data[pos].data.data1))
            {
                ++subPos;
            }
            if (subPos < size())
            {
                // There was a matching MidiCommand_NoteOff. Move it in.
                data[pos].offData = data[subPos].data;
                data[pos].offTime = data[subPos].time;
                data.erase(data.begin()+subPos);
            }
            else
            {
                // There was no matching MidiCommand_NoteOff. Make one up.
                data[pos].offData        = data[pos].data;
                data[pos].offData.status = MidiCommand_NoteOff;
                data[pos].offTime        = endTime;
            }
        }
    }

    // If there are unmatched MidiCommand_NoteOffs, remove them
    for (int n = size()-1; n >= 0; --n)
    {
        if (data[n].data.status == MidiCommand_NoteOff)
        {
            data.erase(data.begin()+n);
        }
    }

    updateSelectionInfo();
    Notifier<PhraseEditListener>::notify
        (&PhraseEditListener::PhraseEdit_Tidied);

    setModified(true);
}


void PhraseEdit::timeShift(Clock delta)
{
    for (size_t pos = 0; pos < size(); ++pos)
    {
        data[pos].time    += delta;
        data[pos].offTime += delta;
    }
    setModified(true);
}


Phrase *PhraseEdit::createPhrase(PhraseList *pl, const std::string &title) const
{
    Impl::CritSec cs;

    if (title.size() && pl->phrase(title))
    {
        throw PhraseListError(PhraseNameExistsErr);
    }

    Phrase *phr = new Phrase(size());
    copy(data.begin(), data.end(), back_inserter(phr->data));

    if (title.size() == 0)
    {
        phr->setTitle(pl->newPhraseTitle(PhraseList::newPhraseString));
    }
    else
    {
        phr->setTitle(title);
    }

    pl->insert(phr);

    return phr;
}


void PhraseEdit::insert(MidiEvent event)
{
    Impl::CritSec cs;

    if (event.data.status == MidiCommand_Invalid) return;

    // Find position to insert at (attempting to use cached position)
    if (hint < data.size())
    {
        if (data[hint].time > event.time) hint = 0;
    }
    else
    {
        hint = 0;
    }
    typedef std::vector<MidiEvent>::iterator DI;
    DI i = std::find_if(data.begin()+hint, data.end(),
                        std::bind2nd(std::greater<MidiEvent>(), event));
    hint = i - data.begin();

    // Insert event at this position
    data.insert(i, event);
    Notifier<PhraseEditListener>::notify
        (&PhraseEditListener::PhraseEdit_Inserted, hint);

    // Deal with selection status
    if (hint <= _firstSelectionIndex) ++_firstSelectionIndex;
    if (hint >= _lastSelectionIndex)  ++_lastSelectionIndex;
    if (event.data.selected)
    {
        selected(hint, true);
    }

    setModified(true);
}


void PhraseEdit::erase(size_t n)
{
    Impl::CritSec cs;

    if (data[n].data.selected)
    {
        data[n].data.selected = false;
        selected(n, false);
    }

    data.erase(data.begin() + n);
    hint = 0;
    if (n <= _firstSelectionIndex) --_firstSelectionIndex;
    if (n <= _lastSelectionIndex)  --_lastSelectionIndex;
    Notifier<PhraseEditListener>::notify
        (&PhraseEditListener::PhraseEdit_Erased, n);

    setModified(true);
}


void PhraseEdit::erase(MidiEvent event)
{
    Impl::CritSec cs;

    std::vector<MidiEvent>::iterator i =
       std::find_if(data.begin(), data.end(), MidiEvent::equal_to(event));
    if (i != data.end())
    {
        size_t index = i - data.begin();

        if (data[index].data.selected)
        {
            data[index].data.selected = false;
            selected(index, false);
        }

        data.erase(i);
        hint = 0;
        if (index <= _firstSelectionIndex) --_firstSelectionIndex;
        if (index <= _lastSelectionIndex)  --_lastSelectionIndex;
        Notifier<PhraseEditListener>::notify
            (&PhraseEditListener::PhraseEdit_Erased, index);
    }

    setModified(true);
}


/******************************************************************************
 * Accessing the selection
 *****************************************************************************/

void PhraseEdit::select(size_t index)
{
    if (!data[index].data.selected && index < size())
    {
        data[index].data.selected = true;
        selected(index, true);
    }
}


void PhraseEdit::selectRange(size_t from, size_t to)
{
    for (size_t n = from; n < to; ++n)
    {
        select(n);
    }
}


void PhraseEdit::deselect(size_t index)
{
    if (data[index].data.selected && index < size())
    {
        data[index].data.selected = false;
        selected(index, false);
    }
}


void PhraseEdit::clearSelection()
{
    for (size_t n = 0; n < size(); ++n)
    {
        deselect(n);
    }
}


void PhraseEdit::invertSelection()
{
    for (size_t n = 0; n < size(); ++n)
    {
        if (data[n].data.selected)
        {
            data[n].data.selected = false;
            selected(n, false);
        }
        else
        {
            data[n].data.selected = true;
            selected(n, true);
        }
    }
}


void PhraseEdit::eraseSelection()
{
    while (_selection)
    {
        erase(_firstSelectionIndex);
    }
}


void PhraseEdit::updateSelectionInfo()
{
    _selection = false;
    for (size_t n = 0; n < size(); ++n)
    {
        if (data[n].data.selected)
        {
            if (!_selection)
            {
                _selection           = true;
                _firstSelectionIndex = n;
            }
            _lastSelectionIndex = n;
        }
    }
}


void PhraseEdit::selected(size_t index, bool sel)
{
    if (sel)
    {
        if (!_selection)
        {
            _firstSelectionIndex = _lastSelectionIndex = index;
            _selection           = true;
        }
        else if (index < _firstSelectionIndex)
        {
            _firstSelectionIndex = index;
        }
        else if (index > _lastSelectionIndex)
        {
            _lastSelectionIndex = index;
        }
    }
    else
    {
        if (index == _firstSelectionIndex
            && _firstSelectionIndex == _lastSelectionIndex)
        {
            _selection = false;
        }
        else if (index == _firstSelectionIndex)
        {
            size_t n = index;
            while (n < data.size() && !data[n].data.selected) ++n;
            _firstSelectionIndex = n;
        }
        else if (index == _lastSelectionIndex)
        {
            size_t n = index;
            while (n && !data[n].data.selected) --n;
            _lastSelectionIndex = n;
        }
    }

    Notifier<PhraseEditListener>::notify
        (&PhraseEditListener::PhraseEdit_Selection, index, sel);
}


/******************************************************************************
 * Accessing the modification
 *****************************************************************************/

void PhraseEdit::modified(bool m)
{
    _modified = m;
    Notifier<PhraseEditListener>::notify
        (&PhraseEditListener::PhraseEdit_Modified, m);
}

