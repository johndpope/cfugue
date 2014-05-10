/*
 * @(#)cmd/Track.h 3.00 8 July 2000
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

#ifndef TSE3_CMD_TRACK_H
#define TSE3_CMD_TRACK_H

#include "tse3/cmd/Command.h"
#include "tse3/MidiFilter.h"
#include "tse3/MidiParams.h"
#include "tse3/DisplayParams.h"

#include <string>
#include <cstddef>

namespace TSE3
{
    class Track;
    class Part;

    namespace App
    {
        class TrackSelection;
    }

    namespace Cmd
    {
        /**
         * Command to set a @ref TSE3::Track's info (the title, it's
         * @ref TSE3::MidiParams setup and it's @ref TSE3::MidiFilter setup).
         *
         * @short   Set @ref TSE3::Track info Command
         * @author  Pete Goodliffe
         * @version 1.00
         * @see     Command
         */
        class Track_SetInfo : public Command
        {
            public:

                /**
                 * To create this command specify the @ref TSE3::Track object to
                 * alter and the new information.
                 */
                Track_SetInfo(TSE3::Track               *track,
                              const std::string         &title,
                              const TSE3::MidiFilter    &smef,
                              const TSE3::MidiParams    &mp,
                              const TSE3::DisplayParams &dp);

            protected:

                /**
                 * @reimplemented
                 */
                virtual void executeImpl();

                /**
                 * @reimplemented
                 */
                virtual void undoImpl();

            private:

                TSE3::Track         *track;
                std::string          newTitle, oldTitle;
                TSE3::MidiFilter     smef;
                TSE3::MidiParams     mp;
                TSE3::DisplayParams  dp;
        };

        /**
         * Command to snip a @ref TSE3::Part in a @ref TSE3::Track.
         *
         * If there is no @ref TSE3::Part at the specified time, then nothing
         * will happen, and no error will be raised. It is suggested that
         * you check that the command is worthwhile before you perform it.
         *
         * @short   Snip @ref TSE3::Part command
         * @author  Pete Goodliffe
         * @version 1.00
         * @see     Command
         */
        class Track_Snip : public Command
        {
            public:

                /**
                 * Create a Track_Snip command for the given @ref TSE3::Track
                 * at the given time. If there is no @ref TSE3::Part at this
                 * time then the command will do nothing.
                 */
                Track_Snip(TSE3::Track *track, TSE3::Clock snipTime);
                virtual ~Track_Snip();

            protected:

                /**
                 * @reimplemented
                 */
                virtual void executeImpl();

                /**
                 * @reimplemented
                 */
                virtual void undoImpl();

            private:

                TSE3::Track                 *track;
                TSE3::Clock                  snipTime;
                bool                         valid;
                TSE3::Part                  *oldPart, *newPart;
                TSE3::Clock                  oldEndTime;
                bool                         shouldDelete;
        };

        /**
         * Command to glue two @ref TSE3::Part objects in a @ref TSE3::Track.
         *
         * If there is no @ref TSE3::Part at the specified time or it is not
         * directly adjacent to another @ref TSE3::part, then nothing
         * will happen, and no error will be raised. It is suggested that
         * you check that the command is worthwhile before you perform it.
         * You can use the static @ref valid() method to find out whether this
         * command is valid before performing it.
         *
         * @short   Snip @ref TSE3::Part command
         * @author  Pete Goodliffe
         * @version 1.00
         * @see     Command
         */
        class Track_Glue : public Command
        {
            public:

                /**
                 * Create a Track_Glue command for the given @ref TSE3::Track
                 * at the given time.
                 */
                Track_Glue(TSE3::Track *track, TSE3::Clock glueTime);
                virtual ~Track_Glue();

                /**
                 * Returns wether the command for this time is valid.
                 *
                 * To be valid, there must be a @ref TSE3::Part at @p glueTime
                 * and it must directly follow another @ref TSE3::Part object
                 * with no gap inbetween.
                 */
                static bool valid(TSE3::Track *track, TSE3::Clock glueTime);

            protected:

                /**
                 * @reimplemented
                 */
                virtual void executeImpl();

                /**
                 * @reimplemented
                 */
                virtual void undoImpl();

            private:

                TSE3::Track                 *track;
                TSE3::Clock                  glueTime;
                bool                         _valid;
                size_t                       pos;
                TSE3::Part                  *oldPart;
                TSE3::Clock                  oldEndTime;
        };

        /**
         * Command to remove a @ref TSE3::Part from a @ref TSE3::Track.
         *
         * (Too insert a @ref TSE3::Part use @ref Part_Move.)
         *
         * @short   Remove Part Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Track_RemovePart : public Command
        {
            public:

                /**
                 * Remove the specified @ref TSE3::Part from it's parent
                 * @ref TSE3::Track.
                 *
                 * @param part @ref TSE3:Part to remove
                 */
                Track_RemovePart(TSE3::Part *part);

                /**
                 * Remove the @ref TSE3::Part specified by the index
                 * @p part in the @ref TSE3::Track @p track.
                 *
                 * @param track @ref TSE3::Track to remove @ref TSE3::Part from
                 * @param part  Index of @ref TSE3:Part to remove
                 */
                Track_RemovePart(TSE3::Track *track, size_t part);

                /**
                 * Remove the specified @ref TSE3::Part @p part from the
                 * @ref TSE3::Track @p track.
                 *
                 * @param track @ref TSE3::Track to remove @ref TSE3::Part from
                 * @param part  @ref TSE3::Part to remove
                 */
                Track_RemovePart(TSE3::Track *track, Part *part);

                virtual ~Track_RemovePart();

            protected:

                /**
                 * @reimplemented
                 */
                virtual void executeImpl();

                /**
                 * @reimplemented
                 */
                virtual void undoImpl();

            private:

                TSE3::Track *track;
                TSE3::Part  *part;
                int          partno;
        };

        /**
         * Command to sort a;; @ref TSE3::Track objects in a @ref TSE3::Song.
         *
         * @short   Sort Tracks Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Track_Sort : public Command
        {
            public:

                /**
                 * Describes the Track sort criteria.
                 */
                enum SortBy
                {
                    ByName,
                    ByMuted,
                    BySelected,
                    ByPort,
                    ByChannel,
                    BySize
                };

                /**
                 * Describes the Track sort order
                 */
                enum SortOrder
                {
                    Ascending,
                    Descending
                };

                /**
                 * You specify the @ref TSE3::Song to sort, and the
                 * sort parameters.
                 *
                 * @param song      Song to sort Tracks of
                 * @param by        Sort criteria
                 * @param order     Sort order
                 * @param selection Track selection to use for BySelected
                 *                  (zero if not supported)
                 */
                Track_Sort(TSE3::Song                *song,
                           SortBy                     by,
                           SortOrder                  order,
                           TSE3::App::TrackSelection *selection = 0);
                virtual ~Track_Sort();

            protected:

                /**
                 * @reimplemented
                 */
                virtual void executeImpl();

                /**
                 * @reimplemented
                 */
                virtual void undoImpl();

            private:

                class Track_SortImpl *pimpl;
        };
    }
}

#endif
