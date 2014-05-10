/*
 * @(#)Track.cpp 3.00 20 May 1999
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

#include "tse3/Track.h"

#include "tse3/Part.h"
#include "tse3/MidiFilter.h"
#include "tse3/MidiParams.h"
#include "tse3/DisplayParams.h"
#include "tse3/FileBlockParser.h"
#include "tse3/Error.h"
#include "tse3/Mutex.h"

#include <vector>
#include <algorithm>

using namespace TSE3;

/******************************************************************************
 * TrackIterator class
 *****************************************************************************/

namespace
{
    /**
     * The @ref PlayableIterator for the @ref Track class.
     *
     * @short   Track PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     Track
     */
    class TrackIterator : public PlayableIterator,
                          public Listener<TrackListener>
    {
        public:

            TrackIterator(Track *t, Clock c);
            virtual ~TrackIterator();
            virtual void moveTo(Clock c);

            virtual void Notifier_Deleted(Track *);

        protected:

            virtual void getNextEvent();

            int                 _pos;     // 0 for MidiParams, 1 for Parts
            int                 _partPos; // current Part index
            Track              *_track;
            PlayableIterator   *_mpi;     // MidiParamsIterator
            PlayableIterator   *_pi;      // PartIterator

        private:

            TrackIterator &operator=(const TrackIterator &);
            TrackIterator(const TrackIterator &);
    };
}


TrackIterator::TrackIterator(Track *t, Clock c)
: _pos(0), _partPos(-1), _track(t), _mpi(0), _pi(0)
{
    attachTo(_track);
    _mpi = _track->params()->iterator(0);
    moveTo(c);
}


TrackIterator::~TrackIterator()
{
    delete _mpi;
    delete _pi;
    _pi = 0;
}


void TrackIterator::moveTo(Clock c)
{
    _mpi->moveTo(c);
    _more = true;
    _next = *(*_mpi);
    if (_track)
    {
        _next = _track->filter()->filter(_next);
    }
    _pos  = 0;

    if (_pi)
    {
        delete _pi;
        _pi = 0;
    }

    if (_track)
    {
        _partPos = _track->index(c);
        if (_partPos < static_cast<int>(_track->size()))
        {
            _pi = (*_track)[_partPos]
                      ->iterator(c - (*_track)[_partPos]->start());
        }
    }
}


void TrackIterator::getNextEvent()
{
    if (_pos == 0)
    {
        // MidiParams iterator

        ++(*_mpi);
        if (_mpi->more())
        {
            // Get another event from the params
            _next = *(*_mpi);
            _next = _track->filter()->filter(_next);
            return;
        }
        else
        {
            _pos = 1;
        }
    }

    // Now playing the Parts

    while (1)
    {
        //if (_partPos != static_cast<int>(_track->size()) && _partPos != -1)
        if (_pi)
        {
            ++(*_pi);
            if (_pi->more())
            {
                _next = *(*_pi);
                if (_next.time+(*_track)[_partPos]->start()
                    < (*_track)[_partPos]->end())
                {
                    _next          = _track->filter()->filter(_next);
                    _next          = _track->params()->filter(_next);
                    _next.time    += (*_track)[_partPos]->start();
                    _next.offTime += (*_track)[_partPos]->start();
                    return;
                }
            }
        }

        // Move onto the next Part
        if (_pi)
        {
            delete _pi;
            _pi = 0;
            ++_partPos;
        }
        if (!_track || _partPos >= static_cast<int>(_track->size()))
        {
            _more = false;
            _next = MidiEvent(MidiCommand(), 0);
            return;
        }
        _pi = (*_track)[_partPos]->iterator(0);
    }
}


void TrackIterator::Notifier_Deleted(Track *)
{
    _track = 0;
    moveTo(0);
}


/******************************************************************************
 * TrackImpl class
 *****************************************************************************/

class TSE3::TrackImpl
{
    public:
        std::string         title;
        std::vector<Part*>  parts;
        MidiFilter          filter;
        MidiParams          params;
        DisplayParams       display;
        Song               *song;

        TrackImpl()
        : title("Untitled track"), song(0)
        {}
};


/******************************************************************************
 * Track class
 *****************************************************************************/

Track::Track()
: pimpl(new TrackImpl())
{
    Listener<DisplayParamsListener>::attachTo(&pimpl->display);
}


Track::~Track()
{
    while (pimpl->parts.size())
    {
        Part *p = pimpl->parts[0];
        pimpl->parts.erase(pimpl->parts.begin());
        delete p;
    }
    delete pimpl;
}


const std::string &Track::title() const
{
    return pimpl->title;
}


void Track::setTitle(const std::string &s)
{
    Impl::CritSec cs;

    pimpl->title = s;
    Notifier<TrackListener>::notify(&TrackListener::Track_TitleAltered);
}


MidiFilter *Track::filter()
{
    return &pimpl->filter;
}


MidiParams *Track::params()
{
    return &pimpl->params;
}


DisplayParams *Track::displayParams()
{
    return &pimpl->display;
}


Song *Track::parent() const
{
    return pimpl->song;
}


size_t Track::size() const
{
    return pimpl->parts.size();
}


Part *Track::operator[](size_t n) const
{
    return pimpl->parts[n];
}


Part *Track::insert(Clock start, Clock end)
{
    Part *part = 0;

    if (start > end)
    {
        throw TrackError(PartTimeErr);
    }
    if (numPartsBetween(start, end))
    {
        throw TrackError(PartOverlapErr);
    }

    part = new Part(start, end);
    prvInsertPart(part);
    Notifier<TrackListener>::notify(&TrackListener::Track_PartInserted, part);

    return part;
}


void Track::insert(Part *part)
{
    if (part->parent())
    {
        throw TrackError(PartAlreadyInsertedErr);
    }
    if (part->start() > part->end())
    {
        throw TrackError(PartTimeErr);
    }
    if (numPartsBetween(part->start(), part->end()))
    {
        throw TrackError(PartOverlapErr);
    }

    prvInsertPart(part);
    Notifier<TrackListener>::notify(&TrackListener::Track_PartInserted, part);
}


void Track::prvInsertPart(Part *part)
{
    Impl::CritSec cs;

    part->setParentTrack(this);
    Listener<PartListener>::attachTo(part);

    std::vector<Part*>::iterator i = pimpl->parts.begin();

    while (i != pimpl->parts.end() && (*i)->start() < part->start())
    {
        ++i;
    }

    pimpl->parts.insert(i, part);
}


void Track::remove(Part *part)
{
    Impl::CritSec cs;

    std::vector<Part*>::iterator i
        = find(pimpl->parts.begin(), pimpl->parts.end(), part);
    if (i == pimpl->parts.end()) return;

    pimpl->parts.erase(i);
    part->setParentTrack(0);
    Listener<PartListener>::detachFrom(part);
    Notifier<TrackListener>::notify(&TrackListener::Track_PartRemoved, part);
}


void Track::remove(size_t index)
{
    Impl::CritSec cs;

    if (index >= pimpl->parts.size()) return;

    Part *part = *(pimpl->parts.begin()+index);

    part->setParentTrack(0);
    Listener<PartListener>::detachFrom(part);
    pimpl->parts.erase(pimpl->parts.begin()+index);
    Notifier<TrackListener>::notify(&TrackListener::Track_PartRemoved, part);
}


size_t Track::index(Clock c) const
{
    Impl::CritSec cs;

    std::vector<Part*>::const_iterator i = pimpl->parts.begin();
    while (i != pimpl->parts.end() && c >= (*i)->end())
    {
        ++i;
    }
    return (i == pimpl->parts.end()) ? size() : i - pimpl->parts.begin();
}


size_t Track::index(Part *part) const
{
    Impl::CritSec cs;

    std::vector<Part*>::const_iterator i
        = find(pimpl->parts.begin(), pimpl->parts.end(), part);
    return i - pimpl->parts.begin();
}


int Track::operator<(const Track &t) const
{
    return pimpl->title < t.pimpl->title;
}


void Track::Notifier_Deleted(Part *part)
{
    Impl::CritSec cs;

    std::vector<Part*>::iterator i
        = find(pimpl->parts.begin(), pimpl->parts.end(), part);

    if (i != pimpl->parts.end())
    {
        pimpl->parts.erase(i);
        Notifier<TrackListener>::notify
            (&TrackListener::Track_PartRemoved, part);
    }
}


PlayableIterator *Track::iterator(Clock index)
{
    return new TrackIterator(this, index);
}


Clock Track::lastClock() const
{
    Impl::CritSec cs;

    return (!pimpl->parts.empty())
               ? pimpl->parts[size()-1]->lastClock()
               : Clock(0);
}


void Track::setParentSong(Song *parent)
{
    Impl::CritSec cs;

    pimpl->song = parent;
    Notifier<TrackListener>::notify(&TrackListener::Track_Reparented);
}


size_t Track::numPartsBetween(Clock start, Clock end)
{
    Impl::CritSec cs;

    size_t count = 0;
    size_t pos   = index(start);
    while (pos != pimpl->parts.size() && pimpl->parts[pos]->start() < end)
    {
        ++count;
        ++pos;
    }
    return count;
}


void Track::DisplayParams_Altered(DisplayParams *)
{
    Notifier<TrackListener>::notify(&TrackListener::Track_DisplayParamsAltered);
}


/******************************************************************************
 * Track Serializable interface
 *****************************************************************************/

void Track::save(std::ostream &o, int i) const
{
    o << indent(i)   << "{\n";
    o << indent(i+1) << "Title:" << pimpl->title << "\n";
    o << indent(i+1) << "MidiFilter\n";
    pimpl->filter.save(o,i+1);
    o << indent(i+1) << "MidiParams\n";
    pimpl->params.save(o, i+1);
    o << indent(i+1) << "DisplayParams\n";
    pimpl->display.save(o, i+1);
    o << indent(i+1) << "NoParts:" << pimpl->parts.size() << "\n";
    std::vector<Part*>::const_iterator p = pimpl->parts.begin();
    while (p != pimpl->parts.end())
    {
        o << indent(i+1) << "Part\n";
        (*p)->save(o, i+1);
        ++p;
    }
    o << indent(i)   << "}\n";
}


namespace
{
    /**
     * A simple Serializable class that creates a simple FileBlockParser
     * to load the Events sub-block of the TempoTrack block.
     */
    class PartLoader : public Serializable
    {
        public:
            PartLoader(Track *t)
            : t(t) {}
            virtual void load(std::istream &in, SerializableLoadInfo &info)
            {
                Part *part = new Part;
                part->load(in, info);
                t->insert(part);
            }
        private:
            Track *t;
    };
}


void Track::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_String<Track> title(this, &Track::setTitle);
    PartLoader partLoader(this);
    FileBlockParser parser;
    parser.add("Title",      &title);
    parser.add("MidiFilter", &pimpl->filter);
    parser.add("MidiParams", &pimpl->params);
    parser.add("DisplayParams",  &pimpl->display);
    //parser.add("NoParts",  &nothing); // we ignore this data line
    parser.add("Part",       &partLoader);
    parser.parse(in, info);
}

