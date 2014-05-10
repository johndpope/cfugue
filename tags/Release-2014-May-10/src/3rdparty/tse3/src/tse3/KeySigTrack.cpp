/*
 * @(#)KeySigTrack.cpp 3.00 30 May 2000
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

#include "tse3/KeySigTrack.h"
#include "tse3/FileBlockParser.h"

#include <sstream>

using namespace TSE3;

/******************************************************************************
 * KeySigTrackIterator class
 *****************************************************************************/

namespace TSE3
{
    /**
     * The @ref PlayableIterator for the @ref KeySigTrack class.
     *
     * @short   KeySigTrack PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     KeySigTrack
     */
    class KeySigTrackIterator : public PlayableIterator,
                                public Listener<KeySigTrackListener>
    {
        public:

            KeySigTrackIterator(KeySigTrack *t, Clock m);
            virtual ~KeySigTrackIterator();
            virtual void moveTo(Clock m);

            virtual void EventTrack_EventAltered(EventTrack<KeySig> *);
            virtual void EventTrack_EventInserted(EventTrack<KeySig> *);
            virtual void EventTrack_EventErased(EventTrack<KeySig> *);
            virtual void Notifier_Deleted(EventTrack<KeySig> *);

        protected:

            virtual void getNextEvent();

            size_t       _pos;
            KeySigTrack *_kstrack;

        private:

            KeySigTrackIterator &operator=(const KeySigTrackIterator &);
            KeySigTrackIterator(const KeySigTrackIterator &);
    };
}


KeySigTrackIterator::KeySigTrackIterator(KeySigTrack *t, Clock c)
: _pos(0), _kstrack(t)
{
    moveTo(c);
    attachTo(_kstrack);
}


KeySigTrackIterator::~KeySigTrackIterator()
{
}


void KeySigTrackIterator::moveTo(Clock c)
{
    if (_kstrack) _pos = _kstrack->index(c);
    if (!_kstrack || _pos == _kstrack->size() || !_kstrack->status())
    {
        _more = false;
        _next = MidiEvent();
    }
    else
    {
        _more = true;
        _next = MidiEvent(MidiCommand(MidiCommand_TSE_Meta, 0, 0,
                                      MidiCommand_TSE_Meta_KeySig,
                                      ((*_kstrack)[_pos].data.incidentals << 4)
                                      | (*_kstrack)[_pos].data.type),
                          (*_kstrack)[_pos].time);
    }
}


void KeySigTrackIterator::getNextEvent()
{
    ++_pos;
    if (_pos == _kstrack->size())
    {
        _more = false;
        _next = MidiEvent();
    }
    else
    {
        _more = true;
        _next = MidiEvent(MidiCommand(MidiCommand_TSE_Meta, 0, 0,
                                      MidiCommand_TSE_Meta_KeySig,
                                      ((*_kstrack)[_pos].data.incidentals << 4)
                                      | (*_kstrack)[_pos].data.type),
                          (*_kstrack)[_pos].time);
    }
}


void KeySigTrackIterator::EventTrack_EventAltered(EventTrack<KeySig> *)
{
    moveTo(_next.time);
}


void KeySigTrackIterator::EventTrack_EventInserted(EventTrack<KeySig> *)
{
    moveTo(_next.time);
}


void KeySigTrackIterator::EventTrack_EventErased(EventTrack<KeySig> *)
{
    moveTo(_next.time);
}


void KeySigTrackIterator::Notifier_Deleted(EventTrack<KeySig> *)
{
    _kstrack = 0;
    _more    = false;
    _next    = MidiEvent();
}


/******************************************************************************
 * KeySigTrack class
 *****************************************************************************/

KeySigTrack::KeySigTrack()
: _status(true)
{
    insert(Event<KeySig>(KeySig(), 0));
}


KeySigTrack::~KeySigTrack()
{
}


PlayableIterator *KeySigTrack::iterator(Clock index)
{
    return new KeySigTrackIterator(this, index);
}


Clock KeySigTrack::lastClock() const
{
    return (!data.empty()) ? data[size()-1].time : Clock(0);
}


/******************************************************************************
 * KeySigTrack Serializable interface
 *****************************************************************************/

void KeySigTrack::save(std::ostream &o, int i) const
{
    o << indent(i)   << "{\n";
    o << indent(i+1) << "Status:";
    if (_status) o << "On\n"; else o << "Off\n";
    o << indent(i+1) << "Events\n";
    o << indent(i+1) << "{\n";
    for (size_t n = 0; n < size(); ++n)
    {
        o << indent(i+2) << data[n].time << ":" << data[n].data.incidentals
          << "/" << data[n].data.type << "\n";
    }
    o << indent(i+1) << "}\n";
    o << indent(i) << "}\n";
}


namespace
{
    /**
     * A catch-all FileItemParser to handle data items in the KeySigTrack
     * Events sub-block.
     */
    class FileItemParser_Events : public FileItemParser
    {
        public:
            FileItemParser_Events(KeySigTrack *kst, int PPQN)
            : kst(kst), PPQN(PPQN) {}
            virtual void parse(const std::string &line)
            {
                int time, incidentals, type;
                std::istringstream si(line);
                si >> time;
                si.ignore(1); // skip colon
                si >> incidentals;
                si.ignore(1); // skip slash
                si >> type;
                time = Clock::convert(time, PPQN);
                kst->insert(Event<KeySig>(KeySig(incidentals, type), time));
            }
        private:
            KeySigTrack *kst;
            int          PPQN;
    };

    /**
     * A simple Serializable class that creates a simple FileBlockParser
     * to load the Events sub-block of the KeySigTrack block.
     */
    class Events : public Serializable
    {
        public:
            Events(KeySigTrack *kst)
            : kst(kst) {}
            virtual void load(std::istream &in, SerializableLoadInfo &info)
            {
                FileItemParser_Events events(kst, info.PPQN);
                FileBlockParser parser;
                parser.add(&events);
                parser.parse(in, info);
            }
        private:
            KeySigTrack *kst;
    };
}


void KeySigTrack::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_OnOff<KeySigTrack> status(this, &KeySigTrack::setStatus);
    Events events(this);
    FileBlockParser parser;
    parser.add("Status", &status);
    parser.add("Events", &events);
    parser.parse(in, info);
}
