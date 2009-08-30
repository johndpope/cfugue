#ifndef __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__
#define __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__

#include "MString.h"
#include "MusicStringParser.h"
#include "MIDIRenderer.h"

namespace MusicNoteLib
{
	class Player
	{			
		MIDIRenderer		m_Renderer;
		MusicStringParser	m_Parser;
	public:

		Player(void)
		{
			m_Parser.AddListener(&m_Renderer);
		}

		~Player(void)
		{
		}

		inline MusicStringParser& Parser() { return m_Parser; }

		/// <Summary>
		/// Plays a string of music notes. Will not return till the play is complete.
		/// To Play the Notes asynchronously, use the PlayAsync method instead.
		/// </Summary>
		void Play(const MString& strMusicNotes)
		{
			PlayAsync(strMusicNotes);
			
			while(m_Renderer.IsPlaying())
				Sleep(1000);//TODO: Ensure Platform compatibility for Sleep

			StopPlay();
		}

		/// <Summary>Starts playing the notes asynchronously. Use the StopPlay method to stop the play. </Summary>
		inline void PlayAsync(const MString& strMusicNotes)
		{
			m_Renderer.Clear(); // Clear any previous Notes

			m_Parser.Parse(strMusicNotes);	// Parse and Load the Notes into MIDI MultiTrack

			m_Renderer.BeginPlayAsync(); // Start Playing
		}

		/// <Summary>Stops the Play</Summary>
		inline void StopPlay()
		{
			m_Renderer.EndPlayAsync();
		}

		/// <Summary>Saves the Music Notes into a Midi output file</Summary>
		inline bool SaveAsMidiFile(const MString& strMusicNotes, const char* szOutputFilePath)
		{
			m_Renderer.Clear(); // Clear any previous Notes

			m_Parser.Parse(strMusicNotes);	// Parse the Notes and Load the Midi Events into MIDI MultiTrack

			return m_Renderer.SaveToFile(szOutputFilePath);
		}

		/// <Summary>
		/// Saves any previously played Music Notes into a Midi output file
		/// </Summary>
		inline bool SaveToMidiFile(const char* szOutputFilePath)
		{
			return m_Renderer.SaveToFile(szOutputFilePath);
		}
	};

} // namespace MusicNoteLib

#endif // __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__