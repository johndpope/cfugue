#ifndef __PARSERLISTENER_H__D2514D7C_F971_49d6_B8E0_E892725FEB73__
#define __PARSERLISTENER_H__D2514D7C_F971_49d6_B8E0_E892725FEB73__

#include "EventHandler.h"

namespace MusicNoteLib
{
	//Forward Declarations
	class CParser;
	class Instrument;
    class Tempo;
    class Voice;
	struct Note;

	class CParserListener : public OIL::CEventReceiver
	{
	public:

		inline CParserListener(void) {	}

		inline virtual ~CParserListener(void)	{	}

		inline virtual void OnInstrumentEvent(const CParser* pParser, Instrument* pInstrument) { } 

		inline virtual void OnTempoEvent(const CParser* pParser, Tempo* pTempo) { } 

		inline virtual void OnVoiceEvent(const CParser* pParser, Voice* pVoice) { } 

		inline virtual void OnNoteEvent(const CParser* pParser, Note* pNote) { } 
	};

} // namespace MusicNoteLib

#endif // __PARSERLISTENER_H__D2514D7C_F971_49d6_B8E0_E892725FEB73__