/*
 * @(#)util/Song.h 3.00 30 August 2000
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

#ifndef TSE3_UTIL_SONG_H
#define TSE3_UTIL_SONG_H

#include <vector>

namespace TSE3
{
    class Song;
    class Part;
    class Phrase;

    namespace Util
    {
        /**
         * Search for the given @ref TSE3::Phrase in all of the @ref TSE3::Part
         * objects in this @ref TSE3::Song. A pointer to each @ref Part is
         * placed into the vector @p list.
         *
         * This is a useful way to discover which @ref TSE3::Part objects
         * @ref Song_ReplacePhrase is going to alter.
         *
         * @param song   @ref TSE3::Song to work with
         * @param phrase @ref TSE3::Phrase to search for
         * @param list   Vector to place @ref Parts into
         * @see   Song_ReplacePhrase
         */
        void Song_SearchForPhrase(TSE3::Song               *song,
                                  TSE3::Phrase             *phrase,
                                  std::vector<TSE3::Part*> &list);

        /**
         * Replace the given @ref TSE3::Phrase with a new @ref TSE3::Phrase
         * in the @ref TSE3::Song.
         *
         * Often the new @ref TSE3::Phrase will have the same 'unique'
         * reference name as the old one, and the old @ref Phrase will be
         * deleted directly after this call, after which the new
         * @ref TSE3::Phrase will be placed in the @ref TSE3::PhraseList.
         *
         * @param song   @ref TSE3::Song to work with
         * @param oldphr Old @ref TSE3::Phrase that will be replaced.
         * @param newphr New @ref TSE3::Phrase to replace the old one.
         * @see   Song_SearchForPhrase
         */
        void Song_ReplacePhrase(TSE3::Song   *song,
                                TSE3::Phrase *oldphr,
                                TSE3::Phrase *newphr);
    }
}

#endif
