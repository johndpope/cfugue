/*
 * @(#)app/Choices.h 1.00 16 Nov 1999
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

#ifndef TSE3_APP_CHOICES_H
#define TSE3_APP_CHOICES_H

#include "tse3/Notifier.h"
#include "tse3/Serializable.h"
#include "tse3/FileBlockParser.h"

#include <string>
#include <list>

namespace TSE3
{
    class Metronome;
    class Transport;
    class Panic;
    class MidiMapper;
    class MidiScheduler;

    namespace Ins
    {
        class Destination;
    }

    namespace App
    {
        class Application;

        /**
         * ChoiceHandlers are used by the @ref ChoicesManger object. You access
         * the choices save/load mechanism through that object.
         *
         * This is a base class for objects that can save a particular block of
         * choices. ChoiceHandlers are registered with the @ref ChoicesManager
         * which delegates responsibility for saving/loading to each
         * ChoiceHandler.
         *
         * ChoiceHandlers implement the @ref Serializable interface as the
         * mechansim with which to perform saving, however they differ from the
         * normal @ref TSE3::Song hierarchy save method since the object whose
         * state is being saved is not the @ref TSE3::Serializable object - it
         * is a ChoiceHandler based on it that does the saving.
         *
         * A number of stock implementations of ChoiceHandler are provided to
         * save the common @ref TSE3 objects that are not directly related to a
         * @ref Song. However, an application will want to add many more
         * ChoiceHandlers to save it's internal settings (e.g. window positions,
         * feature statuses end so on).
         *
         * @short   Choice save/load base class
         * @author  Pete Goodliffe
         * @version 1.00
         * @see     ChoicesManager
         */
        class ChoiceHandler : public TSE3::Serializable
        {
            public:

                /**
                 * Create a ChoiceHandler with the given name.
                 */
                ChoiceHandler(const std::string &choicename);
                virtual ~ChoiceHandler();

                /**
                 * Returns the name of this ChoiceHandler. This is a unique
                 * text string that will identify a block of data in the
                 * choices file as having been saved by this ChoiceHandler.
                 */
                const std::string &name() const { return _name; }

            protected:

                std::string _name;
        };

        /**
         * This class provides a mechanism for saving 'choices'. These are
         * the configuration aspects of any objects in the @ref TSE3 sequencer
         * application.
         *
         * It employs the singleton design pattern.
         *
         * The format of a choices file is the blocked @ref TSE3MDL format.
         *
         * @short   Choices file saving management
         * @author  Pete Goodliffe
         * @version 1.00
         * @see     TSE3
         */
        class ChoicesManager// : public Listener
        {
            public:

                ChoicesManager();
                ~ChoicesManager();

                /**
                 * Add a new @ref ChoiceHandler to the manager. This ties the
                 * object's lifecycle to the ChoicesManager; if it is not
                 * removed then the destruction of this object will bring about
                 * the destruction of the added ChoiceHandler.
                 */
                void add(ChoiceHandler *ch) { handler.add(ch); }

                /**
                 * Remove the @ref ChoiceHandler from the manager. The
                 * @ref ChoiceHandler's life is not tied to the manager's any
                 * more.
                 */
                void remove(ChoiceHandler *ch) { handler.remove(ch); }

                /**
                 * This method steps through each @ref ChoiceHandler registered
                 * and requests that it saves it's choices in the appropriate
                 * file format.
                 */
                void save(const std::string &filename);

                /**
                 * This method reads and interprets the given choices file.
                 * It reads each block and then intructs the appropriate
                 * @ref ChoiceHandler to perform the necessary settings based
                 * on it.
                 *
                 * If the file doesn't exist then no error is thrown.
                 */
                void load(const std::string &filename);

            private:

                /**
                 * This internal class does the real work of the
                 * @ref ChoicesHandler saving. We do this in a nested class
                 * since the agent performing the top level save needs to be a
                 * @ref ChoiceHandler, but it is unnecssarily messy for the
                 * ChoiesManager to be a @ref ChoiceHandler (this would give it
                 * member functions that could be called incorrectly).
                 */
                class ChoicesChoiceHandler : public ChoiceHandler
                {
                    public:
                        ChoicesChoiceHandler();
                        ~ChoicesChoiceHandler();
                        void add(ChoiceHandler *ch);
                        void remove(ChoiceHandler *ch);
                        /**
                         * @reimplemented
                         */
                        virtual void save(std::ostream &out, int indent) const;
                        /**
                         * @reimplemented
                         */
                        virtual void load(std::istream &in,
                                          TSE3::SerializableLoadInfo &info);
                    private:
                        std::list<ChoiceHandler *> handlers;
                };
                ChoicesChoiceHandler handler;

        };

        /**********************************************************************
         * ChoiceHandlers
         **********************************************************************/

        /**
         * ChoiceHandler for the @ref TSE3::App::Application class.
         *
         * @short   @ref TSE3::App::Application ChoiceHandler
         * @author  Pete Goodliffe
         * @version 3.0
         */
        class ApplicationChoiceHandler : public ChoiceHandler
        {
            public:
                ApplicationChoiceHandler(Application *a);
                ~ApplicationChoiceHandler();
                /**
                 * @reimplemented
                 */
                virtual void save(std::ostream &out, int indent) const;
                /**
                 * @reimplemented
                 */
                virtual void load(std::istream &in,
                                  TSE3::SerializableLoadInfo &info);
            private:
                Application *a;
        };

        /**
         * ChoiceHandler for the @ref TSE3::Metronome class.
         *
         * @short   @ref TSE3::Metronome ChoiceHandler
         * @author  Pete Goodliffe
         * @version 3.0
         */
        class MetronomeChoiceHandler : public ChoiceHandler
        {
            public:
                MetronomeChoiceHandler(TSE3::Metronome *m);
                ~MetronomeChoiceHandler();
                /**
                 * @reimplemented
                 */
                virtual void save(std::ostream &out, int indent) const;
                /**
                 * @reimplemented
                 */
                virtual void load(std::istream &in,
                                  TSE3::SerializableLoadInfo &info);
            private:
                TSE3::Metronome *m;
        };

        /**
         * ChoiceHandler for the @ref TSE3::Panic class.
         *
         * @short   @ref TSE3::Panic ChoiceHandler
         * @author  Pete Goodliffe
         * @version 3.0
         */
        class PanicChoiceHandler : public ChoiceHandler
        {
            public:
                PanicChoiceHandler(TSE3::Panic *p);
                ~PanicChoiceHandler();
                /**
                 * @reimplemented
                 */
                virtual void save(std::ostream &out, int indent) const;
                /**
                 * @reimplemented
                 */
                virtual void load(std::istream &in,
                                  TSE3::SerializableLoadInfo &info);
            private:
                TSE3::Panic *p;
        };

        /**
         * ChoiceHandler for the @ref TSE3::MidiMapper class.
         *
         * @short   @ref TSE3::MidiMapper ChoiceHandler
         * @author  Pete Goodliffe
         * @version 3.0
         */
        class MidiMapperChoiceHandler : public ChoiceHandler
        {
            public:
                MidiMapperChoiceHandler(TSE3::MidiMapper *m);
                ~MidiMapperChoiceHandler();
                /**
                 * @reimplemented
                 */
                virtual void save(std::ostream &out, int indent) const;
                /**
                 * @reimplemented
                 */
                virtual void load(std::istream &in,
                                  TSE3::SerializableLoadInfo &info);
            private:
                TSE3::MidiMapper *m;
        };

        /**
         * ChoiceHandler for the @ref TSE3::Transport class.
         *
         * @short   @ref TSE3::Transport ChoiceHandler
         * @author  Pete Goodliffe
         * @version 3.0
         */
        class TransportChoiceHandler : public ChoiceHandler
        {
            public:
                TransportChoiceHandler(TSE3::Transport *t);
                ~TransportChoiceHandler();
                /**
                 * @reimplemented
                 */
                virtual void save(std::ostream &out, int indent) const;
                /**
                 * @reimplemented
                 */
                virtual void load(std::istream &in,
                                  TSE3::SerializableLoadInfo &info);
            private:
                TSE3::Transport         *t;
                PanicChoiceHandler       startPanicHandler;
                PanicChoiceHandler       endPanicHandler;
                MidiMapperChoiceHandler  mapperHandler;
        };

        /**
         * ChoiceHandler for the @ref TSE3::Ins::Destination class.
         *
         * @short   @ref TSE3::Ins::Destination ChoiceHandler
         * @author  Pete Goodliffe
         * @version 3.0
         */
        class DestinationChoiceHandler : public ChoiceHandler
        {
            public:
                DestinationChoiceHandler(TSE3::Ins::Destination *d,
                                         TSE3::MidiScheduler    *ms);
                ~DestinationChoiceHandler();
                /**
                 * @reimplemented
                 */
                virtual void save(std::ostream &out, int indent) const;
                /**
                 * @reimplemented
                 */
                virtual void load(std::istream &in,
                                  TSE3::SerializableLoadInfo &info);
            private:
                TSE3::Ins::Destination *d;
                TSE3::MidiScheduler    *ms;
        };
    }
}

#endif
