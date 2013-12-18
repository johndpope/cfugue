/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __PITCHBEND_H__898E3A18_93D1_411D_B7D0_77269C3A0108__
#define __PITCHBEND_H__898E3A18_93D1_411D_B7D0_77269C3A0108__

namespace CFugue
{
	class PitchBend
	{
		unsigned char uLow;
		unsigned char uHigh;
	public:
		PitchBend(unsigned char nLow, unsigned char nHigh)
			: uLow(nLow), uHigh(nHigh)
		{
		}

		///<Summary>Returns the Low Byte value of the PitchBend value</Summary>
		inline unsigned char GetLowByte() const { return uLow; }

		///<Summary>Returns the High Byte value of the PitchBend value</Summary>
		inline unsigned char GetHighByte() const { return uHigh; }
	};
} // namespace 

#endif // __PITCHBEND_H__898E3A18_93D1_411D_B7D0_77269C3A0108__