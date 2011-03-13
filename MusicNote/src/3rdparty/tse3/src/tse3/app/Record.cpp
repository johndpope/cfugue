/*
 * @(#)Record.cpp 3.00 19 June 2000
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

#include "tse3/app/Record.h"

#include "tse3/Song.h"
#include "tse3/Transport.h"
#include "tse3/PhraseEdit.h"
#include "tse3/Error.h"
#include "tse3/MidiFilter.h"
#include "tse3/PhraseList.h"
#include "tse3/Part.h"
#include "tse3/Track.h"
#include "tse3/MidiScheduler.h"
#include "tse3/cmd/Part.h"
#include "tse3/cmd/Phrase.h"
#include "tse3/cmd/CommandHistory.h"
#include "tse3/cmd/CommandGroup.h"

using namespace TSE3;
using namespace TSE3::App;

/******************************************************************************
 * Record class
 *****************************************************************************/

Record::Record(TSE3::Transport *t)
: _transport(t), _phraseEdit(0), _startTime(-1), _endTime(-1), recording(false)
{
    attachTo(_transport);
}


Record::~Record()
{
}


void Record::start(TSE3::Song *s, TSE3::Track *t)
{
    if (!recording && _phraseEdit)
    {
        reset();
    }
    if (!recording && _transport->status() == TSE3::Transport::Resting)
    {
        _startTime  = _transport->scheduler()->clock();
        _phraseEdit = new TSE3::PhraseEdit();
        _song       = s;
        _track      = t;
        TSE3::MidiFilter *f = t ? t->filter() : 0;
        _transport->record(_song, _startTime, _phraseEdit, f);
        recording = true;
    }
    else if (recording && _phraseEdit)
    {
        stop();
    }
}


void Record::stop()
{
    if (_phraseEdit && recording
        && _transport->status() == TSE3::Transport::Recording)
    {
        _transport->stop();
        // This will cause us to get a notification
    }
}


void Record::reset()
{
    delete _phraseEdit;
    _phraseEdit = 0;
    _startTime  = -1;
    _endTime    = -1;
    recording   = false;
}


void Record::Transport_Status(TSE3::Transport *, int status)
{
    if (_phraseEdit && status == TSE3::Transport::Resting && recording)
    {
        recording = false;
        _endTime  = _transport->scheduler()->clock();
        _phraseEdit->timeShift(-_startTime);
        _phraseEdit->tidy(_endTime-_startTime);
        if (_phraseEdit->size())
        {
            notify(&RecordListener::Record_RecordingEnded, _song, _track);
        }
        else
        {
            delete _phraseEdit;
            _phraseEdit = 0;
        }
    }
}


void Record::insertPhrase(const std::string &title, bool replacePhrase,
                          bool insertPart, int insertAction,
                          TSE3::Cmd::CommandHistory *history)
{
    TSE3::Phrase *existing = _song->phraseList()->phrase(title);
    if (existing && !replacePhrase)
    {
        throw TSE3::PhraseListError(PhraseNameExistsErr);
    }

    Phrase *phrase = 0;

    if (replacePhrase && existing)
    {
        Cmd::Phrase_Replace *cmd
            = new Cmd::Phrase_Replace(existing, _phraseEdit, _song);
        cmd->execute();
        phrase = cmd->phrase();
        if (history)
        {
            history->add(cmd);
        }
        else
        {
            delete cmd;
        }
    }
    else
    {
        Cmd::Phrase_Create *cmd
            = new Cmd::Phrase_Create(_song->phraseList(), _phraseEdit, title);
        cmd->execute();
        phrase = cmd->phrase();
        if (history)
        {
            history->add(cmd);
        }
        else
        {
            delete cmd;
        }
    }

    if (insertPart && _track)
    {
        TSE3::Part *part = new TSE3::Part();
        part->setStartEnd(_startTime, _endTime);
        Cmd::CommandGroup *group = new Cmd::CommandGroup();
        group->add(new Cmd::Part_Move(insertAction, part, _track));
        group->add(new Cmd::Part_SetPhrase(part, phrase));
        group->execute();
        if (history)
        {
            history->add(group);
        }
        else
        {
            delete group;
        }
    }

    reset();
}

