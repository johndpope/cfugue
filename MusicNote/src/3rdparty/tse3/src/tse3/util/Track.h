/*
 * @(#)util/Track.h 3.00 25 May 1999
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

#ifndef TSE3_UTIL_TRACK_H
#define TSE3_UTIL_TRACK_H

#include <vector>

namespace TSE3
{
    class Part;
    class Track;
    class Clock;

    namespace Util
    {
        /**
         * Removes any @ref TSE3::Part objects that are between the given times.
         * The @ref TSE3::Part objects will be 'intelligently' clipped, i.e.
         * if a @ref TSE3::Part starts in the time zone, but ends after it,
         * it's start will be moved, and an offset calculated for the
         * @ref TSE3::Part's @ref TSE3::MidiFilter.
         *
         * The @ref Part objects will be deleted.
         *
         * If there are no @ref TSE3::Part objects deleted, no error will be
         * raised and the method wil silently return.
         *
         * @param track @ref TSE3::Track to remove @ref TSE3::Parts from
         * @param start Start of time zone to clear
         * @param end   End of time zone to clear
         */
        void Track_RemoveParts(TSE3::Track *track,
                               TSE3::Clock  start,
                               TSE3::Clock  end);

        /**
         * This method acts like the remove above, but has more bells and
         * whistles.
         *
         * The reason for using this method over the simpler remove above
         * is that with the information returned from this call you can
         * reconstruct the @ref TSE3::Track again afterwards, for example
         * to provide an undo facility. The @ref Track_UnremoveParts function
         * exists to do this. You can also safely 'redo' the operation again.
         *
         * Unless you need this ability don't read the following description
         * since it will give you a headache ;-)
         *
         * Rather than delete any @ref TSE3::Part objects in the
         * @ref TSE3::Track, they are removed, and placed into the @p removed
         * vector supplied by the user.
         *
         * Additionally as a @ref TSE3::Part at the start of the zone may be
         * clipped, and a @ref TSE3::Part at the end of the zone may be clipped,
         * the funtion provides a way of reporting this.
         *
         * If cliping does take place, the old @ref TSE3::Part end/start values
         * (respectively) are placed in the @p clippedStart and @p clippedEnd
         * parameters. If no @ref TSE3::Part is clipped at either end, the
         * parameter is set to -1.
         *
         * A second type of clipping may occur - there could be a single big
         * @ref TSE3::Part that starts before start and ends after end.
         * Track_RemoveParts will 'cut a hole' into it. @p clippedEnd is set to
         * -2 and @p clippedStart is set to the original end time of this 'big
         * Part'. A new Part is created to fit after the @p end time. The
         * new Part is returned in @p newPart.
         *
         * If you do an 'undo' (calling @ref Track_UnremoveParts) on this
         * operation, and need to redo it, it can be important that the
         * same @ref TSE3::Part is added at the end - so if you were given a
         * @p newPart value on return of the first call, pass this parameter
         * back in the subsequent call.
         *
         * @param track        @ref TSE3::Track to remove @ref TSE3::Parts from
         * @param start        Start of time zone to clear
         * @param end          End of time zone to clear
         * @param removed      Every @ref TSE3::Part that is removed is put in
         *                     this vector
         * @param clippedStart If there is a @ref TSE3::Part that overlaps with
         *                     @p start and has been clipped, this parameter
         *                     is set to it's old end time.
         *                     If there is no overlap, this parameter is set
         *                     to -1.
         * @param clippedEnd   If there is a @ref TSE3::Part that overlaps with
         *                     @p end and has been clipped, this parameter
         *                     is set to it's old start time.
         *                     If there is no overlap, this parameter is set
         *                     to -1.
         * @param part         Set initially to zero, this may return a
         *                     pointer to a created @ref TSE3::Part. If it does
         *                     you should pass this value back in a subsequent
         *                     'redo'.
         * @see Track_UnremoveParts
         */
        void Track_RemoveParts(TSE3::Track               *track,
                               TSE3::Clock                start,
                               TSE3::Clock                end,
                               std::vector<TSE3::Part*>  &removed,
                               TSE3::Clock               &clippedStart,
                               TSE3::Clock               &clippedEnd,
                               TSE3::Part               *&newPart);

        /**
         * Undoes the removal of any @ref TSE3::Part from the second
         * @ref Track_RemoveParts function above. Pass the same @p start
         * and @p stop times and the @p removed, @p clippedStart and
         * @p clippedEnd parameters you originally passed. As
         * @ref TSE3::Part objects are put back into the @ref TSE3::Track
         * they will be removed from the @p vector.
         *
         * If @p newPart was not zero on return from @ref Track_RemoveParts
         * then it is now <b>your</b> responsibility to delete it.
         *
         * @param track        Same as previously passed to
         *                     @ref Track_RemoveParts
         * @param start        Same as previously passed to
         *                     @ref Track_RemoveParts
         * @param end          Same as previously passed to
         *                     @ref Track_RemoveParts
         * @param removed      Same as previously passed to
         *                     @ref Track_RemoveParts
         *                     Same as previously passed to
         *                     @ref Track_RemoveParts
         * @param clippedStart Same as previously passed to
         *                     @ref Track_RemoveParts
         * @param clippedEnd   Same as previously passed to
         *                     @ref Track_RemoveParts
         * @see Track_RemoveParts
         */
        void Track_UnremoveParts(TSE3::Track              *track,
                                 TSE3::Clock               start,
                                 TSE3::Clock               end,
                                 std::vector<TSE3::Part*> &removed,
                                 TSE3::Clock               clippedStart,
                                 TSE3::Clock               clippedEnd);

    }
}

#endif
