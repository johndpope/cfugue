/*
 * @(#)cmd/Song.h 3.00 10 June 1999
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

#ifndef TSE3_CMD_SONG_H
#define TSE3_CMD_SONG_H

#include "tse3/cmd/Command.h"
#include "tse3/Song.h"

#include <string>
#include <vector>
#include <cstddef>

namespace TSE3
{
    class Part;
    class Phrase;

    namespace Cmd
    {
        /**
         * Command to set a @ref Song's info (the title, author, copyright
         * and date strings).
         *
         * @short   Set Song info strings Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Song_SetInfo : public Command
        {
            public:

                /**
                 * To create this command specify the @ref Song object to alter
                 * and the new information.
                 */
                Song_SetInfo(TSE3::Song        *s,
                             const std::string &title,
                             const std::string &author,
                             const std::string &copyright,
                             const std::string &date);

            protected:

                /**
                 * @reimplemented
                 */
                virtual void executeImpl();

                /**
                 * @reimplemented
                 */
                virtual void undoImpl();

            private:

                TSE3::Song  *song;
                std::string  newTitle,     oldTitle;
                std::string  newAuthor,    oldAuthor;
                std::string  newCopyright, oldCopyright;
                std::string  newDate,      oldDate;
        };

        /**
         * Command to set a @ref Song's title.
         *
         * @short   Set Song title Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Song_SetTitle
        : public VariableSetCommand<TSE3::Song, std::string,
                                    const std::string &,
                                    &TSE3::Song::title, &TSE3::Song::setTitle>
        {
            public:
                /**
                 * To create this command specify the @ref Song object to alter
                 * and the new title.
                 */
                Song_SetTitle(TSE3::Song *s, const std::string &str)
                : VariableSetCommand<TSE3::Song, std::string,
                                     const std::string &,
                                     &TSE3::Song::title, &TSE3::Song::setTitle>
                                         (s, str, "song title") {}
        };

        /**
         * Command to set a @ref Song's author.
         *
         * @short   Set Song author Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Song_SetAuthor
        : public VariableSetCommand<TSE3::Song, std::string,
                                    const std::string &,
                                    &TSE3::Song::author, &TSE3::Song::setAuthor>
        {
            public:
                /**
                 * To create this command specify the @ref Song object to alter
                 * and the new author.
                 */
                Song_SetAuthor(TSE3::Song *s, const std::string &str)
                : VariableSetCommand<TSE3::Song, std::string,
                                     const std::string &,
                                    &TSE3::Song::author, &TSE3::Song::setAuthor>
                                         (s, str, "author") {}
        };

        /**
         * Command to set a @ref Song's copyright.
         *
         * @short   Set Song copyright Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Song_SetCopyright
        : public VariableSetCommand<TSE3::Song, std::string,
                                    const std::string &,
                                    &TSE3::Song::copyright,
                                    &TSE3::Song::setCopyright>
        {
            public:
                /**
                 * To create this command specify the @ref Song object to alter
                 * and the new copyright message.
                 */
                Song_SetCopyright(TSE3::Song *s, const std::string &str)
                : VariableSetCommand<TSE3::Song, std::string,
                                     const std::string &,
                                     &TSE3::Song::copyright,
                                     &TSE3::Song::setCopyright>
                                        (s, str, "copyright") {}
        };

        /**
         * Command to set a @ref Song's date.
         *
         * @short   Set Song date Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Song_SetDate
        : public VariableSetCommand<TSE3::Song, std::string,
                                    const std::string &,
                                    &TSE3::Song::date, &TSE3::Song::setDate>
        {
            public:
                /**
                 * To create this command specify the @ref Song object to alter
                 * and the new date.
                 */
                Song_SetDate(TSE3::Song *s, const std::string &str)
                : VariableSetCommand<TSE3::Song, std::string,
                                     const std::string &,
                                     &TSE3::Song::date, &TSE3::Song::setDate>
                                         (s, str, "song date") {}
        };

        /**
         * Command to insert a new blank @ref Track into a @ref Song.
         *
         * @short   Insert Track Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Song_InsertTrack : public Command
        {
            public:

                /**
                 * Insert a new blank @ref TSE3::Track into the @ref TSE3::Song
                 * @p song at index @p track.
                 *
                 * @param song  @ref TSE3::Song to insert @ref TSE3::Track into
                 * @param track Index at which to insert new @ref TSE3::Track
                 */
                Song_InsertTrack(TSE3::Song *song, size_t track);

            protected:

                /**
                 * @reimplemented
                 */
                virtual void executeImpl();

                /**
                 * @reimplemented
                 */
                virtual void undoImpl();

            private:

                TSE3::Song *song;
                int         track;
        };

        /**
         * Command to remove a @ref Track from a @ref Song.
         *
         * @short   Remove Track Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Song_RemoveTrack : public Command
        {
            public:

                /**
                 * Remove the specified @ref TSE3::Track from it's parent
                 * @ref TSE3::Song.
                 *
                 * @param track @ref TSE3:Track to remove
                 */
                Song_RemoveTrack(TSE3::Track *track);

                /**
                 * Remove the @ref TSE3::Track specified by the index
                 * @p track in the @ref TSE3::Song @p song.
                 *
                 * @param song  @ref TSE3::Song to remove @ref TSE3::Track from
                 * @param track Index of @ref TSE3::Track to remove
                 */
                Song_RemoveTrack(TSE3::Song *song, size_t track);

                /**
                 * Remove the specified @ref TSE3::Track @p track from the
                 * @ref TSE3::Song @p song.
                 *
                 * @param song  @ref TSE3::Song to remove @ref TSE3::Track from
                 * @param track @ref TSE3::Track to remove
                 */
                Song_RemoveTrack(TSE3::Song *song, Track *track);

                virtual ~Song_RemoveTrack();

            protected:

                /**
                 * @reimplemented
                 */
                virtual void executeImpl();

                /**
                 * @reimplemented
                 */
                virtual void undoImpl();

            private:

                TSE3::Song  *song;
                TSE3::Track *track;
                int          trackno;
        };

        /**
         * Command to set the solo @ref Track in a @ref Song.
         *
         * @short   Solo Track Command
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Command
         */
        class Song_SoloTrack : public Command
        {
            public:

                Song_SoloTrack(TSE3::Song *song, int track);

            protected:

                /**
                 * @reimplemented
                 */
                virtual void executeImpl();

                /**
                 * @reimplemented
                 */
                virtual void undoImpl();

            private:

                TSE3::Song *song;
                int         track;
                int         old;
        };
    }
}

#endif
