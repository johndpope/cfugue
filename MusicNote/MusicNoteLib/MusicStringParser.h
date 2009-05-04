#ifndef __MUSICNOTEREADER_H__506A239B_50B0_472d_B070_69BFBEF3C6CB__
#define __MUSICNOTEREADER_H__506A239B_50B0_472d_B070_69BFBEF3C6CB__

/// <Summary>
/// You can enable or disable tracing events in the MusicStringParser by
/// defining ENABLE_TRACING to be 1 or 0 before including this header file.
/// By default, ENABLE_TRACING is defined to be 1 in DEBUG builds, and 0 in
/// RELEASE builds.
/// </Summary>
#if defined(DEBUG) || defined (_DEBUG)
#define ENABLE_TRACING 1
#endif // if defined(DEBUG)

#include "MusicNoteLib.h"
#include "MString.h"
#include "EventHandler.h"
#include <map>

namespace MusicNoteLib
{
	/// <Summary>
	///
	/// </Summary>
	class MusicStringParser : public OIL::CEventSource
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
			DEFAULT_ATTACK_VELOCITY		= 64,
			DEFAULT_DECAY_VELOCITY		= 64,
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

		struct TokenClassifierDef
		{
			MusicChar startChar;
			//TOKEN_HANDLER_PROC proc;
		};

		const TokenClassifierDef* m_pDef;

#if ENABLE_TRACING
	public:
		/// <Summary>
		/// The structure that is passed as argument to the Trace Event Handler procedure.
		/// Listeners who want to know about the trace events can subscribe to the evTrace
		/// event and handle the trace events raised by MusicStringParser as the parsing progresses.
		/// </Summary>
		struct TraceEventHandlerArgs : public OIL::CEventHandlerArgs 
		{  
			const TCHAR* szTraceMsg;
			inline TraceEventHandlerArgs(const TCHAR* sz) : szTraceMsg(sz) { }
		};

		OIL::CEventT<const MusicStringParser, TraceEventHandlerArgs> evTrace;
	protected:
		/// <Summary>
		/// Trace method is called through out parsing process. However, the functionality
		/// is enabled only if ENABLE_TRACING directive is defined to be 1 (which is default for
		/// DEBUG builds, but not for RELEASE builds).
		/// When enabled, Trace will raise events through the MusicStringParser::evTrace event
		/// variable. Listeners can subscribe to this event to get to know the trace messages.
		/// </Summary>
		inline virtual void Trace(const TCHAR* szTraceMsg)
		{
			OIL::CEventSource::RaiseEvent(&evTrace, &TraceEventHandlerArgs(szTraceMsg));
		}
#else

#define Trace(x) 

#endif // #if ENABLE_TRACING

	public:
		MusicStringParser(const TokenClassifierDef* pDef) : m_pDef(pDef)
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

		/// <Summary>Error code values used by ErrorEventHandlerArgs</Summary>
		enum ErrorCode
		{
			CRITICAL_ERROR_MEMORY_ALLOCATION,	// Memory allocation failed
			PARSE_ERROR_MISSING_ASSIGNMENT,		// No Assignment symbol found
			PARSE_ERROR_NUMERIC_NOTE_END,		// MACRO_END missing while parsing a numeric note.			
			PARSE_ERROR_NUMERIC_NOTE_VALUE,		// Failure while converting/retrieving a numeric note number.
			PARSE_ERROR_LETTER_NOTE,			// Invalid Alphabet encountering while trying to read a Note Symbol
			PARSE_ERROR_OCTAVE_MACRO_END,		// MACRO_END missing while parsing an Octave Macro
			PARSE_ERROR_OCTAVE_VALUE,			// Failure while conveting/retrieving an Octave macro number
			PARSE_ERROR_OCTAVE_MAXLIMIT,		// Specified an octave that is beyond the permitted range
			PARSE_ERROR_DURATION_MACRO_END,		// MACRO_END missing while parsing an Duration Macro
			PARSE_ERROR_DURATION_VALUE,			// Failure while conveting/retrieving a Duration number
			PARSE_ERROR_TUPLET_NUMERATOR,		// Failure while conveting/retrieving the Numerator of Tuplet fraction
			PARSE_ERROR_TUPLET_DENOMINATOR,		// Failure while conveting/retrieving the Denominator of Tuplet fraction
			PARSE_ERROR_VELOCITY_MACRO_END,		// MACRO_END missing while parsing a Velocity Macro
			PARSE_ERROR_VELOCITY_VALUE,			// Failure while conveting/retrieving a Velocity number
			PARSE_ERROR_NOTEVALUE_MAXLIMIT,		// Computed Note Value is going beyond the permitted range [0, 127]
		};
		/// <Summary>
		/// The structure that is passed as argument to the Error Event Handler procedure.
		/// Listeners who want to know about the error events can subscribe to the evError
		/// event and handle the trace events raised by MusicStringParser as the parsing progresses.
		/// 
		/// Unlike the other programming paradigms that teach greedy methods asking you to exit 
		/// the moment any error occurs, MusicNote library fights to live its best before it gives up.
		/// So, usually if there is any error in parsing a token, parser does not abort and tries to continue 
		/// parsing the rest of the tokens. 
		/// In case this behavior is not suitable for your application and if you would like to stop the processing
		/// immediately when there is an error, you can indicate your preference to stop the processing 
		/// by using the <code>bShouldStopParsing</code> variable. Set it to true in the error handler
		/// procedure, to halt the processing immediately. By default it would be False (meaning the parser
		/// should continue its best to resolve the token ignoring the error).
		///
		/// Note that, when there are multiple listeners for the event, options set by one listener might be
		/// overriden by the next listener. Parser only respects the last value it sees. So, use care while
		/// dealing with multiple listeners, and ensure that they do not use conflicting settings.
		///
		/// Ofcourse, in case of critical errors such as memory allocation failure etc.. there is nothing 
		/// that could be done, so the listener preferences are ignored and processing would be stopped.
		/// </Summary>
		struct ErrorEventHandlerArgs : public OIL::CEventHandlerArgs 
		{  
			const ErrorCode	errCode; /// Error Code
			const TCHAR* szErrMsg;	/// The Error message sent by MusicStringParser
			const TCHAR* szToken;	/// Token the error is applicable for
			bool bShouldStopParsing; /// Should Parser continue to parse or should stop now? (only for PARSE_ERRORS)

			inline ErrorEventHandlerArgs(ErrorCode argErrCode, const TCHAR* argErrMsg, const TCHAR* argSzToken)
				: errCode(argErrCode), szErrMsg(argErrMsg), szToken(argSzToken), bShouldStopParsing(false) { }
		};

		OIL::CEventT<const MusicStringParser, ErrorEventHandlerArgs> evError;
	protected:
		/// <Summary>
		/// Error method is called whenever there is a parse error. 
		/// This method will raise events through the <code>MusicStringParser::evError</code> event
		/// variable. Listeners can subscribe to it to get to know about the parse errors.
		///
		/// You can override this method in derived classes if you wish to modify this error
		/// reporting implementation.
		///
		/// @param szTraceMsg Error message details
		/// @param szToken The token that is being parsed while the error happened
		/// @return Boolean value indicating if the parsing should stop now or should continue
		/// </Summary>
		inline virtual bool Error(ErrorCode argErrCode, const TCHAR* szTraceMsg, const TCHAR* szToken)
		{
			ErrorEventHandlerArgs evArgs(argErrCode, szTraceMsg, szToken);
			OIL::CEventSource::RaiseEvent(&evError, &evArgs);
			return evArgs.bShouldStopParsing;
		}

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

		struct NoteContext
		{
			bool isRest;
			bool isNumeric;
			bool isChord;
			bool isFirstNote;
			bool isSequentialNote;
			bool isParallelNote;
			bool isNatural;
			bool existsAnotherNote;
			bool anotherIsSequential;
			bool anotherIsParallel;
			bool isStartOfTie;
			bool isEndOfTie;
			unsigned char	halfSteps[8];  
			unsigned short	numHalfSteps;
			short			noteNumber;
			short			octaveNumber;
			double			decimalDuration;
			long			duration;
			unsigned short	attackVelocity;
			unsigned short	decayVelocity;
			NoteContext()
				: isRest(0),
				isNumeric(0),
				isChord(0),
				isFirstNote(1),
				isSequentialNote(0),
				isParallelNote(0),
				isNatural(0),
				existsAnotherNote(1),
				anotherIsSequential(0),
				anotherIsParallel(0),
				isStartOfTie(0),
				isEndOfTie(0),
				numHalfSteps(0),
				noteNumber(0),
				octaveNumber(-1),
				decimalDuration(0.0),
				duration(0),
				attackVelocity(DEFAULT_ATTACK_VELOCITY),
				decayVelocity(DEFAULT_DECAY_VELOCITY)
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