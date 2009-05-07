// PlayByEarDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CPlayByEarDlg dialog
class CPlayByEarDlg : public CDialog, public CPianoCtrlListener,
                 public midi::CMIDIReceiver
{
// Construction
public:
	CPlayByEarDlg(CWnd* pParent = NULL);	// standard constructor

    void OnNoteOn(CPianoCtrl &PianoCtrl, unsigned char NoteId);
    void OnNoteOff(CPianoCtrl &PianoCtrl, unsigned char NoteId);

    // Receives short messages
    void ReceiveMsg(DWORD Msg, DWORD TimeStamp);

    // Receives long messages
    void ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp) {}

    // Called when an invalid short message is received
    void OnError(DWORD Msg, DWORD TimeStamp);

    // Called when an invalid long message is received
    void OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp) {}

// Dialog Data
	enum { IDD = IDD_PLAYBYEAR_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
    CMIDIKeyboard m_Keys;
    midi::CMIDIOutDevice m_OutDevice;
    midi::CMIDIInDevice m_InDevice;

    enum MusicMode {CARNATIC = 0, WESTERN = 1};
    MusicMode m_Mode; // Western or Carnatic

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPrefNoteColor();
	afx_msg void OnPrefMididevices();
	afx_msg void OnFileExit();
    afx_msg void OnSelchangeGmList();
	afx_msg void OnHelpAbout();
	DECLARE_MESSAGE_MAP()

protected:
    virtual void OnCancel();
public:
    afx_msg void OnClose();
protected:
    virtual void OnOK();
    void AdjustDisplayForMode();
public:
    CComboBox m_GMCombo;
    afx_msg void OnShowHideKeyNames();
    afx_msg void OnPrefCarnaticMusicMode();
    afx_msg void OnPrefWesternMusicMode();
};
