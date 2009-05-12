#if !defined(AFX_MIDIKEYBOARD_H__51B0338E_1002_11D7_865D_0030BD08B6D9__INCLUDED_)
#define AFX_MIDIKEYBOARD_H__51B0338E_1002_11D7_865D_0030BD08B6D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MIDIKeyboard.h : header file
//

#include <afxtempl.h>
#include "PianoCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CMIDIKeyboard window

class CMIDIKeyboard : public CPianoCtrl
{
// Construction
public:
	CMIDIKeyboard();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMIDIKeyboard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMIDIKeyboard();

private:
    // Convert key to note
    BOOL KeyToNote(UINT Char, unsigned char &NoteId);

public:

    static const unsigned char LOW_NOTE;
    static const unsigned char HIGH_NOTE;

	// Generated message map functions
protected:
	//{{AFX_MSG(CMIDIKeyboard)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void NotifyKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void NotifyKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

private:
    // Map for holding key/note pairs
    CMap<UINT, UINT, unsigned char, unsigned char> m_KeyMap;

    // Octave
    unsigned char m_Oct;

    // Key down flag
    static const unsigned short KEY_DOWN;

    // Notes per octave
    static const unsigned char NOTE_PER_OCTAVE;

    // Size of key/note map
    static const int MAP_SIZE;

public:
    inline int GetCurrentOctave() const { return m_Oct; }
    inline void SetCurrentOctave(int oct) 
    { 
        m_Oct = (unsigned char) oct; 
        NotifyOctaveChange(m_Oct);
    }
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDIKEYBOARD_H__51B0338E_1002_11D7_865D_0030BD08B6D9__INCLUDED_)
