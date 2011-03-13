/*
 * @(#)file/XML.cpp 3.00 10 Aug 2001
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

#include "tse3/file/XML.h"

#include "tse3/Progress.h"

#include <stack>

// Loading Songs
#include <memory>
#include "tse3/Song.h"

/******************************************************************************
 * XmlLoadInfo
 *****************************************************************************/

TSE3::File::XmlLoadInfo::XmlLoadInfo()
: PPQN(TSE3::Clock::PPQN), song(0),
//  major(-1), minor(-1), gcc 3.0 doesn't like this XXX
  unknownChunks(false), unknownData(false),
  noChunks(0), progress(0)
{
    major = -1;
    minor = -1;
}


/******************************************************************************
 * XmlFileWriter
 *****************************************************************************/

class TSE3::File::XmlFileWriterImpl
{
    public:
        std::stack<std::string> elements;
};

TSE3::File::XmlFileWriter::XmlFileWriter(std::ostream &o)
: out(o), indentLevel(0), pimpl(new TSE3::File::XmlFileWriterImpl)
{
    //out << "<?xml version=\"1.0\"?>\n";
    //TSE3MDL isn't real XML (yet) so we don't write this
}


TSE3::File::XmlFileWriter::~XmlFileWriter()
{
    delete pimpl;
}


void TSE3::File::XmlFileWriter::indent(std::ostream &out)
{
    for (int n = 0; n < indentLevel; n++) out << "  ";
}


void TSE3::File::XmlFileWriter::openElement(const std::string &name)
{
    indent(out);
    out << "<" << name << ">\n";
    pimpl->elements.push(name);
    indentLevel++;
}


void TSE3::File::XmlFileWriter::closeElement()
{
    indentLevel--;
    indent(out);
    out << "</" << pimpl->elements.top() << ">\n";
    pimpl->elements.pop();
}


void TSE3::File::XmlFileWriter::element(const std::string &name,
                                        const std::string &value)
{
    indent(out);
    out << "<" << name << " value=\"" << value << "\"/>\n";
}


void TSE3::File::XmlFileWriter::element(const std::string &name,
                                        const char        *value)
{
    indent(out);
    out << "<" << name << " value=\"" << value << "\"/>\n";
}


void TSE3::File::XmlFileWriter::element(const std::string &name, int value)
{
    indent(out);
    out << "<" << name << " value=\"" << value << "\"/>\n";
}


void TSE3::File::XmlFileWriter::element(const std::string &name, unsigned int value)
{
    indent(out);
    out << "<" << name << " value=\"" << value << "\"/>\n";
}


void TSE3::File::XmlFileWriter::element(const std::string &name, bool value)
{
    indent(out);
    out << "<" << name << " value=\"" << (value ? "true" : "false") << "\"/>\n";
}


void TSE3::File::XmlFileWriter::comment(const std::string &comment)
{
    indent(out);
    out << "<!-- " << comment << " -->\n";
}



/******************************************************************************
 * XmlFileReader
 *****************************************************************************/

TSE3::File::XmlFileReader::XmlFileReader(std::istream &in) : in(in)
{
}


TSE3::File::XmlFileReader::~XmlFileReader()
{
}


namespace
{
    class SimpleNumberParser : public TSE3::File::XmlElementParser
    {
        public:
            SimpleNumberParser(int &number)
            : number(number) {}
            /**
             * @reimplemented
             */
            void parse(const std::string &data)
            {
                int i;
                std::istringstream si(data);
                si >> i;
                number = i;
            }
        private:
            int &number;
    };
}


TSE3::Song *TSE3::File::XmlFileReader::load()
{
    XmlBlockParser            parser;
    XmlLoadInfo               info;
    XmlBlockParser            tse3parser;
    SimpleNumberParser        versionMajor(info.major);
    SimpleNumberParser        versionMinor(info.minor);
    SimpleNumberParser        ppqn(info.PPQN);
    std::auto_ptr<TSE3::Song> song(new TSE3::Song(0));

    info.song     = song.get();
    info.progress = 0;//progress;

    parser.add("TSE3", tse3parser);

    tse3parser.add("Version-Major", versionMajor);
    tse3parser.add("Version-Minor", versionMinor);
    tse3parser.add("PPQN",          ppqn);

    parser.parse(in, "", info);
    return song.release();
}


/******************************************************************************
 * XmlBlockParser
 *****************************************************************************/

TSE3::File::XmlBlockParser::XmlBlockParser()
: catchAll(0)
{
}


void TSE3::File::XmlBlockParser::add(const std::string &name, XmlBlockParser &block)
{
    blocks[name] = &block;
}


void TSE3::File::XmlBlockParser::add(const std::string &name, XmlElementParser &item)
{
    elements[name] = &item;
}


void TSE3::File::XmlBlockParser::add(XmlElementParser &item)
{
    catchAll = &item;
}


void TSE3::File::XmlBlockParser::parse(std::istream &in, const std::string &tag,
                           XmlLoadInfo &info)
{
    // Check that the chunk opens correctly
std::cout << "XBP: start of tag given as \""<<tag<<"\"\n";


    if (info.progress)
    {
        info.progress->progress(in.tellg());
    }

    bool        more = true;
    std::string line;
    while (more && getline(ws(in), line))
    {
std::cout << "XBP: line[ " << line << " ]";
        if (line == "</" + tag + ">")
        {
std::cout << " is matching end tag\n";
            more = false;
        }
        else if (!line.size()
                 || line.find("<!--") != std::string::npos
                 || line[0] != '<')
        {
std::cout << " is comment, skipping...\n";
            // skip the comment
        }
        else if (line[0] != '<')
        {
std::cout << " is some content, skipping...\n";
        }
        else if (line.find("/>") == std::string::npos)
        {
            std::string tag = line.substr(1, std::string::npos);
            tag = tag.substr(0, tag.find(">"));
            tag = tag.substr(0, tag.find(" "));
std::cout << " is open element \""<<tag<<"\" ";

            // search for it in the blocks
            if (blocks.find(tag) != blocks.end())
            {
std::cout << "with match\n";
                blocks[tag]->parse(in, tag, info);
            }
            else
            {
std::cout << "without match, skipping contents\n";
                skipBlock(in);
                info.unknownChunks = true;
            }
        }
        else
        {
            // search for it in the elements
            line = line.substr(1, std::string::npos);
            const std::string name = line.substr(0, line.find(" "));
            std::string data;
            if (line.find("value=\""))
            {
                data = line.substr(line.find("value=\"")+7);
                data = data.substr(0, data.find("\""));
            }
std::cout << " is single element \""<<name<<"\" with data \""<<data<<"\" ";
            if (elements.find(name) != elements.end())
            {
std::cout << "with match\n";
                elements[name]->parse(data);
            }
            else if (catchAll)
            {
std::cout << "without match, calling catch all\n";
                catchAll->parse(line);
            }
            else
            {
std::cout << "without match\n";
                info.unknownData = true;
            }
        }
    }
std::cout << "XBP: end\n";
}


void TSE3::File::XmlBlockParser::skipBlock(std::istream &in)
{
    std::string open;
    getline(ws(in), open);
    int depth = 1;
    std::string line;
    do
    {
        getline(ws(in), line);
        if (line.find("</") == 0)
        {
            --depth;
        }
        else if (line[0] == '<'
                 && line[1] != '!'
                 && line[1] != '?'
                 && (line.find("/>") == std::string::npos))
        {
            ++depth;
        }
    }
    while (!in.eof() && depth);
}


/******************************************************************************
 * XmlElementParser class
 *****************************************************************************/

TSE3::File::XmlElementParser::~XmlElementParser()
{
}
