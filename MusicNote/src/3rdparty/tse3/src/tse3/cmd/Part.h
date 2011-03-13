/*
 * @(#)cmd/Part.h 3.00 9 July 2000
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

#ifndef TSE3_CMD_PART_H
#define TSE3_CMD_PART_H

#include "tse3/cmd/Command.h"
#include "tse3/util/Track.h"
#include "tse3/MidiFilter.h"
#include "tse3/MidiParams.h"
#include "tse3/DisplayParams.h"

namespace TSE3
{
    class Part;
    class Phrase;

    namespace Cmd
    {
        /**
         * Command to set a @ref TSE3::Part's info (the @ref TSE3::Phrase,
         * it's @ref TSE3::MidiParams setup, the @ref TSE3::DisplayParams and
         * it's @ref TSE3::MidiFilter setup).
         *
         * If you just want to set the @ref TSE3::Phrase, use
         * @ref Part_SetPhrase.
         *
         * @short   Set @ref TSE3::Part info Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Part_SetInfo : public Command
        {
            public:

                /**
                 * To create this command specify the @ref Part object to alter
                 * and the new information.
                 */
                Part_SetInfo(TSE3::Part                *part,
                             TSE3::Phrase              *phrase,
                             TSE3::Clock                repeat,
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

                TSE3::Part          *part;
                TSE3::Phrase        *newPhrase, *oldPhrase;
                TSE3::Clock          newRepeat, oldRepeat;
                TSE3::MidiFilter     smef;
                TSE3::MidiParams     mp;
                TSE3::DisplayParams  dp;
        };

        /**
         * Command to set a @ref TSE3::Part's @ref TSE3::Phrase.
         *
         * @short   Set @ref TSE3::Part @ref TSE3::Phrase Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Part_SetPhrase : public Command
        {
            public:

                /**
                 * To create this command specify the @ref Part object to alter
                 * and the new @ref TSe3::Phrase.
                 */
                Part_SetPhrase(TSE3::Part *part, TSE3::Phrase *phrase);

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

                TSE3::Part          *part;
                TSE3::Phrase        *newPhrase, *oldPhrase;
        };

        /**
         * Command to move a @ref TSE3::Part's from it's current position
         * (which may be in a @ref TSE3::Track or not - in which case the
         * @ref TSE3::Part is being 'inserted') to another position in a
         * (possibly different) @ref TSE3::Track.
         *
         * @short   Move @ref TSE3::Part Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Part_Move : public Command
        {
            public:

                /**
                 * This enum type describes how a @ref TSE3::Part should be
                 * inserted into a @ref TSE3::Track by the Part_Move
                 * @ref Command. The items are:
                 *
                 * @li NoOverlap The new @ref TSE3::Part is not allowed to
                 *               overlap any existing Parts in the
                 *               @ref TSE3::Track. If inserting it will cause
                 *               an overlap, a @ref TSE3::TrackError exception
                 *               will be thrown by the @ref TSE3::Track.
                 * @li Replace   The new @ref TSE3::Part will replace any
                 *               @ref TSE3::Part objects that it overlaps. This
                 *               may result in some existing @ref TSE3::Part
                 *               objects being deleted.
                 * @li Under     If there are any overlaps, the inserted
                 *               @ref TSE3::Part will be placed 'underneath'
                 *               the other @ref TSE3::Part objects (i.e. it
                 *               will have holes cut in it for each existing
                 *               @ref TSE3::Part position). This may result in
                 *               more than one @ref TSE3::Part being inserted
                 *               in the Track.
                 */
                enum PartInsertAction
                {
                    NoOverlap,
                    Replace,
                    Under
                };

                /**
                 * To create this command specify the @ref Part object to alter
                 * and the new information.
                 *
                 * @param action   @ref PartInsertAction to perform Command with
                 * @param part     @ref TSE3::Part to move/insert
                 * @param track    @ref TSE3::Track to insert into
                 * @param newStart New @ref TSE3::Part start time
                 *                 (leave as default to maintain original value)
                 * @param newEnd   New @ref TSE3::Part end time
                 *                 (leave as default to maintain original Part
                 *                 length)
                 */
                Part_Move(int          action,
                          TSE3::Part  *part,
                          TSE3::Track *track,
                          TSE3::Clock  newStart = -1,
                          TSE3::Clock  newEnd   = -1);
                virtual ~Part_Move();

            protected:

                /**
                 * @reimplemented
                 * May throw a @ref TSE3::PartError if insert fails.
                 */
                virtual void executeImpl();

                /**
                 * @reimplemented
                 */
                virtual void undoImpl();

            private:

                /**
                 * Returns a title for this command worked out from the
                 * ctor parameters specified.
                 *
                 * @param hasParent   Set to the value of (part->parent() != 0)
                 * @param relativeEnd Set to the value of (end == -1)
                 */
                static const char *prvTitle(bool hasParent, bool relativeEnd,
                                            bool sameTrack);

                void removeAndSetPart();
                void unsetAndReinsertPart();
                TSE3::Part               *part;
                TSE3::Track              *newTrack, *oldTrack;
                TSE3::Clock               newStart, oldStart;
                TSE3::Clock               newEnd,   oldEnd;
                int                       action;
                std::vector<TSE3::Part*>  removed;
                TSE3::Clock               clippedStart;
                TSE3::Clock               clippedEnd;
                TSE3::Part               *newPart;
                bool                      valid;
        };
    }
}

#endif
