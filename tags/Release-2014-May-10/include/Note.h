/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://cfugue.sourceforge.net/>.
    
    $LastChangedDate$
    $Rev$
    $LastChangedBy$
*/

#ifndef __NOTE_H__AA5D5FE9_22AC_4e8c_911A_BB682EC71179__
#define __NOTE_H__AA5D5FE9_22AC_4e8c_911A_BB682EC71179__

namespace CFugue
{
    /// <Summary> Class representing a Musical Note </Summary>
	struct Note
	{
		bool	isRest;             ///< Specifies if this is a Rest Note or not
		short   noteNumber;         ///< Midi Note Value. C5 is 60	
		double	decimalDuration;    ///< Specifies the Duration of the note in double
		long	duration;           ///< Specifies the Duration of the note in long
		bool	isStartOfTie;       ///< Indicates if this is the first note in a Tie
		bool	isEndOfTie;         ///< Indciates if this is the last note in a Tie
		unsigned short	attackVelocity; ///< Specifies the attack velocity of this note
		unsigned short	decayVelocity;  ///< Specifies the decay velocity of this note
		enum NoteTypes 
        { 
            FIRST,      ///< Indicates if this is a First note in a group of notes
            SEQUENTIAL, ///< Indicates if this is a Sequential note in a group of notes
            PARALLEL    ///< Indicates if this is a Parallel note in a group of notes
        } type;    ///< Specifies the type of this note

		/// <Summary> Default Values </Summary>
		enum DefaultVelocities : long
		{
			DEFAULT_ATTACK_VELOCITY		= 64,   ///< Default Note Attack Velocity
			DEFAULT_DECAY_VELOCITY		= 64,   ///< Default Note Decay Velocity
		};

		inline Note(void)
			: isRest(false),
			noteNumber(0),
			duration(0),
			decimalDuration(0.0),
			isStartOfTie(false),
			isEndOfTie(false),
			attackVelocity(DEFAULT_ATTACK_VELOCITY),
			decayVelocity(DEFAULT_DECAY_VELOCITY),
			type(FIRST)
		{
		}

		inline ~Note(void)
		{
		}
	};

} // namespace CFugue

#endif // __NOTE_H__AA5D5FE9_22AC_4e8c_911A_BB682EC71179__
