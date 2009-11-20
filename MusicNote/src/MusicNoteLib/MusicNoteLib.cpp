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
        LPFNERRORPROC lpfnErrorProc;
        void* lpUserData;
    };

    static void OnParseTrace(const MusicNoteLib::CParser* pParser, MusicNoteLib::CParser::TraceEventHandlerArgs* pEvArgs)
    {
        const PARSETRACEARGS* pCallbackData = (const PARSETRACEARGS*) pParser->GetUserData();
        pCallbackData->lpfnTraceProc(pCallbackData->lpUserData, pEvArgs->szTraceMsg);
    }
    
	static void OnParseError(const MusicNoteLib::CParser* pParser, MusicNoteLib::CParser::ErrorEventHandlerArgs* pEvArgs)
    {
        const PARSETRACEARGS* pCallbackData = (const PARSETRACEARGS*) pParser->GetUserData();
		pCallbackData->lpfnErrorProc(pCallbackData->lpUserData, pEvArgs->errCode, pEvArgs->szErrMsg, pEvArgs->szToken);
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

    MUSICNOTELIB_API bool SaveAsMidiFile(const TCHAR* szMusicNotes, const char* szOutputFilePath)
    {
        Player playerObj;
        return playerObj.SaveAsMidiFile(szMusicNotes, szOutputFilePath);
    }

    //static void OnParseError(const MusicNoteLib::CParser* pParser, MusicNoteLib::CParser::ErrorEventHandlerArgs* pEvArgs)
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

	MUSICNOTELIB_API bool PlayMusicStringWithOpts(LPCTSTR szMusicNotes, int nMidiOutPortID, unsigned int nTimerResMS)
	{
		Player playerObj;
		return playerObj.Play(szMusicNotes, nMidiOutPortID, nTimerResMS);
	}

	MUSICNOTELIB_API bool PlayMusicStringWithOptsCB(LPCTSTR szMusicNotes, 
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

		Player playerObj;
        playerObj.Parser().SetUserData(&callbackArgs);
        playerObj.Parser().evTrace.Subscribe(OnParseTrace);
		playerObj.Parser().evError.Subscribe(OnParseError);
		return playerObj.Play(szMusicNotes, nMidiOutPortID, nTimerResMS);
	}
} // extern "C"

} // namespace MusicNoteLib