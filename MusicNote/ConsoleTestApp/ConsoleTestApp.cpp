// ConsoleTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//#include "MusicNoteReader.h"
#include "MusicNoteLib.h"


void OnParseTrace(const MusicNoteLib::CParser*, MusicNoteLib::MusicStringParser::TraceEventHandlerArgs* pEvArgs)
{
	OutputDebugString(_T("\n"));
	OutputDebugString(pEvArgs->szTraceMsg);
}

void OnParseError(const MusicNoteLib::CParser*, MusicNoteLib::MusicStringParser::ErrorEventHandlerArgs* pEvArgs)
{
	OutputDebugString(_T("\nError --> "));
	OutputDebugString(pEvArgs->szErrMsg);
	if(pEvArgs->szToken)
	{
		OutputDebugString(_T("\t Token: "));	 
		OutputDebugString(pEvArgs->szToken);
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	MusicNoteLib::GetCarnaticMusicNoteReader();

	MusicNoteLib::MusicStringParser Parser;

	// Subscribe to the Events
	Parser.evTrace.Subscribe(&OnParseTrace);
	Parser.evError.Subscribe(&OnParseError);

	bool bSuccess = false;
	
	// Dictionary Element Definition
	bSuccess = Parser.ParseToken(_T("$Val=12.5"));
	if(bSuccess == false) printf("\n Error while parsing $Val=12.5\n");

	// Numeric Note
	bSuccess = Parser.ParseToken(_T("[60]wq")); // Middle-C (C5) whole quarter Note
	if(bSuccess == false) printf("\n Error while parsing [60]wq\n");

	// Alphabetic Note
	bSuccess = Parser.ParseToken(_T("C10wq")); // C in 10 th octave whole quarter Note
	if(bSuccess == false) printf("\n Error while parsing C10wq\n");
	bSuccess = Parser.ParseToken(_T("Cwq")); // Default-C  whole quarter Note
	if(bSuccess == false) printf("\n Error while parsing Cwq\n");
	bSuccess = Parser.ParseToken(_T("Cwx*5:4")); // Default-C  whole quarter Note
	if(bSuccess == false) printf("\n Error while parsing Cwx*3:2\n");
	bSuccess = Parser.ParseToken(_T("Cwx*")); // Default-C  whole sixteenth Note as a triplet
	if(bSuccess == false) printf("\n Error while parsing Cwx*\n");
	bSuccess = Parser.ParseToken(_T("C*")); // Default-C  whole quarter Note
	if(bSuccess == false) printf("\n Error while parsing C*\n");
	bSuccess = Parser.ParseToken(_T("C/120")); // Default-C  whole quarter Note with duration 120
	if(bSuccess == false) printf("\n Error while parsing C*\n");
	bSuccess = Parser.ParseToken(_T("G##/120")); // Default-GSharpSharp  whole quarter Note with duration 120
	if(bSuccess == false) printf("\n Error while parsing G##/120\n");
	bSuccess = Parser.ParseToken(_T("Cbb/120")); // Default-CFlatFlat  whole quarter Note with duration 120
	if(bSuccess == false) printf("\n Error while parsing Cbb/120\n");

	// Note Velocities
	bSuccess = Parser.ParseToken(_T("Cb4qa45")); // CFlat in 4th Octave quarter note with attack velocity 45
	if(bSuccess == false) printf("\n Error while parsing Cb4qa45\n");
	bSuccess = Parser.ParseToken(_T("Gb5hd67")); // GFlat in 5th Octave half note with Decay velocity 67
	if(bSuccess == false) printf("\n Error while parsing Gb5hd67\n");
	bSuccess = Parser.ParseToken(_T("F#4Wa55d77")); // FSharp in 4th Octave Whole note with Attack 55, Decay 77
	if(bSuccess == false) printf("\n Error while parsing F#4Wa55d7\n");
	bSuccess = Parser.ParseToken(_T("B4Wa[AtkVol]d[DecVol]")); // B 4th Octave Whole note with Attack [AtkVol], Decay [DecVol]
	if(bSuccess == false) printf("\n Error while parsing B4Wa[AtkVol]d[DecVol]\n");


	// Error Dealing Robustness
	bSuccess = Parser.ParseToken(_T("C[wq")); // should be able to ignore [
	bSuccess = Parser.ParseToken(_T("C[]wq")); // should be able to ignore [ and ]
	bSuccess = Parser.ParseToken(_T("C[5]wq")); // should be able to ignore [ and ]
	bSuccess = Parser.ParseToken(_T("C[Bingo]wq")); // should be able to ignore [ and ] and Bingo
	bSuccess = Parser.ParseToken(_T("C[")); // should be able to ignore [ 
	bSuccess = Parser.ParseToken(_T("C[]")); // should be able to ignore [ and ]
	bSuccess = Parser.ParseToken(_T("C[Bingo]")); // should be able to ignore [ and ] and Bingo
	bSuccess = Parser.ParseToken(_T("C10wq C5wq")); // parsing should stop at the first space character
	bSuccess = Parser.ParseToken(_T("C10 C5wq")); // parsing should stop at the first space character
	bSuccess = Parser.ParseToken(_T("Cwx*Ding:Dong")); // parsing should be able to read the Tuplet as Triplet
	bSuccess = Parser.ParseToken(_T("Cwx*Ding:")); // parsing should be able to read the Tuplet as Triplet
	bSuccess = Parser.ParseToken(_T("Cwx*5:")); // parsing should be able to read the Tuplet
	bSuccess = Parser.ParseToken(_T("C/")); // parsing should be able to ignore the /
	bSuccess = Parser.ParseToken(_T("C/WH")); // parsing should be able to ignore the /
	bSuccess = Parser.ParseToken(_T("C/*5:4")); // parsing should be able to ignore the /
	bSuccess = Parser.ParseToken(_T("[260]wq")); // should be able to ceil the numeric note to 127

	MusicNoteLib::Player player;

	player.Play(_T("C D E F G A B"));

	getchar();

	player.Stop();

	return 0;
}

