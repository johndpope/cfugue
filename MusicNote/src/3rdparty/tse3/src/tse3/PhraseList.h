/*
 * @(#)PhraseList.h 3.00 14 May 1999
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

#ifndef TSE3_PHRASELIST_H
#define TSE3_PHRASELIST_H

#include "tse3/listen/PhraseList.h"

#include "tse3/Notifier.h"
#include "tse3/Serializable.h"
#include "tse3/listen/Phrase.h"

#include <string>
#include <vector>
#include <cstddef>

namespace TSE3
{
    /**
     * A list of @ref Phrases within a certain @ref Song.
     *
     * @sect Command classes
     *
     * Use the following command classes to manipute this object in a undo/redo
     * environment.
     *
     *     @li @ref TSE3::Cmd::Phrase_Create
     *     @li @ref TSE3::Cmd::Phrase_Erase
     *
     * @short   A list of Phrases
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Phrase
     * @see     Song
     */
    class PhraseList : public Notifier<PhraseListListener>,
                       public Listener<PhraseListener>,
                       public Serializable
    {
        public:

            /**
             * The base name to use when creating a new @ref Phrase.
             *
             * @see newPhraseTitle
             */
            static const std::string newPhraseString;

            /**
             * The base name to use when creating a new @ref Phrase that has
             * been imported.
             *
             * @see newPhraseTitle
             */
            static const std::string importedString;

            /**
             * The base name to use when creating a new @ref Phrase that has
             * been merged into the @ref PhraseList.
             *
             * @see newPhraseTitle
             */
            static const std::string mergedPhraseString;

            /**
             * The base name to use when creating a new @ref Phrase that has
             * been 'exploded' by a Phrase Utility.
             *
             * @see newPhraseTitle
             */
            static const std::string explodedPhraseString;

            /**
             * Creates an empty PhraseList
             */
            PhraseList();

            /**
             * The destructor: all @ref Phrases contained in the PhraseList will
             * also be deleted.
             */
            virtual ~PhraseList();

            /**
             * Returns the number of @ref Phrases in the PhraseList.
             *
             * @return Number of @ref Phrases
             */
            size_t size() const { return list.size(); }

            /**
             * Returns the @ref Phrase at positon n
             *
             * The value returned for an index that is out of range is
             * undefined. The @ref size method describes the valid
             * values.
             *
             * @param  n Index
             * @return @ref Phrase at index n
             */
            Phrase *operator[](size_t n) const { return list[n]; }

            /**
             * Returns the @ref Phrase with the given title.
             *
             * @param  title Unique @ref Phrase reference title
             * @return Phrase with given name
             */
            Phrase *phrase(const std::string &title) const;

            /**
             * Returns the index of the given @ref Phrase. If the @ref Phrase
             * is no in the PhraseList, returns the value of @ref size().
             *
             * @param phrase @ref Phrase to search for
             * @return index of @ref Phraes in PhraseList, or @ref size()
             */
            size_t index(const Phrase *phrase) const;

            /**
             * Deletes the @ref Phrase (consequently removing it from the
             * PhraseList). If the @ref Phrase is not in the PhraseList no
             * error is raised.
             *
             * It is also safe to just delete a @ref Phrase through a pointer
             * to it. However, that's a little 'unconventional'.
             *
             * @param phrase The @ref Phrase to delete
             */
            void erase(Phrase *phrase);

            /**
             * Removes the @ref Phrase from the PhraseList but does not
             * delete it.
             *
             * Note that this leaves you with a pretty useless @ref Phrase. You
             * cannot use it in a @ref Part. Any @ref Part objects that were
             * using the @ref Phrase will reset their @ref Phrase reference.
             *
             * The only way to make the @ref Phrase useful again is to insert
             * it back into a PhraseList.
             *
             * Once you have removed a @ref Phrase from the PhraseList it is
             * no longer owned by the PhraseList, and it is your responsibility
             * to delete it.
             *
             * If the specified @ref Phrase is not in the @ref PhraseList then
             * no error is raised.
             *
             * @param phrase The @ref Phrase to remove
             * @see insert
             * @internal
             */
            void remove(Phrase *phrase);

            /**
             * Inserts an unparented @ref Phrase back into the PhraseList.
             *
             * Throws @ref PhraseNameExists if there is already a @ref Phrase
             * with that name in the PhraseList.
             *
             * You cannot insert a Phrase with no title - the same exception
             * will be thrown.
             *
             * This causes the Phrase to be 'owned' by the PhraseList, it will
             * be deleted when the PhraseList is deleted.
             *
             * @param  phrase The @ref Phrase to insert
             * @throws PhraseListError
             */
            void insert(Phrase *phrase);

            /**
             * Returns a @ref Phrase name based on the given base string that is
             * guaranteed to not be used by any @ref Phrase in this PhraseList.
             *
             * If the base name already exists in the PhraseList, a unique
             * variant of it will be returned (by appending a number).
             *
             * @param  baseName The base name string
             * @return A new unused Phrase name string
             * @see    newPhraseString
             * @see    importedString
             * @see    mergedPhraseString
             * @see    explodedPhraseString
             */
            std::string newPhraseTitle
                (const std::string &baseName = newPhraseString);

            /**
             * @reimplemented
             */
            virtual void Phrase_TitleAltered(Phrase *);

            /**
             * @reimplemented
             */
            virtual void Notifier_Deleted(Phrase *);

            /**
             * @reimplemented
             */
            virtual void save(std::ostream &o, int i) const;

            /**
             * @reimplemented
             *
             * NOTE: The PhraseList actually loads the @ref Phrase object
             * chunks.
             */
            virtual void load(std::istream &i, SerializableLoadInfo &info);

            friend class Phrase;

        private:

            PhraseList &operator=(const PhraseList &);
            PhraseList(const PhraseList &);

            /**
             * Private internal part of insert, that adds the Phrase to the
             * @p list vector, but doesn't send any notifications.
             */
            void insertInList(Phrase *phrase);

            /**
             * This is the only private method that the @ref Phrase
             * friend class calls.
             *
             * It informs the PhraseList that a @ref Phrase has changed its
             * name.
             *
             * @param p @ref Phrase that has changed its name
             */
            void phraseTitleChanged(Phrase *p);

            /**
             * Used by load to load a Phrase from the Events block and
             * to insert it into the PhraseList
             */
            void loadPhraseEvents(std::istream &i, int filePPQN,
                                  std::string &title);

            std::vector<Phrase*> list;
    };
}

#endif
