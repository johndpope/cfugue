/*
 * @(#)FlagTrack.cpp 3.00 7 June 1999
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

#include "tse3/FlagTrack.h"
#include "tse3/FileBlockParser.h"

#include <sstream>

using namespace TSE3;

/******************************************************************************
 * FlagTrackIterator class
 *****************************************************************************/

namespace TSE3
{
    /**
     * The @ref PlayableIterator for the @ref FlagTrack class.
     *
     * The @ref FlagTrack actually doesn't produce any MIDI output, so
     * you can count on this class producing no output.
     *
     * @short   FlagTrack PlayableIterator.
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     PlayableIterator
     * @see     FlagTrack
     */
    class FlagTrackIterator : public PlayableIterator
    {
        public:

            FlagTrackIterator(FlagTrack *t, Clock m);
            virtual ~FlagTrackIterator();
            virtual void moveTo(Clock m);

        protected:

            virtual void getNextEvent();

        private:

            FlagTrackIterator &operator=(const FlagTrackIterator &);
            FlagTrackIterator(const FlagTrackIterator &);
    };
}


FlagTrackIterator::FlagTrackIterator(FlagTrack * /*t*/, Clock /*c*/)
{
}


FlagTrackIterator::~FlagTrackIterator()
{
}


void FlagTrackIterator::moveTo(Clock /*c*/)
{
}


void FlagTrackIterator::getNextEvent()
{
    _more = false;
    _next = MidiEvent();
}


/******************************************************************************
 * FlagTrack class
 *****************************************************************************/

FlagTrack::FlagTrack()
{
}


FlagTrack::~FlagTrack()
{
}


PlayableIterator *FlagTrack::iterator(Clock index)
{
    return new FlagTrackIterator(this, index);
}


Clock FlagTrack::lastClock() const
{
    return 0;
}


/******************************************************************************
 * FlagTrack Serializable interface
 *****************************************************************************/

void FlagTrack::save(std::ostream &o, int i) const
{
    o << indent(i)   << "{\n";
    o << indent(i+1) << "Events\n";
    o << indent(i+1) << "{\n";
    for (size_t n = 0; n < size(); ++n)
    {
        o << indent(i+2) << data[n].time << ":" << data[n].data.title() << "\n";
    }
    o << indent(i+1) << "}\n";
    o << indent(i) << "}\n";
}


namespace
{
    /**
     * A catch-all FileItemParser to handle data items in the FlagTrack
     * Events sub-block.
     */
    class FileItemParser_Events : public FileItemParser
    {
        public:
            FileItemParser_Events(FlagTrack *ft, int PPQN)
            : ft(ft), PPQN(PPQN) {}
            virtual void parse(const std::string &line)
            {
                int time;
                std::istringstream si(line);
                si >> time;
                si.ignore(1); // skip colon
                std::string title;
                si >> title;
                time = Clock::convert(time, PPQN);
                ft->insert(Event<Flag>(Flag(title), time));
            }
        private:
            FlagTrack *ft;
            int        PPQN;
    };

    /**
     * A simple Serializable class that creates a simple FileBlockParser
     * to load the Events sub-block of the FlagTrack block.
     */
    class Events : public Serializable
    {
        public:
            Events(FlagTrack *ft)
            : ft(ft) {}
            virtual void load(std::istream &in, SerializableLoadInfo &info)
            {
                FileItemParser_Events events(ft, info.PPQN);
                FileBlockParser parser;
                parser.add(&events);
                parser.parse(in, info);
            }
        private:
            FlagTrack *ft;
    };
}


void FlagTrack::load(std::istream &in, SerializableLoadInfo &info)
{
    Events events(this);
    FileBlockParser parser;
    parser.add("Events", &events);
    parser.parse(in, info);
}
