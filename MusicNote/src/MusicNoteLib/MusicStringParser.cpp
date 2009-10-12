#include "stdafx.h"
#include "MusicStringParser.h"
#include "Instrument.h"
#include "Layer.h"
#include "Tempo.h"
#include "Time.h"
#include "Voice.h"

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

    template<typename T>
    inline int LoadIntegralValueFromString(const TCHAR* sz, T* pRetVal)
    {
        T nVal = 0;

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
	inline int LoadValueFromString(const TCHAR* sz, unsigned char* pRetVal) // Load Byte
	{
        return LoadIntegralValueFromString(sz, pRetVal);
	}

	template<>
	inline int LoadValueFromString(const TCHAR* sz, unsigned short* pRetVal) // Load Short int
	{
        return LoadIntegralValueFromString(sz, pRetVal);
	}

	template<>
	inline int LoadValueFromString(const TCHAR* sz, unsigned long* pRetVal) // Load Long int
	{
        return LoadIntegralValueFromString(sz, pRetVal);
	}

	template<>
	inline int LoadValueFromString(const TCHAR* sz, double* pRetVal) // Load double
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

		TCHAR* szTokensDup = _tcsdup(szTokens);	// Create a modifiable string
		if(szTokensDup == NULL)
		{
			Error(CRITICAL_ERROR_MEMORY_ALLOCATION, _T("Memory allocation failure in MusicStringParser::Parse"), szTokens); 		
            return false;
		}

        const TCHAR* szDelim = _T(" \t\n\r");  // Delimiters
        TCHAR* szToken = _tcstok(szTokensDup, szDelim);
        while(szToken != NULL)
        {				
            if(false == ParseToken(szToken, &bNonContinuableErrorOccured) && 
				true == bNonContinuableErrorOccured)
                return false;

            szToken = _tcstok(NULL, szDelim); // Get Next Token
        }

        free(szTokensDup); // free the memory allocated by _tcsdup

		return !bNonContinuableErrorOccured;
	}

	bool MusicStringParser::ParseToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured/* = NULL*/)
	{			
		bool bNonContinuableErrorOccured = false, bRetVal;

        TCHAR* szUppercaseToken = _tcsupr(szToken); // convert the string to upper case

        Trace(MString(_T("MusicStringParser::ParseToken: ")) + szUppercaseToken);

        switch(szUppercaseToken[0])
        {
        case TOKEN_DOUBLESPEED_END:
        case TOKEN_DOUBLESPEED_START: bRetVal = ParseSpeedModulatorToken(szUppercaseToken, &bNonContinuableErrorOccured); break;
        case TOKEN_START_VOICE: bRetVal = ParseVoiceToken(szUppercaseToken+1, &bNonContinuableErrorOccured); break; 
        case TOKEN_START_TEMPO:	bRetVal = ParseTempoToken(szUppercaseToken+1, &bNonContinuableErrorOccured);  break; 
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
            
		if(pbNonContinuableErrorOccured != NULL)
			*pbNonContinuableErrorOccured = bNonContinuableErrorOccured;

		return bRetVal;
	}

    bool MusicStringParser::ParseSpeedModulatorToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
    {
        TCHAR* pszToken = szToken;

        unsigned short nSpeed = m_KeySig.Speed();

        while(*pszToken != NULL)
        {
            switch(pszToken[0])
            {
            case TOKEN_DOUBLESPEED_START: nSpeed++; break;
            case TOKEN_DOUBLESPEED_END: nSpeed--; break;
            }
            pszToken++;
        }

        if(nSpeed > 6 || nSpeed <= 0)
        {
			MString str(_T("Speed ") + OIL::ToString(nSpeed) + _T(" is beyond the range [1, 6]"));
            if(Error(PARSE_ERROR_SPEED_MAXLIMIT, str, szToken)) { *pbNonContinuableErrorOccured = true; return false;}; // if we should stop processing any further
			if(nSpeed <= 0)
                nSpeed = 1;
            else if(nSpeed > 6)
                nSpeed = 6;
        }

        m_KeySig.Speed() = nSpeed;

		Trace(MString(_T("MusicStringParser::ParseSpeedModulatorToken Speed = ")) + OIL::ToString(m_KeySig.Speed()));

        return true;
    }
		
    bool MusicStringParser::ParseVoiceToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
    {
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned char nVoice = 0;

		int nLen = ParseNumber(szToken, &nVoice, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_VOICE_MACRO_END, PARSE_ERROR_VOICE_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false; } // Some irrevocable error occured
		if(bSuccess)
		{
			Trace(MString(_T("MusicStringParser::ParseVoiceToken: Voice = ")) + OIL::ToString(nVoice));

			if(nVoice > 15)
			{
				MString str(_T("Voice ") + OIL::ToString(nVoice) + _T(" is beyond the range [0, 15]"));
                if(Error(PARSE_ERROR_VOICE_MAXLIMIT, str, szToken)) { *pbNonContinuableErrorOccured = true; return false;}; // if we should stop processing any further
				nVoice = 15; // if we need to continue despite the error, ceil the value
			}

			Voice voiceObj(nVoice);

			RaiseEvent(&evVoice, &voiceObj);

			return true;
		}
		return false;
    }

    bool MusicStringParser::ParseTempoToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
    {
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned short nTempo = 0;

		int nLen = ParseNumber(szToken, &nTempo, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_TEMPO_MACRO_END, PARSE_ERROR_TEMPO_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false; } // Some irrevocable error occured
		if(bSuccess)
		{
			Trace(MString(_T("MusicStringParser::ParseTempoToken: Tempo = ")) + OIL::ToString(nTempo));

			Tempo tempoObj(nTempo);

			RaiseEvent(&evTempo, &tempoObj);

			return true;
		}
		return false;
    }
		
    bool MusicStringParser::ParseTimeToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
    {
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned long nTime = 0;

		int nLen = ParseNumber(szToken, &nTime, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_TIME_MACRO_END, PARSE_ERROR_TIME_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false; } // Some irrevocable error occured
		if(bSuccess)
		{
			Trace(MString(_T("MusicStringParser::ParseTimeToken: Time = ")) + OIL::ToString(nTime));

			Time timeObj(nTime);

			RaiseEvent(&evTime, &timeObj);

			return true;
		}
		return false;
    }

	///<Summary>
	/// Parses an Instrument Element Token. 
	/// @param szToken the Token that contains the Music Instrument Element
	/// @param pbNonContinuableErrorOccured return value that indicates the caller if further parsing should stopped
	///</Summary>
	bool MusicStringParser::ParseInstrumentToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
	{
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned char nInstrument = 0;

		int nLen = ParseNumber(szToken, &nInstrument, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_INSTRUMENT_MACRO_END, PARSE_ERROR_INSTRUMENT_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false; } // Some irrevocable error occured
		if(bSuccess)
		{
			Trace(MString(_T("MusicStringParser::ParseInstrumentToken: Instrument = ")) + OIL::ToString(nInstrument));

			Instrument instrumentObj(nInstrument);

			RaiseEvent(&evInstrument, &instrumentObj);

			return true;
		}
		return false;
	}

    bool MusicStringParser::ParseLayerToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
    {
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned char nLayer = 0;

		int nLen = ParseNumber(szToken, &nLayer, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_LAYER_MACRO_END, PARSE_ERROR_LAYER_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false; } // Some irrevocable error occured
		if(bSuccess)
		{
			Trace(MString(_T("MusicStringParser::ParseLayerToken: Layer = ")) + OIL::ToString(nLayer));

			Layer layerObj(nLayer);

			RaiseEvent(&evLayer, &layerObj);

			return true;
		}
		return false;
    }

    /// <Summary>
    /// KeySignature plays an important role in differentiating Carnatic and Western style Notes
    /// and makes it possible to seamlessly integrate both into a single MusicString. 
    /// While a Western style KeySignature accepts only the Scale value, the Carnatic counterpart 
    /// on the otherhand supports few more options such as Talam and Speed along with the Ragam.
    ///
    /// In other words, the Western syntax for KeySignature token is: K[SCALE]
    /// where as the Carnatic syntax for the KeySignature token is: K[RAGAM]T[TALAM]S[SPEED]
    /// 
    /// SCALE values are specified using any of the pre-defined macro values such as CMAJ, F#MIN etc.
    /// RAGAM values are specified using the form MELA_x, where x is a number in the range [1, 72].
    /// TALAM values are specified using numbers in the range [1, 35], and SPEED values are specified
    /// using numbers in the range [1, 3]. Also, few pre-defined macros are available for RAGAM, TALAM, 
    /// and SPEED specifiers, such as KALYANI, ADI, VARNAM etc.
    ///
    /// The Ragam, Talam and Speed values can be changed any time during the song. For example, to 
    /// change the Talam while retaining the Ragam and Speed to their current values, use the form: KT[NEW_TALAM]
    ///
    /// Similarily, to Change only Ragam without affecting the Talam and Speed, omit the T and S specifiers
    /// such as K[NEW_RAGAM]. To Change only the Speed, the form then will be KTS[NEW_SPEED].
    ///
    /// Note that the T and S sub-tokens are only valid in the Carnatic Music Mode. Specifying them for 
    /// Western Music will not have any effect and they will not be read or remembered.
    /// </Summary>
    bool MusicStringParser::ParseKeySignatureToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
    {
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned short nValue = 0;

		int nLen = ParseNumber(szToken, &nValue, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_KEYSIGNATURE_MACRO_END, PARSE_ERROR_KEYSIGNATURE_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false; } // Some irrevocable error occured
		if(bSuccess)
		{
            if(nValue >= 128) // Carnatic Mode Key Signature
            {
                nValue = nValue - 128;

			    if(nValue > 72 || nValue <= 0) // Ensure the Range. Valid Range: [1, 72]
			    {
				    MString str(_T("KeySignature ") + OIL::ToString(nValue) + _T(" is beyond the range [0~14 / 64~78 / 129~200]"));
                    if(Error(PARSE_ERROR_KEYSIGNATURE_MAXLIMIT, str, szToken)) { *pbNonContinuableErrorOccured = true; return false;}; // if we should stop processing any further
                    nValue = KeySignature::DEFAULT_RAGAM; // if we need to continue despite the error, use default Value
			    }

			    Trace(MString(_T(" Mela = ")) + OIL::ToString(nValue));

                m_KeySig.SetRagam(nValue);

                // NOTE: We do not RaiseEvent for the Carnatic Mode KeySignature
            }
            else // Western Mode Key Signature
            {
                KeySignature::Scale bScale = ((nValue >= 64) ? KeySignature::MAJOR_SCALE : KeySignature::MINOR_SCALE);

                if(nValue >= 64) nValue = nValue - 64;

			    Trace(MString(_T(" KeySignature = ")) + OIL::ToString(nValue)
                    + MString(_T(" Scale = ")) + (bScale == KeySignature::MAJOR_SCALE ? _T("Major") : _T("Minor")) );

			    if(nValue >= 15) // Ensure the Range. Valid Range: [0, 14]
			    {
				    MString str(_T("KeySignature ") + OIL::ToString(nValue) + _T(" is beyond the range [0~14 / 64~78 / 129~200]"));
                    if(Error(PARSE_ERROR_KEYSIGNATURE_MAXLIMIT, str, szToken)) { *pbNonContinuableErrorOccured = true; return false;}; // if we should stop processing any further
                    nValue = KeySignature::DEFAULT_KEY; // if we need to continue despite the error, use Default Key
			    }

                KeySignature keyObj(nValue > 7 ? (7-nValue) : nValue, bScale);

                m_KeySig = keyObj; // Save a copy for computing the note values later
			
                RaiseEvent(&evKeySignature, &keyObj);

                return true;
            }
		}

        if(m_KeySig.GetMode() != KeySignature::CARNATIC) return false; // [Western Mode] An error reading the KeySig value

        // In case of Carnatic Music, KeySignature can contain an additional Tala specifier. 
        szToken += nLen; unsigned short nTalam = m_KeySig.GetTalam();
        if(szToken[0] == _T('T')) // is there a Tala specifier present?
        {
            nLen = ParseNumber(++szToken, &nTalam, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_TALAM_MACRO_END, PARSE_ERROR_TALAM_VALUE);
            if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false; } // Some irrevocable error occured
            if(bSuccess)
            {
                if(nTalam > 35) // Ensure Range
	            {
		            MString str(_T("Talam ") + OIL::ToString(nTalam) + _T(" is beyond the range [0 ~ 35]"));
                    if(Error(PARSE_ERROR_TALAM_MAXLIMIT, str, szToken)) { *pbNonContinuableErrorOccured = true; return false;}; // if we should stop processing any further
                    nTalam = m_KeySig.GetTalam(); // if we need to continue despite the error, use existing Value
	            }
            }
			    
            Trace(MString(_T(" Talam = ")) + OIL::ToString(nTalam));
        }
        else {  Trace(MString(_T(" No Talam Specified !! Using Talam: ")) + OIL::ToString(nTalam));   }

        m_KeySig.SetTalam(nTalam);

        // Check for Speed specifier
        szToken += nLen; unsigned short nSpeed = m_KeySig.Speed();
        if(szToken[0] == _T('S')) // is there a Speed specifier ?
        {
            nLen = ParseNumber(++szToken, &nSpeed, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_SPEED_MACRO_END, PARSE_ERROR_SPEED_VALUE);
            if(nLen == -1) { *pbNonContinuableErrorOccured = true; return false; } // Some irrevocable error occured
            if(bSuccess)
            {
                if(nSpeed > 6) // Ensure Range
	            {
		            MString str(_T("Speed ") + OIL::ToString(nSpeed) + _T(" is beyond the range [1 ~ 6]"));
                    if(Error(PARSE_ERROR_SPEED_MAXLIMIT, str, szToken)) { *pbNonContinuableErrorOccured = true; return false;}; // if we should stop processing any further
                    nSpeed = m_KeySig.Speed(); // if we need to continue despite the error, use existing Value
	            }
            }

            Trace(MString(_T(" Speed = ")) + OIL::ToString(nSpeed));
        }
        else {  Trace(MString(_T(" No Speed Specified !! Using Speed: ")) + OIL::ToString(nSpeed));   }

        m_KeySig.Speed() = nSpeed;

		return true;
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
			
		Trace(MString(_T("MusicStringParser::ParseDictionaryToken: Defined [")) + pszKey + MString(_T("]=")) + pszValue);

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
        case REST_NOTE: // Rest Note concept is valid only for Western Music
            {                
                if(m_KeySig.GetMode() == KeySignature::WESTERN)
                    return ParseRest(szToken, ctx);
                else
                    ; // follow on 
            }
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
        if(m_KeySig.GetMode() == KeySignature::CARNATIC) // Carnatic Mode Notes
        {
            const TCHAR* pszToken = szToken;
            switch(pszToken[0])
            {
		    case SWARA_S:
                {
                    pszToken++; ctx.noteNumber = SWARA_S_Value;
                    if(pszToken[0] == _T('A')) { pszToken++; ctx.numSwaras = 2;  }
                    break;
                }
		    case SWARA_P:
                {
                    pszToken++; ctx.noteNumber = SWARA_P_Value;
                    if(pszToken[0] == _T('A')) { pszToken++; ctx.numSwaras = 2; }
                    break;
                }
		    case SWARA_M:
                {
                    pszToken++; ctx.noteNumber = SWARA_M_Value; 
                    if(pszToken[0] == _T('A')) { pszToken++; ctx.numSwaras = 2;  }
                    if(pszToken[0] == _T('1')) { pszToken++; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('2')) { pszToken++; ctx.noteNumber++; ctx.isNatural = true; }
                    break;
                }
		    case SWARA_R:
                {
                    pszToken++; ctx.noteNumber = SWARA_R_Value; 
                    if(pszToken[0] == _T('I')) { pszToken++; ctx.numSwaras = 2; }
                    if(pszToken[0] == _T('1')) { pszToken++; ctx.noteNumber--; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('2')) { pszToken++; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('3')) { pszToken++; ctx.noteNumber++; ctx.isNatural = true; }
                    break;
                }
		    case SWARA_G:
                {
                    pszToken++; ctx.noteNumber = SWARA_G_Value; 
                    if(pszToken[0] == _T('A')) { pszToken++; ctx.numSwaras = 2; }
                    if(pszToken[0] == _T('1')) { pszToken++; ctx.noteNumber-=2; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('2')) { pszToken++; ctx.noteNumber--; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('3')) { pszToken++; ctx.isNatural = true; }
                    break;
                }
		    case SWARA_D:
                {
                    pszToken++; ctx.noteNumber = SWARA_D_Value; 
                    if(pszToken[0] == _T('A')) { pszToken++; ctx.numSwaras = 2; }
                    if(pszToken[0] == _T('1')) { pszToken++; ctx.noteNumber--; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('2')) { pszToken++; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('3')) { pszToken++; ctx.noteNumber++; ctx.isNatural = true; }
                    break;
                }
		    case SWARA_N:
                {
                    pszToken++; ctx.noteNumber = SWARA_N_Value; 
                    if(pszToken[0] == _T('I')) { pszToken++; ctx.numSwaras = 2; }
                    if(pszToken[0] == _T('1')) { pszToken++; ctx.noteNumber-=2; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('2')) { pszToken++; ctx.noteNumber--; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('3')) { pszToken++; ctx.isNatural = true; }
                    break;
                }
		    default:
			    {
                    ctx.isRest = true; // treat the unknown notes as rest notes
				    return Error(PARSE_ERROR_LETTER_NOTE, MString(_T("Invalid Note: ")) + pszToken[0], pszToken) ? -1 : 1;
			    }
            }
    		
		    Trace(_T("Note number within octave: ") + OIL::ToString(ctx.noteNumber));

		    return pszToken - szToken; // return the number of characters read
        }

        // Western Mode Notes
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
                ctx.isRest = true; // treat the unknown notes as rest notes
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
           
        ctx.octaveNumber = ctx.isChord ? this->m_nDefChordOctave : this->m_nDefNoteOctave;

        int nLen = 0;

        if(m_KeySig.GetMode() == KeySignature::CARNATIC)
        {
            // Carnatic Mode:
            // Unlike Western Music, Octaves are not absolute numbers in Carnatic Music.
            // They are relative to the Medium Octave, going couple of Octaves above and
            // couple of Octaves below the Medium.
            // Use XOctave token to change the default Medium Octave. For example,
            // XOctave=7 changes the Medium Octave to 7 instead of the default 5.

            // TODO: Implement the XOctave (XShruti) and XChord directives

            bool bCheckForModifiers = true; const TCHAR* pszSymbol = szToken;
            while(bCheckForModifiers)
            {
                switch(pszSymbol[0])
                {
                case NOTE_SHRUTI_UP: ctx.octaveNumber ++; pszSymbol++; break;
                case NOTE_SHRUTI_DOWN: ctx.octaveNumber --; pszSymbol++; break;
                default: bCheckForModifiers = false; break;
                }
            }
            nLen = pszSymbol - szToken; // Number of Characters consumed
        }
        else
        {
            // Western Mode:
		    // Octaves are optional - they may or may not be present. 
		    // Only values in the range [0, 10] are valid.
		    // Check if macro is used, otherwise scan it.
    		
		    bool bSuccess = false; unsigned short nOctave=0;
    			
		    nLen = ParseNumber(szToken, &nOctave, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_OCTAVE_MACRO_END, PARSE_ERROR_OCTAVE_VALUE);
		    if(nLen == -1) return -1;	// Some irrevocable error occured
		    if(bSuccess)
			    ctx.octaveNumber = nOctave;
        }
        
        if(ctx.octaveNumber < 0 || ctx.octaveNumber > 10) // Octave not within range
        {
			MString str(_T("Octave ") + OIL::ToString(ctx.octaveNumber) + _T(" is beyond the range [0, 10]"));
			if(Error(PARSE_ERROR_OCTAVE_MAXLIMIT, str, szToken)) return -1; // if we should stop processing any further, return -1

            if(ctx.octaveNumber < 0) ctx.octaveNumber = 0;
            else if(ctx.octaveNumber > 10) ctx.octaveNumber = 10;
        }
		
		Trace( _T("Octave Number: ") + OIL::ToString(ctx.octaveNumber));

		return nLen; // Number of characters consumed here
	}

	int MusicStringParser::ComputeNoteValue(NoteContext& ctx)
	{
		if(ctx.isRest) return 0;	// No note values for Rest Notes

		//if(ctx.octaveNumber == -1 && !ctx.isNumeric) 	// if Octave is not specified (and if this is non-numeric note) give some default octave value
		//{
		//	if(ctx.isChord) 
		//		ctx.octaveNumber = DEFAULT_CHORD_OCTAVE;
		//	else 
		//		ctx.octaveNumber = DEFAULT_NONCHORD_OCTAVE;		
		//	
		//	Trace(_T("Choosing Default Octave:") + OIL::ToString(ctx.octaveNumber));
		//}

		// Adjust the Notes based on KeySignature
        int nKeySig = m_KeySig.GetKey();

        if(m_KeySig.GetMode() == KeySignature::CARNATIC)
        {
            if(!ctx.isNatural && !ctx.isNumeric) // Key Signatures are applied only to non-numeric Notes
                ctx.noteNumber = KeySignature::LookupSwaraSthanaForMela(ctx.noteNumber, nKeySig);
        }
        else // Western Mode
        {
            if ((nKeySig != 0) && (!ctx.isNatural) && (!ctx.isNumeric)) // Key Signatures are applied only to non-numeric Notes
            {
                if ((nKeySig <= -1) && (ctx.noteNumber == 11)) ctx.noteNumber = 10;
                if ((nKeySig <= -2) && (ctx.noteNumber == 4)) ctx.noteNumber = 3;
                if ((nKeySig <= -3) && (ctx.noteNumber == 9)) ctx.noteNumber = 8;
                if ((nKeySig <= -4) && (ctx.noteNumber == 2)) ctx.noteNumber = 1;
                if ((nKeySig <= -5) && (ctx.noteNumber == 7)) ctx.noteNumber = 6;
                if ((nKeySig <= -6) && (ctx.noteNumber == 0)) { ctx.noteNumber = 11; ctx.octaveNumber--; }
                if ((nKeySig <= -7) && (ctx.noteNumber == 5)) ctx.noteNumber = 4;
                if ((nKeySig >= +1) && (ctx.noteNumber == 5)) ctx.noteNumber = 6;
                if ((nKeySig >= +2) && (ctx.noteNumber == 0)) ctx.noteNumber = 1;
                if ((nKeySig >= +3) && (ctx.noteNumber == 7)) ctx.noteNumber = 8;
                if ((nKeySig >= +4) && (ctx.noteNumber == 2)) ctx.noteNumber = 3;
                if ((nKeySig >= +5) && (ctx.noteNumber == 9)) ctx.noteNumber = 10;
                if ((nKeySig >= +6) && (ctx.noteNumber == 4)) ctx.noteNumber = 5;
                if ((nKeySig >= +7) && (ctx.noteNumber == 11)) { ctx.noteNumber = 0; ctx.octaveNumber++; }
            }
        }

        Trace(_T("After adjusting for Key, Note Number=") + OIL::ToString(ctx.noteNumber));
		
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

        // Western Mode
        {
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
        }

        if(m_KeySig.GetMode() == KeySignature::CARNATIC) // Carnatic Mode does not have explicit Note durations
        {
            ctx.decimalDuration = ctx.numSwaras * 1.0f / (4.0 * pow((double)2.0f, (double)m_KeySig.Speed()-1));//(unsigned short) m_KeySig.GetTalam();
        }

		// if No Duration specified, Default to a Quarter note
		if(ctx.decimalDuration == 0) 
			ctx.decimalDuration = 1.0 / 4.0;

		Trace(_T("Decimal Duration is: ") + OIL::ToString(ctx.decimalDuration));

		// Scan for any optional Tuplet specifications
		int nTupletLen = ParseTuplet(pszDuration, ctx);
		if(nTupletLen < 0) return -1; // some irrevocable error happened

		ctx.duration = (long) (128.0 * ctx.decimalDuration);	//TODO: Compute the duration based on the current Tempo

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

			double nDurationNumber = 0;

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
            case SWARA_DURATION_ONE_EXTRA: ctx.numSwaras++; nDurationNumber = 1; break;
            case SWARA_DURATION_TWO_EXTRA: ctx.numSwaras += 2; nDurationNumber = 0.5; break;
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


	/// <Summary> Parses Attack and Decay velocities of notes </Summary>		
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
            case NOTE_VELOCITY:
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

				Trace(_T("  Ignoring ") + MacroBracketStart);

                return pszNumberStart - szToken; // Failure; return the lentgh scanned
			}
			else // We found an End Bracket
			{
				*pszEndBracket = _T('\0'); 

				if(false == GetValueFromDictionary(szToken + 1, pRetVal)) // Convert the string between [ and ] to Number
				{
					MString str(_T("Unable to retrieve Number from ") + MString(szToken+1));
					if(Error(NumberParseErrorCode, str, szToken)) return -1; 	

					// if we have to ignore this error, we ignore the whole string between the [ and ]  
					Trace(_T("  Ignoring ") + MString(szToken + 1));

					return (pszEndBracket - szToken) +1; // Failure; return the length consumed
				}
				else // The Macro got read successfully
				{
					bSuccess = true;

					return (pszEndBracket - szToken) +1; // return the number of characters scanned
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

		return pszNumberStart - szToken + nStrLen; // return the number of characters consumed
	}

} // namespace MusicNoteLib

