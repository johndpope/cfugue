/*
 * @(#)listen/Track.h 3.00 17 May 1999
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

#ifndef TSE3_LISTEN_TRACK_H
#define TSE3_LISTEN_TRACK_H

namespace TSE3
{
    class Track;
    class Part;

    /**
     * @ref Track listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class TrackListener
    {
        public:
            typedef Track notifier_type;

            virtual void Track_TitleAltered(Track *)                  {}
            virtual void Track_PartInserted(Track *, Part * /*part*/) {}
            virtual void Track_PartRemoved(Track *, Part * /*part*/)  {}
            virtual void Track_Reparented(Track *)                    {}

            virtual void Track_DisplayParamsAltered(Track *)          {}
    };
}

#endif
