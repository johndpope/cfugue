/*
 * @(#)TimeSigTrack.cpp 3.00 24 May 1999
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

#include "tse3/TimeSigTrack.h"
#include "tse3/FileBlockParser.h"

#include <sstream>

using namespace TSE3;

/******************************************************************************
 * TimeSigTrackIterator class
 *****************************************************************************/

namespace TSE3
{
    /**
     * The @ref PlayableIterator for the @ref TimeSigTrack class.
     *
     * @short   TimeSigTrack PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     TimeSigTrack
     */
    class TimeSigTrackIterator : public PlayableIterator,
                                 public Listener<TimeSigTrackListener>
    {
        public:

            TimeSigTrackIterator(TimeSigTrack *t, Clock c);
            virtual ~TimeSigTrackIterator();
            virtual void moveTo(Clock c);

            virtual void EventTrack_EventAltered(EventTrack<TimeSig> *);
            virtual void EventTrack_EventInserted(EventTrack<TimeSig> *);
            virtual void EventTrack_EventErased(EventTrack<TimeSig> *);
            virtual void Notifier_Deleted(EventTrack<TimeSig> *);

        protected:

            virtual void getNextEvent();

            size_t        _pos;
            TimeSigTrack *_tstrack;

        private:

            TimeSigTrackIterator &operator=(const TimeSigTrackIterator &);
            TimeSigTrackIterator(const TimeSigTrackIterator &);
    };
}


TimeSigTrackIterator::TimeSigTrackIterator(TimeSigTrack *t, Clock c)
: _pos(0), _tstrack(t)
{
    moveTo(c);
    attachTo(_tstrack);
}


TimeSigTrackIterator::~TimeSigTrackIterator()
{
}


void TimeSigTrackIterator::moveTo(Clock c)
{
    if (_tstrack) _pos = _tstrack->index(c);
    if (!_tstrack || _pos == _tstrack->size() || !_tstrack->status())
    {
        _more = false;
        _next = MidiEvent();
    }
    else
    {
        _more = true;
        _next = MidiEvent(MidiCommand(MidiCommand_TSE_Meta, 0, 0,
                                      MidiCommand_TSE_Meta_TimeSig,
                                      ((*_tstrack)[_pos].data.top << 4)
                                      | (*_tstrack)[_pos].data.bottom),
                          (*_tstrack)[_pos].time);
    }
}


void TimeSigTrackIterator::getNextEvent()
{
    ++_pos;
    if (_pos == _tstrack->size())
    {
        _more = false;
        _next = MidiEvent();
    }
    else
    {
        _more = true;
        _next = MidiEvent(MidiCommand(MidiCommand_TSE_Meta, 0, 0,
                                      MidiCommand_TSE_Meta_TimeSig,
                                      ((*_tstrack)[_pos].data.top << 4)
                                      | (*_tstrack)[_pos].data.bottom),
                          (*_tstrack)[_pos].time);
    }
}


void TimeSigTrackIterator::EventTrack_EventAltered(EventTrack<TimeSig> *)
{
    moveTo(_next.time);
}


void TimeSigTrackIterator::EventTrack_EventInserted(EventTrack<TimeSig> *)
{
    moveTo(_next.time);
}


void TimeSigTrackIterator::EventTrack_EventErased(EventTrack<TimeSig> *)
{
    moveTo(_next.time);
}


void TimeSigTrackIterator::Notifier_Deleted(EventTrack<TimeSig> *)
{
    _tstrack = 0;
    _more    = false;
    _next    = MidiEvent();
}


/******************************************************************************
 * TimeSigTrack class
 *****************************************************************************/

TimeSigTrack::TimeSigTrack()
: _status(true)
{
    insert(Event<TimeSig>(TimeSig(), 0));
}


TimeSigTrack::~TimeSigTrack()
{
}


PlayableIterator *TimeSigTrack::iterator(Clock index)
{
    return new TimeSigTrackIterator(this, index);
}


Clock TimeSigTrack::lastClock() const
{
    return (!data.empty()) ? data[size()-1].time : Clock(0);
}


void TimeSigTrack::barBeatPulse(Clock time, int &bar, int &beat, int &pulse)
{
    if (data.empty())
    {
        bar   = 0;
        beat  = 0;
        pulse = time;
        return;
    }

    // Find the relevant TimeSig index, plus calculate bar along the way
    bar = 0;
    std::vector<event_type>::iterator i = data.begin(), j = i;
    while (i != data.end() && i->time < time)
    {
        ++i;
        if (i != data.end() && i->time < time)
        {
            const Clock fudge = Clock::PPQN*4/j->data.bottom / 2;
                // we add this fudge to cope with no-aligned TimeSig changes

            bar += (i->time - j->time + fudge)
                 / (Clock::PPQN*4*j->data.top)
                 * j->data.bottom;
        }
        ++j;
    }
    if (i != data.begin())
    {
        // Back pedal to the relevant TimeSig
        i--;
    }

    // Calculate the number of bars into this TimeSig

    const Clock beatlen = Clock::PPQN*4 / i->data.bottom;
    const Clock barlen  = beatlen * i->data.top;

    bar += (time - i->time) / barlen;

    const Clock barstart = (i->time + bar*barlen);
    beat = (time-barstart) / beatlen;

    const Clock beatstart = (i->time + bar*barlen + beat*beatlen);
    pulse = time - beatstart;
}


/******************************************************************************
 * TimeSigTrack Serializable interface
 *****************************************************************************/

void TimeSigTrack::save(std::ostream &o, int i) const
{
    o << indent(i)   << "{\n";
    o << indent(i+1) << "Status:";
    if (_status) o << "On\n"; else o << "Off\n";
    o << indent(i+1) << "Events\n";
    o << indent(i+1) << "{\n";
    for (size_t n = 0; n < size(); ++n)
    {
        o << indent(i+2) << data[n].time << ":" << data[n].data.top
          << "/" << data[n].data.bottom << "\n";
    }
    o << indent(i+1) << "}\n";
    o << indent(i) << "}\n";
}


namespace
{
    /**
     * A catch-all FileItemParser to handle data items in the TimeSigTrack
     * Events sub-block.
     */
    class FileItemParser_Events : public FileItemParser
    {
        public:
            FileItemParser_Events(TimeSigTrack *tst, int PPQN)
            : tst(tst), PPQN(PPQN) {}
            virtual void parse(const std::string &line)
            {
                int time, top, bottom;
                std::istringstream si(line);
                si >> time;
                si.ignore(1); // skip colon
                si >> top;
                si.ignore(1); // skip slash
                si >> bottom;
                time = Clock::convert(time, PPQN);
                tst->insert(Event<TimeSig>(TimeSig(top, bottom), time));
            }
        private:
            TimeSigTrack *tst;
            int           PPQN;
    };

    /**
     * A simple Serializable class that creates a simple FileBlockParser
     * to load the Events sub-block of the TimeSigTrack block.
     */
    class Events : public Serializable
    {
        public:
            Events(TimeSigTrack *tst)
            : tst(tst) {}
            virtual void load(std::istream &in, SerializableLoadInfo &info)
            {
                FileItemParser_Events events(tst, info.PPQN);
                FileBlockParser parser;
                parser.add(&events);
                parser.parse(in, info);
            }
        private:
            TimeSigTrack *tst;
    };
}


void TimeSigTrack::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_OnOff<TimeSigTrack> status(this, &TimeSigTrack::setStatus);
    Events events(this);
    FileBlockParser parser;
    parser.add("Status", &status);
    parser.add("Events", &events);
    parser.parse(in, info);
}
