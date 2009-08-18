#ifndef __MSTRING_H__2B50AFA1_EFB9_428a_A397_3FFEA175FA33__
#define __MSTRING_H__2B50AFA1_EFB9_428a_A397_3FFEA175FA33__

#include "TChar.h"	// On Non win32 platforms we use a local TChar.h
#include <string>

namespace MusicNoteLib
{
	/// <Summary>
	/// Helper class for simple string manipulations
	/// </Summary>
	class MString : public 
#ifdef UNICODE
		std::wstring
#else
		std::string
#endif // UNICODE
	{
#ifdef UNICODE
		typedef std::wstring base;
#else
		typedef std::string base;
#endif // UNICODE
	public:
		inline MString() : base() { }
		inline MString(const TCHAR* sz) : base(sz) { }
		inline MString(const base& sz) : base(sz) { }
		inline MString& operator += (const TCHAR* sz)
		{
			base::operator += (sz);
			return *this;
		}	
		inline MString operator + (const TCHAR* sz) const
		{
			return (base&)(*this) + sz;
		}
		inline MString operator + (const TCHAR ch) const
		{
			return (base&)(*this) + ch;
		}
		inline MString operator + (const MString& other) const
		{
			return (*this) + (const TCHAR*)other;
		}
		inline friend MString operator + ( const TCHAR* sz, const MString& obj)
		{
			return MString(sz) + obj;
		}
		inline operator const TCHAR* () const {	return c_str(); }
#ifdef UNICODE
		inline MString(const char* sz) { _ASSERTE(_T("This is not Supported") == NULL); }
#else
		inline MString(const wchar_t* sz) { _ASSERTE(_T("This is not Supported") == NULL); }
#endif
	}; // class MString

} // namespace MusicNoteLib

#endif // __MSTRING_H__2B50AFA1_EFB9_428a_A397_3FFEA175FA33__