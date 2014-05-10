/*
 * @(#)RepeatTrack.cpp 3.00 22 June 1999
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

#include "tse3/RepeatTrack.h"
#include "tse3/FileBlockParser.h"

#include <sstream>

using namespace TSE3;

/******************************************************************************
 * RepeatTrackIterator class
 *****************************************************************************/

namespace TSE3
{
    /**
     * The @ref PlayableIterator for the @ref RepeatTrack class.
     *
     * @short   RepeatTrack PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     RepeatTrack
     */
    class RepeatTrackIterator : public PlayableIterator,
                                public Listener<RepeatTrackListener>
    {
        public:

            /**
             * Create a RepeatTrackIterator for the given @ref RepeatTrack
             * starting at the given @ref Clock.
             */
            RepeatTrackIterator(RepeatTrack *r, Clock c);
            ~RepeatTrackIterator();
            virtual void moveTo(Clock c);

            virtual void EventTrack_EventAltered(EventTrack<Repeat> *);
            virtual void EventTrack_EventInserted(EventTrack<Repeat> *);
            virtual void EventTrack_EventErased(EventTrack<Repeat> *);
            virtual void Notifier_Deleted(EventTrack<Repeat> *);

        protected:

            virtual void getNextEvent();

            size_t       _pos;
            RepeatTrack *_rtrack;

        private:

            RepeatTrackIterator &operator=(const RepeatTrackIterator &);
            RepeatTrackIterator(const RepeatTrackIterator &);
    };
}


RepeatTrackIterator::RepeatTrackIterator(RepeatTrack *r, Clock c)
: _pos(0), _rtrack(r)
{
    moveTo(c);
    attachTo(_rtrack);
}


RepeatTrackIterator::~RepeatTrackIterator()
{
}


void RepeatTrackIterator::moveTo(Clock c)
{
    if (_rtrack) _pos = _rtrack->index(c);
    if (!_rtrack || _pos == _rtrack->size() || !_rtrack->status())
    {
        _more = false;
        _next = MidiEvent();
    }
    else
    {
        _more = true;
        _next = MidiEvent(MidiCommand(MidiCommand_TSE_Meta, 0, 0,
                                      MidiCommand_TSE_Meta_MoveTo, 0),
                          (*_rtrack)[_pos].time,
                          MidiCommand(MidiCommand_Invalid, 0, 0, 0, 0),
                          (*_rtrack)[_pos].data.repeat);
    }
}


void RepeatTrackIterator::getNextEvent()
{
    ++_pos;
    if (_pos == _rtrack->size())
    {
        _more = false;
        _next = MidiEvent();
    }
    else
    {
        _more = true;
        if ((*_rtrack)[_pos].data.status)
        {
          _next = MidiEvent(MidiCommand(MidiCommand_TSE_Meta, 0, 0,
                                        MidiCommand_TSE_Meta_MoveTo, 0),
                            (*_rtrack)[_pos].time,
                            MidiCommand(MidiCommand_Invalid, 0, 0, 0, 0),
                            (*_rtrack)[_pos].data.repeat);
        }
        else
        {
            _next = MidiEvent(MidiCommand(), (*_rtrack)[_pos].time);
        }
    }
}


void RepeatTrackIterator::EventTrack_EventAltered(EventTrack<Repeat> *)
{
    moveTo(_next.time);
}


void RepeatTrackIterator::EventTrack_EventInserted(EventTrack<Repeat> *)
{
    moveTo(_next.time);
}


void RepeatTrackIterator::EventTrack_EventErased(EventTrack<Repeat> *)
{
    moveTo(_next.time);
}


void RepeatTrackIterator::Notifier_Deleted(EventTrack<Repeat> *)
{
    _rtrack = 0;
    _more   = false;
    _next   = MidiEvent();
}


/******************************************************************************
 * RepeatTrack class
 *****************************************************************************/

RepeatTrack::RepeatTrack()
: _status(false)
{
    insert(Event<Repeat>(Repeat(), Clock::PPQN*4));
}


RepeatTrack::~RepeatTrack()
{
}


PlayableIterator *RepeatTrack::iterator(Clock index)
{
    return new RepeatTrackIterator(this, index);
}


Clock RepeatTrack::lastClock() const
{
    return (!data.empty()) ? data[size()-1].time : Clock(0);
}


/******************************************************************************
 * RepeatTrack Serializable interface
 *****************************************************************************/

void RepeatTrack::save(std::ostream &o, int i) const
{
    o << indent(i)   << "{\n";
    o << indent(i+1) << "Status:";
    if (_status) o << "On\n"; else o << "Off\n";
    o << indent(i+1) << "Events\n";
    o << indent(i+1) << "{\n";
    for (size_t n = 0; n < size(); ++n) {
        o << indent(i+2) << data[n].time << ":" << data[n].data.repeat
          << ":";
        if (data[n].data.status) o << "On\n"; else o << "Off\n";
        o << "\n";
    }
    o << indent(i+1) << "}\n";
    o << indent(i) << "}\n";
}


namespace
{
    /**
     * A catch-all FileItemParser to handle data items in the RepeatTrack
     * Events sub-block.
     */
    class FileItemParser_Events : public FileItemParser
    {
        public:
            FileItemParser_Events(RepeatTrack *rt, int PPQN)
            : rt(rt), PPQN(PPQN) {}
            virtual void parse(const std::string &line)
            {
                int time, repeat;
                bool status = true;
                std::istringstream si(line);
                si >> time;
                si.ignore(1); // skip colon
                si >> repeat;
                si.ignore(1); // skip colon
                std::string statusline;
                getline(si, statusline);
                if (statusline != "On") status = false;
                time = Clock::convert(time, PPQN);
                rt->insert(Event<Repeat>(Repeat(repeat, status), time));
            }
        private:
            RepeatTrack *rt;
            int          PPQN;
    };

    /**
     * A simple Serializable class that creates a simple FileBlockParser
     * to load the Events sub-block of the RepeatTrack block.
     */
    class Events : public Serializable
    {
        public:
            Events(RepeatTrack *rt)
            : rt(rt) {}
            virtual void load(std::istream &in, SerializableLoadInfo &info)
            {
                FileItemParser_Events events(rt, info.PPQN);
                FileBlockParser parser;
                parser.add(&events);
                parser.parse(in, info);
            }
        private:
            RepeatTrack *rt;
    };
}


void RepeatTrack::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_OnOff<RepeatTrack> status(this, &RepeatTrack::setStatus);
    Events events(this);
    FileBlockParser parser;
    parser.add("Status", &status);
    parser.add("Events", &events);
    parser.parse(in, info);
}
