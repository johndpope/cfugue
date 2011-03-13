/*
 * @(#)file/XML.h 3.00 10 Aug 2001
 *
 * Copyright (c) 2001 Pete Goodliffe (pete@cthree.org)
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

#ifndef TSE3_FILE_XML_H
#define TSE3_FILE_XML_H

#include <iosfwd>
#include <iomanip>
#include <cstddef>
#include <string>
#include <map>
#include <sstream>

#include "tse3/Midi.h"

namespace TSE3
{
    class Song;
    class Progress;

    namespace File
    {

        /**
         * Data structure used by the @ref Serializable class during loading.
         * It contains information that has been gained about the file, mostly
         * from the "Header" chunk. Most @ref Serializable classes will not need
         * to write to this struct, but will almost certainly need to read it
         * (at least to convert local file PPQN resolution to TSE3 system PPQN).
         */
        struct XmlLoadInfo
        {
            /**
             * The PPQN resolution of timestamps in this file. You can use
             * this to convert timestamps to the correct values in use in the
             * TSE3 library. See @ref Clock's convert() method.
             * This information is read from the file's "Header" chunk.
             */
            int PPQN;

            /**
             * The current Song. This is set once by the @ref TSE3MDL class.
             * You may not write over this value afterwards or you will
             * create undefined behaviour.
             */
            Song *song;

            /**
             * The files's najor version number, read from it's "Header" chunk.
             * This should be informational since the TSE3MDL file format is
             * forwards and backwards compatible.
             *
             * The (invalid) value -1 means that no major version has been read.
             */
            int major;

            /**
             * The file's minor version number, read from it's "Header" chunk.
             * This should be informational since the TSE3MDL file format is
             * forwards and backwards compatible.
             *
             * The (invalid) value -1 means that no minor version has been read.
             */
            int minor;

            /**
             * A boolean flag which reports whether the loading encountered
             * any unknown chunks (which will have been skipped over).
             *
             * If you are not using the @ref FileBlockParser class to perform
             * parsing, you must set this flag to true if you come across any
             * unrecognised chunks. The @ref FileBlockParser class does this
             * automatically.
             */
            bool unknownChunks;

            /**
             * A boolean flag which reports whether the loading encountered
             * any unknown data lines (which will have been ignored).
             *
             * If you are not using the @ref FileBlockParser class to perform
             * parsing, you must set this flag to true if you come across any
             * unrecognised data lines. The @ref FileBlockParser class does this
             * automatically.
             */
            bool unknownData;

            /**
             * A counter which reports how many chunks (and sub-chunks) were
             * in the file.
             *
             * If you are not using the @ref FileBlockParser class to perform
             * parsing, you must increment this counter if you come across a new
             * chunk. The @ref FileBlockParser class does this automatically.
             */
            size_t noChunks;

            /**
             * If the operation that triggered this serializable operation
             * provided a @ref Progress callback object, this points to it.
             *
             * This information is used by the @ref FileBlockParser utility.
             * You do not need to handle this in client code.
             */
            Progress *progress;

            /**
             * Sets up some default values for the struct values.
             *
             * These are:
             * @li PPQN          - @ref Clock::PPQN
             * @li song          - 0
             * @li major         - -1
             * @li minor         - -1
             * @li unknownChunks - false
             * @li unknownData   - false
             * @li progress      - 0
             */
            XmlLoadInfo();
        };


        /**********************************************************************
         * Writing XML files
         *********************************************************************/

        /**
         * A utility class to make writing XML files easier.
         */
        class XmlFileWriter
        {
            public:

                XmlFileWriter(std::ostream &out);
                ~XmlFileWriter();

                void openElement(const std::string &name);
                void closeElement();

                void element(const std::string &name, const std::string &value);
                void element(const std::string &name, const char        *value);
                void element(const std::string &name, int                value);
                void element(const std::string &name, unsigned int       value);
                void element(const std::string &name, bool               value);

                void comment(const std::string &comment);

                class AutoElement
                {
                    public:
                        AutoElement(TSE3::File::XmlFileWriter &writer,
                                    const std::string &name)
                        : _writer(writer)
                        {
                            _writer.openElement(name);
                        }
                        ~AutoElement()
                        {
                            _writer.closeElement();
                        }
                    private:
                        TSE3::File::XmlFileWriter &_writer;
                };

            private:

                void indent(std::ostream &out);

                std::ostream &out;
                int           indentLevel;

                class XmlFileWriterImpl *pimpl;
        };

        /**
         * A utility class to make reading TSE3MDL files easier.
         */
        class XmlFileReader
        {
            public:

                XmlFileReader(std::istream &in);
                ~XmlFileReader();

                TSE3::Song *load();
            private:
                std::istream &in;
        };


        /**********************************************************************
         * Reading XML files
         *********************************************************************/

        class XmlElementParser;

        class XmlBlockParser
        {
            public:
                typedef void (block_handler_t)();

                XmlBlockParser();

                void add(const std::string &tag, XmlBlockParser &handler);
                void add(const std::string &tag, XmlElementParser &handler);
                void add(XmlElementParser &handler);

                void parse(std::istream &in, const std::string &tag,
                           XmlLoadInfo &info);

            private:

                void skipBlock(std::istream &in);

                std::map<std::string, XmlElementParser*>  elements;
                std::map<std::string, XmlBlockParser*>    blocks;
                XmlElementParser                         *catchAll;
        };


        class XmlElementParser
        {
            public:

                XmlElementParser() {}
                virtual ~XmlElementParser() = 0;

                /**
                 * This method is called by the @ref XmlBlockParser when it
                 * finds a data line that is handled by this XmlElementParser.
                 *
                 * @param data The data from the "value" attribute to be
                 *             handled.
                 */
                virtual void parse(const std::string &data) = 0;

            private:

                XmlElementParser &operator=(const XmlElementParser &);
                XmlElementParser(const XmlElementParser &);
        };

        /**
         * A TSE3 utility class.
         *
         * A utility class implementing a specific type of @ref XmlElementParser.
         * This class will call a member function with signature
         * void setXXX(reason r, bool) with the boolean value of the data string.
         *
         * @short   Internal utility for parsing boolean values with reason codes
         * @short   Internal utility for parsing numeric data lines
         * @author  Pete Goodliffe
         * @version 1.00
         */
        template<class T, class reason>
        class XmlElementParser_ReasonOnOff : public XmlElementParser
        {
            public:
                typedef void (T::*fn_t)(reason, bool);
                XmlElementParser_ReasonOnOff(T *obj, fn_t mfun, reason r)
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
         * A utility class implementing a specific type of @ref XmlElementParser.
         * This class will call a member function with signature void setXXX(int)
         * with the numeric value of the data string in a similar manner to
         * XmlElementParser_OnOff.
         *
         * @short   Internal utility for parsing numeric data lines
         * @author  Pete Goodliffe
         * @version 1.00
         */
        template <class T>
        class XmlElementParser_Number : public XmlElementParser
        {
            public:
                typedef void (T::*fn_t)(int);
                XmlElementParser_Number(T *obj, fn_t mfun)
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
         * A utility class implementing a specific type of @ref XmlElementParser.
         * This class will call a member function with signature
         * void setXXX(Clock) with the numeric value of the data string in
         * a similar manner to XmlElementParser_OnOff.
         *
         * This XmlElementParser looks partically identical to the _Number version.
         *
         * @short   Internal utility for parsing @ref Clock data lines
         * @author  Pete Goodliffe
         * @version 1.00
         */
        template <class T>
        class XmlElementParser_Clock : public XmlElementParser
        {
            public:
                typedef void (T::*fn_t)(Clock);
                XmlElementParser_Clock(T *obj, fn_t mfun)
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
         * A utility class implementing a specific type of @ref XmlElementParser.
         * This class will call a member function with signature
         * void setXXX(const string &) with the string in the data string.
         *
         * @short   Internal utility for parsing string data lines
         * @author  Pete Goodliffe
         * @version 1.00
         */
        template <class T>
        class XmlElementParser_String : public XmlElementParser
        {
            public:
                typedef void (T::*fn_t)(const std::string &);
                XmlElementParser_String(T *obj, fn_t mfun)
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
}

#endif
