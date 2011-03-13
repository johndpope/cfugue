/*
 * @(#)ports.cpp 3.00 17 May 2002
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

    /***************************************************************
     * TSE3 example port enumeration program
     * =====================================
     *
     * This program uses the MidiScheduler class to find out
     * what ports are available on your system and prints them
     * to standard out.
     *
     **************************************************************/

#include <iostream>
#include <cstring>

#include "tse3/MidiScheduler.h"

int main(int, char *[])
{
    using std::cout;

    TSE3::MidiSchedulerFactory factory;

    // You might want to influence what type of scheduler is produced here.
    // See tse3/plt/Factory.h for details.

    TSE3::MidiScheduler *sch = factory.createScheduler();

    if (sch)
    {
        cout << "MidiScheduler details follow\n"
             << "\n  Implementation name: " << sch->implementationName().c_str()
             << "\n            Num ports: " << sch->numPorts();
        std::vector<int> portNums;
        sch->portNumbers(portNums);
        for (size_t port = 0; port < sch->numPorts(); port++)
        {            
            cout << "\n  ------- Port number: " << portNums[port]
                 << "\n                 Type: " << sch->portType(portNums[port]).c_str()
                 << "\n                 Name: " << sch->portName(portNums[port]).c_str()
                 << "\n          Is readable: ";
            if (sch->portReadable(portNums[port]))
                cout << "Yes";
            else
                cout << "No";
            cout << "\n         Is writeable: ";
            if (sch->portWriteable(portNums[port]))
                cout << "Yes";
            else
                cout << "No";
        }
        cout << "\n\n";
    }
    else
    {
        cout << "Couldn't create a MidiScheduler for this platform";
    }
    return 0;
}
