/*
 * @(#)Application.cpp 3.00 25 May 1999
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

#include "tse3/app/Application.h"

#include "tse3/app/Record.h"
#include "tse3/app/Choices.h"
#include "tse3/Metronome.h"
#include "tse3/Transport.h"
#include "tse3/Error.h"
#include "tse3/Song.h"
#include "tse3/DisplayParams.h"
#include "tse3/cmd/CommandHistory.h"
#include "tse3/util/MidiScheduler.h"
#include "tse3/ins/Destination.h"

#include <algorithm>

using namespace TSE3;
using namespace TSE3::App;

Application::Application(const std::string &a, const std::string &v,
                         MidiSchedulerFactory *msf,
                         const std::string &cf)
: _appname(a), _appversion(v), _choicesFile(cf), _record(0)
{
    // Create aggregate objects
    _metronome = new Metronome;
    try
    {
        _scheduler = msf->createScheduler();
    }
    catch (const TSE3::Error &e)
    {
        std::cerr << "TSE3: Failed to create MidiScheduler, "
                  << "creating NullMidiScheduler instead\n";
        _scheduler = new Util::NullMidiScheduler();
    }
    _transport     = new Transport(_metronome, _scheduler);
    _cm            = new ChoicesManager;
    _destination   = new Ins::Destination();
    _presetColours = new PresetColours;

    // Set up Application object
    _saveChoicesOnDestroy = false;

    // Populate the ChoicesManager
    _cm->add(new ApplicationChoiceHandler(this));
    _cm->add(new MetronomeChoiceHandler(_metronome));
    _cm->add(new TransportChoiceHandler(_transport));
    _cm->add(new DestinationChoiceHandler(_destination, _scheduler));

    // Load any application choices that may have been saved previously
    if(!_choicesFile.empty())
    {
        _cm->load(_choicesFile);
    }
}


Application::~Application()
{
    if (_saveChoicesOnDestroy)
    {
        _cm->save(_choicesFile);
    }
    delete _presetColours;
    delete _destination;
    delete _cm; // this deletes ChoicesHandlers too
    delete _transport;
    delete _metronome;
    delete _scheduler;
}


Record *Application::record() const
{
    if (!_record)
    {
        Application *me = const_cast<Application*>(this);
        me->_record = new Record(_transport);
    }
    return _record;
}


void Application::setSaveChoicesOnDestroy(bool s)
{
    _saveChoicesOnDestroy = s;
    // notify
}


void Application::saveChoices(const std::string &filename)
{
    if (filename != "")
    {
        _cm->save(filename);
    }
    else if (_choicesFile != "")
    {
        _cm->save(_choicesFile);
    }
}


TSE3::Song *Application::addSong(TSE3::Song *song)
{
    if (!song)
    {
        song = new TSE3::Song();
    }
    songs.push_back(song);
    histories[song] = new TSE3::Cmd::CommandHistory();
    return song;
}


size_t Application::numSongs() const
{
    return songs.size();
}


TSE3::Cmd::CommandHistory *Application::history(TSE3::Song *song)
{
    if (find(songs.begin(), songs.end(), song) == songs.end())
    {
        return 0;
    }
    return histories[song];
}


void Application::Notifier_Deleted(TSE3::Song *song)
{
    if (find(songs.begin(), songs.end(), song) != songs.end())
    {
        Cmd::CommandHistory *history = histories[song];
        histories.erase(song);
        delete history;
    }
}

