/*
 * @(#)listen/PhraseEdit.h 3.00 19 May 1999
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

#ifndef TSE3_LISTEN_PHRASEEDIT_H
#define TSE3_LISTEN_PHRASEEDIT_H

#include "tse3/listen/MidiData.h"

#include <cstddef>

namespace TSE3
{
    class PhraseEdit;

    /**
     * @ref PhraseEdit listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class PhraseEditListener : public MidiDataListener
    {
        public:
            typedef PhraseEdit notifier_type;

            virtual void PhraseEdit_Reset(PhraseEdit *)                      {}
            virtual void PhraseEdit_Tidied(PhraseEdit *)                     {}
            virtual void PhraseEdit_Inserted(PhraseEdit *, size_t /*index*/) {}
            virtual void PhraseEdit_Erased(PhraseEdit *, size_t /*index*/)   {}
            virtual void PhraseEdit_Selection(PhraseEdit *,
                                              size_t /*index*/,
                                              bool /*selected*/)             {}
            virtual void PhraseEdit_Modified(PhraseEdit *, bool /*mod*/)     {}
    };
}

#endif
