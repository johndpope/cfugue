/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef _MIDI_TIMER_H__D2A4A592_DAE0_46DC_ADA6_6191407F567E__
#define _MIDI_TIMER_H__D2A4A592_DAE0_46DC_ADA6_6191407F567E__

#if defined WIN32 || defined _WIN32
#include <MMSystem.h>
#else
#include <chrono>
#endif

namespace MusicNoteLib
{
	///<Summary>Plays the role of a pseudo MIDI Timer for MIDI Sequencer</Summary>
	class MidiTimer
	{
	public:
		///<Summary>
		/// Returns elapsed time in MilliSeconds for MIDI Sequencing.
		/// On Windows it is the elapsed time since Windows started.
		/// On other systems it is the time elapsed since epoch.
		/// @return psuedo time tick offset that is suitable for MIDI sequencer
		///</Summary>
		static inline unsigned long CurrentTimeOffset()
		{
#if defined WIN32 || defined _WIN32
			return timeGetTime();
#else
			auto t0 = std::chrono::high_resolution_clock::now();			
			   
			return std::chrono::duration_cast<std::chrono::milliseconds>(t0.time_since_epoch()).count();
#endif
		}
	};

} // namespace MusicNoteLib

#endif // _MIDI_TIMER_H__D2A4A592_DAE0_46DC_ADA6_6191407F567E__