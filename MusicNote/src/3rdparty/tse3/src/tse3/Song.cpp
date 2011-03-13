/*
 * @(#)Song.cpp 3.00 26 May 1999
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

#include "tse3/Song.h"

#include "tse3/Track.h"
#include "tse3/TempoTrack.h"
#include "tse3/TimeSigTrack.h"
#include "tse3/KeySigTrack.h"
#include "tse3/PhraseList.h"
#include "tse3/FlagTrack.h"
#include "tse3/FileBlockParser.h"
#include "tse3/Error.h"
#include "tse3/Mutex.h"

using namespace TSE3;

/******************************************************************************
 * RepeatIterator class
 *****************************************************************************/

namespace TSE3
{
    /**
     * The @ref PlayableIterator for the @ref Song class' repeat parameter.
     *
     * @short   Song PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     Song
     */
    class RepeatIterator : public PlayableIterator,
                           public Listener<SongListener>
    {
        public:

            RepeatIterator(Song *s, Clock c);
            virtual ~RepeatIterator();
            virtual void moveTo(Clock c);

            virtual void Song_RepeatAltered(Song *);
            virtual void Song_FromAltered(Song *);
            virtual void Song_ToAltered(Song *);
            virtual void Notifier_Deleted(Song *);

        protected:

            virtual void getNextEvent();
            void         updateIterators(Clock c);

            Song *_song;

        private:

            RepeatIterator &operator=(const RepeatIterator &);
            RepeatIterator(const RepeatIterator &);
    };
}


RepeatIterator::RepeatIterator(Song *s, Clock c)
: _song(s)
{
    moveTo(c);
    attachTo(_song);
}


RepeatIterator::~RepeatIterator()
{
}


void RepeatIterator::moveTo(Clock c)
{
    if (!_song || !_song->repeat() || _song->to() < c)
    {
        _more = false;
        _next = MidiEvent();
    }
    else
    {
        _more = true;
        _next = MidiEvent(MidiCommand(MidiCommand_TSE_Meta, 0, 0,
                                      MidiCommand_TSE_Meta_MoveTo, 0),
                          _song->to(),
                          MidiCommand(MidiCommand_Invalid, 0, 0, 0, 0),
                          _song->from());
    }
}


void RepeatIterator::getNextEvent()
{
    _more = false;
    _next = MidiEvent();
}


void RepeatIterator::Song_RepeatAltered(Song *)
{
    moveTo(_next.time);
}


void RepeatIterator::Song_FromAltered(Song *)
{
    moveTo(_next.time);
}


void RepeatIterator::Song_ToAltered(Song *)
{
    moveTo(_next.time);
}


void RepeatIterator::Notifier_Deleted(Song *)
{
    _song = 0;
    _more = false;
    _next = MidiEvent();
}


/******************************************************************************
 * SongIterator class
 *****************************************************************************/

namespace
{
    /**
     * The @ref PlayableIterator for the @ref Song class.
     *
     * @short   Song PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     Song
     */
    class SongIterator : public PlayableIterator,
                         public Listener<SongListener>
    {
        public:

            SongIterator(Song *s, Clock c);
            virtual ~SongIterator();
            virtual void moveTo(Clock c);

            virtual void Notifier_Deleted(Song *);

        protected:

            virtual void getNextEvent();
            void         updateIterators(Clock c);

            Song                           *_song;
            std::vector<PlayableIterator*>  _ti;      // TrackIterators
            PlayableIterator               *_tti;     // TempoTrackIterator
            PlayableIterator               *_tsti;    // TimeSigTrackIterator
            PlayableIterator               *_ksti;    // KeySigTrackIterator
            PlayableIterator               *_rti;     // RepeatIterator
            int                             _source;  // where _next came from

            /**
             * An enum type that describes where the current event came
             * from.
             */
            enum IteratorSource
            {
                TempoTrack   = -1,
                TimeSigTrack = -2,
                KeySigTrack  = -3,
                RepeatEvent  = -4,
                None         = -5
            };

        private:

            SongIterator &operator=(const SongIterator &);
            SongIterator(const SongIterator &);
    };
}


SongIterator::SongIterator(Song *s, Clock c)
: _song(s), _source(None)
{
    _tti  = _song->tempoTrack()->iterator(c);
    _tsti = _song->timeSigTrack()->iterator(c);
    _ksti = _song->keySigTrack()->iterator(c);
    _rti  = new RepeatIterator(_song, c);
    updateIterators(c);
    moveTo(c);
    attachTo(_song);
}


SongIterator::~SongIterator()
{
    _song = 0;
    updateIterators(0);
    delete _tti;
    delete _tsti;
    delete _ksti;
    delete _rti;
}


void SongIterator::updateIterators(Clock c)
{
    while (!_ti.empty())
    {
        PlayableIterator *ti = *(_ti.begin());
        _ti.erase(_ti.begin());
        delete ti;
    }
    if (!_song) return;
    for (size_t n = 0; n < _song->size(); ++n)
    {
        PlayableIterator *pi = (*_song)[n]->iterator(c);
        _ti.push_back(pi);
    }
}


void SongIterator::moveTo(Clock c)
{
    if (_tti)  _tti->moveTo(c);
    if (_tsti) _tsti->moveTo(c);
    if (_ksti) _ksti->moveTo(c);
    if (_rti)  _rti->moveTo(c);
    std::vector<PlayableIterator*>::iterator i = _ti.begin();
    while (i != _ti.end())
    {
        (*i)->moveTo(c);
        ++i;
    }
    _more   = true;
    _source = None;
    getNextEvent();
}


void SongIterator::getNextEvent()
{
    // Consume the last event
    if (_source == TempoTrack)
    {
        if (_tti)
        {
            ++(*_tti);
        }
    }
    else if (_source == TimeSigTrack)
    {
        if (_tsti)
        {
            ++(*_tsti);
        }
    }
    else if (_source == KeySigTrack)
    {
        if (_ksti)
        {
            ++(*_ksti);
        }
    }
    else if (_source == RepeatEvent)
    {
        if (_rti)
        {
            ++(*_rti);
        }
    }
    else if (_source != None)
    {
        ++(*(_ti[_source]));
    }

    // Get the next event
    _more   = false;
    _source = None;
    _next   = MidiEvent();

    if (_tti && _tti->more())
    {
        _more   = true;
        _next   = *(*_tti);
        _source = TempoTrack;
    }

    if (_tsti && _tsti->more())
    {
        MidiEvent tmp = *(*_tsti);
        if (_more == false || tmp.time < _next.time)
        {
            _next   = tmp;
            _source = TimeSigTrack;
        }
        _more = true;
    }

    if (_ksti && _ksti->more())
    {
        MidiEvent tmp = *(*_ksti);
        if (_more == false || tmp.time < _next.time)
        {
            _next   = tmp;
            _source = KeySigTrack;
        }
        _more = true;
    }

    if (_rti && _rti->more())
    {
        MidiEvent tmp = *(*_rti);
        if (_more == false || tmp.time < _next.time)
        {
            _next   = tmp;
            _source = RepeatEvent;
        }
        _more = true;
    }

    std::vector<PlayableIterator*>::iterator i = _ti.begin();
    int trackNo = 0;
    while (i != _ti.end())
    {
        if ((*i)->more())
        {
            MidiEvent tmp = *(*(*i));
            if (_song->soloTrack() != -1
                && _song->soloTrack() != trackNo)
            {
                tmp.data.status = MidiCommand_Invalid;
            }
            if (_more == false || tmp.time < _next.time)
            {
                _next   = tmp;
                _source = i - _ti.begin();
            }
            _more = true;
        }
        ++i;
        ++trackNo;
    }
}


void SongIterator::Notifier_Deleted(Song *)
{
    _song = 0;
    updateIterators(0);
    moveTo(0);
}


/******************************************************************************
 * SongImpl class
 *****************************************************************************/

class TSE3::SongImpl
{
    public:
        std::string          title;
        std::string          author;
        std::string          copyright;
        std::string          date;

        PhraseList           phraseList;
        TempoTrack           tempoTrack;
        TimeSigTrack         timeSigTrack;
        KeySigTrack          keySigTrack;
        FlagTrack            flagTrack;

        std::vector<Track *> tracks;
        int                  soloTrack; // -1 for no solo
        bool                 repeat;
        Clock                from;
        Clock                to;
        Clock                lastClock;

        SongImpl()
        : title("Title"), author("Author"),
          copyright("Copyright message"), date("Date"),
          soloTrack(-1), repeat(false), from(0), to(Clock::PPQN*4),
          lastClock(0)
        {}
};


/******************************************************************************
 * Song class
 *****************************************************************************/

Song::Song(int noTracks)
: pimpl(new SongImpl())
{
    while (noTracks)
    {
        Track *t = new Track();
        attachTo(t);
        t->setParentSong(this);
        pimpl->tracks.insert(pimpl->tracks.end(), t);
        --noTracks;
    }
}


Song::~Song()
{
    while (!pimpl->tracks.empty())
    {
        Track *t = pimpl->tracks[0];
        pimpl->tracks.erase(pimpl->tracks.begin());
        t->setParentSong(0);
        delete t;
    }
    delete pimpl;
}


PhraseList *Song::phraseList()
{
    return &pimpl->phraseList;
}


TempoTrack *Song::tempoTrack()
{
    return &pimpl->tempoTrack;
}


TimeSigTrack *Song::timeSigTrack()
{
    return &pimpl->timeSigTrack;
}


KeySigTrack *Song::keySigTrack()
{
    return &pimpl->keySigTrack;
}


FlagTrack *Song::flagTrack()
{
    return &pimpl->flagTrack;
}


const std::string &Song::title() const
{
    return pimpl->title;
}


void Song::setTitle(const std::string &s)
{
    Impl::CritSec cs;

    if (pimpl->title != s)
    {
        pimpl->title = s;
        Notifier<SongListener>::notify(&SongListener::Song_InfoAltered);
    }
}


const std::string &Song::author() const
{
    return pimpl->author;
}


void Song::setAuthor(const std::string &s)
{
    Impl::CritSec cs;

    if (pimpl->author != s)
    {
        pimpl->author = s;
        Notifier<SongListener>::notify(&SongListener::Song_InfoAltered);
    }
}


const std::string &Song::copyright() const
{
    return pimpl->copyright;
}


void Song::setCopyright(const std::string &s)
{
    Impl::CritSec cs;

    if (pimpl->copyright != s)
    {
        pimpl->copyright = s;
        Notifier<SongListener>::notify(&SongListener::Song_InfoAltered);
    }
}


const std::string &Song::date() const
{
    return pimpl->date;
}


void Song::setDate(const std::string &s)
{
    Impl::CritSec cs;

    if (pimpl->date != s)
    {
        pimpl->date = s;
        Notifier<SongListener>::notify(&SongListener::Song_InfoAltered);
    }
}


size_t Song::size() const
{
    return pimpl->tracks.size();
}


Track *Song::operator[](size_t n) const
{
    return pimpl->tracks[n];
}


size_t Song::index(Track *track) const
{
    Impl::CritSec cs;

    std::vector<Track*>::const_iterator i
        = std::find(pimpl->tracks.begin(), pimpl->tracks.end(), track);
    return i - pimpl->tracks.begin();
}


Track *Song::insert(int n)
{
    Track *t = new Track();
    {
        Impl::CritSec cs;

        if (n == -1 || n > (int)size())
        {
            n = size();
        }
        std::vector<Track*>::iterator i = pimpl->tracks.begin() + n;
        pimpl->tracks.insert(i, t);
        attachTo(t);
        t->setParentSong(this);
    }
    Notifier<SongListener>::notify(&SongListener::Song_TrackInserted, t);
    return t;
}


void Song::insert(Track *t, int n)
{
    {
        Impl::CritSec cs;

        if (t->parent())
        {
            throw SongError(TrackAlreadyInsertedErr);
        }

        if (n == -1 || n > (int)size())
        {
            n = size();
        }
        std::vector<Track*>::iterator i = pimpl->tracks.begin() + n;
        attachTo(t);
        t->setParentSong(this);
        pimpl->tracks.insert(i, t);
    }
    Notifier<SongListener>::notify(&SongListener::Song_TrackInserted, t);
}


void Song::remove(size_t n)
{
    Track *t = 0;
    {
        Impl::CritSec cs;

        if (n < size())
        {
            std::vector<Track*>::iterator i = pimpl->tracks.begin() + n;
            t = *i;
            pimpl->tracks.erase(i);
            detachFrom(t);
            t->setParentSong(0);
        }
    }
    if (t)
    {
        Notifier<SongListener>::notify(&SongListener::Song_TrackRemoved, t, n);
    }
}


void Song::remove(Track *t)
{
    size_t index = 0;
    {
        Impl::CritSec cs;

        std::vector<Track*>::iterator i
            = std::find(pimpl->tracks.begin(), pimpl->tracks.end(), t);
        if (i != pimpl->tracks.end())
        {
            index = i - pimpl->tracks.begin();
            pimpl->tracks.erase(i);
            detachFrom(t);
            t->setParentSong(0);
        }
        else
        {
            t = 0;
        }
    }
    if (t)
    {
        Notifier<SongListener>::notify(&SongListener::Song_TrackRemoved,
                                       t, index);
    }
}


int Song::soloTrack() const
{
    return pimpl->soloTrack;
}


void Song::setSoloTrack(int t)
{
    Impl::CritSec cs;

    if (t < -1 || t >= (int) pimpl->tracks.size() || pimpl->soloTrack == t)
    {
        return;
    }
    pimpl->soloTrack = t;
    Notifier<SongListener>::notify(&SongListener::Song_SoloTrackAltered, t);
}


bool Song::repeat() const
{
    return pimpl->repeat;
}


void Song::setRepeat(bool r)
{
    Impl::CritSec cs;

    if (pimpl->repeat != r)
    {
        pimpl->repeat = r;
        Notifier<SongListener>::notify(&SongListener::Song_RepeatAltered, r);
    }
}


Clock Song::from() const
{
    return pimpl->from;
}


void Song::setFrom(Clock f)
{
    Impl::CritSec cs;

    if (pimpl->from != f)
    {
        pimpl->from = f;
        Notifier<SongListener>::notify(&SongListener::Song_FromAltered, f);
    }
}


Clock Song::to() const
{
    return pimpl->to;
}


void Song::setTo(Clock t)
{
    Impl::CritSec cs;

    if (pimpl->to != t)
    {
        pimpl->to = t;
        Notifier<SongListener>::notify(&SongListener::Song_ToAltered, t);
    }
}


void Song::Notifier_Deleted(Track *source)
{
    Impl::CritSec cs;

    std::vector<Track*>::iterator i
        = std::find(pimpl->tracks.begin(), pimpl->tracks.end(), source);
    if (i == pimpl->tracks.end()) return; // It might be another child but we
                                          // don't care. Heartless.

    // Look out for the solo track
    int trackNo = i - pimpl->tracks.begin();
    if (trackNo == pimpl->soloTrack)
    {
        pimpl->soloTrack = -1;
        Notifier<SongListener>::notify
            (&SongListener::Song_SoloTrackAltered, -1);
    }
    else if (trackNo < pimpl->soloTrack)
    {
        --pimpl->soloTrack;
    }

    pimpl->tracks.erase(i);
}


PlayableIterator *Song::iterator(Clock index)
{
    return new SongIterator(this, index);
}


Clock Song::lastClock() const
{
    return pimpl->lastClock;
}


void Song::recalculateLastClock() const
{
    Impl::CritSec cs;

    pimpl->lastClock = 0;
    std::vector<Track *>::const_iterator i = pimpl->tracks.begin();
    while (i != pimpl->tracks.end())
    {
        if ((*i)->lastClock() > pimpl->lastClock)
        {
            pimpl->lastClock = (*i)->lastClock();
        }
        ++i;
    }
    // Should I include the TempoTrack?
    // Should I include the TimeSigTrack?
    // Should I include the KeySigTrack?
}


void Song::Track_PartInserted(Track *track, Part *)
{
    if (track->lastClock() > pimpl->lastClock)
    {
        pimpl->lastClock = track->lastClock();
    }
}


void Song::Track_PartRemoved(Track *, Part *)
{
    recalculateLastClock();
}


/******************************************************************************
 * Song Serializable interface
 *****************************************************************************/

void Song::save(std::ostream &o, int i) const
{
    o << indent(i)   << "{\n";
    o << indent(i+1) << "Title:"     << pimpl->title.c_str()     << "\n";
    o << indent(i+1) << "Author:"    << pimpl->author.c_str()    << "\n";
    o << indent(i+1) << "Copyright:" << pimpl->copyright.c_str() << "\n";
    o << indent(i+1) << "Date:"      << pimpl->date.c_str()      << "\n";
    o << indent(i+1) << "NoTracks:"  << size()                   << "\n";
    o << indent(i+1) << "TempoTrack\n";
    pimpl->tempoTrack.save(o, i+1);
    o << indent(i+1) << "TimeSigTrack\n";
    pimpl->timeSigTrack.save(o, i+1);
    o << indent(i+1) << "KeySigTrack\n";
    pimpl->keySigTrack.save(o, i+1);
    o << indent(i+1) << "FlagTrack\n";
    pimpl->flagTrack.save(o, i+1);
    o << indent(i+1) << "SoloTrack:" << soloTrack()              << "\n";
    o << indent(i+1) << "Repeat:";
    if (pimpl->repeat) o << "On\n"; else o << "Off\n";
    o << indent(i+1) << "From:"      << from()                   << "\n";
    o << indent(i+1) << "To:"        << to()                     << "\n";
    pimpl->phraseList.save(o, i+1);
    std::vector<Track*>::const_iterator t = pimpl->tracks.begin();
    while (t != pimpl->tracks.end())
    {
        o << indent(i+1) << "Track\n";
        (*t)->save(o, i+1);
        ++t;
    }
    o << indent(i)   << "}\n";
}


namespace
{
    /**
     * A simple Serializable class that creates a simple FileBlockParser
     * to load the Events sub-block of the TempoTrack block.
     */
    class TrackLoader : public Serializable
    {
        public:
            TrackLoader(Song *s)
            : s(s) {}
            virtual void load(std::istream &in, SerializableLoadInfo &info)
            {
                Track *track = new Track;
                s->insert(track);
                track->load(in, info);
            }
        private:
            Song *s;
    };
}


void Song::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_String<Song> title(this,     &Song::setTitle);
    FileItemParser_String<Song> author(this,    &Song::setAuthor);
    FileItemParser_String<Song> copyright(this, &Song::setCopyright);
    FileItemParser_String<Song> date(this,      &Song::setDate);
    FileItemParser_Number<Song> soloTrack(this, &Song::setSoloTrack);
    FileItemParser_OnOff<Song>  repeat(this,    &Song::setRepeat);
    FileItemParser_Clock<Song>  from(this,      &Song::setFrom);
    FileItemParser_Clock<Song>  to(this,        &Song::setTo);
    TrackLoader trackLoader(this);
    FileBlockParser parser;
    parser.add("Title",        &title);
    parser.add("Author",       &author);
    parser.add("Copyright",    &copyright);
    parser.add("Date",         &date);
    //parser.add("NoTracks",   &nothing); // we ignore this data line
    parser.add("SoloTrack",    &soloTrack);
    parser.add("Repeat",       &repeat);
    parser.add("From",         &from);
    parser.add("To",           &to);
    parser.add("TempoTrack",   &pimpl->tempoTrack);
    parser.add("TimeSigTrack", &pimpl->timeSigTrack);
    parser.add("KeySigTrack",  &pimpl->keySigTrack);
    parser.add("FlagTrack",    &pimpl->flagTrack);
    parser.add("Phrase",       &pimpl->phraseList);
    parser.add("Track",        &trackLoader);
    parser.parse(in, info);
}
