/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __MIDIEVENTMANAGER_H__74C2A3BA_DFCF_4048_BC1D_20E9E04E809A__
#define __MIDIEVENTMANAGER_H__74C2A3BA_DFCF_4048_BC1D_20E9E04E809A__

#include "jdkmidi/sequencer.h"

namespace MusicNoteLib
{
    /// <Summary> Takes care of MIDI Events, Tracks and Sequencing </Summary>
    class MIDIEventManager
    {
    protected:
	    unsigned short m_nCurrentTrack;
	    unsigned short m_nCurrentLayer;

	    enum { MAX_CHANNELS = 16, MAX_LAYERS = 16 };

	    unsigned short m_CurrentLayer[MAX_CHANNELS];
	    unsigned long m_Time[MAX_CHANNELS][MAX_LAYERS];

	    jdkmidi::MIDIMultiTrack m_Tracks;

	    jdkmidi::MIDISequencer m_Sequencer;

    public:

	    inline MIDIEventManager(void) : m_Sequencer(&m_Tracks)
	    {
		    Clear();
		    m_Tracks.SetClksPerBeat(24); //TODO: Correct this
	    }

	    inline ~MIDIEventManager(void)
	    {
	    }

	    /// <Summary> Returns the Sequencer holding the collection of tracks </Summary>
	    inline jdkmidi::MIDISequencer* GetSequencer() { return &m_Sequencer; }

	    /// <Summary> Returns the Multitrack object </Summary>
	    inline jdkmidi::MIDIMultiTrack* GetTracks() { return &m_Tracks; }

	    /// <Summary>
	    /// Clears all the Events stored in the tracks and Resets the Track Timers
	    /// </Summary>
	    inline void Clear()
	    {
		    memset(m_CurrentLayer, 0, sizeof(m_CurrentLayer));
		    memset(m_Time, 0, sizeof(m_Time));
		    m_nCurrentTrack = 0;
		    m_nCurrentLayer = 0;
		    m_Tracks.Clear();
            m_Sequencer.ResetAllTracks();
	    }

	    /// <Summary>Sets the current Track/Channel to which new Events will be added
	    /// </Summary>
	    inline void SetCurrentTrack(unsigned short nTrack)
	    {
		    m_nCurrentTrack = nTrack;
	    }

	    inline void SetCurrentLayer(unsigned short nLayer)
	    {
		    m_CurrentLayer[m_nCurrentTrack] = nLayer;
	    }

	    /// <Summary>
	    /// Returns the Timer value for the current track (in Pulses Per Quarter)
	    /// </Summary>
	    inline unsigned long GetTrackTime() const
	    {
		    return m_Time[m_nCurrentTrack][m_CurrentLayer[m_nCurrentTrack]];
	    }

	    /// <Summary>
	    /// Sets the current Track's time to the supplied new time (specified in Pulses Per Quarter)
	    /// </Summary>
	    inline void SetTrackTime(unsigned long lNewTime)
	    {
		    m_Time[m_nCurrentTrack][m_CurrentLayer[m_nCurrentTrack]] = lNewTime;
	    }

	    /// <Summary>
	    /// Advances the timer for the current Track by the duration (specified in Pulses Per Quarter)
	    /// </Summary>
	    inline void AdvanceTrackTime(unsigned long lDuration)
	    {
		    m_Time[m_nCurrentTrack][m_CurrentLayer[m_nCurrentTrack]] += lDuration;
	    }

		/// <Summary>
		/// Adds a Channel Pressure event to the current track
		/// @param uPressure the pressure value that should be applied to the notes on the current channel
		/// </Summary>
		inline void AddChannelPressureEvent(unsigned char uPressure)
		{
			jdkmidi::MIDITimedBigMessage msg;
			msg.SetTime(GetTrackTime());
			msg.SetChannelPressure((unsigned char)m_nCurrentTrack, uPressure);
			m_Tracks.GetTrack(m_nCurrentTrack)->PutEvent(msg);
		}

	    /// <Summary>
	    /// Adds a MIDI controller event to the current track.
        /// @param uControlIndex the controller 
        /// @param uControlValue value for the controller
	    /// </Summary>
		inline void AddControllerEvent(unsigned char uControlIndex, unsigned char uControlValue)
		{
			jdkmidi::MIDITimedBigMessage msg;
			msg.SetTime(GetTrackTime());
			msg.SetControlChange((unsigned char)m_nCurrentTrack, uControlIndex, uControlValue);
			m_Tracks.GetTrack(m_nCurrentTrack)->PutEvent(msg);
		}

	    /// <Summary>
	    /// Adds a short Key Singature event to the current track.
        /// @param nKeySig a value in the range [-7, 7]
        /// @param MajMin indicates if this is Major (0) scale or Min (1) scale
	    /// </Summary>
        inline void AddKeySignatureEvent(signed char nKeySig, unsigned char MajMin)
        {
            jdkmidi::MIDITimedBigMessage msg;
            msg.SetTime(GetTrackTime());
            msg.SetKeySig(nKeySig, MajMin);
            m_Tracks.GetTrack(m_nCurrentTrack)->PutEvent(msg);
        }

		/// <Summary>
		/// Adds a PitchBend event to the current track.
		/// @param uLowByte the low byte value of the pitch bend
		/// @param uHighByte the high byte value of the pitch bend
		/// </Summary>
		inline void AddPitchBendEvent(unsigned char uLowByte, unsigned char uHighByte)
		{
			jdkmidi::MIDITimedBigMessage msg;
			msg.SetTime(GetTrackTime());
			msg.SetPitchBend((unsigned char)m_nCurrentTrack, uLowByte, uHighByte);
			m_Tracks.GetTrack(m_nCurrentTrack)->PutEvent(msg);
		}

		/// <Summary>
		/// Adds a key pressure event to the current track.
		/// @param uKey the note the pressure should be applied to
		/// @param uPressure the Pressure value that should be applied to the key
		/// </Summary>
		inline void AddPolyphonicPressureEvent(unsigned char uKey, unsigned char uPressure)
		{
			jdkmidi::MIDITimedBigMessage msg;
			msg.SetTime(GetTrackTime());
			msg.SetPolyPressure((unsigned char)m_nCurrentTrack, uKey, uPressure);
			m_Tracks.GetTrack(m_nCurrentTrack)->PutEvent(msg);
		}

	    /// <Summary>
	    /// Adds a short ProgramChange message event to the current track.
	    /// @param nInstrumentID indicates the selected instrument that should be used for playing further notes;
	    /// </Summary>
        inline void AddProgramChangeEvent(unsigned char nInstrumentID)
        {
            jdkmidi::MIDITimedBigMessage msg;
            msg.SetTime(GetTrackTime());
            msg.SetProgramChange((unsigned char)m_nCurrentTrack, nInstrumentID);
            m_Tracks.GetTrack(m_nCurrentTrack)->PutEvent(msg);
        }

	    /// <Summary>
	    /// Adds a short Tempo message event to the current track.
	    /// @param nTempo indicates the selected Tempo value in BPM that should be used for playing further notes;
	    /// </Summary>
        inline void AddTempoEvent(unsigned short nTempo)
        {
            jdkmidi::MIDITimedBigMessage msg;
            msg.SetTime(GetTrackTime());
            msg.SetTempo32(nTempo * 32); // Line 415 of jdkmidi_sequencer.cpp indicates 1/32 th part. So we compensate it here with *32
            m_Tracks.GetTrack(m_nCurrentTrack)->PutEvent(msg);
        }

	    /// <Summary>
	    /// Adds a short NoteOn message event to the current track using attack/decay velocities. Automatically
	    /// adds a NoteOff message using the specified duration. These NoteOn and NoteOff can be suppressed (useful 
	    /// when notes are tied).
        /// @param noteValue the MIDI note value. For example, Mid-C = 60
        /// @param attackVel the attack Velocity of the note
        /// @param decayVel the decay Velocity of the note
        /// @param lNoteDuration duration of the MIDI note
	    /// @param addNoteOn indicates if NoteOn event should be created automatically. For the end of a tied note, this should be false; otherwise true;
	    /// @param addNoteOff indicates if NoteOff event should be created automatically. For the start of a tied note, this should be false; otherwise true;	
	    /// </Summary>
	    inline void AddNoteEvent(int noteValue, int attackVel, int decayVel, long lNoteDuration, bool addNoteOn, bool addNoteOff)
	    {
		    if(addNoteOn)
		    {
			    jdkmidi::MIDITimedBigMessage msg;

			    msg.SetTime(GetTrackTime());

			    msg.SetNoteOn((unsigned char)m_nCurrentTrack, (unsigned char) noteValue, (unsigned char) attackVel);
    			
			    m_Tracks.GetTrack(m_nCurrentTrack)->PutEvent(msg);
		    }

		    AdvanceTrackTime(lNoteDuration);

		    if(addNoteOff)
		    {
			    jdkmidi::MIDITimedBigMessage msg;

			    msg.SetTime(GetTrackTime());
    			
			    msg.SetNoteOff((unsigned char)m_nCurrentTrack, (unsigned char)noteValue, (unsigned char)decayVel);

			    m_Tracks.GetTrack(m_nCurrentTrack)->PutEvent(msg);
		    }
	    }
    };

} // namespace MusicNoteLib 

#endif // __MIDIEVENTMANAGER_H__74C2A3BA_DFCF_4048_BC1D_20E9E04E809A__