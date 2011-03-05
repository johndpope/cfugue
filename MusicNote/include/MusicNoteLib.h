/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __MUSICNOTE_LIB_H__EBEE094C_FF6E_43a1_A6CE_D619564F9C6A__
#define __MUSICNOTE_LIB_H__EBEE094C_FF6E_43a1_A6CE_D619564F9C6A__

/** @file MusicNoteLib.h
 * \brief Main header file for accessing the MusicNote Library
 */

/// <Summary>
/// This header file can be included directly in your project or through
/// MusicNoteDll.h of the MusicNoteDll project. If included directly, this
/// will be built directly as a satic library. If included through MusicNoteDll.h
/// this will use dllImports through MUSICNOTELIB_API
/// </Summary>
#ifndef MUSICNOTELIB_API
#define MUSICNOTELIB_API
#endif // MUSICNOTELIB_API

#include "Player.h"
#include "MidiDevice.h"

#if defined WIN32 || defined _WIN32
#define STDCALL __stdcall
#else
#define STDCALL  //__attribute__((stdcall))
#endif

namespace MusicNoteLib /// Music Programming Library
{
typedef  void (STDCALL *LPFNTRACEPROC)(void* pUserData, const TCHAR* szTraceMsg);
typedef  void (STDCALL *LPFNERRORPROC)(void* pUserData, long lErrCode, const TCHAR* szErrorMsg, const TCHAR* szToken);

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
	/// Same as PlayMusicString() except that this method accepts Callbacks.
    /// The Trace and Error callbacks will be used during the Parse of the Music Notes.
    /// @param szMusicNotes the Music string to be played on the MIDI output device
    /// @param traceCallbackProc the Callback to used to report Trace messages
    /// @param errorCallbackProc the Callback to used to report Error messages
    /// @param pUserData any user supplied data that should be sent to the Callback
    /// @return True if the notes were played successfully, False otherwise
	/// </Summary>
	MUSICNOTELIB_API bool PlayMusicStringCB(const TCHAR* szMusicNotes,
											LPFNTRACEPROC traceCallbackProc,
											LPFNERRORPROC errorCallbackProc,
											void* pUserData);

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
	/// Same as PlayMusicStringWithOpts() except that this method accepts Callbacks.
    /// The Trace and Error callbacks will be used during the Parse of the Music Notes.
    /// @param szMusicNotes the Music notes to be played
    /// @param nMidiOutPortID the device ID of the MIDI output port to be used for the play
    /// @param nTimerResMS preferred MIDI timer resolution, in MilliSeconds
    /// @param traceCallbackProc the Callback to used to report Trace messages
    /// @param errorCallbackProc the Callback to used to report Error messages
    /// @param pUserData any user supplied data that should be sent to the Callback
    /// @return True if Play was successful, False otherwise
	/// </Summary>
	MUSICNOTELIB_API bool PlayMusicStringWithOptsCB(const TCHAR* szMusicNotes,
													int nMidiOutPortID,
													unsigned int nTimerResMS,
													LPFNTRACEPROC traceCallbackProc,
													LPFNERRORPROC errorCallbackProc,
													void* pUserData);
    /// <Summary>
    /// Save the given MusicString content into a MIDI output file
    /// @param szMusicNotes Music Notes to be converted to MIDI output
    /// @param szOutputFilePath path of the MIDI output file
    /// @return True if the the content was saved successfully, False otherwise
    /// </Summary>
    MUSICNOTELIB_API bool SaveAsMidiFile(const TCHAR* szMusicNotes, const char* szOutputFilePath);

    //MUSICNOTELIB_API typedef void (*ParseErrorProc)(const MusicNoteLib::CParser*, MusicNoteLib::CParser::ErrorEventHandlerArgs* pEvArgs);
    //MUSICNOTELIB_API typedef void (*ParseTraceProc)(const MusicNoteLib::CParser*, MusicNoteLib::CParser::TraceEventHandlerArgs* pEvArgs);

    MUSICNOTELIB_API void Parse(const TCHAR* szNotes, LPFNTRACEPROC traceCallbackProc, void* pUserData);

} // extern "C"



} // namespace MusicNoteLib

#endif // __MUSICNOTE_LIB_H__EBEE094C_FF6E_43a1_A6CE_D619564F9C6A__
