/*

  CPianoCtrl.cpp

  This file represents the implementation for the CPianoCtrl class

  Copyright (c) 2008 Leslie Sanford

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    Contact: Leslie Sanford (jabberdabber@hotmail.com)

    // Modified: To Confine the Cursor with in the Paino control when mouse pressed
*/

//---------------------------------------------------------------------
// Dependencies
//---------------------------------------------------------------------


#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPianoCtrl


//---------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------


// Class name for this control
const TCHAR CPianoCtrl::CLASS_NAME[] = _T("PianoCtrl");

// Default note-on color
const COLORREF CPianoCtrl::DEF_NOTE_ON_COLOR = RGB(0, 150, 250);

// Maximum number of notes
const int CPianoCtrl::MAX_NOTE_COUNT = 128;

// Note table 
const int CPianoCtrl::NOTE_TABLE[] = 
{ 
    C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, A_SHARP, B,
    C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, A_SHARP, B,
    C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, A_SHARP, B,
    C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, A_SHARP, B,
    C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, A_SHARP, B,
    C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, A_SHARP, B,
    C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, A_SHARP, B,
    C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, A_SHARP, B,
    C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, A_SHARP, B,
    C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, A_SHARP, B,
    C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G
};

// Range error messages
const CString CPianoCtrl::NOTE_RANGE_ERR = _T("Note out of range.");
const CString CPianoCtrl::LOW_NOTE_RANGE_ERR = _T("Low note out of range.");
const CString CPianoCtrl::HIGH_NOTE_RANGE_ERR = _T("High note out of range.");
const CString CPianoCtrl::INVALID_RANGE_ERR = _T("Note range is invalid.");
const CString CPianoCtrl::NATURAL_NOTE_ERR = _T("Notes must be natural.");

// Flags whether or not this custom control class has been registered.
bool CPianoCtrl::REGISTER_FLAG = false;
  

//---------------------------------------------------------------------
// CPianoCtrl class implementation
//---------------------------------------------------------------------


// Constructor
CPianoCtrl::CPianoCtrl() : 
m_Keys(0),
m_LowNote(0),
m_HighNote(0),
m_CurrKey(-1),
m_IsInitialized(false),
m_HasCapture(false)
{
    // If this control has not been registered, do it here.
    if(!REGISTER_FLAG)
    {
        RegisterWindowClass();
    }

    // Initialize critical section
    ::InitializeCriticalSection(&m_CriticalSection);
}


// Destructor
CPianoCtrl::~CPianoCtrl()
{
    DestroyPianoKeys();

    ::DeleteCriticalSection(&m_CriticalSection);
}


// Create function for creating the control dynamically
BOOL CPianoCtrl::Create(CWnd *pParentWnd, const RECT &rect, UINT nId, 
                        DWORD dwStyle)
{
    return CWnd::Create(CLASS_NAME, _T(""), dwStyle, rect, pParentWnd, 
                        nId);
}


// Initializes the control
BOOL CPianoCtrl::Initialize(unsigned char LowNote, 
                            unsigned char HighNote,
                            COLORREF NoteOnColor)
{
    CRect ClientRect;
    
    // Get the rectangular coordinates of this control and get its 
    // width and length
    GetClientRect(&ClientRect);
    m_Width = ClientRect.bottom;
    m_Length = ClientRect.right - 1;

    // Initialize note-on color
    m_NoteOnColor = NoteOnColor;

    // If we successfully set the note range, we are finished 
    // initializing this control. Indicate that it has been initialized.
    if(SetNoteRange(LowNote, HighNote))
    {
        m_IsInitialized = true;
    }

    return TRUE ? m_IsInitialized : FALSE;
}


// Turn on a note 
void CPianoCtrl::NoteOn(unsigned char NoteId, COLORREF NoteOnColor)
{
    // Make sure this control has been initialized
    if(m_IsInitialized)
    {
        // Range checking
        if(NoteId < m_LowNote || NoteId > m_HighNote)
        {
            MessageBox(NOTE_RANGE_ERR, _T("Error"), MB_OK | MB_ICONSTOP);
        }
        // Turn on note and invalidate its area so that it is repainted.
        else
        {
            CRect InvalidRect;

            m_Keys[NoteId - m_LowNote]->NoteOn(NoteOnColor, InvalidRect);
            NotifyNoteOn(NoteId);

            InvalidateRect(InvalidRect);
        }
    }
}


// Turn on note
void CPianoCtrl::NoteOn(unsigned char NoteId)
{
    NoteOn(NoteId, m_NoteOnColor);
}


// Turn off note
void CPianoCtrl::NoteOff(unsigned char NoteId)
{
    // Make sure this control has been initialized
    if(m_IsInitialized)
    {
        // Range checking
        if(NoteId < m_LowNote || NoteId > m_HighNote)
        {
            MessageBox(NOTE_RANGE_ERR, _T("Error"), MB_OK | MB_ICONSTOP);
        }
        // Turn note off and invalidate its area so that it can be 
        // repainted
        else
        {
            CRect InvalidRect;

            m_Keys[NoteId - m_LowNote]->NoteOff(InvalidRect);
            NotifyNoteOff(NoteId);

            InvalidateRect(InvalidRect);
        }
    }
}


// Attach listener for event notification
void CPianoCtrl::AttachListener(CPianoCtrlListener &Listener)
{
    // Protect list with critical section
    ::EnterCriticalSection(&m_CriticalSection);

    m_Listeners.push_front(&Listener);

    ::LeaveCriticalSection(&m_CriticalSection);
}


// Detach listener
void CPianoCtrl::DetachListener(CPianoCtrlListener &Listener)
{
    ::EnterCriticalSection(&m_CriticalSection);

    m_Listeners.remove(&Listener);

    ::LeaveCriticalSection(&m_CriticalSection);
}


// Set note range
BOOL CPianoCtrl::SetNoteRange(unsigned char LowNote, 
                              unsigned char HighNote)
{
    BOOL Result = FALSE;

    // Range checking
    if(LowNote < 0)
    {
        MessageBox(LOW_NOTE_RANGE_ERR, _T("Error"), MB_OK | MB_ICONSTOP);
    }
    // High note beyond maximum number allowed
    else if(HighNote >= MAX_NOTE_COUNT)
    {
        MessageBox(HIGH_NOTE_RANGE_ERR, _T("Error"), MB_OK | MB_ICONSTOP);
    }
    // Low note is higher than high note
    else if(LowNote >= HighNote)
    {
        MessageBox(INVALID_RANGE_ERR, _T("Error"), MB_OK | MB_ICONSTOP);
    }
    // Either the low note or the high note is not natural
    else if(!IsNoteNatural(LowNote) || !IsNoteNatural(HighNote))
    {
        MessageBox(NATURAL_NOTE_ERR, _T("Error"), MB_OK | MB_ICONSTOP);
    }
    // Initialize range and create keys
    else
    {
        m_LowNote = LowNote;
        m_HighNote = HighNote;

        CreatePianoKeys();

        // Indicate that the operation was successful
        Result = TRUE;
    }

    return Result;
}
    

// Determines if a note is natural
bool CPianoCtrl::IsNoteNatural(unsigned char Note)
{
    bool Result = true;

    if(NOTE_TABLE[Note] == C_SHARP || NOTE_TABLE[Note] == D_SHARP ||
        NOTE_TABLE[Note] == F_SHARP || NOTE_TABLE[Note] == G_SHARP ||
        NOTE_TABLE[Note] == A_SHARP)
    {
        Result = false;
    }

    return Result;
}


// Get the number of natural notes in this control's range
int CPianoCtrl::GetNaturalNoteCount()
{
    int NatNoteCount = 0;

    for(unsigned char i = m_LowNote; i <= m_HighNote; i++)
    {
        if(IsNoteNatural(i))
        {
            NatNoteCount++;
        }
    }

    return NatNoteCount;
}


// Calculate the unit length for this control
double CPianoCtrl::GetUnitLength()
{
    // Determine the number of natural notes within this note range
    int NatNoteCount = GetNaturalNoteCount();

    double Result = static_cast<double>(m_Length) / NatNoteCount;

    // Return the length of each unit
    return Result / CPianoKey::UNIT_PER_NAT_KEY;
}


// Create the piano keys
void CPianoCtrl::CreatePianoKeys()
{
    double Position = 0;

    DestroyPianoKeys();

    m_Keys.resize(m_HighNote - m_LowNote + 1);

    m_UnitLength = GetUnitLength();

    //
    // The leftmost key is a special case. Take care of it first.
    //

    if(NOTE_TABLE[m_LowNote] == B || NOTE_TABLE[m_LowNote] == E)
    {
        m_Keys[0] = new CWhiteKeyFull(m_UnitLength, m_Width, 0);

        // Move position to the right
        Position = m_UnitLength * CPianoKey::UNIT_PER_NAT_KEY;
    }
    else
    {
        m_Keys[0] = new CWhiteKeyLeft(m_UnitLength, m_Width, 0);

        // Move position to the right
        Position = m_UnitLength * (CPianoKey::UNIT_PER_NAT_KEY - 1);
    }

    //
    // Create all but the rightmost key. 
    //

    for(int i = m_LowNote + 1; i < m_HighNote; i++)
    {
        // CWhiteKeyLeft cases
        if(NOTE_TABLE[i] == C || NOTE_TABLE[i] == F)
        {          
            m_Keys[i - m_LowNote] = new CWhiteKeyLeft(m_UnitLength, 
                m_Width, Position);

            // Move position to the right
            Position += m_UnitLength * (CPianoKey::UNIT_PER_NAT_KEY - 1);
        }
        // CWhiteKeyMiddle cases
        else if(NOTE_TABLE[i] == G || NOTE_TABLE[i] == A ||
            NOTE_TABLE[i] == D)
        {
            m_Keys[i - m_LowNote] = new CWhiteKeyMiddle(m_UnitLength, 
                m_Width, Position);

            // Move position to the right
            Position += m_UnitLength * (CPianoKey::UNIT_PER_NAT_KEY - 1);
        }
        // CWhiteKeyRight cases
        else if(NOTE_TABLE[i] == B || NOTE_TABLE[i] == E)
        {
            m_Keys[i - m_LowNote] = new CWhiteKeyRight(m_UnitLength, 
                m_Width, Position);

            // Move position to the right
            Position += m_UnitLength * (CPianoKey::UNIT_PER_NAT_KEY);
        }
        // CBlackKey cases
        else
        {
            m_Keys[i - m_LowNote] = new CBlackKey(m_UnitLength, 
                m_Width, Position);

            // Move position to the right
            Position += m_UnitLength;
        }
    }

    //
    // The rightmost key is a special case. Take care of it last.
    //

    if(NOTE_TABLE[m_HighNote] == C || 
        NOTE_TABLE[m_HighNote] == F)
    {
        m_Keys[m_HighNote - m_LowNote] = new 
            CWhiteKeyFull(m_UnitLength, m_Width, Position);
    }
    else
    {
        m_Keys[m_HighNote - m_LowNote] = new 
            CWhiteKeyRight(m_UnitLength, m_Width, Position);
    }

    // Paint new keyboard
    Invalidate();
}


// Destroy the piano keys
void CPianoCtrl::DestroyPianoKeys()
{
    // Make sure there are keys to destroy
    if(m_Keys.size() > 0)
    {
        for(unsigned int i = 0; i < m_Keys.size(); i++)
        {
            delete m_Keys[i];
        }
    }

    // Resise the container to 0 indicating that there are no keys in 
    // the control at this time
    m_Keys.resize(0);
}


// Find a key in this control
int CPianoCtrl::FindKey(CPoint &point)
{
    bool FoundFlag = false;
    int Key = 0;

    // This algorithm does a simple linear search for the key that was
    // pressed. This is not an efficient way to search. However, it's
    // the simplest and works well enough in practise for this situation.
    while(!FoundFlag && Key < m_Keys.size() - 1)
    {
        if(m_Keys[Key]->IsPointInKey(point))
        {
            FoundFlag = true;
        }
        else
        {
            Key++;
        }
    }

    return Key;
}


// Notify all listeners that a note-on event has occurred
void CPianoCtrl::NotifyNoteOn(unsigned char NoteId)
{
    // Protect list with critical section
    ::EnterCriticalSection(&m_CriticalSection);

    std::list<CPianoCtrlListener *>::iterator i;

    for(i = m_Listeners.begin(); i != m_Listeners.end(); i++)
    {
        (*i)->OnNoteOn(*this, NoteId);
    }

    ::LeaveCriticalSection(&m_CriticalSection);
}


// Notify listeners that a note-off event has occurred
void CPianoCtrl::NotifyNoteOff(unsigned char NoteId)
{
    // Protect list with critical section
    ::EnterCriticalSection(&m_CriticalSection);

    std::list<CPianoCtrlListener *>::iterator i;

    for(i = m_Listeners.begin(); i != m_Listeners.end(); i++)
    {
        (*i)->OnNoteOff(*this, NoteId);
    }

    ::LeaveCriticalSection(&m_CriticalSection);
}

// Notify listeners that the Active Octave for Keyboard Input has occurred
void CPianoCtrl::NotifyOctaveChange(unsigned char newOctave)
{
    // Protect list with critical section
    ::EnterCriticalSection(&m_CriticalSection);

    std::list<CPianoCtrlListener *>::iterator i;

    for(i = m_Listeners.begin(); i != m_Listeners.end(); i++)
    {
        (*i)->OnActiveOctaveChanged(*this, newOctave);
    }

    ::LeaveCriticalSection(&m_CriticalSection);
}

// Register this controls Window class
void CPianoCtrl::RegisterWindowClass()
{
    WNDCLASS WndClass;
    HINSTANCE hInst = ::AfxGetInstanceHandle();
    HCURSOR Cursor = ::AfxGetApp()->LoadStandardCursor(IDC_ARROW);

    WndClass.style         = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc   = ::DefWindowProc;
    WndClass.cbClsExtra    = WndClass.cbWndExtra = 0;
    WndClass.hInstance     = hInst;
    WndClass.hIcon         = NULL;
    WndClass.hCursor       = Cursor;
    WndClass.hbrBackground = NULL;
    WndClass.lpszMenuName  = NULL;
    WndClass.lpszClassName = CLASS_NAME;

    if (::AfxRegisterClass(&WndClass))
    {
        // Indicate that this Window class has now been registered
        REGISTER_FLAG = true;
    }
    else
    {
        ::AfxThrowResourceException();
    }
}


BEGIN_MESSAGE_MAP(CPianoCtrl, CWnd)
	//{{AFX_MSG_MAP(CPianoCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPianoCtrl message handlers


// Paints the control
void CPianoCtrl::OnPaint() 
{
	PAINTSTRUCT ps;
    CDC *dc = BeginPaint(&ps);

    for(int i = 0; i < m_Keys.size(); i++)
    {
        m_Keys[i]->Paint(dc);
    }

    EndPaint(&ps);
}


// On left mouse button down
void CPianoCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
    // Capture the mouse if we haven't already
    if(!m_HasCapture)
    {
        SetCapture();
        m_HasCapture = true;
    }

    // Confine the cursor to this window region till button up 
    RECT rect = {4,4,m_Length-4, m_Width-4};
    this->ClientToScreen(&rect);
    ClipCursor(&rect);

    // Make sure this control has been initialized
    if(m_IsInitialized)
    {
        // Find the key to turn on. This will become the current key. 
        // We keep track of it so that we can turn it off later
        m_CurrKey = FindKey(point);

        // Turn on note, get its area, and then invalidate so that the
        // control is repainted to indicate that this note is being 
        // played
        CRect InvalidRect;
        m_Keys[m_CurrKey]->NoteOn(m_NoteOnColor, InvalidRect);
        NotifyNoteOn(m_CurrKey + m_LowNote);    

        InvalidateRect(&InvalidRect);
    }
	
	CWnd::OnLButtonDown(nFlags, point);
}


// On left mouse button up
void CPianoCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
    // If there is a key already being played by the mouse, turn it off
    if(m_CurrKey >= 0)
    {
        CRect InvalidRect;
        m_Keys[m_CurrKey]->NoteOff(InvalidRect);
        NotifyNoteOff(m_CurrKey + m_LowNote);

        // Indicate that there is no key active
        m_CurrKey = -1;

        InvalidateRect(&InvalidRect);
    }

    // Leave the confinement and make it free
    ClipCursor(NULL);
	
	CWnd::OnLButtonUp(nFlags, point);
}


// On mouse move
void CPianoCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
    // If the mouse has moved outside of this window, turn off any 
    // currently active key and release the mouse.
    if(point.x <= 0 || point.x >= m_Length || point.y <= 0 || 
        point.y >= m_Width)
    {   
        // If there is a key currently active key
        if(m_CurrKey >= 0)
        {
            CRect InvalidRect;

            m_Keys[m_CurrKey]->NoteOff(InvalidRect);
            NotifyNoteOff(m_CurrKey + m_LowNote);
            InvalidateRect(&InvalidRect);
        }

        ReleaseCapture();
        m_HasCapture = false;
    }

    // If this piano control has been intialized, and it has captured the
    // mouse, and the left mouse button is down...
    if(m_IsInitialized && m_HasCapture && (nFlags & MK_LBUTTON))
    {
        // Find the key beneath the mouse cursor
        int Key = FindKey(point); 

        // If the mouse has moved to a new key
        if(Key != m_CurrKey)
        {
            CRect InvalidRect;

            // If the current key is valid, turn note off
            if(m_CurrKey >= 0)
            {               
                m_Keys[m_CurrKey]->NoteOff(InvalidRect);
                NotifyNoteOff(m_CurrKey + m_LowNote);
                InvalidateRect(&InvalidRect);
            }

            // Turn new note on
            m_CurrKey = Key;
            m_Keys[m_CurrKey]->NoteOn(m_NoteOnColor, InvalidRect);
            NotifyNoteOn(m_CurrKey + m_LowNote);

            InvalidateRect(&InvalidRect);
        }
    }
            
	CWnd::OnMouseMove(nFlags, point);
}




