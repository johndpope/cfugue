#ifndef PARESER_H__B6901FE6_6B09_486c_87AB_E584C1DBE30F__
#define PARESER_H__B6901FE6_6B09_486c_87AB_E584C1DBE30F__

/// <Summary>
/// You can enable or disable tracing events in the CParser derived classes by
/// defining ENABLE_TRACING to be 1 or 0 before including this header file.
/// By default, ENABLE_TRACING is defined to be 1 in DEBUG builds, and 0 in
/// RELEASE builds.
/// </Summary>
#ifndef ENABLE_TRACING
#if defined(DEBUG) || defined (_DEBUG)
#define ENABLE_TRACING 1
#endif // if defined(DEBUG)
#endif // ifndef ENABLE_TRACING

#include "EventHandler.h"

namespace MusicNoteLib
{
	// Forward Declarations
	class Instrument;
    class Tempo;
    class Voice;
	struct Note;
	class CParserListener;

	class CParser : public OIL::CEventSource
	{
	public:
			
		OIL::CEventT<const CParser> evController;
		OIL::CEventT<const CParser, Instrument> evInstrument; // Parser encountered an Instrument command
		OIL::CEventT<const CParser> evKeySignature;
		OIL::CEventT<const CParser> evLayer; // Parser encountered a Layer command
		OIL::CEventT<const CParser> evMeasure;
		OIL::CEventT<const CParser> evChannelPressure;
		OIL::CEventT<const CParser> evPolyphonicPressure;
		OIL::CEventT<const CParser> evPitchBend;
		OIL::CEventT<const CParser, Tempo> evTempo; // Parser encountered a Tempo command
		OIL::CEventT<const CParser> evTime;
		OIL::CEventT<const CParser, Voice> evVoice; // Parser encountered a Voice command
		OIL::CEventT<const CParser, Note> evNote;	// Raised for a first note in a group of notes
		OIL::CEventT<const CParser> evSequentialNote; // Encountered a Sequential note after a first note
		OIL::CEventT<const CParser> evParalleNote; // Encountered a Parallel note after a first note

		inline CParser(void) { }

		inline virtual ~CParser(void) {	}

		///<Summary> Subscribes a Listener object for all events </Summary>
		void AddListener(CParserListener* pListener);

		///<Summary> UnSubscribes the Listener Object from the events </Summary>
		void RemoveListener(CParserListener* pListener);

	public:
		/// <Summary>Error code values used by ErrorEventHandlerArgs</Summary>
		enum ErrorCode
		{
			CRITICAL_ERROR_MEMORY_ALLOCATION,	// Memory allocation failed
			PARSE_ERROR_MISSING_ASSIGNMENT,		// No Assignment symbol found
			PARSE_ERROR_INSTRUMENT_MACRO_END,	// MACRO_END missing while parsing a Instrument Macro.			
			PARSE_ERROR_INSTRUMENT_VALUE,		// Failure while converting/retrieving a Instrument number.
			PARSE_ERROR_TEMPO_MACRO_END,	    // MACRO_END missing while parsing a Tempo Macro.			
			PARSE_ERROR_TEMPO_VALUE,		    // Failure while converting/retrieving a Tempo number.
			PARSE_ERROR_VOICE_MACRO_END,	    // MACRO_END missing while parsing a Voice Macro.			
			PARSE_ERROR_VOICE_VALUE,		    // Failure while converting/retrieving a Voice number.
			PARSE_ERROR_VOICE_MAXLIMIT,		    // Specified a voice that is beyond the permitted range [0, 15]
			PARSE_ERROR_NUMERIC_NOTE_END,		// MACRO_END missing while parsing a numeric note.			
			PARSE_ERROR_NUMERIC_NOTE_VALUE,		// Failure while converting/retrieving a numeric note number.
			PARSE_ERROR_LETTER_NOTE,			// Invalid Alphabet encountering while trying to read a Note Symbol
			PARSE_ERROR_OCTAVE_MACRO_END,		// MACRO_END missing while parsing an Octave Macro
			PARSE_ERROR_OCTAVE_VALUE,			// Failure while conveting/retrieving an Octave macro number
			PARSE_ERROR_OCTAVE_MAXLIMIT,		// Specified an octave that is beyond the permitted range [0, 10]
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

		OIL::CEventT<const CParser, ErrorEventHandlerArgs> evError;
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

		OIL::CEventT<const CParser, TraceEventHandlerArgs> evTrace;

#if ENABLE_TRACING
	protected:
		/// <Summary>
		/// Trace method is called through out the parsing process. However, the functionality
		/// is enabled only if ENABLE_TRACING directive is defined to be 1 (which is default for
		/// DEBUG builds, but not for RELEASE builds).
		/// When enabled, Trace will raise events through the CParser::evTrace event
		/// variable. Listeners can subscribe to this event to get to know the trace messages.
		/// </Summary>
		inline virtual void Trace(const TCHAR* szTraceMsg)
		{
			OIL::CEventSource::RaiseEvent(&evTrace, &TraceEventHandlerArgs(szTraceMsg));
		}
#else

#define Trace(x) 

#endif // #if ENABLE_TRACING
	};

} // namespace MusicNoteLib



#endif // PARESER_H__B6901FE6_6B09_486c_87AB_E584C1DBE30F__