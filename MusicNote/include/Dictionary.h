/*
	This is part of CFugue, the C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

#ifndef __DICTIONARY_H__57D24F57_01B6_4fcd_B92E_7B7849C51407__
#define __DICTIONARY_H__57D24F57_01B6_4fcd_B92E_7B7849C51407__

/** @file Dictionary.h 
 * \brief Declares Dictionary type used by the Parsers
 */ 

#include "Common/StrUtils.h"
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

	/// <Summary> Accessor method to Populate standard macro definitions </Summary>
	void PopulateStandardDefinitions(DICTIONARY& dictionaryObj);

} // namespace MusicNoteLib

#endif // __DICTIONARY_H__57D24F57_01B6_4fcd_B92E_7B7849C51407__