/*
 * @(#)Phrase.cpp 3.00 7 June 1999
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

#include "tse3/Phrase.h"

#include "tse3/PhraseList.h"
#include "tse3/Error.h"
#include "tse3/Mutex.h"
#include "tse3/util/NoteNumber.h"

using namespace TSE3;

/******************************************************************************
 * Phrase class
 *****************************************************************************/

Phrase::Phrase(const int noEvents)
: MidiData(noEvents), _title(""), _parent(0)
{
    Listener<DisplayParamsListener>::attachTo(&_display);
}


Phrase::~Phrase()
{
}


void Phrase::setTitle(const std::string &t)
{
    Impl::CritSec cs;

    if (_parent && _parent->phrase(t))
    {
        throw PhraseListError(PhraseNameExistsErr);
    }

    _title = t;
    if (_parent) _parent->phraseTitleChanged(this);

    Notifier<PhraseListener>::notify(&PhraseListener::Phrase_TitleAltered);
}


void Phrase::setParent(PhraseList *parent)
{
    _parent = parent;
    Notifier<PhraseListener>::notify(&PhraseListener::Phrase_Reparented);
}


void Phrase::DisplayParams_Altered(DisplayParams *)
{
    Notifier<PhraseListener>::notify
        (&PhraseListener::Phrase_DisplayParamsAltered);
}


/******************************************************************************
 * Phrase Serializable interface
 *****************************************************************************/

void Phrase::save(std::ostream &o, int i) const
{
    o << indent(i)   << "{\n";
    o << indent(i+1) << "Title:" << _title << "\n";
    o << indent(i+1) << "DisplayParams\n";
    _display.save(o, i+1);
    o << indent(i+1) << "Events\n";
    o << indent(i+1) << "{\n";
    for (std::vector<MidiEvent>::size_type n = 0; n < data.size(); ++n) {
        o << indent(i+2) << data[n].time << ":"
          << data[n].data.status << "/"
          << data[n].data.data1 << "/"
          << data[n].data.data2 << "/"
          << data[n].data.channel << "/"
          << data[n].data.port;
        if (data[n].data.status == MidiCommand_NoteOn)
            o << "-" << data[n].offTime << ":"
              << data[n].offData.status << "/"
              << data[n].offData.data1 << "/"
              << data[n].offData.data2 << "/"
              << data[n].offData.channel << "/"
              << data[n].offData.port;
        if (data[n].data.isNote())
        {
            o << "        # " << TSE3::Util::numberToNote(data[n].data.data1);
        }
        o << "\n";
    }
    o << indent(i+1) << "}\n";
    o << indent(i)   << "}\n";
}
