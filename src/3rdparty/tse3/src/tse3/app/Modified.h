/*
 * @(#)app/Modified.h 1.00 27 October 2000
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

#ifndef TSE3_APP_MODIFIED_H
#define TSE3_APP_MODIFIED_H

#include "tse3/listen/app/Modified.h"

#include "tse3/Notifier.h"
#include "tse3/listen/Song.h"
#include "tse3/listen/Track.h"
#include "tse3/listen/Part.h"
#include "tse3/listen/DisplayParams.h"
#include "tse3/listen/Phrase.h"
#include "tse3/listen/PhraseList.h"
#include "tse3/listen/FlagTrack.h"
#include "tse3/listen/TimeSigTrack.h"
#include "tse3/listen/TempoTrack.h"
#include "tse3/listen/MidiFilter.h"
#include "tse3/listen/MidiParams.h"

#include <cstddef>

namespace TSE3
{
    namespace App
    {
        /**
         * Listens to a @ref TSE3::Song and all its subcomponents, and
         * works out whether it has been 'modified' at all.
         *
         * You can change which @ref TSE3::Song is monitored at any
         * time.
         *
         * @short   @ref TSE3::Song modified status monitor
         * @author  Pete Goodliffe
         * @version 1.00
         * @see     TSE3
         */
        class Modified : public TSE3::Notifier<ModifiedListener>,
                         public TSE3::Listener<TSE3::SongListener>,
                         public TSE3::Listener<TSE3::TrackListener>,
                         public TSE3::Listener<TSE3::PartListener>,
                         public TSE3::Listener<TSE3::PhraseListListener>,
                         public TSE3::Listener<TSE3::MidiParamsListener>,
                         public TSE3::Listener<TSE3::DisplayParamsListener>,
                         public TSE3::Listener<TSE3::FlagTrackListener>,
                         public TSE3::Listener<TSE3::TimeSigTrackListener>,
                         public TSE3::Listener<TSE3::TempoTrackListener>,
                         public TSE3::Listener<TSE3::MidiFilterListener>,
                         public TSE3::Listener<TSE3::PhraseListener>
        {
            public:

                /**
                 * Create a Modified object for the specified @ref TSE3::Song.
                 * If no song is specified then this will return false from
                 * @ref modified until a @ref TSE3::Song is set with
                 * @ref setSong.
                 *
                 * @param song @ref TSE3::Song to monitor at first.
                 */
                Modified(TSE3::Song *song = 0);

                /**
                 * Returns the @ref TSE3::Song currently being monitored,
                 * or zero if no song is being monitored.
                 *
                 * @return @ref TSE3::Song being monitored
                 * @see    setSong
                 */
                TSE3::Song *song() const { return _song; }

                /**
                 * Sets the @ref TSE3::Song currently being monitored. This
                 * will clear the return value of @ref modified if it
                 * is currently true.
                 *
                 * @see song
                 */
                void setSong(TSE3::Song *song);

                /**
                 * Returns whether or not the @ref TSE3::Song has been
                 * modified.
                 *
                 * @return Whether the @ref TSE3::Song has been modified.
                 * @see    setModified
                 */
                bool modified() const { return _modified; }

                /**
                 * Causes the modified status to be set. If necessary, this
                 * will emit a Modified_Changed notification.
                 *
                 * @param m New modified status
                 * @see   modified
                 */
                void setModified(bool modified = true);

                /**
                 * @reimplemented
                 */
                virtual void Song_InfoAltered(Song *);

                /**
                 * @reimplemented
                 */
                virtual void Song_FromAltered(Song *, Clock /*from*/);

                /**
                 * @reimplemented
                 */
                virtual void Song_ToAltered(Song *, Clock /*to*/);

                /**
                 * @reimplemented
                 */
                virtual void Song_TrackInserted(Song *, Track *);

                /**
                 * @reimplemented
                 */
                virtual void Song_TrackRemoved(Song *, Track *, size_t);

                /**
                 * @reimplemented
                 */
                virtual void Track_TitleAltered(Track *);

                /**
                 * @reimplemented
                 */
                virtual void Track_PartInserted(Track *, Part *);

                /**
                 * @reimplemented
                 */
                virtual void Track_PartRemoved(Track *, Part *);

                /**
                 * @reimplemented
                 */
                virtual void Part_StartAltered(Part *, Clock /*start*/);

                /**
                 * @reimplemented
                 */
                virtual void Part_EndAltered(Part *, Clock /*end*/);

                /**
                 * @reimplemented
                 */
                virtual void Part_RepeatAltered(Part *, Clock /*repeat*/);

                /**
                 * @reimplemented
                 */
                virtual void Part_PhraseAltered(Part *, Phrase * /*phrase*/);

                /**
                 * @reimplemented
                 */
                virtual void PhraseList_Inserted(PhraseList *, Phrase * /*p*/);

                /**
                 * @reimplemented
                 */
                virtual void PhraseList_Removed(PhraseList *, Phrase * /*p*/);

                /**
                 * @reimplemented
                 */
                virtual void MidiParams_Altered(MidiParams *, int /*what*/);

                /**
                 * @reimplemented
                 */
                virtual void DisplayParams_Altered(DisplayParams *);

                /**
                 * @reimplemented
                 */
                virtual void EventTrack_EventAltered(EventTrack<Flag> *,
                                                     size_t);

                /**
                 * @reimplemented
                 */
                virtual void EventTrack_EventInserted(EventTrack<Flag> *,
                                                      size_t);

                /**
                 * @reimplemented
                 */
                virtual void EventTrack_EventErased(EventTrack<Flag> *,
                                                    size_t);

                /**
                 * @reimplemented
                 */
                virtual void EventTrack_EventAltered(EventTrack<TimeSig> *);

                /**
                 * @reimplemented
                 */
                virtual void EventTrack_EventInserted(EventTrack<TimeSig> *);

                /**
                 * @reimplemented
                 */
                virtual void EventTrack_EventErased(EventTrack<TimeSig> *);

                /**
                 * @reimplemented
                 */
                virtual void EventTrack_EventAltered(EventTrack<Tempo> *);

                /**
                 * @reimplemented
                 */
                virtual void EventTrack_EventInserted(EventTrack<Tempo> *);

                /**
                 * @reimplemented
                 */
                virtual void EventTrack_EventErased(EventTrack<Tempo> *);

                /**
                 * @reimplemented
                 */
                void MidiFilter_Altered(MidiFilter *, int /*what*/);

            private:

                void attachToTrack(TSE3::Track *track);
                void detachFromTrack(TSE3::Track *track);
                void attachToPart(TSE3::Part *part);
                void detachFromPart(TSE3::Part *part);

                TSE3::Song *_song;
                bool        _modified;
        };
    }
}

#endif
