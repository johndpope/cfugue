#include "stdafx.h"
#include "jdkmidi/filewritemultitrack.h"
#include "MIDIRenderer.h"
#include "Note.h"
#include "Instrument.h"
#include "Tempo.h"
#include "Voice.h"

namespace MusicNoteLib
{
    bool MIDIRenderer::SaveToFile(const char* szOutputFilePath)
	{
		jdkmidi::MIDIFileWriteStreamFileName outFile(szOutputFilePath);

		if(outFile.IsValid() == false) return false;
				
		jdkmidi::MIDIFileWriteMultiTrack WriterObj(&m_Tracks, &outFile);

		return WriterObj.Write();
	}

    void MIDIRenderer::OnInstrumentEvent(const CParser* pParser, Instrument* pInstrument)
    {
        AddProgramChangeEvent(pInstrument->GetInstrumentID());
    }

    void MIDIRenderer::OnTempoEvent(const CParser* pParser, Tempo* pTempo)
    {
        AddTempoEvent(pTempo->GetTempo());
    }

    void MIDIRenderer::OnVoiceEvent(const CParser* pParser, Voice* pVoice)
    {
        SetCurrentTrack(pVoice->GetVoice());
    }

    void MIDIRenderer::OnNoteEvent(const CParser* pParser, Note* pNote)
    {
        if(pNote->duration == 0) return;

        if(pNote->isRest)	// if this is a rest note, simply advance the track timer
        {
            AdvanceTrackTime(pNote->duration);
        }

        if(pNote->type == pNote->FIRST) // if this is the first note save its track time. Useful for any later parallel notes
            m_lFirstNoteTime = GetTrackTime();

        if(pNote->type == pNote->PARALLEL) // if this is a parallel note, use the same start time as the last seen first note
            SetTrackTime(m_lFirstNoteTime);

        AddNoteEvent(pNote->noteNumber, pNote->attackVelocity, pNote->decayVelocity, pNote->duration, !pNote->isEndOfTie, !pNote->isStartOfTie);
    }


} // namespace MusicNoteLib