/*
 * @(#)PowerQuantise.cpp 3.00 2 April 2000
 *
 * Copyright (c) 2000 Pete Goodliffe (pete@cthree.org)
 *
 * This file is part of TSE3 - the Trax Sequencer Engine version 3.00.
 *
 * This library is modifiable/redistributable under the terms of the GNU
 * General Public License.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; see the file COPYING. If not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include "tse3/util/PowerQuantise.h"
#include "tse3/PhraseEdit.h"

#include <algorithm>
#include <cstdlib>

using namespace TSE3;
using namespace TSE3::Util;

/******************************************************************************
 * PowerQuantise::Pattern class
 *****************************************************************************/

PowerQuantise::Pattern::Pattern()
: _length(Clock::PPQN*4)
{
    points.push_back(0);
    points.push_back(Clock::PPQN);
    points.push_back(Clock::PPQN*2);
    points.push_back(Clock::PPQN*3);
}


void PowerQuantise::Pattern::setLength(Clock length)
{
    if (length > 0)
    {
        _length = length;
        // notify?
    }
}


void PowerQuantise::Pattern::insert(Clock point)
{
    points.push_back(point);
    sort(points.begin(), points.end());
    // notify?
}


void PowerQuantise::Pattern::erase(size_t n)
{
    points.erase(points.begin() + n);
    // notify?
}


void PowerQuantise::Pattern::erase(Clock point)
{
    std::vector<Clock>::iterator i = find(points.begin(), points.end(), point);
    if (i != points.end())
    {
        points.erase(i);
        // notify?
    }
}


/******************************************************************************
 * PowerQuantise class
 *****************************************************************************/

PowerQuantise::PowerQuantise()
: _pattern(), _by(100), _window(100),
  _onlySelected(false), _onlyNotes(false),
  _spreadCtrl(true), _direction(nearest),
  _length(quantiseOffTime), _lengthBy(100),
  _humanise(0), _lengthHumanise(0)
{
}


void PowerQuantise::setBy(int by)
{
    _by = by;
}


void PowerQuantise::setWindow(int window)
{
    _window = window;
}


void PowerQuantise::setOnlySelected(bool s)
{
    _onlySelected = s;
}


void PowerQuantise::setOnlyNotes(bool n)
{
    _onlyNotes = n;
}


void PowerQuantise::setSpreadCtrl(bool s)
{
    _spreadCtrl = s;
}


void PowerQuantise::setDirection(Direction d)
{
    _direction = d;
}


void PowerQuantise::setLength(Length l)
{
    _length = l;
}


void PowerQuantise::setLengthBy(int by)
{
    _lengthBy = by;
}


void PowerQuantise::setHumanise(Clock h)
{
    _humanise = h;
}


void PowerQuantise::setLengthHumanise(Clock h)
{
    _lengthHumanise = h;
}


void PowerQuantise::go(PhraseEdit *phraseEdit)
{
    Clock lastClock             = 0; // time of last event in PhraseEdit
    Clock lastNonCont_Original  = 0; // time of last non continuous event
    Clock lastNonCont_Quantised = 0; // quantised time of last non cont ev

    for (size_t pos = 0; pos < phraseEdit->size(); ++pos)
    {
        MidiEvent e = (*phraseEdit)[pos];
        if (e.time > lastClock) lastClock = e.time;
        if (!isContinuous(e))
        {
            lastNonCont_Quantised = lastNonCont_Original = e.time;
        }
        if (shouldBeQuantised(e))
        {
            MidiEvent qe = e; // qe: quantised event
            qe.time = quantise(e.time, _by);
            qe.time = humanise(qe.time, _humanise);
            if (!isContinuous(qe))   lastNonCont_Quantised = qe.time;
            if (qe.time > lastClock) lastClock = qe.time;
            if (qe.data.status == MidiCommand_NoteOn)
            {
                // Need to quantise note off too
                if (qe.offTime > lastClock) lastClock = qe.offTime;
                if (_length == quantiseLength)
                {
                    qe.offTime = qe.time
                               + quantise(qe.offTime - e.time, _lengthBy);
                }
                else
                {
                    qe.offTime = quantise(qe.offTime, _lengthBy);
                }
                qe.offTime = humanise(qe.offTime, _lengthHumanise);
                if (qe.offTime > lastClock) lastClock = qe.offTime;
            }
            (*phraseEdit)[pos] = qe;
        }
        else if (shouldBeSpread(e))
        {
            Clock c = spreadContinuous(phraseEdit, pos,
                                       lastNonCont_Original,
                                       lastNonCont_Quantised);
            e.time = c;
            (*phraseEdit)[pos] = e;
        }
    }

    phraseEdit->tidy(lastClock);
}


Clock PowerQuantise::quantise(Clock time, int percentage)
{
    if (!_window) return time;

    // Split the time up into the number of whole patterns and the position
    // in the pattern
    int   noPatternLengths = time / _pattern.length();
    Clock pulses           = time % _pattern.length();

    // Work out the previous and next snap point indexes for this time
    int prevPoint = -1;
    int nextPoint = -1;
    for (size_t n = 0; n < _pattern.size(); ++n)
    {
        if (_pattern[n] <= pulses && prevPoint == -1) prevPoint = n;
        if (_pattern[n] >= pulses && nextPoint == -1) nextPoint = n;
    }
    if (prevPoint == -1)
    {
        prevPoint = _pattern.size()-1;
        ++noPatternLengths;
    }
    if (nextPoint == -1)
    {
        nextPoint = 0;
        ++noPatternLengths;
    }

    // Work out the time of the snap points
    Clock point         = noPatternLengths * _pattern.length();
    Clock prevPointTime = point + _pattern[prevPoint];
    Clock nextPointTime = point + _pattern[nextPoint];

    // Work out the window size (time between the nearest two snap points)
    Clock windowSize;
    if (prevPoint < static_cast<int>(_pattern.size()-1))
        windowSize = _pattern[prevPoint+1] - _pattern[prevPoint];
    else
        windowSize = _pattern.length() + _pattern[0] - _pattern[prevPoint];
    windowSize *= _window;
    windowSize /= 200;

    // Work out which point to aim for and put it in 'point'
    if (_direction == backwards)
        point = prevPointTime;
    else if (_direction == forwards)
        point = nextPointTime;
    else
        point = (time-prevPointTime < nextPointTime-time)
              ? prevPointTime
              : nextPointTime;

    // Do a window check: if event falls outside the window, don't quantise it
    if (point == prevPointTime)
    {
        if (time-prevPointTime > windowSize) return time;
    }
    else
    {
        if (nextPointTime-time > windowSize) return time;
    }

    // Move towards this snap point by the percentage
    Clock delta = time-point;
    return time - delta*percentage/100;
}


Clock PowerQuantise::humanise(Clock time, Clock maxVal)
{
    if (maxVal != 0)
    {
        time += (rand()/(RAND_MAX/(maxVal*2))) - maxVal;
    }
    return time;
}


Clock PowerQuantise::spreadContinuous(PhraseEdit *phraseEdit, size_t pos,
                                      Clock lastNonCont_Original,
                                      Clock lastNonCont_Quantised)
{
    MidiEvent e = (*phraseEdit)[pos];

    // Find the time of the next non-continuous event
    Clock nextNonCont_Original = e.time;
    MidiEvent e2 = e;
    size_t pos2 = pos;
    do
    {
        ++pos2;
        if (pos2 <= phraseEdit->size())
        {
            e2 = (*phraseEdit)[pos2];
            nextNonCont_Original = e2.time;
        }
    }
    while (isContinuous(e2) && pos2 < phraseEdit->size());

    // Now work out when the next note on time will be quantised
    Clock nextNonCont_Quantised = quantise(nextNonCont_Original, _by);

    Clock newTime = (nextNonCont_Quantised-lastNonCont_Quantised)
                  * (e.time - lastNonCont_Original)
                  / (nextNonCont_Original - lastNonCont_Original)
                  + lastNonCont_Quantised;
    return newTime;
}

