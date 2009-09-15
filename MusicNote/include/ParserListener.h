#ifndef __PARSERLISTENER_H__D2514D7C_F971_49d6_B8E0_E892725FEB73__
#define __PARSERLISTENER_H__D2514D7C_F971_49d6_B8E0_E892725FEB73__

#include "EventHandler.h"

namespace MusicNoteLib
{
	//Forward Declarations
	class CParser;
	class Instrument;
    class KeySignature;
    class Layer;
    class Tempo;
    class Time;
    class Voice;
	struct Note;

    /// <Summary>
    /// \brief Base class that represents a Renderer Object
    /// </Summary>
	class CParserListener : public OIL::CEventReceiver
	{
	public:

		inline CParserListener(void) {	}

		inline virtual ~CParserListener(void)	{	}

		inline virtual void OnInstrumentEvent(const CParser* pParser, Instrument* pInstrument) { } 

		inline virtual void OnKeySignatureEvent(const CParser* pParser, KeySignature* pKeySig) { } 

		inline virtual void OnLayerEvent(const CParser* pParser, Layer* pLayer) { } 

		inline virtual void OnTempoEvent(const CParser* pParser, Tempo* pTempo) { } 

		inline virtual void OnTimeEvent(const CParser* pParser, Time* pTime) { } 

		inline virtual void OnVoiceEvent(const CParser* pParser, Voice* pVoice) { } 

		inline virtual void OnNoteEvent(const CParser* pParser, Note* pNote) { } 
	};

} // namespace MusicNoteLib

#endif // __PARSERLISTENER_H__D2514D7C_F971_49d6_B8E0_E892725FEB73__