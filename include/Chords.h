/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __INDEXED_VECTORS_H__E2C81E6D_A292_41d7_B830_F7C07D335205__
#define __INDEXED_VECTORS_H__E2C81E6D_A292_41d7_B830_F7C07D335205__

#include "Common/_TChar.h"
#include <vector>
#include <map>

namespace CFugue
{
    /// Definition of a Chord entry
    struct ChordDef
    {
        typedef signed short HALFSTEP;
        
        enum { MAX_INTERVALS = 16, MAX_NAME = 16 };

        TCHAR    szChordName[MAX_NAME];     ///< Name of the Chord
        HALFSTEP Intervals[MAX_INTERVALS];  ///< The half-step intervals for the Chord. Zero indicates root note
        unsigned short nIntervalCount;      ///< Number of valid entries in the Intervals[] array
    };

    /// <Summary>Maniuplates Chord definitions for Western Music</Summary>
    class Chords
    {    
        std::map<TCHAR, std::vector<const ChordDef *> > m_Definitions;
    public:
        /// Initialize the Chords with default definitions
        Chords();

        /// Initialize the Chords with custom Chord definitions.
        /// If supplied array is empty, default values will be loaded.
        /// Only references to the ChordDef are stored in the Chords class.
        /// So ensure that the supplied pChords is not freed till this Chords
        /// object is released.
        /// @param pChords Array of Chord definitions.
        /// @param nSize Size of the pChords Array
        Chords(const ChordDef* pChords, int nSize);

        /// Overrides the existing definitions with the supplied values. 
        /// If supplied array is empty, default values will be loaded. 
        /// The supplied array should not be freed before this Chords object is freed.
        /// @param pChords Array of Chord definitions. Use NULL to load the default values
        /// @param nSize Size of the pChords Array
        void LoadDefinitions(const ChordDef* pChords = NULL, int nSize = 0);

        /// Adds the supplied definitions to the existing ones. 
        /// Caller has to ensure that the new values do not add duplicates to existing ones.
        /// The input array should not be freed before this Chords object is freed.
        /// @param pChords Array of Chord definitions.
        /// @param nSize Size of the pChords Array
        void AddDefinitions(const ChordDef* pChords, int nSize);

        /// Retrieves the chord that suits the first part of the given string.
		/// If you have not added any custom definitions with AddDefinitions(), then
		/// you might find the static GetDefaultMatchingChord() method more convenient.
        /// @param szToken the string that has any Chord name at its start
        /// @param retVal the ChordDef object that has a Chord with its name present in the szToken
		/// @return the number of characters correctly matched. Zero, if no match found
        unsigned int ExtractMatchingChord(const TCHAR* szToken, ChordDef* retVal) const;

        /// Retrieves the chord that suits the first part of the given string.
		/// Same as ExtractMatchingChord, except that this method is static and uses 
		/// only the in-built default chord definitions for the search. For custom chord
		/// definitions, you need to create a Chords object and use ExtractMatchingChord.
        /// @param szToken the string that has any Chord name at its start
        /// @param retVal the ChordDef object that has a Chord with its name present in the szToken
		/// @return the number of characters correctly matched. Zero, if no match found
        static unsigned int GetDefaultMatchingChord(const TCHAR* szToken, ChordDef* retVal);
    };

} // namespace CFugue



#endif // __INDEXED_VECTORS_H__E2C81E6D_A292_41d7_B830_F7C07D335205__