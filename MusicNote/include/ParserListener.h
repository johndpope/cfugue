/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __PARSERLISTENER_H__D2514D7C_F971_49d6_B8E0_E892725FEB73__
#define __PARSERLISTENER_H__D2514D7C_F971_49d6_B8E0_E892725FEB73__

#include "Common/EventHandler.h"

namespace MusicNoteLib
{
	//Forward Declarations
	class CParser;
	class ChannelPressure;
	class ControllerEvent;
	class Instrument;
    class KeySignature;
    class Layer;
	class PitchBend;
	class PolyphonicPressure;
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

		inline virtual void OnChannelPressureEvent(const CParser* pParser, const ChannelPressure* pCP) { }

		inline virtual void OnControllerEvent(const CParser* pParser, const ControllerEvent* pCEvent) { }

		inline virtual void OnInstrumentEvent(const CParser* pParser, const Instrument* pInstrument) { } 

		inline virtual void OnKeySignatureEvent(const CParser* pParser, const KeySignature* pKeySig) { } 

		inline virtual void OnLayerEvent(const CParser* pParser, const Layer* pLayer) { } 

		inline virtual void OnMeasureEvent(const CParser* pParser, OIL::CEventHandlerArgs* pArgs) { }

		inline virtual void OnPitchBendEvent(const CParser* pParser, const PitchBend* pPB) { }

		inline virtual void OnPolyphonicPressureEvent(const CParser* pParser, const PolyphonicPressure* pPressure) { }

		inline virtual void OnTempoEvent(const CParser* pParser, const Tempo* pTempo) { } 

		inline virtual void OnTimeEvent(const CParser* pParser, const Time* pTime) { } 

		inline virtual void OnVoiceEvent(const CParser* pParser, const Voice* pVoice) { } 

		inline virtual void OnNoteEvent(const CParser* pParser, const Note* pNote) { } 
	};

} // namespace MusicNoteLib

#endif // __PARSERLISTENER_H__D2514D7C_F971_49d6_B8E0_E892725FEB73__