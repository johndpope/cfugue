#ifndef __MUSICNOTEREADER_H__506A239B_50B0_472d_B070_69BFBEF3C6CB__
#define __MUSICNOTEREADER_H__506A239B_50B0_472d_B070_69BFBEF3C6CB__

#include "MString.h"
#include "Parser.h"
#include "Note.h"
#include <map>

namespace MusicNoteLib
{
	/// <Summary>
	///
	/// </Summary>
	class MusicStringParser : public CParser
	{
	protected:
		/// <Summary>
		/// Token constants used in the parsing for classifying their types
		/// </Summary>
		enum : TCHAR
		{
			TOKEN_START_VOICE			= _T('V'),
			TOKEN_START_TEMP			= _T('T'),
			TOKEN_START_INSTRUMENT		= _T('I'),
			TOKEN_START_LAYER			= _T('L'), 
			TOKEN_START_SIGNATURE		= _T('K'), 
			TOKEN_START_CONTROLLER		= _T('X'),
			TOKEN_START_TIME			= _T('@'),
			TOKEN_START_KEYPRESSURE		= _T('*'),
			TOKEN_START_CHANNELPRESSURE	= _T('+'), 
			TOKEN_START_PITCHBEND		= _T('&'),
			TOKEN_START_MEASURE			= _T('|'),
			TOKEN_START_DICTIONARY		= _T('$'),
			TOKEN_START_NOTE			= _T('['),

			// Dictionary Related
			ASSIGNMENT_SYMBOL			= _T('='),

			// Note Related
			REST_NOTE					= _T('R'),
			MACRO_START					= _T('['),
			MACRO_END					= _T(']'),
		};

		/// <Summary> Note Numbers </Summary>
		enum : unsigned short
		{
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
		};

		/// <Summary> Note Modifiers</Summary>
		enum : TCHAR
		{
			NOTE_MODIFIER_SHARP = _T('#'),
			NOTE_MODIFIER_FLAT	= _T('B'),
			NOTE_MODIFIER_NATURAL = _T('N')
		};

		/// <Summary> Note Durations </Summary>
		enum : TCHAR
		{
			NOTE_TIE					= _T('-'),	// Used when decorating durations for ties
			NOTE_DURATION_NUMERIC		= _T('/'),	// Used when Duration is to be expressed as a numeric value
			NOTE_DURATION_WHOLE			= _T('W'),
			NOTE_DURATION_HALF			= _T('H'),
			NOTE_DURATION_QUARTER		= _T('Q'),
			NOTE_DURATION_EIGTH			= _T('I'),
			NOTE_DURATION_SIXTEENTH		= _T('S'),
			NOTE_DURATION_THIRTYSECOND	= _T('T'),
			NOTE_DURATION_SIXTYFOURTH	= _T('X'),
			NOTE_DURATION_128			= _T('O'),
			NOTE_DURATION_DOT			= _T('.'),
			NOTE_TUPLET_START			= _T('*'),
			NOTE_TUPLET_RATIOMARK		= _T(':')
		};

		/// <Summary> Note Velocities </Summary>
		enum : TCHAR
		{
			NOTE_VELOCITY_ATTACK	= _T('A'),
			NOTE_VELOCITY_DECAY		= _T('D')
		};

		/// <Summary> Note Connectors </Summary>
		enum : TCHAR
		{
			NOTE_CONNECTOR_SEQUENTIAL	= _T('_'),
			NOTE_CONNECTOR_PARALLEL		= _T('+')
		};

		/// <Summary> Default Values </Summary>
		enum : long
		{
			DEFAULT_CHORD_OCTAVE		= 3,
			DEFAULT_NONCHORD_OCTAVE		= 5,
		};


		/// <Summary>Implements the less operator for Maps with string case-insensitive comparisions </Summary>
		template<class _Ty>
		struct StringLess : public std::binary_function<_Ty, _Ty, bool>
		{
			inline bool operator()(const _Ty& _Left, const _Ty& _Right) const
			{	
				return _tcsicmp(_Left, _Right) < 0 ? true : false; // check if (_Left < _Right);
			}
		};

		/// <Summary> Dictionary type maps a string key to a string value. The string
		/// value can be an integer or double, in text form, or a string </Summary>
		typedef std::map<MString, MString, StringLess<const TCHAR*> > DICTIONARY;

		DICTIONARY m_Dictionary;	// Holds the custom MACRO definitions for Music Strings

	public:
		//typedef void (*TOKEN_HANDLER_PROC)(const TCHAR* );

		//struct TokenClassifierDef
		//{
		//	MusicChar startChar;
		//	//TOKEN_HANDLER_PROC proc;
		//};

		//const TokenClassifierDef* m_pDef;

	public:
		MusicStringParser()//const TokenClassifierDef* pDef) : m_pDef(pDef)
		{
		}

		/// <Summary>
		/// Parses a single token. To Parse a string that contains multiple tokens, 
		/// use the <code>Parse</code> method. We consider a string to be having multiple
		/// tokens if there are whitespace charactes in it such as \n \t or Space.
		///
		/// @param szToken The token to be parsed. We expect this to be a single token 
		/// free of any whitespace characters, such as \t \n or Space. If the input token
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
		bool ParseToken(const TCHAR* szToken, bool* pbNonContinuableErrorOccured = NULL); 

		bool Parse(const TCHAR* szTokens, bool* pbNonContinuableErrorOccured = NULL); 

	private:
		// Token Parserer Methods. Return value indicates success or failure.
		bool ParseVoiceToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured) { return false; }
		bool ParseTempoToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured) { return false; }
		bool ParseInstrumentToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured) { return false; }
		bool ParseLayerToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured) { return false; } 
		bool ParseKeySignatureToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured) { return false; }
		bool ParseControllerToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured) { return false; }
		bool ParseTimeToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured) { return false; }
		bool ParseKeyPressureToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured) { return false; }
		bool ParseChannelPressureToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured) { return false; }
		bool ParsePitchBendToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured) { return false; }
		bool ParseMeasureToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured) { return false; }
		bool ParseDictionaryToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);
		bool ParseNoteToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured);

		struct NoteContext : public Note
		{
			bool isNumeric;
			bool isChord;
			//bool isFirstNote;
			//bool isSequentialNote;
			//bool isParallelNote;
			bool isNatural;
			bool existsAnotherNote;
			bool anotherIsSequential;
			bool anotherIsParallel;
			unsigned char	halfSteps[8];  
			unsigned short	numHalfSteps;
			short			octaveNumber;
			NoteContext()	:
				isNumeric(0),
				isChord(0),
				//isFirstNote(1),
				//isSequentialNote(0),
				//isParallelNote(0),
				isNatural(0),
				existsAnotherNote(1),
				anotherIsSequential(0),
				anotherIsParallel(0),
				numHalfSteps(0),
				octaveNumber(-1)
			{
				memset(halfSteps, 0, sizeof(halfSteps));
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
		/// <Summary> Checks & Parses any associated notes </Summary>
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