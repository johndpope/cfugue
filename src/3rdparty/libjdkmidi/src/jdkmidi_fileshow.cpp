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
/*
** Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
** All rights reserved.
**
** No one may duplicate this source code in any form for any reason
** without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#include "jdkmidi/world.h"

#include "jdkmidi/fileshow.h"

#ifndef DEBUG_MDFSH
# define DEBUG_MDFSH 0
#endif

#if DEBUG_MDFSH
# undef DBG
# define DBG(a) a
#endif

namespace jdkmidi
{


  MIDIFileShow::MIDIFileShow ( FILE *out_ )
      :
      out ( out_ )
  {
    ENTER ( "MIDIFileShow::MIDIFileShow()" );
    
  }
  
  MIDIFileShow::~MIDIFileShow()
  {
    ENTER ( "MIDIFileShow::~MIDIFileShow()" );
    
  }
  
  void    MIDIFileShow::mf_error ( const char *e )
  {
    fprintf ( out, "\nParse Error: %s\n", e );
    MIDIFileEvents::mf_error ( e );
  }
  
  void    MIDIFileShow::mf_starttrack ( int trk )
  {
    fprintf ( out, "Start Track #%d\n", trk );
  }
  
  void    MIDIFileShow::mf_endtrack ( int trk )
  {
    fprintf ( out, "End Track   #%d\n", trk );
  }
  
  void    MIDIFileShow::mf_header ( int format, int ntrks, int d )
  {
    fprintf ( out, "Header: Type=%d Tracks=%d", format, ntrks );
    
    
    division=d;
    
    if ( division>0x8000 )
    {
      unsigned char smpte_rate = ( ( unsigned char ) ( ( -division ) >>8 ) );
      unsigned char smpte_division = ( unsigned char ) ( division&0xff );
      
      fprintf ( out, " SMPTE=%d Division=%d\n", smpte_rate, smpte_division );
    }
    else
    {
      fprintf ( out, " Division=%d\n", division );
    }
  }
  
  void MIDIFileShow::show_time ( MIDITickMS time )
  {
    if ( division>0 )
    {
      long long beat = time.count()/division;
      long long clk = time.count()%division;
      
      fprintf ( out, "Time: %6lld:%3lld    ", beat, clk );
    }
    else
    {
      fprintf ( out, "Time: %9ld     ", time.count() );
    }
  }
  
//
// The possible events in a MIDI Files
//

  void    MIDIFileShow::mf_system_mode ( const MIDITimedMessage &msg  )
  {
    show_time ( msg.GetTime() );
    char buf[64];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
  }
  
  void    MIDIFileShow::mf_note_on ( const MIDITimedMessage &msg  )
  {
    show_time ( msg.GetTime() );
    char buf[64];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
    
  }
  
  void    MIDIFileShow::mf_note_off ( const MIDITimedMessage &msg  )
  {
    show_time ( msg.GetTime() );
    char buf[64];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
  }
  
  void    MIDIFileShow::mf_poly_after ( const MIDITimedMessage &msg  )
  {
    show_time ( msg.GetTime() );
    char buf[64];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
  }
  
  void    MIDIFileShow::mf_bender ( const MIDITimedMessage &msg  )
  {
    show_time ( msg.GetTime() );
    char buf[64];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
  }
  
  void    MIDIFileShow::mf_program ( const MIDITimedMessage &msg  )
  {
    show_time ( msg.GetTime() );
    char buf[64];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
  }
  
  void    MIDIFileShow::mf_chan_after ( const MIDITimedMessage &msg  )
  {
    show_time ( msg.GetTime() );
    char buf[64];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
  }
  
  void    MIDIFileShow::mf_control ( const MIDITimedMessage &msg  )
  {
    show_time ( msg.GetTime() );
    char buf[64];
    fprintf ( out, "%s\n", msg.MsgToText ( buf ) );
  }
  
  void    MIDIFileShow::mf_sysex ( MIDITickMS time, const MIDISystemExclusive &ex )
  {
    show_time ( time );
    fprintf ( out, "SysEx     Length=%d\n", ex.GetLength() );
    
    for ( int i=0; i<ex.GetLength(); ++i )
    {
      if ( ( i&0x1f ) ==0 )
        fprintf ( out, "\n" );
      fprintf ( out, "%02x ", ( int ) ex.GetData ( i ) );
    }
    fprintf ( out, "\n" );
  }
  
  void    MIDIFileShow::mf_arbitrary ( MIDITickMS time, int len, unsigned char *data  )
  {
    show_time ( time );
    fprintf ( out, "RAW MIDI DATA    Length=%d\n", len );
    
    for ( int i=0; i<len; ++i )
    {
      if ( ( i&0x1f ) ==0 )
        fprintf ( out, "\n" );
      fprintf ( out, "%02x ", ( int ) data[i] );
    }
    fprintf ( out, "\n" );
  }
  
  void    MIDIFileShow::mf_metamisc ( MIDITickMS time, int type, int len, unsigned char *data  )
  {
    show_time ( time );
    
    fprintf ( out, "META-EVENT       TYPE=%d Length=%d\n", type, len );
    
    for ( int i=0; i<len; ++i )
    {
      if ( ( i&0x1f ) ==0 )
        fprintf ( out, "\n" );
      fprintf ( out, "%02x ", ( int ) data[i] );
    }
    fprintf ( out, "\n" );
  }
  
  void    MIDIFileShow::mf_seqnum ( MIDITickMS time, int num )
  {
    show_time ( time );
    
    fprintf ( out, "Sequence Number  %d\n", num );
  }
  
  void    MIDIFileShow::mf_smpte ( MIDITickMS time, int a, int b, int c, int d, int e )
  {
    show_time ( time );
    
    fprintf ( out, "SMPTE Event      %02x,%02x,%02x,%02x,%02x\n", a,b,c,d,e );
  }
  
  void    MIDIFileShow::mf_timesig (
    MIDITickMS time,
    int num,
    int denom_power,
    int midi_clocks_per_metronome,
    int notated_32nds_per_midi_quarter_note )
  {
    show_time ( time );
    
    fprintf ( out, "Time Signature   %d/%d  Clks/Metro.=%d 32nd/Quarter=%d\n",
              num,
              denom_power,
              midi_clocks_per_metronome,
              notated_32nds_per_midi_quarter_note );
              
  }
  
  
  void    MIDIFileShow::mf_tempo ( MIDITickMS time, unsigned long tempo )
  {
    show_time ( time );
    
    fprintf ( out, "Tempo              %4.2f BPM (%9ld usec/beat)\n",
              ( 60000000.0/ ( double ) tempo ), tempo );
  }
  
  void    MIDIFileShow::mf_keysig ( MIDITickMS time, int sf, int mi )
  {
    show_time ( time );
    
    fprintf ( out, "Key Signature      " );
    
    if ( mi )
      fprintf ( out, "MINOR KEY  " );
    else
      fprintf ( out, "MAJOR KEY  " );
      
    if ( sf<0 )
      fprintf ( out, "%d Flats\n", -sf );
    else
      fprintf ( out, "%d Sharps\n", sf );
  }
  
  void    MIDIFileShow::mf_sqspecific ( MIDITickMS time, int len, unsigned char *data )
  {
    show_time ( time );
    
    fprintf ( out, "Sequencer Specific     Length=%d\n", len );
    
    for ( int i=0; i<len; ++i )
    {
      if ( ( i&0x1f ) ==0 )
        fprintf ( out, "\n" );
      fprintf ( out, "%02x ", ( int ) data[i] );
    }
    fprintf ( out, "\n" );
    
  }
  
  void    MIDIFileShow::mf_text ( MIDITickMS time, int type, int len, unsigned char *txt )
  {
    static const char * text_event_names[16] =
    {
      "SEQ. #    ",
      "GENERIC   ",
      "COPYRIGHT ",
      "INST. NAME",
      "TRACK NAME",
      "LYRIC     ",
      "MARKER    ",
      "CUE       ",
      "UNKNOWN   ",
      "UNKNOWN   ",
      "UNKNOWN   ",
      "UNKNOWN   ",
      "UNKNOWN   ",
      "UNKNOWN   ",
      "UNKNOWN   ",
      "UNKNOWN   "
    };
    
    if ( type>15 )
      type=15;
      
    show_time ( time );
    
    fprintf ( out, "TEXT   %s  '%s'\n", text_event_names[type], ( char * ) txt );
  }
  
  void    MIDIFileShow::mf_eot ( MIDITickMS time )
  {
    show_time ( time );
    fprintf ( out, "End Of Track\n" );
  }
  
  
  
}
