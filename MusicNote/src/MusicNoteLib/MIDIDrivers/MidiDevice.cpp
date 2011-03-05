/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#include "MidiDevice.h"
#include "rtmidi/RtMidi.h"

namespace MusicNoteLib
{
	unsigned int GetMidiOutPortCount()
	{
		static RtMidiOut midiOutObj;
		return midiOutObj.getPortCount();		
	}

	unsigned int GetMidiInPortCount()
	{
		static RtMidiIn midiInObj;
		return midiInObj.getPortCount();
	}

	std::string GetMidiInPortName(unsigned int nPortIndex)
	{
		static RtMidiIn midiInObj;
		return midiInObj.getPortName(nPortIndex);
	}

	std::string GetMidiOutPortName(unsigned int nPortIndex)
	{
		static RtMidiOut midiOutObj;
		return midiOutObj.getPortName(nPortIndex);
	}

} // namespace MusicNoteLib 