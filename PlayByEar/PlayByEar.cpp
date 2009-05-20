// PlayByEar.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PlayByEar.h"
#include "PlayByEarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPlayByEarApp

BEGIN_MESSAGE_MAP(CPlayByEarApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPlayByEarApp construction

CPlayByEarApp::CPlayByEarApp()
{
}


// The one and only CPlayByEarApp object

CPlayByEarApp theApp;


// CPlayByEarApp initialization

BOOL CPlayByEarApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

    //Initialize the QHtml Control
    QHTM_Initialize( AfxGetInstanceHandle() );

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	SetRegistryKey(IDS_REGISTRY_STRING);

	CPlayByEarDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
