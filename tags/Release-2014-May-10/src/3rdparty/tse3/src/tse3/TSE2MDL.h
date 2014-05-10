/*
 * @(#)TSE2MDL.h 3.00 28 May 1999
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

#ifndef TSE3_FILEOPS2_H
#define TSE3_FILEOPS2_H

#include "tse3/Midi.h"

#include <string>
#include <iostream>
#include <cstddef>

namespace TSE3
{
    class Song;
    class Track;
    class Part;
    class Progress;

    /**
     * This class provides file import for TSE2MDL files.
     *
     * It uses the TSE2MDL file type, as created by programs based on the
     * earlier TSE2 library. This format differs somewhat from the
     * TSE3MDL format, notably in that it is a binary format. It is not
     * forwards and backwards compatible, and it does not support all of
     * TSE3's @ref Song structure. Since these files are produced by programs
     * based on the older TSE2 library this class is provided merely
     * for compatibility.
     *
     * When saving/loading TSE3 @ref Song objects, you should prefer the
     * TSE3MDL file format which is accessed via the @ref TSE3MDL class.
     *
     * You may see TSE2MDL files referred to simply as 'TSEMDL' files.
     *
     * @short   TSE version 2 file operations
     * @author  Pete Goodliffe
     * @version 3.00
     */
    class TSE2MDL
    {

        public:

            /**
             * Creates a TSE2MDL object for the given TSEMDL file.
             *
             * @param appname Name of the application using TSE3.
             *                This is saved into the TSE3MDL file header
             *                (Not currently used, would be by a future
             *                export method.)
             * @param verbose Whether to produce diagnostic output.
             * @param out     ostream to produce diagnostic output on.
             */
            TSE2MDL(const std::string &appname = "",
                    bool               verbose = false,
                    std::ostream      &out     = std::cout);

            /**
             * Performs the TSE2MDL file import, and returns a pointer to a
             * @ref Song object holding the imported data.
             *
             * This @ref Song object has been newed by the TSE2MDL
             * object; it is your responsibility to delete it.
             *
             * @param  filename Filename of TSE2MDL file to import.
             * @return The imported @ref Song - you must delete it
             */
            Song *load(const std::string &filename, Progress *progress = 0);

        private:

            /*
             * Internal methods that load TSE2 chunks.
             */
            bool load_header(std::istream &in);
            bool load_songTitle(std::istream &in);
            bool load_songAuthor(std::istream &in);
            bool load_songCopyright(std::istream &in);
            bool load_songDate(std::istream &in);
            bool load_TempoTrack(std::istream &in, int len);
            bool load_TimeSigTrack(std::istream &in, int len);
            bool load_FlagTrack(std::istream &in, int len);
            bool load_Track(std::istream &in);
            bool load_ExtendedTrack(std::istream &in, int length);
            bool load_Phrase(std::istream &in, int len);
            bool load_Part(std::istream &in);
            bool load_ExtendedPart(std::istream &in, int len);
            bool load_Choices(std::istream &in, int len);
            bool skip(std::istream &in, int len);

            int freadInt(std::istream &in, int size);
            int freadPString(std::istream &in, char *buffer);

            std::string   appname;
            bool          verbose;
            std::ostream &out;

            Track     *prevTrack;
            Part      *prevPart;
            Clock      file_PPQN;
            size_t     file_noTracks;
            int        file_size;
            Song      *song;
    };
}

#endif
