#if !defined(AFX_PIANOCTRL_H__C84F71CE_FF29_11D6_865D_0030BD08B6D9__INCLUDED_)
#define AFX_PIANOCTRL_H__C84F71CE_FF29_11D6_865D_0030BD08B6D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PianoCtrl.h : header file
//

/*

  PianoCtrl.h

  This header file contains the class declaration for the CPianoCtrl and
  its associated classes. The CPianoCtrl class is a custom MFC control 
  representing a piano keyboard display.

  Copyright (C) 2002 Leslie Sanford

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
  USA

  Contact: Leslie Sanford (jabberdabber@hotmail.com)

  Last modified: 12/10/2002

*/


//---------------------------------------------------------------------
// Dependencies
//---------------------------------------------------------------------


#include <list>         // For holding the list of CPianoCtrlListeners
#include <vector>       // For holding the CPianoKey objects


//---------------------------------------------------------------------
// CPianoCtrlListener class
//
// This class represents an observer class. It receives notification
// for note on and note off events from the CPianoCtrl class. 
//---------------------------------------------------------------------


// Forward declaration
class CPianoCtrl;


class CPianoCtrlListener
{
public:
    virtual ~CPianoCtrlListener() {}

    virtual void OnNoteOn(CPianoCtrl &PianoCtrl, 
                          unsigned char NoteId) = 0;
    virtual void OnNoteOff(CPianoCtrl &PianoCtrl,
                           unsigned char NoteId) = 0;
    // Raised when the keyboard active octave is changed
    virtual void OnActiveOctaveChanged(CPianoCtrl &PianoCtrl,
                           unsigned char newOctave)
    { }
    // Raised when the KeyUp event is received
    virtual void OnKeyUp(CPianoCtrl &PianoCtrl,
                           UINT nChar, UINT nRepCnt, UINT nFlags)
    { }
    // Raised when the KeyDown event is received
    virtual void OnKeyDown(CPianoCtrl &PianoCtrl,
                           UINT nChar, UINT nRepCnt, UINT nFlags)
    { }
};


//---------------------------------------------------------------------
// CPianoCtrl class
//
// This class represents a piano keyboard. 
//---------------------------------------------------------------------
//
// To use the CPianoCtrl class in a dialog based application, use the 
// following steps:
//
//  1. In the resource editor, place a custom control onto the dialog
//     box.
//
//  2. Set the class name in the custom control's property box to match 
//     the CPianoCtrl's class name.
//
//  3. Add a CPianoCtrl instance variable to the dialog class.
//
//  4. Add a DDX_Control call in the DoDataExchange method in the dialog 
//     class. For example, if your dialog class was named CPianoDlg, the 
//     CPianoCtrl variable was named m_PianoCtrl, and the Id for the
//     control was IDC_PIANOCTRL, you would place the following in the
//     dialog class' DoDataExchange function:
//
//      void CPianoDlg::DoDataExchange(CDataExchange* pDX)
//      {
//      	CDialog::DoDataExchange(pDX);
//	        //{{AFX_DATA_MAP(CPianoDlg)
//		    // NOTE: the ClassWizard will add DDX and DDV calls here
//	        //}}AFX_DATA_MAP
//          DDX_Control(pDX, IDC_PIANOCTRL, m_PianoCtrl);
//      }
//
//  5. In the dialog class' OnInitDialog function, initialize the 
//     CPianoCtrl object by calling its Initialize function. Here, you 
//     will pass the desired note range and optionally the note-on 
//     color. 
//
// To use the control dynamically within a dialog box or within a window,
// use the following steps:
//
//  1. Add a CPianoCtrl instance variable to your class. This can be a 
//     pointer to a CPianoCtrl, but if so, you will need to allocate
//     memory for it before using it and deallocate its memory after you
//     are done with it.
//
//  2. Call the CPianoCtrl object's Create function. Here, you will 
//     pass the parent window, the CPianoCtrl's position and size, its
//     Id, and optionally its window style.
//
//  3. Call the CPianoCtrl object's Initialize function. Here, you will 
//     pass the desired note range and optionally the note-on color. 
//
//---------------------------------------------------------------------
//
// Some important notes concerning the CPianoCtrl class:
//
//  It can have up to 128 keys (0 - 127). 
//
//  The range is set with the Initialize method or the SetNoteRange
//  method. You specify the lowest note of the range and the highest 
//  note of the range. The lowest note must be less than the highest 
//  note and both the lowest note and the highest note must be natural.
//
//  The number 0 is considered a C note. As you go up from 0, you are 
//  ascending the chromatic scale. Therefore, 0 equals C, 1 equals C#,
//  2 equals D, etc. After you've reached the B note (the first B note 
//  is number 11), the scale starts over from C.
//
//  To be notified of note on and note off events, derive a class from
//  CPianoCtrlListener. Implement its methods in your derived class 
//  and attach an object of that class to a CPianoCtrl object with the
//  AttachListener method. When the CPianoCtrl object receives note on
//  and note off events, it will notify all of its listeners.
//
//---------------------------------------------------------------------  
//
// CPianoCtrl Member Methods
//
//---------------------------------------------------------------------
//
//  Construction
//
//      CPianoCtrl()      
//
//          Constructs a CPianoCtrl object. It's important to note that
//          the object has not been fully constructed at this point.
//          A call to the Initialize method is still needed to ready
//          the object for use.
//
//      BOOL Create(CWnd *pParentWnd, const RECT &rect, UINT nID, 
//                  DWORD dwStyle = WS_VISIBLE)   
//
//          Creates an CPianoCtrl object dynamically. Returns TRUE if 
//          the operation was successful.
//
//              pParentWnd - Parent window for this control.
//              rect       - Rectangular coordinates for this control.
//              nID        - Identifier for this control.
//              dwStyle    - The window style for this control.   
//
//      BOOL Initialize(unsigned char LowNote, unsigned char HighNote,
//                      COLORREF NoteOnColor = DEF_NOTE_ON_COLOR)
//
//          Initializes this control. Returns true if the operation was
//          successful.
//
//              LowNote     - The lowest note on the keyboard. Must be 
//                            less than the highest note.
//              HighNote    - The highest note on the keyboard. Must be 
//                            less than MAX_NOTE_COUNT.
//              NoteOnColor - The color used to indicate that a key is 
//                            is being played.
//
//---------------------------------------------------------------------
//
//  Operations
//
//      void NoteOn(COLORREF NoteOnColor, unsigned char NoteId)
//      void NoteOn(unsigned char NoteId)
//
//          Turns a note on. 
//
//              NoteOnColor - The color indicating that this key is 
//                            being played.
//              NoteId      - The note to play.       
//
//      void NoteOff(unsigned char NoteId)
//
//          Turns a note off.
//
//              NoteId - The note to stop playing. 
//
//      void AttachListener(CPianoCtrlListener &Listener)
//      void DetachListener(CPianoCtrlListener &Listener)
//
//          Attaches/Detaches CPianoCtrlListener objects. When an 
//          CPianoCtrlListener object is attached to a CPianoCtrl 
//          object, the CPianoCtrl object will notify it when note-on 
//          and note-off events occur.
//
//              Listener - The CPianoCtrlListener to attach/detach.
//
//---------------------------------------------------------------------
//
//  Attributes
//
//      unsigned char GetLowNote() const
//
//          Returns the lowest note for this CPianoCtrl object.      
//
//      unsigned char GetHighNote() const
//
//          Returns the highest note for this CPianoCtrl object.
//
//      BOOL SetNoteRange(unsigned char LowNote, 
//                        unsigned char HighNote)
//      
//          Sets the note range for this CPianoCtrl object. Returns
//          TRUE if the operation was successful.
//
//              LowNote  - The lowest note on the keyboard. Must be 
//                         less than the highest note.
//              HighNote - The highest note on the keyboard. Must be 
//                         less than MAX_NOTE_COUNT.
//
//      COLORREF GetNoteOnColor() const
//
//          Returns the color that indicates a key is being played.
//
//      void SetNoteOnColor(COLORREF NoteOnColor) 
//
//          Sets the color to indicate a key is being played.
//
//              NoteOnColor - The color indicating that a key is 
//                            being played.  
//   
//---------------------------------------------------------------------


class CPianoCtrl : public CWnd
{
public:
    // Construction/Destruction
	CPianoCtrl();
    virtual ~CPianoCtrl();

    // Creates the CPianoCtrl
    BOOL Create(CWnd *pParentWnd, const RECT &rect, UINT nID, 
                DWORD dwStyle = WS_VISIBLE);

    // Initializes the CPianoCtrl - must be called before this 
    // CPianoCtrl object can be used.
    BOOL Initialize(unsigned char LowNote, unsigned char HighNote,
                    COLORREF NoteOnColor = DEF_NOTE_ON_COLOR);

    // Turns note on
    void NoteOn(unsigned char NoteId, COLORREF NoteOnColor);
    void NoteOn(unsigned char NoteId);

    // Turns note off
    void NoteOff(unsigned char NoteId);

    // Attach/Detach CPianoCtrlListener objects
    void AttachListener(CPianoCtrlListener &Listener);
    void DetachListener(CPianoCtrlListener &Listener);

    //
    // Accessors/Mutators
    //

    unsigned char GetLowNote() const { return m_LowNote; }
    unsigned char GetHighNote() const { return m_HighNote; }
    BOOL SetNoteRange(unsigned char LowNote, unsigned char HighNote);

    COLORREF GetNoteOnColor() const { return m_NoteOnColor; }
    void SetNoteOnColor(COLORREF NoteOnColor) 
        { m_NoteOnColor = NoteOnColor; }

protected:
    // Notify CPianoCtrlListener objects that a note on/off event has
    // occurred.
    void NotifyNoteOn(unsigned char NoteId);
    void NotifyNoteOff(unsigned char NoteId);
    // Notify about changes to the Active Octave for Keyboard Input
    void NotifyOctaveChange(unsigned char newOctave);

// Private functions
private:
    bool IsNoteNatural(unsigned char Note);
    int GetNaturalNoteCount();
    double GetUnitLength();

    void CreatePianoKeys();
    void DestroyPianoKeys();

    int FindKey(CPoint &point);


    // Register this control's window class
    static void RegisterWindowClass();

    //
    // Copying not allowed
    //

    CPianoCtrl(const CPianoCtrl &PianoCtrl);
    CPianoCtrl &operator = (const CPianoCtrl &PianoCtrl);

// Private classes
private:

    //------------------------------------------------------------------
    // Piano key classes
    //
    // The following classes represent the keys on the CPianoCtrl class. 
    //
    //------------------------------------------------------------------
    //
    // To understand how these classes work, first imagine a piano 
    // keyboard display with a range of 12 keys from C to B:
    //
    //                  ----------------------
    //                  | | || | | | || || | |
    //                  | |_||_| | |_||_||_| |
    //                  |  |  |  |  |  |  |  |
    //                  ----------------------
    //
    // We can divide the keys into 4 types:
    //
    //                  ----------------------
    //                  | | || | | | || || | |
    //                  | |_||_| | |_||_||_| |
    //                  |  |  |  |  |  |  |  |
    //                  ----------------------
    //                  /|\      /|\
    //                   |        | 
    //
    //                  White key left (C and F)
    //
    //
    //                  ----------------------
    //                  | | || | | | || || | |
    //                  | |_||_| | |_||_||_| |
    //                  |  |  |  |  |  |  |  |
    //                  ----------------------
    //                     /|\      /|\ /|\
    //                      |        |   |
    //
    //                White key middle (D, G, and A)
    //
    //
    //                  ----------------------
    //                  | | || | | | || || | |
    //                  | |_||_| | |_||_||_| |
    //                  |  |  |  |  |  |  |  |
    //                  ----------------------
    //                        /|\          /|\ 
    //                         |            |
    //
    //                  White key right (E and B)
    //
    //
    //                     |  |     |  |  |
    //                    \|/\|/   \|/\|/\|/
    //                  ----------------------
    //                  | | || | | | || || | |
    //                  | |_||_| | |_||_||_| |
    //                  |  |  |  |  |  |  |  |
    //                  ----------------------
    //                 
    //            Black key (C#, D#, F#, G#, and A#)
    //
    //
    // There is also a fifth type of key. This is a special key used 
    // when certain ranges are set. For example, look at a range of keys
    // from C to C one octave higher:
    //
    //                  ------------------------
    //                  | | || | | | || || | | | |
    //                  | |_||_| | |_||_||_| | |_|
    //                  |  |  |  |  |  |  |  |  |
    //                  -------------------------
    //
    // A problem occurs because we have half of a black key dangling off
    // the side of the keyboard. This really isn't acceptable, so we 
    // need an additional key to take care of this special case, the
    // white full key:
    //
    //                  -------------------------
    //                  | | || | | | || || | |  |
    //                  | |_||_| | |_||_||_| |  |
    //                  |  |  |  |  |  |  |  |  |
    //                  -------------------------
    //                                        /|\
    //                                         |
    //
    //                        White full key
    //
    // The white full key takes care of those cases in which there would
    // otherwise be a black key chopped in half.
    //
    // The five types of piano keys are each represented by their own 
    // class. Each class knows how to draw itself so that it has the 
    // proper shape on the piano keyboard. All of the classes derive 
    // from one parent class called CPianoKey. This class provides the 
    // interface for all piano key classes.
    //
    // The CPianoCtrl class only allows ranges that begin and end with 
    // natural notes (white keys). This was a judgement call on my part.
    // Ranges in which the client chose a sharp/flat key as the lowest
    // or highest note would create a strange situation in which a white
    // key would be chopped in half:
    //
    //                    --------------------
    //                    | || | | | || || | |
    //                    |_||_| | |_||_||_| |
    //                    ||  |  |  |  |  |  |
    //                    --------------------
    //                   /|\
    //                    |
    //      
    //              Half-white key?
    //
    // This situation could certainly be dealt with, but if the client
    // specified that C# should be the lowest key, they would have to 
    // deal with the extra half-white key that would actually be the 
    // lowest note, in this case C, and not the one they had specified.
    // I decided this situation was more trouble than it was worth, so
    // I limited the low and high end of the range to natural notes.
    //
    //------------------------------------------------------------------
    //
    // The keys are divided into "units." Units are used to determine 
    // how large to draw each key. The length of a unit is represented 
    // by the total length of the control divided by the number of 
    // natural keys in the range divided by 3. 
    //
    // So if the length of the control is 100 and the number of natural
    // keys in the range is 12, than the unit length would be 2.77778.
    //
    // Here is how the unit length is used:
    //
    // All of the white keys at their longest part take up 3 units:
    //
    //                          ----------
    //                          | | || | |
    //                          | |_||_| |
    //                          |  |  |  |
    //                          ----------
    //                          |<>|<>|<>|
    //                          
    //
    //                         3 Units Long
    //
    // The black keys are 2 units long and the white keys at their 
    // narrowest part are also 2 units long.
    //
    //                         2 Units Long
    //
    //                          \/ \/ \/
    //                          ----------
    //                          | | || | |
    //                          | |_||_| |
    //                          |  |  |  |
    //                          ----------
    //
    // The unit length is passed to each key object so that it can know
    // how large to draw itself. The position on the control is also 
    // passed to itself so that it can know where to draw itself.
    //
    //-----------------------------------------------------------------


    // Parent piano key class
    class CPianoKey
    {
    public:
        CPianoKey() : m_NoteOnFlag(false) {}
        virtual ~CPianoKey() {}

        // Hit detection
        virtual bool IsPointInKey(const CPoint &pt) const = 0;

        // Turns note on/off
        virtual void NoteOn(COLORREF NoteOnColor, CRect &Rect) = 0;
        virtual void NoteOff(CRect &Rect) = 0;

        // Paints this key
        virtual void Paint(CDC *dc) = 0;

    public:

        // Units per natural key
        static const int UNIT_PER_NAT_KEY;

        // Determines black keys' width
        static const double BLACK_KEY_OFFSET;

    protected:
        // Function for determining if a point is inside a rectangle.
        // The CRect method PtInRect is inadequate in this context 
        // because it does not count a point on the bottom or on the 
        // right side of the rectangle as being within the rectangle.
        static bool IsPointInRect(const CPoint &pt, const CRect &Rect)
        { 
            return (pt.x >= Rect.left && pt.x <= Rect.right &&
                  pt.y >= Rect.top && pt.y <= Rect.bottom);
        }

    protected:
        bool m_NoteOnFlag;
        COLORREF m_NoteOnColor;
    };


    // CWhiteKeyLeft class
    class CWhiteKeyLeft : public CPianoKey
    {
    public:
        CWhiteKeyLeft(double UnitLength, int Width, double Position);

        bool IsPointInKey(const CPoint &pt) const;

        void NoteOn(COLORREF NoteOnColor, CRect &Rect);
        void NoteOff(CRect &Rect);

        void Paint(CDC *dc);

    private:
        void CalcInvalidRect(CRect &Rect);

    private:
        CRect m_UpperRect;
        CRect m_LowerRect;
    };


    // CWhiteKeyMiddle class
    class CWhiteKeyMiddle : public CPianoKey
    {
    public:
        CWhiteKeyMiddle(double UnitLength, int Width, double Position);

        bool IsPointInKey(const CPoint &pt) const;

        void NoteOn(COLORREF NoteOnColor, CRect &Rect);
        void NoteOff(CRect &Rect);

        void Paint(CDC *dc);

    private:
        void CalcInvalidRect(CRect &Rect);

    private:
        CRect m_UpperRect;
        CRect m_LowerRect;
    };


    // CWhiteKeyRight class
    class CWhiteKeyRight : public CPianoKey
    {
    public:
        CWhiteKeyRight(double UnitLength, int Width, double Position);

        bool IsPointInKey(const CPoint &pt) const;

        void NoteOn(COLORREF NoteOnColor, CRect &Rect);
        void NoteOff(CRect &Rect);

        void Paint(CDC *dc);

    private:
        void CalcInvalidRect(CRect &Rect);

    private:
        CRect m_UpperRect;
        CRect m_LowerRect;
    };


    // CWhiteKeyFull class
    class CWhiteKeyFull : public CPianoKey
    {
    public:
        CWhiteKeyFull(double UnitLength, int Width, double Position);

        bool IsPointInKey(const CPoint &pt) const;

        void NoteOn(COLORREF NoteOnColor, CRect &Rect);
        void NoteOff(CRect &Rect);

        void Paint(CDC *dc);

    private:
        CRect m_Rect;
    };


    // CBlackKey class
    class CBlackKey : public CPianoKey
    {
    public:
        CBlackKey(double UnitLength, int Width, double Position);

        bool IsPointInKey(const CPoint &pt) const;

        void NoteOn(COLORREF NoteOnColor, CRect &Rect);
        void NoteOff(CRect &Rect);

        void Paint(CDC *dc);

    private:
        CRect m_Rect;
    };


// Public attributes
public:
    // The class name of this control
    static const TCHAR CLASS_NAME[];

    // Default color used for indicating that a note is being played
    static const COLORREF DEF_NOTE_ON_COLOR;

    // The maximum number of notes this piano control can have
    static const int MAX_NOTE_COUNT;

    // Note identifiers
    enum NoteId { C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, 
                  A_SHARP, B };

    // Table of note identifiers
    static const int NOTE_TABLE[];

// Private attributes
protected:
    // For protecting access to the list of listeners
    CRITICAL_SECTION m_CriticalSection;

    // Color indicating a key is being played
    COLORREF m_NoteOnColor;

    // Lenght of this control
    int m_Length;

    // Width of this control
    int m_Width;            
    
    // Low note of the range
    unsigned char m_LowNote;

    // High note of the range
    unsigned char m_HighNote;      

    // Unit length for each key
    double m_UnitLength;

    // Flag for indicating whether or not this object has been 
    // intialized.
    bool m_IsInitialized;

    // Flag for indicating whether or not this window has captured the 
    // mouse. 
    bool m_HasCapture;    

    // Collection of piano key objects
    std::vector<CPianoKey *> m_Keys;

    // The current active key (triggered by the mouse)
    int m_CurrKey;

    // Collection of piano control listeners
    std::list<CPianoCtrlListener *> m_Listeners; 

    // Flag for whether or note this custom control's Window class has 
    // been registered.
    static bool REGISTER_FLAG;

    // Messages for range errors
    static const CString NOTE_RANGE_ERR;
    static const CString LOW_NOTE_RANGE_ERR;
    static const CString HIGH_NOTE_RANGE_ERR;
    static const CString INVALID_RANGE_ERR;
    static const CString NATURAL_NOTE_ERR;


//----------------------------------------------------------------------
// Wizard generated code 
//----------------------------------------------------------------------


public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPianoCtrl)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CPianoCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIANOCTRL_H__C84F71CE_FF29_11D6_865D_0030BD08B6D9__INCLUDED_)
