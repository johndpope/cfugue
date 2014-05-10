/*
 * @(#)DisplayParams.h 3.00 24 June 2000
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

#ifndef TSE3_DISPLAYPARAMS_H
#define TSE3_DISPLAYPARAMS_H

#include "tse3/listen/DisplayParams.h"

#include "tse3/Notifier.h"
#include "tse3/Serializable.h"

namespace TSE3
{
    /**
     * The DisplayParams class holds data related to how a TSE3 object should
     * be drawn on the screen by some application that uses the TSE3 library.
     *
     * This is perhaps unusual for a pure 'sequencer engine' library, however
     * it is a useful piece of application support. It also ensures that
     * different applications based on TSE3 will display @ref Song data in
     * a relatively similar manner (as deemed appropriate).
     *
     * Obviously, TSE3 does not use these display values itself (since it
     * does no displaying whatsoever). An application is not required
     * to use the information in this class; however for inter-application
     * constency it is encouraged.
     *
     * The classes that contain a DisplayParams object are:
     * @li @ref Part
     * @li @ref Phrase
     *
     * The @ref Part's DisplayParams are considered to 'override' that of it's
     * @ref Phrase.
     *
     * The DisplayParams object allows an object to be displayed in a number
     * of different states (see the @ref DrawingStyle enum type). For some of
     * these states the DisplayParams object holds extra information (for
     * example a specific colour).
     *
     * @sect Command classes
     *
     * Use the following command classes to manipute this object in a undo/redo
     * environment.
     *
     *     @li @ref TSE3::Cmd::Track_SetInfo
     *     @li @ref TSE3::Cmd::Part_SetInfo
     *     @li @ref TSE3::Cmd::Phrase_SetInfo
     *
     * @short   A collection of application display parameter values
     * @author  Pete Goodliffe
     * @version 3.00
     */
    class DisplayParams : public Notifier<DisplayParamsListener>,
                          public Serializable
    {
        public:

            /**
             * The default DisplayParams has the @ref DrawingStyle None
             * (but the @ref colour() set to white, and @ref presetColour()
             * set to Verse).
             */
            DisplayParams();
            DisplayParams(const DisplayParams &);
            virtual ~DisplayParams();
            DisplayParams &operator=(const DisplayParams &);

            /**
             * Enum type describing how colour should be displayed.
             *
             * The values are:
             * @li @p Default      Do whatever the default for this displayable
             *                     object is. Usually this will be the same as
             *                     None - some default system drawing style will
             *                     be used. Default may allow a colour to be
             *                     used, for example when a @ref Part is set to
             *                     Default but its @ref Phrase is set to
             *                     UseColour.
             * @li @p PresetColour Display the object using the preset colour
             *                     defined by this DisplayParams object (see
             *                     @ref presetColour())
             * @li @p Colour       Display the object using the colour defined
             *                     by this DisplayParams object (see @ref
             *                     colour()).
             * @li @p None         Force no colour to be used, and the object to
             *                     be displayed with some system default drawing
             *                     style.
             */
            enum DrawingStyle
            {
                Default,
                PresetColour,
                Colour,
                None
            };

            /**
             * As enum type describing a number of preset 'global' colours.
             * The user may choose to display a DisplayParams object with
             * a specific colour (see @ref setColour()) or use a global
             * colour from this list.
             *
             * The @ref PresetColours object provides a central definiton
             * of the colour rgb values.
             *
             * The final anum value (@p NoPresetColours) is not a valid value
             * but a descrition of the number of PresetColours available.
             */
            enum PresetColours
            {
                Intro,
                Verse,
                Chorus,
                Bridge,
                Coda,
                Refrain,
                Melody,
                Solo,
                Backing,

                Brass,
                Percussion,
                Drums,
                Guitar,
                Bass,
                Flute,
                Strings,
                Keyboard,
                Piano,
                Saxophone,

                NoPresetColours
            };

            /**
             * Returns a string containing a English representation of the
             * specified @ref PresetColours value.
             *
             * @param  pc @ref PresetColours value to get string for
             * @return String represeting @p pc
             */
            static const char *presetColourString(int pc);

            /**
             * Returns the drawing style of this displayable object.
             *
             * The value is defined by the @ref DrawingStyle enum type.
             *
             * @return Whether to use to r, g, b values
             * @see    setStyle
             * @see    colour
             * @see    DrawingStyle
             */
            int style() { return _style; }

            /**
             * Sets the draing style of this displayable object. The value
             * is defined by the @ref DrawingStyle enum type.
             *
             * @see style
             */
            void setStyle(int s);

            /**
             * Returns the selected preset colour in the reference parameters.
             * The colour is defined by the @ref PresetColours enum type.
             *
             * This describes the colour used when in the PresetColour
             * @ref style().
             *
             * Note that although you can specify both a presetColour and a
             * @ref colour, only one of them (or possibly neither) will be
             * used at any once time, as specified by the @ref style() method.
             *
             * @return Current preset colour
             * @see    setPresetColour
             */
            int presetColour() const { return _preset; }

            /**
             * Sets the preset colour value.
             *
             * @param pc New preset colour
             * @see   presetColour
             */
            void setPresetColour(int pc);

            /**
             * Returns the selected colour in the reference parameters.
             * The colour is returned as RGB values, each between 0-255.
             *
             * This is the colour used when in the Colour @ref style().
             *
             * Note that although you can specify both a @ref presetColour and
             * a colour, only one of them (or possibly neither) will be
             * used at any once time, as specified by the @ref style() method.
             *
             * @param r Integer the blue value is returned in (0-255)
             * @param g Integer the green value is returned in (0-255)
             * @param b Integer the red value is returned in (0-255)
             * @see   setColour
             */
            void colour(int &r, int &g, int &b)
            {
                r = _r;
                g = _g;
                b = _b;
            }

            /**
             * Sets the colour value.
             *
             * @param r New blue value (0-255)
             * @param g New green value (0-255)
             * @param b New red value (0-255)
             * @see   colour
             */
            void setColour(int r, int g, int b);

            /**
             * @reimplemented
             */
            virtual void save(std::ostream &o, int i) const;

            /**
             * @reimplemented
             */
            virtual void load(std::istream &in, SerializableLoadInfo &info);

        private:

            int _style;
            int _r, _g, _b;
            int _preset;
    };

    /**
     * This class holds information about a set of global colours that
     * can be used by a @ref DisplayParams object whose
     * @ref DisplayParams::style() is set to @p PresetColour.
     *
     * @short   A collection of application display parameter values
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     DisplayParams
     */
    class PresetColours : public Notifier<PresetColoursListener>
    {
        public:

            PresetColours();

            /**
             * Returns the RGB values of the specified preset colour in the
             * reference parameters. The colour is returned as RGB values, each
             * between 0-255.
             *
             * @param preset @ref DisplayParams::PresetColours value to
             *               read RGB values for
             *               If you pass an invalid @p preset value, then the
             *               result is undefined.
             * @param r      Integer the blue value is returned in (0-255)
             * @param g      Integer the green value is returned in (0-255)
             * @param b      Integer the red value is returned in (0-255)
             * @see   setColour
             */
            void colour(int preset, int &r, int &g, int &b)
            {
                r = _r[preset];
                g = _g[preset];
                b = _b[preset];
            }

            /**
             * Sets the preset colour's RGB value.
             *
             * @param preset @ref DisplayParams::PresetColours value to
             *               set RGB values for
             *               If you pass an invalid @p preset value, then the
             *               result is undefined.
             * @param r      New blue value (0-255)
             * @param g      New green value (0-255)
             * @param b      New red value (0-255)
             * @see   colour
             */
            void setColour(int preset, int r, int g, int b);

        private:

            int _r[DisplayParams::NoPresetColours];
            int _g[DisplayParams::NoPresetColours];
            int _b[DisplayParams::NoPresetColours];
    };
}

#endif
