/**
 * Dodgy test harness.
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
#include "tse3/Track.h"
#include "tse3/FlagTrack.h"
#include "tse3/TempoTrack.h"
#include "tse3/Part.h"
*/

#include "tse3/Song.h"
#include "tse3/PhraseEdit.h"
#include "tse3/Serializable.h"
#include "tse3/Transport.h"
#include "tse3/MidiFile.h"
#include "tse3/TSE3MDL.h"
#include "tse3/file/XML.h"
#include "tse3/file/Write.h"

//#include "TSE3Commands/CommandHistory.h"
//#include "TSE3Commands/SongCommands.h"

#include <fstream>

using namespace TSE3;

void play(Playable *p);
void display(Song *s);

extern void insTest();

int main()
{
  using std::cout;

  cout << "Started...\n\n";
/*
  // This will prove that my representation of MidiCommands is equivalent to
  // the old TSE2 word typedef.
  MidiCommand com1(MidiCommand_NoteOn, 0, 0, 0x40, 100);
  cout << "command is " << *((int*)&com1) << "\n\n";

  // Proves I can actually create these objects ;-)
  MidiScheduler s;
  Track t;
  cout << "Created a PhraseEdit and everything else\n";

  // This will prove the Notifier framework.
  t.params.setMute(false);
  t.params.setVolume(127);
  t.params.setVolume(128);

  // Test MidiParams p[laybale interface
  t.params.moveTo(0);
  t.params.setBankLSB(0x50);
  t.params.setBankMSB(0x40);
  t.params.setChorus(0x30);
  t.params.setProgram(0x20);
  do
  {
    MidiEvent e = t.params.event();
    cout << "Event is " << e.event.data.status << "," << e.event.data.data1
         << "," << e.event.data.data2<< " (" << *((int*)&(e.event.data))
         << ")\n";
  } while (t.params.consume());

  // Try out flag tracks
  cout << "\nCreating FlagTrack\n";
  FlagTrack f;
  f.insert(Event<Flag>(Flag("Time 1"), 0));
  f.insert(Event<Flag>(Flag("Time 2"), Clock::PPQN));
  f.insert(Event<Flag>(Flag("Time 4"), Clock::PPQN*4));
  f.insert(Event<Flag>(Flag("Time 3"), Clock::PPQN*2));
  f.insert(Event<Flag>(Flag("Time X"), Clock::PPQN*3));
  f.erase(Event<Flag>(Flag("Time X"), Clock::PPQN*3));
  for (int i = 0; i < f.size(); ++i)
  {
    cout << "  " << i << ": " << f[i].time << " - " << f[i].data.title << "\n";
  }

  int pos = f.index(Clock::PPQN*2.5);
  cout << "\nIndex at " << Clock::PPQN*2.5 << " is " << pos << "\n\n";

  // Test the TempoTrack
  TempoTrack tt;
  tt.insert(Event<Tempo>(100, 0));
  tt.insert(Event<Tempo>(120, 2*Clock::PPQN));
  cout << "Playing TempoTrack...\n";
  play(&tt);
*/
  // Test utilities
  /*
  string str;
  NoteNumberConversion::numberToNote(60,str);
  cout << "Note string is: '" << str << "'\n";
  int note = NoteNumberConversion::noteToNumber("C#-6");
  cout << "Note number is: " << note << "\n\n\n";
  */

  // Test the PhraseEdit
  /*
  PhraseEdit pe;
  pe.insert(MidiEvent(MidiCommand(MidiCommand_ProgramChange, 0, 0, 10),
                      0));
  pe.insert(MidiEvent(MidiCommand(MidiCommand_ProgramChange, 0, 0, 20),
                      Clock::PPQN));
  pe.insert(MidiEvent(MidiCommand(MidiCommand_ProgramChange, 0, 0, 30),
                      Clock::PPQN*2));
  pe.insert(MidiEvent(MidiCommand(MidiCommand_ProgramChange, 0, 0, 40),
                      Clock::PPQN*3));
  cout << "Phrase edit size is " << pe.size() << "\n";
  Phrase *phr = pe.createPhrase();
  cout << "New Phrase size is " << phr->size() << "\n\n";
  */

/*
  // Test a Part
  Part part;
  part.setPhrase(phr);
  part.params.setPan(100);
  part.params.setReverb(90);
  part.params.setChorus(80);
  part.setRepeat(Clock::PPQN*4);
  part.setStart(0);
  part.setEnd(Clock::PPQN*8);
  //cout << "\n\nPlaying Part...\n";
  //play(&part);

  Track track;
  track.insert(&part);
  play(&track);
*/

  Song song;
  cout << "No Tracks in the song is: " << song.size() << "\n";
/*
  song.track(0)->insert(0, Clock::PPQN*4)->setPhrase(phr);
  song.track(0)->insert(Clock::PPQN*12, Clock::PPQN*14)->setPhrase(phr);
  song.track(0)->insert(Clock::PPQN*8, Clock::PPQN*9)->setPhrase(phr);
  cout << song.track(0)->size() << "\n\n";

  play(song.track(0));

  song.track(1)->insert(Clock::PPQN*16, Clock::PPQN*20)->setPhrase(phr);

  play(&song);
*/

  // Load a TSE2 song
  /*
  FileOperations_2 fo2;
  fo2.load(&song, "song.tsemdl");
  cout << "No of tracks in the song is " << song.size() << ". Marvelous.\n\n";
  //play((*(song.track(0)))[0]);
  //play(song.track(0));
  //play(&song);
  //display(&song);
  */

  /*
  // Load a MIDI file
  cout << "Importing test.mid...\n";
  Song *newSong;
  try
  {
      MidiFileImport import("test.mid", 2, cout);
      newSong = import.import();
  }
  catch (MidiFileImportError &e)
  {
      cout << "Exception thrown: " << *e;
  }
  */

  // Testing PhraseList::newPhraseTitle
  /*
  PhraseEdit pe;
  Phrase *p = pe.createPhrase();
  p->setTitle(song.phraseList.newPhraseTitle(PhraseList_NewPhraseString));
  song.phraseList.insert(p);
  cout << "New Phrasename: " << p->title();
  p = pe.createPhrase();
  p->setTitle(song.phraseList.newPhraseTitle(PhraseList_NewPhraseString));
  song.phraseList.insert(p);
  cout << "New Phrasename: " << p->title();
  */

  //cout << (bool)(*(song.track(0)))[0]->filter.status() << "\n";

  // Test the scheduler/transport bobbins
/*  Metronome             metronome;
  MidiSchedulerFactory  msf;
  MidiScheduler        *scheduler = msf.createScheduler();
  Transport     transport(&metronome, scheduler);
  cout << "Scheduler type is '" << scheduler->implementationName() << "'\n";

  // Test playing
  cout << "Playing the song through the Transport system...\n";
  transport.play(&song, 0);
  while (transport.status() != Transport::resting) transport.poll();
*/
  // Testing saving
  /*
  cout << "Saving Song in TSE3MDL format...\n";
  TSE3MDL_File file(&song);
  ofstream o("song.tse3mdl");
  file.save(o, 0);
  o.close();
  */

  // Testing loading
  /*
  cout << "Loading file in TSE3MDL format...\n";
  TSE3MDL loader;
  Song *newSong
      = loader.load("/export/pgoodlif/work/TSE3/tse3/misc/Demo.tse3");
  cout << newSong->title() << endl;
  cout << "No Tracks in the song is: " << song.size() << "\n";

  cout << "Writing in the XML stylee...\n";
  {
      std::ofstream out("out.xml");
      TSE3::File::XmlFileWriter writer(out);
      TSE3::File::writeSong(writer, *newSong);
  }
  */
  cout << "Loading file in XML format...\n";
  std::ifstream in("out.xml");
  TSE3::File::XmlFileReader reader(in);
  Song *newSong = reader.load();
  newSong = newSong;


  // Testing Command interface
  /*
  CommandHistory history;
  Song_SetTitle s(newSong, "This is a new Song title");
  cout << "Song title is " << newSong->title() << endl;
  s.execute();
  cout << "Song title is " << newSong->title() << endl;
  history.add(&s);
  history.undo();
  cout << "Song title is " << newSong->title() << endl;
  history.redo();
  cout << "Song title is " << newSong->title() << endl;
  */

  // Testing playing a Panic object
  /*
  Panic panic;
  panic.setAllNotesOff(true);
  cout << "Playing a Panic object...\n";
  transport.play(&panic, 0);
  while (transport.status() != Transport::resting) transport.poll();
  */

  // Testing the file recogniser
  //FileRecogniser fr("song.tse3mdl");
  /*
  FileRecogniser fr("test.mid");
  cout << "File format is " << fr.type() << endl;
  */

  // Test Cakewalk .ins file import routines
  //insTest();

/*
  cout << "-----------------------------------------\n";
  Track *track = newSong->track(0);
  //play((*track)[0]);
  play(newSong);
  cout << "-----------------------------------------\n";
*/

  /*
  // Testing MidiFileExport
  ofstream o("export.mid", ios::binary | ios::out);
  MidiFileExport mfe(newSong, 1, o, true, 2);
  mfe.export();
  o.close();
  */

  cout << "\n\nFinishing...\n";

  return 0;

}


/* ----------------------------------------------------------------------------
 * Useful functions
 */

#if 0
void play(Playable *p)
{

    cout << "Creating iterator:\n";
    PlayableIterator *i = p->iterator(0);
    cout << "Playing a playable:\n";
    while (i->more())
    {
        MidiEvent e = *(*i);
        if (e.event.data.status)
            cout << "  MidiEvent: " << e.event.time
                 << " - " << e.event.data.status << ":"
                 << e.event.data.data1
                 << ":" << e.event.data.data2 << "\n";
        ++(*i);
    }

    cout << "End of playable\n\n";

}


void display(Song *s)
{
    for (int n = 0; n < s->size(); ++n)
    {
        Track *t = s->track(n);
        cout << "-- ";
        for (int m = 0; m < t->size(); ++m)
        {
            Part *p = (*t)[m];
            cout << "|"<<p->phrase()->title();
            cout << ","<<p->start()<<"-"<<p->end();
            cout <<"| ";
        }
        cout << "\n";
    }
}

/*
void insTest()
{
    cout << ">> insTest\n";
    CakewalkInstrumentFile cif("Yamaha.ins");

    cout << "Created the ins file object.\n";

    cout << "Instruments in this file are (" << cif.instruments().size() << "):\n";
    list<string>::const_iterator i = cif.instruments().begin();
    int n = 0;
    while (i != cif.instruments().end())
    {
        cout << n << "  " << *i << endl ;
        ++n;
        ++i;
    }

    cout << "\nTrying to create an instrument...\n";
    Instrument *ins = cif.instrument("Yamaha SY-35");

    cout << "\nTrying to write an instrument...\n";
    ofstream outfile("out.ins");
    ins->write(outfile);
    outfile.close();

    cout << "<< insTest\n";
}
*/
#endif
