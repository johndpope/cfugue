/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __ALSADRIVER_H__9857EC39_234E_411E_9558_EFDA218796AA__
#define __ALSADRIVER_H__9857EC39_234E_411E_9558_EFDA218796AA__

#include "rtmidi/RtMidi.h"
#include "jdkmidi/msg.h"
#include "jdkmidi/driver.h"
#include "jdkmidi/sequencer.h"

#include <thread> // std::thread

  class MIDIDriverAlsa : public jdkmidi::MIDIDriver
  {
	  RtMidiIn*		m_pMidiIn;
	  RtMidiOut*	m_pMidiOut;

    public:
      MIDIDriverAlsa ( int queue_size );
      virtual ~MIDIDriverAlsa();

      void ResetMIDIOut();

      bool StartTimer ( int resolution_ms );
      bool OpenMIDIInPort ( int id );

      bool OpenMIDIOutPort ( int id );

      void StopTimer();
      void CloseMIDIInPort();
      void CloseMIDIOutPort();


      bool HardwareMsgOut ( const jdkmidi::MIDITimedBigMessage &msg );

    protected:
      void operator() (); // This is the Time-tick procedure called from std::thread

      std::thread* m_pThread;

//      static void CALLBACK win32_timer (
//        UINT wTimerID,
//        UINT msg,
//        DWORD_PTR dwUser,
//        DWORD_PTR dw1,
//        DWORD_PTR dw2
//      );
//
//      static void CALLBACK win32_midi_in (
//        HMIDIIN hMidiIn,
//        UINT wMsg,
//        DWORD dwInstance,
//        DWORD dwParam1,
//        DWORD dwParam2
//      );

      //HMIDIIN in_handle;
      //HMIDIOUT out_handle;
      int timer_id;
      int timer_res;

      //bool in_open;
      //bool out_open;
      //bool timer_open;
  };


#endif // __ALSADRIVER_H__9857EC39_234E_411E_9558_EFDA218796AA__
