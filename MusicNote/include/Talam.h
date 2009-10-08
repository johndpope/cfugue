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
            stdDefns[_T("ADI")]       = _T("8");

            // Infer the Talam based on the Markers present in the Music String
            stdDefns[_T("TALAM_UNKNOWN")]       = _T("0"); 
            stdDefns[_T("TALAM_INFER")]         = _T("0");
            stdDefns[_T("TALAM_USE_MARKERS")]   = _T("0");
       }
                
    }; // class Talam

} // namespace MusicNoteLib

#endif // _TALAM_H__D487B423_41C7_4ec5_BA4F_EB0D375C7425__