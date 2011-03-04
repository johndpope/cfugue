/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef _TIME_H__AB00FD0B_E6BC_40cf_B86B_796212E2FA47__
#define _TIME_H__AB00FD0B_E6BC_40cf_B86B_796212E2FA47__

namespace MusicNoteLib
{
    /// <Summary>
    /// Represents a timing value. Useful for indicating when certain events are played.
    /// </Summary>
    class Time
    {
        unsigned long m_nTime;
    public:
        inline Time(unsigned long nTime) : m_nTime(nTime) { }

        /// Returns the Time value the current object represents
        inline unsigned long GetTime() const { return m_nTime; }
    };

} // namespace MusicNoteLib

#endif // _TIME_H__AB00FD0B_E6BC_40cf_B86B_796212E2FA47__