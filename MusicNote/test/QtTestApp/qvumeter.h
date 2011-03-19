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

#ifndef QVUMETER_H
#define QVUMETER_H

#include <QtGui>
#include <QtDesigner/QDesignerExportWidget>
#include <math.h>
#include <QList>
#include <QStringList>

class QUVBar : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor colorOfText READ colorOfText WRITE setColorOfText);
    Q_PROPERTY(QColor colorLow READ colorLow WRITE setColorLow);
    Q_PROPERTY(QColor colorHigh READ colorHigh WRITE setColorHigh);
    Q_PROPERTY(double Value READ value WRITE setValue);
    Q_PROPERTY(double minValue READ minValue WRITE setMinValue);
    Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue);
    Q_PROPERTY(int dimValue READ valueDim WRITE setValueDim);
    Q_PROPERTY(QString lable READ lable WRITE setLable);

    QColor colorOfText() const
    {
        return colText;
    }
    QColor colorLow() const
    {
        return colLow;
    }
    QColor colorHigh() const
    {
        return colHigh;
    }
    double value() const
    {
        return Value;
    }
    double minValue() const
    {
        return min;
    }
    double maxValue() const
    {
        return max;
    }
    int valueDim() const
    {
        return dimVal;
    }
    QString lable() const
    {
        return _lable;
    }


public:

    QUVBar(QString strLabel = "", QObject* pParent = NULL);


signals:

    void valueChanged(double);

public slots:

    void setColorOfText(QColor);
    void setColorHigh(QColor);
    void setColorLow(QColor);
    void setValueDim(int);
    void setValue(double);
    void setMinValue(double);
    void setMaxValue(double);
    void setLable(QString);


protected:

    void paintEvent(QPaintEvent *);
    void paintBorder();
    void paintBar();
    void paintValue();

    friend class QVUMeter;

private:

    double min;
    double max;
    double Value;
    int dimVal;
    QColor colText;
    QColor colHigh;
    QColor colLow;
    QString _lable;
};

class /*QDESIGNER_WIDGET_EXPORT*/ QVUMeter : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor colorBg READ colorBg WRITE setColorBg);

    QColor colorBg() const
    {
        return colBack;
    }

public:

    /*  <Summary>
        @param nBars number of individual bars that should be drawn
        @param parent the parent widget
        </Summary>
    */
    QVUMeter(QStringList strBarLables, QWidget *parent = 0) ;
    QSize minimumSizeHint() const;
    QSize sizeHint() const;


signals:

    void valueLChanged(double);
    void valueRChanged(double);

public slots:

    void setColorBg(QColor);
    QUVBar* GetBar(int nIndex) const;
    //void setColorValue(QColor);
    //void setColorHigh(QColor);
    //void setColorLow(QColor);
    //void setValueDim(int);
    //void setLeftValue(double);
    //void setRightValue(double);
    //void setMinValue(double);
    //void setMaxValue(double);


protected:

    void paintEvent(QPaintEvent *);
    void paintBorder();
    void paintBar();
    void paintValue();



private:

    QColor colBack;

    int m_nBars;    // Number of vertical bars to show
    int m_nTotalWindowWidth;

    QList<QUVBar*> m_Bars;

};

#endif
