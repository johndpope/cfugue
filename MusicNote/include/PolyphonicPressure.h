/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __POLYPHONICPRESSURE_H__CA14D2EE_2BC0_4448_AA3D_5AC1F14983E9_
#define __POLYPHONICPRESSURE_H__CA14D2EE_2BC0_4448_AA3D_5AC1F14983E9_

namespace CFugue
{
	class PolyphonicPressure
	{
		unsigned char uKey;
		unsigned char uPressure;
	public:
		PolyphonicPressure(unsigned char argKey, unsigned char argPressure)
			: uKey(argKey), uPressure(argPressure)
		{
		}

		/// Returns the Key value that the pressure should be applied to
		inline unsigned char GetKey() const { return uKey; }

		/// Returns the Pressure value that should be applied to the Key
		inline unsigned char GetPressure() const { return uPressure; }
	};
} // namespace CFugue

#endif // __POLYPHONICPRESSURE_H__CA14D2EE_2BC0_4448_AA3D_5AC1F14983E9_