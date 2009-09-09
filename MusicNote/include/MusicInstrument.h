#ifndef __MUSICINSTRUMENT_H__0823ABB9_BB3E_46a6_84DE_BBA9F4E7F821__
#define __MUSICINSTRUMENT_H__0823ABB9_BB3E_46a6_84DE_BBA9F4E7F821__

#include "Dictionary.h"

namespace MusicNoteLib
{
	/**
	 * Represents MIDI instrument objects. Useful for instrument changes,
	 * also known as <i>patch changes</i>.
	 */
	class MusicInstrument
	{
		unsigned char m_nInstrumentID; //MIDI instrument ID in range [0, 127]
	public:
		inline MusicInstrument(unsigned char nInstrumentID) : m_nInstrumentID(nInstrumentID) { }
		
		///<Summary>Returns the MIDI Instrument ID this object represents</Summary>		 
		inline unsigned char GetInstrumentID() const { return m_nInstrumentID; }

		/// <Summary>Returns the name of the music instrument this object represents</Summary>
		MString GetInstrumentName() const;

		/// Populates the standard musical instrument Macro defintions into Music String Dictionary
		static void PopulateStandardDefinitions(DICTIONARY& stdDefns);
	};

} // namespace MusicNoteLib

#endif // __MUSICINSTRUMENT_H__0823ABB9_BB3E_46a6_84DE_BBA9F4E7F821__