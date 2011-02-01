/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __MUSICNOTEREADER_H__506A239B_50B0_472d_B070_69BFBEF3C6CB__
#define __MUSICNOTEREADER_H__506A239B_50B0_472d_B070_69BFBEF3C6CB__

#include "Parser.h"
#include "Note.h"
#include "KeySignature.h"
#include "Chords.h"

namespace MusicNoteLib
{
	/// <Summary> Implements a MusicString Parsing functionality </Summary>
	class MusicStringParser : public CParser
	{
	protected:
		/// <Summary>
		/// Token constants used in the parsing for classifying their types
		/// </Summary>
		enum TokenSpecifiers : TCHAR
		{
			TOKEN_START_VOICE			= _T('V'),  ///< Specifies a Voice token 
			TOKEN_START_TEMPO			= _T('T'),  ///< Specifies a Tempo token 
			TOKEN_START_INSTRUMENT		= _T('I'),  ///< Specifies a Instrument token 
			TOKEN_START_LAYER			= _T('L'),  ///< Specifies a Layer token 
			TOKEN_START_KEYSIGNATURE	= _T('K'),  ///< Specifies a KeySignature token 
			TOKEN_START_CONTROLLER		= _T('X'),  ///< Specifies a Controller token 
			TOKEN_START_TIME			= _T('@'),  ///< Specifies a Time token
			TOKEN_START_KEYPRESSURE		= _T('*'),  ///< Specifies a Keypressure token
			TOKEN_START_CHANNELPRESSURE	= _T('+'),  ///< Specifies a Channel pressure token
			TOKEN_START_PITCHBEND		= _T('&'),  ///< Specifies a Pitchbend token
			TOKEN_START_MEASURE			= _T('|'),  ///< Specifies a Measure token
			TOKEN_START_DICTIONARY		= _T('$'),  ///< Specifies a Dictionary token
			TOKEN_START_NOTE			= _T('['),  ///< Specifies a Note token

            // Talam Related
            TOKEN_DOUBLESPEED_START     = _T('('),  ///< Specifies the start of 2x speed
            TOKEN_DOUBLESPEED_END       = _T(')'),  ///< Specifies the end of 2x speed

			// Dictionary Related
			ASSIGNMENT_SYMBOL			= _T('='),

			// Note Related
            MACRO_START					= _T('['),  ///< Signifies a Macro start symbol
            MACRO_END					= _T(']'),  ///< Signifies the Macro end symbol
            WESTERN_REST_NOTE	        = _T('R'),  ///< Specifies a Rest Note token [Western Music Only]
            REST_NOTE1					= _T(','),  ///< Specifies a Rest Note of one duration
            REST_NOTE2					= _T(';'),  ///< Specifies a Rest Note of two durations
        };

		/// <Summary> Note Numbers </Summary>
		enum NoteNumbers : unsigned short
		{
			// These are relative half-step values in an octave
			NOTE_C	= _T('C'),
			NOTE_C_Value	= 0,
			NOTE_D	= _T('D'),
			NOTE_D_Value	= 2,
			NOTE_E	= _T('E'),
			NOTE_E_Value	= 4,
			NOTE_F	= _T('F'),
			NOTE_F_Value	= 5,
			NOTE_G	= _T('G'),
			NOTE_G_Value	= 7,
			NOTE_A	= _T('A'),
			NOTE_A_Value	= 9,
			NOTE_B	= _T('B'),
			NOTE_B_Value	= 11,

			// These are array indices for SwaraSthanas[] based on KeySignature Mela
			SWARA_S	= _T('S'),
			SWARA_S_Value	= 0,
			SWARA_R	= _T('R'),
			SWARA_R_Value	= 1,
			SWARA_G	= _T('G'),
			SWARA_G_Value   = 2,
			SWARA_M	= _T('M'),
			SWARA_M_Value	= 3,
			SWARA_P	= _T('P'),
			SWARA_P_Value	= 4,
			SWARA_D	= _T('D'),
			SWARA_D_Value	= 5,
			SWARA_N	= _T('N'),
			SWARA_N_Value	= 6,
		};

		/// <Summary> Note Modifiers</Summary>
		enum NoteModifiers : TCHAR
		{
			NOTE_MODIFIER_SHARP = _T('#'),  ///< Note Sharp symbol
			NOTE_MODIFIER_FLAT	= _T('B'),  ///< Note Flat symbol
			NOTE_MODIFIER_NATURAL = _T('N') ///< Note Natural symbol
		};

		/// <Summary> Note Durations </Summary>
		enum NoteDurations : TCHAR
		{
			NOTE_TIE					= _T('-'),	///< Used when decorating durations for ties
			NOTE_DURATION_NUMERIC		= _T('/'),	///< Used when Duration is to be expressed as a numeric value
			NOTE_DURATION_WHOLE			= _T('W'),  ///< Specifies Whole Note 
			NOTE_DURATION_HALF			= _T('H'),  ///< Specifies Half Note
			NOTE_DURATION_QUARTER		= _T('Q'),  ///< Specifies Quarter Note
			NOTE_DURATION_EIGTH			= _T('I'),  ///< Specifies One-Eight Note
			NOTE_DURATION_SIXTEENTH		= _T('S'),  ///< Specifies One-Sixteenth Note
			NOTE_DURATION_THIRTYSECOND	= _T('T'),  ///< Specifies One-ThirtySecond Note
			NOTE_DURATION_SIXTYFOURTH	= _T('X'),  ///< Specifies One-SixtyFourth Note
			NOTE_DURATION_128			= _T('O'),  ///< Specifies 1/128th Note
			NOTE_DURATION_DOT			= _T('.'),  ///< Specifies 1.5 times the normal duration
			NOTE_TUPLET_START			= _T('*'),  ///< Indicates a Tuplet start
			NOTE_TUPLET_RATIOMARK		= _T(':'),  ///< Indicates the Tuplet Ratio
            SWARA_DURATION_ONE_EXTRA    = _T(','),  ///< Indicates one extra time unit [Carnatic Music Only]
            SWARA_DURATION_TWO_EXTRA    = _T(';'),  ///< Indicates two extra time units [Carnatic Music Only]
		};

		/// <Summary> Note Velocities </Summary>
		enum NoteVelocities : TCHAR
		{
			NOTE_VELOCITY       	= _T('V'),  ///< Note Attack Velocity specifier (Carnatic Mode)
			NOTE_VELOCITY_ATTACK	= _T('A'),  ///< Note Attack Velocity specifier (Obsolete. only for compatability with JFugue)
			NOTE_VELOCITY_DECAY		= _T('D')   ///< Note Decay Velocity specifier
		};

		/// <Summary> Note Connectors </Summary>
		enum NoteConnectors : TCHAR
		{
			NOTE_CONNECTOR_SEQUENTIAL	= _T('_'),  ///< Note Sequential connector symbol
			NOTE_CONNECTOR_PARALLEL		= _T('+')   ///< Note Parallel connector symbol
		};

		/// <Summary> Default Octave Values </Summary>
		enum DefaultOctaves : long
		{
			DEFAULT_CHORD_OCTAVE		= 3,    ///< Default chord octave value
			DEFAULT_NONCHORD_OCTAVE		= 5,    ///< Default non-chord note octave value
		};

        /// <Summary> Note Shruti Modifiers </Summary>
        enum NoteShrutis : TCHAR
        {
            NOTE_SHRUTI_UP      = _T('\''),  ///< Increments Note Shruti one level upward the Tara Sthayi
            NOTE_SHRUTI_DOWN    = _T('.'),   ///< Decrements Note Shruti one level downward the Mandra Sthayi
        };

        const Chords*	m_pChords;		// Holds the custom Chord Definitions, if any, supplied by user
		DICTIONARY		m_Dictionary;	// Holds the default && custom MACRO definitions for Music Strings
        KeySignature	m_KeySig;		// Holds the last seen Key Signature. Useful for computing Note value.

        unsigned short m_nDefNoteOctave;    // Holds the Default Octave Value to be used for Notes
        unsigned short m_nDefChordOctave;   // Holds the Default Octave Value to be used for Chords


		/// <Summary>
		/// Parses a single token. To Parse a string that contains multiple tokens, 
		/// use the <code>Parse</code> method. We consider a string to be having multiple
		/// tokens if there are whitespace charactes in it such as \\n \\t or Space.
		///
		/// @param szToken The token to be parsed. We expect this to be a single token 
		/// free of any whitespace characters, such as \\t \\n or Space. If the input token
		/// contains any of these characters, results are undefined.
		///
		/// @param pbNonContinuableErrorOccured Indicates the event handlers' response to
		/// the errors, if any. While calling ParseToken in a loop, when the return value
		/// is false, use this value to check if you should continue further or exit now.
		/// This usually will be the preference of errors handler procedures for the error.
		/// You can ignore this parameter if you are not doing any error handling or if the
		/// <code>ParseToken</code> is not called multiple times.
		///
		/// @return True if success, False in case of any failures.
		///
		/// Note that it is possible to have a return value of False, and *pbNonContinuableErrorOccured
		/// as True or False. This happens when an event handler responding to the error modifies the 
		/// error continuability. In any case, return value of false indicates there was an error, but
		/// if *pbNonContinuableErrorOccured is false, then an error event hanlder is asking you to ignore it.
		/// </Summary>
		bool ParseToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured = NULL); 

	public:
		//typedef void (*TOKEN_HANDLER_PROC)(const TCHAR* );

		//struct TokenClassifierDef
		//{
		//	MusicChar startChar;
		//	//TOKEN_HANDLER_PROC proc;
		//};

		//const TokenClassifierDef* m_pDef;

		MusicStringParser() 
		{ 
			ResetDefinitions();
		}

		/// <Summary>
        /// Macros defined in the Music Strings are <i>remembered</i> across the Parse() method calls.
        /// Call ResetDefinitions() before calling Parse() to clear the previous definitions.
		/// </Summary>
		void ResetDefinitions()
		{
			m_Dictionary.clear();
			PopulateStandardDefinitions(m_Dictionary); // Load standard macro definitions
            SetKeySignature(KeySignature());
            SetOctaveDefaults();
            m_pChords = NULL;
		}

        /// Sets the KeySignature to be used for further Note parsing
        /// @param keySig the KeySignature to be used
        inline void SetKeySignature(const KeySignature& keySig) { m_KeySig = keySig; }
        /// Returns the KeySignature being used
        /// @return the KeySignature being used
        inline const KeySignature& GetKeySignature() const { return m_KeySig; }
        /// Sets the Default values for Note Octaves
        inline void SetOctaveDefaults(unsigned short nNoteDefOctave = DEFAULT_NONCHORD_OCTAVE, 
                                      unsigned short nChordDefOctave = DEFAULT_CHORD_OCTAVE)
        {
            m_nDefNoteOctave = nNoteDefOctave;
            m_nDefChordOctave = nChordDefOctave;
        }

        /// <Summary>
        /// \brief Resets the Chord definitions to the supplied values.
		///
        /// A refernce to the supplied Chords object is stored by the Parser. 
		/// So, make sure the supplied Chords object stays valid as long as this
		/// Parser object is valid.
		///
		/// Since a reference is stored by the Parser, any modification you do for the 
		/// supplied Chords object will get refelected in the Parser too.
		///
        /// See Chords::LoadDefinitions() and Chords::AddDefinitions() for more details.
        /// </Summary>
        inline void LoadChords(const Chords& chords)
        {
            m_pChords = &chords;
        }

		/// <Summary>
		/// Parses a string that contains multiple tokens. Raises appropriate events as and when
		/// the tokens are parsed. We consider the input string to be having multiple
		/// tokens seperated with whitespace charactes such as \\n \\t or Space. If there are no
		/// whitespace characters, the whole string will be considered as a single token.
		///
		/// Use MusicStringParser::AddListener method to get notified about the token events.
		///
		/// @param szTokens The string to be parsed.
		///
		/// @return True if success, False in case of any failures.
		/// </Summary>
        /// Example Usage:
        /** <pre>
            \#include "MusicNoteLib.h"

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
        
            MusicNoteLib::MusicStringParser Parser; // Create the Parser Object
            
              Parser.evTrace.Subscribe(&OnParseTrace); // To get notified about the Parse Trace Events
              Parser.evError.Subscribe(&OnParseError); // To get notified about the Parse Error Events

            MIDIRenderer Renderer; // Create the Listener Object, in this case a MIDI Renderer
            Parser.AddListener(&Renderer);  // Add Renderer as a listener to get notified about the Tokens encountered during the Parse

            Parser.Parse(_T("I[Flute] C D E F G A B")); // Do the Parsing. Notifies the listeners during the process
         </pre> */
		bool Parse(const TCHAR* szTokens); 

	private:
		// Token Parserer Methods. Return value indicates success or failure.
		bool ParseChannelPressureToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParseControllerToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParseDictionaryToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParseInstrumentToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParseKeyPressureToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParseKeySignatureToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParseLayerToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParseMeasureToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParseNoteToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParsePitchBendToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
        bool ParseSpeedModulatorToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParseTempoToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParseTimeToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParseVoiceToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);

		struct NoteContext : public Note
		{
            typedef ChordDef::HALFSTEP HALFSTEP;
			bool isNumeric;
			bool isChord;
			bool isNatural;
			bool existsAnotherNote;
			bool anotherIsSequential;
			bool anotherIsParallel;
			short			octaveNumber;
            unsigned short  numSwaras; // [Carnatic] No.of Swaras the note spans, such as Sa, Pa etc...
            ChordDef        chord; // Holds the Chord details, if isChord is true
			NoteContext()	:
				isNumeric(0),
				isChord(0),
				isNatural(0),
				existsAnotherNote(1),
				anotherIsSequential(0),
				anotherIsParallel(0),
				octaveNumber(-1),
                numSwaras(1)
			{
			}
		};

		// Helper Methods
		/// <Summary>Tests if a Note is known to be Sequential (connected with _)
		/// or Parallel (connected with +) </Summary>
		void DecideSequentialOrParallel(NoteContext& ctx);

		/// <Summary> Identifies Numeric, Rest, Letter Note types and calls the correct method. 
		/// Returns the index to start parsing the next part of the string </Summary>
		int ParseNoteRoot(TCHAR* szToken, NoteContext& ctx);
		/// <Summary> Parses Numeric Note. 
		/// Returns the number of characaters consumed from the string during the parsing </Summary>
		int ParseNumericNote(TCHAR* szToken, NoteContext& ctx);
		/// <Summary> Parses Rest Note. 
		/// Returns the number of characaters consumed from the string during the parsing </Summary>
		int ParseRest(TCHAR* szToken, NoteContext& ctx);
		/// <Summary> Parses Letters Notes (such as C, D, E etc...). 
		/// Returns the number of characaters consumed from the string during the parsing </Summary>
		int ParseLetterNote(TCHAR* szToken, NoteContext& ctx);

		/// <Summary> Parses duration of note </Summary>
		int ParseNoteDuration(TCHAR* szToken, NoteContext& ctx);
		/// <Summary> Parses duration expressed as Alphabet W, H etc.. </Summary>
		int ParseLetterDuration(TCHAR* szToken, NoteContext& ctx);
		/// <Summary> Parses tuples expressed with * : </Summary>
		int ParseTuplet(TCHAR* szToken, NoteContext& ctx);

		int ComputeNoteValue(NoteContext& ctx);

		/// <Summary> Parses Attack and Decay velocity of notes </Summary>
		int ParseNoteVelocity(TCHAR* szToken, NoteContext& ctx);

		/// <Summary> Parses any associated note octave </Summary>
		int ParseNoteOctave(TCHAR* szToken, NoteContext& ctx);
		/// <Summary> Parses any chords specified for the note </Summary>
		int ParseNoteChord(TCHAR* szToken, NoteContext& ctx);
		/// <Summary> Parses any chord inversions specified for the note </Summary>
		int ParseNoteChordInversion(TCHAR* szToken, NoteContext& ctx);
		/// <Summary> Checks and Parses any associated notes </Summary>
		int ParseNoteConnector(TCHAR* szToken, NoteContext& ctx);

		/// <Summary> Upon completion of parsing a Note token, raises the Events to process the Note</Summary>
		void RaiseNoteEvents(NoteContext& ctx);

		/// <Summary> 
		/// Tries to load a value from Dictionary with the given key. 
		/// If the key is undefined, then itself is converted to the required type
		/// and returned as the value.
		///
        /// @return 0 in case of failures of lookup or conversion. Length of the string
        /// scanned, in case of szKey being a number. To be ignored, if szKey is a macro.
        /// </Summary>
		template<typename T>
		int GetValueFromDictionary(const TCHAR* szKey, T* pRetVal);

		/// <Summary> 
		/// Tries to read a number from the given token. The number might been
		/// specified as a macro between the supplied MacroBracketStart and 
		/// MacroBracketEnd characters or as a direct number value.
		///
        /// @return -1 in case of some irrovacable failures. Length of the string
		/// scanned, otherwise. Check bSuccess if number was read successfully (despite errors).
        /// </Summary>
		template<typename T>
		int ParseNumber(TCHAR* szToken, T* pRetVal, bool& bSuccess,
			TCHAR MacroBracketStart, TCHAR MacroBracketEnd, ErrorCode MacroReadErrorCode, ErrorCode NumberParseErrorCode);

	};

} // namespace MusicNoteLib

#endif // __MUSICNOTEREADER_H__506A239B_50B0_472d_B070_69BFBEF3C6CB__