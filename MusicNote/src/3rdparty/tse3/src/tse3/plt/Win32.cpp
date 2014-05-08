/*
 * @(#)Win32.cpp 1.00 30NocemberJuly 1999
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

/*
 * NOTE: This has neither been tested or even compiled. I've just had a quick
 *       stab at a Win32 implemetation based on some docs I've found.
 *       In fact, it's pretty half baked ;-)
 */

#include "tse3/plt/Win32.h"
#include "tse3/Error.h"
#include "tse3/util/MulDiv.h"
#include "tse3/util/MidiScheduler.h"

#include <windows.h>

using namespace TSE3;
using namespace TSE3::Plt;


static char *STR_MOD_FMSYNTH  = "FM synthesizer";
static char *STR_MOD_MAPPER   = "MIDI mapper";
static char *STR_MOD_MIDIPORT = "MIDI hardware port";
static char *STR_MOD_SQSYNTH  = "Square wave synthesizer";
static char *STR_MOD_SYNTH    = "Synthesizer";
static char *STR_MOD_UNKNOWN  = "Unknown MIDI device";


/******************************************************************************
 * Win32 MidiSchedulerFactory class
 *****************************************************************************/

MidiSchedulerFactory::MidiSchedulerFactory(bool b)
: _canReturnNull(b)
{
}


MidiSchedulerFactory::~MidiSchedulerFactory()
{
}


MidiScheduler *MidiSchedulerFactory::createScheduler()
{
    try
    {
        Win32MidiScheduler *ms = new Win32MidiScheduler();
        std::cout << "Created new Win32MidiScheduler seccussfully\n";
        return ms;
    }
    catch (...)//Win32MidiSchedulerException)
    {
        std::cout << "Failed to create a Win32MidiScheduler\n";
        if (_canReturnNull)
        {
            return new Util::NullMidiScheduler();
        }
        else
        {
            throw;
        }
    }
}



/******************************************************************************
 * Win32MidiScheduler class
 *****************************************************************************/

Win32MidiScheduler::Win32MidiScheduler()
: hMidi(NULL), nMidi(0)
{
    unsigned int nMidiIn = midiInGetNumDevs();
    nMidi                = midiOutGetNumDevs() + nMidiIn;
    hMidi                = new HMIDI[nMidi];

    for (unsigned int i = 0; i < nMidi; i++)
    {
       hMidi[i].in = NULL;
    }

    for (unsigned int i = 0; i < nMidi; i++)
    {
       if (i < nMidiIn)
       {
         if (midiInOpen(&(hMidi[i].in), i, 0, 0, 0) != MMSYSERR_NOERROR)
            throw TSE3::MidiSchedulerError(MidiSchedulerCreateErr);
       }
       else
       {
         if (midiOutOpen(&(hMidi[i].out), i - nMidiIn, 0, 0, 0) != MMSYSERR_NOERROR)
            throw TSE3::MidiSchedulerError(MidiSchedulerCreateErr);
       }
       addPort(i, false, i);
    }
    //    int error = midiOutOpen(&hMidiOut, MIDI_MAPPER, 0, 0, 0);
    //    if (error != MMSYSERR_NOERROR) 
    //      throw TSE3::MidiSchedulerError(MidiSchedulerCreateErr);
}


Win32MidiScheduler::~Win32MidiScheduler()
{
    // if playing, stop first!
    if (running()) stop(-1);
    unsigned int nMidiIn = midiInGetNumDevs();
    for (unsigned int i = 0; i < nMidi; i++)
    {
      if (i < nMidiIn)
      {
        midiInClose(hMidi[i].in);
      }
      else
      {
        midiOutClose(hMidi[i].out);
      }
    }
}


OIL::StrUtils_Return_Type Win32MidiScheduler::impl_implementationName() const
{
    return "Win32MidiScheduler version 0.00 [dev].";
}


OIL::StrUtils_Return_Type Win32MidiScheduler::impl_portName(int port) const
{
  if (port > numPorts()) 
    return "";
  else if (port < midiInGetNumDevs()) {
    MIDIINCAPS m;
    midiInGetDevCaps(port, &m, sizeof(m));
    return m.szPname;
  } else {
    MIDIOUTCAPS m;
    midiOutGetDevCaps(port - midiInGetNumDevs(), &m, sizeof(m));
    return m.szPname;
  }
}

OIL::StrUtils_Return_Type Win32MidiScheduler::impl_portType(int port) const
{
  if (port > numPorts()) return "";
  if (port < midiInGetNumDevs()) {
    return "MIDI Input Device";
  } else {
    MIDIOUTCAPS m;
    midiOutGetDevCaps(port - midiInGetNumDevs(), &m, sizeof(m));
    switch(m.wTechnology) {
       case MOD_FMSYNTH:   return STR_MOD_FMSYNTH; 
       case MOD_MAPPER:    return STR_MOD_MAPPER; 
       case MOD_MIDIPORT:  return STR_MOD_MIDIPORT; 
       case MOD_SQSYNTH:   return STR_MOD_SQSYNTH; 
       case MOD_SYNTH:     return STR_MOD_SYNTH;
       default:            return STR_MOD_UNKNOWN;
    }
  }
}



bool Win32MidiScheduler::impl_portReadable(int port) const {
  if (port < midiInGetNumDevs())
    return true;
  else
    return false;
}

bool Win32MidiScheduler::impl_portWriteable(int port) const {
  return !portReadable(port);
}


void Win32MidiScheduler::impl_tx(MidiCommand mc)
{
  if (mc.port < midiInGetNumDevs())
    return;
  runMidiData(hMidi[mc.port].out, mc);
//    midiShortMsg((int)mc);
}

void Win32MidiScheduler::runMidiData(HMIDIOUT o, MidiCommand mc) {
  union {
    DWORD dwData;
    BYTE  bData[4];
  } u;

  u.bData[0] = (mc.status<<4) + mc.channel;
  u.bData[1] = mc.data1;
  u.bData[2] = mc.data2;
  u.bData[3] = 0;

  midiOutShortMsg(o, u.dwData);
}

void Win32MidiScheduler::impl_start(Clock s)
{
    if (!_running) {
       TIMECAPS timecaps;
       timeGetDevCaps(&timecaps, sizeof(timecaps));
       timeBeginPeriod(10);

       startTime = timeGetTime();
       clockStarted(s);
    }
}


void Win32MidiScheduler::impl_stop(Clock t)
{
    if (!_running) return;
    if (t != -1) {
      _restingClock = t;
    } else {
      _restingClock = clock();
    }
    timeEndPeriod(10);
    clockStopped(t);
}


void Win32MidiScheduler::impl_moveTo(Clock moveTime, Clock newTime)
{
    clockMoved(moveTime, newTime);
}


Clock Win32MidiScheduler::impl_clock()
{
    int time = timeGetTime() - startTime;
    return msToClock(time);
}


int Win32MidiScheduler::impl_msecs()
{
    return timeGetTime() - startTime;
}


void Win32MidiScheduler::impl_setTempo(int newTempo, Clock changeTime)
{
  tempoChanged(newTempo, changeTime);

/*  MIDIPROPTEMPO m;
  m.cbStruct = sizeof(m);
  m.dwTempo  = newTempo;
  for (unsigned int i = 0; i < nMidiOut; i++) {
    midiStreamProperty(hMidiOut[i], MIDIPROP_SET | MIDIPROP_TEMPO, &m);
  }*/
}


bool Win32MidiScheduler::impl_eventWaiting()
{
    return false;
}


MidiEvent Win32MidiScheduler::impl_rx()
{
    return MidiEvent();
}


struct CallbackData
{
  HMIDIOUT port;
  MidiCommand e;
  Win32MidiScheduler* sch;
};


void CALLBACK callback(UINT uID,    UINT uMsg, 
                                  DWORD_PTR _data, DWORD_PTR dw1, DWORD_PTR dw2)
{
  CallbackData *data = (CallbackData*) _data;
  data->sch->runMidiData(data->port, data->e);
  delete data;
}


void Win32MidiScheduler::impl_tx(MidiEvent e)
{
  unsigned int msecs = clockToMs(e.time);  
  if (msecs > timeGetTime()) {
    if (  e.data.port < midiInGetNumDevs() 
       || e.data.port > numPorts()) 
          return;
    CallbackData* data = new CallbackData;
    data->port = hMidi[e.data.port].out;
    data->e    = e.data;
    data->sch  = this;
    timeSetEvent(msecs-timeGetTime(), 10, &callback, (DWORD_PTR) data, TIME_ONESHOT);
  } else
    tx(e.data);
}


void Win32MidiScheduler::impl_txSysEx(int port,
                                      const unsigned char* data, size_t size)
{
    std::cerr << "No implemented Sys Ex in Win32" << std::endl;
}


