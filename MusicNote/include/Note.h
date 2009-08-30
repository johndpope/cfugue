#ifndef __NOTE_H__AA5D5FE9_22AC_4e8c_911A_BB682EC71179__
#define __NOTE_H__AA5D5FE9_22AC_4e8c_911A_BB682EC71179__

namespace MusicNoteLib
{
	struct Note
	{
		bool	isRest;
		short   noteNumber; // Midi Note Value. C5 is 60	
		double	decimalDuration;
		long	duration;
		bool	isStartOfTie;
		bool	isEndOfTie;
		unsigned short	attackVelocity;
		unsigned short	decayVelocity;
		enum { FIRST, SEQUENTIAL, PARALLEL } type;

		/// <Summary> Default Values </Summary>
		enum : long
		{
			DEFAULT_ATTACK_VELOCITY		= 64,
			DEFAULT_DECAY_VELOCITY		= 64,
		};

		Note(void)
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

		~Note(void)
		{
		}
	};

} // namespace MusicNoteLib

#endif // __NOTE_H__AA5D5FE9_22AC_4e8c_911A_BB682EC71179__
