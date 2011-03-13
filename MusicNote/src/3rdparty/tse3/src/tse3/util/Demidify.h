/*
 * @(#)util/Demidify.h 3.00 3 April 2000
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

#ifndef TSE3_UTIL_DEMIDIFY_H
#define TSE3_UTIL_DEMIDIFY_H

#include "tse3/Midi.h"

#include <iostream>
#include <exception>
#include <cstddef>

namespace TSE3
{
    class Song;
    class Phrase;
    class Progress;

    namespace Util
    {
        /**
         * Exception type thrown by @ref Demidify.
         */
        class DemidifyException : public std::exception
        {
            public:
        };

        /**
         * A utility that de 'MIDI' fies a @ref Song. If you use the @ref
         * MidiFileImport class to load a MIDI file, you will see that the
         * MTrks are mapped into each @ref Track and any repeated data that
         * could exploit the TSE3 @ref Phrase facility.
         *
         * This class analyses each @ref Track and reconstructs the @ref Song
         * in a more structured manner.
         *
         * @short   Reconstruct Song structure utility
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     MidiFileImport
         */
        class Demidify
        {
            public:

                /**
                 * Creates a Demidify utility class.
                 *
                 * @param compactParts
                 * @param pullTrackParameters
                 * @param partSize
                 * @param aggressive
                 * @param verbose       Diagnostic level:
                 *                      0 - silent, 1 - errors
                 *                      2 - informational, 3 - far too much!
                 * @param out           ostream to produce diagnostics on
                 */
                Demidify(bool            compactParts,
                         bool            pullTrackParameters,
                         TSE3::Clock     partSize,
                         bool            aggressive,
                         TSE3::Progress *progress     = 0,
                         int             verbose      = 0,
                         std::ostream   &out          = std::cout);

                /**
                 * Perform the demidify.
                 *
                 * @param song The song to demidify.
                 */
                void go(TSE3::Song *song);

            private:

                /**
                 * Finds repeated patterns in the @ref Phrase and breaks the
                 * @ref TSE3::Parts in the @ref TSE3::Track up if it finds any.
                 *
                 * @param song           Song
                 * @param trackNo        Track Phrase is in
                 * @param prog_base      Current progress value at beginning of
                 *                       disection
                 * @param max_prog_delta Number of progress values this
                 *                       disection can increment by
                 */
                void disectPhrase(TSE3::Song *song, size_t trackNo,
                                  int prog_base, int max_prog_delta);

                /**
                 * Reduces repeated @ref Parts with same @ref Phrase to one @ref
                 * Part using the repeat parameter.
                 */
                void reduceParts(TSE3::Song *song, size_t trackNo);

                /**
                 * Do a simple pattern match on @ref Part at startPos.
                 *
                 * Returns the number of Parts removed.
                 */
                int matchParts(TSE3::Song *song, size_t trackNo,
                               size_t startPos);

                bool            compactParts;
                bool            pullTrackParameters;
                TSE3::Clock     partSize;
                bool            aggressive;
                TSE3::Progress *progress;
                int             verbose;
                std::ostream   &out;

                TSE3::Song     *song;
        };
    }
}

#endif
