/*
 * @(#)recording.cpp 3.00 06 Mar 2003
 *
 * Copyright (c) 2003 Pete Goodliffe (pete@cthree.org)
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
     * TSE3 recording application
     * ==========================
     *
     * This example contains three "steps"
     *
     *   1. We create TSE3 objects to do recording.
     *   3. We record eight bars of midi data.
     *   4. We export this recorded data as a MIDI file.
     *
     **************************************************************/

#include <iostream>
#include <stdlib.h>

// Used in step 1
#include "tse3/Metronome.h"
#include "tse3/util/MidiScheduler.h"
#include "tse3/Transport.h"

// Used in step 2
#include "tse3/Song.h"
#include "tse3/PhraseEdit.h"
#include "tse3/Part.h"
#include "tse3/Track.h"

// Used in step 3
#include "tse3/MidiFile.h"


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Useage: recording <filename>\n";
        exit(1);
    }

    /**************************************************************************
     * 1. Create a Transport object to do some recording with
     *************************************************************************/

    TSE3::Metronome             metronome;
    TSE3::MidiSchedulerFactory  msFactory;
    TSE3::MidiScheduler        *scheduler = msFactory.createScheduler();
    TSE3::Transport             transport(&metronome, scheduler);

    /**************************************************************************
     * 2. Record some MIDI events
     *************************************************************************/

    TSE3::Song         song;
    TSE3::PhraseEdit   phraseEdit;
    const TSE3::Clock  stopTime    = TSE3::Clock::PPQN*4*8;

    // Record the events. Display hashes for each input event
    std::cout << "Recording MIDI data now...\n";
    size_t counter = phraseEdit.size();
    transport.record(&song, 0, &phraseEdit, 0);
    while (scheduler->clock() < stopTime)
    {
        transport.poll();

        // Print a hash if something was recieved
        if (phraseEdit.size() != counter)
        {
            std::cout << "#";
            counter = phraseEdit.size();
        }

        // Perhaps sleep here to prevent slaughtering the CPU
    }
    transport.stop();
    std::cout << "\nFinished recording. There were "
              << phraseEdit.size() << " input events.\n";

    // Having finished recording, we have to "tidy" the recorded data
    // See the docs for this function to understand why.
    phraseEdit.tidy();
    std::cout << "This tidies to " << phraseEdit.size() << " events.\n";

    // Now we've recorded a phrase of data, we have to insert it into the Song
    // This means:
    //   i)  Insert it in the PhraseList
    //   ii) Create a Part (in a Track) using it
    TSE3::Phrase *phrase = phraseEdit.createPhrase(song.phraseList());
    TSE3::Part   *part   = new TSE3::Part(0, stopTime);
    part->setPhrase(phrase);
    song[0]->insert(part); // We rely on track 0 existing

    /**************************************************************************
     * 3. Export as a MIDI file
     *************************************************************************/

    // Created with default parameters, so you'll get a type 1 MIDI file
    TSE3::MidiFileExport midiExport;

    midiExport.save(argv[1], &song);

    /**************************************************************************
     * All done
     *************************************************************************/

    delete scheduler;
    return 0;
}
