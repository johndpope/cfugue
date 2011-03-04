/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2011 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/
#if defined _WIN32 || defined WIN32

#else	// only if not Windows

#include "AlsaDriver.h"

#if defined __GNUC__	// MSVC doesnt have support for std::thread yet
#include <thread>
#endif

using namespace jdkmidi;

  MIDIDriverAlsa::MIDIDriverAlsa ( int queue_size )
      :
      MIDIDriver ( queue_size ),
      m_pMidiIn ( 0 ),
      m_pMidiOut ( 0 ),
      m_pThread ( NULL )
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

	bool MIDIDriverAlsa::StartTimer ( int res )
	{
	    if(m_pThread == NULL)
	    {
	        m_pThread = new std::thread(std::Ref(*this));

	    }
	}

#endif // _ifndef _Win32