/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2011 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://cfugue.sourceforge.net/>.
*/
#if defined _WIN32 || defined WIN32

#else	// only if not Windows

#include "AlsaDriver.h"
#include "MidiTimer.h"

using namespace jdkmidi;

namespace CFugue
{
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

    bool MIDIDriverAlsa::HardwareMsgOut ( const jdkmidi::MIDITimedBigMessage &msg )
    {
        if(m_pMidiOut != NULL)
        {
            unsigned char status = msg.GetStatus();

            // dont send sysex or meta-events

            if ( status <0xff && status !=0xf0 )
            {
                unsigned char msgBytes[] = {status, msg.GetByte1(), msg.GetByte2(), msg.GetByte3()};

                std::vector<unsigned char> vec(msgBytes, msgBytes+3);

                m_pMidiOut->sendMessage(&vec);
            }

            return true;
        }
        return false;
    }

    // This is thread procedure to pump MIDI events
    // We maintain the supplied Timer Resolution by adjusting the sleep duration
	bool AlsaDriverThreadProc(MIDIDriverAlsa* pAlsaDriver, int nTimerResMS)
	{
		MidiTimer::TimePoint tBefore, tAfter;
	    unsigned long nElapsed, nTimeToSleep;

	    while(true)
	    {
            tBefore = MidiTimer::Now();

            if(pAlsaDriver->TimeTick(tBefore) == false) break;

            tAfter = MidiTimer::Now();

			nElapsed = std::chrono::duration_cast<MidiTimer::Duration>(tAfter - tBefore).count();

            nTimeToSleep = (nElapsed > nTimerResMS ? 0 : nTimerResMS - nElapsed);

            MidiTimer::Sleep(nTimeToSleep);
	    }

        return true;
	}

	bool MIDIDriverAlsa::StartTimer ( int res )
	{
	    if(m_bgTaskResult.valid()) // Already running
            return false;

        m_bgTaskResult = std::async(std::launch::async, &AlsaDriverThreadProc, this, res);

        return m_bgTaskResult.valid();
	}

	void MIDIDriverAlsa::WaitTillDone()
	{
	    if(m_bgTaskResult.valid() == false) return; // if not running

        auto waitStatus = m_bgTaskResult.wait_for(std::chrono::milliseconds(0));

        while(waitStatus != std::future_status::ready)
        {
             waitStatus = m_bgTaskResult.wait_for(std::chrono::milliseconds(500));
        }
	}

	void MIDIDriverAlsa::StopTimer()
	{
	    // std::future requires get() to be called before it can be used again.
	    // valid() keeps returning true till get() is called. And get() can be
	    // called only once. Once it is called valid() becomes false again.
	    if(m_bgTaskResult.valid())
            m_bgTaskResult.get();
	}

	void MIDIDriverAlsa::CloseMIDIInPort()
	{
	    if(m_pMidiIn != NULL)
	    {
	        m_pMidiIn->closePort();
	        delete m_pMidiIn;
	        m_pMidiIn = NULL;
	    }
	}

	void MIDIDriverAlsa::CloseMIDIOutPort()
	{
	    if(m_pMidiOut != NULL)
	    {
	        m_pMidiOut->closePort();
	        delete m_pMidiOut;
	        m_pMidiOut = NULL;
	    }
	}

} // namespace CFugue



#endif // _ifndef _Win32
