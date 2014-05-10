/*
 * @(#)util/NoteNumber.h 3.00 2 April 2000
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

#ifndef TSE3_UTIL_NOTENUMBER_H
#define TSE3_UTIL_NOTENUMBER_H

#include <string>

namespace TSE3
{
    /**
     * The Util namespace contains classes and functions that build upon
     * the @ref TSE3 class API to provide more sophisitacted functionality.
     *
     * These utilities are contained in the separate Util namespace to prevent
     * the basic @ref TSE3 API becoming large and complex to understand.
     *
     * @short   @ref TSE3 library utilities
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     TSE3
     */
    namespace Util
    {
        /**
         * Converts a MIDI note number to a name string.
         *
         * Note name 'C5' is middle C (note number 60).
         *
         * @param  note MIDI note number
         * @return Note name string
         * @see    noteToNumber
         */
        std::string numberToNote(int note);

        /**
         * Converts a MIDI name string to a note number
         *
         * Note name 'C5' is middle C (note number 60).
         *
         * @param  src Source name string
         * @return Integer MIDI note value
         * @see    numberToNote
         */
        int noteToNumber(const std::string &src);
    }
}

#endif
