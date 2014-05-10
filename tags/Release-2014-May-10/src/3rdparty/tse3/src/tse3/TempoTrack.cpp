/*
 * @(#)TempoTrack.cpp 3.00 24 May 1999
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

#include "tse3/TempoTrack.h"
#include "tse3/FileBlockParser.h"

#include <sstream>

using namespace TSE3;

/******************************************************************************
 * TempoTrackIterator class
 *****************************************************************************/

namespace TSE3
{
    /**
     * The @ref PlayableIterator for the @ref TempoTrack class.
     *
     * @short   TempoTrack PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     TempoTrack
     */
    class TempoTrackIterator : public PlayableIterator,
                               public Listener<TempoTrackListener>
    {
        public:

            TempoTrackIterator(TempoTrack *t, Clock c);
            virtual ~TempoTrackIterator();
            virtual void moveTo(Clock c);

            virtual void EventTrack_EventAltered(EventTrack<Tempo> *);
            virtual void EventTrack_EventInserted(EventTrack<Tempo> *);
            virtual void EventTrack_EventErased(EventTrack<Tempo> *);
            virtual void Notifier_Deleted(EventTrack<Tempo> *);

        protected:

            virtual void getNextEvent();

            size_t      _pos;
            TempoTrack *_ttrack;

        private:

            TempoTrackIterator &operator=(const TempoTrackIterator &);
            TempoTrackIterator(const TempoTrackIterator &);
    };
}


TempoTrackIterator::TempoTrackIterator(TempoTrack *t, Clock c)
: _pos(0), _ttrack(t)
{
    moveTo(c);
    attachTo(_ttrack);
}


TempoTrackIterator::~TempoTrackIterator()
{
}


void TempoTrackIterator::moveTo(Clock c)
{
    if (_ttrack) _pos = _ttrack->index(c);
    if (!_ttrack || _pos == _ttrack->size() || !_ttrack->status())
    {
        _more = false;
        _next = MidiEvent();
    }
    else
    {
        _more = true;
        _next = MidiEvent(MidiCommand(MidiCommand_TSE_Meta, 0, 0,
                                      MidiCommand_TSE_Meta_Tempo,
                                      (*_ttrack)[_pos].data.tempo),
                         (*_ttrack)[_pos].time);
    }
}


void TempoTrackIterator::getNextEvent()
{
    ++_pos;
    if (_pos == _ttrack->size())
    {
        _more = false;
        _next = MidiEvent();
    }
    else
    {
        _more = true;
        _next = MidiEvent(MidiCommand(MidiCommand_TSE_Meta, 0, 0,
                                      MidiCommand_TSE_Meta_Tempo,
                                      (*_ttrack)[_pos].data.tempo),
                         (*_ttrack)[_pos].time);
    }
}


void TempoTrackIterator::EventTrack_EventAltered(EventTrack<Tempo> *)
{
    moveTo(_next.time);
}


void TempoTrackIterator::EventTrack_EventInserted(EventTrack<Tempo> *)
{
    moveTo(_next.time);
}


void TempoTrackIterator::EventTrack_EventErased(EventTrack<Tempo> *)
{
    moveTo(_next.time);
}


void TempoTrackIterator::Notifier_Deleted(EventTrack<Tempo> *)
{
    _ttrack = 0;
    _more   = false;
    _next   = MidiEvent();
}


/******************************************************************************
 * TempoTrack class
 *****************************************************************************/

TempoTrack::TempoTrack()
: _status(true)
{
    insert(Event<Tempo>(Tempo(), 0));
}


TempoTrack::~TempoTrack()
{
}


PlayableIterator *TempoTrack::iterator(Clock index)
{
    return new TempoTrackIterator(this, index);
}


Clock TempoTrack::lastClock() const
{
    return (!data.empty()) ? data[size()-1].time : Clock(0);
}


/******************************************************************************
 * TempoTrack Serializable interface
 *****************************************************************************/

void TempoTrack::save(std::ostream &o, int i) const
{
    o << indent(i)   << "{\n";
    o << indent(i+1) << "Status:";
    if (_status)
        o << "On\n";
    else
        o << "Off\n";
    o << indent(i+1) << "Events\n";
    o << indent(i+1) << "{\n";
    for (size_t n = 0; n < size(); ++n)
    {
        o << indent(i+2) << data[n].time << ":" << data[n].data.tempo << "\n";
    }
    o << indent(i+1) << "}\n";
    o << indent(i) << "}\n";
}


namespace
{
    /**
     * A catch-all FileItemParser to handle data items in the TempoTrack
     * Events sub-block.
     */
    class FileItemParser_Events : public FileItemParser
    {
        public:
            FileItemParser_Events(TempoTrack *tt, int PPQN)
            : tt(tt), PPQN(PPQN) {}
            virtual void parse(const std::string &line)
            {
                int time, tempo;
                std::istringstream si(line);
                si >> time;
                si.ignore(1); // skip colon
                si >> tempo;
                time = Clock::convert(time, PPQN);
                tt->insert(Event<Tempo>(Tempo(tempo), time));
            }
        private:
            TempoTrack *tt;
            int         PPQN;
    };

    /**
     * A simple Serializable class that creates a simple FileBlockParser
     * to load the Events sub-block of the TempoTrack block.
     */
    class Events : public Serializable
    {
        public:
            Events(TempoTrack *tt)
            : tt(tt) {}
            virtual void load(std::istream &in, SerializableLoadInfo &info)
            {
                FileItemParser_Events events(tt, info.PPQN);
                FileBlockParser parser;
                parser.add(&events);
                parser.parse(in, info);
            }
        private:
            TempoTrack *tt;
    };
}


void TempoTrack::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_OnOff<TempoTrack> status(this, &TempoTrack::setStatus);
    Events events(this);
    FileBlockParser parser;
    parser.add("Status", &status);
    parser.add("Events", &events);
    parser.parse(in, info);
}
