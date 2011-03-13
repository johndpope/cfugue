/*
 * @(#)Serializable.h 3.00 7 Jun 1999
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

#ifndef TSE3_SERIALIZABLE_H
#define TSE3_SERIALIZABLE_H

#include <iosfwd>
#include <iomanip>
#include <cstddef>
#include <ostream>

namespace TSE3
{
    class Song;
    class Progress;

    /**
     * Data structure used by the @ref Serializable class during loading.
     * It contains information that has been gained about the file, mostly
     * from the "Header" chunk. Most @ref Serializable classes will not need
     * to write to this struct, but will almost certainly need to read it
     * (at least to convert local file PPQN resolution to TSE3 system PPQN).
     */
    struct SerializableLoadInfo
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
        SerializableLoadInfo();
    };

    /**
     * Serializable is the base class of objects that can save their state
     * to a file in the block structure of the TSE3MDL specification.
     * Any class that wishes to be able to persist itself needs to implement
     * this interface.
     *
     * @sect Important note
     *
     * The entry point to save/load facilities is the @ref TSE3MDL
     * class, you should use that rather than the @ref Song class's Serializable
     * interface directly.
     *
     * See the @ref TSE3MDL class documentation for more details.
     *
     * @sect File format
     *
     * Data is to be saved in the TSE3MDL file format. Each object will save
     * an amount of data in a block, whose format follows the layout below.
     *
     * <pre>
     *   IDENTIFIER
     *   {
     *       DATA_TAG1:DATA
     *       DATA_TAG2:DATA
     *   }
     * </pre>
     *
     * One data item exists per line. A mechanism exists to determine the
     * number of indentation spaces that precedes each line (it is passed
     * as a parameter to each @ref save() call). There are means to make
     * it easy to produce this indentation (see the @ref indent() method).
     *
     * When parsing the data unrecognised data items should be ignored, and
     * unrecognised chunks shall be skipped.
     *
     * The file format is further described in the HTML documentation that
     * comes with the TSE3 library, in the "doc" directory.
     *
     * @short   Interface for serializable data
     * @author  Pete Goodliffe
     * @version 1.00
     * @see     TSE3MDL
     */
    class Serializable
    {
        public:

            Serializable() {}

            virtual ~Serializable() {}

            /**
             * This method causes the Serializable object to save a block
             * representing it's state to the ostream. The block will consist
             * of an opening "{" line, the data, zero or more sub-blocks
             * and a closing "}" line.
             *
             * It is the caller's responsibility to put an identifying tag
             * line in the ostream before calling this method so that it can
             * uniquely identify this Serializable.
             *
             * Calls to Serializable::save naturally nest. For example, the Song
             * Serializable class calls save on the @ref Track Serializable
             * class (amongst others).
             *
             * The default save will save an empty block.
             * (Not much use really ;-)
             *
             * @param out         ostream to send output to
             * @param indentLevel Current indent level (this will produce
             *                    indentLevel*4 spaces at the start of the
             *                    output lines).
             */
            virtual void save(std::ostream &out, int indentLevel) const;

            /**
             * This method loads a block for this particular Serializable
             * object. It is a precondition that it was a Serialiazble object
             * of this particular type that saved the block.
             *
             * The block tag will have been consumed from the stream, and the
             * next line to be parsed will be the opening "{". The block
             * and any sub-blocks (whether recognised or not) will be read
             * from the stream up to and including the closing "}".
             *
             * The default load will just skip the block without attempting
             * to interpret the contents.
             *
             * A utility class exists to aid writing the load method. This is
             * the @ref FileBlockParser, and it's related classes.
             *
             * @param in   istream to take input from
             * @param info Object holding information about the @ref Song
             *             being loaded
             * @see   FileBlockParser.
             * @see   SerializableLoadInfo
             */
            virtual void load(std::istream &in, SerializableLoadInfo &info);

        protected:

            class indent
            {
                public:
                    indent(int lvl) : level(lvl) {}
                    int level;
            };

            friend std::ostream &operator<<(std::ostream &, const indent &);

        private:

            Serializable &operator=(const Serializable &);
            Serializable(const Serializable &);
    };

    /**
     * Used by implementations of Serializable::save.
     *
     * An implementation can call this to indent the line to the
     * given level.
     *
     * e.g. o << indent(5) << "DATA_TAG:" << data << "\n";
     *
     * @internal
     */
    inline std::ostream &operator<<(std::ostream &s,
                                    const Serializable::indent &i)
    {
        for (int n = 0; n < i.level; ++n) s << "    ";
        return s;
    }
}

#endif
