// BootStrapper.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "BootStrapper.h"
#include "ResourceManipulator.h"
#include <strsafe.h>

#include "shellapi.h"
#pragma comment(lib, "shell32.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name


void ErrorExit(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    // Create a Temporary directory to dump our files to 
    TCHAR lpszTempPath[4096];

    DWORD dwRetVal = GetTempPath(_countof(lpszTempPath), lpszTempPath);
    if (dwRetVal > _countof(lpszTempPath) || (dwRetVal == 0))
    {
        ErrorExit(_T("GetTempPath"));
    }

    TCHAR lpszTempFileName[4096];
    dwRetVal = GetTempFileName(lpszTempPath, _T("Play"), 0, lpszTempFileName);
    if(dwRetVal == 0)
        ErrorExit(_T("GetTempFileName"));

    if(CreateDirectory(lpszTempFileName, NULL) == 0)
    {
        DeleteFile(lpszTempFileName);
        if(CreateDirectory(lpszTempFileName, NULL) == 0)
            ErrorExit(_T("CreateDirectory"));
    }

    CString strDumpDir(lpszTempFileName);

    CResourceManipulator::DumpResource(MAKEINTRESOURCE(IDR_EMBEDDED_EXE),
            _T("EMBEDDED_RESOURCE"), strDumpDir + _T("\\") + _T("PlayByEar.exe"));
    CResourceManipulator::DumpResource(MAKEINTRESOURCE(IDR_EMBEDDED_DLL),
            _T("EMBEDDED_RESOURCE"), strDumpDir + _T("\\") + _T("qhtmlight.dll"));
    CResourceManipulator::DumpResource(MAKEINTRESOURCE(IDR_EMBEDDED_HTML),
            _T("EMBEDDED_RESOURCE"), strDumpDir + _T("\\") + _T("UserManual.html"));

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    CreateProcess(strDumpDir + _T("\\") + _T("PlayByEar.exe"),
        NULL, NULL, NULL, FALSE, 0, NULL, strDumpDir + _T("\\"),
        &si, &pi);

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    // Delete the dumped files
    DeleteFile(strDumpDir + _T("\\") + _T("UserManual.html"));
    DeleteFile(strDumpDir + _T("\\") + _T("qhtmlight.dll"));
    DeleteFile(strDumpDir + _T("\\") + _T("PlayByEar.exe"));
    RemoveDirectory(strDumpDir + _T("\\"));

    // Lets try deleting once more, just in case
    TCHAR szDeleteDir[4096];
    ZeroMemory( &szDeleteDir, _countof(szDeleteDir) );
    _tcscpy(szDeleteDir, strDumpDir);

    SHFILEOPSTRUCT shFileOp;
    ZeroMemory( &shFileOp, sizeof(shFileOp) );
    shFileOp.hwnd = NULL;
    shFileOp.wFunc = FO_DELETE;
    shFileOp.pFrom = szDeleteDir;
    shFileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;

    int nRetVal = SHFileOperation(&shFileOp); // Recursively delete the Directory
    
	return 0;
}


