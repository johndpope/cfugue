// PlayByEarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlayByEar.h"
#include "PlayByEarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const TCHAR* gpszKey = _T("LastSettings"); // Registry Key Section for this Dialog Data


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
    DDX_Control(pDX, IDC_CTRL_INFO, m_ctrlInfo);
    DDX_Control(pDX, IDC_RAGA_LIST, m_RagaListCombo);
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
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_PLAYANSWER, &CPlayByEarDlg::OnNMClickSyslinkPlayAnswer)
    ON_NOTIFY(NM_SETFOCUS, IDC_SYSLINK_SUBMIT, &CPlayByEarDlg::OnNMClickSyslinkSubmit)
    ON_NOTIFY(NM_SETFOCUS, IDC_SYSLINK_REPLAY, &CPlayByEarDlg::OnNMClickSyslinkReplay)
    ON_NOTIFY(NM_SETFOCUS, IDC_SYSLINK_NEXTQUESTION, &CPlayByEarDlg::OnNMClickSyslinkNextquestion)
    ON_NOTIFY(NM_SETFOCUS, IDC_SYSLINK_PLAYANSWER, &CPlayByEarDlg::OnNMClickSyslinkPlayAnswer)
    ON_NOTIFY(NM_RCLICK, IDC_SYSLINK_SUBMIT, &CPlayByEarDlg::OnNMClickSyslinkSubmit)
    ON_NOTIFY(NM_RCLICK, IDC_SYSLINK_REPLAY, &CPlayByEarDlg::OnNMClickSyslinkReplay)
    ON_NOTIFY(NM_RCLICK, IDC_SYSLINK_NEXTQUESTION, &CPlayByEarDlg::OnNMClickSyslinkNextquestion)
    ON_NOTIFY(NM_RCLICK, IDC_SYSLINK_PLAYANSWER, &CPlayByEarDlg::OnNMClickSyslinkPlayAnswer)
    ON_COMMAND(ID_HELP_HOWDOI, &CPlayByEarDlg::OnHelpHowdoi)
    ON_WM_TIMER()
    ON_COMMAND(ID_FILE_EXITAPPLICATION, &CPlayByEarDlg::OnFileExitapplication)
    ON_COMMAND(ID_TEST_START, &CPlayByEarDlg::OnTestStart)
    ON_COMMAND(ID_TEST_STOP, &CPlayByEarDlg::OnTestStop)
    ON_CBN_SELCHANGE(IDC_RAGA_LIST, &CPlayByEarDlg::OnSelchangeRagaList)
//    ON_CBN_DROPDOWN(IDC_RAGA_LIST, &CPlayByEarDlg::OnCbnDropdownRagaList)
ON_COMMAND(ID_TEST_WAITBEFORERETRY, &CPlayByEarDlg::OnTestWaitBeforeRetry)
ON_COMMAND(ID_TEST_WAITBEFORENEWQUESTION, &CPlayByEarDlg::OnTestWaitBeforeNewQuestion)
END_MESSAGE_MAP()


// CPlayByEarDlg message handlers

BOOL CPlayByEarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//// Add "About..." menu item to system menu.
	//// IDM_ABOUTBOX must be in the system command range.
	//ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	//ASSERT(IDM_ABOUTBOX < 0xF000);

	//CMenu* pSysMenu = GetSystemMenu(FALSE);
	//if (pSysMenu != NULL)
	//{
	//	CString strAboutMenu;
	//	strAboutMenu.LoadString(IDS_ABOUTBOX);
	//	if (!strAboutMenu.IsEmpty())
	//	{
	//		pSysMenu->AppendMenu(MF_SEPARATOR); 
	//		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	//	}
	//}

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
        UINT inDevID = AfxGetApp()->GetProfileInt(gpszKey, _T("InputDevice"), 0);
        UINT outDevID = AfxGetApp()->GetProfileInt(gpszKey, _T("OutputDevice"), 0);

        // If there are any MIDI output devices available, open one.
        UINT nOutDevCount = midi::CMIDIOutDevice::GetNumDevs();
        if(nOutDevCount > 0)
        {
            m_OutDevice.Open(nOutDevCount > outDevID ? outDevID : 0);
        }        
        else // Else there are no MIDI output devices available.
        {
            MessageBox(_T("No MIDI output devices available."), _T("Warning"), 
                MB_ICONWARNING | MB_OK);
        }

        // If there are any MIDI input devices available, open one and begin
        // recording.
        UINT nInDevCount = midi::CMIDIInDevice::GetNumDevs();
        if(nInDevCount > 0)
        {
            m_InDevice.SetReceiver(*this);
            m_InDevice.Open(nInDevCount > inDevID ? inDevID : 0);
            // Start receiving MIDI events
            m_InDevice.StartRecording();           
        }        
        else // Else there are no MIDI input devices available.
        {
            // MessageBox("No MIDI input devices available.", "Warning", MB_ICONWARNING | MB_OK);
        }

        // Retrieve and Initialize Settings
        this->m_Mode = (MusicMode)AfxGetApp()->GetProfileInt(gpszKey, _T("Mode"), WESTERN);
        AdjustDisplayForMode();

        // Display Key Names
        m_bDisplayKeyNames = AfxGetApp()->GetProfileInt(gpszKey, _T("KeyNames"), TRUE);               
        this->GetMenu()->CheckMenuItem(ID_PREFERENCES_SHOW, MF_BYCOMMAND | m_bDisplayKeyNames ? MF_CHECKED : MF_UNCHECKED);

        // NoteOn Color
        m_Keys.SetNoteOnColor(AfxGetApp()->GetProfileInt(gpszKey, _T("NoteColor"), CPianoCtrl::DEF_NOTE_ON_COLOR));

        // Octave        
        m_Keys.SetCurrentOctave(AfxGetApp()->GetProfileInt(gpszKey, _T("Octave"), 3));

        // MIDI Instrument combo box
        m_GMCombo.SetCurSel(AfxGetApp()->GetProfileInt(gpszKey, _T("Instrument"), 0));

        // Set the Level
        CQASession::LEVEL level = (CQASession::LEVEL)AfxGetApp()->GetProfileInt(gpszKey, _T("QALevel"), 0);
        m_QASession.SetCurrentLevel(level); 
        if(level == CQASession::WESTERN_SCALE) level = CQASession::CARNATIC_RAGA; // for radio buttons we need this small adjustment
        CheckRadioButton(IDC_RADIO_LEVEL1, IDC_RADIO_LEVEL4, IDC_RADIO_LEVEL1+level);
        if(level == CQASession::CARNATIC_RAGA)
        {
            // Show the Ragalist combo and Hide the Answer Static Control
            GetDlgItem(IDC_STATIC_ANSWER)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_RAGA_LIST)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_RAGA_LIST)->EnableWindow(false);
        }

        // Set the Question Wait Options
        m_QASession.SetWaitBeforeRetry(AfxGetApp()->GetProfileInt(gpszKey, _T("WaitBeforeRetry"), true)?true:false);
        m_QASession.SetWaitBeforeNewQuestion(AfxGetApp()->GetProfileInt(gpszKey, _T("WaitBeforeNewQuestion"), true)?true:false);
        this->GetMenu()->CheckMenuItem(ID_TEST_WAITBEFORERETRY, 
        (m_QASession.GetWaitBeforeRetry() ? MF_CHECKED : MF_UNCHECKED) |MF_BYCOMMAND);
        this->GetMenu()->CheckMenuItem(ID_TEST_WAITBEFORENEWQUESTION, 
        (m_QASession.GetWaitBeforeNewQuestion() ? MF_CHECKED : MF_UNCHECKED) |MF_BYCOMMAND);

        // Make sure the piano control regains focus when we are done 
        m_Keys.SetFocus(); //

        // Default to Invalid Timer
        m_nTimer = 0;

        // Subscribe to the QASessionComplete Event
        m_QASession.evQASessionComplete.Subscribe(this, &CPlayByEarDlg::OnQASessionComplete);
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
    OnTestStop();

    // Persist the Values to Registry
    AfxGetApp()->WriteProfileInt(gpszKey, _T("InputDevice"), m_InDevice.IsOpen() ? m_InDevice.GetDevID() : 0);
    AfxGetApp()->WriteProfileInt(gpszKey, _T("OutputDevice"), m_OutDevice.IsOpen() ? m_OutDevice.GetDevID() : 0);
    AfxGetApp()->WriteProfileInt(gpszKey, _T("Mode"), m_Mode);
    AfxGetApp()->WriteProfileInt(gpszKey, _T("KeyNames"), m_bDisplayKeyNames);
    AfxGetApp()->WriteProfileInt(gpszKey, _T("NoteColor"), m_Keys.GetNoteOnColor());
    AfxGetApp()->WriteProfileInt(gpszKey, _T("Octave"), m_Keys.GetCurrentOctave());
    AfxGetApp()->WriteProfileInt(gpszKey, _T("Instrument"), m_GMCombo.GetCurSel());
    AfxGetApp()->WriteProfileInt(gpszKey, _T("QALevel"), m_QASession.GetCurrentLevel());
    AfxGetApp()->WriteProfileInt(gpszKey, _T("WaitBeforeRetry"), m_QASession.GetWaitBeforeRetry());
    AfxGetApp()->WriteProfileInt(gpszKey, _T("WaitBeforeNewQuestion"), m_QASession.GetWaitBeforeNewQuestion());

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

TCHAR* gszNoteName[]={
        _T("S"), _T("R1"), _T("R2"), _T("G2"), _T("G3"), _T("M1"), _T("M2"), _T("P"), _T("D1"), _T("D2"), _T("N2"), _T("N3"),
        _T("C"), _T("C#"), _T("D"), _T("D#"), _T("E"), _T("F"), _T("F#"), _T("G"), _T("G#"), _T("A"), _T("A#"), _T("B"),
    };

// Converts the Answer Notes into Display String
CString CPlayByEarDlg::ConvertAnswerNotesToString(const CQASession::ANSWERNOTES& AnswerNotes)
{
    CString str, strResult;
    int nOctave, nNoteWithInOctave; 

    CQASession::ANSWERNOTES::const_iterator iter = AnswerNotes.begin();
    CQASession::ANSWERNOTES::const_iterator& iterEnd = AnswerNotes.end();
    while(iter != iterEnd)
    {
        // Decompose the MIDINote Number into Octave and Note
        m_Keys.GetOctaveNote(*iter, nOctave, nNoteWithInOctave);    
        str.Format(_T("%s[%d] "), gszNoteName[nNoteWithInOctave + (m_Mode*12)], nOctave);
        strResult += str;
        iter++;
    }

    if(strResult.IsEmpty()) strResult = _T("-");

    return strResult;
}

// Note-on event notification
void CPlayByEarDlg::OnNoteOn(CPianoCtrl &PianoCtrl, unsigned char NoteId)
{
    // Play the note on MIDI output device
    midi::CShortMsg ShortMsg(midi::NOTE_ON, 0, NoteId, 127, 0);
    ShortMsg.SendMsg(m_OutDevice);

    if(m_QASession.GetCurrentLevel() >= CQASession::CARNATIC_RAGA)
        return;

    if(m_QASession.IsSessionActive())
    {
        switch(m_QASession.GetCurrentState())
        {
        case CQASession::AWAITING_ANSWER:
            {
                // We started Receiving the Answer. Enable the controls
                GetDlgItem(IDC_SYSLINK_SUBMIT)->SetWindowText(_T("<a>Submit</a>"));
                GetDlgItem(IDC_SYSLINK_PLAYANSWER)->SetWindowText(_T("<a>Play the Answer</a>"));
                // follow on ...
            }
        case CQASession::RECEIVING_ANSWER:
            {
                // Add the Note as Answer
                m_QASession.AnswerEntered(NoteId);
                // Display the Notes on Answer Control
                SetDlgItemText(IDC_STATIC_ANSWER, ConvertAnswerNotesToString(m_QASession.GetAnswerNotes()));    
                break;
            }
        }
    }
}


// Note-off event notification
void CPlayByEarDlg::OnNoteOff(CPianoCtrl &PianoCtrl, unsigned char NoteId)
{
    midi::CShortMsg ShortMsg(midi::NOTE_OFF, 0, NoteId, 0, 0);
    ShortMsg.SendMsg(m_OutDevice);
}

// User select an entry from the Raga list.
// If this is the first selection while waiting for answer, 
// enable the 'submit answer' option.
void CPlayByEarDlg::OnSelchangeRagaList()
{
    if(m_QASession.GetCurrentLevel() < CQASession::CARNATIC_RAGA)
        return;

    if(m_QASession.IsSessionActive())
    {
        switch(m_QASession.GetCurrentState())
        {
        case CQASession::AWAITING_ANSWER:
            {
                // We started Receiving the Answer. Enable the controls
                GetDlgItem(IDC_SYSLINK_SUBMIT)->SetWindowText(_T("<a>Submit</a>"));
                GetDlgItem(IDC_SYSLINK_PLAYANSWER)->SetWindowText(_T("<a>Play the Answer</a>"));
                // follow on ...
            }
        case CQASession::RECEIVING_ANSWER:
            {
                CString strItemText;
                this->m_RagaListCombo.GetLBText(this->m_RagaListCombo.GetCurSel(), strItemText);
               
                // Add the Note as Answer
                m_QASession.AnswerEntered(60); // TODO: Take care of entering Raga Notes
                break;
            }
        }
    }

    // Make sure the Paino Control gets the Focus
    m_Keys.SetFocus();
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

UINT gStaticID1[] = {   IDC_STATIC_KEYNAMES_0, 
                        IDC_STATIC_KEYNAMES_1, 
                        IDC_STATIC_KEYNAMES_2,
                        IDC_STATIC_KEYNAMES_3,
                        IDC_STATIC_KEYNAMES_4};
UINT gStaticID2[] = {   IDC_STATIC_KEYNAMESTOP_0, 
                        IDC_STATIC_KEYNAMESTOP_1, 
                        IDC_STATIC_KEYNAMESTOP_2,
                        IDC_STATIC_KEYNAMESTOP_3,
                        IDC_STATIC_KEYNAMESTOP_4};

// Adjusts the menu and static/text boxes based on the current music mode
// to display the key names in correct mode (C, C# or S, R1 etc..)
void CPlayByEarDlg::AdjustDisplayForMode()
{
    this->GetMenu()->CheckMenuRadioItem(ID_PREFERENCES_CARNATIC, ID_PREFERENCES_WESTERN,
                                MenuIds[m_Mode], MF_BYCOMMAND);

    for(unsigned int i=0, nMax = _countof(gStaticID1); i < nMax; ++i)
    {
        SetDlgItemText(gStaticID1[i], gszKeyNames[m_Mode]);
        SetDlgItemText(gStaticID2[i], gszKeyNames2[m_Mode]);
    }

    if(m_QASession.IsSessionActive())    // Display the Notes on Answer Control
        SetDlgItemText(IDC_STATIC_ANSWER, ConvertAnswerNotesToString(m_QASession.GetAnswerNotes()));    
}
 
// When the active octave changes, we need to hide and show the 
// appropriate static controls (key names)
void CPlayByEarDlg::OnActiveOctaveChanged(CPianoCtrl &PianoCtrl, unsigned char newOctave)
{
    // First TurnOff everything
    for(unsigned int i=0, nMax = _countof(gStaticID1); i < nMax; ++i)
    {
        GetDlgItem(gStaticID1[i])->ShowWindow(SW_HIDE);
        GetDlgItem(gStaticID2[i])->ShowWindow(SW_HIDE);
    }
    // Now show only the current octave, if required
    if(m_bDisplayKeyNames)
    {
        GetDlgItem(gStaticID1[m_Keys.GetCurrentOctave()])->ShowWindow(SW_SHOW);
        GetDlgItem(gStaticID2[m_Keys.GetCurrentOctave()])->ShowWindow(SW_SHOW);
    }
}

void CPlayByEarDlg::OnPrefCarnaticMusicMode()
{
    if(m_QASession.IsSessionActive() && m_QASession.GetCurrentLevel() >= CQASession::CARNATIC_RAGA)
    {
        MessageBox(_T("Sorry, You cannot change the Mode on this level while a QA Session is active."), _T("Restricted !!"), MB_OK|MB_ICONINFORMATION);
        return;
    }
    this->m_Mode = CARNATIC;
    AdjustDisplayForMode();
}

void CPlayByEarDlg::OnPrefWesternMusicMode()
{
    if(m_QASession.IsSessionActive() && m_QASession.GetCurrentLevel() >= CQASession::CARNATIC_RAGA)
    {
        MessageBox(_T("Sorry, You cannot change the Mode on this level while a QA Session is active."), _T("Restricted !!"), MB_OK|MB_ICONINFORMATION);
        return;
    }        
    this->m_Mode = WESTERN;
    AdjustDisplayForMode();    
}


void CPlayByEarDlg::OnShowHideKeyNames()
{
    m_bDisplayKeyNames = !m_bDisplayKeyNames;

    // Show/Hide the Static controls
    OnActiveOctaveChanged(this->m_Keys, this->m_Keys.GetCurrentOctave());

    this->GetMenu()->CheckMenuItem(ID_PREFERENCES_SHOW, MF_BYCOMMAND | m_bDisplayKeyNames ? MF_CHECKED : MF_UNCHECKED);
}



void CPlayByEarDlg::OnBnClickedRadioLevel1()
{
    m_QASession.SetCurrentLevel(CQASession::SINGLE_NOTE_SINGLE_OCTAVE);

    // Hide the Ragalist combo and Show the Answer Static Control
    GetDlgItem(IDC_RAGA_LIST)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC_ANSWER)->ShowWindow(SW_SHOW);

    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnBnClickedRadioLevel2()
{
    m_QASession.SetCurrentLevel(CQASession::SINGLE_NOTE_MULTI_OCTAVE);

    // Hide the Ragalist combo and Show the Answer Static Control
    GetDlgItem(IDC_RAGA_LIST)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC_ANSWER)->ShowWindow(SW_SHOW);

    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnBnClickedRadioLevel3()
{
    m_QASession.SetCurrentLevel(CQASession::MULTINOTE);

    // Hide the Ragalist combo and Show the Answer Static Control
    GetDlgItem(IDC_RAGA_LIST)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC_ANSWER)->ShowWindow(SW_SHOW);

    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnBnClickedRadioLevel4()
{
    m_QASession.SetCurrentLevel(m_Mode==CARNATIC ? CQASession::CARNATIC_RAGA : CQASession::WESTERN_SCALE);

    // Show the Ragalist combo and Hide the Answer Static Control
    GetDlgItem(IDC_RAGA_LIST)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_STATIC_ANSWER)->ShowWindow(SW_HIDE);

    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnNMClickSyslinkSubmit(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    m_QASession.SubmitAnswer();

    // We have submitted the answer. Disable the controls            
    GetDlgItem(IDC_SYSLINK_SUBMIT)->SetWindowText(_T(" "));
    GetDlgItem(IDC_SYSLINK_PLAYANSWER)->SetWindowText(_T(" "));
    GetDlgItem(IDC_SYSLINK_REPLAY)->SetWindowText(_T(" "));
    this->m_RagaListCombo.EnableWindow(false);

    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnNMClickSyslinkReplay(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    m_QASession.ReplayQuestion();

    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnNMClickSyslinkNextquestion(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    m_QASession.GoToNextQuestion();

    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnNMClickSyslinkPlayAnswer(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    // Make sure the piano control regains focus 
    m_Keys.SetFocus();
}

void CPlayByEarDlg::OnHelpHowdoi()
{
    TCHAR szImagePath[1024]; DWORD dwSize = _countof(szImagePath);
    QueryFullProcessImageName(GetCurrentProcess(), 0, szImagePath, &dwSize);
    CString strPath(szImagePath);
    strPath = strPath.Mid(0, strPath.ReverseFind(_T('\\')));
        
    HINSTANCE hInst = ShellExecute(this->m_hWnd, _T("open"), _T("File:///") + strPath + _T("\\UserManual.html##How_It_Works"), NULL, NULL, SW_SHOW);
    switch((long)hInst)
    {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND: MessageBox(_T("Unable to locate UserManual.html"), _T("Error"), MB_OK|MB_ICONERROR); break;
    }
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
    
void CPlayByEarDlg::OnKeyUp(CPianoCtrl &PianoCtrl, UINT nChar, UINT nRepCnt, UINT nFlags)
{
    LRESULT result;
    switch(nChar)
    {
    case VK_RETURN: 
        {
            switch(m_QASession.GetCurrentState())
            {
            case CQASession::RECEIVING_ANSWER: OnNMClickSyslinkSubmit(NULL, &result);break; // Simulate Submit
            case CQASession::WRONG_ANSWER: m_QASession.TryAgain(); break;// Try the same question again
            case CQASession::RIGHT_ANSWER: m_QASession.GoToNextQuestion(); break; // User is ready to goto next question
            }
            break;
        }
    }
}

void CPlayByEarDlg::OnTimer(UINT_PTR nIDEvent)
{
    if(m_QASession.ShouldHaltProcessing() == false)
    {
        switch(m_QASession.GetCurrentState())
        {
        case CQASession::PREPARING_QUESTION:
            {
                // Clear the Notes on Answer Control        
                SetDlgItemText(IDC_STATIC_ANSWER, _T("-")); 
                // Make the current Combo selection Invalid
                this->m_RagaListCombo.SetCurSel(-1);
                // Follow on
            }
        case CQASession::POSING_QUESTION: 
            {
                // Disable Options
                GetDlgItem(IDC_SYSLINK_REPLAY)->SetWindowText(_T(" ")); 
                GetDlgItem(IDC_SYSLINK_PLAYANSWER)->SetWindowText(_T(" ")); 
                GetDlgItem(IDC_SYSLINK_SUBMIT)->SetWindowText(_T(" ")); 
                GetDlgItem(IDC_SYSLINK_NEXTQUESTION)->SetWindowText(_T(" "));
                GetDlgItem(IDC_RAGA_LIST)->EnableWindow(false);
                break;
            }
        case CQASession::COMPLETED_QUESTION:
            {
                // Enable the 'Replay Question' option (we disable it on submit again)
                GetDlgItem(IDC_SYSLINK_REPLAY)->SetWindowText(_T("<a>Replay Question</a>"));
                GetDlgItem(IDC_SYSLINK_NEXTQUESTION)->SetWindowText(_T("<a>Next Question</a>"));
                GetDlgItem(IDC_RAGA_LIST)->EnableWindow(true);
                if(m_QASession.GetAnswerNotes().size() != 0) // if answer notes present
                {
                    GetDlgItem(IDC_SYSLINK_SUBMIT)->SetWindowText(_T("<a>Submit</a>"));
                    GetDlgItem(IDC_SYSLINK_PLAYANSWER)->SetWindowText(_T("<a>Play the Answer</a>"));
                }
                break;
            }
        }
      
        // Process the Active State
        m_QASession.ProcessCurrentState();

        GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(m_QASession.GetStatusString());
        this->SetWindowText(_T("Play By Ear: ") + m_QASession.GetStatusString());
        this->m_ctrlInfo.SetWindowText(m_QASession.GetInfoString());

    }
}

void CPlayByEarDlg::OnFileExitapplication()
{
    OnClose();
}

void CPlayByEarDlg::OnTestStart()
{
    // Disable the Level Options
    GetDlgItem(IDC_RADIO_LEVEL1)->EnableWindow(false);
    GetDlgItem(IDC_RADIO_LEVEL2)->EnableWindow(false);
    GetDlgItem(IDC_RADIO_LEVEL3)->EnableWindow(false);
    GetDlgItem(IDC_RADIO_LEVEL4)->EnableWindow(false);

    // Start the Timer
    m_nTimer = SetTimer(1, 500, NULL); // 500 ms
    if(m_nTimer)
    {
        this->GetMenu()->EnableMenuItem(ID_TEST_STOP, MF_ENABLED | MF_BYCOMMAND);
        this->GetMenu()->EnableMenuItem(ID_TEST_START, MF_GRAYED | MF_BYCOMMAND);
        m_QASession.Start();
    }
}

void CPlayByEarDlg::OnTestStop()
{    
    if(m_nTimer) { KillTimer(m_nTimer); m_nTimer=0; }
    this->GetMenu()->EnableMenuItem(ID_TEST_START, MF_ENABLED | MF_BYCOMMAND);
    this->GetMenu()->EnableMenuItem(ID_TEST_STOP, MF_GRAYED | MF_BYCOMMAND);

    m_QASession.Stop();

    // Enable the Level Options
    GetDlgItem(IDC_RADIO_LEVEL1)->EnableWindow(true);
    GetDlgItem(IDC_RADIO_LEVEL2)->EnableWindow(true);
    GetDlgItem(IDC_RADIO_LEVEL3)->EnableWindow(true);
    GetDlgItem(IDC_RADIO_LEVEL4)->EnableWindow(true);
}
    
void CPlayByEarDlg::OnQASessionComplete(const OIL::CEventSource* pSender, OIL::CEventHandlerArgs* pArgs)
{
    OnTestStop();
}

void CPlayByEarDlg::OnTestWaitBeforeRetry()
{
    m_QASession.SetWaitBeforeRetry(!m_QASession.GetWaitBeforeRetry());
    this->GetMenu()->CheckMenuItem(ID_TEST_WAITBEFORERETRY, 
        (m_QASession.GetWaitBeforeRetry() ? MF_CHECKED : MF_UNCHECKED) |MF_BYCOMMAND);

    // If we are currently waiting, apply the new 'non-wait' option immediately
    if(m_QASession.GetWaitBeforeRetry() == false &&
        m_QASession.GetCurrentState() == CQASession::WRONG_ANSWER)
        m_QASession.TryAgain();
}

void CPlayByEarDlg::OnTestWaitBeforeNewQuestion()
{
    m_QASession.SetWaitBeforeNewQuestion(!m_QASession.GetWaitBeforeNewQuestion());
    this->GetMenu()->CheckMenuItem(ID_TEST_WAITBEFORENEWQUESTION, 
        (m_QASession.GetWaitBeforeNewQuestion() ? MF_CHECKED : MF_UNCHECKED) |MF_BYCOMMAND);

    // If we are currently waiting, apply the new 'non-wait' option immediately
    if(m_QASession.GetWaitBeforeNewQuestion() == false &&
        m_QASession.GetCurrentState() == CQASession::RIGHT_ANSWER)
        m_QASession.GoToNextQuestion();
}
