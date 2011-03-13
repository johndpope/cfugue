/*
 * @(#)Modified.cpp 3.00 27 October 2000
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

#include "tse3/app/Modified.h"

#include "tse3/Mutex.h"
#include "tse3/Song.h"
#include "tse3/Track.h"
#include "tse3/Part.h"
#include "tse3/DisplayParams.h"
#include "tse3/Phrase.h"
#include "tse3/PhraseList.h"
#include "tse3/FlagTrack.h"
#include "tse3/TimeSigTrack.h"
#include "tse3/TempoTrack.h"
#include "tse3/MidiFilter.h"
#include "tse3/MidiParams.h"

using namespace TSE3;
using namespace TSE3::App;

Modified::Modified(TSE3::Song *s)
:_song(0), _modified(false)
{
    setSong(s);
}


void Modified::setModified(bool m)
{
    if (_modified != m)
    {
        _modified = m;
        notify(&ModifiedListener::Modified_Changed);
    }
}


void Modified::setSong(TSE3::Song *s)
{
    Impl::CritSec cs;

    // 1. Detach from the current song
    if (_song)
    {
        Listener<SongListener>::detachFrom(_song);
        Listener<FlagTrackListener>::detachFrom(_song->flagTrack());
        Listener<TimeSigTrackListener>::detachFrom(_song->timeSigTrack());
        Listener<TempoTrackListener>::detachFrom(_song->tempoTrack());
        Listener<PhraseListListener>::detachFrom(_song->phraseList());
        for (size_t trk = 0; trk < _song->size(); trk++)
        {
            detachFromTrack((*_song)[trk]);
        }
        for (size_t phr = 0; phr < _song->phraseList()->size(); phr++)
        {
            Listener<PhraseListener>::detachFrom((*(_song->phraseList()))[phr]);
            Listener<DisplayParamsListener>::detachFrom
                ((*(_song->phraseList()))[phr]->displayParams());
        }
    }

    // 2. Attach to new song

    if (s)
    {
        Listener<SongListener>::attachTo(s);
        Listener<FlagTrackListener>::attachTo(s->flagTrack());
        Listener<TimeSigTrackListener>::attachTo(s->timeSigTrack());
        Listener<TempoTrackListener>::attachTo(s->tempoTrack());
        Listener<PhraseListListener>::attachTo(s->phraseList());
        for (size_t trk = 0; trk < s->size(); trk++)
        {
            attachToTrack((*s)[trk]);
        }
        for (size_t phr = 0; phr < s->phraseList()->size(); phr++)
        {
            Listener<PhraseListener>::attachTo((*(s->phraseList()))[phr]);
            Listener<DisplayParamsListener>::attachTo
                ((*(s->phraseList()))[phr]->displayParams());
        }
    }

    // 3. Set new values

    _song = s;
    setModified(false);
}


void Modified::attachToTrack(TSE3::Track *track)
{
    Impl::CritSec cs;
    Listener<TrackListener>::attachTo(track);
    Listener<MidiParamsListener>::attachTo(track->params());
    Listener<MidiFilterListener>::attachTo(track->filter());
    for (size_t prt = 0; prt < track->size(); prt++)
    {
        attachToPart((*track)[prt]);
    }
}


void Modified::detachFromTrack(TSE3::Track *track)
{
    Impl::CritSec cs;
    for (size_t prt = 0; prt < track->size(); prt++)
    {
        detachFromPart((*track)[prt]);
    }
    Listener<TrackListener>::detachFrom(track);
    Listener<MidiParamsListener>::detachFrom(track->params());
    Listener<MidiFilterListener>::detachFrom(track->filter());
}


void Modified::attachToPart(TSE3::Part *part)
{
    Impl::CritSec cs;
    Listener<PartListener>::attachTo(part);
    Listener<MidiParamsListener>::attachTo(part->params());
    Listener<MidiFilterListener>::attachTo(part->filter());
    Listener<DisplayParamsListener>::attachTo(part->displayParams());
}


void Modified::detachFromPart(TSE3::Part *part)
{
    Impl::CritSec cs;
    Listener<MidiParamsListener>::detachFrom(part->params());
    Listener<MidiFilterListener>::detachFrom(part->filter());
    Listener<DisplayParamsListener>::detachFrom(part->displayParams());
    Listener<PartListener>::detachFrom(part);
}


void Modified::Song_InfoAltered(Song *)
{
    setModified();
}


void Modified::Song_FromAltered(Song *, Clock /*from*/)
{
    setModified();
}


void Modified::Song_ToAltered(Song *, Clock /*to*/)
{
    setModified();
}


void Modified::Song_TrackInserted(Song *, Track *track)
{
    setModified();
    attachToTrack(track);
}


void Modified::Song_TrackRemoved(Song *, Track *track, size_t)
{
    setModified();
    detachFromTrack(track);
}


void Modified::Track_TitleAltered(Track *)
{
    setModified();
}


void Modified::Track_PartInserted(Track *, Part *part)
{
    setModified();
    attachToPart(part);
}


void Modified::Track_PartRemoved(Track *, Part *part)
{
    setModified();
    detachFromPart(part);
}


void Modified::Part_StartAltered(Part *, Clock /*start*/)
{
    setModified();
}


void Modified::Part_EndAltered(Part *, Clock /*end*/)
{
    setModified();
}


void Modified::Part_RepeatAltered(Part *, Clock /*repeat*/)
{
    setModified();
}


void Modified::Part_PhraseAltered(Part *, Phrase * /*phrase*/)
{
    setModified();
}


void Modified::PhraseList_Inserted(PhraseList *, Phrase *phrase)
{
    setModified();
    Listener<PhraseListener>::attachTo(phrase);
}


void Modified::PhraseList_Removed(PhraseList *, Phrase *phrase)
{
    setModified();
    Listener<PhraseListener>::detachFrom(phrase);
}


void Modified::MidiParams_Altered(MidiParams *, int /*what*/)
{
    setModified();
}


void Modified::DisplayParams_Altered(DisplayParams *)
{
    setModified();
}


void Modified::EventTrack_EventAltered(EventTrack<Flag> *, size_t)
{
    setModified();
}


void Modified::EventTrack_EventInserted(EventTrack<Flag> *, size_t)
{
    setModified();
}


void Modified::EventTrack_EventErased(EventTrack<Flag> *, size_t)
{
    setModified();
}


void Modified::EventTrack_EventAltered(EventTrack<TimeSig> *)
{
    setModified();
}


void Modified::EventTrack_EventInserted(EventTrack<TimeSig> *)
{
    setModified();
}


void Modified::EventTrack_EventErased(EventTrack<TimeSig> *)
{
    setModified();
}


void Modified::EventTrack_EventAltered(EventTrack<Tempo> *)
{
    setModified();
}


void Modified::EventTrack_EventInserted(EventTrack<Tempo> *)
{
    setModified();
}


void Modified::EventTrack_EventErased(EventTrack<Tempo> *)
{
    setModified();
}


void Modified::MidiFilter_Altered(MidiFilter *, int /*what*/)
{
    setModified();
}



