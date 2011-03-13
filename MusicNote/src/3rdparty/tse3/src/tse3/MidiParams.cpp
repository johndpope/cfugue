/*
 * @(#)MidiParams.cpp 3.00 1 June 1999
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

#include "tse3/MidiParams.h"

#include "tse3/FileBlockParser.h"
#include "tse3/Mutex.h"

using namespace TSE3;

/******************************************************************************
 * MidiParamsIterator class
 *****************************************************************************/

namespace TSE3
{
    /**
     * The @ref PlayableIterator for the @ref MidiParams class.
     *
     * @short   MidiData PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     MidiData
     */
    class MidiParamsIterator : public PlayableIterator,
                               public Listener<MidiParamsListener>
    {
        public:

            /**
             * Create a MidiParamsIterator for the given @ref MidiParams
             * object starting at the given @ref Clock.
             */
            MidiParamsIterator(MidiParams *mp, Clock c);
            ~MidiParamsIterator();
            virtual void moveTo(Clock c);

            virtual void Notifier_Deleted(MidiParams *source);

        protected:

            virtual void getNextEvent();

            int         _pos;
            MidiParams *_params;

            /**
             * What the iterator is currently producing.
             */
            enum IteratorPos
            {
                Start   = 0,
                BankLSB = 1,
                BankMSB = 2,
                Program = 3,
                Pan     = 4,
                Reverb  = 5,
                Chorus  = 6,
                Volume  = 7,
                End     = 8
            };

        private:

            MidiParamsIterator &operator=(const MidiParamsIterator &);
            MidiParamsIterator(const MidiParamsIterator &);
    };
}


MidiParamsIterator::MidiParamsIterator(MidiParams *mp, Clock c)
: _pos(End), _params(mp)
{
    moveTo(c);
    attachTo(_params);
}


MidiParamsIterator::~MidiParamsIterator()
{
}


void MidiParamsIterator::moveTo(Clock c)
{
    if (!_params || c > 0)
    {
        _pos  = End;
        _more = false;
    }
    else
    {
        _pos = Start;
        _more = true;
        getNextEvent();
    }
}


void MidiParamsIterator::getNextEvent()
{
    ++_pos;

    while (_pos != End)
    {
        switch (_pos)
        {
          case BankLSB:
              if (_params->bankLSB() >= 0)
              {
                  _next = MidiEvent(MidiCommand(MidiCommand_ControlChange,
                                                0, 0, // this c/p ignored
                                                MidiControl_BankSelectLSB,
                                                _params->bankLSB()), 0);
                  return;
              }
              ++_pos;
              break;

          case BankMSB:
              if (_params->bankMSB() >= 0)
              {
                  _next = MidiEvent(MidiCommand(MidiCommand_ControlChange,
                                                0, 0, // this c/p ignored
                                                MidiControl_BankSelectMSB,
                                                _params->bankMSB()), 0);
                  return;
              }
              ++_pos;
              break;

          case Program:
              if (_params->program() >= 0)
              {
                  _next = MidiEvent(MidiCommand(MidiCommand_ProgramChange,
                                                0, 0, // this c/p ignored
                                                _params->program()), 0);
                  return;
              }
              ++_pos;
              break;

          case Pan:
              if (_params->pan() >= 0)
              {
                  _next = MidiEvent(MidiCommand(MidiCommand_ControlChange,
                                                0, 0, // this c/p ignored
                                                MidiControl_PanMSB,
                                                _params->pan()), 0);
                  return;
              }
              ++_pos;
              break;

          case Reverb:
              if (_params->reverb() >= 0)
              {
                  _next = MidiEvent(MidiCommand(MidiCommand_ControlChange,
                                                0, 0, // this c/p ignored
                                                MidiControl_ReverbDepth,
                                                _params->reverb()), 0);
                  return;
              }
              ++_pos;
              break;

          case Chorus:
              if (_params->chorus() >= 0)
              {
                  _next = MidiEvent(MidiCommand(MidiCommand_ControlChange,
                                                0, 0, // this c/p ignored
                                                MidiControl_ChorusDepth,
                                                _params->chorus()), 0);
                  return;
              }
              ++_pos;
              break;

          case Volume:
              if (_params->volume() >= 0)
              {
                  _next = MidiEvent(MidiCommand(MidiCommand_ControlChange,
                                                0, 0, // this c/p ignored
                                                MidiControl_ChannelVolumeMSB,
                                                _params->volume()), 0);
                  return;
              }
              ++_pos;
              break;
        }
    }

    _more = false;
    _next = MidiEvent();
}


void MidiParamsIterator::Notifier_Deleted(MidiParams *source)
{
    if (source == _params)
    {
        _params = 0;
        moveTo(0);
    }
}


/******************************************************************************
 * MidiParams class
 *****************************************************************************/

MidiParams::MidiParams()
: _bankLSB(off), _bankMSB(off), _program(off), _pan(off), _reverb(off),
  _chorus(off), _volume(off)
{
}


MidiParams::MidiParams(const MidiParams &p)
: Playable(), Filter(), Serializable(), Notifier<MidiParamsListener>(),
  _bankLSB(p._bankLSB), _bankMSB(p._bankMSB),
  _program(p._program), _pan(p._pan), _reverb(p._reverb),
  _chorus(p._chorus), _volume(p._volume)
{
}


MidiParams::~MidiParams()
{
}


MidiParams &MidiParams::operator=(const MidiParams &mp)
{
    Impl::CritSec cs;

    _bankLSB = mp._bankLSB;
    _bankMSB = mp._bankMSB;
    _program = mp._program;
    _pan     = mp._pan;
    _reverb  = mp._reverb;
    _chorus  = mp._chorus;
    _volume  = mp._volume;

    Notifier<MidiParamsListener>::notify
        (&MidiParamsListener::MidiParams_Altered, 0xff);
    return *this;
}


void MidiParams::setBankLSB(int b)
{
    Impl::CritSec cs;

    if (b >= -2 && b <= 127)
    {
        _bankLSB = b;
        Notifier<MidiParamsListener>::notify
            (&MidiParamsListener::MidiParams_Altered,
             MidiParamsListener::BankLsbChanged);
    }
}


void MidiParams::setBankMSB(int b)
{
    Impl::CritSec cs;

    if (b >= -2 && b <= 127)
    {
        _bankMSB = b;
        Notifier<MidiParamsListener>::notify
            (&MidiParamsListener::MidiParams_Altered,
             MidiParamsListener::BankMsbChanged);
    }
}


void MidiParams::setProgram(int p)
{
    Impl::CritSec cs;

    if (p >= -2 && p <= 127)
    {
        _program = p;
        Notifier<MidiParamsListener>::notify
            (&MidiParamsListener::MidiParams_Altered,
             MidiParamsListener::ProgramChanged);
    }
}


void MidiParams::setPan(int p)
{
    Impl::CritSec cs;

    if (p >= -2 && p <= 127)
    {
        _pan = p;
        Notifier<MidiParamsListener>::notify
            (&MidiParamsListener::MidiParams_Altered,
             MidiParamsListener::PanChanged);
    }
}


void MidiParams::setReverb(int r)
{
    Impl::CritSec cs;

    if (r >= -2 && r <= 127)
    {
        _reverb = r;
        Notifier<MidiParamsListener>::notify
            (&MidiParamsListener::MidiParams_Altered,
             MidiParamsListener::ReverbChanged);
    }
}


void MidiParams::setChorus(int c)
{
    Impl::CritSec cs;

    if (c >= -2 && c <= 127)
    {
        _chorus = c;
        Notifier<MidiParamsListener>::notify
            (&MidiParamsListener::MidiParams_Altered,
             MidiParamsListener::ChorusChanged);
    }
}


void MidiParams::setVolume(int v)
{
    Impl::CritSec cs;

    if (v >= -2 && v <= 127)
    {
        _volume = v;
        Notifier<MidiParamsListener>::notify
            (&MidiParamsListener::MidiParams_Altered,
             MidiParamsListener::VolumeChanged);
    }
}


MidiEvent MidiParams::filter(const MidiEvent &e) const
{
    Impl::CritSec cs;

    if (_bankLSB == forceNone
        && e.data.status == MidiCommand_ControlChange
        && e.data.data1 == MidiControl_BankSelectLSB)
        return MidiEvent(MidiCommand(), e.time);

    if (_bankMSB == forceNone
        && e.data.status == MidiCommand_ControlChange
        && e.data.data1 == MidiControl_BankSelectMSB)
        return MidiEvent(MidiCommand(), e.time);

    if (_program == forceNone
        && e.data.status == MidiCommand_ProgramChange)
        return MidiEvent(MidiCommand(), e.time);

    if (_pan == forceNone
        && e.data.status == MidiCommand_ControlChange
        && e.data.data1 == MidiControl_PanMSB)
        return MidiEvent(MidiCommand(), e.time);

    if (_reverb == forceNone
        && e.data.status == MidiCommand_ControlChange
        && e.data.data1 == MidiControl_ReverbDepth)
        return MidiEvent(MidiCommand(), e.time);

    if (_chorus == forceNone
        && e.data.status == MidiCommand_ControlChange
        && e.data.data1 == MidiControl_ChorusDepth)
        return MidiEvent(MidiCommand(), e.time);

    if (_volume == forceNone
        && e.data.status == MidiCommand_ControlChange
        && e.data.data1 == MidiControl_ChannelVolumeMSB)
        return MidiEvent(MidiCommand(), e.time);

    return e;
}


PlayableIterator *MidiParams::iterator(Clock index)
{
    return new MidiParamsIterator(this, index);
}


Clock MidiParams::lastClock() const
{
    return 0;
}


/******************************************************************************
 * MidiParams Serializable interface
 *****************************************************************************/

void MidiParams::save(std::ostream &o, int i) const
{
    o << indent(i)   << "{\n";
    o << indent(i+1) << "BankLSB:" << _bankLSB << "\n";
    o << indent(i+1) << "BankMSB:" << _bankMSB << "\n";
    o << indent(i+1) << "Program:" << _program << "\n";
    o << indent(i+1) << "Pan:"     << _pan     << "\n";
    o << indent(i+1) << "Reverb:"  << _reverb  << "\n";
    o << indent(i+1) << "Chorus:"  << _chorus  << "\n";
    o << indent(i+1) << "Volume:"  << _volume  << "\n";
    o << indent(i)   << "}\n";
}


void MidiParams::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_Number<MidiParams> lsb(this,     &MidiParams::setBankLSB);
    FileItemParser_Number<MidiParams> msb(this,     &MidiParams::setBankMSB);
    FileItemParser_Number<MidiParams> program(this, &MidiParams::setProgram);
    FileItemParser_Number<MidiParams> pan(this,     &MidiParams::setPan);
    FileItemParser_Number<MidiParams> reverb(this,  &MidiParams::setReverb);
    FileItemParser_Number<MidiParams> chorus(this,  &MidiParams::setChorus);
    FileItemParser_Number<MidiParams> volume(this,  &MidiParams::setVolume);
    FileBlockParser parser;
    parser.add("BankLSB", &lsb);
    parser.add("BankMSB", &msb);
    parser.add("Program", &program);
    parser.add("Pan",     &pan);
    parser.add("Reverb",  &reverb);
    parser.add("Chorus",  &chorus);
    parser.add("Volume",  &volume);
    parser.parse(in, info);
}
