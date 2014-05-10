/*
 * @(#)Error.h 3.00 22 June 2000
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

#ifndef TSE3_ERROR_H
#define TSE3_ERROR_H

#include <string>
#include <exception>

namespace TSE3
{
    /**************************************************************************
     * General error handling declarations
     *************************************************************************/

    /**
     * This enum type represents each of the different types of error that
     * may be raised by the TSE3 API. They are used by the exception class
     * @ref Error and it's subclasses.
     *
     * You can use the @ref errString() function to convert the ErrorCode into
     * an English description string.
     *
     * @see Error
     * @see errString
     */
    enum ErrorCode
    {
        // Song error codes
        TrackAlreadyInsertedErr,

        // Track error codes
        PartAlreadyInsertedErr,
        PartOverlapErr,
        NoPartInsertedErr,

        // Track and Part error codes
        PartTimeErr,
        PhraseUnparentedErr,

        // Phraselist error codes
        PhraseNameExistsErr,
        PhraseAlreadyInsertedErr,
        InvalidPhraseNameErr,

        // MidiFile error codes
        MidiFileImportErr,
        MidiFileExportErr,

        // Serializable error codes (via TSE3MDL, TSE2MDL)
        CouldntOpenFileErr,
        InvalidFileTypeErr,
        FileFormatErr,

        // MidiScheduler error codes,
        MidiSchedulerCreateErr,

        /**
         * An error not specified by the core TSE3 library.
         */
        OtherErr,

        /**
         * A convenience value representing the number of TSE3 error reason
         * codes.
         */
        NoErrorCodes
    };

    /**
     * This is the base class of all TSE3 errors.
     *
     * The Error class provides a reason code for each type of error that
     * can be raised, although subclasses are also used to distinguish the
     * type of the error. Each subclass may have more than one reason code
     * associated with it. The reason codes are provided by the
     * @ref ErrorCodes enum type.
     *
     * The rationale for the reason codes, rather than use of pure subclassing
     * to denote error type, is to allow the application to provide
     * internationalised error messages for the user. It is easier to lookup a
     * string via an integer reason code than convert a class type to some
     * error string. (See @ref ErrorCode and @ref errString() for more
     * information on this.)
     *
     * @short   TSE3 exception base class
     * @author  Pete Goodliffe
     * @version 3.00
     */
    class Error : public std::exception
    {
        public:

            /**
             * Creates an Error object with the specified reason code.
             *
             * @param reason Reason code for this Error
             */
            Error(ErrorCode reason) : _reason(reason) {}

            ~Error() throw () {}

            /**
             * Returns the @ref ErrorCode for this Error object.
             *
             * Use @ref errString to get a string representation of this
             * error.
             */
            ErrorCode reason() const { return _reason; }

            /**
             * @reimplemented
             *
             * @see TSE3::errString
             */
            virtual const char *what() const throw();

        private:

            ErrorCode _reason;
    };

    /**
     * Returns a string containing an English representation of the
     * specified TSE3 @ref ErrorCode.
     *
     * @see ErrorCode
     * @see Error
     */
    const char *errString(ErrorCode reason);

    /**************************************************************************
     * Specific Error subclasses
     *************************************************************************/

    /**
     * Class of exception thrown by the @ref MidiFileImport class.
     *
     * @short   Exception thrown by MidiFileImport
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Song
     */
    class MidiFileImportError : public Error
    {
        public:
            MidiFileImportError(std::string const &str)
                : Error(MidiFileImportErr), s(str) {}
            ~MidiFileImportError() throw () {}
            const std::string &operator *() const { return s; }
        private:
            const std::string s;
    };

    /**
     * Class of exception thrown by the @ref MidiFileExport class.
     *
     * @short   Exception thrown by MidiFileExport
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Song
     */
    class MidiFileExportError : public Error
    {
        public:
            MidiFileExportError(std::string const &str)
                : Error(MidiFileExportErr), s(str) {}
            ~MidiFileExportError() throw () {}
            const std::string &operator *() const { return s; }
        private:
            const std::string s;
    };

    /**
     * Exception thrown by @ref PhraseList class.
     *
     * @short   PhraseList exception
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PhraseList
     */
    class PhraseListError : public Error
    {
        public:
            PhraseListError(ErrorCode rc) : Error(rc) {}
            ~PhraseListError() throw () {}
    };

    /**
     * Class of exception thrown by the @ref Song insert methods.
     */
    class SongError : public Error
    {
        public:
            SongError(ErrorCode rc) : Error(rc) {}
            ~SongError() throw () {}
    };

    /**
     * Class of exception thrown by the @ref Track class when inserting
     * a @ref Part fails.
     */
    class TrackError : public Error
    {
        public:
            TrackError(ErrorCode rc) : Error(rc) {}
            ~TrackError() throw () {}
    };

    /**
     * Class of exception thrown by the @ref Part class.
     */
    class PartError : public Error
    {
        public:
            PartError(ErrorCode rc) : Error(rc) {}
            ~PartError() throw () {}
    };

    /**
     * Exception class thrown by OSS MidiScheduler classes.
     */
    class MidiSchedulerError : public Error
    {
        public:
            MidiSchedulerError(ErrorCode rc) : Error(rc) {}
            ~MidiSchedulerError() throw () {}
    };

    /**
     * Exception class thrown by Serializable classes.
     */
    class SerializableError : public Error
    {
        public:
            SerializableError(ErrorCode rc) : Error(rc) {}
            ~SerializableError() throw () {}
    };
}

#endif
