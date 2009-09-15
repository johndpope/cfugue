#ifndef KEYSIGNATURE_H__56751CC1_46A2_4238_BAF1_7884CDA8089B__
#define KEYSIGNATURE_H__56751CC1_46A2_4238_BAF1_7884CDA8089B__

#include "Dictionary.h"

namespace MusicNoteLib
{
    /// <Summary>
    /// \brief Class representing the Key Signature
    ///
    /// Key Signatures are applicable for the whole song, independent of tracks. It
    /// can be changed during a song, though. Player will automatically adjust
    /// note values based on the current Key Signature. For example, in a F-Major
    /// Key Signature, all B notes will be converted to B-Flat automatically, unless
    /// explicitly indicated as a B natural (i.e., "Bn")
    /// </Summary>
    class KeySignature
    {        
    public:
        enum Scale { MAJOR_SCALE = 0, MINOR_SCALE = 1 };

    protected:
        signed short m_nKeySig;   // Values are in the range [-7, 7]
        Scale m_bScale; // Indicates Major scale or Minor scale

    public:
        inline KeySignature() 
            : m_nKeySig(0), m_bScale(MAJOR_SCALE)
        { }

        inline KeySignature(const signed short nKeySig, const Scale bScale)
            : m_nKeySig(nKeySig), m_bScale(bScale) 
        { }

        /// Returns the Key Signature value in the range [-7, 7]
        inline signed char GetKey() const { return (signed char) m_nKeySig; }

        /// Returns if the Key Signature refers to a Major (0) scale or Minor(1) scale
        inline unsigned char GetMajMin() const { return m_bScale; }

        /// Sets the Key Signature value
        /// @param nKeySig Key Signature value in the range [-7, 7]
        inline void SetKey(const signed short nKeySig) { m_nKeySig = nKeySig; }

        /// Sets if the Key Signature refers to a Major (0) scale or Minor(1) scale
        /// @param bScale Value indicating if the scale is Major (0) or Minor (1)
        inline void SetMajMin(const Scale bScale) { m_bScale = bScale; }

        /// Populates the standard KeySignature Macro defintions into Music String Dictionary
        inline static void PopulateStandardDefinitions(DICTIONARY& stdDefns)
        {
            // Key signature values span from -7 to 7. But our dictionary does not
            // allow negative values. So we make the values to be from 0 to 14. We
            // move the negative values to the 8 to 14 range. So to get back the
            // correct value, we have to check if the dictionary value is > 7 and
            // substract the number from 7 if so. That will map the 8 to 14 range
            // values, back to -1 to -7 range correctly.

            // Also, Major and Minor scale usually share same Keysignature value.
            // To distinguish, we add 128 to the Major Keysignature values. This
            // will set the highest bit in the byte to 1. To get the correct value, 
            // boolean-and the value with 0x0F; That is, scale == maj if value >= 128; 

            // See MusicStringParser::ParseKeySignatureToken to see how 
            // these values are interpreted correctly.

            stdDefns[_T("CBMAJ")]       = _T("142");
            stdDefns[_T("ABMIN")]       = _T("14");

            stdDefns[_T("GBMAJ")]       = _T("141");
            stdDefns[_T("EBMIN")]       = _T("13");

            stdDefns[_T("DBMAJ")]       = _T("140");
            stdDefns[_T("BBMIN")]       = _T("12");

            stdDefns[_T("ABMAJ")]       = _T("139");
            stdDefns[_T("FMIN" )]       = _T("11");

            stdDefns[_T("EBMAJ")]       = _T("138");
            stdDefns[_T("CMIN" )]       = _T("10");

            stdDefns[_T("BBMAJ")]       = _T("137");
            stdDefns[_T("GMIN" )]       = _T("9");

            stdDefns[_T("FMAJ") ]       = _T("136");
            stdDefns[_T("DMIN" )]       = _T("8");

            stdDefns[_T("CMAJ") ]       = _T("128");
            stdDefns[_T("AMIN") ]       = _T("0");

            stdDefns[_T("GMAJ")]       = _T("129");
            stdDefns[_T("EMIN")]       = _T("1");

            stdDefns[_T("DMAJ")]       = _T("130");
            stdDefns[_T("BMIN")]       = _T("2");

            stdDefns[_T("AMAJ") ]       = _T("131");
            stdDefns[_T("F#MIN")]       = _T("3");

            stdDefns[_T("EMAJ") ]       = _T("132");
            stdDefns[_T("C#MIN")]       = _T("4");

            stdDefns[_T("BMAJ") ]       = _T("133");
            stdDefns[_T("G#MIN")]       = _T("5");

            stdDefns[_T("F#MAJ")]       = _T("134");
            stdDefns[_T("D#MIN")]       = _T("6");

            stdDefns[_T("C#MAJ")]       = _T("135");
            stdDefns[_T("A#MIN")]       = _T("7");
       }

    };

} // namespace MusicNoteLib

#endif // KEYSIGNATURE_H__56751CC1_46A2_4238_BAF1_7884CDA8089B__