/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://cfugue.sourceforge.net/>.
*/

#include "stdafx.h"

#include "ParserListener.h"
#include "Parser.h"

namespace CFugue
{
    void CParser::AddListener(CParserListener* pListener)
    {
        //TODO: Add All Listeners
        //evController;
        //evMeasure;
        //evChannelPressure;
        //evPolyphonicPressure;
        //evPitchBend;
        evInstrument.Subscribe(pListener, &CParserListener::OnInstrumentEvent);
        evKeySignature.Subscribe(pListener, &CParserListener::OnKeySignatureEvent);
        evLayer.Subscribe(pListener, &CParserListener::OnLayerEvent); // Parser encountered a Layer command
        evTempo.Subscribe(pListener, &CParserListener::OnTempoEvent); // Parser encountered a Tempo command
        evTime.Subscribe(pListener, &CParserListener::OnTimeEvent);   // Parser encountered a Time command
        evVoice.Subscribe(pListener, &CParserListener::OnVoiceEvent); // Parser encountered a Voice command
        evNote.Subscribe(pListener, &CParserListener::OnNoteEvent);
        //evSequentialNote; // Encountered a Sequential note after a first note
        //evParalleNote; // Encountered a Parallel note after a first note
    }

    void CParser::RemoveListener(CParserListener* pListener)
    {
        //evController;
        //evMeasure;
        //evChannelPressure;
        //evPolyphonicPressure;
        //evPitchBend;
        evInstrument.UnSubscribe(pListener);
        evKeySignature.UnSubscribe(pListener);
        evLayer.UnSubscribe(pListener); 
        evTempo.UnSubscribe(pListener);
        evTime.UnSubscribe(pListener);
        evVoice.UnSubscribe(pListener);
        evNote.UnSubscribe(pListener);
        //evSequentialNote; // Encountered a Sequential note after a first note
        //evParalleNote; // Encountered a Parallel note after a first note
    }

} // namespace 