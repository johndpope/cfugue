/*
 * @(#)Choices.cpp 3.00 25 May 1999
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

#include "tse3/app/Choices.h"

#include <fstream>
#include <algorithm>
#include <ctime>
#include <sstream>

#include "tse3/MidiScheduler.h"
#include "tse3/FileBlockParser.h"
#include "tse3/app/Application.h"
#include "tse3/Error.h"
#include "tse3/Metronome.h"
#include "tse3/Transport.h"
#include "tse3/ins/Destination.h"
#include "tse3/ins/Instrument.h"

using namespace TSE3;
using namespace TSE3::App;

/******************************************************************************
 * ChoiceHandler class
 *****************************************************************************/

ChoiceHandler::ChoiceHandler(const std::string &choicename)
: _name(choicename)
{
}


ChoiceHandler::~ChoiceHandler()
{
}


/******************************************************************************
 * ChoicesManager class
 *****************************************************************************/

ChoicesManager::ChoicesManager()
{
}


ChoicesManager::~ChoicesManager()
{
}


void ChoicesManager::save(const std::string &filename)
{
    std::ofstream out(filename.c_str());
    if (!out)
    {
        std::cerr << "TSE3: Couldn't save application choices to '"
                  << filename
                  << "'.\n";
    }
    out << "TSE3MDL\n"
        << "# This is an automatically generated file containing choices for\n"
        << "# applications that use the TSE3 library (available from\n"
        << "# <http://TSE3.sourceforge.net/>).\n"
        << "# You shouldn't need to edit this file by hand.\n"
        << "{\n"
        << "    Choices\n";
    handler.save(out, 1);
    out << "}\n";
}


void ChoicesManager::load(const std::string &filename)
{
    std::ifstream in(filename.c_str());
    if (!in)
    {
        std::cerr << "TSE3: Couldn't load application choices from '"
                  << filename
                  << "'.\n";
        return;
    }

    // Check first line reads TSE3MDL
    std::string tse3mdl;
    getline(in, tse3mdl);
    if (tse3mdl != "TSE3MDL")
    {
        std::cerr << "TSE3: " << filename
                  << " is not a TSE3MDL choices file.\n";
        return;
    }

    SerializableLoadInfo info;

    // Now scan the file contents.
    FileBlockParser parser;
    parser.add("Choices", &handler);
    try
    {
        parser.parse(in, info);
    }
    catch (const TSE3::Error &e)
    {
        std::cerr << "TSE3: Failed to parse choices file\n";
    }

    if (info.noChunks == 0)
    {
        std::cerr << "TSE3: Choices file contained no choices\n";
    }

    in.close();
}


/******************************************************************************
 * ChoicesManager::ChoicesChoiceHandler class
 *****************************************************************************/

ChoicesManager::ChoicesChoiceHandler::ChoicesChoiceHandler()
: ChoiceHandler("Choices")
{
}


ChoicesManager::ChoicesChoiceHandler::~ChoicesChoiceHandler()
{
    while (handlers.size())
    {
        delete *(handlers.begin());
        handlers.remove(*(handlers.begin()));
    }
}


void ChoicesManager::ChoicesChoiceHandler::add(ChoiceHandler *ch)
{
    handlers.push_back(ch);
}


void ChoicesManager::ChoicesChoiceHandler::remove(ChoiceHandler *ch)
{
    handlers.remove(ch);
}


void ChoicesManager::ChoicesChoiceHandler::save(std::ostream &out, int ind)
    const
{
    out << indent(ind) << "{\n";
    std::list<ChoiceHandler*>::const_iterator i = handlers.begin();
    while (i != handlers.end())
    {
        out << indent(ind+1) << (*i)->name() << "\n";
        (*i)->save(out, ind+1);
        ++i;
    }
    out << indent(ind) << "}\n";
}


void ChoicesManager::ChoicesChoiceHandler::load(std::istream &in,
                                                SerializableLoadInfo &info)
{
    FileBlockParser parser;
    std::list<ChoiceHandler*>::iterator i = handlers.begin();
    while (i != handlers.end())
    {
        parser.add((*i)->name(), *i);
        ++i;
    }
    parser.parse(in, info);
}


/******************************************************************************
 * ApplicationChoiceHandler class
 *****************************************************************************/

ApplicationChoiceHandler::ApplicationChoiceHandler(Application *a)
: ChoiceHandler("Application"), a(a)
{
}


ApplicationChoiceHandler::~ApplicationChoiceHandler()
{
}


void ApplicationChoiceHandler::save(std::ostream &out, int i) const
{
    std::time_t time = std::time(0);
    out << indent(i)   << "{\n"
        << indent(i+1) << "AppName:"    << a->appName().c_str()    << "\n"
        << indent(i+1) << "AppVersion:" << a->appVersion().c_str() << "\n";
    if (time != time_t(-1))
    {
        const char *days[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri" };
        std::tm *gt = std::gmtime(&time);
        out << indent(i+1) << "ThisFileCreatedOn:" << 1900+gt->tm_year << "-"
            << gt->tm_mon+1 << "-" << gt->tm_mday << "-"
            << gt->tm_hour << "-" << gt->tm_min << "-" << gt->tm_sec
            << " (" << days[gt->tm_wday] << ")\n";
    }
    out << indent(i+1) << "SaveChoicesOnDestroy:";
    if (a->saveChoicesOnDestroy()) out << "Yes\n"; else out << "No\n";
    out << indent(i)   << "}\n";
}


void ApplicationChoiceHandler::load(std::istream &in,
                                    SerializableLoadInfo &info)
{
    FileItemParser_OnOff<Application>
        scod(a, &Application::setSaveChoicesOnDestroy);
    FileBlockParser parser;
    parser.add("SaveChoicesOnDestroy", &scod);
    parser.parse(in, info);
}


/******************************************************************************
 * MetronomeChoiceHandler class
 *****************************************************************************/

MetronomeChoiceHandler::MetronomeChoiceHandler(Metronome *m)
: ChoiceHandler("Metronome"), m(m)
{
}


MetronomeChoiceHandler::~MetronomeChoiceHandler()
{
}


void MetronomeChoiceHandler::save(std::ostream &out, int i) const
{
    out << indent(i)   << "{\n";
    out << indent(i+1) << "Channel:"         << m->channel()      << "\n";
    out << indent(i+1) << "Port:"            << m->port()         << "\n";
    out << indent(i+1) << "Duration:"        << m->duration()     << "\n";
    out << indent(i+1) << "BarNote:"         << m->barNote()      << "\n";
    out << indent(i+1) << "BarVelocity:"     << m->barVelocity()  << "\n";
    out << indent(i+1) << "BeatNote:"        << m->beatNote()     << "\n";
    out << indent(i+1) << "BeatVelocity:"    << m->beatVelocity() << "\n";
    out << indent(i+1) << "PlayingStatus:";
    if (m->status(Transport::Playing)) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "RecordingStatus:";
    if (m->status(Transport::Recording)) out << "On\n"; else out << "Off\n";
    out << indent(i)   << "}\n";
}


void MetronomeChoiceHandler::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_Number<Metronome> channel(m, &Metronome::setChannel);
    FileItemParser_Number<Metronome> port(m, &Metronome::setPort);
    FileItemParser_Number<Metronome> duration(m, &Metronome::setDuration);
    FileItemParser_Number<Metronome> barNote(m, &Metronome::setBarNote);
    FileItemParser_Number<Metronome> barVelocity(m, &Metronome::setBarVelocity);
    FileItemParser_Number<Metronome> beatNote(m, &Metronome::setBeatNote);
    FileItemParser_Number<Metronome> beatVelocity(m, &Metronome::setBeatVelocity);
    FileItemParser_ReasonOnOff<Metronome, int>
        playing(m, &Metronome::setStatus, Transport::Playing);
    FileItemParser_ReasonOnOff<Metronome, int>
        recording(m, &Metronome::setStatus, Transport::Recording);
    FileBlockParser parser;
    parser.add("Channel",         &channel);
    parser.add("Port",            &port);
    parser.add("Duration",        &duration);
    parser.add("BarNote",         &barNote);
    parser.add("BarVelocity",     &barVelocity);
    parser.add("BeatNote",        &beatNote);
    parser.add("BeatVelocity",    &beatVelocity);
    parser.add("PlayingStatus",   &playing);
    parser.add("RecordingStatus", &recording);
    parser.parse(in, info);
}


/******************************************************************************
 * TransportChoiceHandler class
 *****************************************************************************/

TransportChoiceHandler::TransportChoiceHandler(Transport *t)
: ChoiceHandler("Transport"), t(t),
  startPanicHandler(t->startPanic()),
  endPanicHandler(t->endPanic()),
  mapperHandler(t->midiMapper())
{
}


TransportChoiceHandler::~TransportChoiceHandler()
{
}


void TransportChoiceHandler::save(std::ostream &out, int i) const
{
    out << indent(i)   << "{\n";
    out << indent(i+1) << "Synchro:";
    if (t->synchro()) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "PuchIn:";
    if (t->punchIn()) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "AutoStop:";
    if (t->autoStop()) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "StartPanic\n";
    startPanicHandler.save(out, i+1);
    out << indent(i+1) << "EndPanic\n";
    endPanicHandler.save(out, i+1);
    out << indent(i+1) << "MidiMapper\n";
    mapperHandler.save(out, i+1);
    out << indent(i)   << "}\n";
}


void TransportChoiceHandler::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_OnOff<Transport> synchro(t, &Transport::setSynchro);
    FileItemParser_OnOff<Transport> punchIn(t, &Transport::setPunchIn);
    FileItemParser_OnOff<Transport> autoStop(t, &Transport::setAutoStop);
    PanicChoiceHandler      startPanic(t->startPanic());
    PanicChoiceHandler      endPanic(t->endPanic());
    MidiMapperChoiceHandler mapper(t->midiMapper());
    FileBlockParser parser;
    parser.add("Synchro",    &synchro);
    parser.add("PunchIn",    &punchIn);
    parser.add("AutoStop",   &autoStop);
    parser.add("StartPanic", &startPanic);
    parser.add("EndPanic",   &endPanic);
    parser.add("MidiMapper", &mapper);
    parser.parse(in, info);
}


/******************************************************************************
 * PanicChoiceHandler class
 *****************************************************************************/

PanicChoiceHandler::PanicChoiceHandler(Panic *p)
: ChoiceHandler("Panic"), p(p)
{
}


PanicChoiceHandler::~PanicChoiceHandler()
{
}


void PanicChoiceHandler::save(std::ostream &out, int i) const
{
    out << indent(i)   << "{\n";
    out << indent(i+1) << "Status:";
    if (p->status()) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "MidiReset:";
    if (p->midiReset()) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "GmReset:";
    if (p->gmReset()) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "GsReset:";
    if (p->gsReset()) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "XgReset:";
    if (p->xgReset()) out << "On\n"; else out << "Off\n";
    int gsIDMask = 0;
    for (int n = 0; n < 32; ++n) if (p->gsIDMask(n)) gsIDMask |= 1 << n;
    out << indent(i+1) << "GsIDMask:" << std::hex << gsIDMask
        << std::dec << "\n";
    int xgIDMask = 0;
    for (int n = 0; n < 16; ++n) if (p->xgIDMask(n)) xgIDMask |= 1 << n;
    out << indent(i+1) << "XgIDMask:" << std::hex << xgIDMask
        << std::dec << "\n";
    out << indent(i+1) << "AllNotesOff:";
    if (p->allNotesOff()) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "AllNotesOffMan:";
    if (p->allNotesOffManually()) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "AllModsOff:";
    if (p->allModsOff()) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "AllPitchOff:";
    if (p->allPitchOff()) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "AllCtrlOff:";
    if (p->allCtrlOff()) out << "On\n"; else out << "Off\n";
    out << indent(i+1) << "LiftSustain:";
    if (p->liftSustain()) out << "On\n"; else out << "Off\n";
    out << indent(i)   << "}\n";
}


namespace
{
    /**
     * A utility class implementing a specific type of FileItemParser.
     * This class will call a member function with signature
     * void setXXX(int maskNo, bool) with mask values from 0 to maskSize-1
     * and values of bool based on a hexidecimal mask string representing
     * a bitfield where bit 0 corresponds to maskNo 0.
     *
     * @short   Internal utility for parsing bitfield data lines
     * @author  Pete Goodliffe
     * @version 1.00
     */
    template <class T>
    class FileItemParser_Mask : public FileItemParser
    {
        public:
            typedef void (T::*fn_t)(size_t mask, bool);
            FileItemParser_Mask(T *obj, fn_t mfun, int maskSize)
            : obj(obj), mfun(mfun), maskSize(maskSize) {}
            void parse(const std::string &data)
            {
                size_t mask;
                std::stringstream si(data);
                si >> std::hex >> mask;
                for (size_t n = 0; n < maskSize; ++n)
                    (obj->*mfun)(n, mask&(1<<n));
            }
        private:
            T      *obj;
            fn_t    mfun;
            size_t  maskSize;
    };
}


void PanicChoiceHandler::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_OnOff<Panic> status(p, &Panic::setStatus);
    FileItemParser_OnOff<Panic> midi(p, &Panic::setMidiReset);
    FileItemParser_OnOff<Panic> gm(p, &Panic::setGmReset);
    FileItemParser_OnOff<Panic> gs(p, &Panic::setGsReset);
    FileItemParser_OnOff<Panic> xg(p, &Panic::setXgReset);
    FileItemParser_OnOff<Panic> notes(p, &Panic::setAllNotesOff);
    FileItemParser_OnOff<Panic> notesMan(p, &Panic::setAllNotesOffManually);
    FileItemParser_OnOff<Panic> mods(p, &Panic::setAllModsOff);
    FileItemParser_OnOff<Panic> pitch(p, &Panic::setAllPitchOff);
    FileItemParser_OnOff<Panic> ctrl(p, &Panic::setAllCtrlOff);
    FileItemParser_OnOff<Panic> sustain(p, &Panic::setLiftSustain);
    FileItemParser_Mask<Panic>  gsMask(p, &Panic::setGsIDMask, 32);
    FileItemParser_Mask<Panic>  xgMask(p, &Panic::setXgIDMask, 16);
    FileBlockParser parser;
    parser.add("Status",         &status);
    parser.add("MidiReset",      &midi);
    parser.add("GmReset",        &gm);
    parser.add("GsReset",        &gs);
    parser.add("GsIDMask",       &gsMask);
    parser.add("XgReset",        &xg);
    parser.add("XgIDMask",       &xgMask);
    parser.add("AllNotesOff",    &notes);
    parser.add("AllNotesOffMan", &notesMan);
    parser.add("AllModsOff",     &mods);
    parser.add("AllPitchOff",    &pitch);
    parser.add("AllCtrlOff",     &ctrl);
    parser.add("LiftSustain",    &sustain);
    parser.parse(in, info);
}


/******************************************************************************
 * MidiMapperChoiceHandler class
 *****************************************************************************/

MidiMapperChoiceHandler::MidiMapperChoiceHandler(MidiMapper *m)
: ChoiceHandler("MidiMapper"), m(m)
{
}


MidiMapperChoiceHandler::~MidiMapperChoiceHandler()
{
}


void MidiMapperChoiceHandler::save(std::ostream &out, int i) const
{
    out << indent(i)   << "{\n";
    out << indent(i+1) << "MaximumMap:" << m->maximumMap() << "\n";
    for (int port = 0; port < m->maximumMap(); ++port)
    {
        int map = m->map(port);
        out << indent(i+1) << "Map:" << port << "," << map << "\n";
    }
    out << indent(i)   << "}\n";
}


namespace
{
    /**
     * A utility class implementing a specific type of FileItemParser.
     * It interprets the Map lines saved by the MidiMapperChoiceHandler.
     *
     * @short   Internal utility for parsing MidiMapper block items.
     * @author  Pete Goodliffe
     * @version 1.00
     */
    class FileItemParser_Map : public FileItemParser
    {
        public:
            FileItemParser_Map(MidiMapper *obj)
            : obj(obj) {}
            void parse(const std::string &data)
            {
                if (data.substr(0, 8) == "MaximumMap:")
                {
                    int i;
                    std::istringstream si(data.c_str()+8);
                    si >> i;
                }
                else if (data.substr(0, 4) == "Map:")
                {
                    int fromPort, toPort;
                    std::istringstream si(data.c_str()+4);
                    si >> fromPort;
                    si.ignore(1);
                    si >> toPort;
                    obj->setMap(fromPort, toPort);
                }
            }
        private:
            MidiMapper *obj;
    };
}


void MidiMapperChoiceHandler::load(std::istream &in, SerializableLoadInfo &info)
{
    m->reset();
    FileBlockParser parser;
    FileItemParser_Map itemParser(m);
    parser.add(&itemParser);
    parser.parse(in, info);
}


/******************************************************************************
 * DestinationChoiceHandler class
 *****************************************************************************/

DestinationChoiceHandler::DestinationChoiceHandler(TSE3::Ins::Destination *d,
                                                   TSE3::MidiScheduler    *ms)
: ChoiceHandler("Destination"), d(d), ms(ms)
{
}


DestinationChoiceHandler::~DestinationChoiceHandler()
{
}


void DestinationChoiceHandler::save(std::ostream &out, int i) const
{
    out << indent(i)   << "{\n";
    out << indent(i+1) << "NoInstruments:" << d->numInstruments() << "\n";
    for (size_t ins = 0; ins < d->numInstruments(); ++ins)
    {
        out << indent(i+1) << "Instrument\n"
            << indent(i+1) << "{\n"
            << indent(i+2) << "Title:"    << d->instrument(ins)->title()
            << "\n"
            << indent(i+2) << "Filename:" << d->instrument(ins)->filename()
            << "\n"
            << indent(i+1) << "}\n";
    }
    for (size_t port = 0; port < ms->numPorts(); ++port)
    {
        out << indent(i+1) << "AllChannels:" << port << ",";
        if (d->allChannels(port))
        {
            out << "Yes\n";
            TSE3::Ins::Instrument *ins = d->port(port);
            out << indent(i+1) << "AllChannelsPort:" << port << ",";
            if (ins)
            {
                out << ins->title() << "\n";
            }
            else
            {
                out << "\n";
            }
        }
        else
        {
            out << "No\n";
            for (size_t ch = 0; ch < 16; ++ch)
            {
                if (d->channel(port, ch))
                {
                    out << indent(i+1) << "Channel:" << port << "," << ch << ","
                        << d->channel(port, ch)->title() << "\n";
                }
            }
        }
    }
    out << indent(i)   << "}\n";
}


namespace
{
    /**
     * Loads the Destination "AllChannels" data item.
     */
    class FileItemParser_AllChannels : public FileItemParser
    {
        public:
            FileItemParser_AllChannels(TSE3::Ins::Destination *d) : d(d) {}
            void parse(const std::string &data)
            {
                size_t channel;
                std::istringstream si(data);
                si >> channel;
                si.ignore(1);
                std::string s;
                getline(si ,s);
                d->setPort(channel, d->instrument(s));
            }
        private:
            TSE3::Ins::Destination *d;
    };

    /**
     * Loads the Destination "Channel" data item.
     */
    class FileItemParser_Channel : public FileItemParser
    {
        public:
            FileItemParser_Channel(TSE3::Ins::Destination *d) : d(d) {}
            void parse(const std::string &data)
            {
                size_t port, channel;
                std::istringstream si(data);
                si >> port;
                si.ignore(1);
                si >> channel;
                si.ignore(1);
                std::string s;
                getline(si ,s);
                d->setChannel(port, channel, d->instrument(s));
            }
        private:
            TSE3::Ins::Destination *d;
    };

    class InstrumentLoader : public Serializable
    {
        public:
            InstrumentLoader(Ins::Destination *d) : d(d) {}
            virtual void load(std::istream &in, SerializableLoadInfo &info);
            void setTitle(const std::string &t) { title = t; }
            void setFilename(const std::string &f) { filename = f; }
        private:
            TSE3::Ins::Destination *d;
            std::string             title;
            std::string             filename;
    };
            void InstrumentLoader::load(std::istream &in,
                                        SerializableLoadInfo &info)
            {
                FileItemParser_String<InstrumentLoader>
                    t(this, &InstrumentLoader::setTitle);
                FileItemParser_String<InstrumentLoader>
                    f(this, &InstrumentLoader::setFilename);
                FileBlockParser parser;
                parser.add("Title",    &t);
                parser.add("Filename", &f);
                parser.parse(in, info);

                if (!title.empty() && !filename.empty())
                {
                    Ins::Instrument *ins = new Ins::Instrument(title, filename);
                    d->addInstrument(ins);
                }
            }
}


void DestinationChoiceHandler::load(std::istream &in,
                                    SerializableLoadInfo &info)
{
    // clear all instruments
    if (d->numInstruments())
    {
        // XXX clean instruments
    }

    FileItemParser_AllChannels ac(d);
    FileItemParser_Channel     c(d);
    InstrumentLoader           ins(d);
    FileBlockParser            parser;
    parser.add("AllChannelsPort", &ac);
    parser.add("Channel",       &c);
    parser.add("Instrument",      &ins);
    parser.parse(in, info);
}

