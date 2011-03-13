/*
 * @(#)CommandGroup.cpp 3.00 5 September 2000
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

#include "tse3/cmd/CommandGroup.h"

#include <iostream>

using namespace TSE3;
using namespace TSE3::Cmd;

CommandGroup::CommandGroup(const std::string &title)
: Command(title), canAdd(true)
{
}


CommandGroup::~CommandGroup()
{
    // delete every command
    while (!cmds.empty())
    {
        delete cmds.back();
        cmds.pop_back();
    }
}


void CommandGroup::add(Command *command)
{
    if (canAdd)
    {
        cmds.push_back(command);
        if (title() == "")
        {
            setTitle(command->title());
        }
    }
    else
    {
        std::cerr << "TSE3: Bad attempt to add a Command to a CommandGroup\n";
    }
}


void CommandGroup::executeImpl()
{
    canAdd = false;

    std::vector<Command*>::iterator i = cmds.begin();
    while (i != cmds.end())
    {
        (*i)->execute();
        i++;
    }
}


void CommandGroup::undoImpl()
{
    std::vector<Command*>::reverse_iterator i = cmds.rbegin();
    while (i != cmds.rend())
    {
        (*i)->undo();
        i++;
    }
}

