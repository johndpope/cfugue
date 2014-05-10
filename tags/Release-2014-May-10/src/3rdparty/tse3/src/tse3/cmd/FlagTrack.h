/*
 * @(#)cmd/FlagTrack.h 3.00 26 April 2002
 *
 * Copyright (c) 2002 Pete Goodliffe (pete@cthree.org)
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

#ifndef TSE3_CMD_FLAG_H
#define TSE3_CMD_FLAG_H

#include "tse3/cmd/Command.h"
#include "tse3/FlagTrack.h"

#include <string>
#include <cstddef>

namespace TSE3
{
    namespace Cmd
    {
        /**
         * Command to add a @ref TSE3::Flag to a @ref TSE3::FlagTrack.
         *
         * @short   Add @ref TSE3::Flag Command
         * @author  Pete Goodliffe
         * @version 1.00
         * @see     Command
         */
        class FlagTrack_Add : public Command
        {
            public:

                FlagTrack_Add(TSE3::FlagTrack               *flagTrack,
                              const TSE3::Event<TSE3::Flag> &flag);

            protected:

                /**
                 * @reimplemented
                 */
                virtual void executeImpl();

                /**
                 * @reimplemented
                 */
                virtual void undoImpl();

            private:

                TSE3::FlagTrack               *flagTrack;
                const TSE3::Event<TSE3::Flag>  flag;
                size_t                         insertIndex;
        };
    }
}

#endif
