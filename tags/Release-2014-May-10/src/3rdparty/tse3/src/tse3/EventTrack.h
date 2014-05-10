/*
 * @(#)EventTrack.h 3.00 24 May 1999
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

#ifndef TSE3_EVENTTRACK_H
#define TSE3_EVENTTRACK_H

#include "tse3/listen/EventTrack.h"

#include "tse3/Notifier.h"
#include "tse3/Midi.h"
#include "tse3/Playable.h"

#include <vector>
#include <algorithm>
#include <functional>
#include <cstddef>

namespace TSE3
{
    /**
     * The EventTrack provides a simple track containing a given type of
     * events.
     *
     * It is used with instantiations of the @ref Event generic class.
     *
     * The EventTrack is used as a base class for other types of TSE3 track,
     * for example the @ref TimeSigTrack and @ref TempoTrack.
     *
     * The contained @ref Event objects are held by value.
     *
     * The EventTrack has two modes of operating, specified as a boolean
     * parameter in the constructor. The normal operation (as used by most
     * subclasses) is to prevent the insertion of two events with the same
     * time. For example, you don't want to have two different time signatures
     * at the same time. The second mode of operation allows this time
     * duplication.
     *
     * @short   A simple track containing @ref Event objects
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Event
     * @see     EventTrackListener
     */
    template <class etype>
    class EventTrack : public Playable,
                       public Notifier<EventTrackListener<etype> >
    {
        public:

            /**
             * Creates an EventTrack.
             *
             * The parameter specifies whether or not duplicate time values
             * are allowed in the EventTrack. When false, you may not insert
             * two events with the same time.
             *
             * @param allowDuplicates Whether to allow duplicate time values
             */
            EventTrack(bool allowDuplicates = false) : dup(allowDuplicates) {}
            ~EventTrack() {}

            typedef Event<etype> event_type;

            /**
             * Returns the number of events in this track.
             *
             * @return The number of events in this track
             */
            size_t size() const { return data.size(); }

            /**
             * Returns the nth event in the track.
             *
             * The value returned for an index that is out of range is
             * undefined. The @ref size method describes the valid
             * values.
             *
             * @param  n Index
             * @return Event at index n
             */
            event_type const &operator[](const size_t n) const
            {
                return data[n];
            }

            /**
             * Returns the index of the event at a given @ref Clock.
             *
             * @param  c       Clock value to search for.
             * @param  roundup If true index returns the event at or after
             *                 the specified @ref Clock. If false, returns
             *                 the event at of before the specified @ref
             *                 Clock.
             * @return Index of event. If past the end of the data then returns
             *         'size'.
             */
            int index(const Clock c, bool roundup = true);

            /**
             * Inserts an @ref Event into the EventTrack, and returns the
             * index of the insertion point.
             *
             * If duplicate times are not allowed (see
             * @ref EventTrack::EventTrack) then the new @ref Event will
             * replace the existing @ref Event at this time.
             *
             * If duplicate times are allowed then the new @ref Event is
             * inserted after any other events with the same time.
             *
             * @param   event The event to insert
             * @returns Index of new event
             * @see     erase
             */
            size_t insert(const event_type &event);

            /**
             * Erase an event.
             *
             * If the event is not in this track then no error is raised.
             *
             * @param event The event to erase
             * @see   insert
             */
            void erase(const event_type &event);

            /**
             * Erase an event.
             *
             * If the index is invalid then no error is raised.
             *
             * @param index Index of the event to erase
             * @see   insert
             */
            void erase(size_t index);

        protected:

            std::vector<event_type> data;

        private:

            EventTrack &operator=(const EventTrack &);
            EventTrack(const EventTrack &);

            bool dup;
    };

    template <class etype>
    int EventTrack<etype>::index(const Clock c, bool roundup)
    {
        typename std::vector< TSE3::Event<etype> >::iterator i = data.begin();
        while (i != data.end() && !(c <= (*i).time))
        {
            ++i;
        }
        if (!roundup
            && i != data.begin()
            && (i == data.end() || (*i).time != c))
        {
            --i;
        }
        return i - data.begin();
    }

    template <class etype>
    size_t EventTrack<etype>::insert(const event_type &event)
    {
        typename std::vector<event_type>::iterator i = data.begin();
        while (i != data.end() && *i <= event) ++i;
        if (!dup && i != data.begin() && (i-1)->time == event.time)
        {
            *(i-1) = event;
            size_t index = i - data.begin();
            Notifier<EventTrackListener<etype> >::notify
                (&EventTrackListener<etype>::EventTrack_EventAltered, index);
            return index;
        }
        else
        {
            size_t index = i - data.begin();
            data.insert(i, event);
            Notifier<EventTrackListener<etype> >::notify
                (&EventTrackListener<etype>::EventTrack_EventInserted, index);
            return index;
        }
    }

    template <class etype>
    void EventTrack<etype>::erase(const event_type &event)
    {
        typename std::vector<event_type>::iterator i =
           std::find_if(data.begin(), data.end(),
                   typename event_type::equal_to(event));
        if (i != data.end())
        {
            size_t index = i - data.begin();
            data.erase(i);
            Notifier<EventTrackListener<etype> >::notify
                (&EventTrackListener<etype>::EventTrack_EventErased, index);
        }
    }

    template <class etype>
    void EventTrack<etype>::erase(size_t index)
    {
        if (index < data.size())
        {
            data.erase(data.begin()+index);
            Notifier<EventTrackListener<etype> >::notify
                (&EventTrackListener<etype>::EventTrack_EventErased, index);
        }
    }
}

#endif
