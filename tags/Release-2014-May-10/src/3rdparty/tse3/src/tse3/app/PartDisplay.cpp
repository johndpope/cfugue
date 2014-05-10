/*
 * @(#)PartDisplay.cpp 3.00 25 May 1999
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

#include "tse3/app/PartDisplay.h"

#include "tse3/app/Application.h"
#include "tse3/DisplayParams.h"
#include "tse3/Part.h"
#include "tse3/Phrase.h"

using namespace TSE3;
using namespace TSE3::App;

PartDisplay::PartDisplay(Part *p, PresetColours *pc)
: p(p), preset(pc), _calculated(false)
{
}


void PartDisplay::calculate()
{
    _useColour = false;
    _r         = 255;
    _g         = 255;
    _b         = 255;

    Phrase        *phr  = p->phrase();
    DisplayParams *d    = p->displayParams();
    DisplayParams *phrd = phr ? phr->displayParams() : 0;

    if (d->style() != DisplayParams::None)
    {
        if (d->style() == DisplayParams::Default && phr)
        {
            // Get values from Phrase
            if (phrd->style() == DisplayParams::Colour)
            {
                phrd->colour(_r, _g, _b);
                _useColour = true;
            }
            else if (phrd->style() == DisplayParams::PresetColour && preset)
            {
                preset->colour(phrd->presetColour(), _r, _g, _b);
                _useColour = true;
            }
        }
        else if (d->style() != DisplayParams::Default)
        {
            // Values from Part: must be using colour
            _useColour = true;
            if (d->style() == DisplayParams::Colour)
            {
                d->colour(_r, _g, _b);
                _useColour = true;
            }
            else if (preset)
            {
                preset->colour(d->presetColour(), _r, _g, _b);
                _useColour = true;
            }
        }
    }

    _calculated = true;
}


bool PartDisplay::useColour()
{
    if (!_calculated)
    {
        calculate();
    }

    return _useColour;
}


void PartDisplay::colour(int &r, int &g, int &b)
{
    if (!_calculated)
    {
        calculate();
    }
    r = _r;
    g = _g;
    b = _b;
}
