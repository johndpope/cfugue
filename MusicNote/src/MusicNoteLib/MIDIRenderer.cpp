#include "stdafx.h"
#include "jdkmidi/filewritemultitrack.h"
#include "MIDIRenderer.h"
#include "Note.h"
#include "ChannelPressure.h"
#include "ControllerEvent.h"
#include "Instrument.h"
#include "KeySignature.h"
#include "Layer.h"
#include "PitchBend.h"
#include "PolyphonicPressure.h"
#include "Tempo.h"
#include "TimeToken.h"
#include "Voice.h"

namespace MusicNoteLib
{
    void MIDIRenderer::WaitTillDone()
    {
#if defined WIN32 || defined _WIN32
		while(IsPlaying()) Sleep(500);
#else
        m_MIDIDriver.WaitTillDone();
#endif
    }

    bool MIDIRenderer::SaveToFile(const char* szOutputFilePath)
	{
		jdkmidi::MIDIFileWriteStreamFileName outFile(szOutputFilePath);

		if(outFile.IsValid() == false) return false;

		jdkmidi::MIDIFileWriteMultiTrack WriterObj(&m_Tracks, &outFile);

		return WriterObj.Write();
	}

	void MIDIRenderer::OnChannelPressureEvent(const CParser* pParser, const ChannelPressure* pCP)
	{
		AddChannelPressureEvent(pCP->GetPressure());
	}

	void MIDIRenderer::OnControllerEvent(const CParser* pParser, const ControllerEvent* pConEvent)
	{
		AddControllerEvent(pConEvent->GetControl(), pConEvent->GetValue());
	}

    void MIDIRenderer::OnInstrumentEvent(const CParser* pParser, const Instrument* pInstrument)
    {
        AddProgramChangeEvent(pInstrument->GetInstrumentID());
    }

    void MIDIRenderer::OnKeySignatureEvent(const CParser* pParser, const KeySignature* pKeySig)
    {
        AddKeySignatureEvent(pKeySig->GetKey(), pKeySig->GetMajMin());
    }

    void MIDIRenderer::OnLayerEvent(const CParser* pParser, const Layer* pLayer)
    {
        SetCurrentLayer(pLayer->GetLayer());
    }

	void MIDIRenderer::OnPitchBendEvent(const CParser* pParser, const PitchBend* pPB)
	{
		AddPitchBendEvent(pPB->GetLowByte(), pPB->GetHighByte());
	}

	void MIDIRenderer::OnPolyphonicPressureEvent(const CParser* pParser, const PolyphonicPressure* pPressure)
	{
		AddPolyphonicPressureEvent(pPressure->GetKey(), pPressure->GetPressure());
	}

    void MIDIRenderer::OnTempoEvent(const CParser* pParser, const Tempo* pTempo)
    {
        AddTempoEvent(pTempo->GetTempo());
    }

    void MIDIRenderer::OnTimeEvent(const CParser* pParser, const Time* pTime)
    {
        SetTrackTime(pTime->GetTime());
    }

    void MIDIRenderer::OnVoiceEvent(const CParser* pParser, const Voice* pVoice)
    {
        SetCurrentTrack(pVoice->GetVoice());
    }

    void MIDIRenderer::OnNoteEvent(const CParser* pParser, const Note* pNote)
    {
        if(pNote->duration == 0) return;

        if(pNote->isRest)	// if this is a rest note, simply advance the track timer
        {
            AdvanceTrackTime(pNote->duration); return;
        }

        if(pNote->type == pNote->FIRST) // if this is the first note save its track time. Useful for any later parallel notes
            m_lFirstNoteTime = GetTrackTime();

        if(pNote->type == pNote->PARALLEL) // if this is a parallel note, use the same start time as the last seen first note
            SetTrackTime(m_lFirstNoteTime);

        AddNoteEvent(pNote->noteNumber, pNote->attackVelocity, pNote->decayVelocity, pNote->duration, !pNote->isEndOfTie, !pNote->isStartOfTie);
    }


} // namespace MusicNoteLib
