/*
 * @(#)cmd/Command.h 3.00 10 June 1999
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

#ifndef TSE3_CMD_COMMAND_H
#define TSE3_CMD_COMMAND_H

#include <string>

namespace TSE3
{
    /**
     * The Cmd namespace contains classes that implement the Command
     * pattern (GoF book). They use the @ref TSE3 API to provide undo/redoable
     * commands objects for all commonly performed operations.
     *
     * There are a large number of commands that can be used for different
     * purposes. The base class for all @ref TSE3 commands is the
     * @ref Command. There exists one command class for every useful
     * operation on the @ref TSE3::Song data structure and it's subcomponents.
     *
     * There is one proviso with using commands: if you manipulate some
     * of a @ref TSE3::Song with commands, for the undo/redo to work you
     * must (logically) manipulate the @ref TSE3::Song entirely through
     * @ref Command objects. Otherwise the undo/redo system will be
     * faced with complicated inconsistencies which cannot be resolved.
     *
     * You can store executed @ref Command objects on the
     * @ref CommandHistory buffer to automate the undo/redo process.
     *
     * @ref Command subclasses are easy to write, so it's not difficult
     * to add your own commands and use them with the @ref CommandHistory.
     * See the notes in the main TSE3 library documentation for more
     * information.
     *
     * @short   @ref TSE3 library undo/redoable commands
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     TSE3
     */
    namespace Cmd
    {
        /**
         * A base class for implementing the 'Command' design pattern [Gof]
         *
         * If you are implementing a new Command then inherit publicly
         * from this class, implement the @ref executeImpl() and
         * @ref undoImpl() methods and ensure that your constructor calls the
         * Command protected constructor.
         *
         * @short   Base command class
         * @author  Pete Goodliffe
         * @version 3.00
         */
        class Command
        {
            public:

                virtual ~Command() {}

                /**
                 * Call this to execute the Command.
                 *
                 * If the Command has already been executed, nothing will
                 * happen.
                 *
                 * @see undo
                 */
                void execute()
                {
                    if (!_done)
                    {
                        executeImpl();
                        _done = true;
                    }
                }

                /**
                 * Call this to undo the Command.
                 *
                 * If the Command can't be undone, or it has already been
                 * undone, then nothing will happen.
                 */
                void undo()
                {
                    if (_done && _undoable)
                    {
                        undoImpl();
                        _done = false;
                    }
                }

                /**
                 * Returns a descriptive string for the command.
                 *
                 * The string begins with a lower case letter and is capable
                 * of following "Undo " or "Redo ".
                 */
                const std::string &title() const { return _title; }

                /**
                 * Returns whether this Command is capable of being undone.
                 */
                bool undoable() const { return _undoable; }

                /**
                 * Returns whether this command has been executed.
                 *
                 * @see execute
                 * @see undo
                 */
                bool done() const { return _done; }

            protected:

                /**
                 * Create a Command with a descriptive string. This must begin
                 * with a lower case letter and be capable of following "Undo "
                 * or "Redo ".
                 *
                 * @param title    Name for Command
                 * @param undoable Whether the Command is undoable
                 */
                Command(const std::string &title, bool undoable = true)
                : _title(title), _undoable(undoable), _done(false)
                {}

                /**
                 * Implementations of Command override this method to implement
                 * the execute action.
                 */
                virtual void executeImpl() = 0;

                /**
                 * Implementations of Command override this method to implement
                 * to undo action.
                 */
                virtual void undoImpl() = 0;

                /**
                 * Sets the Command title. Normally you will do this in
                 * the ctor and never use this.
                 */
                void setTitle(const std::string &title) { _title = title; }

            private:

                std::string _title;
                bool        _undoable;
                bool        _done;
        };

        /**
         * A template class for implementing @ref Command objects that set a
         * particular class' variable state.
         *
         * The template takes five parameters:
         * @li cls_type   - the type of the class to be altered
         * @li arg_type   - the type of variable being set
         * @li param_type - the type of variable that is accepted as a parameter
         * @li getmfn     - cls_type's method to read the variable,
         *                  returns param_type
         * @li setmfn     - cls_type's method to set the variable, takes
         *                  param_type
         *
         * @short   Variable setting command template class
         * @author  Pete Goodliffe
         * @version 3.00
         */
        template <class cls_type, class arg_type, class param_type,
                  param_type (cls_type::*getmfn)() const,
                  void (cls_type::*setmfn)(param_type)>
        class VariableSetCommand : public Command
        {
            public:
                VariableSetCommand(cls_type *cls, param_type arg,
                                   const std::string &title)
                : Command(title), cls(cls), arg(arg) {}
                virtual ~VariableSetCommand() {}
            protected:
                /**
                 * @reimplemented
                 */
                virtual void executeImpl()
                {
                    old = (cls->*getmfn)();
                    (cls->*setmfn)(arg);
                }
                /**
                 * @reimplemented
                 */
                virtual void undoImpl()
                {
                    (cls->*setmfn)(old);
                }
            private:
                cls_type *cls;
                arg_type  arg;
                arg_type  old;
        };
    }
}

#endif
