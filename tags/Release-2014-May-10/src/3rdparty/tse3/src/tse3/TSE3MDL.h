/*
 * @(#)TSE3MDL.h 3.00 20 August 2000
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

#ifndef TSE3_TSE3MDL_H
#define TSE3_TSE3MDL_H

#include "tse3/Serializable.h"

#include <string>
#include <cstddef>
#include <iostream>

namespace TSE3
{
    class Song;

    /**
     * The TSE3MDL object provides file operations for the @ref TSE3 library,
     * being the main entry point for saving/loading in the TSE3MDL file
     * format.
     *
     * It uses the @ref Serializable interface implemented by the various TSE3
     * classes.
     *
     * @short   Object used to load/save TSE3MDL song files
     * @author  Pete Goodliffe
     * @version 1.00
     * @see     Serializable
     */
    class TSE3MDL
    {
        public:
            /**
             * Create a TSE3MDL file operations object.
             *
             * @param appname Name of the application using TSE3.
             *                This is saved into the TSE3MDL file header
             * @param verbose Diagnostic level, normally you want to ignore
             *                this and accept the default value
             * @param diag    Where to send any diagnostic output
             */
            TSE3MDL(const std::string &appname = "",
                    int                verbose = 0,
                    std::ostream      &diag    = std::cout);

            /**
             * Save the given @ref Song to the file specified. If the file
             * already exists it will be overwritten.
             *
             * This will save the appropriate fill header and then call the
             * @ref Song class's save method.
             *
             * @param  filename Filename to save to
             * @param  song     Song object to save
             * @throws SerializableError
             */
            void save(const std::string &filename, const Song *song);

            /**
             * As @ref save above, but you specify the ostream.
             */
            void save(std::ostream &out, const Song *song);

            /**
             * Load a @ref Song from the file specified. The returned @ref Song
             * will have been newed from the free store, and so when you have
             * finished with it it is your responsibility to delete it.
             *
             * @param  filename Filename to save to
             * @param  progress The progress callback to keep informed of
             *                  progress, or zero for no callback
             * @return The loaded @ref Song - you must delete it
             * @throws SerializableError
             */
            Song *load(const std::string &filename, Progress *progress = 0);

            static const int MajorVersion = 100;
            static const int MinorVersion = 100;

        private:

            /**
             * An internal class for saving and loading the TSE3MDL file
             * header.
             *
             * The notable feature of this class is that it is the only one
             * that actually writes into most of the @ref SerializableLoadInfo
             * structure.
             */
            class Header : public Serializable
            {
                public:
                    Header(const std::string &originator);
                    virtual ~Header();
                    virtual void save(std::ostream &out, int indentLevel);
                    virtual void load(std::istream &in,
                                      SerializableLoadInfo &info);
                private:
                    std::string originator;
                    Header &operator=(const Header &);
                    Header(const Header &);
            } header;

            int           verbose;
            std::ostream &diag;
    };

    /**
     * The FileRegoniser class will return information about a given file.
     *
     * It can recognise three file types - the three types that TSE3 provides
     * support for. These are:
     * @li Native TSE3MDL files
     * @li TSE2MDL (the file format of TSE2)
     * @li Standard MIDI files
     *
     * @short   Object used to work out a file's type
     * @author  Pete Goodliffe
     * @version 1.00
     */
    class FileRecogniser
    {
        public:

            /**
             * Creates a FileRecogniser for a particular file.
             *
             * @param filename The name of the file to query
             */
            FileRecogniser(const std::string &filename);

            enum
            {
                Type_Error,   // file could not be read (does it exist?)
                Type_Unknown, // file type not recognised
                Type_TSE3MDL, // TSE3MDL file
                Type_TSE2MDL, // TSEMDL (TSE2) file
                Type_Midi     // MIDI file
            };

            /**
             * Returns the type of the file. The value will be one of the
             * below:
             * @li Type_Unknown - No recognised music file type
             * @li Type_TSE3MDL - A TSE3MDL file
             * @li Type_TSE2MDL - A TSEMDL file (from TSE2)
             * @li Type_Midi    - A standard MIDI file
             *
             * @return The type of the specified file
             */
            int type() const { return _type; }

            /**
             * Returns the size of the file.
             *
             * @return The size of the file in bytes.
             */
            size_t size() const { return _size; }

            /**
             * Loads the file in the appropriate way.
             *
             * This will use either the @ref TSE3MDL, @ref MidiImport or
             * @ref TSE2MDL classes to create a @ref Song from the data in
             * the file.
             *
             * If the file is of an unknown type, this will return zero.
             *
             * Any exception that may be thrown by the three file loading
             * classes may pass from this method.
             *
             * The loaded @ref Song will have been created with new, it is
             * now your responsibility to delete it.
             *
             * @param  progress A callback progress interface, or zero for none
             * @return A newly loaded @ref Song, or zero for faliure
             */
            Song *load(Progress *progress);

        private:

            std::string filename;
            int         _type;
            size_t      _size;
    };
}

#endif
