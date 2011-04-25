/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __MIDI_DEVICE_H__6EE03181_DEEC_4771_A660_1597700B5773__
#define __MIDI_DEVICE_H__6EE03181_DEEC_4771_A660_1597700B5773__

#include <string>
#include <vector>

namespace MusicNoteLib
{
	/// Returns the number of Output Midi Ports
	unsigned int GetMidiOutPortCount();

	/// Returns the number of Input Midi Ports
	unsigned int GetMidiInPortCount();

	/// Returns the name of the given Midi Input Port
	std::string GetMidiInPortName(unsigned int nPortIndex);

	/// Returns the name of the given Midi Output Port
	std::string GetMidiOutPortName(unsigned int nPortIndex);

	typedef std::vector<std::string> StringList;
	
	/// Returns the names of all Midi Input Ports available
	StringList GetMidiInPortNames();

	/// Returns the names of all Midi Output Ports available
	StringList GetMidiOutPortNames();

} // namespace MusicNoteLib

#endif // __MIDI_DEVICE_H__6EE03181_DEEC_4771_A660_1597700B5773__