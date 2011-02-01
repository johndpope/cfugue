/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef _TEMPO_H___1A1E8E8A_DFBA_40c7_B45D_7B5EC45BEE4D__
#define _TEMPO_H___1A1E8E8A_DFBA_40c7_B45D_7B5EC45BEE4D__

/** @file Tempo.h 
 * \brief Declares Tempo class used in Western music
 */ 
#include "Dictionary.h"

namespace MusicNoteLib
{
    /// <Summary>
    /// Represents Tempo for a song. This is independent of the individual tracks in a song.
    /// Tempo can be changed during a song.
    /// </Summary>
    class Tempo
    {
        unsigned short m_nTempo;   // Tempo in Beats Per Minute (BPM)
    public:
        inline Tempo(unsigned short nTempoInBPM) : m_nTempo(nTempoInBPM) { }

        /// Returns the current Tempo (in Beats Per Minute)
        inline unsigned short GetTempo() const { return m_nTempo; }

        /// Populates the standard Tempo Macro defintions into Music String Dictionary
        inline static void PopulateStandardDefinitions(DICTIONARY& stdDefns)
        {
            stdDefns[_T("GRAVE")]       = _T("40");
            stdDefns[_T("LARGO")]       = _T("45");
            stdDefns[_T("LARGHETTO")]   = _T("50");
            stdDefns[_T("LENTO")]       = _T("55");
            stdDefns[_T("ADAGIO")]      = _T("60");
            stdDefns[_T("ADAGIETTO")]   = _T("65");

            stdDefns[_T("ANDANTE")]     = _T("70");
            stdDefns[_T("ANDANTINO")]   = _T("80");
            stdDefns[_T("MODERATO")]    = _T("95");
            stdDefns[_T("ALLEGRETTO")]  = _T("110");

            stdDefns[_T("ALLEGRO")]     = _T("120");
            stdDefns[_T("VIVACE")]      = _T("145");
            stdDefns[_T("PRESTO")]      = _T("180");
            stdDefns[_T("PRETISSIMO")]  = _T("220");
        }

    };

} // namespace MusicNoteLib 

#endif // _TEMPO_H___1A1E8E8A_DFBA_40c7_B45D_7B5EC45BEE4D__