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
			printf("\n---------  The MIDI Out Port indices  --------\n\n");
			/////////////////////////////////////////
			/// List the MIDI Ports
			for(unsigned int i=0; i < nOutPortCount; ++i)
				printf("\t%d\t:  %s\n", i, MusicNoteLib::GetMidiOutPortName(i).c_str());
			//////////////////////////////////////////
			/// Chose a Midi output port
			printf("\nChoose your MIDI Port ID for the play:");
			scanf("%d", &nPortID);
		}
		else
		{
			printf("\nUsing %s as the MIDI output port\n", MusicNoteLib::GetMidiOutPortName(0).c_str());
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

	// Uncomment the below to save notes as Midi file
	//_tprintf(_T("\nSaving to Midi file.."));
	//MusicNoteLib::SaveAsMidiFile(_T("C D E F G A B"), _T("Output.Mid"));
	//_tprintf(_T("\tDone !!"));

	return 0;
}

