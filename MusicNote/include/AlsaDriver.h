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
#include <future> // std::future

namespace MusicNoteLib
{
	///<Summary>MIDI Driver for Linux Alsa based machines</Summary>
	class MIDIDriverAlsa : public jdkmidi::MIDIDriver
	{
		RtMidiIn*		    m_pMidiIn;
		RtMidiOut*	        m_pMidiOut;
        std::future<bool>   m_bgTaskResult;
	public:
		MIDIDriverAlsa ( int queue_size );
		virtual ~MIDIDriverAlsa();

		void ResetMIDIOut();

        /// <Summary>
        /// Creates a background thread to pump MIDI events
        /// at the supplied timer resolution.
        /// Use WaitTillDone() to wait till the background processing completes.
        /// Use StopTimer() after the background processing is completed, to release resources.
        /// @param resolution_ms MIDI Timer resolution in milliseconds
        /// @return false if background thread cannot be started
        /// </Summary>
		bool StartTimer ( int resolution_ms );

		/// Waits (blocks) till the background thread created with StartTimer()
		/// completes its processing.
		/// After StartTimer() succeeds, use WaitTillDone() followed by StopTimer().
		/// Returns immediately if no background thread is running.
		void WaitTillDone();

        /// Call StopTimer() to release the resources used by the background
        /// procedure created with StartTimer(). StopTimer() Should be called
        /// <i>after</i> the background procedure is done (indicated by BGThreadStatus::COMPLETED).
        /// If background procedure is still running while StopTimer() is called, caller gets
        /// blocked till the background procedure completes.
        /// If no background procedure exists, returns immediately.
		void StopTimer();

		/// Opens the MIDI input port with the given ID
		/// @return false if the given input port cannot be opened
		bool OpenMIDIInPort ( int id );

        /// Opens the MIDI output port with the given ID
        /// @return false if the given output port cannot be opened
		bool OpenMIDIOutPort ( int id );

		/// Closed any previously opened MIDI Input port
		void CloseMIDIInPort();

		/// Closed any previously opened MIDI Output port
		void CloseMIDIOutPort();

		enum BGThreadStatus {   RUNNING,    ///< Async procedure is running - use WaitTillDone() to wait for completion
                                COMPLETED,  ///< Async procedure completed running - use StopTimer() to finish
                                INVALID     ///< No background procedure running - use StartTimer() to start one
                            };
	protected:
		bool HardwareMsgOut ( const jdkmidi::MIDITimedBigMessage &msg );

		std::thread* m_pThread;

		int timer_id;
		int timer_res;

		//bool in_open;
		//bool out_open;
		//bool timer_open;
	};

} // namespace MusicNoteLib

#endif // __ALSADRIVER_H__9857EC39_234E_411E_9558_EFDA218796AA__
