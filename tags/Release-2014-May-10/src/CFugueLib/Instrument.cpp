#include "stdafx.h"
#include "Instrument.h"

namespace CFugue
{
    /**
     * Music Instrument Names in String representation
     */
    static const TCHAR* INSTRUMENT_NAME[] = 
    {
         _T("Piano "),
         _T("Bright_Acoustic "),
         _T("Electric_Grand "),
         _T("Honkey_Tonk "),
         _T("Electric_Piano "),
         _T("Electric_Piano_2 "),
         _T("Harpischord "),
         _T("Clavinet "),
         _T("Celesta "),
         _T("Glockenspiel "),

         _T("Music_Box "),
         _T("Vibraphone "),
         _T("Marimba "),
         _T("Xylophone "),
         _T("Tubular_Bells "),
         _T("Dulcimer "),
         _T("Drawbar_Organ "),
         _T("Percussive_Organ "),
         _T("Rock_Organ "),
         _T("Church_Organ "),

         _T("Reed_Organ "),
         _T("Accordian "),
         _T("Harmonica "),
         _T("Tango_Accordian "),
         _T("Guitar "),
         _T("Steel_String_Guitar "),
         _T("Electric_Jazz_Guitar "),
         _T("Electric_Clean_Guitar "),
         _T("Electric_muted_Guitar "),
         _T("Overdriven_Guitar "),
         _T("Distortion_Guitar "),

         _T("Guitar_Harmonics "),
         _T("Acoustic_Bass "),
         _T("Electric_Bass_Finger "),
         _T("Electric_Bass_Pick "),
         _T("Fretless_Bass "),
         _T("Slap_Bass_1 "),
         _T("Slap_Bass_2 "),
         _T("Synth_Bass_1 "),
         _T("Synth_Bass_2 "),

         _T("Violin "),
         _T("Viola "),
         _T("Cello "),
         _T("Contrabass "),
         _T("Tremolo_Strings "),
         _T("Pizzicato_Strings "),
         _T("Orchestral_Strings "),
         _T("Timpani "),
         _T("String_Ensemble_1 "),
         _T("String_Ensemble_2 "),

         _T("Synth_strings_1 "),
         _T("Synth_strings_2 "),
         _T("Choir_Aahs "),
         _T("Voice_Oohs "),
         _T("Synth_Voice "),
         _T("Orchestra_Hit "),
         _T("Trumpet "),
         _T("Trombone "),
         _T("Tuba "),
         _T("Muted_Trumpet "),

         _T("French_Horn "),
         _T("Brass_Section "),
         _T("Synth_brass_1 "),
         _T("Synth_brass_2 "),
         _T("Soprano_Sax "),
         _T("Alto_Sax "),
         _T("Tenor_Sax "),
         _T("Baritone_Sax "),
         _T("Oboe "),
         _T("English_Horn "),

         _T("Bassoon "),
         _T("Clarinet "),
         _T("Piccolo "),
         _T("Flute "),
         _T("Recorder "),
         _T("Pan_Flute "),
         _T("Blown_Bottle "),
         _T("Skakuhachi "),
         _T("Whistle "),
         _T("Ocarina "),

         _T("Square "),
         _T("Sawtooth "),
         _T("Calliope "),
         _T("Chiff "),
         _T("Charang "),
         _T("Voice "),
         _T("Fifths "),
         _T("Basslead "),
         _T("New_Age "),
         _T("Warm "),

         _T("Polysynth "),
         _T("Choir "),
         _T("Bowed "),
         _T("Metallic "),
         _T("Halo "),
         _T("Sweep "),
         _T("Rain "),
         _T("Soundtrack "),
         _T("Crystal "),
         _T("Atmosphere "),

         _T("Brightness "),
         _T("Goblins "),
         _T("Echoes "),
         _T("Sci-fi "),
         _T("Sitar "),
         _T("Banjo "),
         _T("Shamisen "),
         _T("Koto "),
         _T("Kalimba "),
         _T("Bagpipe "),

         _T("Fiddle "),
         _T("Shanai "),
         _T("Tinkle_Bell "),
         _T("Agogo "),
         _T("Steel_Drums "),
         _T("Woodblock "),
         _T("Taiko_Drum "),
         _T("Melodic_Tom "),
         _T("Synth_Drum "),
         _T("Reverse_Cymbal "),

         _T("Guitar_Fret_Noise "),
         _T("Breath_Noise "),
         _T("Seashore "),
         _T("Bird_Tweet "),
         _T("Telephone_Ring "),
         _T("Helicopter "),
         _T("Applause "),
         _T("Gunshot ") 
    };

    MString Instrument::GetInstrumentName() const
    {
        return INSTRUMENT_NAME[m_nInstrumentID];
    }

    void Instrument::PopulateStandardDefinitions(DICTIONARY& stdDefns)
    {
        //
        // Insert Instrument names
        //
        stdDefns[_T("PIANO"                    )] = _T("0");
        stdDefns[_T("ACOUSTIC_GRAND"           )] = _T("0");
        stdDefns[_T("BRIGHT_ACOUSTIC"          )] = _T("1");
        stdDefns[_T("ELECTRIC_GRAND"           )] = _T("2");
        stdDefns[_T("HONKEY_TONK"              )] = _T("3");
        stdDefns[_T("ELECTRIC_PIANO"           )] = _T("4");
        stdDefns[_T("ELECTRIC_PIANO_1"         )] = _T("4");
        stdDefns[_T("ELECTRIC_PIANO_2"         )] = _T("5");
        stdDefns[_T("HARPISCHORD"              )] = _T("6");
        stdDefns[_T("CLAVINET"                 )] = _T("7");
        stdDefns[_T("CELESTA"                  )] = _T("8");
        stdDefns[_T("GLOCKENSPIEL"             )] = _T("9");

        stdDefns[_T("MUSIC_BOX"                )] = _T("10");
        stdDefns[_T("VIBRAPHONE"               )] = _T("11");
        stdDefns[_T("MARIMBA"                  )] = _T("12");
        stdDefns[_T("XYLOPHONE"                )] = _T("13");
        stdDefns[_T("TUBULAR_BELLS"            )] = _T("14");
        stdDefns[_T("DULCIMER"                 )] = _T("15");
        stdDefns[_T("DRAWBAR_ORGAN"            )] = _T("16");
        stdDefns[_T("PERCUSSIVE_ORGAN"         )] = _T("17");
        stdDefns[_T("ROCK_ORGAN"               )] = _T("18");
        stdDefns[_T("CHURCH_ORGAN"             )] = _T("19");

        stdDefns[_T("REED_ORGAN"               )] = _T("20");
        stdDefns[_T("ACCORDIAN"                )] = _T("21");
        stdDefns[_T("HARMONICA"                )] = _T("22");
        stdDefns[_T("TANGO_ACCORDIAN"          )] = _T("23");
        stdDefns[_T("GUITAR"                   )] = _T("24");
        stdDefns[_T("NYLON_STRING_GUITAR"      )] = _T("24");
        stdDefns[_T("STEEL_STRING_GUITAR"      )] = _T("25");
        stdDefns[_T("ELECTRIC_JAZZ_GUITAR"     )] = _T("26");
        stdDefns[_T("ELECTRIC_CLEAN_GUITAR"    )] = _T("27");
        stdDefns[_T("ELECTRIC_MUTED_GUITAR"    )] = _T("28");
        stdDefns[_T("OVERDRIVEN_GUITAR"        )] = _T("29");

        stdDefns[_T("DISTORTION_GUITAR"        )] = _T("30");
        stdDefns[_T("GUITAR_HARMONICS"         )] = _T("31");
        stdDefns[_T("ACOUSTIC_BASS"            )] = _T("32");
        stdDefns[_T("ELECTRIC_BASS_FINGER"     )] = _T("33");
        stdDefns[_T("ELECTRIC_BASS_PICK"       )] = _T("34");
        stdDefns[_T("FRETLESS_BASS"            )] = _T("35");
        stdDefns[_T("SLAP_BASS_1"              )] = _T("36");
        stdDefns[_T("SLAP_BASS_2"              )] = _T("37");
        stdDefns[_T("SYNTH_BASS_1"             )] = _T("38");
        stdDefns[_T("SYNTH_BASS_2"             )] = _T("39");

        stdDefns[_T("VIOLIN"                   )] = _T("40");
        stdDefns[_T("VIOLA"                    )] = _T("41");
        stdDefns[_T("CELLO"                    )] = _T("42");
        stdDefns[_T("CONTRABASS"               )] = _T("43");
        stdDefns[_T("TREMOLO_STRINGS"          )] = _T("44");
        stdDefns[_T("PIZZICATO_STRINGS"        )] = _T("45");
        stdDefns[_T("ORCHESTRAL_STRINGS"       )] = _T("46");
        stdDefns[_T("TIMPANI"                  )] = _T("47");
        stdDefns[_T("STRING_ENSEMBLE_1"        )] = _T("48");
        stdDefns[_T("STRING_ENSEMBLE_2"        )] = _T("49");

        stdDefns[_T("SYNTH_STRINGS_1"          )] = _T("50");
        stdDefns[_T("SYNTH_STRINGS_2"          )] = _T("51");
        stdDefns[_T("CHOIR_AAHS"               )] = _T("52");
        stdDefns[_T("VOICE_OOHS"               )] = _T("53");
        stdDefns[_T("SYNTH_VOICE"              )] = _T("54");
        stdDefns[_T("ORCHESTRA_HIT"            )] = _T("55");
        stdDefns[_T("TRUMPET"                  )] = _T("56");
        stdDefns[_T("TROMBONE"                 )] = _T("57");
        stdDefns[_T("TUBA"                     )] = _T("58");
        stdDefns[_T("MUTED_TRUMPET"            )] = _T("59");

        stdDefns[_T("FRENCH_HORN"              )] = _T("60");
        stdDefns[_T("BRASS_SECTION"            )] = _T("61");
        stdDefns[_T("SYNTHBRASS_1"             )] = _T("62");
        stdDefns[_T("SYNTHBRASS_2"             )] = _T("63");
        stdDefns[_T("SOPRANO_SAX"              )] = _T("64");
        stdDefns[_T("ALTO_SAX"                 )] = _T("65");
        stdDefns[_T("TENOR_SAX"                )] = _T("66");
        stdDefns[_T("BARITONE_SAX"             )] = _T("67");
        stdDefns[_T("OBOE"                     )] = _T("68");
        stdDefns[_T("ENGLISH_HORN"             )] = _T("69");

        stdDefns[_T("BASSOON"                  )] = _T("70");
        stdDefns[_T("CLARINET"                 )] = _T("71");
        stdDefns[_T("PICCOLO"                  )] = _T("72");
        stdDefns[_T("FLUTE"                    )] = _T("73");
        stdDefns[_T("RECORDER"                 )] = _T("74");
        stdDefns[_T("PAN_FLUTE"                )] = _T("75");
        stdDefns[_T("BLOWN_BOTTLE"             )] = _T("76");
        stdDefns[_T("SKAKUHACHI"               )] = _T("77");
        stdDefns[_T("WHISTLE"                  )] = _T("78");
        stdDefns[_T("OCARINA"                  )] = _T("79");

        stdDefns[_T("LEAD_SQUARE"              )] = _T("80");
        stdDefns[_T("SQUARE"                   )] = _T("80");
        stdDefns[_T("LEAD_SAWTOOTH"            )] = _T("81");
        stdDefns[_T("SAWTOOTH"                 )] = _T("81");
        stdDefns[_T("LEAD_CALLIOPE"            )] = _T("82");
        stdDefns[_T("CALLIOPE"                 )] = _T("82");
        stdDefns[_T("LEAD_CHIFF"               )] = _T("83");
        stdDefns[_T("CHIFF"                    )] = _T("83");
        stdDefns[_T("LEAD_CHARANG"             )] = _T("84");
        stdDefns[_T("CHARANG"                  )] = _T("84");
        stdDefns[_T("LEAD_VOICE"               )] = _T("85");
        stdDefns[_T("VOICE"                    )] = _T("85");
        stdDefns[_T("LEAD_FIFTHS"              )] = _T("86");
        stdDefns[_T("FIFTHS"                   )] = _T("86");
        stdDefns[_T("LEAD_BASSLEAD"            )] = _T("87");
        stdDefns[_T("BASSLEAD"                 )] = _T("87");
        stdDefns[_T("PAD_NEW_AGE"              )] = _T("88");
        stdDefns[_T("NEW_AGE"                  )] = _T("88");
        stdDefns[_T("PAD_WARM"                 )] = _T("89");
        stdDefns[_T("WARM"                     )] = _T("89");

        stdDefns[_T("PAD_POLYSYNTH"            )] = _T("90");
        stdDefns[_T("POLYSYNTH"                )] = _T("90");
        stdDefns[_T("PAD_CHOIR"                )] = _T("91");
        stdDefns[_T("CHOIR"                    )] = _T("91");
        stdDefns[_T("PAD_BOWED"                )] = _T("92");
        stdDefns[_T("BOWED"                    )] = _T("92");
        stdDefns[_T("PAD_METALLIC"             )] = _T("93");
        stdDefns[_T("METALLIC"                 )] = _T("93");
        stdDefns[_T("PAD_HALO"                 )] = _T("94");
        stdDefns[_T("HALO"                     )] = _T("94");
        stdDefns[_T("PAD_SWEEP"                )] = _T("95");
        stdDefns[_T("SWEEP"                    )] = _T("95");
        stdDefns[_T("FX_RAIN"                  )] = _T("96");
        stdDefns[_T("RAIN"                     )] = _T("96");
        stdDefns[_T("FX_SOUNDTRACK"            )] = _T("97");
        stdDefns[_T("SOUNDTRACK"               )] = _T("97");
        stdDefns[_T("FX_CRYSTAL"               )] = _T("98");
        stdDefns[_T("CRYSTAL"                  )] = _T("98");
        stdDefns[_T("FX_ATMOSPHERE"            )] = _T("99");
        stdDefns[_T("ATMOSPHERE"               )] = _T("99");

        stdDefns[_T("FX_BRIGHTNESS"            )] = _T("100");
        stdDefns[_T("BRIGHTNESS"               )] = _T("100");
        stdDefns[_T("FX_GOBLINS"               )] = _T("101");
        stdDefns[_T("GOBLINS"                  )] = _T("101");
        stdDefns[_T("FX_ECHOES"                )] = _T("102");
        stdDefns[_T("ECHOES"                   )] = _T("102");
        stdDefns[_T("FX_SCI-FI"                )] = _T("103");
        stdDefns[_T("SCI-FI"                   )] = _T("103");
        stdDefns[_T("SITAR"                    )] = _T("104");
        stdDefns[_T("BANJO"                    )] = _T("105");
        stdDefns[_T("SHAMISEN"                 )] = _T("106");
        stdDefns[_T("KOTO"                     )] = _T("107");
        stdDefns[_T("KALIMBA"                  )] = _T("108");
        stdDefns[_T("BAGPIPE"                  )] = _T("109");

        stdDefns[_T("FIDDLE"                   )] = _T("110");
        stdDefns[_T("SHANAI"                   )] = _T("111");
        stdDefns[_T("TINKLE_BELL"              )] = _T("112");
        stdDefns[_T("AGOGO"                    )] = _T("113");
        stdDefns[_T("STEEL_DRUMS"              )] = _T("114");
        stdDefns[_T("WOODBLOCK"                )] = _T("115");
        stdDefns[_T("TAIKO_DRUM"               )] = _T("116");
        stdDefns[_T("MELODIC_TOM"              )] = _T("117");
        stdDefns[_T("SYNTH_DRUM"               )] = _T("118");
        stdDefns[_T("REVERSE_CYMBAL"           )] = _T("119");

        stdDefns[_T("GUITAR_FRET_NOISE"        )] = _T("120");
        stdDefns[_T("BREATH_NOISE"             )] = _T("121");
        stdDefns[_T("SEASHORE"                 )] = _T("122");
        stdDefns[_T("BIRD_TWEET"               )] = _T("123");
        stdDefns[_T("TELEPHONE_RING"           )] = _T("124");
        stdDefns[_T("HELICOPTER"               )] = _T("125");
        stdDefns[_T("APPLAUSE"                 )] = _T("126");
        stdDefns[_T("GUNSHOT"                  )] = _T("127");

        //
        // Insert Percussion names
        //
        stdDefns[_T("ACOUSTIC_BASS_DRUM"     )] = _T("35");
        stdDefns[_T("BASS_DRUM"              )] = _T("36");
        stdDefns[_T("SIDE_STICK"             )] = _T("37");
        stdDefns[_T("ACOUSTIC_SNARE"         )] = _T("38");
        stdDefns[_T("HAND_CLAP"              )] = _T("39");

        stdDefns[_T("ELECTRIC_SNARE"         )] = _T("40");
        stdDefns[_T("LOW_FLOOR_TOM"          )] = _T("41");
        stdDefns[_T("CLOSED_HI_HAT"          )] = _T("42");
        stdDefns[_T("HIGH_FLOOR_TOM"         )] = _T("43");
        stdDefns[_T("PEDAL_HI_HAT"           )] = _T("44");
        stdDefns[_T("LOW_TOM"                )] = _T("45");
        stdDefns[_T("OPEN_HI_HAT"            )] = _T("46");
        stdDefns[_T("LOW_MID_TOM"            )] = _T("47");
        stdDefns[_T("HI_MID_TOM"             )] = _T("48");
        stdDefns[_T("CRASH_CYMBAL_1"         )] = _T("49");

        stdDefns[_T("HIGH_TOM"               )] = _T("50");
        stdDefns[_T("RIDE_CYMBAL_1"          )] = _T("51");
        stdDefns[_T("CHINESE_CYMBAL"         )] = _T("52");
        stdDefns[_T("RIDE_BELL"              )] = _T("53");
        stdDefns[_T("TAMBOURINE"             )] = _T("54");
        stdDefns[_T("SPLASH_CYMBAL"          )] = _T("55");
        stdDefns[_T("COWBELL"                )] = _T("56");
        stdDefns[_T("CRASH_CYMBAL_2"         )] = _T("57");
        stdDefns[_T("VIBRASLAP"              )] = _T("58");
        stdDefns[_T("RIDE_CYMBAL_2"          )] = _T("59");

        stdDefns[_T("HI_BONGO"               )] = _T("60");
        stdDefns[_T("LOW_BONGO"              )] = _T("61");
        stdDefns[_T("MUTE_HI_CONGA"          )] = _T("62");
        stdDefns[_T("OPEN_HI_CONGA"          )] = _T("63");
        stdDefns[_T("LOW_CONGA"              )] = _T("64");
        stdDefns[_T("HIGH_TIMBALE"           )] = _T("65");
        stdDefns[_T("LOW_TIMBALE"            )] = _T("66");
        stdDefns[_T("HIGH_AGOGO"             )] = _T("67");
        stdDefns[_T("LOW_AGOGO"              )] = _T("68");
        stdDefns[_T("CABASA"                 )] = _T("69");

        stdDefns[_T("MARACAS"                )] = _T("70");
        stdDefns[_T("SHORT_WHISTLE"          )] = _T("71");
        stdDefns[_T("LONG_WHISTLE"           )] = _T("72");
        stdDefns[_T("SHORT_GUIRO"            )] = _T("73");
        stdDefns[_T("LONG_GUIRO"             )] = _T("74");
        stdDefns[_T("CLAVES"                 )] = _T("75");
        stdDefns[_T("HI_WOOD_BLOCK"          )] = _T("76");
        stdDefns[_T("LOW_WOOD_BLOCK"         )] = _T("77");
        stdDefns[_T("MUTE_CUICA"             )] = _T("78");
        stdDefns[_T("OPEN_CUICA"             )] = _T("79");

        stdDefns[_T("MUTE_TRIANGLE"          )] = _T("80");
        stdDefns[_T("OPEN_TRIANGLE"          )] = _T("81");
    }

} // namespace CFugue
