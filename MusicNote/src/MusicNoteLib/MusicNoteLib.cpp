#include "stdafx.h"

#ifdef _USRDLL
#define MUSICNOTELIB_API __declspec(dllexport)
#endif
#include "MusicNoteLib.h"

namespace MusicNoteLib
{
    struct PARSETRACEARGS
    {
        LPFNTRACEPROC lpfnTraceProc;
        void* lpUserData;
    };

    static void OnParseTrace(const MusicNoteLib::CParser* pParser, MusicNoteLib::CParser::TraceEventHandlerArgs* pEvArgs)
    {
        const PARSETRACEARGS* pCallbackData = (const PARSETRACEARGS*) pParser->GetUserData();
        pCallbackData->lpfnTraceProc(pCallbackData->lpUserData, pEvArgs->szTraceMsg);
    }

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

	MUSICNOTELIB_API bool PlayMusicString(LPCTSTR szMusicNotes)
	{
		Player playerObj;
		return playerObj.Play(szMusicNotes);
	}
	
	MUSICNOTELIB_API bool PlayMusicStringWithOpts(LPCTSTR szMusicNotes, int nMidiOutPortID, unsigned int nTimerResMS)
	{
		Player playerObj;
		return playerObj.Play(szMusicNotes, nMidiOutPortID, nTimerResMS);
	}

    MUSICNOTELIB_API bool SaveAsMidiFile(const TCHAR* szMusicNotes, const char* szOutputFilePath)
    {
        Player playerObj;
        return playerObj.SaveAsMidiFile(szMusicNotes, szOutputFilePath);
    }



    static void OnParseError(const MusicNoteLib::CParser* pParser, MusicNoteLib::CParser::ErrorEventHandlerArgs* pEvArgs)
    {
	    OutputDebugString(_T("\nError --> "));
	    OutputDebugString(pEvArgs->szErrMsg);
	    if(pEvArgs->szToken)
	    {
		    OutputDebugString(_T("\t Token: "));	 
		    OutputDebugString(pEvArgs->szToken);
	    }
    }

    MUSICNOTELIB_API void Parse(const TCHAR* szNotes, LPFNTRACEPROC traceCallbackProc, void* pCallbackData)
    {
        PARSETRACEARGS callbackArgs;
        callbackArgs.lpfnTraceProc = traceCallbackProc;
        callbackArgs.lpUserData = pCallbackData;

        Player playerObj;
        playerObj.Parser().SetUserData(&callbackArgs);
        playerObj.Parser().evTrace.Subscribe(OnParseTrace);
        playerObj.Play(szNotes);
    }

} // extern "C"

} // namespace MusicNoteLib