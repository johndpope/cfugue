/*
 * @(#)Panic.cpp 3.00 21 June 1999
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

#include "tse3/Panic.h"

#include "tse3/Mutex.h"

using namespace TSE3;

/******************************************************************************
 * Compile time assertion that unsigned int is at least 32 bits.
 *****************************************************************************/

namespace
{
    template <bool expression>
    class compile_assertion;
    template <>
    class compile_assertion<true> {};

    // If this line generates an error then your ints aren't big enough
    compile_assertion<sizeof(unsigned int)*8 >= 32> uint_size_test;
}

/******************************************************************************
 * PanicIterator class
 *****************************************************************************/

namespace TSE3
{
    /**
     * The @ref PlayableIterator for the @ref Panic class.
     *
     * @short   Panic PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     Panic
     */
    class PanicIterator : public PlayableIterator,
                          public Listener<PanicListener>
    {
        public:

            /**
             * Create a PanicIterator for the given @ref Panic object
             * starting at the given @ref Clock.
             */
            PanicIterator(Panic *p, Clock c);
            virtual void moveTo(Clock c);

            virtual void Notifier_Deleted(Panic *);

        protected:

            virtual void getNextEvent();

            int    _pos, _subPos, _channel;
            Panic *_panic;

            /**
             * What the iterator is currently producing.
             */
            enum IteratorPos
            {
                Start               = 0,
                Midi                = 1,
                Gm                  = 2,
                Gs                  = 3,
                Xg                  = 4,
                AllNotesOff         = 5,
                AllNotesOffManually = 6,
                AllModsOff          = 7,
                AllPitchOff         = 8,
                AllCtrlOff          = 9,
                LiftSustain         = 10,
                End                 = 11
            };

        private:

            PanicIterator &operator=(const PanicIterator &);
            PanicIterator(const PanicIterator &);
    };
}


PanicIterator::PanicIterator(Panic *p, Clock c)
: _panic(p)
{
    moveTo(c);
}


void PanicIterator::moveTo(Clock c)
{
    if (!_panic || c > 0 || !_panic->status())
    {
        _pos = End;
        return;
    }
    _pos  = Start;
    _more = true;
    getNextEvent();
}


void PanicIterator::getNextEvent()
{
    _next = MidiEvent();

    if (_pos == End) return;

    if (_pos == Start)
    {
        ++_pos;
        _channel = 0;
        _subPos  = 0;
    }

    if (_pos == Midi)
    {
        if (!_panic->midiReset())
        {
            ++_pos;
        }
        else
        {
            _next = MidiEvent(MidiCommand(MidiCommand_System,
                                          MidiSystem_SystemReset,
                                          _panic->port(), 0), 0);
            ++_pos;
            return;
        }
    }

    if (_pos == Gm)
    {
        const int           size        = 4;
        const unsigned char sysex[size] = {0x7e, 0x7f, 0x09, 0x01};

        if (!_panic->gmReset() || _subPos == size+1)
        {
            ++_pos;
            _channel = 0;
            _subPos  = 0;
        }
        else if (_subPos == size)
        {
            _next = MidiEvent(MidiCommand(MidiCommand_System,
                                          MidiSystem_SysExEnd,
                                          _panic->port(), 0), 0);
            ++_subPos;
            return;
        }
        else
        {
            _next = MidiEvent(MidiCommand(MidiCommand_System,
                                          MidiSystem_SysExStart, _panic->port(),
                                          sysex[_subPos]), 0);
            ++_subPos;
            return;
        }
    }

    if (_pos == Gs)
    {
        const int           size        = 9;
        const unsigned char sysex[size] = {0x41, 0x00 /*devID*/, 0x42, 0x12,
                                           0x40, 0x00,           0x7f, 0x00,
                                           0x41};
        const int           devIdPos    = 1;
        const int           noDevIds    = 32;

        // We use channel as the devID (0-31) counter
        if (!_panic->gsReset() || _channel == noDevIds)
        {
            ++_pos;
            _channel = 0;
            _subPos  = 0;
        }
        else if (_subPos == size+1 || !_panic->gsIDMask(_channel))
        {
            ++_channel;
            _subPos = 0;
            // return a null event, and pick up from start the next time
            return;
        }
        else if (_subPos == size)
        {
            _next = MidiEvent(MidiCommand(MidiCommand_System,
                                          MidiSystem_SysExEnd, _panic->port(),
                                          0), 0);
            ++_subPos;
            return;
        }
        else
        {
            _next = MidiEvent(MidiCommand(MidiCommand_System,
                                          MidiSystem_SysExStart, _panic->port(),
                                          sysex[_subPos]), 0);
            if (_subPos == devIdPos)
            {
                _next.data.data1 = _channel;
            }
            ++_subPos;
            return;
        }
    }

    if (_pos == Xg)
    {
        const int           size        = 7;
        const unsigned char sysex[size] = {0x43, 0x10 /*+devID*/, 0x4c, 0x00,
                                           0x00, 0x7e,            0x00};
        const int           devIdPos    = 1;
        const int           noDevIds    = 16;

        // We use channel as the devID (0-31) counter
        if (!_panic->xgReset() || _channel == noDevIds)
        {
            ++_pos;
            _channel = 0;
            _subPos  = 0;
        }
        else if (_subPos == size+1 || !_panic->xgIDMask(_channel))
        {
            ++_channel;
            _subPos = 0;
            // return a null event, and pick up from start the next time
            return;
        }
        else if (_subPos == size)
        {
            _next = MidiEvent(MidiCommand(MidiCommand_System,
                                          MidiSystem_SysExEnd, _panic->port(),
                                          0), 0);
            ++_subPos;
            return;
        }
        else
        {
            _next = MidiEvent(MidiCommand(MidiCommand_System,
                                          MidiSystem_SysExStart, _panic->port(),
                                          sysex[_subPos]), 0);
            if (_subPos == devIdPos)
            {
                _next.data.data1 += _channel;
            }
            ++_subPos;
            return;
        }
    }

    if (_pos == AllNotesOff)
    {
        if (!(_panic->allNotesOff()) || _channel == 16)
        {
            ++_pos;
            _channel = 0;
            _subPos  = 0;
        }
        else
        {
             _next = MidiEvent(MidiCommand(MidiCommand_ControlChange,
                                           _channel, _panic->port(),
                                           MidiControl_AllNotesOff), 0);
            ++_channel;
            return;
        }
    }

    if (_pos == AllNotesOffManually)
    {
        if (!_panic->allNotesOffManually() || _channel == 16)
        {
            ++_pos;
            _channel = 0;
            _subPos  = 0;
        }
        else
        {
            _next = MidiEvent(MidiCommand(MidiCommand_NoteOff, _channel,
                                          _panic->port(), _subPos, 0), 0);
            if (++_subPos == 128)
            {
                ++_channel;
                _subPos = 0;
            }
            return;
        }
    }

    if (_pos == AllModsOff)
    {
        if (!(_panic->allModsOff()) || _channel == 16)
        {
            ++_pos;
            _channel = 0;
        }
        else
        {
             _next = MidiEvent(MidiCommand(MidiCommand_ControlChange,
                                           _channel, _panic->port(),
                                           MidiControl_ModulationMSB, 0), 0);
            ++_channel;
            return;
        }
    }

    if (_pos == AllPitchOff)
    {
        if (!(_panic->allPitchOff()) || _channel == 16)
        {
            ++_pos;
            _channel = 0;
        }
        else
        {
             _next = MidiEvent(MidiCommand(MidiCommand_PitchBend,
                                           _channel, _panic->port(), 0, 0x40),
                               0);
            ++_channel;
            return;
        }
    }

    if (_pos == AllCtrlOff)
    {
        if (!(_panic->allCtrlOff()) || _channel == 16)
        {
            ++_pos;
            _channel = 0;
        }
        else
        {
             _next = MidiEvent(MidiCommand(MidiCommand_ControlChange,
                                           _channel, _panic->port(),
                                           MidiControl_ResetAllControllers, 0),
                               0);
            ++_channel;
            return;
        }
    }

    if (_pos == LiftSustain)
    {
        if (!(_panic->liftSustain()) || _channel == 16)
        {
          ++_pos;
          _channel = 0;
        }
        else
        {
             _next = MidiEvent(MidiCommand(MidiCommand_ControlChange,
                                           _channel, _panic->port(),
                                           MidiControl_SustainPedal, 0), 0);
            ++_channel;
            return;
        }
    }

    // If we got here there are no more events to be generated
    _more = false;
}


void PanicIterator::Notifier_Deleted(Panic *)
{
    _panic = 0;
    moveTo(0);
}


/******************************************************************************
 * Panic class
 *****************************************************************************/

Panic::Panic()
: _status(true), _port(MidiCommand::AllPorts),
  _midi(true), _gm(true), _gs(false), _xg(false),
  gsMask(0xffffffff), xgMask(0xffffffff),
  _allNotesOff(false), _allNotesOffManually(false),
  _allModsOff(false), _allPitchOff(false), _allCtrlOff(false),
  _liftSustain(false)
{
}


Panic::~Panic()
{
}


void Panic::setPort(int p)
{
    Impl::CritSec cs;

    _port = p;
    Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                    PanicListener::PortChanged);
}


void Panic::setStatus(bool s)
{
    Impl::CritSec cs;

    _status = s;
    Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                    PanicListener::StatusChanged);
}


void Panic::setMidiReset(bool s)
{
    Impl::CritSec cs;

    _midi = s;
    Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                    PanicListener::MidiResetChanged);
}


void Panic::setGmReset(bool s)
{
    Impl::CritSec cs;

    _gm = s;
    Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                    PanicListener::GmResetChanged);
}


void Panic::setGsReset(bool s)
{
    Impl::CritSec cs;

    _gs = s;
    Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                    PanicListener::GsResetChanged);
}


void Panic::setXgReset(bool s)
{
    Impl::CritSec cs;

    _xg = s;
    Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                    PanicListener::XgResetChanged);
}


bool Panic::gsIDMask(size_t device) const
{
    Impl::CritSec cs;

    return (gsMask >> device) & 1;
}


void Panic::setGsIDMask(size_t device, bool s)
{
    if (device < 32)
    {
        Impl::CritSec cs;

        gsMask &= 0xffffffff ^ (1 << device);
        if (s) gsMask |= 1 << device;
        Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                        PanicListener::GsIDMaskChanged);
    }
}


bool Panic::xgIDMask(size_t device) const
{
    Impl::CritSec cs;

    return (xgMask >> device) & 1;
}


void Panic::setXgIDMask(size_t device, bool s)
{
    if (device < 16)
    {
        Impl::CritSec cs;

        xgMask &= 0xffffffff ^ (1 << device);
        if (s) xgMask |= 1 << device;
        Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                        PanicListener::XgIDMaskChanged);
    }
}


void Panic::setAllNotesOff(bool s)
{
    Impl::CritSec cs;

    _allNotesOff = s;
    Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                    PanicListener::AllNotesOffChanged);
}


void Panic::setAllNotesOffManually(bool s)
{
    Impl::CritSec cs;

    _allNotesOffManually = s;
    Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                    PanicListener::AllNotesOffManuallyChanged);
}


void Panic::setAllModsOff(bool s)
{
    Impl::CritSec cs;

    _allModsOff = s;
    Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                    PanicListener::AllModsOffChanged);
}


void Panic::setAllPitchOff(bool s)
{
    Impl::CritSec cs;

    _allPitchOff = s;
    Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                    PanicListener::AllPitchOffChanged);
}


void Panic::setAllCtrlOff(bool s)
{
    Impl::CritSec cs;

    _allCtrlOff = s;
    Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                    PanicListener::AllCtrlOffChanged);
}


void Panic::setLiftSustain(bool s)
{
    Impl::CritSec cs;

    _liftSustain = s;
    Notifier<PanicListener>::notify(&PanicListener::Panic_Altered,
                                    PanicListener::LiftSustainChanged);
}


PlayableIterator *Panic::iterator(Clock index)
{
    return new PanicIterator(this, index);
}


Clock Panic::lastClock() const
{
    return 0;
}

