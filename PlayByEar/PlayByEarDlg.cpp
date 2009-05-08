// PlayByEarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlayByEar.h"
#include "PlayByEarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const TCHAR* gpszKey = _T("Play By Ear"); // Registry Key Section for this Dialog Data


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPlayByEarDlg dialog




CPlayByEarDlg::CPlayByEarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayByEarDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPlayByEarDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_GM_LIST, m_GMCombo);
    DDX_Control(pDX, IDC_MIDI_KEYS, m_Keys);
}

BEGIN_MESSAGE_MAP(CPlayByEarDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_WM_CLOSE()
	ON_COMMAND(ID_PREF_NOTE_COLOR, OnPrefNoteColor)
	ON_COMMAND(ID_PREF_MIDIDEVICES, OnPrefMididevices)
    ON_CBN_SELCHANGE(IDC_GM_LIST, OnSelchangeGmList)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
    ON_COMMAND(ID_PREFERENCES_SHOW, &CPlayByEarDlg::OnShowHideKeyNames)
    ON_COMMAND(ID_PREFERENCES_CARNATIC, &CPlayByEarDlg::OnPrefCarnaticMusicMode)
    ON_COMMAND(ID_PREFERENCES_WESTERN, &CPlayByEarDlg::OnPrefWesternMusicMode)
    ON_COMMAND(ID_HELP_KEYBINDINGS, &CPlayByEarDlg::OnShowKeyBindings)
    ON_BN_CLICKED(IDC_RADIO_LEVEL1, &CPlayByEarDlg::OnBnClickedRadioLevel1)
    ON_BN_CLICKED(IDC_RADIO_LEVEL2, &CPlayByEarDlg::OnBnClickedRadioLevel2)
    ON_BN_CLICKED(IDC_RADIO_LEVEL3, &CPlayByEarDlg::OnBnClickedRadioLevel3)
    ON_BN_CLICKED(IDC_RADIO_LEVEL4, &CPlayByEarDlg::OnBnClickedRadioLevel4)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_SUBMIT, &CPlayByEarDlg::OnNMClickSyslinkSubmit)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_REPLAY, &CPlayByEarDlg::OnNMClickSyslinkReplay)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_NEXTQUESTION, &CPlayByEarDlg::OnNMClickSyslinkNextquestion)
END_MESSAGE_MAP()


// CPlayByEarDlg message handlers

BOOL CPlayByEarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    // Initialize piano control
    m_Keys.Initialize(CMIDIKeyboard::LOW_NOTE, CMIDIKeyboard::HIGH_NOTE);

    // Attach ourselves to the piano control so that we will be notified
    // when note-on and note-off events occur
    m_Keys.AttachListener(*this);

    
    // Attempt to open MIDI input and output devices
    try
    {
        // If there are any MIDI output devices available, open one.
        if(midi::CMIDIOutDevice::GetNumDevs() > 0)
        {
            m_OutDevice.Open(0);
        }
        // Else there are no MIDI output devices available.
        else
        {
            MessageBox(_T("No MIDI output devices available."), _T("Warning"), 
                MB_ICONWARNING | MB_OK);
        }

        // If there are any MIDI input devices available, open one and begin
        // recording.
        if(midi::CMIDIInDevice::GetNumDevs() > 0)
        {
            m_InDevice.SetReceiver(*this);
            m_InDevice.Open(0);
            // Start receiving MIDI events
            m_InDevice.StartRecording();           
        }
        // Else there are no MIDI input devices available.
        else
        {
            // MessageBox("No MIDI input devices available.", "Warning", MB_ICONWARNING | MB_OK);
        }

        // Retrieve and Initialize Settings
        this->m_Mode = (MusicMode)AfxGetApp()->GetProfileInt(gpszKey, _T("Mode"), WESTERN);
        AdjustDisplayForMode();

        // Display Key Names
        BOOL bVisible = AfxGetApp()->GetProfileInt(gpszKey, _T("KeyNames"), TRUE);               
        GetDlgItem(IDC_STATIC_KEYNAMES)->ShowWindow(bVisible ? SW_SHOW : SW_HIDE);
        this->GetMenu()->CheckMenuItem(ID_PREFERENCES_SHOW, MF_BYCOMMAND | bVisible ? MF_CHECKED : MF_UNCHECKED);

        // Note On Color
        m_Keys.SetNoteOnColor(AfxGetApp()->GetProfileInt(gpszKey, _T("NoteColor"), CPianoCtrl::DEF_NOTE_ON_COLOR));

        // MIDI Instrument combo box
        m_GMCombo.SetCurSel(AfxGetApp()->GetProfileInt(gpszKey, _T("Instrument"), 0));

        // Make sure the piano control regains focus when we are done 
        m_Keys.SetFocus();
    }
    catch(const std::exception &ex)
    {
        MessageBoxA(NULL, ex.what(), "Error", MB_ICONSTOP | MB_OK);
    }
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPlayByEarDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPlayByEarDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPlayByEarDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPlayByEarDlg::OnCancel()
{
    //    CDialog::OnCancel();

    // Comment the above to prevent the dialog from getting
    // closed when ESCAPE key is pressed. However, this disables
    // even the close button. So, we should add the OnClose()
    // handler can make call to OnCancel there as shown below.
}

void CPlayByEarDlg::OnClose()
{
    CWnd* pDlgItem = this->GetDlgItem(IDC_STATIC_KEYNAMES);
    BOOL bVisible = pDlgItem->IsWindowVisible();

    // Persist the Values to Registry
    AfxGetApp()->WriteProfileInt(gpszKey, _T("Mode"), m_Mode);
    AfxGetApp()->WriteProfileInt(gpszKey, _T("KeyNames"), bVisible);
    AfxGetApp()->WriteProfileInt(gpszKey, _T("NoteColor"), m_Keys.GetNoteOnColor());
    AfxGetApp()->WriteProfileInt(gpszKey, _T("Instrument"), m_GMCombo.GetCurSel());

    CDialog::OnOK();
}

void CPlayByEarDlg::OnOK()
{
    //CDialog::OnOK();

    // Comment the above to prevent the dialog from getting
    // closed when Enter key is pressed. However, this disables
    // even the close button. So, we should add the OnClose()
    // handler can make call to OnCancel there as shown above.
}


// Note-on event notification
void CPlayByEarDlg::OnNoteOn(CPianoCtrl &PianoCtrl, unsigned char NoteId)
{
    midi::CShortMsg ShortMsg(midi::NOTE_ON, 0, NoteId, 127, 0);
    ShortMsg.SendMsg(m_OutDevice);
}


// Note-off event notification
void CPlayByEarDlg::OnNoteOff(CPianoCtrl &PianoCtrl, unsigned char NoteId)
{
    midi::CShortMsg ShortMsg(midi::NOTE_OFF, 0, NoteId, 0, 0);
    ShortMsg.SendMsg(m_OutDevice);
}


// Receives MIDI short message
void CPlayByEarDlg::ReceiveMsg(DWORD Msg, DWORD TimeStamp)
{
    midi::CShortMsg ShortMsg(Msg, TimeStamp);
    unsigned char Command = ShortMsg.GetCommand();
    CString Text;

    //// Display command value
    //Text.Format(_T("%d"), ShortMsg.GetCommand());
    //m_Command.SetWindowText(Text);

    //// Display channel value
    //Text.Format(_T("%d"), ShortMsg.GetChannel());
    //m_Channel.SetWindowText(Text);

    //// Display data byte 1 value
    //Text.Format(_T("%d"), ShortMsg.GetData1());
    //m_Data1.SetWindowText(Text);

    //// Display data byte 2 value
    //Text.Format(_T("%d"), ShortMsg.GetData2());
    //m_Data2.SetWindowText(Text);
    
    // 
    // If this is a note-on or note-off event, notify piano control so
    // that it can update its display
    //

    if(Command == midi::NOTE_ON && ShortMsg.GetData2() > 0)
    {
        unsigned char Note = ShortMsg.GetData1();

        // Range checking
        if(Note >= m_Keys.GetLowNote() && 
            Note <= m_Keys.GetHighNote())
        {
            m_Keys.NoteOn(Note);
        }
    }
    else if(Command == midi::NOTE_OFF || (Command == midi::NOTE_ON && 
        ShortMsg.GetData2() == 0))
    {
        unsigned char Note = ShortMsg.GetData1();

        // Range checking
        if(Note >= m_Keys.GetLowNote() && 
            Note <= m_Keys.GetHighNote())
        {
            m_Keys.NoteOff(Note);
        }
    }
}


// Receives an erroneous short message
void CPlayByEarDlg::OnError(DWORD Msg, DWORD TimeStamp)
{
    MessageBox(_T("An illegal MIDI short message was received."), _T("Error"),
        MB_ICONSTOP | MB_OK);
}

// Note color preference
void CPlayByEarDlg::OnPrefNoteColor() 
{
	CColorDialog Dlg(m_Keys.GetNoteOnColor());

    if(Dlg.DoModal() == IDOK)
    {
        m_Keys.SetNoteOnColor(Dlg.GetColor());
    }	
}


// MIDI device preferences
void CPlayByEarDlg::OnPrefMididevices() 
{
    try
    {
	    CMIDIDevsDlg Dlg;

        //
        // Initialize MIDI device dialog box
        //

        if(m_OutDevice.IsOpen())
        {
            Dlg.SetOutDevId(m_OutDevice.GetDevID());
        }

        if(m_InDevice.IsOpen())
        {
            Dlg.SetInDevId(m_InDevice.GetDevID());
        }

        // Run dialog box
        if(Dlg.DoModal() == IDOK)
        {
            // If the client clicked OK and they chose to change the 
            // MIDI output device
            if(Dlg.IsOutDevChanged())
            {
                m_OutDevice.Close();
                m_OutDevice.Open(Dlg.GetOutDevId());   
                OnSelchangeGmList();
            }

            // If the client clicked OK and they chose to change the 
            // MIDI input device
            if(Dlg.IsInDevChanged())
            {
                if(m_InDevice.IsOpen())
                {
                    if(m_InDevice.IsRecording())
                    {
	                    // Stop receiving MIDI events 	
                        m_InDevice.StopRecording();   	
                    }
                
                    m_InDevice.Close(); 	
                }

                m_InDevice.Open(Dlg.GetInDevId());   	
                
                // Start receiving MIDI events 	
                m_InDevice.StartRecording(); 
            }
        }	
    }
    catch(const std::exception &ex)
    {
        MessageBoxA(NULL, ex.what(), "Error", MB_ICONSTOP | MB_OK);
    }
}


// Exit the program
void CPlayByEarDlg::OnFileExit() 
{
	SendMessage(WM_CLOSE);
}


// Select a new General MIDI patch
void CPlayByEarDlg::OnSelchangeGmList() 
{
	unsigned char Patch = m_GMCombo.GetCurSel();

    // Construct and send program change message
    midi::CShortMsg Msg(midi::PROGRAM_CHANGE, 0, Patch, 0, 0);
    Msg.SendMsg(m_OutDevice);	

    // Make sure the piano control regains focus when we are done with 
    // the Genera MIDI combo box
    m_Keys.SetFocus();
}


void CPlayByEarDlg::OnHelpAbout() 
{
	CAboutDlg Dlg;

    Dlg.DoModal();	
}

TCHAR* gszKeyNames[] = {_T("S  R2  G3 M1  P  D2  N3"),
                        _T("C   D    E    F    G    A   B")};
TCHAR* gszKeyNames2[] = {_T("R1 G2        M2 D1 N2"),
                         _T("C# D#        F# G# A#")};

UINT MenuIds[] = {ID_PREFERENCES_CARNATIC, ID_PREFERENCES_WESTERN};

// Adjusts the menu and static/text boxes based on the current music mode
void CPlayByEarDlg::AdjustDisplayForMode()
{
    this->GetMenu()->CheckMenuRadioItem(ID_PREFERENCES_CARNATIC, ID_PREFERENCES_WESTERN,
                                MenuIds[m_Mode], MF_BYCOMMAND);

    this->SetDlgItemText(IDC_STATIC_KEYNAMES, gszKeyNames[m_Mode]);
    this->SetDlgItemText(IDC_STATIC_KEYNAMES2, gszKeyNames2[m_Mode]);
}

void CPlayByEarDlg::OnPrefCarnaticMusicMode()
{
    this->m_Mode = CARNATIC;
    AdjustDisplayForMode();
}

void CPlayByEarDlg::OnPrefWesternMusicMode()
{
    this->m_Mode = WESTERN;
    AdjustDisplayForMode();    
}

void CPlayByEarDlg::OnShowHideKeyNames()
{
    CWnd* pDlgItem = this->GetDlgItem(IDC_STATIC_KEYNAMES);

    BOOL bVisible = pDlgItem->IsWindowVisible();

    pDlgItem->ShowWindow(bVisible ? SW_HIDE : SW_SHOW);
    this->GetDlgItem(IDC_STATIC_KEYNAMES2)->ShowWindow(bVisible ? SW_HIDE : SW_SHOW);

    this->GetMenu()->CheckMenuItem(ID_PREFERENCES_SHOW, MF_BYCOMMAND | bVisible ? MF_UNCHECKED : MF_CHECKED);
}

void CPlayByEarDlg::OnShowKeyBindings()
{
    TCHAR szImagePath[1024]; DWORD dwSize = _countof(szImagePath);
    QueryFullProcessImageName(GetCurrentProcess(), 0, szImagePath, &dwSize);
    CString strPath(szImagePath);
    strPath = strPath.Mid(0, strPath.ReverseFind(_T('\\')));
        
    HINSTANCE hInst = ShellExecute(this->m_hWnd, _T("open"), _T("File:///") + strPath + _T("\\UserManual.html#Key_Bindings"), NULL, NULL, SW_SHOW);
    switch((long)hInst)
    {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND: MessageBox(_T("Unable to locate UserManual.html"), _T("Error"), MB_OK|MB_ICONERROR); break;
    }
}


void CPlayByEarDlg::OnBnClickedRadioLevel1()
{
    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnBnClickedRadioLevel2()
{
    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnBnClickedRadioLevel3()
{
    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnBnClickedRadioLevel4()
{
    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnNMClickSyslinkSubmit(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnNMClickSyslinkReplay(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnNMClickSyslinkNextquestion(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}
