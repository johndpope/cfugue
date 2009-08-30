#ifndef __MIDIEVENTMANAGER_H__74C2A3BA_DFCF_4048_BC1D_20E9E04E809A__
#define __MIDIEVENTMANAGER_H__74C2A3BA_DFCF_4048_BC1D_20E9E04E809A__

#include "jdkmidi/multitrack.h"

class MIDIEventManager
{
	unsigned short m_nCurrentTrack;
	unsigned short m_nCurrentLayer;

	enum { MAX_CHANNELS = 16, MAX_LAYERS = 16 };

	unsigned short m_CurrentLayer[MAX_CHANNELS];
	unsigned long m_Time[MAX_CHANNELS][MAX_LAYERS];

	jdkmidi::MIDIMultiTrack m_Tracks;

	jdkmidi::MIDISequencer m_Sequencer;

public:

	MIDIEventManager(void) : m_Sequencer(&m_Tracks)
	{
		Clear();
		m_Tracks.SetClksPerBeat(24); //TODO: Correct this
	}

	~MIDIEventManager(void)
	{
	}

	/// <Summary> Returns the Sequencer holding the collection of tracks </Summary>
	jdkmidi::MIDISequencer* GetSequencer() { return &m_Sequencer; }

	/// <Summary> Returns the Multitrack object </Summary>
	jdkmidi::MIDIMultiTrack* GetTracks() { return &m_Tracks; }

	/// <Summary>
	/// Clears all the Events stored in the tracks and Resets the Track Timers
	/// </Summary>
	void Clear()
	{
		memset(m_CurrentLayer, 0, sizeof(m_CurrentLayer));
		memset(m_Time, 0, sizeof(m_Time));
		m_nCurrentTrack = 0;
		m_nCurrentLayer = 0;
		m_Tracks.Clear();
	}

	/// <Summary>Sets the current Track/Channel to which new Events will be added
	/// </Summary>
	void SetCurrentTrack(unsigned short nTrack)
	{
		m_nCurrentTrack = nTrack;
	}

	void SetCurrentLayer(unsigned short nLayer)
	{
		m_CurrentLayer[m_nCurrentTrack] = nLayer;
	}

	/// <Summary>
	/// Returns the Timer value for the current track (in Pulses Per Quarter)
	/// </Summary>
	unsigned long GetTrackTime()
	{
		return m_Time[m_nCurrentTrack][m_CurrentLayer[m_nCurrentTrack]];
	}

	/// <Summary>
	/// Sets the current Track's time to the supplied new time (specified in Pulses Per Quarter)
	/// </Summary>
	void SetTrackTime(unsigned long lNewTime)
	{
		m_Time[m_nCurrentTrack][m_CurrentLayer[m_nCurrentTrack]] = lNewTime;
	}

	/// <Summary>
	/// Advances the timer for the current Track by the duration (specified in Pulses Per Quarter)
	/// </Summary>
	void AdvanceTrackTime(unsigned long lDuration)
	{
		m_Time[m_nCurrentTrack][m_CurrentLayer[m_nCurrentTrack]] += lDuration;
	}

	/// <Summary>
	/// Adds a short NoteOn message event to the current track using attack/decay velocities. Automatically
	/// adds a NoteOff message using the specified duration. These NoteOn and NoteOff can be suppressed (useful 
	/// when notes are tied).
	/// @param addNoteOn indicates if NoteOn event should be created automatically. For the end of a tied note, this should be false; otherwise true;
	/// @param addNoteOff indicates if NoteOff event should be created automatically. For the start of a tied note, this should be false; otherwise true;	
	/// </Summary>
	void AddNoteEvent(int noteValue, int attackVel, int decayVel, long lNoteDuration, bool addNoteOn, bool addNoteOff)
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

#endif // __MIDIEVENTMANAGER_H__74C2A3BA_DFCF_4048_BC1D_20E9E04E809A__