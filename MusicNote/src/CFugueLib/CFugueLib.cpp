/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://cfugue.sourceforge.net/>.
*/

#include "stdafx.h"

#ifdef _USRDLL
#define MUSICNOTELIB_API __declspec(dllexport)
#endif
#include "CFugueLib.h"

namespace CFugue
{
    struct PARSETRACEARGS
    {
        LPFNTRACEPROC lpfnTraceProc;
        LPFNERRORPROC lpfnErrorProc;
        void* lpUserData;
    };

    static void OnParseTrace(const CFugue::CParser* pParser, CFugue::CParser::TraceEventHandlerArgs* pEvArgs)
    {
        const PARSETRACEARGS* pCallbackData = (const PARSETRACEARGS*) pParser->GetUserData();
        pCallbackData->lpfnTraceProc(pCallbackData->lpUserData, pEvArgs->szTraceMsg);
    }

	static void OnParseError(const CFugue::CParser* pParser, CFugue::CParser::ErrorEventHandlerArgs* pEvArgs)
    {
        const PARSETRACEARGS* pCallbackData = (const PARSETRACEARGS*) pParser->GetUserData();
		pCallbackData->lpfnErrorProc(pCallbackData->lpUserData, pEvArgs->errCode, pEvArgs->szErrMsg, pEvArgs->szToken);
    }

extern "C"
{
    MUSICNOTELIB_API void* GetCarnaticMusicNoteReader()
	{
		CFugue::MusicStringParser Parser;

		return NULL;
	}

	MUSICNOTELIB_API MStringPlayer* CreateMusicStringPlayer()
	{
		return NULL;
	}

    MUSICNOTELIB_API bool SaveAsMidiFile(const TCHAR* szMusicNotes, const char* szOutputFilePath)
    {
        Player playerObj;
        return playerObj.SaveAsMidiFile(szMusicNotes, szOutputFilePath);
    }

    //static void OnParseError(const CFugue::CParser* pParser, CFugue::CParser::ErrorEventHandlerArgs* pEvArgs)
    //{
	   // OutputDebugString(_T("\nError --> "));
	   // OutputDebugString(pEvArgs->szErrMsg);
	   // if(pEvArgs->szToken)
	   // {
		  //  OutputDebugString(_T("\t Token: "));
		  //  OutputDebugString(pEvArgs->szToken);
	   // }
    //}

    MUSICNOTELIB_API bool PlayMusicString(const TCHAR* szNotes)
    {
        return Player().Play(szNotes);
    }

    MUSICNOTELIB_API bool PlayMusicStringCB(const TCHAR* szNotes,
											LPFNTRACEPROC traceCallbackProc,
											LPFNERRORPROC errorCallbackProc,
											void* pCallbackData)
    {
        PARSETRACEARGS callbackArgs;
        callbackArgs.lpfnTraceProc = traceCallbackProc;
		callbackArgs.lpfnErrorProc = errorCallbackProc;
        callbackArgs.lpUserData = pCallbackData;

        Player playerObj;
        playerObj.Parser().SetUserData(&callbackArgs);
        playerObj.Parser().evTrace.Subscribe(OnParseTrace);
		playerObj.Parser().evError.Subscribe(OnParseError);
        return playerObj.Play(szNotes);
    }

	MUSICNOTELIB_API bool PlayMusicStringWithOpts(const TCHAR* szMusicNotes, int nMidiOutPortID, unsigned int nTimerResMS)
	{
		Player playerObj(nMidiOutPortID, nTimerResMS);
		return playerObj.Play(szMusicNotes);
	}

	MUSICNOTELIB_API bool PlayMusicStringWithOptsCB(const TCHAR* szMusicNotes,
													int nMidiOutPortID,
													unsigned int nTimerResMS,
													LPFNTRACEPROC traceCallbackProc,
													LPFNERRORPROC errorCallbackProc,
													void* pCallbackData)
	{
        PARSETRACEARGS callbackArgs;
        callbackArgs.lpfnTraceProc = traceCallbackProc;
		callbackArgs.lpfnErrorProc = errorCallbackProc;
        callbackArgs.lpUserData = pCallbackData;

		Player playerObj(nMidiOutPortID, nTimerResMS);
        playerObj.Parser().SetUserData(&callbackArgs);
        playerObj.Parser().evTrace.Subscribe(OnParseTrace);
		playerObj.Parser().evError.Subscribe(OnParseError);
		return playerObj.Play(szMusicNotes);
	}
} // extern "C"

} // namespace CFugue
