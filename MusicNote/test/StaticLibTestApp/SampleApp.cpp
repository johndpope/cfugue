/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

// SampleApp.cpp
//
// Demonstrates the usage of CFugue as a Statically Linked Library
//
// CFugue is under active development.
// Please refer to documentation and latest releases at: http://musicnote.sourceforge.net/

#include "stdafx.h"
#include "stdlib.h"

#include "MusicNoteLib.h"

#if defined WIN32 || defined _WIN32
   #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

void OnParseTrace(const MusicNoteLib::CParser*, MusicNoteLib::CParser::TraceEventHandlerArgs* pEvArgs)
{
    _tprintf(_T("\n\t"));
	_tprintf(pEvArgs->szTraceMsg);
}

void OnParseError(const MusicNoteLib::CParser*, MusicNoteLib::CParser::ErrorEventHandlerArgs* pEvArgs)
{
	_tprintf(_T("\n\t Error --> ")); _tprintf(pEvArgs->szErrMsg);
	if(pEvArgs->szToken)
	{
		_tprintf(_T("\t Token: ")); _tprintf(pEvArgs->szToken);
	}
}

#if defined _UNICODE || defined UNICODE
#define Str_Type std::wstring
    std::wstring ToUnicode(std::string&& str)
    {
        wchar_t *pSz = new wchar_t[str.length() +2];
        mbstowcs(pSz, str.c_str(), str.length()+1);
        std::wstring strRetVal(pSz);
        delete pSz;
        return strRetVal;
    }
#else
#define Str_Type std::string
#define ToUnicode(x)    x
#endif

int main(int argc, char* argv[])
{
	int nPortID=MIDI_MAPPER, nTimerRes = 20;

	if(argc < 2)
	{
		unsigned int nOutPortCount = MusicNoteLib::GetMidiOutPortCount();
		if(nOutPortCount <= 0)
			exit(fprintf(stderr, "No MIDI Output Ports found !!"));
		if(nOutPortCount > 1)
		{
			_tprintf(_T("\n---------  The MIDI Out Port indices  --------\n\n"));
			/////////////////////////////////////////
			/// List the MIDI Ports
			for(unsigned int i=0; i < nOutPortCount; ++i)
            {
                Str_Type strName = ToUnicode(MusicNoteLib::GetMidiOutPortName(i));
                _tprintf(_T("\t %d \t: "), i );
                _tprintf(strName.c_str());
                _tprintf(_T("\n"));
            }
			//////////////////////////////////////////
			/// Chose a Midi output port
			_tprintf(_T("\nChoose your MIDI Port ID for the play:"));
			_tscanf(_T("%d"), &nPortID);
		}
		else
		{
            Str_Type strName = ToUnicode(MusicNoteLib::GetMidiOutPortName(0));
			_tprintf(_T("\nUsing the MIDI output port: "));
            _tprintf(strName.c_str());
            _tprintf(_T("\n"));
		}
	}
	else if(argc == 2)
	{
		nPortID = atoi(argv[1]);
	}
	else if(argc > 2)
	{
		nPortID = atoi(argv[1]);
		nTimerRes = atoi(argv[2]);
	}


	_tprintf(_T("\n -------- C API Tests ------------\n"));
	/////////////////////////////////////////
	/// C API
	///
	_tprintf(_T("\nPlaying Notes.."));
	MusicNoteLib::PlayMusicStringWithOpts(_T("C D E F G A B"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nPlaying Chrods.."));
	MusicNoteLib::PlayMusicStringWithOpts(_T("CMajH DMaj EMajH"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nPlaying Parallel Notes.."));
	MusicNoteLib::PlayMusicStringWithOpts(_T("C+D+E+F+G+A+B"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nPlaying Sequential Notes.."));
	MusicNoteLib::PlayMusicStringWithOpts(_T("B_A_G_F_E_D_C"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nChanging instrument to Flute and to Violin and back to Piano.."));
	MusicNoteLib::PlayMusicStringWithOpts(_T("C D I[Flute] E F I[Violin] G A I[PIANO] B C6"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	//////////////////////////////////////////////
	_tprintf(_T("\nPlaying Carnatic Music.."));

	_tprintf(_T("\nPlaying Kalyani.."));
	MusicNoteLib::PlayMusicStringWithOpts(_T("K[MELA_65] S R G M P D N S'"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nPlaying Kharaharapriya.."));
	MusicNoteLib::PlayMusicStringWithOpts(_T("K[MELA_22] S R G M P D N S'"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nPlaying Kalyani in Douple speed.."));
	MusicNoteLib::PlayMusicStringWithOpts(_T("K[MELA_65]S[2] S R G M P D N S'"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nPlaying Kharaharapriya in Triple speed.."));
	MusicNoteLib::PlayMusicStringWithOpts(_T("K[MELA_22]S[3] S R G M P D N S'"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));



	_tprintf(_T("\n\n -------- C++ API Tests ------------\n"));
	/////////////////////////////////////////
	/// C++ API
	///
	_tprintf(_T("\nPlaying Middle Octave.."));
    MusicNoteLib::Player player(nPortID, nTimerRes); // Create the Player Object
    player.Play(_T("C D E F G A B")); // Play the Music Notes on the default MIDI output port

	_tprintf(_T("\nPlaying few random notes.."));
	// Asynchronous Play
	if(player.PlayAsync(_T("Cx Dh Ah."))) // Start Playing Asynchronously
    while(player.IsPlaying()) // Wait while the play is in progress
        SLEEP(1000);
	player.StopPlay(); // Match every PlayAsync with a StopPlay

	_tprintf(_T("\nSubscribing to Player parser events..."));
    // Subscribe to the Events
    player.Parser().evTrace.Subscribe(&OnParseTrace);
    player.Parser().evError.Subscribe(&OnParseError);

	_tprintf(_T("\nStarting to Play DDLJ Theme notes...\n"));
    // Play the Music Notes and then Save them to a MIDI File
    player.Play(_T("CI CI CI GI FI GI D#I. FI. G#I GI. RI ")
                _T("CI CI CI GI FI GI D#I. FI. D#I DI."));
    //if(false == player.SaveToMidiFile("PlayedOutput.mid"))
    //    _tprintf(_T("\n Unable to Save Played Music content to Midi Output File \n");
	_tprintf(_T("\n...Done !!...\n"));

	_tprintf(_T("\nStarting to Play Raghuvamsha Sudha notes...\n"));
    player.Play(_T("I[VIOLIN] ")
				_T("ci di f fi ei. di. ci ci cs b4s a4i g4i c ")
				_T("ci di f fi ei. di. ci ci cs b4s a4i g4i c ")
				_T("di fi a bi ei. gi. ")
				_T("ds fs ds fs ")
				_T("di fi a bi ei. gi. ")
				_T("c6 b ai c6s bs as gs fs es ds cs"));
	_tprintf(_T("\n...Done !!...\n"));

	// Uncomment the below to save notes as Midi file
	//_tprintf(_T("\nSaving to Midi file.."));
	//MusicNoteLib::SaveAsMidiFile(_T("C D E F G A B"), _T("Output.Mid"));
	//_tprintf(_T("\tDone !!"));

	return 0;
}

