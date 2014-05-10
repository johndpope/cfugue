/*
 * @(#)Phrase.cpp 3.00 2 April 2000
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

#include "tse3/util/Phrase.h"
#include "tse3/Phrase.h"
#include "tse3/PhraseEdit.h"
#include "tse3/Song.h"
#include "tse3/PhraseList.h"
#include "tse3/Error.h"

#include <memory>

using namespace TSE3;
using namespace TSE3::Util;

void Util::Phrase_Merge(std::vector<TSE3::Playable*> &play,
                        TSE3::PhraseEdit *phraseEdit)
{
    std::vector<Playable*>::iterator i = play.begin();
    while (i != play.end())
    {
        std::auto_ptr<PlayableIterator> pi((*i)->iterator(0));
        while (pi->more())
        {
            phraseEdit->insert(**pi);
            ++(*pi);
        }
        ++i;
    }
    phraseEdit->tidy();
}


void Util::Phrase_Add(TSE3::Phrase *p1, TSE3::Phrase *p2, TSE3::PhraseEdit *pe)
{
    pe->reset(p1);
    for (size_t n = 0; n < p2->size(); ++n)
    {
        pe->insert((*p2)[n]);
    }
}


void Util::Phrase_Subtract(TSE3::Phrase *p1, TSE3::Phrase *p2,
                           TSE3::PhraseEdit *pe)
{
    pe->reset(p1);
    for (size_t n = 0; n < p2->size(); ++n)
    {
        MidiEvent e = (*p2)[n];
        size_t m = pe->index(e.time);
        while (m < pe->size() && (*pe)[m].time == e.time && (*pe)[m] != e)
        {
            ++m;
        }
        if (m < pe->size() && (*pe)[m] == e)
        {
            pe->erase(m);
        }
    }
}


void Util::Phrase_Explode(TSE3::Phrase *phrase, std::string /*baseName*/,
                          int channels, bool insertParts, TSE3::Song *song)
{
/*    if (insertParts)
    {
        while (song->size() < 16)
        {
            song->insertTrack();
        }
    }*/

    for (int channel = 0; channel < 16; ++channel)
    {
        if (channels & (1<<channel))
        {
            PhraseEdit pe;
            Clock stopTime = 0;
            for (size_t n = 0; n < phrase->size(); ++n)
            {
                MidiEvent e = (*phrase)[n];
                if (e.data.status >= MidiCommand_NoteOn
                    && e.data.status <= MidiCommand_PitchBend
                    && e.data.channel == channel)
                {
                    pe.insert(e);
                    if (e.time > stopTime) stopTime = e.time;
                }
                if (!pe.size()) continue;

                Phrase *newPhrase
                    = pe.createPhrase(song->phraseList(), phrase->title());
                // or explodedPhraseString ???

                if (insertParts)
                {
                    //insertPart channel 0 stopTime trackNo, partNo newphrase
                    newPhrase = newPhrase;
                    std::cerr << "TSE3: TODO insertParts in Phrase_Explode\n";
                }
            }
        }
    }
}

