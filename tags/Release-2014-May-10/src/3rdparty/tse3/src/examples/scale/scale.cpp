/*
 * @(#)scale.cpp 3.00 3 Oct 2001
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
     * TSE3 example scale program
     * ==========================
     *
     * This example contains three "steps"
     *
     *   1. We create a Song containing a scale (based on a
     *      specified root note).
     *   2. We play it
     *   3. We save it as a standard MIDI file.
     *
     **************************************************************/

// Used in step 1
#include "tse3/Song.h"
#include "tse3/PhraseEdit.h"
#include "tse3/Part.h"
#include "tse3/Track.h"

// Used in step 2
#include "tse3/Metronome.h"
#include "tse3/util/MidiScheduler.h"
#include "tse3/Transport.h"

// Used in step 3
#include "tse3/MidiFile.h"

// Specifies the root MIDI note of the scale
const int rootNote = 0x60;

// Duration of each note in the scale
const int duration = TSE3::Clock::PPQN;

int main()
{
    /**************************************************************************
     * 1. Create a Song containing a scale
     *************************************************************************/

    // First, we create a scale in a PhraseEdit

    TSE3::PhraseEdit phraseEdit;
    int              note = rootNote;
    TSE3::Clock      time = 0;
    for (int n = 0; n < 8; n++)
    {
        const int delta[]  = {2, 2, 1, 2, 2, 2, 1, 0};
        const int velocity = 0x60;
        const int channel  = 0;
        const int port     = 0;

        phraseEdit.insert
            (TSE3::MidiEvent(TSE3::MidiCommand(TSE3::MidiCommand_NoteOn,
                                               channel, port, note, velocity),
             time, velocity, time+duration));

        note += delta[n];
        time += duration;
    }

    // Now assemble the Song
    TSE3::Song    song(1);
    TSE3::Phrase *phrase = phraseEdit.createPhrase(song.phraseList());
    TSE3::Part   *part   = new TSE3::Part(0, phraseEdit.lastClock());
    part->setPhrase(phrase);
    song[0]->insert(part);

    /**************************************************************************
     * 2. Play the Song
     *************************************************************************/

    // Create transport objects
    // (You really want to create a MidiScheduler for your platform, perhaps
    // you'll use the UnixMidiSchedulerFactory)
    TSE3::Metronome                 metronome;
    TSE3::Util::StreamMidiScheduler scheduler;
    TSE3::Transport                 transport(&metronome, &scheduler);

    // Play and wait for the end
    transport.play(&song, 0);
    while (transport.status() != TSE3::Transport::Resting)
    {
        transport.poll();
        // perhaps sleep here to prevent slaughtering the CPU
    }

    /**************************************************************************
     * 3. Save the Song as a standard MIDI file
     *************************************************************************/

    TSE3::MidiFileExport mfe;
    mfe.save("export.midi", &song);

    /**************************************************************************
     * All done - note that there is no memory leak - when the Song is deleted
     * (it is on the stack, so this will happen automatically) all the other
     * components will be deleted by the Song's destructor.
     *************************************************************************/

    return 0;
}
