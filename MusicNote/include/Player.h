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

		inline Player(void)
		{
			m_Parser.AddListener(&m_Renderer);
		}

		inline ~Player(void)
		{
		}

		inline MusicStringParser& Parser() { return m_Parser; }

		/// <Summary>
		/// Plays a string of music notes. Will not return till the play is complete.
		/// To Play the Notes asynchronously, use the <code>PlayAsync</code> method instead.
		/// </Summary>
		inline void Play(const MString& strMusicNotes, int nMIDIOutPortID = MIDI_MAPPER, unsigned int nMIDITimerResMS = 20)
		{
			PlayAsync(strMusicNotes, nMIDIOutPortID, nMIDITimerResMS);
			
			while(IsPlaying())
				Sleep(1000);//TODO: Ensure Platform compatibility for Sleep

			StopPlay();
		}

		///<Summary>
		/// Starts playing the notes asynchronously. 
		/// Use <code>IsPlaying</code> to determine if the play is still in progress.
		/// Use the StopPlay method to stop the play. 
		/// Each PlayAsync should have a matching StopPlay method call.
		///</Summary>
		inline void PlayAsync(const MString& strMusicNotes, int nMIDIOutPortID = MIDI_MAPPER, unsigned int nMIDITimerResMS = 20 )
		{
			m_Renderer.Clear(); // Clear any previous Notes

			m_Parser.Parse(strMusicNotes);	// Parse and Load the Notes into MIDI MultiTrack

			m_Renderer.BeginPlayAsync(nMIDIOutPortID, nMIDITimerResMS); // Start Playing on the given MIDIport with supplied resolution
		}

		/// <Summary>
		/// Stops the Play started with PlayAsync.
		/// Each PlayAsync should have a matching StopPlay method call.
		/// </Summary>
		inline void StopPlay()
		{
			m_Renderer.EndPlayAsync();
		}

		/// <Summary>Returns true if an asynchronous Play is currently in progress</Summary>
		inline bool IsPlaying() const { return m_Renderer.IsPlaying(); }

		/// <Summary>
		/// Saves the Music Notes into a Midi output file.
		/// If there are any parsing errors for the Music string, result is undefined.
		/// To get notified about the parsing events, retrieve the associated parser 
		/// object using the <code>Parser()</code> method and subscribe to the <code>evTrace</code>
		/// and <code>evError</code> events.
		/** <pre>
			MusicNoteLib::Player player;

			// Subscribe to the Events
			player.Parser().evTrace.Subscribe(&OnParseTrace);
			player.Parser().evError.Subscribe(&OnParseError);

			// Save the Music Notes to Midi file directly, without playing
			if(false == player.SaveAsMidiFile(_T("Cq Dw Eq Fw Gq Aw Bw"), "MidiOutput.midi"))
				printf("\n Unable to Save Music String to Midi Output File \n");
		</pre> */
		/// @param strMusicNotes the input Music string to be converted to MIDI output
		/// @param szOutputFilePath the output MIDI file path
		/// </Summary>
		inline bool SaveAsMidiFile(const MString& strMusicNotes, const char* szOutputFilePath)
		{
			m_Renderer.Clear(); // Clear any previous Notes

			m_Parser.Parse(strMusicNotes);	// Parse the Notes and Load the Midi Events into MIDI MultiTrack

			return m_Renderer.SaveToFile(szOutputFilePath);
		}

		/// <Summary>
		/// Saves any previously played Music Notes into a Midi output file.
		/// If <code>Play</code> or <code>PlayAsync</code> is not called on this object previously,
		/// the output MIDI file will be empty.
		/// To save the notes directly to MIDI file without playing, use <code>SaveAsMidiFile</code>
		/// method instead.
		/** <pre>
			MusicNoteLib::Player player;

			// Subscribe to the Events
			player.Parser().evTrace.Subscribe(&OnParseTrace);
			player.Parser().evError.Subscribe(&OnParseError);

			// Play the Music Notes and then Save them to a MIDI File
			player.Play(_T("ci di f fi ei. di. ci ci cs b4s a4i g4i c "));
			if(false == player.SaveToMidiFile("PlayedOutput.mid"))
			    printf("\n Unable to Save Played Music content to Midi Output File \n");
		</pre> */
		/// @param szOutputFilePath the output MIDI file path
		/// </Summary>
		inline bool SaveToMidiFile(const char* szOutputFilePath)
		{
			return m_Renderer.SaveToFile(szOutputFilePath);
		}
	};

} // namespace MusicNoteLib

#endif // __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__