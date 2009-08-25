// PlayByEarDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "afxmt.h"
#include "QASession.h"

// CPlayByEarDlg dialog
class CPlayByEarDlg : public CDialog, public CPianoCtrlListener,
                      public midi::CMIDIReceiver, public OIL::CEventReceiver
{
// Construction
public:
	CPlayByEarDlg(CWnd* pParent = NULL);	// standard constructor

    void OnNoteOn(CPianoCtrl &PianoCtrl, unsigned char NoteId);
    void OnNoteOff(CPianoCtrl &PianoCtrl, unsigned char NoteId);
    void OnActiveOctaveChanged(CPianoCtrl &PianoCtrl, unsigned char newOctave);
    void OnKeyUp(CPianoCtrl &PianoCtrl, UINT nChar, UINT nRepCnt, UINT nFlags);


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
public:
	HICON m_hIcon;
    CMIDIKeyboard m_Keys;
    midi::CMIDIOutDevice m_OutDevice;
    midi::CMIDIInDevice m_InDevice;

    enum MusicMode {CARNATIC = 0, WESTERN = 1};
    MusicMode m_Mode; // Western or Carnatic
    BOOL m_bDisplayKeyNames; // Show / Hide the key names on the Control

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
    CString ConvertAnswerNotesToString(const CQASession::NOTES& AnswerNotes);

    afx_msg void OnShowHideKeyNames();
    afx_msg void OnPrefCarnaticMusicMode();
    afx_msg void OnPrefWesternMusicMode();
    afx_msg void OnShowKeyBindings();
    afx_msg void OnBnClickedRadioLevel1();
    afx_msg void OnBnClickedRadioLevel2();
    afx_msg void OnBnClickedRadioLevel3();
    afx_msg void OnBnClickedRadioLevel4();
    afx_msg void OnBnClickedRadioLevel5();
    afx_msg void OnNMClickSyslinkSubmit(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickSyslinkReplay(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickSyslinkNextquestion(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickSyslinkPlayAnswer(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnHelpHowdoi();

    void OnQASessionComplete(const OIL::CEventSource* pSender, OIL::CEventHandlerArgs* pArgs);
    void OnAnswerPlayComplete(const OIL::CEventSource* pSender, OIL::CEventHandlerArgs* pArgs);

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnFileExitapplication();
    afx_msg void OnTestStart();
    afx_msg void OnTestStop();
    afx_msg void OnSelchangeRagaList();
private:
public:
    afx_msg void OnTestWaitBeforeRetry();
    afx_msg void OnTestWaitBeforeNewQuestion();
public:
    // Answer Thread Related data
	::CEvent  m_evExitPlayThread; // set by Dlg to Signale the exit of Thread
    ::CEvent  m_evPlayNotes; // set by Dlg to Signale the Play of notes
    ::CEvent  m_evStopNotes; // set by Dlg to Signale Stop Playing the notes
    bool    m_bPlaying; // Set by AnswerThread
    CQASession::NOTES m_NotesToPlay; // Read by AnswerThread when requested for Play
    OIL::CInvokableEvent m_evPlayComplete; // Raised by AnswerPlayThread when Play is complete
protected:
    CQhtmWnd    m_ctrlInfo;
    UINT        m_nTimer;
    CQASession  m_QASession;
    CComboBox   m_GMCombo;
    CComboBox   m_RagaListCombo;
	UINT		m_bConnectMIDIOut;	// Indicates if MIDIOut should be connected or not
	UINT		m_bConnectMIDIIn;	// Indicates if MIDIIn should be connected or not
};
