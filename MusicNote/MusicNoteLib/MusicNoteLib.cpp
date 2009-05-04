#include "stdafx.h"

#ifdef _USRDLL
#define MUSICNOTELIB_API __declspec(dllexport)
#endif
#include "MusicNoteLib.h"

extern "C"
{
    MUSICNOTELIB_API void* GetCarnaticMusicNoteReader()
	{
		return NULL;
	}

} // extern "C"