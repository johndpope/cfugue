/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://cfugue.sourceforge.net/>.
*/

#include "MidiDevice.h"
#include "rtmidi/RtMidi.h"

namespace CFugue
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

	StringList GetMidiInPortNames()
	{
		StringList strRetList;

		for(int i=0, nMax = GetMidiInPortCount(); i < nMax; ++i)
			strRetList.push_back(GetMidiInPortName(i));

		return strRetList;
	}

	StringList GetMidiOutPortNames()
	{
		StringList strRetList;

		for(int i=0, nMax = GetMidiOutPortCount(); i < nMax; ++i)
			strRetList.push_back(GetMidiOutPortName(i));

		return strRetList;
	}

} // namespace CFugue 