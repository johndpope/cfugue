/*
 * @(#)NoteNumber.cpp 3.00 2 April 2000
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

#include "tse3/util/NoteNumber.h"

#include <sstream>

using namespace TSE3;

std::string Util::numberToNote(int note)
{
    std::string dest;

    if (note >= 0 && note <= 127)
    {

        switch (note%12)
        {
            case 0:  dest.append("C");  break;
            case 1:  dest.append("C#"); break;
            case 2:  dest.append("D");  break;
            case 3:  dest.append("D#"); break;
            case 4:  dest.append("E");  break;
            case 5:  dest.append("F");  break;
            case 6:  dest.append("F#"); break;
            case 7:  dest.append("G");  break;
            case 8:  dest.append("G#"); break;
            case 9:  dest.append("A");  break;
            case 10: dest.append("A#"); break;
            case 11: dest.append("B");  break;
        }

        dest.append("-");

        {
            std::ostringstream o;
            o << note/12;
            dest.append(o.str());
        }
    }

    return dest;
}


int Util::noteToNumber(const std::string &src)
{
    int note = 0;

    switch (src[0])
    {
        case 'C': case'c': note = 0;  break;
        case 'D': case'd': note = 2;  break;
        case 'E': case'e': note = 4;  break;
        case 'F': case'f': note = 5;  break;
        case 'G': case'g': note = 7;  break;
        case 'A': case'a': note = 9;  break;
        case 'B': case'b': note = 11; break;
    }
    switch (src[1])
    {
        case '#': ++note; break;
        case 'b': --note; break;
    }

    int n = 1;
    while (src[n] == '#' || src[n] == 'b' || src[n] == '-') ++n;

    int octave = 0;
    std::istringstream i(src.c_str()+n);
    i >> octave;

    note += octave*12;

    if (note < 0)   note = 0;
    if (note > 127) note = 127;

    return note;
}

