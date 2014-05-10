/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://cfugue.sourceforge.net/>.
*/

// SampleApp.cpp
//
// Demonstrates the usage of CFugue as a Statically Linked Library
//
// CFugue is under active development.
// Please refer to documentation and latest releases at: http://cfugue.sourceforge.net/

#include "stdafx.h"
#include "stdlib.h"
#include <iostream>

#include "CFugueLib.h"

void OnParseTrace(const CFugue::CParser*, CFugue::CParser::TraceEventHandlerArgs* pEvArgs)
{
     std::wcout << "\n\t" << pEvArgs->szTraceMsg;
}

void OnParseError(const CFugue::CParser*, CFugue::CParser::ErrorEventHandlerArgs* pEvArgs)
{
	std::wcerr << "\n\t Error --> " << pEvArgs->szErrMsg;
	if(pEvArgs->szToken)
	{
		std::wcerr << "\t Token: " << pEvArgs->szToken;
	}
}

int main(int argc, char* argv[])
{
	int nPortID=MIDI_MAPPER, nTimerRes = 20;

	if(argc < 2)
	{
		unsigned int nOutPortCount = CFugue::GetMidiOutPortCount();
		if(nOutPortCount <= 0)
			exit(fprintf(stderr, "No MIDI Output Ports found !!"));
		if(nOutPortCount > 1)
		{
			_tprintf(_T("\n---------  The MIDI Out Port indices  --------\n\n"));
			/////////////////////////////////////////
			/// List the MIDI Ports
			for(unsigned int i=0; i < nOutPortCount; ++i)
            {
                std::string portName = CFugue::GetMidiOutPortName(i);
				std::wcout << "\t" << i << "\t: " << portName.c_str() << "\n";
            }
			//////////////////////////////////////////
			/// Chose a Midi output port
			_tprintf(_T("\nChoose your MIDI Port ID for the play:"));
			_tscanf(_T("%d"), &nPortID);
		}
		else
		{
            std::string portName = CFugue::GetMidiOutPortName(0);
			std::wcout << "\nUsing the MIDI output port: " << portName.c_str();
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
	CFugue::PlayMusicStringWithOpts(_T("C D E F G A B"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nPlaying Chrods.."));
	CFugue::PlayMusicStringWithOpts(_T("CMajH DMaj EMajH"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nPlaying Parallel Notes.."));
	CFugue::PlayMusicStringWithOpts(_T("C+D+E+F+G+A+B"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nPlaying Sequential Notes.."));
	CFugue::PlayMusicStringWithOpts(_T("B_A_G_F_E_D_C"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nChanging instrument to Flute and to Violin and back to Piano.."));
	CFugue::PlayMusicStringWithOpts(_T("C D I[Flute] E F I[Violin] G A I[PIANO] B C6"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	//////////////////////////////////////////////
	_tprintf(_T("\nPlaying Carnatic Music.."));

	_tprintf(_T("\nPlaying Kalyani.."));
	CFugue::PlayMusicStringWithOpts(_T("K[MELA_65] S R G M P D N S'"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nPlaying Kharaharapriya.."));
	CFugue::PlayMusicStringWithOpts(_T("K[MELA_22] S R G M P D N S'"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nPlaying Kalyani in Douple speed.."));
	CFugue::PlayMusicStringWithOpts(_T("K[MELA_65]S[2] S R G M P D N S'"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));

	_tprintf(_T("\nPlaying Kharaharapriya in Triple speed.."));
	CFugue::PlayMusicStringWithOpts(_T("K[MELA_22]S[3] S R G M P D N S'"), nPortID, nTimerRes);
	_tprintf(_T("    Done"));



	_tprintf(_T("\n\n -------- C++ API Tests ------------\n"));
	/////////////////////////////////////////
	/// C++ API
	///
	_tprintf(_T("\nPlaying Middle Octave.."));
    CFugue::Player player(nPortID, nTimerRes); // Create the Player Object
    player.Play(_T("C D E F G A B")); // Play the Music Notes on the default MIDI output port

	_tprintf(_T("\nPlaying few random notes.."));
	// Asynchronous Play
	if(player.PlayAsync(_T("Cx Dh Ah."))) // Start Playing Asynchronously
    while(player.IsPlaying()) // Wait while the play is in progress
        CFugue::MidiTimer::Sleep(1000);
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
	//CFugue::SaveAsMidiFile(_T("C D E F G A B"), "output.mid");
	//_tprintf(_T("\tDone !!"));

	return 0;
}

