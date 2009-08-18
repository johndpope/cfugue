#ifndef MIDIRENDERER_H__9266AE56_84CB_4662_8328_ED088111CFE0__
#define MIDIRENDERER_H__9266AE56_84CB_4662_8328_ED088111CFE0__

#include "jdkmidi/sequencer.h"
#include "jdkmidi/manager.h"
#include "jdkmidi/driver.h"
#include "jdkmidi/driverwin32.h"
#include "ParserListener.h"
#include "MIDIEventManager.h"

namespace MusicNoteLib
{
	class MIDIRenderer : public CParserListener
	{   
		jdkmidi::MIDIDriverWin32 m_MIDIDriver;
    
		jdkmidi::MIDIManager m_MIDIManager;

		MIDIEventManager m_MIDIEventManager;

		long m_lFirstNoteTime;

	public:

		MIDIRenderer(void) :
			m_MIDIDriver(128), m_MIDIManager(&m_MIDIDriver)
		{
		}

		~MIDIRenderer(void)
		{
		}

		inline virtual void OnNoteEvent(const CParser* pParser, Note* pNote)
		{
			if(pNote->duration == 0) return;

			if(pNote->isRest)	// if this is a rest note, simply advance the track timer
			{
				m_MIDIEventManager.AdvanceTrackTime(pNote->duration);
			}

			if(pNote->type == pNote->FIRST) // if this is the first note save its track time. Useful for any later parallel notes
				m_lFirstNoteTime = m_MIDIEventManager.GetTrackTime();

			if(pNote->type == pNote->PARALLEL) // if this is a parallel note, use the same start time as the last seen first note
				m_MIDIEventManager.SetTrackTime(m_lFirstNoteTime);

			m_MIDIEventManager.AddNoteEvent(pNote->noteNumber, pNote->attackVelocity, pNote->decayVelocity, pNote->duration, !pNote->isEndOfTie, !pNote->isStartOfTie);
		}

		inline virtual void BeginRenderAsync()
		{
			m_MIDIDriver.StartTimer(20);
			m_MIDIDriver.OpenMIDIOutPort(MIDI_MAPPER);
			m_MIDIEventManager.GetSequencer()->GoToZero();
			m_MIDIManager.SetSeq(m_MIDIEventManager.GetSequencer());
			m_MIDIManager.SetTimeOffset(timeGetTime());
			m_MIDIManager.SeqPlay();
		}

		inline virtual void EndRenderAsync()
		{
			m_MIDIManager.SeqStop();
		}
	};

} // namespace MusicNoteLib

#endif MIDIRENDERER_H__9266AE56_84CB_4662_8328_ED088111CFE0__