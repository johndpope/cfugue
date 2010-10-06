/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009-2010 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#include "stdafx.h"
#include "Player.h"

namespace MusicNoteLib
{
	Player::Player(void)
	{
		m_Parser.AddListener(&m_Renderer);
	}

    bool Player::Play(const MString& strMusicNotes, int nMIDIOutPortID /*= MIDI_MAPPER*/, unsigned int nMIDITimerResMS /*= 20*/)
    {
        bool bRetVal = PlayAsync(strMusicNotes, nMIDIOutPortID, nMIDITimerResMS);

        while(IsPlaying())
            Sleep(1000);//TODO: Ensure Platform compatibility for Sleep
            
        StopPlay();

        return bRetVal;
    }


    bool Player::PlayAsync(const MString& strMusicNotes, int nMIDIOutPortID /*= MIDI_MAPPER*/, unsigned int nMIDITimerResMS /*= 20*/ )
    {
        m_Renderer.Clear(); // Clear any previous Notes

        if(false == m_Parser.Parse(strMusicNotes))	// Parse and Load the Notes into MIDI MultiTrack
            return false;

        return m_Renderer.BeginPlayAsync(nMIDIOutPortID, nMIDITimerResMS); // Start Playing on the given MIDIport with supplied resolution
    }

	void Player::StopPlay()
	{
		m_Renderer.EndPlayAsync();
	}

	bool Player::SaveAsMidiFile(const MString& strMusicNotes, const char* szOutputFilePath)
	{
		m_Renderer.Clear(); // Clear any previous Notes

		m_Parser.Parse(strMusicNotes);	// Parse the Notes and Load the Midi Events into MIDI MultiTrack

		return m_Renderer.SaveToFile(szOutputFilePath);
	}

	bool Player::SaveToMidiFile(const char* szOutputFilePath)
	{
		return m_Renderer.SaveToFile(szOutputFilePath);
	}

} // namespace MusicNoteLib
