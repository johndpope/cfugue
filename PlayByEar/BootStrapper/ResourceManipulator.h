#ifndef __RESOURCE_MANIPULATOR_H__A544F719_C473_4287_85BA_2827CA5C2869_
#define __RESOURCE_MANIPULATOR_H__A544F719_C473_4287_85BA_2827CA5C2869_

#include <atlstr.h>

class CResourceManipulator
{
	static CString	m_strErrMsg;

	static bool DumpResource(HINSTANCE hInst, LPCTSTR ResourceName, LPCTSTR ResourceType, LPCTSTR lpszDestinationFilePath);

public:
	CResourceManipulator(void);
	~CResourceManipulator(void);

	static bool DumpResource(LPCTSTR ResourceName, LPCTSTR ResourceType, LPCTSTR lpszDestinationFilePath);

	static bool DumpResource(LPCTSTR lpszResourceFilePath, LPCTSTR ResourceName, LPCTSTR ResourceType, LPCTSTR lpszDestinationFilePath);

	static bool UpdateResource(LPCTSTR lpszResourceFilePath, LPCTSTR ResourceName, LPCTSTR ResourceType, WORD wLanguage, LPVOID lpData, DWORD dwDataLen);

	static const CString* GetErrorMessage() {	return &m_strErrMsg;	}
};

#endif