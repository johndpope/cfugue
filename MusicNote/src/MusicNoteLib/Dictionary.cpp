#include "stdafx.h"
#include "Dictionary.h"
#include "MusicInstrument.h"

namespace MusicNoteLib
{
	/// Helper class to initialize static dictionary object
	class DictionaryInitializer : public DICTIONARY
	{
	public:
		inline DictionaryInitializer()
		{
			// Populate Music Instrument Defintions
			MusicInstrument::PopulateStandardDefinitions(*this);
		}
	};

	/// <Summary> 
	/// Accessor method for the standard macro definitions. 
	/// Typically used in the Parse methods.
	/// </Summary>
	const DICTIONARY& GetStandardDefinitions()
	{
		static DictionaryInitializer stdDefns;
		return stdDefns;
	}

} // namespace MusicNoteLib