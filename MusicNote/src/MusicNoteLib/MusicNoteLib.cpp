#include "stdafx.h"

#ifdef _USRDLL
#define MUSICNOTELIB_API __declspec(dllexport)
#endif
#include "MusicNoteLib.h"

extern "C"
{
    MUSICNOTELIB_API void* GetCarnaticMusicNoteReader()
	{	
		MusicNoteLib::MusicStringParser Parser;

		return NULL;
	}

} // extern "C"