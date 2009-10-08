#ifndef MIDIRENDERER_H__9266AE56_84CB_4662_8328_ED088111CFE0__
#define MIDIRENDERER_H__9266AE56_84CB_4662_8328_ED088111CFE0__

#include "jdkmidi/manager.h"
#include "jdkmidi/driverwin32.h"
#include "ParserListener.h"
#include "MIDIEventManager.h"

namespace MusicNoteLib
{
	///<Summary>Takes care of Rendering MIDI Output either to a file or to a MIDI out Port</Summary>
	class MIDIRenderer : MIDIEventManager, public CParserListener
	{   
		jdkmidi::MIDIDriverWin32 m_MIDIDriver;
    
		jdkmidi::MIDIManager m_MIDIManager;

		// MIDIEventManager m_MIDIEventManager;

		long m_lFirstNoteTime;

		/// <Summary>Event handler for Instrument event Raised by Parser</Summary>
        virtual void OnInstrumentEvent(const CParser* pParser, const Instrument* pInstrument);

		/// <Summary>Event handler for KeySignature event Raised by Parser</Summary>
        virtual void OnKeySignatureEvent(const CParser* pParser, const KeySignature* pKeySig);

		/// <Summary>Event handler for Layer event Raised by Parser</Summary>
        virtual void OnLayerEvent(const CParser* pParser, const Layer* pLayer);

        /// <Summary>Event handler for Tempo event Raised by Parser </Summary>
        virtual void OnTempoEvent(const CParser* pParser, const Tempo* pTempo);

        /// <Summary>Event handler for Time event Raised by Parser </Summary>
        virtual void OnTimeEvent(const CParser* pParser, const Time* pTime);

		/// <Summary>Event handler for Voice event Raised by Parser</Summary>
        virtual void OnVoiceEvent(const CParser* pParser, const Voice* pVoice);

		/// <Summary>Event handler for Note event Raised by Parser</Summary>
		virtual void OnNoteEvent(const CParser* pParser, const Note* pNote);

	public:

		inline MIDIRenderer(void) :
			m_MIDIDriver(128), m_MIDIManager(&m_MIDIDriver)
		{
		}

		inline ~MIDIRenderer(void)
		{
            Clear();
		}

		/// <Summary>
		/// Clears the current state of the Renderer. Empties all the stored tracks and notes.
		/// </Summary>
		inline void Clear()
		{
            EndPlayAsync(); // Stop any current Play in progress
            m_lFirstNoteTime = 0;
            MIDIEventManager::Clear(); // Clear the Track content
		}

		/// <Summary>
		/// Starts Rendering the MIDI output to MIDI port.
		/// Each BeingPlayAsync call should have a matching EndPlayAsync call (no matter success or failure).
		/// nMIDIOutPortID is the ID of the MIDI output port to open for play.
		/// nTimerResolutionMS is the required minimum resolution for the MIDI timer (in MilliSeconds).
		/// Returns false if Unable to open MIDI port or unable to create timer with specified resolution.
		/// </Summary>
		inline bool BeginPlayAsync(int nMIDIOutPortID = MIDI_MAPPER, unsigned int nTimerResolutionMS = 20)
		{
			m_Sequencer.GoToZero();
			m_MIDIManager.SetSeq(&m_Sequencer);
			if(m_MIDIDriver.OpenMIDIOutPort(nMIDIOutPortID))
			{
				if(m_MIDIDriver.StartTimer(nTimerResolutionMS))
				{
					m_MIDIManager.SetTimeOffset(timeGetTime());
					m_MIDIManager.SeqPlay();
					return true;
				}
			}
			return false;
		}

		/// <Summary>
		/// Stops Rendering the MIDI output to MIDI port.
		/// Each BeingPlayAsync call should have a matching EndPlayAsync call.
		/// </Summary>
		inline void EndPlayAsync()
		{
			m_MIDIDriver.StopTimer();
			m_MIDIManager.SeqStop();
			m_MIDIDriver.CloseMIDIOutPort();
		}

		/// <Summary>
		/// Indicates if the MIDI output is still being rendered (to MIDI Port) or finished.
		/// </Summary>
		inline bool IsPlaying() const { return m_MIDIManager.IsSeqPlay(); }

        /// <Summary>
        /// Saves the current track/sequencer content to a MIDI Output file
        /// </Summary>
		bool SaveToFile(const char* szOutputFilePath);
	};

} // namespace MusicNoteLib

#endif MIDIRENDERER_H__9266AE56_84CB_4662_8328_ED088111CFE0__