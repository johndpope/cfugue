/*
 * @(#)util/Phrase.h 3.00 2 April 2000
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

#ifndef TSE3_UTIL_PHRASE_H
#define TSE3_UTIL_PHRASE_H

#include <string>
#include <vector>

namespace TSE3
{
    class Phrase;
    class PhraseEdit;
    class Song;
    class Track;
    class Playable;
    class Part;

    namespace Util
    {
        /**
         * Merge together the contents of the specified @ref TSE3::Playable
         * objects and put the result in the given @ref PhraseEdit object.
         *
         * You can use this, for example, to add together two @ref TSE3::Phrase
         * objects, or to merge together several @ref TSE3::Tracks.
         *
         * The resultant @ref PhraseEdit output is tidy.
         *
         * @param  play A vector containing each @ref TSE3::Playable to merge
         * @param  pe @ref TSE3::PhraseEdit for result
         */
        void Phrase_Merge(std::vector<TSE3::Playable*> &play,
                          TSE3::PhraseEdit *pe);

        /**
         * Add two @ref TSE3::Phrases together and put the result in the given
         * @ref TSE3::PhraseEdit object.
         *
         * @param p1 First @ref TSE3::Phrase
         * @param p2 Second @ref TSE3::Phrase
         * @param pe @ref TSE3::PhraseEdit for result of @ref Phrase addition
         */
        void Phrase_Add(TSE3::Phrase *p1, TSE3::Phrase *p2,
                        TSE3::PhraseEdit *pe);

        /**
         * Subtract two @ref TSE3::Phrases and put the result in the given
         * @ref TSE3::PhraseEdit object.
         *
         * @param p1 First @ref TSE3::Phrase
         * @param p2 Second @ref TSE3::Phrase
         * @param pe @ref TSE3::PhraseEdit for result of
         *           @ref TSE3::Phrase subtraction: result is p1 - p2.
         */
        void Phrase_Subtract(TSE3::Phrase *p1, TSE3::Phrase *p2,
                             TSE3::PhraseEdit *pe);

        /**
         * Arpeggiate @p p1 with the note pattern in @p p2.
         *
         * @param p1 First @ref TSE3::Phrase
         * @param p2 Second @ref TSE3::Phrase
         * @param pe @ref TSE3::PhraseEdit containg p1 arpeggiated with p2.
         */
        void Phrase_Arpeggiate(TSE3::Phrase *p1, TSE3::Phrase *p2,
                               TSE3::PhraseEdit *pe);

        /**
         * Explode the given @ref TSE3::Phrase into one @ref TSE3::Phrase per
         * MIDI channel.
         *
         * The new @ref TSE3::Phrase objects are named <baseName>_1 .. _16.
         *
         * @param p        @ref TSE3::Phrase to explode
         * @param baseName Base string the new @ref TSE3::Phrase objects' names
         *                 are based on
         * @param channels Bitfield for which channels to explode
         * @param song     If 0, ignored; otherwise new @ref TSE3::Part objects
         *                 are placed into the Song's Tracks using each new
         *                 @ref TSE3::Phrase
         */
        void Phrase_Explode(TSE3::Phrase *p, std::string baseName,
                            int channels, bool insertParts,
                            TSE3::Song *song);
    }
}

#endif
