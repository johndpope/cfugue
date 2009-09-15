#include "stdafx.h"
#include "Dictionary.h"
#include "Instrument.h"
#include "Tempo.h"
#include "KeySignature.h"

namespace MusicNoteLib
{
	void PopulateStandardDefinitions(DICTIONARY& dictionaryObj)
	{
        // Populate KeySignature Definitions
        KeySignature::PopulateStandardDefinitions(dictionaryObj);
        // Populate Tempo Definitions
        Tempo::PopulateStandardDefinitions(dictionaryObj);
		// Populate Music Instrument Defintions
		Instrument::PopulateStandardDefinitions(dictionaryObj);
	}

} // namespace MusicNoteLib