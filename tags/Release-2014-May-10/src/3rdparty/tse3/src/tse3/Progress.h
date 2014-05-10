/*
 * @(#)Progress.h 3.00 3 April 2000
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

#ifndef TSE3_PROGRESS_H
#define TSE3_PROGRESS_H

namespace TSE3
{
    /**
     * Callback interface for TSE3 classes that provide progress information.
     * This is used, for example, by the file import/export mechanisms (see
     * @ref TSE3MDL, @ref MidiFileImport and @ref MidiFileExport) and the
     * @ref Util::Demidify utility.
     *
     * @short   Demidify progress callback interface
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Demidify
     */
    class Progress
    {
        public:

            /**
             * Called by a progress information generator before the
             * first call to @ref progress().
             *
             * It defines the minimum and maximum values the progress
             * values range between.
             *
             * @param min Mimumum value returned from @ref progess
             *            (i.e. the first value sent)
             * @param max Maximum value returned from @ref progess
             *            (i.e. the last value sent)
             */
            virtual void progressRange(int min, int max) = 0;

            /**
             * Whilst an operation is being performed by a class that uses
             * the Progress interface, this callback will be intermittantly
             * invoked.
             *
             * Before it is called for the first time, the @ref progressRange
             * method will be invoked, to give meaning to the @p value
             * parameter.
             *
             * @param value Progress indication
             */
            virtual void progress(int value) = 0;
    };
}

#endif
