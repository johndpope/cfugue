/*
 * @(#)listen/DisplayParams.h 3.00 24 June 2000
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

#ifndef TSE3_LISTEN_DISPLAYPARAMS_H
#define TSE3_LISTEN_DISPLAYPARAMS_H

namespace TSE3
{
    class DisplayParams;
    class PresetColours;

    /**
     * @ref DisplayParams listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class DisplayParamsListener
    {
        public:
            typedef DisplayParams notifier_type;

            virtual void DisplayParams_Altered(DisplayParams *) {}
    };

    /**
     * @ref PresetColours listener interface.
     *
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    class PresetColoursListener
    {
        public:
            typedef PresetColours notifier_type;

            virtual void PresetColours_Altered(PresetColours *, int /*colour*/)
                {}
    };
}

#endif
