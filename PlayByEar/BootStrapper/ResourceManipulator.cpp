#include "StdAfx.h"
#include "resourcemanipulator.h"

CString CResourceManipulator::m_strErrMsg;

CResourceManipulator::CResourceManipulator(void)
{
}

CResourceManipulator::~CResourceManipulator(void)
{
}

bool CResourceManipulator::DumpResource(LPCTSTR ResourceName, LPCTSTR ResourceType, LPCTSTR lpszDestinationFilePath)
{
	return DumpResource((HINSTANCE)NULL,ResourceName,ResourceType,lpszDestinationFilePath);
}

bool CResourceManipulator::DumpResource(LPCTSTR lpszResourceFilePath, LPCTSTR ResourceName, LPCTSTR ResourceType, LPCTSTR lpszDestinationFilePath)
{
	return DumpResource(LoadLibrary(lpszResourceFilePath),ResourceName,ResourceType,lpszDestinationFilePath);
}

bool CResourceManipulator::DumpResource(HINSTANCE hInst, LPCTSTR ResourceName, LPCTSTR ResourceType, LPCTSTR lpszDestinationFilePath)
{
	HRSRC	hRC		= NULL;
	HGLOBAL	hRes	= NULL;
	DWORD	dwSize	= NULL;
	LPVOID	pRes	= NULL;

	if(NULL == (hRC = FindResource(hInst,ResourceName,ResourceType)))
	{
		m_strErrMsg = _T("Unable to Find Resource : ") + CString(ResourceName);
		return false;
	}
	if(NULL == (hRes = LoadResource(hInst,hRC)))
	{
		m_strErrMsg = _T("Unable to Load Resource : ") + CString(ResourceName);
		return false;
	}
	if(NULL == (dwSize = SizeofResource(hInst,hRC)))
	{
		m_strErrMsg = _T("Invalid Size Resource : ") + CString(ResourceName);
		return false;
	}

	HANDLE hFile = INVALID_HANDLE_VALUE;
		
	if(INVALID_HANDLE_VALUE == (hFile = CreateFile(lpszDestinationFilePath,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_ALWAYS,0,NULL)))
	{
		m_strErrMsg = _T("Unable to Create/Open File : ") + CString(lpszDestinationFilePath);
		return false;
	}

	bool bSuccess = false;
	do
	{
		if(NULL == (pRes = LockResource(hRes)))
		{
			m_strErrMsg = _T("Unable to Lock Resource : ") + CString(ResourceName);
			break;
		}

		DWORD dwWritten = 0;

		if(!WriteFile(hFile, pRes, dwSize, &dwWritten, NULL))
		{
			m_strErrMsg = _T("Unable to Write to File : ") + CString(lpszDestinationFilePath);
			break;
		}

		bSuccess = true;
	}while(false);

	CloseHandle(hFile);

	UnlockResource(hRes);

	return bSuccess;
}

bool CResourceManipulator::UpdateResource(LPCTSTR lpszResourceFilePath, LPCTSTR ResourceName, LPCTSTR ResourceType, WORD wLanguage, LPVOID lpData, DWORD dwDataLen)
{
	HANDLE hRes = NULL;
	
	if(NULL == (hRes = BeginUpdateResource(lpszResourceFilePath, false)))
	{
		m_strErrMsg = _T("Unable to Load : ") + CString(lpszResourceFilePath);
		return false;
	}

	if(!::UpdateResource(hRes, ResourceType, ResourceName, wLanguage, lpData, dwDataLen))
	{
		m_strErrMsg = _T("Unable to Update Resource : ") + CString(ResourceName);
		return false;
	}

	if(!::EndUpdateResource(hRes,false))
	{
		m_strErrMsg = _T("Unable to Commit Update for Resource : ") + CString(ResourceName);
		return false;
	}

	return true;
}
