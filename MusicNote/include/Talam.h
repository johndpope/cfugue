/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

/*
	This is part of CFugue, the C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef _TALAM_H__D487B423_41C7_4ec5_BA4F_EB0D375C7425__
#define _TALAM_H__D487B423_41C7_4ec5_BA4F_EB0D375C7425__

#include "Dictionary.h"

namespace MusicNoteLib
{
    /// <Summary>Class representing Carnatic Music Talam</Summary>
    class Talam
    {
        unsigned short m_nTalam;
    public:
        enum 
        { 
            DEFAULT_VALUE = 8 ///< Defaults to Adi Talam
        };
        inline Talam() : m_nTalam(DEFAULT_VALUE) 
        {
        }
        inline Talam(unsigned short nTalam) : m_nTalam(nTalam) 
        { 
        }
        /// Returns the Talam value 
        inline operator unsigned short() const { return m_nTalam; }

        /// Populates the standard Talam Macro defintions into Music String Dictionary
        inline static void PopulateStandardDefinitions(DICTIONARY& stdDefns)
        {
            stdDefns[_T("trishra_Dhruva")]      = _T("11");  
            stdDefns[_T("trishra_maThya")]      = _T("8");  
            stdDefns[_T("trishra_rUpaka")]      = _T("5");  
            stdDefns[_T("trishra_jhaMpe")]      = _T("6");  
            stdDefns[_T("trishra_tripuTa")]      = _T("7");  
            stdDefns[_T("trishra_ATa")]         = _T("10");  
            stdDefns[_T("trishra_Eka")]         = _T("3");  

            stdDefns[_T("chaturashra_Dhruva")]      = _T("14");  
            stdDefns[_T("chaturashra_maThya")]      = _T("10");  
            stdDefns[_T("chaturashra_rUpaka")]      = _T("6");  
            stdDefns[_T("chaturashra_jhaMpe")]      = _T("7");  
            stdDefns[_T("chaturashra_tripuTa")]      = _T("8");  
            stdDefns[_T("chaturashra_ATa")]         = _T("12");  
            stdDefns[_T("chaturashra_Eka")]         = _T("4");  

            stdDefns[_T("kaMDa_Dhruva")]      = _T("17");  
            stdDefns[_T("kaMDa_maThya")]      = _T("12");  
            stdDefns[_T("kaMDa_rUpaka")]      = _T("7");  
            stdDefns[_T("kaMDa_jhaMpe")]      = _T("8");  
            stdDefns[_T("kaMDa_tripuTa")]      = _T("9");  
            stdDefns[_T("kaMDa_ATa")]         = _T("14");  
            stdDefns[_T("kaMDa_Eka")]         = _T("5");  

            stdDefns[_T("mishra_Dhruva")]      = _T("23");  
            stdDefns[_T("mishra_maThya")]      = _T("16");  
            stdDefns[_T("mishra_rUpaka")]      = _T("9");  
            stdDefns[_T("mishra_jhaMpe")]      = _T("10");  
            stdDefns[_T("mishra_tripuTa")]      = _T("11");  
            stdDefns[_T("mishra_ATa")]         = _T("18");  
            stdDefns[_T("mishra_Eka")]         = _T("7");  

            stdDefns[_T("saMkeerNa_Dhruva")]      = _T("29");  
            stdDefns[_T("saMkeerNa_maThya")]      = _T("20");  
            stdDefns[_T("saMkeerNa_rUpaka")]      = _T("11");  
            stdDefns[_T("saMkeerNa_jhaMpe")]      = _T("12");  
            stdDefns[_T("saMkeerNa_tripuTa")]      = _T("13");  
            stdDefns[_T("saMkeerNa_ATa")]         = _T("22");  
            stdDefns[_T("saMkeerNa_Eka")]         = _T("9");  

            stdDefns[_T("Dhruva")]      = _T("14");  
            stdDefns[_T("maThya")]      = _T("10");  
            stdDefns[_T("rUpaka")]      = _T("6");  
            stdDefns[_T("jhaMpe")]      = _T("7");  
            stdDefns[_T("tripuTa")]      = _T("8");  
            stdDefns[_T("ATa")]         = _T("12");  
            stdDefns[_T("Eka")]         = _T("4");  

            stdDefns[_T("ADi")]                 = _T("8");

            // Infer the Talam based on the Markers present in the Music String
            stdDefns[_T("TALAM_UNKNOWN")]       = _T("0"); 
            stdDefns[_T("TALAM_INFER")]         = _T("0");
            stdDefns[_T("TALAM_USE_MARKERS")]   = _T("0");
       }
                
    }; // class Talam

} // namespace MusicNoteLib

#endif // _TALAM_H__D487B423_41C7_4ec5_BA4F_EB0D375C7425__