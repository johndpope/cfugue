/*
 * @(#)Metronome.cpp 3.00 25 May 1999
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

#include "tse3/Metronome.h"
#include "tse3/Transport.h"

using namespace TSE3;

/******************************************************************************
 * MetronomeIterator class
 *****************************************************************************/

namespace TSE3
{
    /**
     * The @ref PlayableIterator for the @ref Metronome class.
     *
     * @short   Metronome PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     MidiData
     */
    class MetronomeIterator : public PlayableIterator,
                              public Listener<MetronomeListener>
    {
        public:

            /**
             * Create a MetronomeIterator for the given @ref Metronome,
             * set to start at the given @ref Clock.
             */
            MetronomeIterator(Metronome *m, Clock c);
            ~MetronomeIterator();
            virtual void moveTo(Clock c);

            virtual void Notifier_Deleted(Metronome *);

        protected:

            virtual void getNextEvent();

            Clock      _pos;
            Metronome *_metronome;

        private:

            MetronomeIterator &operator=(const MetronomeIterator &);
            MetronomeIterator(const MetronomeIterator &);
    };
}


MetronomeIterator::MetronomeIterator(Metronome *m, Clock c)
: _metronome(m)
{
    moveTo(c);
    attachTo(_metronome);
    _more = true;
}


MetronomeIterator::~MetronomeIterator()
{
}


void MetronomeIterator::moveTo(Clock c)
{
    _pos = c - _metronome->_barReference;
    _pos /= Clock::PPQN;
    _pos *= Clock::PPQN;
    _pos += _metronome->_barReference;
    getNextEvent();
}


void MetronomeIterator::getNextEvent()
{
    if (((_pos-_metronome->_barReference)/Clock::PPQN)
        % _metronome->_beatsPerBar == 0)
    {
        _next.data    = _metronome->barNoteOn;
        _next.offData = _metronome->barNoteOff;
    }
    else
    {
        _next.data    = _metronome->beatNoteOn;
        _next.offData = _metronome->beatNoteOff;
    }
    _next.time    = _pos;
    _next.offTime = _pos + _metronome->_duration;
    _pos += Clock::PPQN;
}


void MetronomeIterator::Notifier_Deleted(Metronome *)
{
    _metronome = 0;
    _more      = false;
    _next      = MidiEvent();
}


/******************************************************************************
 * Metronome class
 *****************************************************************************/

Metronome::Metronome()
: _channel(9), _port(0), _status(1<<Transport::Recording),
  _duration(Clock::PPQN/16),
  _barNote(43), _barVelocity(127),
  _beatNote(43), _beatVelocity(70),
  _beatsPerBar(4), _barReference(0)
{
    calculateCommands();
}


Metronome::~Metronome()
{
}


void Metronome::setChannel(int c)
{
    if (c < 0 || c > 15) return;
    _channel = c;
    calculateCommands();
    Notifier<MetronomeListener>::notify(&MetronomeListener::Metronome_Altered,
                                        MetronomeListener::ChannelChanged);
}


void Metronome::setPort(int p)
{
    _port = p;
    calculateCommands();
    Notifier<MetronomeListener>::notify(&MetronomeListener::Metronome_Altered,
                                        MetronomeListener::PortChanged);
}


void Metronome::setDuration(int d)
{
    if (d < 0 || d > (Clock::PPQN*4)) return;
    _duration = d;
    Notifier<MetronomeListener>::notify(&MetronomeListener::Metronome_Altered,
                                        MetronomeListener::DurationChanged);
}


void Metronome::setBarNote(int n)
{
    if (n < 0 || n > 127) return;
    _barNote = n;
    calculateCommands();
    Notifier<MetronomeListener>::notify(&MetronomeListener::Metronome_Altered,
                                        MetronomeListener::BarNoteChanged);
}


void Metronome::setBarVelocity(int v)
{
    if (v < 0 || v > 127) return;
    _barVelocity = v;
    calculateCommands();
    Notifier<MetronomeListener>::notify(&MetronomeListener::Metronome_Altered,
                                        MetronomeListener::BarVelocityChanged);
}


void Metronome::setBeatNote(int n)
{
    if (n < 0 || n > 127) return;
    _beatNote = n;
    calculateCommands();
    Notifier<MetronomeListener>::notify(&MetronomeListener::Metronome_Altered,
                                        MetronomeListener::BeatNoteChanged);
}


void Metronome::setBeatVelocity(int v)
{
    if (v < 0 || v > 127) return;
    _beatVelocity = v;
    calculateCommands();
    Notifier<MetronomeListener>::notify(&MetronomeListener::Metronome_Altered,
                                        MetronomeListener::BeatVelocityChanged);
}


void Metronome::setStatus(int mode, bool s)
{
    _status &= 0xff - (1<<mode);
    _status |= s << mode;
    Notifier<MetronomeListener>::notify(&MetronomeListener::Metronome_Altered,
                                        MetronomeListener::StatusChanged);
}


void Metronome::setBeatsPerBar(int bpb)
{
    if (bpb < 0) return;
    _beatsPerBar = bpb;
}


void Metronome::setBarReference(Clock br)
{
    _barReference = br;
}


PlayableIterator *Metronome::iterator(Clock index)
{
    return new MetronomeIterator(this, index);
}


Clock Metronome::lastClock() const
{
    return 0;
}


void Metronome::calculateCommands()
{
    // MidiCommand_NoteOns
    barNoteOn.status  = beatNoteOn.status  = MidiCommand_NoteOn;
    barNoteOn.port    = beatNoteOn.port    = _port;
    barNoteOn.channel = beatNoteOn.channel = _channel;
    barNoteOn.data1   = _barNote;
    barNoteOn.data2   = _barVelocity;
    beatNoteOn.data1  = _beatNote;
    beatNoteOn.data2  = _beatVelocity;

    // MidiCommand_NoteOffs
    barNoteOff  = barNoteOn;
    beatNoteOff = beatNoteOn;
    barNoteOff.status = beatNoteOff.status = MidiCommand_NoteOff;
}
