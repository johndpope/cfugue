/*
 * @(#)TSE3MDL.cpp 3.00 28 May 1999
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

#include "tse3/TSE3MDL.h"

#include "tse3/FileBlockParser.h"
#include "tse3/Song.h"
#include "tse3/Error.h"
#include "tse3/Progress.h"
#include "tse3/MidiFile.h"
#include "tse3/TSE2MDL.h"

#include <sstream>
#include <fstream>
#include <memory>

using namespace TSE3;

/******************************************************************************
 * TSE3MDL class
 *****************************************************************************/

TSE3MDL::TSE3MDL(const std::string &appname, int verbose, std::ostream &diag)
: header(appname), verbose(verbose), diag(diag)
{
}


void TSE3MDL::save(const std::string &filename, const Song *song)
{
    std::ofstream out(filename.c_str());
    if (!out)
    {
        throw SerializableError(CouldntOpenFileErr);
    }

    save(out, song);

    // Perhaps some choices in an epilogue here?
    out.close();
}


void TSE3MDL::save(std::ostream &out, const Song *song)
{
    // Save the header
    out << "TSE3MDL\n"
        << "{\n"
        << "    Header\n";
    header.save(out, 1);
    out << "    Song\n";
    song->save(out, 1);
    out << "}\n";
}


Song *TSE3MDL::load(const std::string &filename, Progress *progress)
{
    std::ifstream in(filename.c_str());
    if (!in)
    {
        throw SerializableError(CouldntOpenFileErr);
    }

    // Progress information
    if (progress)
    {
        in.seekg(0, std::ios::end);
        progress->progressRange(0, in.tellg());
        in.seekg(0, std::ios::beg);
    }

    // Check first line reads TSE3MDL
    std::string tse3mdl;
    getline(in, tse3mdl);
    if (tse3mdl != "TSE3MDL") throw Error(InvalidFileTypeErr);

    std::auto_ptr<Song>   song(new Song(0));
    SerializableLoadInfo  info;
    info.song     = song.get();
    info.progress = progress;

    // Now scan the file contents. Presumably we'll first get the Header chunk
    FileBlockParser parser;
    parser.add("Header", &header);
    parser.add("Song",    song.get());
    parser.parse(in, info);
    return song.release();
}


TSE3MDL::Header::Header(const std::string &originator)
: originator(originator)
{
}


TSE3MDL::Header::~Header()
{
}


void TSE3MDL::Header::save(std::ostream &out, int i)
{
    out << indent(i)   << "{\n"
        << indent(i+1) << "Version-Major:" << TSE3MDL::MajorVersion << "\n"
        << indent(i+1) << "Version-Minor:" << TSE3MDL::MinorVersion << "\n"
        << indent(i+1) << "Originator:"    << originator            << "\n"
        << indent(i+1) << "PPQN:"          << Clock::PPQN           << "\n"
        << indent(i)   <<"}\n";
}


void TSE3MDL::Header::load(std::istream &in, SerializableLoadInfo &info)
{
    std::string open;
    getline(ws(in), open);
    if (open != "{") throw Error(FileFormatErr);
    std::string line;
    bool more = true;
    while (more && getline(ws(in), line))
    {
        if (line.substr(0, 5).compare("PPQN:") == 0)
        {
            std::istringstream si(line.c_str()+5);
            si >> info.PPQN;
        }
        else if (line.substr(0, 14).compare("Version-Major:") == 0)
        {
            std::istringstream si(line.c_str()+14);
            si >> info.major;
        }
        else if (line.substr(0, 14).compare("Version-Minor:") == 0)
        {
            std::istringstream si(line.c_str()+14);
            si >> info.minor;
        }
        else if (line == "}")
        {
            more = false;
        }
    }
}


/******************************************************************************
 * FileRecogniser class
 *****************************************************************************/

FileRecogniser::FileRecogniser(const std::string &f)
: filename(f)
{
    _size = 0;
    std::ifstream i(filename.c_str());

    if (i)
    {

        std::string header;
        char   c;

        while (header.size() < 10
               && i
               && (c = i.get()) != '\n'
               && c > 32)
        {
            header += c;
        }

        _type = (header == "TSE3MDL") ? Type_TSE3MDL
              : (header == "TSEMDL")  ? Type_TSE2MDL
              : (header == "MThd")    ? Type_Midi
              : Type_Unknown;
    }
    else
    {
        _type = Type_Error;
    }
}


Song *FileRecogniser::load(Progress *progress)
{
    Song *song = 0;
    switch (_type)
    {
        case Type_TSE3MDL:
        {
            TSE3MDL tse3mdl;
            song = tse3mdl.load(filename, progress);
            break;
        }
        case Type_TSE2MDL:
        {
            TSE2MDL tse2mdl;
            song = tse2mdl.load(filename, progress);
            break;
        }
        case Type_Midi:
        {
            MidiFileImport mfi(filename);
            song = mfi.load(progress);
            break;
        }
    }
    return song;
}

