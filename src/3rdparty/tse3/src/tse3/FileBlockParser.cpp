/*
 * @(#)FileBlockParser.cpp 3.00 24 Dec 1999
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

#include "tse3/FileBlockParser.h"

#include "tse3/Error.h"
#include "tse3/Progress.h"

#include <fstream>

using namespace TSE3;

/******************************************************************************
 * FileBlockParser class
 *****************************************************************************/

FileBlockParser::FileBlockParser()
: catchAll(0)
{
}


void FileBlockParser::add(const std::string &name, Serializable *block)
{
    blocks[name] = block;
}


void FileBlockParser::add(const std::string &name, FileItemParser *item)
{
    items[name] = item;
}


void FileBlockParser::add(FileItemParser *item)
{
    catchAll = item;
}


void FileBlockParser::parse(std::istream &in, SerializableLoadInfo &info)
{
    // Check that the chunk opens correctly

    bool        more = true;
    std::string line;
    do
    {
        more = getline(ws(in), line);
    }
    while (more && (!line.size() || line[0] == '#'));
    if (line != "{")
    {
        throw Error(FileFormatErr);
    }

    // OK, the chunk has an opening brace, now continue...

    ++info.noChunks;

    if (info.progress)
    {
        info.progress->progress(in.tellg());
    }

    more = true;
    while (more && getline(ws(in), line))
    {
        if (line == "}")
        {
            more = false;
        }
        else if (line.size() && line[0] == '#')
        {
            // skip the comment
        }
        else if (line.find(":") == std::string::npos)
        {
            // search for it in the blocks
            if (blocks.find(line) != blocks.end())
            {
                blocks[line]->load(in, info);
            }
            else
            {
                skipChunk(in);
                info.unknownChunks = true;
            }
        }
        else
        {
            // search for it in the items
            const std::string name = line.substr(0, line.find(":"));
            const std::string data = line.substr(line.find(":")+1);
            if (items.find(name) != items.end())
            {
                items[name]->parse(data);
            }
            else if (catchAll)
            {
                catchAll->parse(line);
            }
            else
            {
                info.unknownData = true;
            }
        }
    }
}


void FileBlockParser::skipChunk(std::istream &in)
{
    std::string open;
    getline(ws(in), open);
    if (open != "{") throw Error(FileFormatErr);
    int depth = 1;
    std::string line;
    do
    {
        getline(ws(in), line);
        if (line == "{")
        {
            ++depth;
        }
        else if (line == "}")
        {
            --depth;
        }
    }
    while (!in.eof() && depth);
}


/******************************************************************************
 * FileItemParser class
 *****************************************************************************/

FileItemParser::~FileItemParser()
{
}
