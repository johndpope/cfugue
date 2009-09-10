#include "stdafx.h"
#include "MusicStringParser.h"

#define STRUTILS_RETURN_TYPE MusicNoteLib::MString
#include "StrUtils.h"

#include "math.h"

namespace MusicNoteLib
{
	// Helper Methods
	template<typename T>
	inline int LoadValueFromString(const TCHAR* sz, T* pRetVal)
	{
		_ASSERTE(_T("LoadValueFromString called with Unknown Type") == NULL);

		return *pRetVal = NULL;
	}

	template<>
	inline int LoadValueFromString(const TCHAR* sz, unsigned short* pRetVal)
	{
		unsigned short nVal = 0;

		const TCHAR* psz = sz;

		while(/*psz != NULL &&*/ *psz >= _T('0') && *psz <= _T('9'))
		{
			nVal = (nVal * 10) + (*psz - _T('0'));
			psz++;
		}

		if(psz != sz) *pRetVal = nVal;

		return psz - sz;
	}

	template<>
	inline int LoadValueFromString(const TCHAR* sz, double* pRetVal)
	{
		double nVal = 0;

		const TCHAR* psz = sz;

		while(*psz >= _T('0') && *psz <= _T('9'))		// Read the Decimal part
		{
			nVal = (nVal * 10) + (*psz - _T('0'));
			psz++;
		}

		if(*psz == _T('.'))
		{
			double nFracIndex = 10;
			psz++; 
			while(*psz >= _T('0') && *psz <= _T('9'))		// Read the fractional part
			{
				nVal = nVal + ((*psz - _T('0')) / nFracIndex);
				nFracIndex *= 10;
				psz++; 
			}
		}

		if(psz != sz) *pRetVal = nVal;

		return psz - sz;
	}	

	bool MusicStringParser::Parse(const TCHAR* szTokens)
	{
		bool bNonContinuableErrorOccured = false;

		const TCHAR* szNextToken = szTokens;

		while(*szNextToken!= NULL)
		{
			while(*szNextToken!= NULL && _istspace(*szNextToken)) ++szNextToken; // Read all the space characters

			if(*szNextToken!= NULL && 
				false == ParseToken(szNextToken, &bNonContinuableErrorOccured) && 
				true == bNonContinuableErrorOccured)
				return false; // Some irrevocable error Occured. Return false;

			while(*szNextToken!= NULL && _istspace(*szNextToken++) == false) ; // Read past the token characters
		}

		return true;
	}

	/// <Summary>
	/// Parses a single token. To Parse a string that contains multiple tokens, 
	/// use the <code>Parse</code> method. We consider a string to be having multiple
	/// tokens if there are whitespace charactes in it such as \n \t or Space.
	///
	/// @param szToken: The token to be parsed. We expect this to be a single token 
	/// free of any whitespace characters, such as \t \n or Space. If the input token
	/// contains any of these characters, results are undefined.
	///
	/// @return True if success, False in case of any failures.
	/// </Summary>
	bool MusicStringParser::ParseToken(const TCHAR* szToken, bool* pbNonContinuableErrorOccured/* = NULL*/)
	{			
		bool bNonContinuableErrorOccured = false, bRetVal;

		TCHAR* szTokenDup = _tcsdup(szToken);	// Create a modifiable string
		if(szTokenDup == NULL)
		{
			Error(CRITICAL_ERROR_MEMORY_ALLOCATION, _T("Memory allocation failure in MusicStringParser::ParseToken"), szToken); 		
			bNonContinuableErrorOccured = true;
			bRetVal = false;
		}
		else
		{
			TCHAR* szUppercaseToken = _tcsupr(szTokenDup); // convert the string to upper case

			Trace(MString(_T("MusicStringParser::ParseToken: ")) + szUppercaseToken);

			switch(szUppercaseToken[0])
			{
			case TOKEN_START_VOICE: bRetVal = ParseVoiceToken(szUppercaseToken+1, &bNonContinuableErrorOccured); break; 
			case TOKEN_START_TEMP:	bRetVal = ParseTempoToken(szUppercaseToken+1, &bNonContinuableErrorOccured);  break; 
			case TOKEN_START_INSTRUMENT: bRetVal = ParseInstrumentToken(szUppercaseToken+1, &bNonContinuableErrorOccured);  break; 
			case TOKEN_START_LAYER: bRetVal = ParseLayerToken(szUppercaseToken+1, &bNonContinuableErrorOccured);  break; 
			case TOKEN_START_SIGNATURE: bRetVal = ParseKeySignatureToken(szUppercaseToken+1, &bNonContinuableErrorOccured);  break; 
			case TOKEN_START_CONTROLLER: bRetVal = ParseControllerToken(szUppercaseToken+1, &bNonContinuableErrorOccured);  break; 
			case TOKEN_START_TIME: bRetVal = ParseTimeToken(szUppercaseToken+1, &bNonContinuableErrorOccured);  break; 
			case TOKEN_START_KEYPRESSURE: bRetVal = ParseKeyPressureToken(szUppercaseToken+1, &bNonContinuableErrorOccured);  break; 
			case TOKEN_START_CHANNELPRESSURE: bRetVal = ParseChannelPressureToken(szUppercaseToken+1, &bNonContinuableErrorOccured);  break; 
			case TOKEN_START_PITCHBEND: bRetVal = ParsePitchBendToken(szUppercaseToken+1, &bNonContinuableErrorOccured);  break; 
			case TOKEN_START_MEASURE: bRetVal = ParseMeasureToken(szUppercaseToken+1, &bNonContinuableErrorOccured); break;
			case TOKEN_START_DICTIONARY: bRetVal = ParseDictionaryToken(szUppercaseToken+1, &bNonContinuableErrorOccured); break;
			case TOKEN_START_NOTE: bRetVal = ParseNoteToken(szUppercaseToken, &bNonContinuableErrorOccured); break;
			default: bRetVal = ParseNoteToken(szUppercaseToken, &bNonContinuableErrorOccured); break;	// The first character is none of the above. Could be a Note such as A B C etc... 
			}
			
			free(szTokenDup); // Free the memory allocated by _tcsdup
		}

		if(pbNonContinuableErrorOccured != NULL)
			*pbNonContinuableErrorOccured = bNonContinuableErrorOccured;

		return bRetVal;
	}

	///<Summary>Parses a Dictionary Element Token. Creates or Updates the Dictionary Element Value</Summary>		
	bool MusicStringParser::ParseDictionaryToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
	{
		TCHAR* pszAssignSymbol = _tcschr(szToken, ASSIGNMENT_SYMBOL); // Find the Equals sign
		if(pszAssignSymbol == NULL)
		{
			MString str(ASSIGNMENT_SYMBOL + MString(_T(" Symbol Not found")));
			*pbNonContinuableErrorOccured = Error(PARSE_ERROR_MISSING_ASSIGNMENT, str, szToken);
			return false;
		}
		
		*pszAssignSymbol = _T('\0');

		const TCHAR* pszKey = szToken;
		const TCHAR* pszValue = pszAssignSymbol + 1;

		m_Dictionary[pszKey] = pszValue; // Create or Update the value

		return true;
	}

	/// <Summary> Parses Tokens that look like Music Note symbols. Decides their Root and
	/// Calls the correct method based on the Note nature, such as Numeric Note, Rest Note etc.</Summary>
	bool MusicStringParser::ParseNoteToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
	{
		NoteContext ctx;

		int nLen = 0;

		while(ctx.existsAnotherNote)
		{
			Trace(MString(_T("MusicStringParser::ParseNote: ")) + szToken);

			DecideSequentialOrParallel(ctx);
		
			nLen = ParseNoteRoot(szToken, ctx); 
			if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false;}
			szToken += nLen;

			nLen = ParseNoteOctave(szToken, ctx);
			if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false;}
			szToken += nLen;

			if(ComputeNoteValue(ctx) == -1) { *pbNonContinuableErrorOccured = true; return false;}

			nLen = ParseNoteDuration(szToken, ctx);
			if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false;}
			szToken += nLen;

			nLen = ParseNoteVelocity(szToken, ctx);
			if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false;}
			szToken += nLen;

			nLen = ParseNoteConnector(szToken, ctx);
			if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false;}
			szToken += nLen;

			RaiseNoteEvents(ctx);
		}

		Trace(szToken[0] != _T('\0') ? (MString(_T("Ignoring: ")) + szToken) : _T(""));

		return true;
	}
	
	/// <Summary>
	/// Tests if a Note is known to be Sequential (connected with _) or Parallel (connected with +) 
	///</Summary>
	void MusicStringParser::DecideSequentialOrParallel(NoteContext& ctx)
	{
		ctx.type = Note::FIRST;
		//ctx.isSequentialNote = false;
		if(ctx.anotherIsSequential)
		{
			//ctx.isSequentialNote = true;
			ctx.type = Note::SEQUENTIAL;
			ctx.anotherIsSequential = false;
			Trace(_T("This note is sequential"));
		}

		//ctx.isParallelNote = false;
		if(ctx.anotherIsParallel)
		{
			//ctx.isParallelNote = true;
			ctx.type = Note::PARALLEL;
			ctx.anotherIsParallel = false;
			Trace(_T("This note is parallel"));
		}
	}

	/// <Summary> Parses Numeric, Rest and Letter Notes. 
	/// Returns the number of characaters consumed from the string during the parsing </Summary>
	int MusicStringParser::ParseNoteRoot(TCHAR* szToken, NoteContext& ctx)
	{
		switch(szToken[0])
		{
		case MACRO_START : return ParseNumericNote(szToken, ctx);
		case REST_NOTE : return ParseRest(szToken, ctx);
		default: return ParseLetterNote(szToken, ctx);
		}
	}

	/// <Summary> Parses Numeric Note. 
	/// Returns the number of characaters consumed from the string during the parsing. </Summary>
	int MusicStringParser::ParseNumericNote(TCHAR* szToken, NoteContext& ctx)
	{
		bool bSuccess = false; unsigned short nNoteNumber=0;
			
		int nLen = ParseNumber(szToken, &nNoteNumber, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_NUMERIC_NOTE_END, PARSE_ERROR_NUMERIC_NOTE_VALUE);
		if(nLen == -1) return -1;	// Some irrevocable error occured
		if(bSuccess)
		{
			ctx.isNumeric = true; 
			ctx.noteNumber = nNoteNumber;
			Trace(_T("Numeric Note with value: ") +	OIL::ToString(ctx.noteNumber));
		}

		return nLen; // Number of characters consumed
	}

	/// <Summary> Parses Rest Note. 
	/// Returns the number of characaters consumed from the string during the parsing </Summary>
	int MusicStringParser::ParseRest(TCHAR* szToken, NoteContext& ctx)
	{
		Trace(_T("This Note is a Rest"));

		ctx.isRest = true;

		return 1;
	}

	/// <Summary> Parses Alphabetic Note. 
	/// Returns the number of characaters consumed from the string during the parsing. </Summary>
	int MusicStringParser::ParseLetterNote(TCHAR* szToken, NoteContext& ctx)
	{
		switch(szToken[0])
		{
		case NOTE_C: ctx.noteNumber = NOTE_C_Value; break;
		case NOTE_D: ctx.noteNumber = NOTE_D_Value; break;
		case NOTE_E: ctx.noteNumber = NOTE_E_Value; break;
		case NOTE_F: ctx.noteNumber = NOTE_F_Value; break;
		case NOTE_G: ctx.noteNumber = NOTE_G_Value; break;
		case NOTE_A: ctx.noteNumber = NOTE_A_Value; break;
		case NOTE_B: ctx.noteNumber = NOTE_B_Value; break;
		default:
			{
				return Error(PARSE_ERROR_LETTER_NOTE, MString(_T("Invalid Note: ")) + szToken[0], szToken) ? -1 : 1;
			}
		}

		// Apply the modifiers such as Sharp, Flat and natural to the Note Number
		bool bCheckForModifiers = true; const TCHAR* pszSymbol = szToken + 1;
		while(bCheckForModifiers)
		{
			switch(pszSymbol[0])
			{
			case NOTE_MODIFIER_SHARP: ctx.noteNumber++;  pszSymbol++; break;
			case NOTE_MODIFIER_FLAT: ctx.noteNumber--; pszSymbol++; break;
			case NOTE_MODIFIER_NATURAL: ctx.isNatural = true; // follow-on 
			case _T('\0'):
			default: bCheckForModifiers = false; break;
			}
		}
		
		Trace(_T("Note number within octave: ") + OIL::ToString(ctx.noteNumber));

		return pszSymbol - szToken; // return the number of characters read
	}

		
	/// <Summary> Parses any associated note octave </Summary>
	int MusicStringParser::ParseNoteOctave(TCHAR* szToken, NoteContext& ctx)
	{
		// Rest notes or Numeric Notes do not have Octaves specified
		if(ctx.isRest || ctx.isNumeric) return 0;

		// Octaves are optional - they may or may not be present. 
		// Only values in the range [0, 10] are valid.
		// Check if macro is used, otherwise scan it.
		
		bool bSuccess = false; unsigned short nOctave=0;
			
		int nLen = ParseNumber(szToken, &nOctave, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_OCTAVE_MACRO_END, PARSE_ERROR_OCTAVE_VALUE);
		if(nLen == -1) return -1;	// Some irrevocable error occured
		if(bSuccess)
		{
			ctx.octaveNumber = nOctave;

			if(ctx.octaveNumber > 10)
			{
				MString str(_T("Octave ") + OIL::ToString(ctx.octaveNumber) + _T(" is beyond the range [0, 10]"));
				if(Error(PARSE_ERROR_OCTAVE_MAXLIMIT, str, szToken)) return -1; // if we should stop processing any further, return -1
				ctx.octaveNumber = 10; // if we need to continue despite the error, ceil the value
			}
		}
		
		Trace(ctx.octaveNumber == (unsigned short)-1 ? _T("No Octave Specified") : _T("Octave Number: ") + OIL::ToString(ctx.octaveNumber));

		return nLen; // Number of characters consumed here
	}

	int MusicStringParser::ComputeNoteValue(NoteContext& ctx)
	{
		if(ctx.isRest) return 0;	// No note values for Rest Notes

		if(ctx.octaveNumber == -1 && !ctx.isNumeric) 	// if Octave is not specified (and if this is non-numeric note) give some default octave value
		{
			if(ctx.isChord) 
				ctx.octaveNumber = DEFAULT_CHORD_OCTAVE;
			else 
				ctx.octaveNumber = DEFAULT_NONCHORD_OCTAVE;		
			
			Trace(_T("Choosing Default Octave:") + OIL::ToString(ctx.octaveNumber));
		}

		//TODO: Adjust the Notes based on KeySignature
		
		if(!ctx.isNumeric) // if this is a non-numeric note, compute its value from octave and local note value
		{
			ctx.noteNumber = (ctx.octaveNumber * 12) + ctx.noteNumber;

			Trace(_T("Computed Note Number:") + OIL::ToString(ctx.noteNumber));
		}	
		
		// Check the limits
		if(ctx.noteNumber > 127 || ctx.noteNumber < 0)
		{
			MString str(_T("Computed Note Number") + OIL::ToString(ctx.noteNumber) + _T(" is going beyond the range [0, 127]"));
			if(Error(PARSE_ERROR_NOTEVALUE_MAXLIMIT, str, NULL)) return -1; // if we should stop processing any further, return -1

			// if we need to continue despite the error, ceil or floor the value
			if(ctx.noteNumber > 127)
				ctx.noteNumber = 127; 
			else if(ctx.noteNumber < 0)
				ctx.noteNumber = 0;

			Trace(_T("  Adjusted the value to") + OIL::ToString(ctx.noteNumber) + _T(" and continuing..."));
		}
		return 0;
	}
	
	/// <Summary> Parses duration of note </Summary>
	int MusicStringParser::ParseNoteDuration(TCHAR* szToken, NoteContext& ctx)
	{
		TCHAR* pszDuration = szToken;

		if(szToken[0] == NOTE_DURATION_NUMERIC) // Check if this is a numeric duration
		{
			bool bSuccess = false;				
			int nLen = ParseNumber(szToken+1, &ctx.decimalDuration, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_DURATION_MACRO_END, PARSE_ERROR_DURATION_VALUE);
			if(nLen == -1)
				return -1;	// Some irrevocable error occured
			if(bSuccess)
			{			
				pszDuration = szToken + 1 + nLen; // Update the position for next scanning
			}
			else
			{
				MString str(MString(_T("Error while reading Numeric Duration Value")));
				if(Error(PARSE_ERROR_DURATION_VALUE, str, szToken)) return -1; 

				pszDuration = szToken + 1; 

				// If we have to ignore this error, ignore this / character	
				Trace(_T(" Ignoring / and proceeding with ") + MString(pszDuration));
			}
		}

		// In case Numeric Duration failed or absent, 
		// Try reading the duration as Alphabets (such as W, H etc..)
		if(!ctx.decimalDuration) 				
			pszDuration += ParseLetterDuration(pszDuration, ctx);

		// if No Duration specified, Default to a Quarter note
		if(ctx.decimalDuration == 0) 
			ctx.decimalDuration = 1.0 / 4.0;

		Trace(_T("Decimal Duration is: ") + OIL::ToString(ctx.decimalDuration));

		// Scan for any optional Tuplet specifications
		int nTupletLen = ParseTuplet(pszDuration, ctx);
		if(nTupletLen < 0) return -1; // some irrevocable error happened

		ctx.duration = (long) (120.0 * ctx.decimalDuration);	//TODO: Compute the duration based on the current Tempo

		Trace(_T("Actual Duration is: ") + OIL::ToString(ctx.duration));

		return pszDuration - szToken + nTupletLen; // Return the number of characters consumed
	}
	
	/// <Summary> Parses duration expressed as Alphabet W, H etc.. </Summary>
	int MusicStringParser::ParseLetterDuration(TCHAR* szToken, NoteContext& ctx)
	{
		TCHAR* pszDuration = szToken;

		while(*pszDuration != NULL)
		{
			if(pszDuration[0] == NOTE_TIE)
			{
				if(!ctx.decimalDuration) // if we have not yet scanned the duration, i.e. - is preceding the duration					
					ctx.isEndOfTie = true; // this could be an end of tie or continuation
				else	// - is following the duration, could be a start of tie or continuation
				{
					if(ctx.isEndOfTie) // if we have seen a tie symbol before duration, then this is a continuation
						ctx.isEndOfTie = false;
					else // This is the first tie symbol on this token, mark this as start of tie
						ctx.isStartOfTie = true;
					break; // When tie symbol is seen after duration, No need to scan further.
				}				
				pszDuration++;
			}

			int nDurationNumber = 0;

			switch(pszDuration[0])
			{
			case NOTE_DURATION_WHOLE: nDurationNumber = 1; break;
			case NOTE_DURATION_HALF: nDurationNumber = 2; break;
			case NOTE_DURATION_QUARTER: nDurationNumber = 4; break;
			case NOTE_DURATION_EIGTH: nDurationNumber = 8; break;
			case NOTE_DURATION_SIXTEENTH: nDurationNumber = 16; break;
			case NOTE_DURATION_THIRTYSECOND: nDurationNumber = 32; break;
			case NOTE_DURATION_SIXTYFOURTH: nDurationNumber = 64; break;
			case NOTE_DURATION_128: nDurationNumber = 128; break;
			}
			if(nDurationNumber > 0)
			{
				double durVal = 1.0/nDurationNumber;
				ctx.decimalDuration += durVal;
				if(pszDuration[1] == NOTE_DURATION_DOT) // if used a dot
				{
					ctx.decimalDuration += (durVal/2.0); // add extra half to the original
					pszDuration++;
				}
			}
			else 
				break; // encountered a character that is not a valid duration symbol. Lets get out of here.

			pszDuration ++;
		}	
		
		return pszDuration - szToken; // return the number of characters consumed
	}

	/// <Summary> Parses tuples expressed with * : </Summary>
	int MusicStringParser::ParseTuplet(TCHAR* szToken, NoteContext& ctx)
	{
		if(szToken[0] == NOTE_TUPLET_START)
		{
			Trace(_T("Scanning a Tuplet..."));
		
			TCHAR* pszTuplet = szToken + 1;

			double dNumerator = 2.0;
			double dDenominator = 3.0;

			TCHAR* pszTupletRatioMark = _tcschr(pszTuplet, NOTE_TUPLET_RATIOMARK);
			if(pszTupletRatioMark != NULL)
			{
				*pszTupletRatioMark = _T('\0');

				if(!LoadValueFromString(pszTuplet, &dNumerator)) // Read the Numerator 
				{
					MString str(MString(_T("Error while reading Numerator of tuplet")));					
					if(Error(PARSE_ERROR_TUPLET_NUMERATOR, str, pszTuplet)) return -1; 
					Trace(_T("  Defaulting to Numerator Value: ") + OIL::ToString(dNumerator));
				}

				int nLen = LoadValueFromString(pszTupletRatioMark+1, &dDenominator); // Read the Denominator 
				if(nLen == 0)
				{
					MString str(MString(_T("Error while reading Denominator of tuplet")));					
					if(Error(PARSE_ERROR_TUPLET_DENOMINATOR, str, pszTupletRatioMark+1)) return -1; 
					Trace(_T("  Defaulting to Denominator Value: ") + OIL::ToString(dDenominator));
				}

				pszTuplet = pszTupletRatioMark + 1 + nLen;
			}

			double tupletRatio = dNumerator / dDenominator;
			
			ctx.decimalDuration *= tupletRatio;

			Trace(_T("Tuplet Ratio is: ") + OIL::ToString((unsigned int)dNumerator) + _T(":") + OIL::ToString((unsigned int)dDenominator));
			Trace(_T("Decimal Duration after applying Tuplet: ") + OIL::ToString(ctx.decimalDuration));

			return pszTuplet - szToken; // return number of characters consumed
		}

		return 0; // This is not a tuplet  
	}


	/// <Summary> Parses Attach and Decay velocities of notes </Summary>		
	int MusicStringParser::ParseNoteVelocity(TCHAR* szToken, NoteContext& ctx)
	{
		if(ctx.isRest)
		{
			Trace(_T("This is a Rest note .. Skipping the Velocities"));
			return 0;
		}

		TCHAR* pszVelocity = szToken;

		do
		{
			bool bAttack = false, bDecay = false;

			switch(pszVelocity[0])
			{
			case NOTE_VELOCITY_ATTACK: bAttack = true; break;
			case NOTE_VELOCITY_DECAY: bDecay = true; break;
			default: 
				{
					Trace(_T("Attack Velocity: ") + OIL::ToString(ctx.attackVelocity) + _T(" Decay Velocity: ") + OIL::ToString(ctx.decayVelocity));
					return pszVelocity - szToken;
				}
			}

			pszVelocity++;

			bool bSuccess = false; unsigned short nVelocity = 0;
				
			int nLen = ParseNumber(pszVelocity, &nVelocity, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_VELOCITY_MACRO_END, PARSE_ERROR_VELOCITY_VALUE);
			if(nLen == -1) return -1;	// Some irrevocable error occured
			if(bSuccess)
			{
				if(bAttack) ctx.attackVelocity = nVelocity;
				if(bDecay) ctx.decayVelocity = nVelocity;
			}

			pszVelocity += nLen;
		}while(true);	// we can have both attack and decay specified for the same note. so loop again.

		return pszVelocity - szToken;
	}

	/// <Summary> Checks & Parses any associated notes </Summary>
	int MusicStringParser::ParseNoteConnector(TCHAR* szToken, NoteContext& ctx)
	{
		ctx.existsAnotherNote = true; // we start with assumpion that there are connectors

		switch(szToken[0])
		{
		case NOTE_CONNECTOR_SEQUENTIAL: ctx.anotherIsSequential = true; return 1;
		case NOTE_CONNECTOR_PARALLEL: ctx.anotherIsParallel = true; return 1;
		default: Trace(_T("No Connector Notes Found in: ") + MString(szToken)); break;
		}

		ctx.existsAnotherNote = false;
		
		return 0; // No Characters Read
	}


	/// <Summary> Upon completion of parsing a Note token, raises the Events to process the Note</Summary>
	void MusicStringParser::RaiseNoteEvents(NoteContext& ctx)
	{
		if(ctx.isRest)
		{
			ctx.attackVelocity = ctx.decayVelocity = 0; // Rest Notes do not make Sound !!
		}

		RaiseEvent(&evNote, (Note*)&ctx);

		if(ctx.isChord)
		{
			for(unsigned short i=0; i < ctx.numHalfSteps; ++i)
			{
				Note noteObj;
				noteObj.noteNumber = ctx.noteNumber + ctx.halfSteps[i];
				noteObj.duration = ctx.duration;
				noteObj.decimalDuration = ctx.decimalDuration;
				noteObj.type = Note::PARALLEL;
				RaiseEvent(&evNote, (Note*)&ctx);
			}
		}
	}


	/// <Summary> 
	/// Tries to load a value from Dictionary with the given key. 
	/// If the key is undefined, then itself is converted to the required type
	/// and returned as the value.
	///
    /// @return 0 in case of failures of lookup or conversion. Length of the string
    /// scanned, in case of szKey being a number. To be ignored, if szKey is a macro.
	/// </Summary>
	template<typename T>
	int MusicStringParser::GetValueFromDictionary(const TCHAR* szKey, T* pRetVal)
	{
		DICTIONARY::const_iterator dictEntry = m_Dictionary.find(szKey);

		// Lookup may fail if the key is not a macro - in such case we use the key itself as the value
		const TCHAR* pszVal = (dictEntry == m_Dictionary.end() ? szKey : dictEntry->second);

		// *pRetVal = NULL;

		return LoadValueFromString(pszVal, pRetVal);
	}

	template<typename T>
	int MusicStringParser::ParseNumber(TCHAR* szToken, T* pRetVal, bool& bSuccess, 
		TCHAR MacroBracketStart, TCHAR MacroBracketEnd, ErrorCode MacroReadErrorCode, ErrorCode NumberParseErrorCode)
	{
		bSuccess = false;

		const TCHAR* pszNumberStart = szToken;

		if(szToken[0] == MacroBracketStart)	// if Number specified as a Macro
		{
			TCHAR* pszEndBracket = _tcschr(szToken, MacroBracketEnd);
			if(pszEndBracket == NULL)
			{
				MString str(MString(_T("No Matching ")) + MacroBracketEnd + _T(" found for ") + MacroBracketStart);				
				if(Error(MacroReadErrorCode, str, szToken)) return -1; 
				
				// if we have to ignore this error, we ignore this [ character and continue
				pszNumberStart = szToken+1;

				Trace(_T("  Ignoring ") + MacroBracketStart + MString(_T(" and proceeding with ")) + MString(pszNumberStart));
			}
			else // We found an End Bracket
			{
				*pszEndBracket = _T('\0'); 

				if(false == GetValueFromDictionary(szToken + 1, pRetVal)) // Convert the string between [ and ] to octave Number
				{
					MString str(_T("Unable to retrieve Number from ") + MString(szToken+1));
					if(Error(NumberParseErrorCode, str, szToken)) return -1; 	

					// if we have to ignore this error, we ignore the whole string between the [ and ]  
					// and try reading the next portion as a number
					pszNumberStart = pszEndBracket + 1;

					Trace(_T("  Ignoring ") + MString(szToken + 1) + _T(" and proceeding with ") + MString(pszNumberStart));
				}
				else // The Macro got read successfully
				{
					bSuccess = true;

					return (pszEndBracket - szToken) +1; // return the pointer from where the next parsing should start
				}
			}
		}	

		// Number seems to be specified as a direct value.

		int nStrLen = LoadValueFromString(pszNumberStart, pRetVal);
		
		if(nStrLen == 0)
		{
			//MString str(_T("Unable to retrieve Number from ") + MString(pszNumberStart));
			//if(Error(NumberParseErrorCode, str, szToken)) return NULL; 
			
			// if we have to ignore this error, we ignore the fact that we tried to read this
			// string as a number. Let next portions try interpret it differently
		}
		else // The number got read successfully
			bSuccess = true;

		return pszNumberStart - szToken + nStrLen; // return the pointer from where the next parsing should start
	}

} // namespace MusicNoteLib

