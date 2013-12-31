/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://cfugue.sourceforge.net/>.

    $LastChangedDate$
    $Rev$
    $LastChangedBy$
*/

#ifndef _MIDI_TIMER_H__D2A4A592_DAE0_46DC_ADA6_6191407F567E__
#define _MIDI_TIMER_H__D2A4A592_DAE0_46DC_ADA6_6191407F567E__

namespace CFugue
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
		static unsigned long CurrentTimeOffset();

		///<Summary>
		/// Causes the calling thread to sleep
		/// @param ms sleep duration (in milli-seconds)
		///</Summary>
		static void Sleep(unsigned long ms);
	};

} // namespace CFugue

#endif // _MIDI_TIMER_H__D2A4A592_DAE0_46DC_ADA6_6191407F567E__
