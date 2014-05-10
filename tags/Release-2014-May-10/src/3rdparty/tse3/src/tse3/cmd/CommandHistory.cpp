/*
 * @(#)CommandHistory.cpp 3.00 20 July 1999
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

#include "tse3/cmd/CommandHistory.h"

using namespace TSE3::Cmd;


/******************************************************************************
 * CommandHistory class
 *****************************************************************************/

CommandHistory::CommandHistory(int l)
: _limit(l)
{
}


void CommandHistory::add(Command *c)
{
    bool sendNotify = (undolist.size() == 0);
    undolist.push_front(c);
    if (!redolist.empty())
    {
        while (!redolist.empty())
        {
            delete redolist.back();
            redolist.pop_back();
        }
        notify(&CommandHistoryListener::CommandHistory_Redo);
    }
    if (_limit != -1 && undolist.size() > static_cast<size_t>(_limit))
    {
        delete undolist.back();
        undolist.pop_back();
    }
    if (!c->undoable())
    {
        while (!undolist.empty())
        {
            delete undolist.back();
            undolist.pop_back();
        }
    }
    if (sendNotify) notify(&CommandHistoryListener::CommandHistory_Undo);
}


bool CommandHistory::undos() const
{
    return undolist.size() > 0;
}


void CommandHistory::undo()
{
    if (undolist.empty()) return;
    undolist.front()->undo();
    bool sendRedoNotify = (redolist.size() == 0);
    redolist.push_front(undolist.front());
    undolist.pop_front();

    if (undolist.size() == 0)
        notify(&CommandHistoryListener::CommandHistory_Undo);
    if (sendRedoNotify)
        notify(&CommandHistoryListener::CommandHistory_Redo);
}


Command *CommandHistory::undoCommand(size_t pos)
{
    Command *cmd = 0;

    if (pos < undolist.size())
    {
        std::list<Command*>::iterator i = undolist.begin();
        for (size_t n = 0; n < pos && i != undolist.end(); n++)
        {
            i++;
        }
        cmd = *i;
    }

    return cmd;
}


bool CommandHistory::redos() const
{
    return redolist.size() > 0;
}


void CommandHistory::redo()
{
    if (redolist.empty()) return;
    redolist.front()->execute();
    bool sendUndoNotify = (undolist.size() == 0);
    undolist.push_front(redolist.front());
    redolist.pop_front();

    if (redolist.size() == 0)
        notify(&CommandHistoryListener::CommandHistory_Redo);
    if (sendUndoNotify)
        notify(&CommandHistoryListener::CommandHistory_Undo);
}


Command *CommandHistory::redoCommand(size_t pos)
{
    Command *cmd = 0;

    if (pos < redolist.size())
    {
        std::list<Command*>::iterator i = redolist.begin();
        for (size_t n = 0; n < pos && i != redolist.end(); n++)
        {
            i++;
        }
        cmd = *i;
    }

    return cmd;
}


int CommandHistory::limit() const
{
    return _limit;
}


void CommandHistory::setLimit(int l)
{
    if (l < -1) l = -1;
    _limit = l;
    while (_limit == -1 || undolist.size() > static_cast<size_t>(_limit))
    {
        delete *(undolist.end());
        undolist.erase(undolist.end());
    }
    while (_limit == -1 || redolist.size() > static_cast<size_t>(_limit))
    {
        delete *(redolist.end());
        redolist.erase(redolist.end());
    }

    // Send notification whatever (this doesn't happen often ;-)
    notify(&CommandHistoryListener::CommandHistory_Undo);
    notify(&CommandHistoryListener::CommandHistory_Redo);
}


void CommandHistory::clearUndos()
{
    if (undolist.size() != 0)
    {
        undolist.clear();
        notify(&CommandHistoryListener::CommandHistory_Undo);
    }
}


void CommandHistory::clearRedos()
{
    if (redolist.size() != 0)
    {
        redolist.clear();
        notify(&CommandHistoryListener::CommandHistory_Redo);
    }
}


