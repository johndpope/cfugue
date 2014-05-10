/*
 * @(#)app/PartDisplay.h 1.00 18 June 2000
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

#ifndef TSE3_APP_PARTDISPLAY_H
#define TSE3_APP_PARTDISPLAY_H

namespace TSE3
{
    class Part;
    class PresetColours;

    namespace App
    {
        /**
         * The PartDisplay class handles interpretting a @ref TSE3::Part's
         * @ref TSE3::DisplayParams alongside it's child @ref TSE3::Phrase's
         * @ref TSE3::DisplayParams.
         *
         * The @ref TSE3::Part parameters take precidence over the
         * @ref TSE3::Phrase parameters. This class centralises the logic for
         * working out which parameter to use.
         *
         * The PartDisplay class uses the @ref Application's
         * @ref TSE3::PresetColours object if necessary to find a preset
         * colour value.
         *
         * @short   Support class for @ref TSE3::Part display
         * @author  Pete Goodliffe
         * @version 1.00
         * @see     TSE3
         */
        class PartDisplay
        {
            public:

                /**
                 * You specify the @ref Part to display and the
                 * @ref TSE3::PresetColours that applies.
                 *
                 * @param part @ref TSE3::Part to create DisplayParams for
                 * @param pc   @ref TSE3::PresetColours object to use
                 */
                PartDisplay(TSE3::Part *part, TSE3::PresetColours *pc = 0);

                /**
                 * Returns whether to display this part in colour or not.
                 */
                bool useColour();

                /**
                 * Returns which colour to use on redraw.
                 */
                void colour(int &r, int &g, int &b);

            private:

                void calculate();

                TSE3::Part          *p;
                TSE3::PresetColours *preset;
                int                  _calculated;
                bool                 _useColour;
                int                  _r, _g, _b;
        };
    }
}

#endif
