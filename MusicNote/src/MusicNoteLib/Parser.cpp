#include "stdafx.h"

#include "ParserListener.h"
#include "Parser.h"

namespace MusicNoteLib
{
    ///<Summary> Subscribes a Listener object for all events </Summary>
    void CParser::AddListener(CParserListener* pListener)
    {
        //TODO: Add All Listeners
        //evController;
        //evKeySignature;
        //evLayer; // Parser encountered a Layer command
        //evMeasure;
        //evChannelPressure;
        //evPolyphonicPressure;
        //evPitchBend;
        //evTime;
        evInstrument.Subscribe(pListener, &CParserListener::OnInstrumentEvent);
        evTempo.Subscribe(pListener, &CParserListener::OnTempoEvent); // Parser encountered a Tempo command
        evVoice.Subscribe(pListener, &CParserListener::OnVoiceEvent); // Parser encountered a Voice command
        evNote.Subscribe(pListener, &CParserListener::OnNoteEvent);
        //evSequentialNote; // Encountered a Sequential note after a first note
        //evParalleNote; // Encountered a Parallel note after a first note
    }

    ///<Summary> UnSubscribes the Listener Object from the events </Summary>
    void CParser::RemoveListener(CParserListener* pListener)
    {
        //evController;
        //evKeySignature;
        //evLayer; // Parser encountered a Layer command
        //evMeasure;
        //evChannelPressure;
        //evPolyphonicPressure;
        //evPitchBend;
        //evTime;
        evInstrument.UnSubscribe(pListener);
        evTempo.UnSubscribe(pListener);
        evVoice.UnSubscribe(pListener);
        evNote.UnSubscribe(pListener);
        //evSequentialNote; // Encountered a Sequential note after a first note
        //evParalleNote; // Encountered a Parallel note after a first note
    }

} // namespace 