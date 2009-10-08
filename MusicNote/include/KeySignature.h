#ifndef KEYSIGNATURE_H__56751CC1_46A2_4238_BAF1_7884CDA8089B__
#define KEYSIGNATURE_H__56751CC1_46A2_4238_BAF1_7884CDA8089B__

#include "Talam.h"

namespace MusicNoteLib
{
    /// <Summary>
    /// \brief Class representing the Key Signature
    ///
    /// Key Signatures are applicable for the whole song, independent of tracks. It
    /// can be changed during a song, though. Player will automatically adjust
    /// note values based on the current Key Signature. For example, in a F-Major
    /// Key Signature, all B notes will be converted to B-Flat automatically, unless
    /// explicitly indicated as a B natural (i.e., "Bn"). However, note that such
    /// modifications are applied only to the Notes that are specified in alphabetic
    /// format. Notes specified using numeric form will not get affected by the
    /// KeySignature manipulations. They will be played as is.
    ///
    /// Key Signatures support both Carnatic and Western Music modes.
    /// Use the appropriate Macros to distinguish between them. 
    ///
    /// In Carnatic mode, all the basic 72 Key Signatures, known as the 
    /// Melakartha Janya Ragas, are supported. These are accessible through MELA_x macro,
    /// where x specifies a number in the range [1, 72]. In addition, MELA_0
    /// and MELA_DEFAULT are available, that map to Melakartha 29 - the Shankarabharanam scale.
    ///
    /// In addition, numerous Ragas are accessible through their names directly,
    /// such as KALYANI, MOHANA etc. Any missing notes in those ragas will default
    /// to their 29th scale values (and not to their Parent Janya raga values, <i>be careful</i>).
    /// In other words, Shankarabharanam is the default scale for all
    /// default Note values, no matter from which Ragam you are referring.  
    ///
    /// In Western mode, a total of 15 Key Signatures are present. These are accessible
    /// through their names, such as CMAJ, DMAJ, F\#MIN etc... Additionally SCALE_DEFAULT
    /// Macro is available that maps to CMAJ scale.
    ///
    /// Western is the default mode, and C-Major is the default KeySignature. All Parsers
    /// start in Western mode with C-Major scale set to their default. If you rather want
    /// the Parser to use a different Scale or different Mode, then you should explicitly
    /// supply a KeySignature token with appropriate macro values before passing on any Notes.
    ///
    /// %Note that once a 
    /// KeySignature is encountered in one particular mode (Carnatic or Western),
    /// then all the subsequent Notes will be interpreted as being in that mode, till
    /// another KeySignature token is encountered with a different mode. 
    ///
    /// For example, once the parser encounters a token such as K[MELA_22] then
    /// it will start interpreting all the subsequent Notes in S R G M P D N
    /// style, instead of the default western C D E F G A B style. 
    /// So any notes that use the latter style of notation will fail to 
    /// render properly,  unless a token with western Key 
    /// Signature, such as K[F\#MAJ] or K[AMIN] etc, is inserted before them.
    /// Same is the case the other way round. Once a KeySignature token with 
    /// Western mode is encountered, all the subsequent Notes will be parsed
    /// using the Western C D E F G A B notation, and any notes that are inscribed
    /// using the S R G M P D N notation will fail to render properly, unless an appropriate
    /// Carnatic KeySignature such as K[MELA_65] or K[KALYANI] etc.. is used to
    /// appropriately direct the Parser about the notation switch to Carnatic Mode.
    ///
    /// Use K[MELA_DEFAULT] for Carnatic Mode and K[SCALE_DEFAULT] for Western Mode if you
    /// are not sure which value to specify for the KeySignature.
    /// </Summary>
    class KeySignature
    {        
    public:
        enum Scale      { MAJOR_SCALE = 0, MINOR_SCALE = 1 }; ///< Scale Identifiers [Western Mode]
        enum Mode       { WESTERN =0, CARNATIC = 1 };   ///< Music Mode Identifiers
        enum Speed      { GEETAM = 1, VARNAM = 2,  };   ///< Song Speed Identifiers [Carnatic Mode]
        enum Defaults   { DEFAULT_KEY = 0, DEFAULT_RAGAM = 29 }; ///< Default Values

    protected:
        signed short m_nKeySig;   // Values are in the range [-7, 7] for Western, and in the range [1, 72] for Carnatic
        Scale m_bScale; // Indicates Major scale or Minor scale for Western Mode
        Mode  m_bMode;  // Indicates Carnatic Mode or Western Mode
        Talam m_nTalam; // Indicates Talam (in Carnatic Mode)
        Speed m_nSpeed; // Indicates Speed (in Carnatic Mode)

    public:
        /// Default constructor
        inline KeySignature() 
            : m_nKeySig(DEFAULT_KEY), m_bScale(MAJOR_SCALE), m_bMode(WESTERN), m_nSpeed(VARNAM)
        { }

        /// Western Mode constructor
        inline KeySignature(const signed short nKeySig, const Scale bScale)
            : m_nKeySig(nKeySig), m_bScale(bScale), m_bMode(WESTERN), m_nSpeed(VARNAM)
        { }

        /// Carnatic Mode constructor
        inline KeySignature(const unsigned short nRagam, const unsigned short nTalam)
            : m_nKeySig(nRagam), m_bScale(MAJOR_SCALE), m_bMode(CARNATIC), m_nTalam(nTalam), m_nSpeed(VARNAM)
        {
        }

        /// \brief Returns the Key Signature value.
        /// Based on the Mode, Value will be in the range [-7, 7] or [1, 72].
        /// Use GetMode() to find the Mode.
        inline signed char GetKey() const { return (signed char) m_nKeySig; }

        /// Returns if the Key Signature refers to a Major (0) scale or Minor(1) scale.
        /// Valid only if the Mode is set to KeySignature::WESTERN. Use GetMode()
        /// to verify the Mode.
        inline unsigned char GetMajMin() const { return m_bScale; }

        /// Returns if the KeySignature refers to Carnatic Mode or Western Mode
        inline Mode GetMode() const { return m_bMode; }

        /// \brief Returns the Talam Value.
        /// Valid only when the Mode is CARNATIC. Use GetMode() to find the Mode;
        inline const Talam& GetTalam() const { return m_nTalam; }

        /// Sets the Key Signature value for Western Scales
        /// @param nKeySig Key Signature value in the range [-7, 7]
        inline void SetKey(const signed short nKeySig) { m_nKeySig = nKeySig; m_bMode = WESTERN; }

        /// Sets if the Key Signature refers to a Major (0) scale or Minor(1) scale
        /// @param bScale Value indicating if the scale is Major (0) or Minor (1)
        inline void SetMajMin(const Scale bScale) { m_bScale = bScale; }

        /// Sets the Key Signature value for Carnatic Ragams
        /// @param nRagam Melakartha Janya Ragam number in the range [1, 72]
        inline void SetRagam(const unsigned short nRagam) { m_nKeySig = nRagam; m_bMode = CARNATIC; }

        /// Sets the Talam to the specified value
        /// @param talam The Talam value to be used from now on
        inline void SetTalam(const Talam& talam) { m_nTalam = talam; }

        /// \brief Sets the Song Speed.
        /// Valid only when the Mode is set to KeySignature::CARNATIC. Use GetMode()
        /// to verify the Mode.
        inline void SetSpeed(const Speed nSpeed) { m_nSpeed = nSpeed; }

        /// \brief Returns the current Song speed.
        /// Valid only for Carnatic Mode. Use GetMode() to verify the Mode.
        inline Speed GetSpeed() const { return m_nSpeed; }

        /// Populates the standard KeySignature Macro defintions into Music String Dictionary
        inline static void PopulateStandardDefinitions(DICTIONARY& stdDefns)
        {
            // We maintain support for both Carnatic and Western Key Signatures.

            // In Carnatic Music,
            // a total of 72 Key Signatures, known as Melakartha Janya Ragas, are
            // present. These values are added to the dictionary with MELA_X key name,
            // where x specifies a number in the range [1, 72]. However, to distinguish
            // these values from the Western Key Signature values, we set the 
            // 8th high bit in the byte to be 1, so all the values for the Carnatic
            // music will be in the range [129, 200]. We also support MELA_0
            // and MELA_DEFAULT macros that maps to Melakartha 29 - Shankarabharanam scale.

            // In Western Music,
            // Key signature values span from -7 to 7. But our dictionary does not
            // allow negative values. So we make the values to be from 0 to 14. We
            // move the negative values to the 8 to 14 range. So to get back the
            // correct value, we have to check if the dictionary value is > 7 and
            // substract the number from 7 if so. That will map the 8 to 14 range
            // values, back to -1 to -7 range correctly.

            // Also, Major and Minor scale usually share same Keysignature value.
            // To distinguish, we add 64 to the Major Keysignature values. This
            // will set the 7th high bit in the byte to 1. To get the correct value, 
            // boolean-and the value with 0x40; That is, scale == maj if value >= 64; 

            // See MusicStringParser::ParseKeySignatureToken to see how 
            // these values are interpreted correctly.

            //
            // Western Definitions
            //
            stdDefns[_T("CBMAJ")]       = _T("78");
            stdDefns[_T("ABMIN")]       = _T("14");

            stdDefns[_T("GBMAJ")]       = _T("77");
            stdDefns[_T("EBMIN")]       = _T("13");

            stdDefns[_T("DBMAJ")]       = _T("76");
            stdDefns[_T("BBMIN")]       = _T("12");

            stdDefns[_T("ABMAJ")]       = _T("75");
            stdDefns[_T("FMIN" )]       = _T("11");

            stdDefns[_T("EBMAJ")]       = _T("74");
            stdDefns[_T("CMIN" )]       = _T("10");

            stdDefns[_T("BBMAJ")]       = _T("73");
            stdDefns[_T("GMIN" )]       = _T("9");

            stdDefns[_T("FMAJ") ]       = _T("72");
            stdDefns[_T("DMIN" )]       = _T("8");

            stdDefns[_T("CMAJ") ]       = _T("64");
            stdDefns[_T("AMIN") ]       = _T("0");

            stdDefns[_T("GMAJ")]       = _T("65");
            stdDefns[_T("EMIN")]       = _T("1");

            stdDefns[_T("DMAJ")]       = _T("66");
            stdDefns[_T("BMIN")]       = _T("2");

            stdDefns[_T("AMAJ") ]       = _T("67");
            stdDefns[_T("F#MIN")]       = _T("3");

            stdDefns[_T("EMAJ") ]       = _T("68");
            stdDefns[_T("C#MIN")]       = _T("4");

            stdDefns[_T("BMAJ") ]       = _T("69");
            stdDefns[_T("G#MIN")]       = _T("5");

            stdDefns[_T("F#MAJ")]       = _T("70");
            stdDefns[_T("D#MIN")]       = _T("6");

            stdDefns[_T("C#MAJ")]       = _T("71");
            stdDefns[_T("A#MIN")]       = _T("7");

            stdDefns[_T("SCALE")]       = _T("64"); // Same as C-Major
            stdDefns[_T("SCALE_DEF")]   = _T("64"); // Same as C-Major
            stdDefns[_T("SCALE_DEFAULT")] = _T("64"); // Same as C-Major

            // Populate the Talam Macro Definitions
            Talam::PopulateStandardDefinitions(stdDefns);

            //
            // Carnatic Definitions
            //
            stdDefns[_T("MELA_1")]       = _T("129");
            stdDefns[_T("MELA_2")]       = _T("130");
            stdDefns[_T("MELA_3")]       = _T("131");
            stdDefns[_T("MELA_4")]       = _T("132");
            stdDefns[_T("MELA_5")]       = _T("133");
            stdDefns[_T("MELA_6")]       = _T("134");
            stdDefns[_T("MELA_7")]       = _T("135");
            stdDefns[_T("MELA_8")]       = _T("136");
            stdDefns[_T("MELA_9")]       = _T("137");
            stdDefns[_T("MELA_10")]       = _T("138");
            stdDefns[_T("MELA_11")]       = _T("139");
            stdDefns[_T("MELA_12")]       = _T("140");
            stdDefns[_T("MELA_13")]       = _T("141");
            stdDefns[_T("MELA_14")]       = _T("142");
            stdDefns[_T("MELA_15")]       = _T("143");
            stdDefns[_T("MELA_16")]       = _T("144");
            stdDefns[_T("MELA_17")]       = _T("145");
            stdDefns[_T("MELA_18")]       = _T("146");
            stdDefns[_T("MELA_19")]       = _T("147");
            stdDefns[_T("MELA_20")]       = _T("148");
            stdDefns[_T("MELA_21")]       = _T("149");
            stdDefns[_T("MELA_22")]       = _T("150");
            stdDefns[_T("MELA_23")]       = _T("151");
            stdDefns[_T("MELA_24")]       = _T("152");
            stdDefns[_T("MELA_25")]       = _T("153");
            stdDefns[_T("MELA_26")]       = _T("154");
            stdDefns[_T("MELA_27")]       = _T("155");
            stdDefns[_T("MELA_28")]       = _T("156");
            stdDefns[_T("MELA_29")]       = _T("157");
            stdDefns[_T("MELA_30")]       = _T("158");
            stdDefns[_T("MELA_31")]       = _T("159");
            stdDefns[_T("MELA_32")]       = _T("160");
            stdDefns[_T("MELA_33")]       = _T("161");
            stdDefns[_T("MELA_34")]       = _T("162");
            stdDefns[_T("MELA_35")]       = _T("163");
            stdDefns[_T("MELA_36")]       = _T("164");
            stdDefns[_T("MELA_37")]       = _T("165");
            stdDefns[_T("MELA_38")]       = _T("166");
            stdDefns[_T("MELA_39")]       = _T("167");
            stdDefns[_T("MELA_40")]       = _T("168");
            stdDefns[_T("MELA_41")]       = _T("169");
            stdDefns[_T("MELA_42")]       = _T("170");
            stdDefns[_T("MELA_43")]       = _T("171");
            stdDefns[_T("MELA_44")]       = _T("172");
            stdDefns[_T("MELA_45")]       = _T("173");
            stdDefns[_T("MELA_46")]       = _T("174");
            stdDefns[_T("MELA_47")]       = _T("175");
            stdDefns[_T("MELA_48")]       = _T("176");
            stdDefns[_T("MELA_49")]       = _T("177");
            stdDefns[_T("MELA_50")]       = _T("178");
            stdDefns[_T("MELA_51")]       = _T("179");
            stdDefns[_T("MELA_52")]       = _T("180");
            stdDefns[_T("MELA_53")]       = _T("181");
            stdDefns[_T("MELA_54")]       = _T("182");
            stdDefns[_T("MELA_55")]       = _T("183");
            stdDefns[_T("MELA_56")]       = _T("184");
            stdDefns[_T("MELA_57")]       = _T("185");
            stdDefns[_T("MELA_58")]       = _T("186");
            stdDefns[_T("MELA_59")]       = _T("187");
            stdDefns[_T("MELA_60")]       = _T("188");
            stdDefns[_T("MELA_61")]       = _T("189");
            stdDefns[_T("MELA_62")]       = _T("190");
            stdDefns[_T("MELA_63")]       = _T("191");
            stdDefns[_T("MELA_64")]       = _T("192");
            stdDefns[_T("MELA_65")]       = _T("193");
            stdDefns[_T("MELA_66")]       = _T("194");
            stdDefns[_T("MELA_67")]       = _T("195");
            stdDefns[_T("MELA_68")]       = _T("196");
            stdDefns[_T("MELA_69")]       = _T("197");
            stdDefns[_T("MELA_70")]       = _T("198");
            stdDefns[_T("MELA_71")]       = _T("199");
            stdDefns[_T("MELA_72")]       = _T("200");

            stdDefns[_T("MELA_0")]        = _T("157"); // Defaults to 29th Mela
            stdDefns[_T("MELA_DEFAULT")]  = _T("157");
            stdDefns[_T("MELA")]          = _T("157");

            // Special Identifiers to Restore the Previous Scale/Raga while Switching the Mode
            stdDefns[_T("CARNATIC")]      = _T("256");
            stdDefns[_T("WESTERN")]       = _T("256");

            //
            // Speed Definitions
            //
            stdDefns[_T("1ST_SPEED")]   = _T("1");
            stdDefns[_T("2ND_SPEED")]   = _T("2");
            stdDefns[_T("3RD_SPEED")]   = _T("3");
            stdDefns[_T("4TH_SPEED")]   = _T("4");
            stdDefns[_T("5TH_SPEED")]   = _T("5");
            stdDefns[_T("6TH_SPEED")]   = _T("6");
            stdDefns[_T("1ST")]         = _T("1");
            stdDefns[_T("2ND")]         = _T("2");
            stdDefns[_T("3RD")]         = _T("3");
            stdDefns[_T("4TH")]         = _T("4");
            stdDefns[_T("5TH")]         = _T("5");
            stdDefns[_T("6TH")]         = _T("6");
            stdDefns[_T("FIRST")]       = _T("1");
            stdDefns[_T("SECOND")]      = _T("2");
            stdDefns[_T("THIRD")]       = _T("3");
            stdDefns[_T("FOURTH")]      = _T("4");
            stdDefns[_T("FIFTH")]       = _T("5");
            stdDefns[_T("SIXTH")]       = _T("6");
       }

    };

} // namespace MusicNoteLib

#endif // KEYSIGNATURE_H__56751CC1_46A2_4238_BAF1_7884CDA8089B__