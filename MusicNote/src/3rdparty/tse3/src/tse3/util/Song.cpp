/*
 * @(#)Song.cpp 3.00 30 August 2000
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

#include "tse3/util/Song.h"

#include "tse3/Song.h"
#include "tse3/Track.h"
#include "tse3/Part.h"
#include "tse3/Phrase.h"

using namespace TSE3;

void Util::Song_SearchForPhrase(Song *song, Phrase *phr, 
                                std::vector<Part*> &list)
{
    for (size_t n = 0; n < song->size(); ++n)
    {
        for (size_t p = 0; p < (*song)[n]->size(); ++p)
        {
            if ((*(*song)[n])[p]->phrase() == phr)
            {
                list.push_back((*(*song)[n])[p]);
            }
        }
    }
}


void Util::Song_ReplacePhrase(Song *song, Phrase *oldphr, Phrase *newphr)
{
    for (size_t n = 0; n < song->size(); ++n)
    {
        for (size_t p = 0; p < (*song)[n]->size(); ++p)
        {
            if ((*(*song)[n])[p]->phrase() == oldphr)
            {
                (*(*song)[n])[p]->setPhrase(newphr);
            }
        }
    }
}

