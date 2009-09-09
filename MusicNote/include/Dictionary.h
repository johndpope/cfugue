#ifndef __DICTIONARY_H__57D24F57_01B6_4fcd_B92E_7B7849C51407__
#define __DICTIONARY_H__57D24F57_01B6_4fcd_B92E_7B7849C51407__

#include "MString.h"
#include <map>

namespace MusicNoteLib
{
	/// <Summary>Implements the less operator for Maps with string case-insensitive comparisions </Summary>
	template<class _Ty>
	struct StringLess : public std::binary_function<_Ty, _Ty, bool>
	{
		inline bool operator()(const _Ty& _Left, const _Ty& _Right) const
		{	
			return _tcsicmp(_Left, _Right) < 0 ? true : false; // check if (_Left < _Right);
		}
	};

	/// <Summary> Dictionary type maps a string key to a string value. The string
	/// value can be an integer or double (in text form), or a string </Summary>
	typedef std::map<MString, MString, StringLess<const TCHAR*> > DICTIONARY;

	/// <Summary> Accessor method for the standard macro definitions </Summary>
	const DICTIONARY& GetStandardDefinitions();

} // namespace MusicNoteLib

#endif // __DICTIONARY_H__57D24F57_01B6_4fcd_B92E_7B7849C51407__