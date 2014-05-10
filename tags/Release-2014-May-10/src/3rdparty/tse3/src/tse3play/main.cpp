/*
 * @(#)main.cpp 3.00 6 July 1999
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

#include <cstdlib>
#include "tse3play/tse3play.h"
#include "tse3/Error.h"

using namespace TSE3_Utilities_Play;

/*
void handleSignal(int signal)
{
    if (signal == SIGHUP)
    {
        std::cerr << "Got SIGHUP" << endl;
        return;
    }

    std::cerr << "Caught signal " << signal << " - exiting" << endl;
    exit(1);
}*/


int TSE3PlayMain(int argc, char *argv[])
{
    try
    {
        TSE3Play tseplay(argc, argv);
        return tseplay.go();
    }
    catch (const TSE3::Error &e)
    {
        std::cerr << "TSE3 error detected: " << TSE3::errString(e.reason())
                  << "\n";
        return 1;
    }
}


int main(int argc, char *argv[])
{
    return TSE3PlayMain(argc, argv);
}
