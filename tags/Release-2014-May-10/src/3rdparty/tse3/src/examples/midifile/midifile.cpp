/*
 * @(#)midifile.cpp 3.00 28 Nov 2001
 *
 * Copyright (c) 2001 Pete Goodliffe (pete@cthree.org)
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
     * TSE3 example MIDI file program
     * ==============================
     *
     * This example contains four "steps"
     *
     *   1. We use a MidiFile object to access a standard MIDI file.
     *   2. We create a Transport object to play it with
     *   3. We create a Song object from the data in the MIDI file
     *      and play it.
     *   4. Creating a Song takes some time (well, not too much really).
     *      If you don't need to manipulate the data in the MIDI file,
     *      just play it, this shows a slightly quicker way.
     *
     **************************************************************/

#include <iostream>

// Used in step 1
#include "tse3/MidiFile.h"

// Used in step 2
#include "tse3/Metronome.h"
#include "tse3/util/MidiScheduler.h"
#include "tse3/Transport.h"

// Used in step 3
#include "tse3/Song.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Useage: midifile <filename>\n"; return -1;
    }

    /**************************************************************************
     * 1. Load the MIDI file
     *************************************************************************/

    TSE3::MidiFileImport mfi(argv[1]);

    /**************************************************************************
     * 2. Create a Transport object to do some playback with
     *************************************************************************/

    // (You really want to create a MidiScheduler for your platform)
    TSE3::Metronome                 metronome;
    TSE3::Util::StreamMidiScheduler scheduler;
    TSE3::Transport                 transport(&metronome, &scheduler);

    /**************************************************************************
     * 3. Create a Song object from the file, and play the Song
     *************************************************************************/

    TSE3::Song *song = mfi.load();

    transport.play(song, 0);
    while (transport.status() != TSE3::Transport::Resting)
    {
        transport.poll();
        // perhaps sleep here to prevent slaughtering the CPU
    }

    /**************************************************************************
     * 4. The quicker way to just play a MIDI file
     *************************************************************************/

    transport.play(&mfi, 0);
    while (transport.status() != TSE3::Transport::Resting)
    {
        transport.poll();
        // perhaps sleep here to prevent slaughtering the CPU
    }

    /**************************************************************************
     * All done
     *************************************************************************/

    delete song;
    return 0;
}
