/*
 * @(#)cmd/CommandGroup.h 3.00 5 Septemeber 2000
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

#ifndef TSE3_CMD_COMMANDGROUP_H
#define TSE3_CMD_COMMANDGROUP_H

#include "tse3/cmd/Command.h"

#include <vector>
#include <cstddef>

namespace TSE3
{
    namespace Cmd
    {
        /**
         * A class that groups @ref Command object together so you can
         * pretend that several commands are one.
         *
         * The commands will be executed in the order that they are added
         * to the CommandGroup (see @ref add). They will be undone in the
         * reverse order.
         *
         * @short   Group of commands @ref Command
         * @author  Pete Goodliffe
         * @version 3.00
         */
        class CommandGroup : public Command
        {
            public:

                /**
                 * Creates an initially empty CommandGroup. Add @ref Command
                 * objects with @ref add.
                 *
                 * You may specify a name for the group, or leave it blank.
                 * If you leave it blank then the first @ref Command added
                 * will give its name to the CommandGroup.
                 *
                 * @param title Command name for CommandGroup
                 */
                CommandGroup(const std::string &title = "");

                virtual ~CommandGroup();

                /**
                 * Adds a @ref Command to the group. You can keep doing this
                 * up to the first time you call @ref Command::execute.
                 *
                 * After the CommandGroup has been executed, this function
                 * will do nothing.
                 *
                 * When you insert the @ref Command you bind it's lifetime
                 * to the lifetime of this @ref CommandGroup.
                 *
                 * You may have already executed this command prior to
                 * inserting it, in which case it will not be run on the
                 * first CommandGroup execute. You may need to be careful
                 * when doing this to ensure you know that Commands won't
                 * interfere with one another.
                 *
                 * @param command New @ref Command to add to the CommandGroup
                 */
                void add(Command *command);

                /**
                 * Returns the number of @ref Command objects that have
                 * been added to this CommandGroup (see @ref add).
                 *
                 * @return Number of @ref Command objects in this CommandGroup
                 */
                size_t size() const { return cmds.size(); }

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

                CommandGroup(const CommandGroup &);
                const CommandGroup &operator=(const CommandGroup &);

                std::vector<Command *> cmds;
                bool                   canAdd;
        };
    }
}

#endif
