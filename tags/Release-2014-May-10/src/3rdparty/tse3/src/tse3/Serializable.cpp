/*
 * @(#)Serializable.cpp 3.00 28 May 1999
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

#include "tse3/Serializable.h"

#include "tse3/Midi.h"
#include "tse3/FileBlockParser.h"

using namespace TSE3;

/******************************************************************************
 * SerializableLoadInfo class
 *****************************************************************************/

SerializableLoadInfo::SerializableLoadInfo()
: PPQN(Clock::PPQN), song(0),
  //major(-1), minor(-1),
  unknownChunks(false), unknownData(false),
  noChunks(0), progress(0)
{
    // This is because of a bug on gcc 3.0.x (tested up to 3.0.4).
    // Go on, try it - uncomment that line above. Sigh.
    major = -1;
    minor = -1;
}


/******************************************************************************
 * Serializable class
 *****************************************************************************/

void Serializable::save(std::ostream &out, int indentLevel) const
{
    out << indent(indentLevel) << "{\n"
        << indent(indentLevel) << "}\n";
}


void Serializable::load(std::istream &in, SerializableLoadInfo &info)
{
    FileBlockParser parser;
    parser.parse(in, info);
}

