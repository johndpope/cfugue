/*
 * @(#)TSE3.h 3.00 20 Jan 2000
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

#ifndef TSE3_TSE3_H
#define TSE3_TSE3_H

/**
 * @libdoc The TSE3 library
 *
 * TSE3 stands for the Trax Sequencer Engine version 3.00.
 *
 * The library provides facilities to record, edit and playback MIDI musical
 * data. It contains powerful facilities to manipulate the MIDI data in real
 * time. It provides an advanced and acclaimed object-oriented @ref Song
 * structure. It provides support for such facilities as on screen keyboards,
 * on screen VU bars, a 'mixing desk' and so on.
 *
 * TSE3 can be used to create a full sequencer application, or a simple music
 * file playback utility.
 *
 * The main TSE3 API can be found in the @ref TSE3 namespace. This namespace
 * contains a number of sub-namespaces, @ref TSE3::App, @ref TSE3::Cmd,
 * @ref TSE3::Ins, @ref TSE3::Plt and @ref TSE3::Util, which provide extra
 * services over the basic @ref TSE3 API. (There is also an internal
 * implementation namespace @ref TSE3::Impl which you can safely ignore).
 *
 * @li @ref TSE3       - Core TSE3 classes.
 * @li @ref TSE3::App  - Sequencer application support classes.
 * @li @ref TSE3::Cmd  - Command classes to provide hierarchical undo/redo.
 * @li @ref TSE3::Ins  - Classes that cope with definitions for different
 *                       physical MIDI instruments.
 * @li @ref TSE3::Plt  - @ref TSE3::MidiScheduler implementations for different
 *                       target platforms.
 * @li @ref TSE3::Util - Various extra utilities related to the @ref TSE3
 *                       classes.
 */

/**
 * The TSE3 namespace contains the API for the TSE3 library.
 *
 * Related namespaces contain APIs for facilities built on top of TSE3.
 * They are 'sub-namespaces' of the TSE3 namespace:
 *
 * @li @ref TSE3::App  - Sequencer application support classes.
 * @li @ref TSE3::Cmd  - Command classes to provide hierarchical undo/redo.
 * @li @ref TSE3::Ins  - Classes that cope with definitions for different
 *                       physical MIDI instruments.
 * @li @ref TSE3::Plt  - @ref MidiScheduler implementations for different
 *                       target platforms.
 * @li @ref TSE3::Util - Various extra utilities related to the TSE3 classes.
 *
 * If you use this library you must provide a display of the
 * @ref TSE3_Version and @ref TSE3_Copyright strings in your program.
 *
 * You include the TSE3 header files using #include "tse3/HeaderFile.h"
 * where "HeaderFile" is the name of the specific TSE3 file. Note the
 * preceeding "tse3/" part. Headers for the sub-namespaces are included
 * similarly, for example:
 * #include "tse3/app/Application.h"
 *
 * @sect API usage
 *
 * You must decide up-front whether or not you are using the undo/redo command
 * architecture or not. This is because use of direct TSE3 APIs do not
 * necessarily mix well with the undo/redo system. (Consider what would happen
 * if you perform an undoable @ref TSE3::Cmd::Command operation on a
 * @ref TSE3::Part, which you then remove and delete, what will the undo do?)
 * (Answer: "undefined behaviour").
 *
 * In the standard TSE3 API documentation, the related @ref TSE3::Cmd command
 * classes are referenced where appropriate.
 *
 * Most users will not need to provide undo/redo support. You may wish to
 * do so if you are providing a MIDI sequencing application, or suchlike.
 *
 * @short   TSE3 library API namespace
 * @author  Pete Goodliffe
 * @version 3.00
 * @see     TSE3::App
 * @see     TSE3::Cmd
 * @see     TSE3::Ins
 * @see     TSE3::Plt
 * @see     TSE3::Util
 */
namespace TSE3
{
    /**
     * Returns a string containing the TSE3 version number.
     *
     * @return TSE3 version string
     */
    const char *TSE3_Version();

    /**
     * Returns a string containing the TSE3 copyright message.
     *
     * @return TSE3 copyright string
     */
    const char *TSE3_Copyright();
}

#endif
