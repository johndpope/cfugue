/*
 * @(#)Part.cpp 3.00 25 May 1999
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

#include "tse3/Part.h"

#include "tse3/Song.h"
#include "tse3/Track.h"
#include "tse3/FileBlockParser.h"
#include "tse3/Error.h"
#include "tse3/Phrase.h"
#include "tse3/MidiParams.h"
#include "tse3/DisplayParams.h"
#include "tse3/MidiFilter.h"
#include "tse3/PhraseList.h"
#include "tse3/Mutex.h"

#include <sstream>

using namespace TSE3;

/******************************************************************************
 * PartIterator class
 *****************************************************************************/

namespace TSE3
{
    /**
     * The @ref PlayableIterator for the @ref Part class.
     *
     * @short   Part PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     Part
     */
    class PartIterator : public PlayableIterator,
                         public Listener<PartListener>
    {
        public:

            /**
             * Creates a PartIterator for the given @ref Part starting at
             * the given @Clock.
             */
            PartIterator(Part *p, Clock c);
            virtual ~PartIterator();
            virtual void moveTo(Clock c);

            virtual void Part_PhraseAltered(Part *, Phrase *phrase);
            virtual void Notifier_Deleted(Part *);

        protected:

            virtual void getNextEvent();

        private:

            int                 _pos; // 0 for MidiParams, 1 for MidiData
            Part               *_part;
            PlayableIterator   *_mpi;  // MidiParamsIterator
            PlayableIterator   *_mdi;  // MidiDataIterator
            Clock               _startTime; // time Phrase started

            PartIterator &operator=(const PartIterator &);
            PartIterator(const PartIterator &);
    };
}


PartIterator::PartIterator(Part *p, Clock c)
: _pos(0), _part(p), _mdi(0)
{
    _mpi = p->params()->iterator(0);
    _mdi = _part->phrase() ? _part->phrase()->iterator(0) : 0;
    moveTo(c);
    attachTo(_part);
}


PartIterator::~PartIterator()
{
    delete _mpi;
    delete _mdi;
}


void PartIterator::moveTo(Clock c)
{
    _mpi->moveTo(c);
    _more = true;
    _next = *(*_mpi);
    _next = _part->filter()->filter(_next);
    _pos  = 0;

    _startTime = 0;

    if (_part && _part->repeat())
    {
        while (_startTime + _part->repeat() < c)
        {
            _startTime += _part->repeat();
        }
    }
    if (_part && _mdi)
    {
        _mdi->moveTo(c - _startTime);
    }
}


void PartIterator::getNextEvent()
{
    if (_pos == 0)
    {
        // MidiParams iterator

        ++(*_mpi);
        if (_mpi->more())
        {
            // Get another event from the params
            _next = *(*_mpi);
            _next = _part->filter()->filter(_next);
            return;
        }
        else
        {
            _pos = 1;
            if (_mdi)
            {
                _more = _mdi->more();
                _next = *(*_mdi);
            }
        }
    }
    else if (_part && _mdi)
    {
        ++(*_mdi);

        _more = _mdi->more();
        _next = *(*_mdi);
    }
    else
    {
        _more = false;
        _next = MidiEvent();
        return;
    }

    if (_part && _mdi)
    {
        if (_part->repeat())
        {
            if (!_more || (_next.time >= _part->repeat()))
            {
                _startTime += _part->repeat();
                _mdi->moveTo(0);
                _more = true;
                _next = *(*_mdi);
            }
        }

        _next = _part->filter()->filter(_next);
        _next = _part->params()->filter(_next);

        _next.time    += _startTime;
        _next.offTime += _startTime;
    }
}


void PartIterator::Part_PhraseAltered(Part *, Phrase *phrase)
{
    delete _mdi;
    _mdi = phrase ? phrase->iterator(0) : 0;
    moveTo(_next.time);
}


void PartIterator::Notifier_Deleted(Part *)
{
    _part = 0;
    if (_mdi)
    {
        delete _mdi;
        _mdi = 0;
    }
    moveTo(1); // 1, so we don't send MidiParams again
}


/******************************************************************************
 * PartImpl class
 *****************************************************************************/

class TSE3::PartImpl
{
    public:
        Clock          start;
        Clock          end;
        Clock          repeat;
        Phrase        *phrase;
        MidiFilter     filter;
        MidiParams     params;
        DisplayParams  display;
        Track         *track;

        PartImpl()
        : start(0), end(Clock::PPQN), repeat(0), phrase(0), track(0)
        {}

        PartImpl(Clock start, Clock end)
        : start(start), end(end), repeat(0), phrase(0), track(0)
        {}
};


/******************************************************************************
 * Part class
 *****************************************************************************/

Part::Part()
: pimpl(new PartImpl())
{
    Listener<MidiFilterListener>::attachTo(&pimpl->filter);
    Listener<MidiParamsListener>::attachTo(&pimpl->params);
    Listener<DisplayParamsListener>::attachTo(&pimpl->display);
}


Part::Part(Clock start, Clock end)
: pimpl(new PartImpl(start, end))
{
    if (start > end)
    {
        throw PartError(PartTimeErr);
    }
    Listener<MidiFilterListener>::attachTo(&pimpl->filter);
    Listener<MidiParamsListener>::attachTo(&pimpl->params);
    Listener<DisplayParamsListener>::attachTo(&pimpl->display);
}


Part::Part(const Part &p)
: Playable(),
  Listener<PhraseListener>(),
  Listener<MidiFilterListener>(),
  Listener<MidiParamsListener>(),
  Listener<DisplayParamsListener>(),
  Serializable(), Notifier<PartListener>(),
  pimpl(new PartImpl(*(p.pimpl)))
{
    pimpl->track = 0;
    if (pimpl->phrase) Listener<PhraseListener>::attachTo(pimpl->phrase);
    Listener<MidiFilterListener>::attachTo(&pimpl->filter);
    Listener<MidiParamsListener>::attachTo(&pimpl->params);
    Listener<DisplayParamsListener>::attachTo(&pimpl->display);
}


Part::~Part()
{
    delete pimpl;
}


MidiFilter *Part::filter()
{
    return &pimpl->filter;
}


MidiParams *Part::params()
{
    return &pimpl->params;

}


DisplayParams *Part::displayParams()
{
    return &pimpl->display;

}


Phrase *Part::phrase() const
{
    return pimpl->phrase;
}


void Part::setPhrase(Phrase *p)
{
    Impl::CritSec cs;

    if (p && !p->parent())
    {
        throw PartError(PhraseUnparentedErr);
    }

    if (pimpl->phrase)
    {
        Listener<PhraseListener>::detachFrom(pimpl->phrase);
    }

    pimpl->phrase = p;
    if (p)
    {
        Listener<PhraseListener>::attachTo(p);
    }

    Notifier<PartListener>::notify(&PartListener::Part_PhraseAltered, p);
}


Track *Part::parent() const
{
    return pimpl->track;
}


Clock Part::start() const
{
    return pimpl->start;
}


void Part::setStart(Clock c)
{
    Impl::CritSec cs;

    if (c < 0 || c == pimpl->start) return;
    if (pimpl->track && c > pimpl->end)
    {
        throw PartError(PartTimeErr);
    }

    if (!pimpl->track)
    {
        // Part is not in a Track, this is an easy operation
        pimpl->start = c;
        Notifier<PartListener>::notify(&PartListener::Part_StartAltered, c);
    }
    else
    {
        // Part is in a Track. Try to move it in the Track, and tidy
        // up if the move fails.
        Clock  oldStart = pimpl->start;
        Track *parent   = pimpl->track;
        parent->remove(this);
        pimpl->start = c;
        try
        {
            parent->insert(this);
        }
        catch (Error)
        {
            pimpl->start = oldStart;

            // We know this will not fail...
            parent->insert(this);

            throw;
        }
        Notifier<PartListener>::notify(&PartListener::Part_StartAltered, c);
    }
}


Clock Part::end() const
{
    return pimpl->end;
}


void Part::setEnd(Clock c)
{
    Impl::CritSec cs;

    if (c < 0 || c == pimpl->end) return;
    if (pimpl->track && c < pimpl->start)
    {
        throw PartError(PartTimeErr);
    }

    if (!pimpl->track)
    {
        // Part is not in a Track, this is an easy operation
        pimpl->end = c;
        Notifier<PartListener>::notify(&PartListener::Part_EndAltered, c);
    }
    else
    {
        // Part is in a Track. Try to move it in the Track, and tidy
        // up if the move fails.
        Clock oldEnd  = pimpl->end;
        Track *parent = pimpl->track;
        parent->remove(this);
        pimpl->end = c;
        try
        {
            parent->insert(this);
        }
        catch (Error)
        {
            pimpl->end = oldEnd;

            // We know this will not fail...
            parent->insert(this);

            throw;
        }
        Notifier<PartListener>::notify(&PartListener::Part_EndAltered, c);
    }
}


void Part::setStartEnd(Clock start, Clock end)
{
    Impl::CritSec cs;

    if (start == pimpl->start && end == pimpl->end)
    {
        return;
    }
    if (start > end || start < 0 || end < 0)
    {
        throw PartError(PartTimeErr);
    }

    if (!pimpl->track)
    {
        // Part is not in a Track, this is an easy operation
        pimpl->start = start;
        pimpl->end   = end;
        Notifier<PartListener>::notify(&PartListener::Part_StartAltered, start);
        Notifier<PartListener>::notify(&PartListener::Part_EndAltered,   end);
    }
    else
    {
        // Part is in a Track. Try to move it in the Track, and tidy
        // up if the move fails.
        Clock  oldStart = pimpl->start;
        Clock  oldEnd   = pimpl->end;
        Track *parent   = pimpl->track;
        parent->remove(this);
        pimpl->start = start;
        pimpl->end   = end;
        try
        {
            parent->insert(this);
        }
        catch (Error)
        {
            pimpl->start = oldStart;
            pimpl->end   = oldEnd;

            // We know this will not fail...
            parent->insert(this);

            throw;
        }
        Notifier<PartListener>::notify(&PartListener::Part_StartAltered, start);
        Notifier<PartListener>::notify(&PartListener::Part_EndAltered,   end);
    }
}


Clock Part::repeat() const
{
    return pimpl->repeat;
}


void Part::setRepeat(Clock r)
{
    Impl::CritSec cs;

    if (r >= 0 && r != pimpl->repeat)
    {
        pimpl->repeat = r;
        Notifier<PartListener>::notify(&PartListener::Part_RepeatAltered, r);
    }
}


void Part::Notifier_Deleted(Phrase *phrase)
{
    if (phrase != pimpl->phrase)
    {
        return; // error
    }

    pimpl->phrase = 0;
    Notifier<PartListener>::notify(&PartListener::Part_PhraseAltered,
                                   (Phrase*)0);
}


void Part::Phrase_Reparented(Phrase *phrase)
{
    if (phrase == pimpl->phrase)
    {
        Listener<PhraseListener>::detachFrom(pimpl->phrase);
        pimpl->phrase = 0;
        Notifier<PartListener>::notify(&PartListener::Part_PhraseAltered,
                                       (Phrase*)0);
    }
}


PlayableIterator *Part::iterator(Clock index)
{
    return new PartIterator(this, index);
}


Clock Part::lastClock() const
{
    return pimpl->end;
}


void Part::setParentTrack(Track *parent)
{
    Impl::CritSec cs;

    pimpl->track = parent;
    Notifier<PartListener>::notify(&PartListener::Part_Reparented);
}


void Part::MidiFilter_Altered(MidiFilter *, int what)
{
    Notifier<PartListener>::notify
        (&PartListener::Part_MidiFilterAltered, what);
}


void Part::MidiParams_Altered(MidiParams *, int what)
{
    Notifier<PartListener>::notify
        (&PartListener::Part_MidiParamsAltered, what);
}


void Part::DisplayParams_Altered(DisplayParams *)
{
    Notifier<PartListener>::notify
        (&PartListener::Part_DisplayParamsAltered);
}


/******************************************************************************
 * Part Serializable interface
 *****************************************************************************/

void Part::save(std::ostream &o, int i) const
{
    o << indent(i)   << "{\n";
    o << indent(i+1) << "MidiFilter\n";
    pimpl->filter.save(o,i+1);
    o << indent(i+1) << "MidiParams\n";
    pimpl->params.save(o, i+1);
    o << indent(i+1) << "DisplayParams\n";
    pimpl->display.save(o, i+1);
    o << indent(i+1) << "Phrase:" << pimpl->phrase->title() << "\n";
    o << indent(i+1) << "Start:"  << pimpl->start           << "\n";
    o << indent(i+1) << "End:"    << pimpl->end             << "\n";
    o << indent(i+1) << "Repeat:" << pimpl->repeat          << "\n";
    o << indent(i)   << "}\n";
}


namespace
{
    /**
     * A catch-all FileItemParser to handle the Phrase data item in a Part.
     */
    class FileItemParser_Phrase : public FileItemParser
    {
        public:
            FileItemParser_Phrase(Part *p, Song *s)
            : p(p), s(s) {}
            virtual void parse(const std::string &data)
            {
                Phrase *phrase = s->phraseList()->phrase(data);
                if (phrase) p->setPhrase(phrase);
            }
        private:
            Part *p;
            Song *s;
    };

    /**
     * A FileItemParser that reads an int and calls either the setStart
     * or setEnd Part methods with the Part::NoOverlap parameter.
     */
    class FileItemParser_StartEnd : public FileItemParser
    {
        public:
            typedef void (Part::*fn_t)(Clock);
            FileItemParser_StartEnd(Part *p, fn_t mfun)
            : p(p), mfun(mfun) {}
            virtual void parse(const std::string &data)
            {
                int i;
                std::istringstream si(data);
                si >> i;
                (p->*mfun)(i);
            }
        private:
            Part *p;
            fn_t  mfun;
    };

}


void Part::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_StartEnd    start(this,  &Part::setStart);
    FileItemParser_StartEnd    end(this,    &Part::setEnd);
    FileItemParser_Clock<Part> repeat(this, &Part::setRepeat);
    FileItemParser_Phrase      phraseLoader(this, info.song);
    FileBlockParser            parser;
    parser.add("Start",          &start);
    parser.add("End",            &end);
    parser.add("Repeat",         &repeat);
    parser.add("MidiFilter",     &pimpl->filter);
    parser.add("MidiParams",     &pimpl->params);
    parser.add("DisplayParams",  &pimpl->display);
    parser.add("Phrase",         &phraseLoader);
    parser.parse(in, info);
}
