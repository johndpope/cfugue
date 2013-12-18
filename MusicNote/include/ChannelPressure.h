/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __CHANNELPRESSURE_H__6DE86217_F83D_430E_8097_406A2E3C2204__
#define __CHANNELPRESSURE_H__6DE86217_F83D_430E_8097_406A2E3C2204__

namespace CFugue
{
	class ChannelPressure
	{
		unsigned char nPressure;
	public:
		ChannelPressure(unsigned char argPressure) : nPressure(argPressure)
		{
		}
		/// Returns the Channel pressure value
		inline unsigned char GetPressure() const { return nPressure; }
	};

} // namespace CFugue

#endif // __CHANNELPRESSURE_H__6DE86217_F83D_430E_8097_406A2E3C2204__