// MIDIDevsDlg.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMIDIDevsDlg dialog


CMIDIDevsDlg::CMIDIDevsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMIDIDevsDlg::IDD, pParent),
    m_OutDevId(0),
    m_InDevId(0),
    m_OutChanged(false),
    m_InChanged(false)
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
}


BEGIN_MESSAGE_MAP(CMIDIDevsDlg, CDialog)
	//{{AFX_MSG_MAP(CMIDIDevsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMIDIDevsDlg message handlers

BOOL CMIDIDevsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    //
    // Initialize output device combo box with the names of all of the 
    // MIDI output devices available
    //
	
    UINT i;
	MIDIOUTCAPS OutCaps;
    for(i = 0; i < midi::CMIDIOutDevice::GetNumDevs(); i++)
    {
        midi::CMIDIOutDevice::GetDevCaps(i, OutCaps);
        m_OutDevsCombo.AddString(OutCaps.szPname);
    }

    if(midi::CMIDIOutDevice::GetNumDevs() > 0)
    {
        m_OutDevsCombo.SetCurSel(m_OutDevId);
    }

    //
    // Initialize input device combo box with the names of all of the
    // MIDI input devices available on this system
    //

    MIDIINCAPS InCaps;
    for(i = 0; i < midi::CMIDIInDevice::GetNumDevs(); i++)
    {
        midi::CMIDIInDevice::GetDevCaps(i, InCaps);
        m_InDevsCombo.AddString(InCaps.szPname);
    }

    if(midi::CMIDIInDevice::GetNumDevs() > 0)
    {
        m_InDevsCombo.SetCurSel(m_InDevId);
    }
	
	return TRUE;  
}


// Client clicked OK
void CMIDIDevsDlg::OnOK() 
{
    //
    // Check to see if the client changed the selection for either the
    // MIDI input or output devices. If so, indicate that a new 
    // selection has been made
    //

    if(m_OutDevsCombo.GetCount() > 0)
    {
        if(m_OutDevId != m_OutDevsCombo.GetCurSel())
        {
	        m_OutDevId = m_OutDevsCombo.GetCurSel();
            m_OutChanged = true;
        }
    }

    if(m_InDevsCombo.GetCount() > 0)
    {
        if(m_InDevId != m_InDevsCombo.GetCurSel())
        {
            m_InDevId = m_InDevsCombo.GetCurSel();
            m_InChanged = true;
        }
    }
	
	CDialog::OnOK();
}
