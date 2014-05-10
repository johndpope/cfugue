/*
 * @(#)PhraseList.cpp 3.00 26 May 1999
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

#include "tse3/PhraseList.h"

#include "tse3/Phrase.h"
#include "tse3/PhraseEdit.h"
#include "tse3/FileBlockParser.h"
#include "tse3/Error.h"
#include "tse3/Mutex.h"

#include <sstream>
#include <algorithm>

using namespace TSE3;

/******************************************************************************
 * PhraseList class
 *****************************************************************************/

const std::string TSE3::PhraseList::newPhraseString      = "Phrase";
const std::string TSE3::PhraseList::importedString       = "Imported Phrase";
const std::string TSE3::PhraseList::mergedPhraseString   = "Merged Phrase";
const std::string TSE3::PhraseList::explodedPhraseString = "Exploded Phrase";


PhraseList::PhraseList()
{
}


PhraseList::~PhraseList()
{
    while (size())
    {
        Phrase *p = *(list.begin());
        list.erase(list.begin());
        delete p;
    }
}


Phrase *PhraseList::phrase(const std::string &title) const
{
    Impl::CritSec cs;

    std::vector<Phrase*>::const_iterator i = list.begin();
    while (i != list.end() && (*i)->title() != title)
    {
        ++i;
    }
    return (i == list.end()) ? 0 : *i;
}


size_t PhraseList::index(const Phrase *phrase) const
{
    Impl::CritSec cs;

    std::vector<Phrase*>::const_iterator i = list.begin();
    while (i != list.end() && *i != phrase)
    {
        ++i;
    }
    return i - list.begin();
}


void PhraseList::insert(Phrase *phrase)
{
    Impl::CritSec cs;

    if (phrase->title().size() == 0)
    {
        throw PhraseListError(PhraseNameExistsErr);
    }
    if (phrase->parent())
    {
        throw PhraseListError(PhraseAlreadyInsertedErr);
    }

    insertInList(phrase);
    phrase->setParent(this);
    attachTo(phrase);

    notify(&PhraseListListener::PhraseList_Inserted, phrase);
}


void PhraseList::insertInList(Phrase *phrase)
{
    std::vector<Phrase*>::iterator i = list.begin();

    while (i != list.end() && **i < *phrase)
    {
        ++i;
    }
    if (i != list.end() && **i == *phrase)
    {
        throw PhraseListError(PhraseNameExistsErr);
    }

    list.insert(i, phrase);
}


void PhraseList::erase(Phrase *phrase)
{
    Impl::CritSec cs;

    std::vector<Phrase*>::iterator i = find(list.begin(), list.end(), phrase);
    if (i != list.end())
    {
        list.erase(i);
        phrase->setParent(0);
        detachFrom(phrase);

        notify(&PhraseListListener::PhraseList_Removed, phrase);

        delete phrase;
    }
}


void PhraseList::remove(Phrase *phrase)
{
    Impl::CritSec cs;

    std::vector<Phrase*>::iterator i = find(list.begin(), list.end(), phrase);
    if (i != list.end())
    {
        list.erase(i);
        phrase->setParent(0);
        detachFrom(phrase);

        notify(&PhraseListListener::PhraseList_Removed, phrase);
    }
}


std::string PhraseList::newPhraseTitle(const std::string &baseName)
{
    Impl::CritSec cs;

    if (!phrase(baseName)) return baseName;

    std::string title;
    int n = 1;
    do
    {
        std::ostringstream o;
        o << " " << n;
        title = baseName;
        title.append(o.str());
        ++n;
    }
    while (phrase(title));
    return title;
}


void PhraseList::Phrase_TitleAltered(Phrase *)
{
    // We can ignore this, since the phraseTitleChanged method *has*
    // already been called
}


void PhraseList::Notifier_Deleted(Phrase *phrase)
{
    Impl::CritSec cs;

    std::vector<Phrase*>::iterator i = find(list.begin(), list.end(), phrase);
    if (i == list.end()) return;
    list.erase(i);
    notify(&PhraseListListener::PhraseList_Removed, phrase);
}


void PhraseList::phraseTitleChanged(Phrase *phrase)
{
    // No need to claim mutex - it has already been claimed

    std::vector<Phrase*>::iterator i = find(list.begin(), list.end(), phrase);
    if (i != list.end())
    {
        list.erase(i);
        insertInList(phrase);
    }
}


/******************************************************************************
 * PhraseList Serializable interface
 *****************************************************************************/

void PhraseList::save(std::ostream &o, int in) const
{
    std::vector<Phrase*>::const_iterator i = list.begin();
    while (i != list.end())
    {
        o << indent(in) << "Phrase\n";
        (*i)->save(o, in);
        ++i;
    }
}


namespace
{
    /**
     * A catch-all FileItemParser to handle data items in the TempoTrack
     * Events sub-block.
     */
    class FileItemParser_Events : public FileItemParser
    {
        public:
            FileItemParser_Events(PhraseEdit *pe, int PPQN)
            : pe(pe), PPQN(PPQN) {}
            virtual ~FileItemParser_Events() {}
            virtual void parse(const std::string &line)
            {
                int time,    offTime    = 0;
                int status,  offStatus  = MidiCommand_Invalid;
                int data1,   offData1   = 0;
                int data2,   offData2   = 0;
                int channel, offChannel = 0;
                int port,    offPort    = 0;
                std::istringstream si(line);
                si >> time;
                si.ignore(1); // skip colon
                si >> status;
                si.ignore(1); // skip slash
                si >> data1;
                si.ignore(1); // skip slash
                si >> data2;
                si.ignore(1); // skip slash
                si >> channel;
                si.ignore(1); // skip slash
                si >> port;
                if (line.find("-")) {
                    si.ignore(1); // skip dash
                    si >> offTime;
                    si.ignore(1); // skip colon
                    si >> offStatus;
                    si.ignore(1); // skip slash
                    si >> offData1;
                    si.ignore(1); // skip slash
                    si >> offData2;
                    si.ignore(1); // skip slash
                    si >> offChannel;
                    si.ignore(1); // skip slash
                    si >> offPort;
                }
                time = Clock::convert(time, PPQN);
                MidiEvent e(MidiCommand(status, channel, port, data1, data2),
                            time,
                            MidiCommand(offStatus, offChannel, offPort,
                                        offData1, offData2),
                            offTime);
                pe->insert(e);
            }
        private:
            PhraseEdit *pe;
            int         PPQN;
    };

    /**
     * A simple Serializable class that creates a simple FileBlockParser
     * to load the Events sub-block of the TempoTrack block.
     */
    class Events : public Serializable
    {
        public:
            Events(PhraseList *pl)
            : pl(pl), _title("") {}
            virtual ~Events() {}
            void setTitle(const std::string &t)
            {
                _title = t;
            }
            const std::string &title() { return _title; }
            virtual void load(std::istream &in, SerializableLoadInfo &info)
            {
                FileItemParser_Events events(&pe, info.PPQN);
                FileBlockParser parser;
                parser.add(&events);
                parser.parse(in, info);
            }
            PhraseEdit    *phraseEdit()    { return &pe; }
            DisplayParams *displayParams() { return &dp; }
        private:
            PhraseEdit     pe;
            DisplayParams  dp;
            PhraseList    *pl;
            std::string    _title;
    };
}


void PhraseList::load(std::istream &in, SerializableLoadInfo &info)
{
    Events events(this);
    FileItemParser_String<Events> title(&events, &Events::setTitle);
    FileBlockParser parser;
    parser.add("Title",         &title);
    parser.add("DisplayParams",  events.displayParams());
    parser.add("Events",        &events);
    parser.parse(in, info);
    Phrase *phrase = events.phraseEdit()->createPhrase(this, events.title());
    if (phrase)
    {
        *(phrase->displayParams()) = *(events.displayParams());
    }
    else
    {
        std::cerr << "TSE3: Phrase creation error during load\n";
    }
}
