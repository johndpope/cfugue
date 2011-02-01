/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __LAYER_H__B93236D6_FA2B_4de1_B1F1_908DBA7B7CE4__
#define __LAYER_H__B93236D6_FA2B_4de1_B1F1_908DBA7B7CE4__

namespace MusicNoteLib
{
    /// <Summary>
    /// Represents Layer object for MIDI tracks. A Layer allows multiple notes to be
    /// played at the same time on a single track (also known as voice), without being
    /// specified as a Chord. This is quite helpful with Track 9, the Percussion Track.
    /// </Summary>
    class Layer
    {
        unsigned char m_nLayer;

    public:
        inline Layer(const unsigned char nLayer) : m_nLayer(nLayer) { }

        /// Returns the Layer ID the present object represents
        inline unsigned char GetLayer() const { return m_nLayer; }
    };

} // namespace MusicNoteLib

#endif // __LAYER_H__B93236D6_FA2B_4de1_B1F1_908DBA7B7CE4__