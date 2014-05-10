/*
 * @(#)cmd/CommandHistory.h 3.00 10 June 1999
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

#ifndef TSE3_CMD_COMMANDHISTORY_H
#define TSE3_CMD_COMMANDHISTORY_H

#include "tse3/listen/cmd/CommandHistory.h"

#include "tse3/Notifier.h"
#include "tse3/cmd/Command.h"

#include <list>
#include <cstddef>

namespace TSE3
{
    namespace Cmd
    {
        /**
         * The history of which @ref Command objects have been executed. Once a
         * @ref Command has been executed, it should be placed into the
         * CommandHistory so that an undo/redo mechanism can be operated by the
         * user.
         *
         * In the TSE3 system there could be be one CommandHistroy object per
         * @ref TSE3::Song or one global CommandHistory.
         *
         * When the availablity of either undos or redos changes, a notification
         * is sent.
         *
         * The CommandHistory class has a size limit facility. When either the
         * undo or redo list goes over a certain threshold (see @ref limit)
         * @ref Commands will be deleted. This can be set to 'infinite'.
         *
         * @short   History of which @ref Command objects have been executed
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class CommandHistory : public TSE3::Notifier<CommandHistoryListener>
        {
            public:

                /**
                 * Create a CommandHistory with the given limit size.
                 * This limit defaults to the value of @ref defaultLimit.
                 *
                 * @param limit CommandHistory size limit
                 */
                CommandHistory(int limit = defaultLimit);

                /**
                 * Adds a @ref Command to the CommandHistory. The @ref Command
                 * must have been executed. Placing it in the CommandHistory
                 * ties it's lifetime to that of the CommandHistory object.
                 *
                 * Adding a @ref Command will wipe any redos that may have been
                 * possible.
                 *
                 * @param command New @ref Command to add
                 */
                void add(Command *command);

                /**
                 * Returns whether there are any @ref Command objects that can
                 * be undone.
                 *
                 * When this information changes a notification will be sent.
                 *
                 * @return True if there are any undos
                 */
                bool undos() const;

                /**
                 * Undoes the last @ref Command, and places it on the redo list.
                 */
                void undo();

                /**
                 * Returns the command @p places down in the undo history.
                 * For example, undoCommand(0) returns the next @ref Command
                 * to be undone.
                 *
                 * If there is no command to be undone at this @p pos then
                 * returns zero.
                 *
                 * @param  pos  Index into undo history
                 * @return @ref Command at this position
                 */
                Command *undoCommand(size_t pos);

                /**
                 * Returns whether there are any @ref Command objects that can
                 * be redone.
                 *
                 * When this information changes a notification will be sent.
                 *
                 * @return True if there are any redos
                 */
                bool redos() const;

                /**
                 * Redoes the last undone @ref Command.
                 */
                void redo();

                /**
                 * Returns the command @p places down in the redo history.
                 * For example, redoCommand(0) returns the next @ref Command
                 * to be redone.
                 *
                 * If there is no command to be redone at this @p pos then
                 * returns zero.
                 *
                 * @param  pos  Index into redo history
                 * @return @ref Command at this position
                 */
                Command *redoCommand(size_t pos);

                /**
                 * Returns the value of the history size limit. This is the
                 * number of @ref Command objects that are stored.
                 *
                 * If the value is -1 then the number of undos is unlimited.
                 *
                 * Clearly, if the value is 0 then undo is effectively 'off'.
                 *
                 * @return CommandHistory size limit
                 */
                int limit() const;

                /**
                 * Sets the CommandHistory limit value. If there are more
                 * @ref Command objects than the new limit currently in the
                 * CommandHistory, they will be removed.
                 *
                 * @param limit New CommandHistory size limit
                 */
                void setLimit(int limit);

                /**
                 * Clears all undo and redo actions in the CommandHistory.
                 *
                 * @see clearUndos
                 * @see clearRedos
                 */
                void clear()
                {
                    clearUndos();
                    clearRedos();
                }

                /**
                 * Clears all undo actions in the CommandHistory.
                 *
                 * @see clear
                 * @see clearRedos
                 */
                void clearUndos();

                /**
                 * Clears all redo actions in the CommandHistory.
                 *
                 * @see clear
                 * @see clearUndos
                 */
                void clearRedos();

                /**
                 * The default CommandHistory limit size.
                 */
                static const int defaultLimit = 20;

            private:

                int _limit;
                std::list<Command*> undolist;
                std::list<Command*> redolist;
        };
    }
}

#endif
