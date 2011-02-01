/*
	This is part of CFugue, the C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __CONTROLLEREVENT_H_501C77B7_6423_4E41_A53D_1516C031BF77
#define __CONTROLLEREVENT_H_501C77B7_6423_4E41_A53D_1516C031BF77

/** @file ControllerEvents.h 
 * \brief Declares MIDI Controller Events used in MusicStrings
 */ 
#include "Dictionary.h"

namespace MusicNoteLib
{
	/// Takes care of defining the MIDI Controller Events used in MusicStrings
	class ControllerEvent
	{
		unsigned char uControl;
		unsigned char uValue;
	public:
		ControllerEvent(unsigned char argControl, unsigned char argValue)
			: uControl(argControl), uValue(argValue)
		{
		}
		/// Returns the current control index
		inline unsigned char GetControl() const { return uControl; }
		/// Returns the current control value
		inline unsigned char GetValue() const { return uValue; }
        /// Populates the standard Tempo Macro defintions into Music String Dictionary
        inline static void PopulateStandardDefinitions(DICTIONARY& stdDefns)
        {
			//
			// Controller names
			//
			stdDefns[_T("BANK_SELECT_COARSE"          )] = _T("0");
			stdDefns[_T("MOD_WHEEL_COARSE"            )] = _T("1");
			stdDefns[_T("BREATH_COARSE"               )] = _T("2");
			stdDefns[_T("FOOT_PEDAL_COARSE"           )] = _T("4");
			stdDefns[_T("PORTAMENTO_TIME_COARSE"      )] = _T("5");
			stdDefns[_T("DATA_ENTRY_COARSE"           )] = _T("6");
			stdDefns[_T("VOLUME_COARSE"               )] = _T("7");
			stdDefns[_T("BALANCE_COARSE"              )] = _T("8");
			stdDefns[_T("PAN_POSITION_COARSE"         )] = _T("10");
			stdDefns[_T("EXPRESSION_COARSE"           )] = _T("11");
			stdDefns[_T("EFFECT_CONTROL_1_COARSE"     )] = _T("12");
			stdDefns[_T("EFFECT_CONTROL_2_COARSE"     )] = _T("13");

			stdDefns[_T("SLIDER_1"                    )] = _T("16");
			stdDefns[_T("SLIDER_2"                    )] = _T("17");
			stdDefns[_T("SLIDER_3"                    )] = _T("18");
			stdDefns[_T("SLIDER_4"                    )] = _T("19");

			stdDefns[_T("BANK_SELECT_FINE"            )] = _T("32");
			stdDefns[_T("MOD_WHEEL_FINE"              )] = _T("33");
			stdDefns[_T("BREATH_FINE"                 )] = _T("34");
			stdDefns[_T("FOOT_PEDAL_FINE"             )] = _T("36");
			stdDefns[_T("PORTAMENTO_TIME_FINE"        )] = _T("37");
			stdDefns[_T("DATA_ENTRY_FINE"             )] = _T("38");
			stdDefns[_T("VOLUME_FINE"                 )] = _T("39");
			stdDefns[_T("BALANCE_FINE"                )] = _T("40");
			stdDefns[_T("PAN_POSITION_FINE"           )] = _T("42");
			stdDefns[_T("EXPRESSION_FINE"             )] = _T("43");
			stdDefns[_T("EFFECT_CONTROL_1_FINE"       )] = _T("44");
			stdDefns[_T("EFFECT_CONTROL_2_FINE"       )] = _T("45");

			stdDefns[_T("HOLD_PEDAL"                  )] = _T("64");
			stdDefns[_T("HOLD"                        )] = _T("64");
			stdDefns[_T("PORTAMENTO"                  )] = _T("65");
			stdDefns[_T("SUSTENUTO_PEDAL"             )] = _T("66");
			stdDefns[_T("SUSTENUTO"                   )] = _T("66");
			stdDefns[_T("SOFT_PEDAL"                  )] = _T("67");
			stdDefns[_T("SOFT"                        )] = _T("67");
			stdDefns[_T("LEGATO_PEDAL"                )] = _T("68");
			stdDefns[_T("LEGATO"                      )] = _T("68");
			stdDefns[_T("HOLD_2_PEDAL"                )] = _T("69");
			stdDefns[_T("HOLD_2"                      )] = _T("69");

			stdDefns[_T("SOUND_VARIATION"             )] = _T("70");
			stdDefns[_T("VARIATION"                   )] = _T("70");
			stdDefns[_T("SOUND_TIMBRE"                )] = _T("71");
			stdDefns[_T("TIMBRE"                      )] = _T("71");
			stdDefns[_T("SOUND_RELEASE_TIME"          )] = _T("72");
			stdDefns[_T("RELEASE_TIME"                )] = _T("72");
			stdDefns[_T("SOUND_ATTACK_TIME"           )] = _T("73");
			stdDefns[_T("ATTACK_TIME"                 )] = _T("73");
			stdDefns[_T("SOUND_BRIGHTNESS"            )] = _T("74");
			stdDefns[_T("BRIGHTNESS"                  )] = _T("74");
			stdDefns[_T("SOUND_CONTROL_6"             )] = _T("75");
			stdDefns[_T("CONTROL_6"                   )] = _T("75");
			stdDefns[_T("SOUND_CONTROL_7"             )] = _T("76");
			stdDefns[_T("CONTROL_7"                   )] = _T("76");
			stdDefns[_T("SOUND_CONTROL_8"             )] = _T("77");
			stdDefns[_T("CONTROL_8"                   )] = _T("77");
			stdDefns[_T("SOUND_CONTROL_9"             )] = _T("78");
			stdDefns[_T("CONTROL_9"                   )] = _T("78");
			stdDefns[_T("SOUND_CONTROL_10"            )] = _T("79");
			stdDefns[_T("CONTROL_10"                  )] = _T("79");

			stdDefns[_T("GENERAL_PURPOSE_BUTTON_1"    )] = _T("80");
			stdDefns[_T("GENERAL_BUTTON_1"            )] = _T("80");
			stdDefns[_T("BUTTON_1"                    )] = _T("80");
			stdDefns[_T("GENERAL_PURPOSE_BUTTON_2"    )] = _T("81");
			stdDefns[_T("GENERAL_BUTTON_2"            )] = _T("81");
			stdDefns[_T("BUTTON_2"                    )] = _T("81");
			stdDefns[_T("GENERAL_PURPOSE_BUTTON_3"    )] = _T("82");
			stdDefns[_T("GENERAL_BUTTON_3"            )] = _T("82");
			stdDefns[_T("BUTTON_3"                    )] = _T("82");
			stdDefns[_T("GENERAL_PURPOSE_BUTTON_4"    )] = _T("83");
			stdDefns[_T("GENERAL_BUTTON_4"            )] = _T("83");
			stdDefns[_T("BUTTON_4"                    )] = _T("83");

			stdDefns[_T("EFFECTS_LEVEL"               )] = _T("91");
			stdDefns[_T("EFFECTS"                     )] = _T("91");
			stdDefns[_T("TREMULO_LEVEL"               )] = _T("92");
			stdDefns[_T("TREMULO"                     )] = _T("92");
			stdDefns[_T("CHORUS_LEVEL"                )] = _T("93");
			stdDefns[_T("CHORUS"                      )] = _T("93");
			stdDefns[_T("CELESTE_LEVEL"               )] = _T("94");
			stdDefns[_T("CELESTE"                     )] = _T("94");
			stdDefns[_T("PHASER_LEVEL"                )] = _T("95");
			stdDefns[_T("PHASER"                      )] = _T("95");

			stdDefns[_T("DATA_BUTTON_INCREMENT"       )] = _T("96");
			stdDefns[_T("DATA_BUTTON_INC"             )] = _T("96");
			stdDefns[_T("BUTTON_INC"                  )] = _T("96");
			stdDefns[_T("BUTTON_INCREMENT"            )] = _T("96");
			stdDefns[_T("DATA_BUTTON_DECREMENT"       )] = _T("97");
			stdDefns[_T("DATA_BUTTON_DEC"             )] = _T("97");
			stdDefns[_T("BUTTON_DEC"                  )] = _T("97");
			stdDefns[_T("BUTTON_DECREMENT"            )] = _T("97");

			stdDefns[_T("NON_REGISTERED_COARSE"       )] = _T("98");
			stdDefns[_T("NON_REGISTERED_FINE"         )] = _T("99");
			stdDefns[_T("REGISTERED_COARSE"           )] = _T("100");
			stdDefns[_T("REGISTERED_FINE"             )] = _T("101");

			stdDefns[_T("ALL_SOUND_OFF"               )] = _T("120");
			stdDefns[_T("ALL_CONTROLLERS_OFF"         )] = _T("121");
			stdDefns[_T("LOCAL_KEYBOARD"              )] = _T("122");
			stdDefns[_T("ALL_NOTES_OFF"               )] = _T("123");
			stdDefns[_T("OMNI_MODE_OFF"               )] = _T("124");
			stdDefns[_T("OMNI_OFF"                    )] = _T("124");
			stdDefns[_T("OMNI_MODE_ON"                )] = _T("125");
			stdDefns[_T("OMNI_ON"                     )] = _T("125");
			stdDefns[_T("MONO_OPERATION"              )] = _T("126");
			stdDefns[_T("MONO"                        )] = _T("126");
			stdDefns[_T("POLY_OPERATION"              )] = _T("127");
			stdDefns[_T("POLY"                        )] = _T("127");

			//
			// Combined Controller names
			// (index = coarse_controller_index + 128 * fine_controller_index)
			// 
			// User can either use 
			//      X[Foot_Pedal_Coarse]=10     
			//      X[Foot_Pedal_Fine]=65
			// or alternately use the combined form
			//      X[Foot_Pedal]=1345
			//
			stdDefns[_T("BANK_SELECT"                )] = _T("4096"); //  0 + 128 * 32
			stdDefns[_T("MOD_WHEEL"                  )] = _T("4225"); //  1 + 128 * 33
			stdDefns[_T("BREATH"                     )] = _T("4354"); //  2 + 128 * 34
			stdDefns[_T("FOOT_PEDAL"                 )] = _T("4612"); //  4 + 128 * 36
			stdDefns[_T("PORTAMENTO_TIME"            )] = _T("4741"); //  5 + 128 * 37
			stdDefns[_T("DATA_ENTRY"                 )] = _T("4870"); //  6 + 128 * 38
			stdDefns[_T("VOLUME"                     )] = _T("4999"); //  7 + 128 * 39
			stdDefns[_T("BALANCE"                    )] = _T("5128"); //  8 + 128 * 40
			stdDefns[_T("PAN_POSITION"               )] = _T("5386"); // 10 + 128 * 42
			stdDefns[_T("EXPRESSION"                 )] = _T("5515"); // 11 + 128 * 43
			stdDefns[_T("EFFECT_CONTROL_1"           )] = _T("5644"); // 12 + 128 * 44
			stdDefns[_T("EFFECT_CONTROL_2"           )] = _T("5773"); // 13 + 128 * 45
			stdDefns[_T("NON_REGISTERED"             )] = _T("12770");// 98 + 128 * 99
			stdDefns[_T("REGISTERED"                 )] = _T("13028");//100 + 128 * 101 

			//
			// On/Off values for controllers
			//
			stdDefns[_T("ON"                         )] = _T("127");
			stdDefns[_T("OFF"                        )] = _T("0");
			stdDefns[_T("DEFAULT"                    )] = _T("64");       
		}
	};

} // namespace MusicNoteLib

#endif // __CONTROLLEREVENT_H_501C77B7_6423_4E41_A53D_1516C031BF77