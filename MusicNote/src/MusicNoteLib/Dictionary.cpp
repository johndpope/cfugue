#include "stdafx.h"
#include "Dictionary.h"
#include "Instrument.h"
#include "Tempo.h"

namespace MusicNoteLib
{
	/// <Summary> 
	/// Accessor method for the standard macro definitions. 
	/// Typically used in the Parse methods.
	/// </Summary>
	void PopulateStandardDefinitions(DICTIONARY& dictionaryObj)
	{
        // Populate Tempo Definitions
        Tempo::PopulateStandardDefinitions(dictionaryObj);
		// Populate Music Instrument Defintions
		Instrument::PopulateStandardDefinitions(dictionaryObj);
	}

} // namespace MusicNoteLib