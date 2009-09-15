#ifndef __MUSICNOTE_LIB_H__EBEE094C_FF6E_43a1_A6CE_D619564F9C6A__
#define __MUSICNOTE_LIB_H__EBEE094C_FF6E_43a1_A6CE_D619564F9C6A__

/// <Summary>
/// This header file can be included directly in your project or through
/// MusicNoteLib.h of the MusicNoteDll project. If included directly, this
/// will be built directly as a satic library. If included through MusicNoteDll
/// this will use dllImports through MUSICNOTELIB_API
/// </Summary>
#ifndef MUSICNOTELIB_API
#define MUSICNOTELIB_API
#endif // MUSICNOTELIB_API

#include "Player.h"

namespace MusicNoteLib /// Music Programming Library
{
typedef  void (__stdcall *LPFNTRACEPROC)(void* pUserData, const TCHAR* szTraceMsg);

extern "C"
{
	MUSICNOTELIB_API typedef void MStringPlayer;

    MUSICNOTELIB_API void* GetCarnaticMusicNoteReader();

	/// <Summary>
	/// Creates a MusicString Player object.
	/// </Summary>
    MUSICNOTELIB_API MStringPlayer* CreateMusicStringPlayer();

	/// <Summary>
	/// Plays Music string notes on the default MIDI Output device with the default Timer Resolution.
    /// Use PlayMusicStringWithOpts() to use custom values.
    /// @param szMusicNotes the Music string to be played on the MIDI output device
    /// @return True if the notes were played successfully, False otherwise
	/// </Summary>
	MUSICNOTELIB_API bool PlayMusicString(const TCHAR* szMusicNotes);
	
	/// <Summary>
	/// Plays Music string notes on the given MIDI Output device using the given Timer Resolution.
    /// Use PlayMusicString() to use default values.
    /// @param szMusicNotes the Music notes to be played
    /// @param nMidiOutPortID the device ID of the MIDI output port to be used for the play
    /// @param nTimerResMS preferred MIDI timer resolution, in MilliSeconds
    /// @return True if Play was successful, False otherwise
	/// </Summary>
	MUSICNOTELIB_API bool PlayMusicStringWithOpts(const TCHAR* szMusicNotes, int nMidiOutPortID, unsigned int nTimerResMS);

    /// <Summary>
    /// Save the given MusicString content into a MIDI output file
    /// @param szMusicNotes Music Notes to be converted to MIDI output
    /// @param szOutputFilePath path of the MIDI output file
    /// @return True if the the content was saved successfully, False otherwise
    /// </Summary>
    MUSICNOTELIB_API bool SaveAsMidiFile(const TCHAR* szMusicNotes, const char* szOutputFilePath);

    //MUSICNOTELIB_API typedef void (*ParseErrorProc)(const MusicNoteLib::CParser*, MusicNoteLib::CParser::ErrorEventHandlerArgs* pEvArgs);
    MUSICNOTELIB_API typedef void (*ParseTraceProc)(const MusicNoteLib::CParser*, MusicNoteLib::CParser::TraceEventHandlerArgs* pEvArgs);

    MUSICNOTELIB_API void Parse(const TCHAR* szNotes, LPFNTRACEPROC traceCallbackProc, void* pCallbackData);

} // extern "C"
    


} // namespace MusicNoteLib


/*! \mainpage MusicNoteLib, The C++ Music Programming Library

\section overview Overview

Popularily referred to as CFugue, meaning Carnatic Fugue or the C/C++
replacement of JFugue, MusicNoteLib is a high level Music Programming Library.

This library makes it possible to play music notes directly from C/C++ programs,
without ever having to deal with the low-level MIDI complexities. This library 
provides a beautiful abstraction that lets you concetrate on programming the <i>Music</i> rather
than worry about the MIDI nuances.

Provides features that makes it possible to use this library directly from many 
platforms, including but not limited to ASP pages, .Net applications and even non-Windows
based systems.

\section usage Usage

Using this library to play music is as easy as writing plain music notes. Just create
a Player object and call the Play method on it supplying the Music notes to be played. Simple.

Example Usage:
<pre>
    MusicNoteLib::Player player; // Create the Player Object    
    player.Play("C D E F G A B"); // Play the Music Notes on the default MIDI output port
</pre>

\section history History

\section theory Theory of Operation

\section limitations Limitations

\section future The Future

\section copyright Copyright

 Copyright (C) 2009 CineFx Digital Media Pvt. Ltd.

This is a product of CineFx Research Labs, distributed under LGPL with the hope that it will be useful. 
No warranty of what-so-ever is implied, including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
Refer to LGPL for more details.

 Author: Gopalakrishna Palem

 Web: http://www.CineFxLabs.com

*/

#endif // __MUSICNOTE_LIB_H__EBEE094C_FF6E_43a1_A6CE_D619564F9C6A__