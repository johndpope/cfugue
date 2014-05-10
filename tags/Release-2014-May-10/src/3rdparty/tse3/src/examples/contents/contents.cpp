/*
 * @(#)contents.cpp 3.00 20 February 2002
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
     * TSE3 example MIDI file display program
     * ======================================
     *
     * This program opens a MIDI file, and streams its contents
     * to std::out
     *
     * This example contains three "steps"
     *
     *   1. We create the scheduler objects
     *   2. We load a song
     *   3. We stream the contents to a StreamMidiScheduler to
     *      print it out.
     *
     **************************************************************/

#include <iostream>

// Used in step 1
#include "tse3/Metronome.h"
#include "tse3/util/MidiScheduler.h"
#include "tse3/Transport.h"

// Used in step 2
#include "tse3/MidiFile.h"
#include "tse3/Song.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Useage: contents <filename>\n";
    }

    /**************************************************************************
     * 1. Create the scheduler objects
     *************************************************************************/

    TSE3::Metronome                 metronome;
    TSE3::Util::StreamMidiScheduler scheduler;
    TSE3::Transport                 transport(&metronome, &scheduler);

    /**************************************************************************
     * 2. Load the Song
     *************************************************************************/

    TSE3::MidiFileImport  midiFile(argv[1]);
    TSE3::Song           *song = midiFile.load();

    /**************************************************************************
     * 2. Play the Song
     *************************************************************************/

    TSE3::PlayableIterator *play = song->iterator(0);

    while(play->more())
    {
        TSE3::MidiEvent event = **play;
        play++;
        scheduler.tx(event);
    }

    /**************************************************************************
     * All done
     *************************************************************************/

    delete song;

    return 0;
}
