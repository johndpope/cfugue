/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2011 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#include "AlsaDriver.h"

using namespace jdkmidi;

  MIDIDriverAlsa::MIDIDriverAlsa ( int queue_size )
      :
      MIDIDriver ( queue_size ),
      m_pMidiIn ( 0 ),
      m_pMidiOut ( 0 ),
      //in_open ( false ),
      //out_open ( false ),
      timer_open ( false )
  {
  }
  
  MIDIDriverAlsa::~MIDIDriverAlsa()
  {
    StopTimer();
    CloseMIDIInPort();
    CloseMIDIOutPort();
  }

	bool MIDIDriverAlsa::OpenMIDIInPort ( int id )
	{
		if(m_pMidiIn != NULL) // close any open port
		{
			delete m_pMidiIn;
			m_pMidiIn = NULL;
		}
		if(m_pMidiIn == NULL)
		{
			try
			{
				m_pMidiIn = new RtMidiIn("MIDIDriverAlsa Client");
			}
			catch ( RtError &error ) 
			{
				error.printMessage();			
				return false;
			}			
		}
		if(m_pMidiIn != NULL)
		{
			try
			{
				m_pMidiIn->openPort(id);
			}
			catch(RtError& error)
			{
				error.printMessage();
				return false;
			}
		}
		return true;
	}


	bool MIDIDriverAlsa::OpenMIDIOutPort ( int id )
	{
		if(m_pMidiOut != NULL) // close any open port
		{
			delete m_pMidiOut;
			m_pMidiOut = NULL;
		}
		if(m_pMidiOut == NULL)
		{
			try
			{
				m_pMidiOut = new RtMidiOut("MIDIDriverAlsa Client");
			}
			catch(RtError &error)
			{
				error.printMessage();
				return false;
			}
		}
		if(m_pMidiOut != NULL)
		{
			try
			{
				m_pMidiOut->openPort(id);
			}
			catch(RtError &error)
			{
				error.printMessage();
				return false;
			}
		}
		return true;
	}
