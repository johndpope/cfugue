/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009-2010 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__
#define __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__

#include "MString.h"
#include "MusicStringParser.h"
#include "MIDIRenderer.h"

namespace MusicNoteLib
{
    /// <Summary> MIDI Player for Music Strings </Summary>
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

        /// Returns the associated Parser object
		inline MusicStringParser& Parser() { return m_Parser; }

        /// <Summary>
        /// Plays a string of music notes. Will not return till the play is complete.
        /// To Play the Notes asynchronously, use the PlayAsync() method instead.
        ///
        /// Returns false if unable to start the play. Failures can happen if unable to open
        /// the MIDI output port or if unable to create a MIDI timer with supplied resolution or
        /// if there are any critical parsing errors in the supplied Music String.
        ///
        /// @param strMusicNotes the input Music string to be played on MIDI output port
		/// @param nMIDIOutPortID the output MIDI port to be used for the play
		/// @param nMIDITimerResMS timer resolution in Milliseconds
        /// @return True if play was successful, false otherwise
        /// </Summary>
        /// Example Usage:
        /** <pre>
            MusicNoteLib::Player player; // Create the Player Object
            
            player.Play(_T("ci di f fi")); // Play the Music Notes on the default MIDI output port
        </pre> */
        inline bool Play(const MString& strMusicNotes, int nMIDIOutPortID = MIDI_MAPPER, unsigned int nMIDITimerResMS = 20)
        {
            bool bRetVal = PlayAsync(strMusicNotes, nMIDIOutPortID, nMIDITimerResMS);

            while(IsPlaying())
                Sleep(1000);//TODO: Ensure Platform compatibility for Sleep
            
            StopPlay();

            return bRetVal;
        }

        /// <Summary>
        /// Starts playing the notes asynchronously. Returns false if unable to start the Play. 
        /// Play failures can happen if unable to open the MIDI output port or if unable to 
        /// create a MIDI timer with supplied resolution or if there any critical errors in 
        /// the Music String parsing.
        ///
        /// After Play starts, use IsPlaying() method to determine if play is still in progress.
        /// Use the StopPlay() method to stop the play. 
        ///
        /// Each PlayAsync() should have a matching StopPlay() method call.
        ///
        /// @param strMusicNotes the input Music string to be played on MIDI output port
		/// @param nMIDIOutPortID the output MIDI port to be used for the play
		/// @param nMIDITimerResMS timer resolution in Milliseconds
        /// @return True if play started successfully, false otherwise
        /// </Summary>
        /// Example Usage:
        /** <pre>
            MusicNoteLib::Player player; // Create the Player object

            if(player.PlayAsync(strMusicNotes, nMIDIOutPortID, nMIDITimerResMS)) // Start Playing Asynchronously 
                while(player.IsPlaying()) // Wait while the play is still in progress
                    Sleep(1000);

            player.StopPlay(); // Stop the Play when done
         </pre> */
        inline bool PlayAsync(const MString& strMusicNotes, int nMIDIOutPortID = MIDI_MAPPER, unsigned int nMIDITimerResMS = 20 )
        {
            m_Renderer.Clear(); // Clear any previous Notes

            if(false == m_Parser.Parse(strMusicNotes))	// Parse and Load the Notes into MIDI MultiTrack
                return false;

            return m_Renderer.BeginPlayAsync(nMIDIOutPortID, nMIDITimerResMS); // Start Playing on the given MIDIport with supplied resolution
        }

        /// <Summary>
        /// Stops the Play started with PlayAsync().
        /// Each PlayAsync() should have a matching StopPlay() method call.
        /// </Summary>
		inline void StopPlay()
		{
			m_Renderer.EndPlayAsync();
		}

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
		inline bool SaveAsMidiFile(const MString& strMusicNotes, const char* szOutputFilePath)
		{
			m_Renderer.Clear(); // Clear any previous Notes

			m_Parser.Parse(strMusicNotes);	// Parse the Notes and Load the Midi Events into MIDI MultiTrack

			return m_Renderer.SaveToFile(szOutputFilePath);
		}

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
		inline bool SaveToMidiFile(const char* szOutputFilePath)
		{
			return m_Renderer.SaveToFile(szOutputFilePath);
		}
	};

} // namespace MusicNoteLib

#endif // __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__