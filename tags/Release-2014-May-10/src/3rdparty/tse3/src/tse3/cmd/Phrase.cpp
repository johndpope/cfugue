/*
 * @(#)Phrase.cpp 3.00 21 November 2000
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

#include "tse3/cmd/Phrase.h"

#include "tse3/Song.h"
#include "tse3/Part.h"
#include "tse3/Phrase.h"
#include "tse3/PhraseEdit.h"
#include "tse3/PhraseList.h"
#include "tse3/Error.h"
#include "tse3/util/Song.h"

using namespace TSE3;
using namespace TSE3::Cmd;

/******************************************************************************
 * Phrase_SetInfo class
 *****************************************************************************/

Phrase_SetInfo::Phrase_SetInfo(TSE3::Phrase              *phrase,
                               const std::string         &title,
                               const TSE3::DisplayParams &dp)
: Command("phrase info"), phrase(phrase), newTitle(title), dp(dp)
{
    if (!phrase->parent())
    {
        throw TSE3::PhraseListError(TSE3::PhraseUnparentedErr);
    }
    if (title.size() && phrase->parent()->phrase(title))
    {
        if (phrase->parent()->phrase(title) != phrase)
        {
            throw TSE3::PhraseListError(TSE3::PhraseNameExistsErr);
        }
        else
        {
            newTitle = "";
        }
    }
}


void Phrase_SetInfo::executeImpl()
{
    oldTitle = phrase->title();

    if (newTitle.size()) phrase->setTitle(newTitle);
    std::swap(dp, *(phrase->displayParams()));
}


void Phrase_SetInfo::undoImpl()
{
    if (newTitle.size()) phrase->setTitle(oldTitle);
    std::swap(dp, *(phrase->displayParams()));
}


/******************************************************************************
 * Phrase_Create class
 *****************************************************************************/

Phrase_Create::Phrase_Create(TSE3::PhraseList  *pl,
                             TSE3::PhraseEdit  *pe,
                             const std::string &t)
: Command("create phrase"), phraseList(pl), phraseEdit(pe), newPhrase(0),
  title(t)
{
    if (phraseList->phrase(title))
    {
        throw TSE3::PhraseListError(TSE3::PhraseNameExistsErr);
    }
}


Phrase_Create::~Phrase_Create()
{
    if (!done())
    {
        delete newPhrase;
    }
}


void Phrase_Create::executeImpl()
{
    if (!newPhrase)
    {
        newPhrase = phraseEdit->createPhrase(phraseList, title);
    }
    else
    {
        phraseList->insert(newPhrase);
    }
}


void Phrase_Create::undoImpl()
{
    phraseList->remove(newPhrase);
}


/******************************************************************************
 * Phrase_Erase class
 *****************************************************************************/

Phrase_Erase::Phrase_Erase(TSE3::Phrase *phrase,
                           TSE3::Song   *song)
: Command("erase phrase"), phrase(phrase), song(song), vector_done(false)
{
    if (!song || phrase->parent() != song->phraseList())
    {
        // phrase == 0 means that the operation is invalid, either
        //   1) the parents don't match, or
        //   2) the Phrase has no parent
        phrase = 0;
    }
}


Phrase_Erase::~Phrase_Erase()
{
    if (phrase && done())
    {
        delete phrase;
    }
}


void Phrase_Erase::executeImpl()
{
    if (!phrase) return;

    if (song)
    {
        TSE3::PhraseList *phraseList = song->phraseList();
        if (phraseList->index(phrase) != phraseList->size())
        {
            if (!vector_done)
            {
                Util::Song_SearchForPhrase(song, phrase, parts);
                vector_done = true;
            }
            phraseList->remove(phrase);
        }
        else
        {
            phrase = 0;
        }
    }
    else
    {
        phrase->parent()->remove(phrase);
    }

}


void Phrase_Erase::undoImpl()
{
    if (phrase)
    {
        song->phraseList()->insert(phrase);
        std::vector<TSE3::Part*>::iterator i = parts.begin();
        while (i != parts.end())
        {
            (*i)->setPhrase(phrase);
            i++;
        }
    }
}


/******************************************************************************
 * Phrase_Replace class
 *****************************************************************************/


Phrase_Replace::Phrase_Replace(TSE3::Phrase *newPhrase,
                               TSE3::Phrase *oldPhrase,
                               TSE3::Song   *song)
: Command("replace phrase"),
  newPhrase(newPhrase), oldPhrase(oldPhrase), phraseEdit(0), song(song)
{
    Util::Song_SearchForPhrase(song, oldPhrase, parts);
}


Phrase_Replace::Phrase_Replace(TSE3::Phrase      *oldPhrase,
                               TSE3::PhraseEdit  *pe,
                               TSE3::Song        *song,
                               const std::string &title)
: Command("replace phrase"),
  newPhrase(0), oldPhrase(oldPhrase), phraseEdit(pe), song(song),
  newTitle(title)
{
    Util::Song_SearchForPhrase(song, oldPhrase, parts);
}


Phrase_Replace::~Phrase_Replace()
{
    if (done())
    {
        delete oldPhrase;
    }
    else
    {
        delete newPhrase;
    }
}


void Phrase_Replace::executeImpl()
{
    TSE3::PhraseList *phraseList = song->phraseList();

    if (newPhrase && !phraseEdit)
    {
        // Just a simple swap between two Phrases in the PhraseList
    }
    else if (newPhrase)
    {
        // Redo of a created Phrase from next case...
        phraseList->remove(oldPhrase);
        phraseList->insert(newPhrase);
    }
    else
    {
        // Create a new phrase
        phraseList->remove(oldPhrase);
        if (newTitle.size() == 0)
        {
            newPhrase
                = phraseEdit->createPhrase(phraseList, oldPhrase->title());
        }
        else
        {
            newPhrase = phraseEdit->createPhrase(phraseList, newTitle);
        }
        // We won't actually dereference PhraseEdit again, (it might now
        // get deleted) but we keep the pointer to remember what kind of
        // operation this is.
    }

    std::vector<Part*>::iterator i = parts.begin();
    while (i != parts.end())
    {
        (*i)->setPhrase(newPhrase);
        ++i;
    }
}


void Phrase_Replace::undoImpl()
{
    TSE3::PhraseList *phraseList = song->phraseList();

    if (phraseEdit)
    {
        phraseList->remove(newPhrase);
        phraseList->insert(oldPhrase);
    }

    std::vector<Part*>::iterator i = parts.begin();
    while (i != parts.end())
    {
        (*i)->setPhrase(oldPhrase);
        ++i;
    }
}

