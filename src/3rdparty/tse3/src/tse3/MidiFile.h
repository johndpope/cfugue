/*
 * @(#)MidiFile.h 3.00 30 June 1999
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

#ifndef TSE3_MIDIFILE_H
#define TSE3_MIDIFILE_H

#include "tse3/listen/MidiFile.h"

#include "tse3/Notifier.h"
#include "tse3/Playable.h"

#include <string>
#include <iosfwd>
#include <cstddef>
#include <cstring>

namespace TSE3
{
    class Song;
    class Track;
    class MidiFileImport;
    class Progress;

    /**
     * Utility to import a MIDI file and convert it into a TSE3 @ref Song.
     *
     * There are two ways to play a standard MIDI file. One is to create
     * a @ref MidiFileImport object, use it to generate a @ref Song and
     * then play that. You will also be able to edit the @ref Song and
     * save it again.
     *
     * The second way to play a MIDI file it to use the @ref MidiFileImport
     * class' @ref Playable interface - this will generate a
     * MidiFileImportIterator object. The advantage of this is that you do
     * not have to delay for the import process (which for large MIDI files
     * can be significant). On the other hand, the data will not be editable
     * or modifiable.
     *
     * If you only need to play the MIDI file, use the second method.
     * If you need to modify the MIDI file data you need to use the @ref load
     * method to generate a @ref Song.
     *
     * @short   MIDI file importer
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Song
     * @see     MidiFileExport
     */
    class MidiFileImport : public Playable,
                           public Notifier<MidiFileImportListener>
    {
        public:

            /**
             * Creates a MidiFileImport object for the given MIDI file.
             *
             * If the file fails to open, then a @ref MidiFileImportError
             * exception is thrown.
             *
             * @param  filename Filename of MIDI file to import.
             * @param  verbose  Level of diagnostic output to produce
             *                  0: none,
             *                  1: basic reporting at chunk level
             *                  2: Include meta information
             *                  3: list all MIDI commands
             * @param  out      ostream to produce output on.
             * @throws MidiFileImportError
             */
            MidiFileImport(const std::string &filename,
                           int                verbose = 0,
                           std::ostream      &out     = std::cout);
            ~MidiFileImport();

            /**
             * Performs the MIDI file import, and returns a pointer to a
             * Song object holding the imported MIDI data.
             *
             * This Song object has been newed by the MidiFileImport object;
             * it is your responsability to delete it.
             *
             * @param  progress Object to call back to, or zero for no
             *                  progress information
             * @return New imported @ref Song - you must delete it
             * @throws MidiFileImportError
             */
            Song *load(Progress *progress = 0);

            /**
             * @reimplemented
             */
            virtual PlayableIterator *iterator(Clock index);

            /**
             * @reimplemented
             *
             * Calling this is likely to take longer than for most other
             * @ref Playable objects since the entire MIDI file has to be
             * scanned to determine the last clock time. This delay will
             * probably not be noticeable for any but the largest MIDI files.
             *
             * After you have done this once the result is cached for faster
             * subsequent access.
             */
            virtual Clock lastClock() const;

            friend class MidiFileImportIterator;

        private:

            /**
             * Loads the MIDI file header.
             */
            void loadHeader();

            /**
             * Loads an MTrk chunk.
             *
             * @param mtrkNo The MTrk's number, only used by the meta event
             *               loader.
             */
            void loadMTrk(size_t &pos, Song *song, int mtrkNo);

            /**
             * Loads a meta event in an MTrk chunk.
             */
            void loadMeta(size_t &pos, Song *song, Track *track,
                          int mtrkNo, Clock time, int &port,
                          Clock &end);

            /**
             * Reads a fixed length big endian value from position pos in
             * the file. Afterwards, pos is updated to point to the first byte
             * after the fixed value.
             *
             * @param pos    Position in file to read from.
             * @param length No bytes to read.
             */
            int readFixed(size_t &pos, int length);

            /**
             * Reads a variable size value as defined by the MIDI file standard.
             * Afterwards, pos is updated to point to the first byte after the
             * variable value.
             *
             * @param pos Position in file to read from.
             */
            int readVariable(size_t &pos);

            std::string   filename;
            int           verbose;
            std::ostream &out;

            unsigned char  *file;
            std::streampos  fileSize;

            int            filePPQN, fileFormat;        // set up by
            size_t         fileNoMTrks, fileHeaderSize; // loadHeader
            Clock          fileLastClock;               // set by the iterator
    };

    /**
     * Utility to export MIDI files converted from a TSE3 @ref Song.
     *
     * @short   MIDI file exporter
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Song
     * @see     MidiFileImport
     */
    class MidiFileExport
    {
        public:

            /**
             * Creates a MidiFileExport object for the given Song.
             *
             * @param song     The Song to convert.
             * @param format   The MIDI file format: 0 or 1. 1 retains the
             *                 most information in the file
             * @param out      The output stream to export to.
             * @param compact  Whether or not to compact MidiCommand_NoteOffs
             *                 to NoteOns with velocity 0 (saves a lot of
             *                 extra status bytes thanks to running status)
             * @param verbose  Level of diagnostic output to produce
             *                 0: none,
             *                 1: basic reporting at chunk level
             *                 2: Include meta information
             *                 3: list all MIDI commands
             * @param d_out    The output stream to produce diagnostics on
             */
            MidiFileExport(int                format  = 1,
                           bool               compact = true,
                           int                verbose = 0,
                           std::ostream      &d_out = std::cout);
            ~MidiFileExport();

            /**
             * Performs the MIDI file export.
             *
             * If the file fails to open, then a @ref MidiFileExportError
             * exception is thrown.
             *
             * @param  progress Object to call back to, or zero for no
             *                  progress information
             */
            void save(const std::string &filename, Song *song,
                      Progress *progress = 0);

            /**
             * As @ref save above, but you specify the ostream.
             */
            void save(std::ostream &out, Song *song, Progress *progress = 0);

            /*
             * Information about the MIDI file gleaned after having been saved.
             */

            /**
             * Returns MIDI file format.
             *
             * This method can be used to find out information after the MIDI
             * file has been exported.
             *
             * @return MIDI file format as supplied to constructor
             */
            int format() const { return _format; }

            /**
             * Returns the size of the exported MIDI file.
             *
             * This method can be used to find out information after the MIDI
             * file has been exported.
             *
             * @return Size in bytes of the exported MIDI file
             */
            size_t size() const { return _size; }

            /**
             * Returns the number of MTrks in the exported MIDI file.
             *
             * This method can be used to find out information after the MIDI
             * file has been exported.
             *
             * @return The number of MTrks in the exported MIDI file
             */
            int noMTrks() const { return _noMTrks; }

            /**
             * Returns whether MidiCommand_NoteOffs were compacted.
             *
             * This method can be used to find out information after the MIDI
             * file has been exported.
             *
             * @return Whether compaction was applied (as supplied to
             *         constructor)
             */
            bool compact() const { return _compact; }

        private:

            /**
             * Writes an MTrk to the output file for the given PlayableIterator
             * object.
             *
             * @param p     PlayableIterator object to export into MTrk
             * @param title Title to give MTrk (used if not type 0 file)
             */
            void writeMTrk(std::ostream &out, PlayableIterator *p,
                           const std::string &title = "");

            /**
             * Internal loop for writeMTrk that sends PlayableIterator data
             * to the file.
             */
            void writeMTrk_outputLoop(std::ostream &out, PlayableIterator *p);

            /**
             * Writes a MIDI event to the output stream.
             *
             * PRE: e is not a TSE meta event.
             */
            void writeMidiEvent(std::ostream &out, const MidiEvent &e);

            /**
             * Writes a TSE meta event to the output stream
             *
             * PRE: e is a TSE meta event.
             */
            void writeMetaEvent(std::ostream &out, const MidiEvent &e);

            /**
             * Writes a fixed length big endian value to the output file.
             *
             * @param value  Value to write.
             * @param length No bytes to read.
             */
            void writeFixed(std::ostream &out, int value, int length);

            /**
             * Writes a variable size value as defined by the MIDI file
             * standard.
             *
             * @param value Value to write.
             */
            void writeVariable(std::ostream &out, int value);

            /**
             * Writes a string to the output file.
             *
             * @param str       String to write.
             * @param terminate Whether or not to add a terminator to the
             *                  string.
             */
            void writeString(std::ostream &out, const std::string &str,
                             bool terminate = false);

            int           verbose;
            std::ostream &diag_out;
            int           _format;
            bool          _compact;

            /*
             * Things used in writeMTrk and lower
             */
            TSE3::Song    *song;
            size_t         _noMTrks;       // no of MTrks in the file so far
            std::streampos MTrkPos;        // where the current MTrk begins
            size_t         MTrkSize;       // whilst creating an MTrk, we have
                                           // to remember how big it is
            unsigned int   runningStatus;  // the current MIDI status byte
            Clock          lastEventClock; // use to calculate delta times
            size_t         _size;          // size of whole file
    };
}

#endif
