/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://cfugue.sourceforge.net/>.
*/

#include "stdafx.h"
#include "Player.h"

#if defined WIN32 || defined _WIN32
#define SLEEP(x)    Sleep(x)
#elif defined __GNUC__
#include <thread>
#include <chrono>
#define SLEEP(x)    std::this_thread::sleep_for(std::chrono::milliseconds(x))
#endif

namespace CFugue
{
	Player::Player(unsigned int nMIDIOutPortID /*= MIDI_MAPPER*/, unsigned int nMIDITimerResMS /*= 20*/)
		: m_nOutPort(nMIDIOutPortID), m_nTimerRes(nMIDITimerResMS)
	{
		m_Parser.AddListener(&m_Renderer);
	}

    bool Player::Play(const MString& strMusicNotes)
    {
        bool bRetVal = PlayAsync(strMusicNotes);

		m_Renderer.WaitTillDone();

        StopPlay();

        return bRetVal;
    }


    bool Player::PlayAsync(const MString& strMusicNotes)
    {
        m_Renderer.Clear(); // Clear any previous Notes

        if(false == m_Parser.Parse(strMusicNotes))	// Parse and Load the Notes into MIDI MultiTrack
            return false;

        return m_Renderer.BeginPlayAsync(m_nOutPort, m_nTimerRes); // Start Playing on the given MIDIport with supplied resolution
    }

	void Player::StopPlay()
	{
		m_Renderer.EndPlayAsync();
	}

	void Player::WaitTillDone()
	{
		m_Renderer.WaitTillDone();
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

} // namespace CFugue
