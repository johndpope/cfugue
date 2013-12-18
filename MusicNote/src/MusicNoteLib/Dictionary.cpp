#include "stdafx.h"
#include "Dictionary.h"
#include "Instrument.h"
#include "Tempo.h"
#include "KeySignature.h"
#include "ControllerEvent.h"

namespace CFugue
{
	void PopulateStandardDefinitions(DICTIONARY& dictionaryObj)
	{
        // Populate KeySignature Definitions
        KeySignature::PopulateStandardDefinitions(dictionaryObj);
        // Populate Tempo Definitions
        Tempo::PopulateStandardDefinitions(dictionaryObj);
		// Populate Music Instrument Defintions
		Instrument::PopulateStandardDefinitions(dictionaryObj);
		// Populate Controller Event Definitions
		ControllerEvent::PopulateStandardDefinitions(dictionaryObj);
	}

} // namespace CFugue