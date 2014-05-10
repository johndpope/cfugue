/*
 * @(#)app/Application.h 1.00 16 Nov 1999
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

#ifndef TSE3_APP_APPLICATION_H
#define TSE3_APP_APPLICATION_H

#include "tse3/Song.h"

#include <string>
#include <vector>
#include <map>
#include <cstddef>

namespace TSE3
{
    class Metronome;
    class Transport;
    class MidiScheduler;
    class MidiSchedulerFactory;
    class PresetColours;

    namespace Cmd
    {
        class CommandHistory;
    }

    namespace Ins
    {
        class Destination;
    }

    /**
     * The App namespace contains classes that use the @ref TSE3
     * API, and provide a level of application support - including facilities
     * such as choices file saving.
     *
     * @short   @ref TSE3 library application support
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     TSE3
     */
    namespace App
    {
        class ChoicesManager;
        class Record;

        /**
         * This class provides the GUI independant core functionality of a
         * sequencer application based upon the @ref TSE3 library.
         *
         * The functionality provided by this class includes:
         * @li Choices file saved to user's directory
         * @li Management of the lifetimes of the common TSE3 objects
         *
         * This class is used as a singleton.
         *
         * @short   Support class providing core TSE3 application functionality
         * @author  Pete Goodliffe
         * @version 1.00
         * @see     TSE3
         */
        class Application : public TSE3::Listener<TSE3::SongListener>
        {
            public:

                /**
                 * Create the application.
                 *
                 * @param appname     The appname supplied is the name of this
                 *                    application. Any use of the application
                 *                    name should access it from this object in
                 *                    future.
                 * @param appversion  A string containing the application
                 *                    version number.
                 * @param msf         A factory use to generate a
                 *                    @ref MidiScheduler.
                 * @param choicesFile The name of the file that choices are
                 *                    loaded from on startup, and saved to by
                 *                    default. You will conventionally want
                 *                    this to be something like "$HOME/.tse3".
                 *                    If you specify no choicesFile then no
                 *                    default choices loading/saving will be
                 *                    performed.
                 */
                Application(const std::string          &appname,
                            const std::string          &appversion,
                            TSE3::MidiSchedulerFactory *msf,
                            const std::string          &choicesFile = "");

                /**
                 * Any @ref Song objects 'managed' by the application will
                 * be deleted.
                 */
                ~Application();

                /**
                 * Returns the application's name.
                 *
                 * If you need to quote the application's name at some
                 * point in your code, use the Application::name() method,
                 * rather than hardcoding a string constant.
                 */
                const std::string &appName() const { return _appname; }

                /**
                 * Returns the application's version.
                 *
                 * If you need to quote the application's version at some
                 * point in your code, use the Application::name() method,
                 * rather than hardcoding a string constant.
                 */
                const std::string &appVersion() const { return _appversion; }

                /**************************************************************
                 * Access to the main non-Song TSE3 components
                 *************************************************************/

                /**
                 * Returns a pointer to the @ref Metronome object used in this
                 * application.
                 */
                TSE3::Metronome *metronome() const { return _metronome; }

                /**
                 * Returns a pointer to the @ref Transport object used in this
                 * application.
                 */
                TSE3::Transport *transport() const { return _transport; }

                /**
                 * Returns a pointer to the @ref MidiScheduler object used in
                 * this application. This has been created from the
                 * @ref MidiSchedulerFactory object passed to the constructor.
                 */
                TSE3::MidiScheduler *scheduler() const { return _scheduler; }

                /**
                 * Returns a pointer to the @ref ChoicesManager.
                 */
                ChoicesManager *choicesManager() const { return _cm; }

                /**
                 * Returns a pointer to the Application's @ref Record object.
                 * This object is only created when it's needed.
                 */
                Record *record() const;

                /**
                 * Returns the @ref TSE3::Ins::Destintation object used in this
                 * application.
                 */
                TSE3::Ins::Destination *destination() const
                    { return _destination; }

                /**
                 * Returns the @ref TSE3::PresetColours object used in this
                 * application.
                 */
                TSE3::PresetColours *presetColours() const
                    { return _presetColours; }

                /**************************************************************
                 * Configuration of the application behaviour
                 *************************************************************/

                /**
                 * Sets whether choices are saved on destruction or not.
                 *
                 * @see setSaveChoicesOnDestroy
                 */
                bool saveChoicesOnDestroy() const
                    { return _saveChoicesOnDestroy; }

                /**
                 * Returns whether choices are saved on destruction.
                 *
                 * @see saveChoicesOnDestroy
                 */
                void setSaveChoicesOnDestroy(bool s);

                /**************************************************************
                 * Functionality
                 *************************************************************/

                /**
                 * Saves the current choices to the given filename. If no
                 * filename is supplied then the default one is used.
                 */
                void saveChoices(const std::string &filename = "");

                /**
                 * Add a new @ref TSE3::Song to the Application object. If you
                 * don't specify a @ref TSE3::Song, then a new @ref TSE3::Song
                 * will be created.
                 *
                 * You can remove the @ref TSE3::Song by simply deleting it.
                 *
                 * @param  song @ref TSE3::Song to add, or zero to create a
                 *              new one.
                 * @return Pointer to the @ref TSE3::Song that has been added
                 *         (useful if @p song was zero).
                 */
                TSE3::Song *addSong(TSE3::Song *song = 0);

                /**
                 * Returns the number of @ref TSE3::Song objects being
                 * managed by this Application.
                 */
                size_t numSongs() const;

                /**
                 * Returns the @ref CommandHistory object associated with this
                 * @ref Song.
                 *
                 * @param song @ref TSE3::Song to get history object for.
                 */
                TSE3::Cmd::CommandHistory *history(TSE3::Song *song);

                /**
                 * @reimplemented
                 */
                virtual void Notifier_Deleted(TSE3::Song *song);

            protected:

                std::string             _appname;
                std::string             _appversion;
                std::string             _choicesFile;

                TSE3::Metronome        *_metronome;
                TSE3::Transport        *_transport;
                TSE3::MidiScheduler    *_scheduler;
                ChoicesManager         *_cm;
                Record                 *_record;
                TSE3::Ins::Destination *_destination;
                TSE3::PresetColours    *_presetColours;

                bool                    _saveChoicesOnDestroy;

                std::vector<TSE3::Song *>                           songs;
                std::map<TSE3::Song *, TSE3::Cmd::CommandHistory *> histories;
        };
    }
}

#endif
