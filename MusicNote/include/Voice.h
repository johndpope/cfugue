/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __VOICE_H__4FBD8E09_755B_4f7f_B129_B22444EA8813__
#define __VOICE_H__4FBD8E09_755B_4f7f_B129_B22444EA8813__

namespace MusicNoteLib
{
    /// <Summary>
    /// Class that represents Voice changes, also known as <i>track</i> changes
    /// </Summary>
    class Voice
    {
        unsigned char m_nTrack;
    public:
        inline Voice(unsigned char nTrackID) : m_nTrack(nTrackID) { }
        /// Returns the Voice represented by this object
        inline unsigned char GetVoice() const { return m_nTrack; }
    };

} // namespace MusicNoteLib

#endif // __VOICE_H__4FBD8E09_755B_4f7f_B129_B22444EA8813__