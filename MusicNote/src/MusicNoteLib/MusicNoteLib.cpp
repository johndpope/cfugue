#include "stdafx.h"

#ifdef _USRDLL
#define MUSICNOTELIB_API __declspec(dllexport)
#endif
#include "MusicNoteLib.h"

namespace MusicNoteLib
{

extern "C"
{
    MUSICNOTELIB_API void* GetCarnaticMusicNoteReader()
	{	
		MusicNoteLib::MusicStringParser Parser;

		return NULL;
	}

	MUSICNOTELIB_API MStringPlayer* CreateMusicStringPlayer()
	{
		return NULL;
	}

	/// <Summary>
	/// Plays Music string notes on default MIDI Output device with default Timer Resolution.
	/// Use PlayMusicStringWithOpts to use non-default values.
	/// </Summary>
	MUSICNOTELIB_API bool PlayMusicString(LPCTSTR szMusicNotes)
	{
		Player playerObj;
		return playerObj.Play(szMusicNotes);
	}
	
	/// <Summary>
	/// Plays Music string notes on given MIDI Output device using given Timer Resolution.
	/// Use PlayMusicString to use default values.
	/// </Summary>
	MUSICNOTELIB_API bool PlayMusicStringWithOpts(LPCTSTR szMusicNotes, int nMidiOutPortID, unsigned int nTimerResMS)
	{
		Player playerObj;
		return playerObj.Play(szMusicNotes, nMidiOutPortID, nTimerResMS);
	}

} // extern "C"

} // namespace MusicNoteLib