/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2011 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/
/*                                                                        */
/************ Modified from original code by Gopalakrishna Palem  *********/
/*                                                                        */
/***************************************************************************
 *   Copyright (C) 2008 - Giuseppe Cigala                                  *
 *   g_cigala@virgilio.it                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "qvumeter.h"

const int nBorderWidth      = 10;
const int nInterBarGap      = 10;

const int nTopLableStartX   = nBorderWidth + nInterBarGap/2;
const int nTopLabelStartY   = 15;
const int nTopLabelWidth    = 15;
const int nTopLabelHeight   = 25;

const int nBarStartX    = nTopLableStartX;
const int nBarStartY    = nTopLabelStartY + nTopLabelHeight + 10;
const int nBarWidth     = nTopLabelWidth;   // we need the text and the bar to start on same X with same width
const int nBarHeight    = 450;

const int nDownLabelStartX  = nBarStartX;
const int nDownLabelStartY  = nBarStartY + nBarHeight + 5; 
const int nDownLabelWidth   = nBarWidth;
const int nDownLabelHeight  = nTopLabelHeight;

const int nTotalWindowHeight = nDownLabelStartY + nDownLabelHeight + nTopLabelStartY;
const int nUnitBarWidth      = nInterBarGap + nTopLabelWidth;


QVUMeter::QVUMeter(QStringList strBarLables, QWidget *parent) : QWidget(parent)
{
    colBack = QColor(60, 60, 60);

    m_nBars = strBarLables.length();

    for(int i=0 ; i < m_nBars; ++i)
        m_Bars.push_back(new QVUBar(this, strBarLables[i]));

    m_nTotalWindowWidth = nUnitBarWidth * m_nBars + 2 * nBorderWidth;
}


QVUMeter::QVUMeter(QWidget *parent) : QWidget(parent)
{
    colBack = QColor(60, 60, 60);

    m_nBars = 1;

    for(int i=0 ; i < m_nBars; ++i)
        m_Bars.push_back(new QVUBar(this));

    m_nTotalWindowWidth = nUnitBarWidth * m_nBars + 2 * nBorderWidth;
}


void QVUMeter::SetBars(QStringList strBarLables, double min, double max)
{
    m_Bars.clear();

    m_nBars = strBarLables.length();

    for(int i=0 ; i < m_nBars; ++i)
        m_Bars.push_back(new QVUBar(this, strBarLables[i], min, max));

    m_nTotalWindowWidth = nUnitBarWidth * m_nBars + 2 * nBorderWidth;

    update();
}

void QVUMeter::SetBarCount(int nCount, double min, double max)
{
    while(m_Bars.size() > nCount) // remove extra bars if required
        m_Bars.removeLast();

    for(int i=0, nMax = m_Bars.size(); i < nMax; ++i) // adjust the min, max values for existing bars
        m_Bars.at(i)->setMinMaxValues(min, max);

    while(m_Bars.size() < nCount)   // add additional bars if required
        m_Bars.push_back(new QVUBar(this, "", min, max));

    m_nTotalWindowWidth = nUnitBarWidth * m_nBars + 2 * nBorderWidth;

    update();
}

void QVUMeter::paintEvent(QPaintEvent *)
{
    paintBorder();
    paintBar();
    paintValue();

}

void QVUMeter::paintBorder()
{
    int nTotalWidth = m_nTotalWindowWidth;
    int nTotalHeight = nTotalWindowHeight;

    QPainter painter(this);
    painter.setWindow(0, 0, nTotalWidth, nTotalHeight);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor light = Qt::white;
    QColor dark = colBack.darker(140);

    painter.setPen(QPen(colBack, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QLinearGradient linGrad(5, nTotalHeight/2, 10, nTotalHeight/2);
    linGrad.setColorAt(0, light);
    linGrad.setColorAt(1, colBack);
    linGrad.setSpread(QGradient::PadSpread);
    painter.setBrush(linGrad);
    QRectF border(5, 5, nTotalWidth - 10, nTotalHeight - 10);
    painter.drawRoundRect(border, 30, 5);
    QLinearGradient linGrad1(nTotalWidth - nBorderWidth, nTotalHeight/2, nTotalWidth - 5, nTotalHeight/2);
    linGrad1.setColorAt(0, colBack);
    linGrad1.setColorAt(1, dark);
    linGrad1.setSpread(QGradient::PadSpread);
    painter.setBrush(linGrad1);
    QRectF border1(nBorderWidth, 5, nTotalWidth - nBorderWidth - 5, nTotalHeight - 10);
    painter.drawRoundRect(border1, 30, 5);

    // paint Down label
    QFont valFont("Arial", 9, QFont::Bold);//valFont("Arial", 12, QFont::Bold);
    painter.setFont(valFont);

    for(int i=0; i < m_nBars; ++i)
    {
        painter.setPen(QPen(m_Bars[i]->colText, 2));

        int nStartX = nDownLabelStartX + i * nUnitBarWidth;
        int nStartY = nDownLabelStartY;        
        QRectF Left(nStartX, nStartY, nDownLabelWidth, nDownLabelHeight);//(20, 505, 25, 20);
        painter.drawText(Left, Qt::AlignCenter, m_Bars[i]->lable());
    }
}

void QVUMeter::paintBar()
{
    int nTotalWidth = m_nTotalWindowWidth;
    int nTotalHeight = nTotalWindowHeight;

    QPainter painter(this);
    painter.setWindow(0, 0, nTotalWidth, nTotalHeight);
    painter.setRenderHint(QPainter::Antialiasing);


    // draw color bars

    for(int i=0; i < m_nBars; ++i)
    {
        const QVUBar& bar = *m_Bars[i];

        int nStartX = nBarStartX + i * nUnitBarWidth;
        int nStartY = nBarStartY;

        QLinearGradient linGrad(nStartX, 0, nStartX, 500);//(50, 0, 50, 500)
        linGrad.setColorAt(0, bar.colHigh);
        linGrad.setColorAt(1, bar.colLow);
        linGrad.setSpread(QGradient::PadSpread);
        painter.setBrush(linGrad);

        painter.drawRect(nStartX, nStartY, nBarWidth, nBarHeight);

        // draw background bar
        painter.setBrush(QColor(40, 40, 40));
    
        double length = nBarHeight;
        double leftBar = abs(length * (bar.Value - bar.min)/(bar.max-bar.min));
        
        painter.drawRect(nStartX, nStartY, nBarWidth, nBarHeight-leftBar);

        painter.setPen(QPen(Qt::black, 2));

        for (int i = 0; i <=60; i++)
        {
            painter.drawLine(nStartX + 1, 500-nBarHeight*i/60, nStartX + nBarWidth - 1, 500-nBarHeight*i/60);//(21, 500-450*i/60, 44, 500-450*i/60);
        }
    }

}

void QVUMeter::paintValue()
{
    int nTotalWidth = m_nTotalWindowWidth;
    int nTotalHeight = nTotalWindowHeight;

    QPainter painter(this);
    painter.setWindow(0, 0, nTotalWidth, nTotalHeight);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont valFont("Arial", 9, QFont::Bold);
    painter.setFont(valFont);
         
    for(int i=0; i < m_nBars; ++i)
    {
        const QVUBar& bar = *m_Bars[i];

        int nStartX = nTopLableStartX + i * nUnitBarWidth;
        int nStartY = nTopLabelStartY;

        painter.setPen(QPen(Qt::black, 2));
        painter.setBrush(Qt::black);
        painter.drawRect(nStartX, nStartY, nTopLabelWidth, nTopLabelHeight);//(20, 15, 25, 25);
       
        painter.setPen(Qt::gray);
        painter.drawLine(nStartX, nStartY + nTopLabelHeight, nStartX + nTopLabelWidth, nStartY + nTopLabelHeight);//(20, 40, 45, 40);

        painter.setPen(QPen(bar.colText, 1));

        QRectF leftR(nStartX, nStartY, nTopLabelWidth, nTopLabelHeight);//(20, 15, 25, 25);
        QString lVal = QString("%1").arg(bar.Value, 0,'f', 0);
        painter.drawText(leftR, Qt::AlignCenter, lVal);

        //emit valueChanged(Value);
    }

}

QVUBar::QVUBar(QObject* pParent, QString strLable, double _min, double _max) : QObject(pParent)
{    
    colText = Qt::white;
    colHigh = Qt::red;
    colLow = Qt::green;
    dimVal = 9;
    min = _min;
    max = _max;
    Value = 0;    
    _lable = strLable;
}

void QVUBar::setValueDim(int dim)
{
    dimVal = dim;
    ((QWidget *)parent())->update();
}

void QVUMeter::setColorBg(QColor color)
{
    colBack = color;
    ((QWidget *)parent())->update();
}

void QVUBar::setColorOfText(QColor color)
{
    colText = color;
    ((QWidget *)parent())->update();
}

void QVUBar::setColorHigh(QColor color)
{
    colHigh = color;
    ((QWidget *)parent())->update();
}


void QVUBar::setColorLow(QColor color)
{
    colLow = color;
    ((QWidget *)parent())->update();
}

void QVUBar::setValue(double leftValue)
{
    if (leftValue > max)
    {
        Value = max;
    }
    else if (leftValue < min)
    {
        Value = min;
    }
    else
    {
        Value = leftValue;
    }
    ((QWidget *)parent())->update();
}


void QVUBar::setMinValue(double minValue)
{
    if (minValue > max)
    {
        min = max;
        max = minValue;
    }
    else
    {
        min = minValue;
    }
    ((QWidget *)parent())->update();
}

void QVUBar::setMaxValue(double maxValue)
{
    if (maxValue < min)
    {
        max = min;
        min = maxValue;
    }
    else
    {
        max = maxValue;
    }
    ((QWidget *)parent())->update();
}

void QVUBar::setMinMaxValues(double minValue, double maxValue)
{
    if(minValue > maxValue)
    {
        min = maxValue;
        max = minValue;
    }
    else
    {
        min = minValue;
        max = maxValue;
    }
    ((QWidget *)parent())->update();
}

void QVUBar::setLable(QString strLable)
{
    _lable = strLable;
    ((QWidget *)parent())->update();
}

QSize QVUMeter::minimumSizeHint() const
{
    return QSize(m_nTotalWindowWidth / 5, nTotalWindowHeight / 10);
}

QSize QVUMeter::sizeHint() const
{
    return QSize(m_nTotalWindowWidth, nTotalWindowHeight);
}


QVUBar* QVUMeter::GetBar(int nIndex) const
{
    if(nIndex >= 0 && nIndex < m_Bars.size())
        return m_Bars.at(nIndex);

    return NULL;
}

void QVUMeter::SetMinMaxValues(double nMin, double nMax)
{
    for(int i=0, nMax = m_Bars.size(); i < nMax; ++i)
       m_Bars.at(i)->setMinMaxValues(nMin, nMax);
}

int QVUMeter::GetBarCount() const
{
    return m_Bars.size();
}