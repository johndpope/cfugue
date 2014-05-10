/*
 * @(#)FileBlockParser.h 3.00 24 Nov 1999
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

#ifndef TSE3_FILEBLOCKPARSER_H
#define TSE3_FILEBLOCKPARSER_H

#include "tse3/Serializable.h"

#include <map>
#include <string>
#include <sstream>

namespace TSE3
{
    class FileItemParser;
    struct Clock;

    /**
     * A TSE3 utility class.
     *
     * This class can be used by @ref Serializable objects to ease writing a
     * block parser. Provides facilities for reading a whole block (enclosed
     * by "{" and "}") in from an input stream, and act on the data contained
     * within it by either loading subblocks or passing on data lines
     * to helper objects of type @ref FileItemParser.
     *
     * The advantage of using this class is that it saves a lot of tedious
     * stream parsing code, and prevents silly mistakes like not handling
     * blocks that are not recognised (in this verison of the TSE3 library the
     * @ref Metronome does not have any sub-blocks, but who's to say there
     * won't be some in future versions?)
     *
     * @short   Internal utility for loading TSE3MDL blocks
     * @author  Pete Goodliffe
     * @version 1.00
     */
    class FileBlockParser
    {
        public:

            /**
             * Creates a FileBlockParser set up to do absolutely nothing.
             * Use the @ref add methods to attach items and sub-blocks.
             */
            FileBlockParser();

            /**
             * Add a sub-block to those understood by this parser. You
             * specify a @ref Serializable object to parse the sub-block, not
             * another FileBlockParser. This is to implement a lazy form of
             * object creation - the @ref Serializable may create another
             * FileBlockParser in it's load method, but only when it is called.
             * If it's block is not present, this FileBlockParser will not be
             * created. This prevents many many FileBlockParsers being created
             * before any of the file is Parsed. It is also useful in the
             * possible presence of cycles in the hierarchy of @ref Serializable
             * object.
             *
             * @param name  Block identification name.
             * @param block Serializable object that will parse the block.
             */
            void add(const std::string &name, Serializable *block);

            /**
             * Add an item to those recoginsed by this parser. These items
             * consist of lines of the form "Identifier:Data".
             *
             * The @ref FileItemParser that handles a particular handle will be
             * sent the "Data" part of the input line with the "Identifier:"
             * part stripped off.
             *
             * @param name String that identifies the line as a particular
             *             item type.
             * @param item FileItemParser to call to handle the data from this
             *             line.
             */
            void add(const std::string &name, FileItemParser *item);

            /**
             * Add a catch-all @ref FileItemParser to handle any lines not
             * regnoised in the list of named items. There can only be one of
             * these registered at one time. If there is no catch-all handler
             * registered than unrecognised items in the block will be ignored.
             *
             * The catch-all @ref FileItemParser will be called with the
             * contents of the entire line. Note that the "Identifier:" part
             * IS NOT stipped.
             *
             * @param item The catch all @ref FileItemParser
             */
            void add(FileItemParser *item);

            /**
             * Parse the block in the istream in.
             *
             * The block tag will have been consumed by the enclosing block
             * parser, and the next line to be parsed will be the opening
             * "{". The parser will consume all lines up to the block's
             * closing "}".
             *
             * All sub-blocks that are recognised will be sent to the
             * corresponding @ref Serializable object for parsing. Any
             * unrecognised sub-blocks will be skipped.
             *
             * All items that are recognised will be sent to the corresponding
             * @ref FileItemParser. Any unrecognised items will be sent to the
             * catch-all handler. If there is no catch-all handler then
             * the line will be ignored.
             *
             * @param in   istream to take input from
             * @param info Object holding information on the input stream
             */
            void parse(std::istream &in, SerializableLoadInfo &info);

        private:

            /**
             * This function skips a chunk in a TSE3MDL file. It expects the
             * chunk tag line to have been consume, and it's next input line to
             * be a "{". It consumes up to and including the final "}" of the
             * chunk, consuming any chunks that may be within this one.
             *
             * @param in istream to take input from
             */
            void skipChunk(std::istream &i);

            std::map<std::string, FileItemParser*>  items;
            std::map<std::string, Serializable*>    blocks;
            FileItemParser                         *catchAll;
    };

    /**
     * A TSE3 utility class.
     *
     * This class is used by the @ref FileBlockParser class to act on lines of
     * the format "Name:Data". The @ref FileBlockParser will identify the line
     * as beginning with Name and mantains the mapping of Name string to
     * FileItemParser. It then passes on the data portion of the line to the
     * appropriate FileItemParser.
     *
     * @short   Internal utility for parsing TSE3MDL data lines
     * @author  Pete Goodliffe
     * @version 1.00
     */
    class FileItemParser
    {
        public:

            FileItemParser() {} // XXX Compiler warning without this
            virtual ~FileItemParser() = 0;

            /**
             * This method is called by the @ref FileBlockParser when it finds
             * a data line that is handled by this FileItemParser.
             *
             * @param data The data line to be handled. The "Name:" prefix
             *             has already been stripped.
             */
            virtual void parse(const std::string &data) = 0;

        private:

            FileItemParser &operator=(const FileItemParser &);
            FileItemParser(const FileItemParser &);
    };

    /**
     * A TSE3 utility class.
     *
     * A utility class implementing a specific type of @ref FileItemParser.
     * This class will call a member function with signature void setXXX(bool)
     * with the value true if the data string is either "On" or "Yes".
     *
     * @short   Internal utility for parsing On/Off/Yes/No data lines
     * @author  Pete Goodliffe
     * @version 1.00
     */
    template <class T>
    class FileItemParser_OnOff : public FileItemParser
    {
        public:
            typedef void (T::*fn_t)(bool);
            FileItemParser_OnOff(T *obj, fn_t mfun)
            : obj(obj), mfun(mfun) {}
            /**
             * @reimplemented
             */
            void parse(const std::string &data)
            {
                (obj->*mfun)(data == "On" || data == "Yes");
            }
        private:
            T    *obj;
            fn_t  mfun;
    };

    /**
     * A TSE3 utility class.
     *
     * A utility class implementing a specific type of @ref FileItemParser.
     * This class will call a member function with signature
     * void setXXX(reason r, bool) with the boolean value of the data string.
     *
     * @short   Internal utility for parsing boolean values with reason codes
     * @short   Internal utility for parsing numeric data lines
     * @author  Pete Goodliffe
     * @version 1.00
     */
    template<class T, class reason>
    class FileItemParser_ReasonOnOff : public FileItemParser
    {
        public:
            typedef void (T::*fn_t)(reason, bool);
            FileItemParser_ReasonOnOff(T *obj, fn_t mfun, reason r)
            : obj(obj), r(r) , mfun(mfun){}
            /**
             * @reimplemented
             */
            void parse(const std::string &data)
            {
                (obj->*mfun)(r, (data == "On" || data == "Yes"));
            }
        private:
            T      *obj;
            reason  r;
            fn_t    mfun;
    };

    /**
     * A TSE3 utility class.
     *
     * A utility class implementing a specific type of @ref FileItemParser.
     * This class will call a member function with signature void setXXX(int)
     * with the numeric value of the data string in a similar manner to
     * FileItemParser_OnOff.
     *
     * @short   Internal utility for parsing numeric data lines
     * @author  Pete Goodliffe
     * @version 1.00
     */
    template <class T>
    class FileItemParser_Number : public FileItemParser
    {
        public:
            typedef void (T::*fn_t)(int);
            FileItemParser_Number(T *obj, fn_t mfun)
            : obj(obj), mfun(mfun) {}
            /**
             * @reimplemented
             */
            void parse(const std::string &data)
            {
                int i;
                std::istringstream si(data);
                si >> i;
                (obj->*mfun)(i);
            }
        private:
            T    *obj;
            fn_t  mfun;
    };

    /**
     * A TSE3 utility class.
     *
     * A utility class implementing a specific type of @ref FileItemParser.
     * This class will call a member function with signature
     * void setXXX(Clock) with the numeric value of the data string in
     * a similar manner to FileItemParser_OnOff.
     *
     * This FileItemParser looks partically identical to the _Number version.
     *
     * @short   Internal utility for parsing @ref Clock data lines
     * @author  Pete Goodliffe
     * @version 1.00
     */
    template <class T>
    class FileItemParser_Clock : public FileItemParser
    {
        public:
            typedef void (T::*fn_t)(Clock);
            FileItemParser_Clock(T *obj, fn_t mfun)
            : obj(obj), mfun(mfun) {}
            /**
             * @reimplemented
             */
            void parse(const std::string &data)
            {
                int i;
                std::istringstream si(data);
                si >> i;
                (obj->*mfun)(i);
            }
        private:
            T    *obj;
            fn_t  mfun;
    };

    /**
     * A TSE3 utility class.
     *
     * A utility class implementing a specific type of @ref FileItemParser.
     * This class will call a member function with signature
     * void setXXX(const string &) with the string in the data string.
     *
     * @short   Internal utility for parsing string data lines
     * @author  Pete Goodliffe
     * @version 1.00
     */
    template <class T>
    class FileItemParser_String : public FileItemParser
    {
        public:
            typedef void (T::*fn_t)(const std::string &);
            FileItemParser_String(T *obj, fn_t mfun)
            : obj(obj), mfun(mfun) {}
            /**
             * @reimplemented
             */
            void parse(const std::string &data)
            {
                (obj->*mfun)(data);
            }
        private:
            T    *obj;
            fn_t  mfun;
    };
}

#endif
