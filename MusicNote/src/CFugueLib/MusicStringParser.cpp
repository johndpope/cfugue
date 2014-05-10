/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://cfugue.sourceforge.net/>.
    
    $LastChangedDate$
    $Rev$
    $LastChangedBy$
*/

#include "stdafx.h"
#include "MusicStringParser.h"
#include "ChannelPressure.h"
#include "Chords.h"
#include "ControllerEvent.h"
#include "Instrument.h"
#include "Layer.h"
#include "PitchBend.h"
#include "PolyphonicPressure.h"
#include "Tempo.h"
#include "TimeToken.h"
#include "Voice.h"

#include "math.h"

//TODO: Use C++0x Futures as asynchronous constructs for the Parsing.
//		No point in parsing in a blocking call. Use external ParserPreference
//		object to control 'exit on first error', 'Synch/Asynch' behaviors

namespace CFugue
{	
	//struct _ssHolder
	//{
	//	static MString ss; //TODO: Single-threaded quick Hack for traces. Doesn't work multi-threaded
	//};
	//decltype(_ssHolder::ss) _ssHolder::ss; // static class member definition
	//#define SSOBJ()		_ssHolder::ss
	//#define MAKESTR(x)	((decltype(SSOBJ())&)(SSOBJ() << x))

    /// <Summary>
    /// Verbose is another mode of tracing where more verbose details of Parsing are reported.
    /// Verbose mode requires the Tracing to be enabled with ENABLE_TRACING defined apriori.
    /// Verbose mode reports all possible internal details for the notes being parsed.
    /// It is possible to limit the Tracing output by defining NO_VERBOSE.
    /// Using NO_VERBOSE with ENABLE_TRACING will only report significant parsing information and not everything.
    /// </Summary>
    #ifndef NO_VERBOSE
		#define Verbose(x)  { MString _str; _str << x; MusicStringParser::Trace(_str); }
    #else
        #define Verbose(x)
    #endif
    #ifndef NO_WARNINGS
		#define Warning(x)  { MString _str; _str << x; MusicStringParser::Trace(_str); }
    #else
        #define Warning(x)
    #endif

	// Helper Methods
	template<typename T>
	inline int LoadValueFromString(const TCHAR* sz, T* pRetVal)
	{
		assert(_T("LoadValueFromString called with Unknown Type") == NULL);

		*pRetVal = _T('0');

		return 0;
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

		return (int)(psz - sz);
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

		return (int)(psz - sz);
	}

    inline bool IsOneOf(const TCHAR ch, const TCHAR* chrArr)
    {
        return _tcschr(chrArr, ch) != NULL;
    }

    inline void EatWhiteSpace(const TCHAR* & psz, const TCHAR* chWhiteSpaces = _T(" \t\n\r"))
    {
        if(psz) while(*psz && IsOneOf(*psz, chWhiteSpaces)) psz++;
    }

    inline void EatComments(const TCHAR* & psz) // CFugue allows C++ style comments: /* */ and //
    {
        if(psz && *psz == _T('/'))
        {
            const TCHAR* pchEndDelimiter = NULL; int nSkipLen=0;
            switch(*(psz + 1))
            {
            case _T('*'): pchEndDelimiter = _T("*/"); nSkipLen = 2; break; // this is a /* .. */ style comment
            case _T('/'): pchEndDelimiter = _T("\n"); nSkipLen = 1; break; // this is a single line // style comment
            default: return;
            }
            const TCHAR* pszEndComment = _tcsstr(psz+2, pchEndDelimiter);
            if(pszEndComment)
                psz = pszEndComment + nSkipLen;
        }
        return ;
    }

    // copies at most nMaxLimit chars
    inline TCHAR* CreateToken(const TCHAR* psz, int& nTokenLen, const TCHAR* szDelim = _T(" \t\n\r")) // returns a new copy - caller need to delete it later
    {
        nTokenLen = 0;
        if(psz)
        {
            const TCHAR* pszStart = psz;
            while(*psz && !IsOneOf(*psz, szDelim)) psz++; // skip till a delimeter is found

            int nLen = (int)(psz - pszStart);
            TCHAR* pszNewCopy = new TCHAR[nLen + 4]; // allocate new buffer - add extra \0s for safety in case parser shoots beyond
            memset(pszNewCopy, 0, sizeof(TCHAR) * (nLen+4));

            _tcsncpy(pszNewCopy, pszStart, nLen); // copy the skipped chars

            nTokenLen = nLen;

            return pszNewCopy;
        }

        return NULL;
    }

	bool MusicStringParser::Parse(const TCHAR* szTokens)
	{
        if(szTokens == NULL) return true;

		bool bNonContinuableErrorOccured = false;

        int nTokenLen, nReadLen;
        const TCHAR* psz = szTokens;
        do
        {
            EatWhiteSpace(psz);

            EatComments(psz);

            //// TODO: Add XML Support
            //if(*psz == _T('<'))
            //    ExtractXmlPortion();
            nTokenLen=0, nReadLen=0;

            TCHAR* pszToken = CreateToken(psz, nTokenLen);            
                
            if(nTokenLen <= 0 || pszToken == NULL) { bNonContinuableErrorOccured= (pszToken==NULL); break;}

            while(nReadLen < nTokenLen)
            {
                int nLen = ParseToken(pszToken, &bNonContinuableErrorOccured);
                if(-1 == nLen && true == bNonContinuableErrorOccured)
                    break;
                nReadLen += nLen;
            }
            delete[] pszToken;

            psz = psz + nReadLen;

        }while(*psz && !bNonContinuableErrorOccured);

		return !bNonContinuableErrorOccured;
	}

	int MusicStringParser::ParseToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured/* = NULL*/)
	{
		bool bNonContinuableErrorOccured = false; int nLen = 0;

        TCHAR* szUppercaseToken = _tcsupr(szToken); // convert the string to upper case

        Verbose(_T("MusicStringParser::ParseToken: ") << szUppercaseToken);

        switch(szUppercaseToken[0])
        {
        case TOKEN_SEPERATOR: nLen = 1; break;
        case TOKEN_DOUBLESPEED_END:
        case TOKEN_DOUBLESPEED_START: nLen = ParseSpeedModulatorToken(szUppercaseToken, &bNonContinuableErrorOccured); break;
        case TOKEN_START_VOICE: nLen = ParseVoiceToken(szUppercaseToken+1, &bNonContinuableErrorOccured) + 1; break;
        case TOKEN_START_TEMPO:	nLen = ParseTempoToken(szUppercaseToken+1, &bNonContinuableErrorOccured) + 1;  break;
        case TOKEN_START_INSTRUMENT: nLen = ParseInstrumentToken(szUppercaseToken+1, &bNonContinuableErrorOccured) + 1;  break;
        case TOKEN_START_LAYER: nLen = ParseLayerToken(szUppercaseToken+1, &bNonContinuableErrorOccured) + 1;  break;
        case TOKEN_START_KEYSIGNATURE: nLen = ParseKeySignatureToken(szUppercaseToken+1, &bNonContinuableErrorOccured) + 1;  break;
        case TOKEN_START_CONTROLLER: nLen = ParseControllerToken(szUppercaseToken+1, &bNonContinuableErrorOccured) + 1;  break;
        case TOKEN_START_TIME: nLen = ParseTimeToken(szUppercaseToken+1, &bNonContinuableErrorOccured) + 1;  break;
        case TOKEN_START_KEYPRESSURE: nLen = ParseKeyPressureToken(szUppercaseToken+1, &bNonContinuableErrorOccured) + 1;  break;
        case TOKEN_START_CHANNELPRESSURE: nLen = ParseChannelPressureToken(szUppercaseToken+1, &bNonContinuableErrorOccured) + 1;  break;
        case TOKEN_START_PITCHBEND: nLen = ParsePitchBendToken(szUppercaseToken+1, &bNonContinuableErrorOccured) + 1;  break;
        case TOKEN_START_MEASURE: nLen = ParseMeasureToken(szUppercaseToken+1, &bNonContinuableErrorOccured) + 1; break;
        case TOKEN_START_DICTIONARY: nLen = ParseDictionaryToken(szUppercaseToken+1, &bNonContinuableErrorOccured) + 1; break;
        case TOKEN_START_NOTE: nLen = ParseNoteToken(szUppercaseToken, &bNonContinuableErrorOccured); break;
        default: nLen = ParseNoteToken(szUppercaseToken, &bNonContinuableErrorOccured); break;	// The first character is none of the above. Could be a Note such as A B C etc...
        }

		if(pbNonContinuableErrorOccured != NULL)
			*pbNonContinuableErrorOccured = bNonContinuableErrorOccured;

		return nLen;
	}

	int MusicStringParser::ParsePitchBendToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
	{
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned int nVal = 0;

		int nLen = ParseNumber(szToken, &nVal, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_PITCHBEND_MACRO_END, PARSE_ERROR_PITCHBEND_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; } // Some irrevocable error occured
		if(bSuccess)
		{
			szToken += nLen;
			if(*szToken == _T(',')) // seems this is in LSB,MSB format
			{
				bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned short nHighVal = 0;

				int nLen2 = ParseNumber(szToken, &nHighVal, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_PITCHBEND_MACRO_END, PARSE_ERROR_PITCHBEND_VALUE);
				if(nLen2 == -1) { *pbNonContinuableErrorOccured = true; return -1; } // Some irrevocable error occured
				if(bSuccess)
				{
					PitchBend pbObj((unsigned char) nVal, (unsigned char) nHighVal);
					RaiseEvent(&evPitchBend, &pbObj);
					return nLen + nLen2;
				}
			}
			else // This is in single integer format
			{
				PitchBend pbObj((unsigned char)(nVal % 128), (unsigned char)(nVal / 128));
				RaiseEvent(&evPitchBend, &pbObj);
				return nLen;
			}
		}
		return 0;
	}

    int MusicStringParser::ParseSpeedModulatorToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
    {
        TCHAR* pszToken = szToken;

        unsigned short nSpeed = m_KeySig.Speed();

        switch(pszToken[0])
        {
        case TOKEN_DOUBLESPEED_START: nSpeed++; break;
        case TOKEN_DOUBLESPEED_END: nSpeed--; break;
        default: return 0;
        }

        if(nSpeed > 6 || nSpeed <= 0)
        {
			MString str;
			str << _T("Speed ") << nSpeed << _T(" is beyond the range [1, 6]");
            if(Error(PARSE_ERROR_SPEED_MAXLIMIT, str, szToken)) { *pbNonContinuableErrorOccured = true; return -1;}; // if we should stop processing any further
			if(nSpeed <= 0)
                nSpeed = 1;
            else if(nSpeed > 6)
                nSpeed = 6;
        }

        m_KeySig.Speed() = nSpeed;

		Verbose(_T("MusicStringParser::ParseSpeedModulatorToken Speed = ") << m_KeySig.Speed());

        return 1;
    }

    int MusicStringParser::ParseVoiceToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
    {
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned short nVoice = 0;

		int nLen = ParseNumber(szToken, &nVoice, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_VOICE_MACRO_END, PARSE_ERROR_VOICE_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; } // Some irrevocable error occured
		if(bSuccess)
		{
			Verbose(_T("MusicStringParser::ParseVoiceToken: Voice = ") << nVoice);

			if(nVoice > 15)
			{
				MString str;
				str << _T("Voice ") << nVoice << _T(" is beyond the range [0, 15]");
                if(Error(PARSE_ERROR_VOICE_MAXLIMIT, str, szToken)) { *pbNonContinuableErrorOccured = true; return -1;}; // if we should stop processing any further
				nVoice = 15; // if we need to continue despite the error, ceil the value
			}

			Voice voiceObj((unsigned char) nVoice);

			RaiseEvent(&evVoice, &voiceObj);

			return nLen;
		}
		return 0;
    }

    int MusicStringParser::ParseTempoToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
    {
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned short nTempo = 0;

		int nLen = ParseNumber(szToken, &nTempo, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_TEMPO_MACRO_END, PARSE_ERROR_TEMPO_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; } // Some irrevocable error occured
		if(bSuccess)
		{
			Verbose(_T("MusicStringParser::ParseTempoToken: Tempo = ") << nTempo);

			Tempo tempoObj(nTempo);

			RaiseEvent(&evTempo, &tempoObj);

			return nLen;
		}
		return 0;
    }

    int MusicStringParser::ParseTimeToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
    {
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned long nTime = 0;

		int nLen = ParseNumber(szToken, &nTime, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_TIME_MACRO_END, PARSE_ERROR_TIME_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; } // Some irrevocable error occured
		if(bSuccess)
		{
			Verbose(_T("MusicStringParser::ParseTimeToken: Time = ") << nTime);

			Time timeObj(nTime);

			RaiseEvent(&evTime, &timeObj);

			return nLen;
		}
		return 0;
    }

	int MusicStringParser::ParseChannelPressureToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
	{
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned short nPressure = 0;

		int nLen = ParseNumber(szToken, &nPressure, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_CPRESSURE_MACRO_END, PARSE_ERROR_CPRESSURE_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; } // Some irrevocable error occured
		if(bSuccess)
		{
			Verbose(_T("MusicStringParser::ParseChannelPressureToken: Pressure = ") << nPressure);

			ChannelPressure cpObj((unsigned char) nPressure);

			RaiseEvent(&evChannelPressure, &cpObj);

			return nLen;
		}
		return 0;
	}

	///<Summary>
	/// Parses an Instrument Element Token.
	/// @param szToken the Token that contains the Music Instrument Element
	/// @param pbNonContinuableErrorOccured return value that indicates the caller if further parsing should stopped
	///</Summary>
	int MusicStringParser::ParseInstrumentToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
	{
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned short nInstrument = 0;

		int nLen = ParseNumber(szToken, &nInstrument, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_INSTRUMENT_MACRO_END, PARSE_ERROR_INSTRUMENT_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; } // Some irrevocable error occured
		if(bSuccess)
		{
			Verbose(_T("MusicStringParser::ParseInstrumentToken: Instrument = ") << nInstrument);

			Instrument instrumentObj((unsigned char) nInstrument);

			RaiseEvent(&evInstrument, &instrumentObj);

			return nLen;
		}
		return 0;
	}

	/// <Summary>
	/// Parses Polyphonic Pressure Token.
	/// The typical structure of this token is: *[NoteNumber],[PressureValue]
	/// @param szToken the Token that contains the Music Instrument Element
	/// @param pbNonContinuableErrorOccured return value that indicates the caller if further parsing should stopped
	/// </Summary>
	int MusicStringParser::ParseKeyPressureToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
	{
        int nReadLen =0;
		NoteContext ctx;

		// Read the Key (Note)
		int	nLen = ParseNoteRoot(szToken, ctx);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1;}
		szToken += nLen; nReadLen += nLen;

		nLen = ParseNoteOctave(szToken, ctx);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1;}
		szToken += nLen; nReadLen += nLen;

		if(ComputeNoteValue(ctx) == -1) { *pbNonContinuableErrorOccured = true; return -1;}

		TCHAR* pszPressure = szToken;
        while(*pszPressure != _T('\0') && *pszPressure == _T(',')) { pszPressure++; nReadLen++; }

		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned short nKeyPressure = 0;

		// Read the Pressure Value
		nLen = ParseNumber(pszPressure, &nKeyPressure, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_KEYPRESSURE_MACRO_END, PARSE_ERROR_KEYPRESSURE_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; } // Some irrevocable error occured
		if(bSuccess)
		{
			Verbose(_T("MusicStringParser::ParseKeyPressureToken: Key = ") << ctx.noteNumber << " Pressure = " << nKeyPressure);

			PolyphonicPressure ppObj((unsigned char) ctx.noteNumber, (unsigned char) nKeyPressure);

			RaiseEvent(&evPolyphonicPressure, &ppObj);

            nReadLen += nLen;
			//return (pszPressure - szToken + nLen);
		}
		return nReadLen;
	}

    int MusicStringParser::ParseLayerToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
    {
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned short nLayer = 0;

		int nLen = ParseNumber(szToken, &nLayer, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_LAYER_MACRO_END, PARSE_ERROR_LAYER_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; } // Some irrevocable error occured
		if(bSuccess)
		{
			Verbose(_T("MusicStringParser::ParseLayerToken: Layer = ") << nLayer);

			Layer layerObj((unsigned char) nLayer);

			RaiseEvent(&evLayer, &layerObj);

			return nLen;
		}
		return 0;
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
    int MusicStringParser::ParseKeySignatureToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
    {
		bool bSuccess = *pbNonContinuableErrorOccured = false; unsigned short nValue = 0;

		int nLen = ParseNumber(szToken, &nValue, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_KEYSIGNATURE_MACRO_END, PARSE_ERROR_KEYSIGNATURE_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; } // Some irrevocable error occured
		if(bSuccess)
		{
            if(nValue >= 128) // Carnatic Mode Key Signature
            {
                nValue = nValue - 128;

			    if(nValue > 72 || nValue <= 0) // Ensure the Range. Valid Range: [1, 72]
			    {
					MString str;
					str << _T("KeySignature ") << nValue << _T(" is beyond the range [0~14 / 64~78 / 129~200]");
                    if(Error(PARSE_ERROR_KEYSIGNATURE_MAXLIMIT, str, szToken)) { *pbNonContinuableErrorOccured = true; return -1;}; // if we should stop processing any further
                    nValue = KeySignature::DEFAULT_RAGAM; // if we need to continue despite the error, use default Value
			    }

			    Verbose(_T(" Mela = ") << nValue);

                m_KeySig.SetRagam(nValue);

                // NOTE: We do not RaiseEvent for the Carnatic Mode KeySignature
            }
            else // Western Mode Key Signature
            {
                KeySignature::Scale bScale = ((nValue >= 64) ? KeySignature::MAJOR_SCALE : KeySignature::MINOR_SCALE);

                if(nValue >= 64) nValue = nValue - 64;

			    Verbose(_T(" KeySignature = ") << nValue << 
						_T(" Scale = ") << (bScale == KeySignature::MAJOR_SCALE ? _T("Major") : _T("Minor")) );

			    if(nValue >= 15) // Ensure the Range. Valid Range: [0, 14]
			    {
					MString str;
					str << _T("KeySignature ") << nValue << _T(" is beyond the range [0~14 / 64~78 / 129~200]");
                    if(Error(PARSE_ERROR_KEYSIGNATURE_MAXLIMIT, str, szToken)) { *pbNonContinuableErrorOccured = true; return -1;}; // if we should stop processing any further
                    nValue = KeySignature::DEFAULT_KEY; // if we need to continue despite the error, use Default Key
			    }

                KeySignature keyObj(nValue > 7 ? (7-nValue) : nValue, bScale);

                m_KeySig = keyObj; // Save a copy for computing the note values later

                RaiseEvent(&evKeySignature, &keyObj);

                return nLen;
            }
		}

        if(m_KeySig.GetMode() != KeySignature::CARNATIC) return 0; // [Western Mode] An error reading the KeySig value

        int nReadLen = nLen;

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
					MString str;
					str << _T("Talam ") << nTalam << _T(" is beyond the range [0 ~ 35]");
                    if(Error(PARSE_ERROR_TALAM_MAXLIMIT, str, szToken)) { *pbNonContinuableErrorOccured = true; return -1;}; // if we should stop processing any further
                    nTalam = m_KeySig.GetTalam(); // if we need to continue despite the error, use existing Value
	            }
                nReadLen += nLen;
            }

            Verbose(_T(" Talam = ") << nTalam);
        }
        else { nLen = 0;  Warning(_T(" No Talam Specified !! Using Talam: ") << nTalam);   }

        m_KeySig.SetTalam(nTalam);

        // Check for Speed specifier
        szToken += nLen; unsigned short nSpeed = m_KeySig.Speed();
        if(szToken[0] == _T('S')) // is there a Speed specifier ?
        {
            nLen = ParseNumber(++szToken, &nSpeed, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_SPEED_MACRO_END, PARSE_ERROR_SPEED_VALUE);
            if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; } // Some irrevocable error occured
            if(bSuccess)
            {
                if(nSpeed > 6) // Ensure Range
	            {
					MString str;
					str << _T("Speed ") << nSpeed <<  _T(" is beyond the range [1 ~ 6]");
                    if(Error(PARSE_ERROR_SPEED_MAXLIMIT, str, szToken)) { *pbNonContinuableErrorOccured = true; return -1;}; // if we should stop processing any further
                    nSpeed = m_KeySig.Speed(); // if we need to continue despite the error, use existing Value
	            }
                nReadLen += nLen;
            }

            Verbose(_T(" Speed = ") << nSpeed);
        }
        else {  Warning(_T(" No Speed Specified !! Using Speed: ") << nSpeed );   }

        m_KeySig.Speed() = nSpeed;

		return nReadLen;
    }

	///<Summary>
	/// Parses a MIDI Controller Event Token.
	/// The typical structure of this token is: X[ControlCode]=[ControlValue]
	/// Some Control Codes have fine and coarse values. Users can either specify
	/// two tokens one for fine and the other for Coarse value, or they can use
	/// the combined Control Code in a single token.
	/// </Summary>
	int MusicStringParser::ParseControllerToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
	{
		TCHAR* pszAssignSymbol = _tcschr(szToken, ASSIGNMENT_SYMBOL); // Find the Equals sign
		if(pszAssignSymbol == NULL)
		{
			MString str(ASSIGNMENT_SYMBOL + MString(_T(" Symbol Not found")));
            if(Error(PARSE_ERROR_MISSING_ASSIGNMENT, str, szToken)) { *pbNonContinuableErrorOccured = true; return -1; }
			return 0;
		}

		*pszAssignSymbol = _T('\0');

		TCHAR* pszKey = szToken;
		TCHAR* pszValue = pszAssignSymbol + 1;

		bool bSuccess = false; int nControllerIndex=-1; int nReadLen = 0;

		int nLen = ParseNumber(pszKey, &nControllerIndex, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_CONTROLLER_MACRO_END, PARSE_ERROR_CONTROLLER_VALUE);
		if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; }	// Some irrevocable error occured
		if(bSuccess)
		{
			if(nControllerIndex >= 128)
			{
				int nControllerCoarse = nControllerIndex % 128; // This is in contrast to JFugue style
				int nControllerFine = nControllerIndex / 128;

				int nControllerValue=0;
				int nLen = ParseNumber(pszValue, &nControllerValue, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_CONTROLLER_MACRO_END, PARSE_ERROR_CONTROLLER_VALUE);
				if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; }	// Some irrevocable error occured

				int coarseValue = (nControllerValue / 128);
				int fineValue = (nControllerValue % 128);

				ControllerEvent evObjCoarse((unsigned char) nControllerCoarse, (unsigned char) coarseValue);
				RaiseEvent(&evController, &evObjCoarse);

				ControllerEvent evObjFine((unsigned char) nControllerFine, (unsigned char) fineValue);
				RaiseEvent(&evController, &evObjFine);

                nReadLen = (int)(pszValue + nLen - szToken);

				Verbose(_T("MusicStringParser::ParseControllerToken: [") << nControllerCoarse << _T("]=") + coarseValue);
				Verbose(_T("MusicStringParser::ParseControllerToken: [") << nControllerFine << _T("]=") << fineValue);
			}
			else
			{
				unsigned short nControllerValue=0;
				int nLen = ParseNumber(pszValue, &nControllerValue, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_CONTROLLER_MACRO_END, PARSE_ERROR_CONTROLLER_VALUE);
				if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1; }	// Some irrevocable error occured

				ControllerEvent evObj((unsigned char) nControllerIndex, (unsigned char) nControllerValue);
				RaiseEvent(&evController, &evObj);

                nReadLen = (int)(pszValue + nLen - szToken);

				Verbose(_T("MusicStringParser::ParseControllerToken: [") << pszKey << _T("]=") << pszValue);
			}
		}

		return nReadLen;

	}

	int MusicStringParser::ParseMeasureToken(TCHAR* szToken, bool* )
	{
		OIL::CEventHandlerArgs args;
		RaiseEvent(&evMeasure, &args);

		Verbose(_T("MusicStringParser::ParseMeasureToken"));
		return 0;
	}

	///<Summary>Parses a Dictionary Element Token. Creates or Updates the Dictionary Element Value</Summary>
	int MusicStringParser::ParseDictionaryToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
	{
		TCHAR* pszAssignSymbol = _tcschr(szToken, ASSIGNMENT_SYMBOL); // Find the Equals sign
		if(pszAssignSymbol == NULL)
		{
			MString str;
			str << ASSIGNMENT_SYMBOL << _T(" Symbol Not found");
            if(Error(PARSE_ERROR_MISSING_ASSIGNMENT, str, szToken)) { *pbNonContinuableErrorOccured = true; return -1; }
			return 0;
		}

		*pszAssignSymbol = _T('\0');

		const TCHAR* pszKey = szToken;
		const TCHAR* pszValue = pszAssignSymbol + 1;

		m_Dictionary[pszKey] = pszValue; // Create or Update the value

		Verbose(_T("MusicStringParser::ParseDictionaryToken: Defined [") << pszKey << _T("]=") << pszValue);

		return (int) (_tcslen(pszKey) + _tcslen(pszValue) + 1);
	}

	/// <Summary> Parses Tokens that look like Music Note symbols. Decides their Root and
	/// Calls the correct method based on the Note nature, such as Numeric Note, Rest Note etc.</Summary>
	int MusicStringParser::ParseNoteToken(TCHAR* szToken, bool* pbNonContinuableErrorOccured)
	{
		NoteContext ctx;

		int nLen = 0, nReadLen = 0;

		while(ctx.existsAnotherNote && *szToken)
		{
			Verbose(_T("MusicStringParser::ParseNote: ") << szToken);

			DecideSequentialOrParallel(ctx);

			nLen = ParseNoteRoot(szToken, ctx);
			if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1;}
			szToken += nLen; nReadLen += nLen;

			nLen = ParseNoteOctave(szToken, ctx);
			if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1;}
			szToken += nLen; nReadLen += nLen;

			nLen = ParseNoteChord(szToken, ctx);
			if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1;}
			szToken += nLen; nReadLen += nLen;

			if(ComputeNoteValue(ctx) == -1) { *pbNonContinuableErrorOccured = true; return -1;}

			nLen = ParseNoteChordInversion(szToken, ctx);
			if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1;}
			szToken += nLen; nReadLen += nLen;

			nLen = ParseNoteDuration(szToken, ctx);
			if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1;}
			szToken += nLen; nReadLen += nLen;

			nLen = ParseNoteVelocity(szToken, ctx);
			if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1;}
			szToken += nLen; nReadLen += nLen;

			nLen = ParseNoteConnector(szToken, ctx);
			if(nLen == -1) { *pbNonContinuableErrorOccured = true; return -1;}
			szToken += nLen; nReadLen += nLen;

			RaiseNoteEvents(ctx);
		}

        //if(szToken[0] != _T('\0')) Warning(MString(_T("Ignoring: ")) + szToken);

		return nReadLen;
	}

	/// <Summary>
	/// Tests if a Note is known to be Sequential (connected with _) or Parallel (connected with +)
	///</Summary>
	void MusicStringParser::DecideSequentialOrParallel(NoteContext& ctx)
	{
        Note::NoteTypes type = Note::FIRST;

		if(ctx.anotherIsSequential)
		{
			type = Note::SEQUENTIAL;
			Verbose(_T("About to read a sequential Note ..."));
		}
		if(ctx.anotherIsParallel)
		{
			type = Note::PARALLEL;
			Verbose(_T("About to read a parallel Note ..."));
		}
        if(type != Note::FIRST)
        {
            ctx = NoteContext(); // We are reading Connected Notes. Reset the Previous Note Context
            ctx.type = type;
        }
	}

	/// <Summary> Parses Numeric, Rest and Letter Notes.
	/// Returns the number of characaters consumed from the string during the parsing </Summary>
	int MusicStringParser::ParseNoteRoot(TCHAR* szToken, NoteContext& ctx)
	{
		switch(szToken[0])
		{
		case MACRO_START : return ParseNumericNote(szToken, ctx);
		case REST_NOTE1:
			{
				Verbose(_T("This Note is a Rest"));
				ctx.isRest = true;
				ctx.numSwaras = 1;
				return 1;
			}
		case REST_NOTE2:
			{
				Verbose(_T("This is a Rest note for Two swara durations"));
				ctx.isRest = true;
				ctx.numSwaras = 2;
				return 1;
			}
        case WESTERN_REST_NOTE: // This Rest Note symbol is valid only for Western Music
            {
                if(m_KeySig.GetMode() == KeySignature::WESTERN)
				{
					Verbose(_T("This Note is a Rest"));

					ctx.isRest = true; return 1;
				}
				break;
            }
		default: break;
		}
		return ParseLetterNote(szToken, ctx);
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
			Verbose(_T("Numeric Note with value: ") << ctx.noteNumber);
		}

		return nLen; // Number of characters consumed
	}

	/// <Summary> Parses Rest Note.
	/// Returns the number of characaters consumed from the string during the parsing </Summary>
	int MusicStringParser::ParseRest(TCHAR* szToken, NoteContext& ctx)
	{
		Verbose(_T("This Note is a Rest"));

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
                    pszToken++; ctx.noteNumber = SWARA_M_Value; // if a note isNatural it will be played as is. No lookups or alterations.
                    if(pszToken[0] == _T('A')) { pszToken++; ctx.numSwaras = 2;  }
                    if(pszToken[0] == _T('1')) { pszToken++; ctx.noteNumber = 5; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('2')) { pszToken++; ctx.noteNumber = 6; ctx.isNatural = true; }
                    break;
                }
		    case SWARA_R:
                {
                    pszToken++; ctx.noteNumber = SWARA_R_Value;
                    if(pszToken[0] == _T('I')) { pszToken++; ctx.numSwaras = 2; }
                    if(pszToken[0] == _T('1')) { pszToken++; ctx.noteNumber = 1; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('2')) { pszToken++; ctx.noteNumber = 2; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('3')) { pszToken++; ctx.noteNumber = 3; ctx.isNatural = true; }
                    break;
                }
		    case SWARA_G:
                {
                    pszToken++; ctx.noteNumber = SWARA_G_Value;
                    if(pszToken[0] == _T('A')) { pszToken++; ctx.numSwaras = 2; }
                    if(pszToken[0] == _T('1')) { pszToken++; ctx.noteNumber = 2; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('2')) { pszToken++; ctx.noteNumber = 3; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('3')) { pszToken++; ctx.noteNumber = 4; ctx.isNatural = true; }
                    break;
                }
		    case SWARA_D:
                {
                    pszToken++; ctx.noteNumber = SWARA_D_Value;
                    if(pszToken[0] == _T('A')) { pszToken++; ctx.numSwaras = 2; }
                    if(pszToken[0] == _T('1')) { pszToken++; ctx.noteNumber = 8; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('2')) { pszToken++; ctx.noteNumber = 9; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('3')) { pszToken++; ctx.noteNumber = 10; ctx.isNatural = true; }
                    break;
                }
		    case SWARA_N:
                {
                    pszToken++; ctx.noteNumber = SWARA_N_Value;
                    if(pszToken[0] == _T('I')) { pszToken++; ctx.numSwaras = 2; }
                    if(pszToken[0] == _T('1')) { pszToken++; ctx.noteNumber = 9; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('2')) { pszToken++; ctx.noteNumber = 10; ctx.isNatural = true; }
                    else if(pszToken[0] == _T('3')) { pszToken++; ctx.noteNumber = 11; ctx.isNatural = true; }
                    break;
                }
		    default:
			    {
                    ctx.isRest = true; // treat the unknown notes as rest notes
					MString str;
					str << _T("Invalid Note: ") << pszToken[0];
				    return Error(PARSE_ERROR_LETTER_NOTE, str, pszToken) ? -1 : 1;
			    }
            }

		    Verbose(_T("Note number within octave: ") << ctx.noteNumber);

		    return (int)(pszToken - szToken); // return the number of characters read
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
				MString str;
				str << _T("Invalid Note: ") << szToken[0];
				return Error(PARSE_ERROR_LETTER_NOTE, str, szToken) ? -1 : 1;
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

		Verbose(_T("Note number within octave: ") << ctx.noteNumber);

		return (int)(pszSymbol - szToken); // return the number of characters read
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

            // TODO: Implement the XOctave (XShruti) and XChord directives; X is already used. Try Z

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
            nLen = (int)(pszSymbol - szToken); // Number of Characters consumed
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
			MString str;
			str << _T("Octave ") << ctx.octaveNumber << _T(" is beyond the range [0, 10]");
			if(Error(PARSE_ERROR_OCTAVE_MAXLIMIT, str, szToken)) return -1; // if we should stop processing any further, return -1

            if(ctx.octaveNumber < 0) ctx.octaveNumber = 0;
            else if(ctx.octaveNumber > 10) ctx.octaveNumber = 10;
        }

		Verbose( _T("Octave Number: ") << ctx.octaveNumber);

		return nLen; // Number of characters consumed here
	}

    int MusicStringParser::ParseNoteChord(TCHAR* szToken, NoteContext& ctx)
    {
        if(ctx.isRest)
        {
            Verbose(_T("ParseNoteChord: Skipping Chords inspection for Rest Note..."));
            return 0;    // No Chords for Rest Notes
        }

        unsigned int nChordNameLen = m_pChords != NULL ?
									 m_pChords->ExtractMatchingChord(szToken, &ctx.chord) :
									 Chords::GetDefaultMatchingChord(szToken, &ctx.chord) ;
        if(nChordNameLen > 0)
        {
            ctx.isChord = true;
            Verbose(_T("ParseNoteChord: Found Chord ") << ctx.chord.szChordName);
        }

        return nChordNameLen;
    }

    int MusicStringParser::ParseNoteChordInversion(TCHAR* szToken, NoteContext& ctx)
    {
        if(ctx.isChord == false)
        {
            Verbose(_T("ParseNoteChordInversion: Skipping Chord Inversions..."));
            return 0;
        }

        TCHAR* pszToken = szToken;

        int nInversionCount = 0;
        int nInversionRootNote = -1;
        int nInversionOctave = -1;
        bool bCheckForInversion = true;

        while(bCheckForInversion && *pszToken != _T('\0'))
        {
            switch(pszToken[0])
            {
            case _T('^'): pszToken++; nInversionCount++; break;
            case _T('C'): pszToken++; nInversionRootNote = NOTE_C_Value; break;
            case _T('D'): pszToken++; nInversionRootNote = NOTE_D_Value; break;
            case _T('E'): pszToken++; nInversionRootNote = NOTE_E_Value; break;
            case _T('F'): pszToken++; nInversionRootNote = NOTE_F_Value; break;
            case _T('G'): pszToken++; nInversionRootNote = NOTE_G_Value; break;
            case _T('A'): pszToken++; nInversionRootNote = NOTE_A_Value; break;
            case _T('B'): pszToken++; // this could be note B or Flat symbol b
                if(nInversionRootNote == -1) nInversionRootNote = NOTE_B_Value;
                else nInversionRootNote--;
                break;
            case _T('#'): pszToken++; nInversionRootNote++; break;
            case _T('1'): pszToken++; nInversionOctave = 1; break;
            case _T('2'): pszToken++; nInversionOctave = 2; break;
            case _T('3'): pszToken++; nInversionOctave = 3; break;
            case _T('4'): pszToken++; nInversionOctave = 4; break;
            case _T('5'): pszToken++; nInversionOctave = 5; break;
            case _T('6'): pszToken++; nInversionOctave = 6; break;
            case _T('7'): pszToken++; nInversionOctave = 7; break;
            case _T('8'): pszToken++; nInversionOctave = 8; break;
            case _T('9'): pszToken++; nInversionOctave = 9; break;
            case _T('0'): pszToken++; // this could be 0th octave or a 0 after a digit
                if(nInversionOctave == -1) nInversionOctave = 0;
                else nInversionOctave *= 10; break;
            case MACRO_START:   // Root note is specified as a Macro number value
                {
                    unsigned short nRoot = 0; bool bSuccess = false;
		            int nLen = ParseNumber(pszToken, &nRoot, bSuccess, MACRO_START, MACRO_END, PARSE_ERROR_CHORDINV_MACRO_END, PARSE_ERROR_CHORDINV_VALUE);
		            if(nLen == -1) { return -1; } // Some irrevocable error occured
		            if(bSuccess)
                    {
                        nInversionRootNote = nRoot;
                        pszToken += nLen;
                        break;
                    }
                    else ; // fall through
                }
            default: bCheckForInversion = false; break;
           }
        } // while

        // Update the values based on Inversion Specifications
        if(nInversionCount > 0)
        {
            if(nInversionRootNote == -1) // if No Root note specified, inversion is based on number of carets
            {
                Verbose(_T("Chord Inversion based on count: ") << nInversionCount);
                // Increase each halfstep before the inversion by 12, the total number of halfsteps in an octave
                ctx.noteNumber += 12;
                for(int i = nInversionCount -1; i < ctx.chord.nIntervalCount; ++i)
                {
                    Verbose(_T("  Inverting ") << ctx.chord.Intervals[i] << _T(" to be ") << (ctx.chord.Intervals[i] - 12));
                    ctx.chord.Intervals[i] -= 12;
                }
            }
            else // Inversion based on root note
            {
                if(nInversionOctave != -1)
                    nInversionRootNote += (nInversionOctave * 12);
                else if(nInversionRootNote < 12)
                {
                    int nCurOctave = ctx.noteNumber / 12; // get the current octave
                    nInversionRootNote += (nCurOctave * 12);
                }

                // Otherwise, InversionRootNote could be a numeric value, such as [60]
                Verbose(_T("Chord Inversion is based on note: ") << nInversionRootNote);

                if((nInversionRootNote  > ctx.noteNumber + ctx.chord.Intervals[ctx.chord.nIntervalCount -1])
                    || (nInversionRootNote < ctx.noteNumber))
                {
                    if(Error(PARSE_ERROR_CHORDINV_MAXLIMIT, _T("Chord Inversion Root Note is beyond range"), szToken))
                        return -1;

                    Verbose(_T("  Adjusted the inversion root note to be ") << ctx.noteNumber <<_T(" and continuing..."));

                    nInversionRootNote = ctx.noteNumber;
                }

                Verbose(_T(" Inverting ") << ctx.noteNumber << _T(" to be ") << (ctx.noteNumber + 12));
                ctx.noteNumber += 12;

                for(int i=0; i < ctx.chord.nIntervalCount; ++ i)
                {
                    if(ctx.noteNumber + ctx.chord.Intervals[i] >= nInversionRootNote + 12)
                    {
                        Verbose(_T("  Inverting ") << ctx.chord.Intervals[i] << _T(" to be ") << (ctx.chord.Intervals[i] - 12));
                        ctx.chord.Intervals[i] -= 12;
                    }
                }
            }
        }

        return (int)(pszToken - szToken); // return the number of characters scanned
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
        //	Warning(_T("Choosing Default Octave:") + OIL::ToString(ctx.octaveNumber));
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

        Verbose(_T("After adjusting for Key, Note Number=") << ctx.noteNumber);

		if(!ctx.isNumeric) // if this is a non-numeric note, compute its value from octave and local note value
		{
			ctx.noteNumber = (ctx.octaveNumber * 12) + ctx.noteNumber;

			Verbose(_T("Computed Note Number:") << ctx.noteNumber);
		}

		// Check the limits
		if(ctx.noteNumber > 127 || ctx.noteNumber < 0)
		{
			MString str;
			str << _T("Computed Note Number") << ctx.noteNumber << _T(" is going beyond the range [0, 127]");
			if(Error(PARSE_ERROR_NOTEVALUE_MAXLIMIT, str, NULL)) return -1; // if we should stop processing any further, return -1

			// if we need to continue despite the error, ceil or floor the value
			if(ctx.noteNumber > 127)
				ctx.noteNumber = 127;
			else if(ctx.noteNumber < 0)
				ctx.noteNumber = 0;

            Warning(_T("  Adjusted the value to") << ctx.noteNumber << _T(" and continuing..."));
		}
		return 0;
	}

	/// <Summary> Parses duration of note </Summary>
	int MusicStringParser::ParseNoteDuration(TCHAR* szToken, NoteContext& ctx)
	{
		TCHAR* pszDuration = szToken;

        if(m_KeySig.GetMode() == KeySignature::WESTERN) // Western Mode
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
				    MString str(_T("Error while reading Numeric Duration Value"));
				    if(Error(PARSE_ERROR_DURATION_VALUE, str, szToken)) return -1;

				    pszDuration = szToken + 1;

				    // If we have to ignore this error, ignore this / character
                    Warning(_T(" Ignoring / and proceeding with ") << pszDuration);
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

		Verbose(_T("Decimal Duration is: ") << ctx.decimalDuration);

		// Scan for any optional Tuplet specifications
		int nTupletLen = ParseTuplet(pszDuration, ctx);
		if(nTupletLen < 0) return -1; // some irrevocable error happened

		ctx.duration = (long) (128.0 * ctx.decimalDuration);	//TODO: Compute the duration based on the current Tempo

		Verbose(_T("Actual Duration is: ") << ctx.duration);

		return (int)(pszDuration - szToken) + nTupletLen; // Return the number of characters consumed
	}

	/// <Summary> Parses duration expressed as Alphabet W, H etc.. </Summary>
	int MusicStringParser::ParseLetterDuration(TCHAR* szToken, NoteContext& ctx)
	{
		TCHAR* pszDuration = szToken;

		while(*pszDuration != _T('\0'))
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

		return (int)(pszDuration - szToken); // return the number of characters consumed
	}

	/// <Summary> Parses tuples expressed with * : </Summary>
	int MusicStringParser::ParseTuplet(TCHAR* szToken, NoteContext& ctx)
	{
		if(szToken[0] == NOTE_TUPLET_START)
		{
			Verbose(_T("Scanning a Tuplet..."));

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
                    Warning(_T("  Defaulting to Numerator Value: ") << dNumerator);
				}

				int nLen = LoadValueFromString(pszTupletRatioMark+1, &dDenominator); // Read the Denominator
				if(nLen == 0)
				{
					MString str(_T("Error while reading Denominator of tuplet"));
					if(Error(PARSE_ERROR_TUPLET_DENOMINATOR, str, pszTupletRatioMark+1)) return -1;
                    Warning(_T("  Defaulting to Denominator Value: ") << dDenominator);
				}

				pszTuplet = pszTupletRatioMark + 1 + nLen;
			}

			double tupletRatio = dNumerator / dDenominator;

			ctx.decimalDuration *= tupletRatio;

			Verbose(_T("Tuplet Ratio is: ") << (unsigned int)dNumerator << _T(":") << (unsigned int)dDenominator);
			Verbose(_T("Decimal Duration after applying Tuplet: ") << ctx.decimalDuration);

			return (int)(pszTuplet - szToken); // return number of characters consumed
		}

		return 0; // This is not a tuplet
	}


	/// <brief> Parses Attack and Decay velocities of note </brief>
    /// <Summary>
    /// Attack and Decay velocities come in the note token after the note duration.
    /// Typically specified by V&lt;attack&gt;V&lt;decay&gt; 
    /// For example, CV20V80 indicates Mid-octave C note with attack velocity 20 and decay velocity 80.
    /// If the number is skipped after the V then default value of 64 will be used. 
    /// For example, DVV80 indicates Mid-octave D note with default attack velocity 64 and decay velocity of 80
    /// </Summary>
	int MusicStringParser::ParseNoteVelocity(TCHAR* szToken, NoteContext& ctx)
	{
		if(ctx.isRest)
		{
			Verbose(_T("This is a Rest note .. Skipping the Velocities"));
			return 0;
		}

		TCHAR* pszVelocity = szToken; bool bAttackRead = false;

		do
		{
			bool bAttack = false, bDecay = false;

			switch(pszVelocity[0])
			{
            case NOTE_VELOCITY: bAttack = !bAttackRead; bDecay = bAttackRead; bAttackRead = bAttack; break;
            case NOTE_VELOCITY_ATTACK: if(m_KeySig.GetMode()==KeySignature::WESTERN) { bAttack = true; break; } // Compatibility with JFugue only in Wester mode
            case NOTE_VELOCITY_DECAY: if(m_KeySig.GetMode()==KeySignature::WESTERN) { bDecay = true; break; }
			default:
				{
					Verbose(_T("Attack Velocity: ") << (int)ctx.attackVelocity << _T(" Decay Velocity: ") << (int)ctx.decayVelocity);
					return (int)(pszVelocity - szToken);
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
            else Warning(_T("Unable to find Attack/Decay velocity for note, while parsing: ") <<  szToken);

			pszVelocity += nLen;
		}while(true);	// we can have both attack and decay specified for the same note. so loop again.

		return (int)(pszVelocity - szToken);
	}

	/// <Summary> Checks & Parses any associated notes </Summary>
	int MusicStringParser::ParseNoteConnector(TCHAR* szToken, NoteContext& ctx)
	{
		ctx.existsAnotherNote = true; // we start with assumpion that there are connectors

		switch(szToken[0])
		{
		case NOTE_CONNECTOR_SEQUENTIAL: ctx.anotherIsSequential = true; return 1;
		case NOTE_CONNECTOR_PARALLEL: ctx.anotherIsParallel = true; return 1;
		default: Verbose(_T("No Connector Notes Found in: ") << szToken); break;
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
            for(unsigned short i=0; i < ctx.chord.nIntervalCount; ++i)
			{
				Note noteObj;
                noteObj.noteNumber = ctx.noteNumber + ctx.chord.Intervals[i];
				noteObj.duration = ctx.duration;
				noteObj.decimalDuration = ctx.decimalDuration;
				noteObj.type = Note::PARALLEL;
				RaiseEvent(&evNote, &noteObj);
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
		const TCHAR* pszVal = (dictEntry == m_Dictionary.end() ? szKey : (const TCHAR*)dictEntry->second);

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
				MString str;
				str << _T("No Matching ") << MacroBracketEnd << _T(" found for ") << MacroBracketStart;
				if(Error(MacroReadErrorCode, str, szToken)) return -1;

				// if we have to ignore this error, we ignore this [ character and continue
				pszNumberStart = szToken+1;

                Warning(_T("  Ignoring ") << MacroBracketStart);

                return 1; // Failure; return the lentgh scanned
			}
			else // We found an End Bracket
			{
				*pszEndBracket = _T('\0');

				if(false == GetValueFromDictionary(szToken + 1, pRetVal)) // Convert the string between [ and ] to Number
				{
					MString str;
					str << _T("Unable to retrieve Number from ") << (szToken + 1);
					if(Error(NumberParseErrorCode, str, szToken)) return -1;

					// if we have to ignore this error, we ignore the whole string between the [ and ]
                    Warning(_T("  Ignoring ") << (szToken + 1));

					return (int)(pszEndBracket - szToken) +1; // Failure; return the length consumed
				}
				else // The Macro got read successfully
				{
					bSuccess = true;

					return (int)(pszEndBracket - szToken) +1; // return the number of characters scanned
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

		return (int)(pszNumberStart - szToken) + nStrLen; // return the number of characters consumed
	}

} // namespace CFugue

