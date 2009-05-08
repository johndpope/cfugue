// MIDIKeyboard.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMIDIKeyboard


// Low note end of the piano control range
const unsigned char CMIDIKeyboard::LOW_NOTE = 36;

// High note end of the piano control range
const unsigned char CMIDIKeyboard::HIGH_NOTE = 100;

// For determining if a key on the computer keyboard is already down
const unsigned short CMIDIKeyboard::KEY_DOWN = 16384;

// Note per octave
const unsigned char CMIDIKeyboard::NOTE_PER_OCTAVE = 12;

// Size of key map
const int CMIDIKeyboard::MAP_SIZE = 17;



CMIDIKeyboard::CMIDIKeyboard() :
m_KeyMap(MAP_SIZE),
m_Oct(0)
{
    //
    // Initialize key map for looking up keys and their note values
    //

    m_KeyMap.SetAt('Z', CPianoCtrl::C);
    m_KeyMap.SetAt('S', CPianoCtrl::C_SHARP);
    m_KeyMap.SetAt('X', CPianoCtrl::D);
    m_KeyMap.SetAt('D', CPianoCtrl::D_SHARP);
    m_KeyMap.SetAt('C', CPianoCtrl::E);
    m_KeyMap.SetAt('V', CPianoCtrl::F);
    m_KeyMap.SetAt('G', CPianoCtrl::F_SHARP);
    m_KeyMap.SetAt('B', CPianoCtrl::G);
    m_KeyMap.SetAt('H', CPianoCtrl::G_SHARP);
    m_KeyMap.SetAt('N', CPianoCtrl::A);
    m_KeyMap.SetAt('J', CPianoCtrl::A_SHARP);
    m_KeyMap.SetAt('M', CPianoCtrl::B);

    // An octave higher
    m_KeyMap.SetAt(0xBC, CPianoCtrl::C + NOTE_PER_OCTAVE);  // Key ,
    m_KeyMap.SetAt('L', CPianoCtrl::C_SHARP + NOTE_PER_OCTAVE);
    m_KeyMap.SetAt(0xBE, CPianoCtrl::D + NOTE_PER_OCTAVE); // Key .
    m_KeyMap.SetAt(0xBA, CPianoCtrl::D_SHARP + NOTE_PER_OCTAVE); // Key ;
    m_KeyMap.SetAt(0xBF, CPianoCtrl::E + NOTE_PER_OCTAVE); // Key /
}

CMIDIKeyboard::~CMIDIKeyboard()
{
}


// Convert key to note
BOOL CMIDIKeyboard::KeyToNote(UINT Char, unsigned char &NoteId)
{
    BOOL Result = m_KeyMap.Lookup(Char, NoteId);

    if(Result)
    {
        NoteId += m_Oct * NOTE_PER_OCTAVE + LOW_NOTE;
    }

    return Result;
}


BEGIN_MESSAGE_MAP(CMIDIKeyboard, CWnd)
	//{{AFX_MSG_MAP(CMIDIKeyboard)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMIDIKeyboard message handlers

UINT CMIDIKeyboard::OnGetDlgCode() 
{	
	return CWnd::OnGetDlgCode() | DLGC_WANTALLKEYS;
}


// When client presses a key
void CMIDIKeyboard::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
    {
    // Keys F1-F5 are for determining the octave setting
    case VK_F1: m_Oct = 0; break;
    case VK_F2: m_Oct = 1; break;
    case VK_F3: m_Oct = 2; break;
    case VK_F4: m_Oct = 3; break;
    case VK_F5: m_Oct = 4; break;  
    case VK_TAB: m_Oct++; if(m_Oct > 4) m_Oct = 4; break; // Increase one Octave
    case VK_BACK: if(m_Oct > 0) m_Oct--; break; // Decrease one Octave

    default:
        // Make sure the key isn't already down
        if(!(nFlags & KEY_DOWN))
        {
            unsigned char NoteId;

            // If the key is in range of the piano control, play note
            if(KeyToNote(nChar, NoteId))
            {
                NoteOn(NoteId);
            }
        }
        break;
    }
	
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


// Client lifts a key up
void CMIDIKeyboard::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	unsigned char NoteId;

    if(KeyToNote(nChar, NoteId))
    {
        NoteOff(NoteId);
    }
	
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

//
// Messages passed to the parent control
//

void CMIDIKeyboard::OnPaint() 
{	
    CPianoCtrl::OnPaint();
}

void CMIDIKeyboard::OnLButtonDown(UINT nFlags, CPoint point) 
{
    CPianoCtrl::OnLButtonDown(nFlags, point);
}

void CMIDIKeyboard::OnLButtonUp(UINT nFlags, CPoint point) 
{
    CPianoCtrl::OnLButtonUp(nFlags, point);
}

void CMIDIKeyboard::OnMouseMove(UINT nFlags, CPoint point) 
{
    CPianoCtrl::OnMouseMove(nFlags, point);
}

