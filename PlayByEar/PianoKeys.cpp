/*

  PianoKeys.cpp

  This class represents the implementation of the CPianoKey derived 
  classes.

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

    Last modified: 01/23/2008

*/


//---------------------------------------------------------------------
// Dependencies
//---------------------------------------------------------------------


#include "stdafx.h"


//---------------------------------------------------------------------
// Constants
//---------------------------------------------------------------------


// The width of the black keys as a percentage of the width of the 
// white keys
const double CPianoCtrl::CPianoKey::BLACK_KEY_OFFSET = 0.7;

// The number of units natural keys use in their widest part.
const int CPianoCtrl::CPianoKey::UNIT_PER_NAT_KEY = 3;


//---------------------------------------------------------------------
// CWhiteKeyLeft class implementation
//---------------------------------------------------------------------


// Constructor
CPianoCtrl::CWhiteKeyLeft::CWhiteKeyLeft(double UnitLength, int Width, 
                                         double Position)
{
    // Initialize upper portion of this key
    m_UpperRect.top    = 0;
    m_UpperRect.bottom = static_cast<long>(Width * BLACK_KEY_OFFSET);
    m_UpperRect.left   = static_cast<long>(Position);
    m_UpperRect.right  = static_cast<long>
        (Position + UnitLength * (UNIT_PER_NAT_KEY - 1));

    // Initialize lower portion of this key
    m_LowerRect.top    = static_cast<long>(Width * BLACK_KEY_OFFSET);
    m_LowerRect.bottom = Width - 1;
    m_LowerRect.left   = static_cast<long>(Position);
    m_LowerRect.right  = static_cast<long>
        (Position + UnitLength * UNIT_PER_NAT_KEY);
}


// Hit detection
bool CPianoCtrl::CWhiteKeyLeft::IsPointInKey(const CPoint &pt) const
{
    return (IsPointInRect(pt, m_UpperRect) || 
            IsPointInRect(pt, m_LowerRect));
}


// Turns note on
void CPianoCtrl::CWhiteKeyLeft::NoteOn(COLORREF NoteOnColor, 
                                       CRect &Rect)
{
    m_NoteOnFlag = true;
    m_NoteOnColor = NoteOnColor;

    CalcInvalidRect(Rect);
}


// Turns note off
void CPianoCtrl::CWhiteKeyLeft::NoteOff(CRect &Rect)
{
    m_NoteOnFlag = false;

    CalcInvalidRect(Rect);
}


// Paints the key
void CPianoCtrl::CWhiteKeyLeft::Paint(CDC *dc)
{
    //
    // Fill key
    //

    // Note is on
    if(m_NoteOnFlag)
    {
        dc->FillSolidRect(&m_UpperRect, m_NoteOnColor);
        dc->FillSolidRect(&m_LowerRect, m_NoteOnColor);
    }
    // Note is off
    else
    {
        dc->FillSolidRect(&m_UpperRect, RGB(250, 250, 250));
        dc->FillSolidRect(&m_LowerRect, RGB(250, 250, 250));
    }

    dc->SelectStockObject(BLACK_PEN);

    // 
    // Draw border
    //

    // Left side
    dc->MoveTo(m_UpperRect.left, m_UpperRect.top);
    dc->LineTo(m_UpperRect.left, m_LowerRect.bottom);

    // Bottom
    dc->LineTo(m_LowerRect.right, m_LowerRect.bottom);

    // Right side
    dc->LineTo(m_LowerRect.right, m_LowerRect.top);
    dc->LineTo(m_UpperRect.right, m_UpperRect.bottom);
    dc->LineTo(m_UpperRect.right, m_UpperRect.top);

    // Top
    dc->LineTo(m_UpperRect.left, m_UpperRect.top);
}


// Calculate the rectangular area to invalidate for this key
void CPianoCtrl::CWhiteKeyLeft::CalcInvalidRect(CRect &Rect)
{
    Rect.top    = m_UpperRect.top;
    Rect.bottom = m_LowerRect.bottom;
    Rect.left   = m_UpperRect.left;
    Rect.right  = m_LowerRect.right;
}


//---------------------------------------------------------------------
// CWhiteKeyMiddle class implementation
//---------------------------------------------------------------------


// Constructor
CPianoCtrl::CWhiteKeyMiddle::CWhiteKeyMiddle(double UnitLength, 
                                             int Width, double Position)
{
    // Initialize upper portion of this key
    m_UpperRect.top    = 0;
    m_UpperRect.bottom = static_cast<long>(Width * BLACK_KEY_OFFSET);
    m_UpperRect.left   = static_cast<long>(Position + UnitLength);
    m_UpperRect.right  = static_cast<long>
        (Position + UnitLength * (UNIT_PER_NAT_KEY - 1));

    // Initialize lower portion of this key
    m_LowerRect.top    = static_cast<long>(Width * BLACK_KEY_OFFSET);
    m_LowerRect.bottom = Width - 1;
    m_LowerRect.left   = static_cast<long>(Position);
    m_LowerRect.right  = static_cast<long>
        (Position + UnitLength * UNIT_PER_NAT_KEY);
}


// Hit detection
bool CPianoCtrl::CWhiteKeyMiddle::IsPointInKey(const CPoint &pt) const
{
    return (IsPointInRect(pt, m_UpperRect) || 
            IsPointInRect(pt, m_LowerRect));
}


// Turns note on
void CPianoCtrl::CWhiteKeyMiddle::NoteOn(COLORREF NoteOnColor, 
                                         CRect &Rect)
{
    m_NoteOnFlag = true;
    m_NoteOnColor = NoteOnColor;

    CalcInvalidRect(Rect);
}


// Turns note off
void CPianoCtrl::CWhiteKeyMiddle::NoteOff(CRect &Rect)
{
    m_NoteOnFlag = false;

    CalcInvalidRect(Rect);
}


// Paints key
void CPianoCtrl::CWhiteKeyMiddle::Paint(CDC *dc)
{
    //
    // Fill key
    //

    // Note is on
    if(m_NoteOnFlag)
    {
        dc->FillSolidRect(&m_UpperRect, m_NoteOnColor);
        dc->FillSolidRect(&m_LowerRect, m_NoteOnColor);
    }
    // Note is off
    else
    {
        dc->FillSolidRect(&m_UpperRect, RGB(250, 250, 250));
        dc->FillSolidRect(&m_LowerRect, RGB(250, 250, 250));
    }

    dc->SelectStockObject(BLACK_PEN);

    // 
    // Draw border
    //

    // Left side
    dc->MoveTo(m_UpperRect.left, m_UpperRect.top);
    dc->LineTo(m_UpperRect.left, m_UpperRect.bottom);
    dc->LineTo(m_LowerRect.left, m_LowerRect.top);
    dc->LineTo(m_LowerRect.left, m_LowerRect.bottom);

    // Bottom
    dc->LineTo(m_LowerRect.right, m_LowerRect.bottom);

    // Right side
    dc->LineTo(m_LowerRect.right, m_LowerRect.top);
    dc->LineTo(m_UpperRect.right, m_UpperRect.bottom);
    dc->LineTo(m_UpperRect.right, m_UpperRect.top);

    // Top
    dc->LineTo(m_UpperRect.left, m_UpperRect.top);
}


// Calculate rectangular area to invalidate for this key
void CPianoCtrl::CWhiteKeyMiddle::CalcInvalidRect(CRect &Rect)
{
    Rect.top    = m_UpperRect.top;
    Rect.bottom = m_LowerRect.bottom;
    Rect.left   = m_LowerRect.left;
    Rect.right  = m_LowerRect.right;
}


//---------------------------------------------------------------------
// CWhiteKeyRight class implementation
//---------------------------------------------------------------------


// Constructor
CPianoCtrl::CWhiteKeyRight::CWhiteKeyRight(double UnitLength, int Width, 
                                           double Position)
{
    // Initialize upper portion of this key
    m_UpperRect.top    = 0;
    m_UpperRect.bottom = static_cast<long>(Width * BLACK_KEY_OFFSET);
    m_UpperRect.left   = static_cast<long>(Position + UnitLength);
    m_UpperRect.right  = static_cast<long>
        (Position + UnitLength * UNIT_PER_NAT_KEY);

    // Initialize lower portion of this key
    m_LowerRect.top    = static_cast<long>(Width * BLACK_KEY_OFFSET);
    m_LowerRect.bottom = Width - 1;
    m_LowerRect.left   = static_cast<long>(Position);
    m_LowerRect.right  = static_cast<long>
        (Position + UnitLength * UNIT_PER_NAT_KEY);
}


// Hit detection
bool CPianoCtrl::CWhiteKeyRight::IsPointInKey(const CPoint &pt) const
{
    return (IsPointInRect(pt, m_UpperRect) || 
            IsPointInRect(pt, m_LowerRect));
}


// Turns note on
void CPianoCtrl::CWhiteKeyRight::NoteOn(COLORREF NoteOnColor, 
                                        CRect &Rect)
{
    m_NoteOnFlag = true;
    m_NoteOnColor = NoteOnColor;

    CalcInvalidRect(Rect);
}


// Turns note off
void CPianoCtrl::CWhiteKeyRight::NoteOff(CRect &Rect)
{
    m_NoteOnFlag = false;

    CalcInvalidRect(Rect);
}


// Paints key
void CPianoCtrl::CWhiteKeyRight::Paint(CDC *dc)
{
    //
    // Fill key
    //

    // Note is on
    if(m_NoteOnFlag)
    {
        dc->FillSolidRect(&m_UpperRect, m_NoteOnColor);
        dc->FillSolidRect(&m_LowerRect, m_NoteOnColor);
    }
    // Note is off
    else
    {
        dc->FillSolidRect(&m_UpperRect, RGB(250, 250, 250));
        dc->FillSolidRect(&m_LowerRect, RGB(250, 250, 250));
    }

    dc->SelectStockObject(BLACK_PEN);

    // 
    // Draw border
    //

    // Left side
    dc->MoveTo(m_UpperRect.left, m_UpperRect.top);
    dc->LineTo(m_UpperRect.left, m_UpperRect.bottom);
    dc->LineTo(m_LowerRect.left, m_LowerRect.top);
    dc->LineTo(m_LowerRect.left, m_LowerRect.bottom);

    // Bottom
    dc->LineTo(m_LowerRect.right, m_LowerRect.bottom);

    // Right side
    dc->LineTo(m_LowerRect.right, m_UpperRect.top);

    // Top
    dc->LineTo(m_UpperRect.left, m_UpperRect.top);
}


// Calculates rectangular area to invalidate for this key
void CPianoCtrl::CWhiteKeyRight::CalcInvalidRect(CRect &Rect)
{
    Rect.top    = m_UpperRect.top;
    Rect.bottom = m_LowerRect.bottom;
    Rect.left   = m_LowerRect.left;
    Rect.right  = m_LowerRect.right;
}


//---------------------------------------------------------------------
// CWhiteKeyFull class implementation
//---------------------------------------------------------------------


// Constructor
CPianoCtrl::CWhiteKeyFull::CWhiteKeyFull(double UnitLength, int Width, 
                                         double Position)
{
    // Initialize this key's rectangular area
    m_Rect.top    = 0;
    m_Rect.bottom = Width - 1;
    m_Rect.left   = static_cast<long>(Position);
    m_Rect.right  = 
        static_cast<long>(Position + UnitLength * UNIT_PER_NAT_KEY);
}


// Hit detection
bool CPianoCtrl::CWhiteKeyFull::IsPointInKey(const CPoint &pt) const
{
    return (IsPointInRect(pt, m_Rect));
}


// Turns note on
void CPianoCtrl::CWhiteKeyFull::NoteOn(COLORREF NoteOnColor, CRect &Rect)
{
    m_NoteOnFlag = true;
    m_NoteOnColor = NoteOnColor;

    Rect = m_Rect;
}


// Turns note off
void CPianoCtrl::CWhiteKeyFull::NoteOff(CRect &Rect)
{
    m_NoteOnFlag = false;

    Rect = m_Rect;
}


// Paints this key
void CPianoCtrl::CWhiteKeyFull::Paint(CDC *dc)
{
    //
    // Fill key
    //

    // Note is on
    if(m_NoteOnFlag)
    {
        dc->FillSolidRect(&m_Rect, m_NoteOnColor);
    }
    // Note is off
    else
    {
        dc->FillSolidRect(&m_Rect, RGB(250, 250, 250));
    }

    dc->SelectStockObject(BLACK_PEN);

    // 
    // Draw border
    //

    // Left side
    dc->MoveTo(m_Rect.left, m_Rect.top);
    dc->LineTo(m_Rect.left, m_Rect.bottom);

    // Bottom
    dc->LineTo(m_Rect.right, m_Rect.bottom);

    // Right side
    dc->LineTo(m_Rect.right, m_Rect.top);

    // Top
    dc->LineTo(m_Rect.left, m_Rect.top);
}


//---------------------------------------------------------------------
// CBlackKey class implementation
//---------------------------------------------------------------------


// Constructor
CPianoCtrl::CBlackKey::CBlackKey(double UnitLength, int Width, 
                                 double Position)
{
    // Initialize this key's rectangular area
    m_Rect.top    = 0;
    m_Rect.bottom = static_cast<long>(Width * BLACK_KEY_OFFSET);
    m_Rect.left   = static_cast<long>(Position);
    m_Rect.right  = static_cast<long>
        (Position + UnitLength * (UNIT_PER_NAT_KEY - 1));
}


// Hit detection
bool CPianoCtrl::CBlackKey::IsPointInKey(const CPoint &pt) const
{
    return (IsPointInRect(pt, m_Rect));
}


// Turns note on
void CPianoCtrl::CBlackKey::NoteOn(COLORREF NoteOnColor, CRect &Rect)
{
    m_NoteOnFlag = true;
    m_NoteOnColor = NoteOnColor;

    Rect = m_Rect;
}


// Turns note off
void CPianoCtrl::CBlackKey::NoteOff(CRect &Rect)
{
    m_NoteOnFlag = false;

    Rect = m_Rect;
}


// Paints this key
void CPianoCtrl::CBlackKey::Paint(CDC *dc)
{
    //
    // Fill key
    //

    // Note is on
    if(m_NoteOnFlag)
    {
        dc->FillSolidRect(&m_Rect, m_NoteOnColor);
    }
    // Note is off
    else
    {
        dc->FillSolidRect(&m_Rect, RGB(0, 0, 0));
    }

    dc->SelectStockObject(BLACK_PEN);

    // 
    // Draw border
    //

    // Left side
    dc->MoveTo(m_Rect.left, m_Rect.top);
    dc->LineTo(m_Rect.left, m_Rect.bottom);

    // Bottom
    dc->LineTo(m_Rect.right, m_Rect.bottom);

    // Right side
    dc->LineTo(m_Rect.right, m_Rect.top);

    // Top
    dc->LineTo(m_Rect.left, m_Rect.top);
}