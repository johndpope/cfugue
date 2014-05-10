/*
 * @(#)Error.cpp 3.00 27 June 2000
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

#include "tse3/Error.h"

using namespace TSE3;

namespace
{
    /**************************************************************************
     * A table of error strings.
     * Take care that this is kept up to date with the ErrorCode enum type.
     *************************************************************************/
    const char *errstrs[] =
    {
        // Song error codes
        "The track has already been inserted in a song",

        // Track error codes
        "The part has already been inserted in a track",
        "This would cause parts to overlap in the track",
        "No part was inserted in the track",

        // Track and Part error codes
        "Part timing error",
        "Part not inserted in a PhraseList",

        // PhraseList error codes
        "The phrase name specified already exists in the phrase list",
        "This phrase has already been inserted in a phrase list",
        "The phrase name specified was invalid",

        // MidiFile error codes
        "MidiFile import was unsucessful",
        "MidiFile export was unsucessful",

        // Serializable error codes (via TSE3MDL, TSE2MDL)
        "Couldn't open file",
        "Invalid file type",
        "File format broken",

        // MidiScheduler error codes
        "Failed to create the MIDI scheduler",

         // Other
        "Unknown TSE3 error"
    };
}


const char *Error::what() const throw()
{
    return errString(_reason);
}


const char *TSE3::errString(ErrorCode reason)
{
    if (reason > 0 && reason < NoErrorCodes)
    {
        return errstrs[reason];
    }
    else
    {
        return "";
    }
}
