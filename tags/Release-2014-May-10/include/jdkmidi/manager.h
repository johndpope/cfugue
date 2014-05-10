/*
 *  libjdkmidi-2004 C++ Class Library for MIDI
 *
 *  Copyright (C) 2004  J.D. Koftinoff Software, Ltd.
 *  www.jdkoftinoff.com
 *  jeffk@jdkoftinoff.com
 *
 *  *** RELEASED UNDER THE GNU GENERAL PUBLIC LICENSE (GPL) April 27, 2004 ***
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef JDKMIDI_MANAGER_H
#define JDKMIDI_MANAGER_H

#include "jdkmidi/msg.h"
#include "jdkmidi/sysex.h"
#include "jdkmidi/driver.h"
#include "jdkmidi/sequencer.h"
#include "jdkmidi/tick.h"

namespace jdkmidi
{
  class MIDIManager : public MIDITick
  {
    public:
      MIDIManager (
        MIDIDriver *drv,
        MIDISequencerGUIEventNotifier *n=0,
        MIDISequencer *seq_=0
      );
      
      virtual ~MIDIManager();
      
      void Reset();
      
      // to set and get the current sequencer
      void SetSeq ( MIDISequencer *seq );
      MIDISequencer *GetSeq();
      const MIDISequencer *GetSeq() const;
      
      // to get the driver that we use
      MIDIDriver *GetDriver()
      {
        return driver;
      }
      
      
      // to set and get the system time offset
	  void SetTimeOffset(MIDITick::time_point off);
	  MIDITick::time_point GetTimeOffset();
      
      // to set and get the sequencer time offset
	  void SetSeqOffset(MIDITickMS seqoff);
	  MIDITickMS GetSeqOffset();
      
      
      // to manage the playback of the sequencer
      void SeqPlay();
      void SeqStop();
      void SetRepeatPlay (
        bool flag,
        unsigned long start_measure,
        unsigned long end_measure
      );
      
      
      // status request functions
      bool IsSeqPlay() const;
      bool IsSeqStop() const;
      bool IsSeqRepeat() const;
      
      // inherited from MIDITick
	  virtual bool TimeTick(MIDITick::time_point sys_time);
      
    protected:
    
	  virtual bool TimeTickPlayMode(MIDITick::time_point sys_time_);
	  virtual bool TimeTickStopMode(MIDITick::time_point sys_time_);
      
      MIDIDriver *driver;
      
      MIDISequencer *sequencer;
      
	  MIDITick::time_point sys_time_offset;
	  MIDITickMS seq_time_offset;
      
      volatile bool play_mode;
      volatile bool stop_mode;
      
      MIDISequencerGUIEventNotifier *notifier;
      
      volatile bool repeat_play_mode;
      long repeat_start_measure;
      long repeat_end_measure;
      
      
  };
  
  
}

#endif
