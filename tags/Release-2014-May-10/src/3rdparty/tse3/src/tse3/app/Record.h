/*
 * @(#)app/Record.h 1.00 18 June 2000
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

#ifndef TSE3_APPLICATION_RECORD_H
#define TSE3_APPLICATION_RECORD_H

#include "tse3/listen/app/Record.h"

#include "tse3/Notifier.h"
#include "tse3/Midi.h"
#include "tse3/listen/Transport.h"

#include <string>

namespace TSE3
{
    class PhraseEdit;
    class Track;
    class Song;
    class Transport;
    class MidiScheduler;

    namespace Cmd
    {
        class CommandHistory;
    }

    namespace App
    {
        /**
         * This class provides help using the @ref Transport object's record
         * facilities. When using the @ref Transport record API, you need
         * to create a @ref PhraseEdit and keep track of when you started
         * and stopped recording.
         *
         * After recording you will need to supply a name for the new @ref
         * Phrase and then have it inserted into the @ref Song.
         *
         * The job of this helper class is to aid this process. The lifetime
         * of the objects involved are managed, and the actions needed are
         * encapsulated in these methods.
         *
         * It is assumed that neither the @ref Transport object or the
         * @ref MidiScheduler object will be deleted from under the Recording
         * object's feet.
         *
         * @short   Support class for recording
         * @author  Pete Goodliffe
         * @version 1.00
         * @see     TSE3
         */
        class Record : public TSE3::Listener<TransportListener>,
                       public TSE3::Notifier<RecordListener>
        {
            public:

                /**
                 * Creates a new recording helper object.
                 *
                 * The initial values of @ref start() and @ref end() default,
                 * and there is no @ref PhraseEdit created (see @ref
                 * phraseEdit()).
                 */
                Record(TSE3::Transport *transport);

                /**
                 * Any allocated object will be deallocated.
                 */
                ~Record();

                /**
                 * Start recording at the current @ref MidiScheduler time.
                 * This will create a new @ref PhraseEdit object.
                 *
                 * You must pass a @ref Song and @ref Track parameter. These
                 * are used to pass a @ref Playable and
                 * @ref SimpleMidiEventFilter
                 * parameter to the @ref TSE3::Transport::record() method.
                 *
                 * It is also used later on to insert a @ref Part into the
                 * @ref Track.
                 *
                 * If you call start a second time, whilst recording is taking
                 * place, then it is equivalent to calling @ref stop().
                 *
                 * You don't have to call @ref Record::stop(), you can just
                 * use @ref TSE3::Transport::stop(). However, read the
                 * documentation for this object's @ref stop method for
                 * information on the callback generated.
                 *
                 * @see stop
                 */
                void start(TSE3::Song *p, TSE3::Track *t);

                /**
                 * Stops recording at the current @ref MidiScheduler time.
                 * Sends a notification to any @ref TSE3::Listener objects.
                 *
                 * You do not have to call this to stop recording: any call to
                 * @ref TSE3::Transport::stop() will cause the Record object
                 * to notice, and notify any listeners.
                 *
                 * @sect Notification
                 *
                 * The @ref RecordingListener::Record_RecordingEnded
                 * notification is only sent if there was something recorded
                 * into the @ref PhraseEdit object.
                 *
                 * If you handle the
                 * @ref RecordingListener::Record_RecordingEnded callback
                 * you must take care to check the @ref TSE3::Song and
                 * @ref TSE3::Track parameters. If there are two or more
                 * objects that use the Record utility, they will both receive
                 * the notification and will have to check whether it was a
                 * recording that they started or not.
                 *
                 * @ref start
                 */
                void stop();

                /**
                 * Returns the time recording was last started at, or -1 if
                 * recording has never been started.
                 */
                TSE3::Clock startTime() const { return _startTime; }

                /**
                 * Returns the time recording was last ended at, or -1 if
                 * recording has never been ended.
                 */
                TSE3::Clock endTime() const { return _endTime; }

                /**
                 * Returns a pointer to the current @ref PhraseEdit object, or
                 * zero if no @ref PhraseEdit has been created.
                 */
                TSE3::PhraseEdit *phraseEdit() const { return _phraseEdit; }

                /**
                 * Inserts the recorded @ref Phrase into the @ref Song.
                 * You specify a name for the new @ref Phrase.
                 *
                 * This method may propagate the @ref PhraseList exception if
                 * the @ref Phrase name already exists.
                 *
                 * After the action has been carried out, the internally
                 * allocated @ref PhraseEdit object is deallocated.
                 *
                 * This operation will be performed using @ref TSE3::Command
                 * objects, and the object will be placed on the
                 * @ref TSE3::Cmd::CommandHistory object specified.
                 *
                 * @param title         New @ref Phrase title (must be unique)
                 * @param replacePhrase If the Phrase name already exists,
                 *                      setting this to true will replace it.
                 * @param insertAction  A @ref
                 *                      TSE3::Cmd::Part_Move::PartInsertAction
                 *                      value.
                 * @param history       @ref TSE3::Cmd::CommandHistory object
                 *                      to place undoable commands onto.
                 *                      Specify zero will not store any
                 *                      commands.
                 */
                void insertPhrase(const std::string         &title,
                                  bool                       replacePhrase,
                                  bool                       insertPart,
                                  int                        insertAction,
                                  TSE3::Cmd::CommandHistory *history = 0);

                /**
                 * Ensures that all objects are deallocated, in case the
                 * recording has ended but the user doesn't want to save the
                 * recording into the @ref Song.
                 *
                 * It's not strictly necessary to call this if you are not
                 * going to call @ref insertPhrase(). Without calling this
                 * the next call to @ref start() will handle object reallocation
                 * successfully. However, by calling this you will free up
                 * resources that aren't currently needed.
                 */
                void reset();

                /**
                 * @reimplemented
                 */
                void Transport_Status(TSE3::Transport *, int status);

            protected:

                TSE3::Transport     *_transport;

                TSE3::PhraseEdit    *_phraseEdit;
                TSE3::Song          *_song;
                TSE3::Track         *_track;

                TSE3::Clock          _startTime;
                TSE3::Clock          _endTime;
                bool                 recording;
        };
    }
}

#endif
