/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__
#define __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__

#include "MusicStringParser.h"
#include "MIDIRenderer.h"

namespace MusicNoteLib
{
    /// <Summary> MIDI Player for Music Strings </Summary>
	class Player
	{
		MIDIRenderer		m_Renderer;
		MusicStringParser	m_Parser;
		unsigned int		m_nOutPort;		// The MIDI Output port that should be used for Play
		unsigned int		m_nTimerRes;	// The Timer Resolution in MilliSeconds
	public:

		/// Construct the Player Object using supplied Midi Output port and Timer Resolution
		/// @param nMIDIOutPortID the output MIDI port to be used for the play
		/// @param nMIDITimerResMS timer resolution in Milliseconds
		Player(unsigned int nMIDIOutPortID = MIDI_MAPPER, unsigned int nMIDITimerResMS = 20);

		inline ~Player(void)
		{
		}

        /// Returns the associated Parser object
		inline MusicStringParser& Parser() { return m_Parser; }

		/// Get/Set the Midi Output Port that should be used with this Player
		inline unsigned int MidiOutPort() { return m_nOutPort; }

		/// Get/Set the Timer Resolution (in MilliSeconds) that should be used with this Player
		inline unsigned int TimerResolution() { return m_nTimerRes; }

        /// <Summary>
        /// Plays a string of music notes. Will not return till the play is complete.
        /// To Play the Notes asynchronously, use the PlayAsync() method instead.
        ///
        /// Returns false if unable to start the play. Failures can happen if unable to open
        /// the MIDI output port or if unable to create a MIDI timer with supplied resolution or
        /// if there are any critical parsing errors in the supplied Music String.
        ///
        /// @param strMusicNotes the input Music string to be played on MIDI output port
        /// @return True if play was successful, false otherwise
        /// </Summary>
        /// Example Usage:
        /** <pre>
            MusicNoteLib::Player player; // Create the Player Object with the default MIDI output port

            player.Play(_T("ci di f fi")); // Play the Music Notes
        </pre> */
        bool Play(const MString& strMusicNotes);

        /// <Summary>
        /// Starts playing the notes asynchronously. Returns false if unable to start the Play.
        /// Play failures can happen if unable to open the MIDI output port or if unable to
        /// create a MIDI timer with supplied resolution or if there any critical errors in
        /// the Music String parsing.
        ///
        /// After Play starts, use IsPlaying() method to determine if play is still in progress.
        /// Use the StopPlay() method to stop the play.
        ///
		/// You can also use WaitTillDone() to wait till the play completes. Refer \ref WaitTillDone.
		///
        /// Note that each PlayAsync() should have a matching StopPlay() method call to release MIDI resources.
        ///
		/// @param strMusicNotes the input Music string to be played on MIDI output port
        /// @return True if play started successfully, false otherwise
        /// </Summary>
        /// Example Usage:
        /** <pre>
            MusicNoteLib::Player player(nMIDIOutPortID, nMIDITimerResMS); // Create the Player object

            if(player.PlayAsync(strMusicNotes) // Start Playing Asynchronously
                while(player.IsPlaying()) // Wait while the play is still in progress
                    Sleep(1000);

            player.StopPlay(); // Stop the Play and release MIDI resources
         </pre> */
        bool PlayAsync(const MString& strMusicNotes);

		/// <Summary>
		/// After play starts asynchronously with PlayAsync(), use WaitTillDone() to wait
		/// till the play completes. Caller gets blocked. Once WaitTillDone() returns call
		/// StopPlay() to release MIDI resources.
		/// </Summary>
        /// Example Usage:
        /** <pre>
            MusicNoteLib::Player player(nMIDIOutPortID, nMIDITimerResMS); // Create the Player object

            if(player.PlayAsync(strMusicNotes) // Start Playing Asynchronously
				player.WaitTillDone(); // wait while the play is in progress

            player.StopPlay(); // Stop the Play and release MIDI resources
         </pre> */
		void WaitTillDone();

        /// <Summary>
        /// Stops the Play started with PlayAsync().
        /// Each PlayAsync() should have a matching StopPlay() method call.
        /// </Summary>
		void StopPlay();

		/// <Summary>Returns true if an asynchronous Play is currently in progress</Summary>
		inline bool IsPlaying() const { return m_Renderer.IsPlaying(); }

		/// <Summary>
		/// Parses the Music String and saves the generated MIDI events to a Midi output file.
        ///
		/// Returns true upon success. If there are any parsing errors for the Music string, result is undefined.
        ///
		/// To get notified about the parsing events, retrieve the associated parser
        /// object using the Player::Parser() method and subscribe to the <code>evTrace</code>
        /// and <code>evError</code> events.
        ///
        /// @param strMusicNotes the input Music string to be converted to MIDI output
		/// @param szOutputFilePath the output MIDI file path
        /// @return True if content is saved successfully, False otherwise
		/// </Summary>
        /// Example Usage:
        /** <pre>
            void OnParseTrace(const MusicNoteLib::CParser*, MusicNoteLib::CParser::TraceEventHandlerArgs* pEvArgs)
            {
                OutputDebugString(_T("\n"));
                OutputDebugString(pEvArgs->szTraceMsg);
            }

            void OnParseError(const MusicNoteLib::CParser*, MusicNoteLib::CParser::ErrorEventHandlerArgs* pEvArgs)
            {
                OutputDebugString(_T("\nError --> "));
                OutputDebugString(pEvArgs->szErrMsg);
                if(pEvArgs->szToken)
                {
                    OutputDebugString(_T("\t Token: "));
                    OutputDebugString(pEvArgs->szToken);
                }
            }

            MusicNoteLib::Player player; // Create the Player Object

            player.Parser().evTrace.Subscribe(&OnParseTrace); // Subscribe to the Trace Events
            player.Parser().evError.Subscribe(&OnParseError); // Subscribe to the Error Events

            player.SaveAsMidiFile(_T("Cq Dw Ex"), "MidiOutput.midi"); // Save the Music Notes to Midi file directly, without playing

        </pre> */
		bool SaveAsMidiFile(const MString& strMusicNotes, const char* szOutputFilePath);

		/// <Summary>
		/// Saves any previously played Music Notes into a Midi output file.
        /// If Play() or PlayAsync() is not called on this object previously,
		/// the output MIDI file will be empty.
        ///
        /// To save the notes directly to MIDI file without playing, use Player::SaveAsMidiFile
		/// method instead.
        ///
        /// @param szOutputFilePath the output MIDI file path
        /// @return True if content is saved successfully, False otherwise
		/// </Summary>
        /// Example Usage:
        /** <pre>
            MusicNoteLib::Player player; // Create the Player Object

            player.Play(_T("ci di f fi")); // Play the Music Notes on MIDI output port

            player.SaveToMidiFile("Output.mid"); // Save the played content to MIDI Output file
        </pre> */
		bool SaveToMidiFile(const char* szOutputFilePath);
	};

} // namespace MusicNoteLib

#endif // __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__
