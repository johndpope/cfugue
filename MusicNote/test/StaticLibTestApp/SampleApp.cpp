/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009-2010 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

// SampleApp.cpp : Defines the entry point for the console application.
//
// CFugue is under active development. 
// Please refer to documentation and latest releases at: http://musicnote.sourceforge.net/

#include "stdafx.h"
#include "MusicNoteLib.h"

int _tmain(int argc, _TCHAR* argv[])
{
	_tprintf(_T("\nPlaying Notes.."));
	MusicNoteLib::PlayMusicString(_T("C D E F G A B"));
	_tprintf(_T("\nDone!!"));

	_tprintf(_T("\nPlaying Chrods.."));
	MusicNoteLib::PlayMusicString(_T("CMajH DMaj EMajH"));
	_tprintf(_T("\nDone!!"));

	_tprintf(_T("\nPlaying Parallel Notes.."));
	MusicNoteLib::PlayMusicString(_T("C+D+E+F+G+A+B"));
	_tprintf(_T("\nDone!!"));

	_tprintf(_T("\nPlaying Sequential Notes.."));
	MusicNoteLib::PlayMusicString(_T("B_A_G_F_E_D_C"));
	_tprintf(_T("\nDone!!"));

	_tprintf(_T("\nChanging instrument to Flute and to Violin and back to Piano.."));
	MusicNoteLib::PlayMusicString(_T("C D I[Flute] E F I[Violin] G A I[PIANO] B C6"));
	_tprintf(_T("\nDone!!"));

	//////////////////////////////////////////////
	_tprintf(_T("\nPlaying Carnatic Music.."));

	_tprintf(_T("\nPlaying Kalyani.."));
	MusicNoteLib::PlayMusicString(_T("K[MELA_65] S R G M P D N S'"));
	_tprintf(_T("\nDone!!"));

	_tprintf(_T("\nPlaying Kharaharapriya.."));
	MusicNoteLib::PlayMusicString(_T("K[MELA_22] S R G M P D N S'"));
	_tprintf(_T("\nDone!!"));

	_tprintf(_T("\nPlaying Kalyani in Douple speed.."));
	MusicNoteLib::PlayMusicString(_T("K[MELA_65]S[2] S R G M P D N S'"));
	_tprintf(_T("\nDone!!"));

	_tprintf(_T("\nPlaying Kharaharapriya in Triple speed.."));
	MusicNoteLib::PlayMusicString(_T("K[MELA_22]S[3] S R G M P D N S'"));
	_tprintf(_T("\nDone!!"));

	// Uncomment the below to save notes as Midi file
	//_tprintf(_T("\nSaving to Midi file.."));
	//MusicNoteLib::SaveAsMidiFile(_T("C D E F G A B"), _T("Output.Mid"));
	//_tprintf(_T("\nDone !!"));

	return 0;
}

