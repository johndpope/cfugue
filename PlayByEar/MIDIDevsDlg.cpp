// MIDIDevsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MIDIDevsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMIDIDevsDlg dialog


CMIDIDevsDlg::CMIDIDevsDlg(UINT bConnectMidiOut, UINT bConnectMidiIn, CWnd* pParent /*=NULL*/)
	: CDialog(CMIDIDevsDlg::IDD, pParent),
    m_OutDevId(0),
    m_InDevId(0),
    m_OutChanged(false),
    m_InChanged(false),
	m_bConnectMIDIOut(bConnectMidiOut),
	m_bConnectMIDIIn(bConnectMidiIn)
{
	//{{AFX_DATA_INIT(CMIDIDevsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMIDIDevsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMIDIDevsDlg)
	DDX_Control(pDX, IDC_MIDI_OUT_DEVS, m_OutDevsCombo);
	DDX_Control(pDX, IDC_MIDI_IN_DEVS, m_InDevsCombo);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CHECK_CONNECT_MIDIOUT, m_ctrlConnectMidiOut);
	DDX_Control(pDX, IDC_CHECK_CONNECT_MIDIIN, m_ctrlConnectMidiIn);
}


BEGIN_MESSAGE_MAP(CMIDIDevsDlg, CDialog)
	//{{AFX_MSG_MAP(CMIDIDevsDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_CONNECT_MIDIOUT, &CMIDIDevsDlg::OnBnClickedCheckConnectMidiout)
	ON_BN_CLICKED(IDC_CHECK_CONNECT_MIDIIN, &CMIDIDevsDlg::OnBnClickedCheckConnectMidiin)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMIDIDevsDlg message handlers

BOOL CMIDIDevsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctrlConnectMidiOut.SetCheck(m_bConnectMIDIOut ? BST_CHECKED : BST_UNCHECKED);
	m_ctrlConnectMidiIn.SetCheck(m_bConnectMIDIIn ? BST_CHECKED : BST_UNCHECKED);

    //
    // Initialize output device combo box with the names of all of the 
    // MIDI output devices available
    //
	
    UINT i;
	MIDIOUTCAPS OutCaps;
	UINT nOutDevCount = midi::CMIDIOutDevice::GetNumDevs();
    for(i = 0; i < nOutDevCount; i++)
    {
        midi::CMIDIOutDevice::GetDevCaps(i, OutCaps);
        m_OutDevsCombo.AddString(OutCaps.szPname);
    }

    if(nOutDevCount > 0)
    {
		m_OutDevsCombo.SetCurSel(nOutDevCount > m_OutDevId ? m_OutDevId : 0);
		m_OutDevsCombo.EnableWindow(m_bConnectMIDIOut);
    }
	else 
		m_ctrlConnectMidiOut.SetCheck(BST_UNCHECKED);


    //
    // Initialize input device combo box with the names of all of the
    // MIDI input devices available on this system
    //

    MIDIINCAPS InCaps;
	UINT nInDevCount = midi::CMIDIInDevice::GetNumDevs();
	for(i = 0; i < nInDevCount; i++)
    {
        midi::CMIDIInDevice::GetDevCaps(i, InCaps);
        m_InDevsCombo.AddString(InCaps.szPname);
    }

    if(nInDevCount > 0)
    {
		m_InDevsCombo.SetCurSel(nInDevCount > m_InDevId ? m_InDevId : 0);
		m_InDevsCombo.EnableWindow(m_bConnectMIDIIn);
    }
	else 
		m_ctrlConnectMidiIn.SetCheck(BST_UNCHECKED);
	
	return TRUE;  
}


// Client clicked OK
void CMIDIDevsDlg::OnOK() 
{
    //
    // Check to see if the client changed the selection for either the
    // MIDI input or output devices. If so, indicate that a new 
    // selection has been made
    // Also it is possible that the device selected earlier may not be 
	// available now and hence might have been defaulted to zeroth device

    if(m_OutDevsCombo.GetCount() > 0)
    {
        if(m_OutDevId != m_OutDevsCombo.GetCurSel())
        {
	        m_OutDevId = m_OutDevsCombo.GetCurSel();
            m_OutChanged = true;
        }
		m_OutChanged = m_OutChanged || 
			((m_bConnectMIDIOut && m_ctrlConnectMidiOut.GetCheck() != BST_CHECKED) ||
			(m_bConnectMIDIOut==false && m_ctrlConnectMidiOut.GetCheck() != BST_UNCHECKED));

		m_bConnectMIDIOut = (m_ctrlConnectMidiOut.GetCheck() == BST_CHECKED) ? TRUE: FALSE;
    }
	else { m_OutChanged = true; m_bConnectMIDIOut = false; }

    if(m_InDevsCombo.GetCount() > 0)
    {
        if(m_InDevId != m_InDevsCombo.GetCurSel())
        {
            m_InDevId = m_InDevsCombo.GetCurSel();
            m_InChanged = true;
        }
		m_InChanged = m_InChanged || 
			((m_bConnectMIDIIn && m_ctrlConnectMidiIn.GetCheck() != BST_CHECKED) ||
			(m_bConnectMIDIIn==false && m_ctrlConnectMidiIn.GetCheck() != BST_UNCHECKED));

		m_bConnectMIDIIn = (m_ctrlConnectMidiIn.GetCheck() == BST_CHECKED) ? TRUE: FALSE;
    }
	else { m_InChanged = true; m_bConnectMIDIIn = false; }
	
	CDialog::OnOK();
}

void CMIDIDevsDlg::OnBnClickedCheckConnectMidiout()
{
	//m_ctrlConnectMidiOut.SetCheck(m_ctrlConnectMidiOut.GetCheck() == BST_CHECKED ? BST_UNCHECKED : BST_CHECKED);

	m_OutDevsCombo.EnableWindow(m_ctrlConnectMidiOut.GetCheck() == BST_CHECKED);
}

void CMIDIDevsDlg::OnBnClickedCheckConnectMidiin()
{
//	m_ctrlConnectMidiIn.SetCheck(m_ctrlConnectMidiIn.GetCheck() == BST_CHECKED ? BST_UNCHECKED : BST_CHECKED);

	m_InDevsCombo.EnableWindow(m_ctrlConnectMidiIn.GetCheck() == BST_CHECKED);
}
