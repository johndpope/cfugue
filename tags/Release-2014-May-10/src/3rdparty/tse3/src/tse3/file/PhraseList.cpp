

#include "tse3/file/Write.h"

#include "tse3/file/XML.h"
#include "tse3/PhraseList.h"
#include "tse3/Phrase.h"

#include <sstream>

void TSE3::File::write(XmlFileWriter &writer, TSE3::PhraseList &pl)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "PhraseList");

    for (size_t n = 0; n < pl.size(); ++n)
    {
        write(writer, *pl[n]);
    }
}


void TSE3::File::write(XmlFileWriter &writer, TSE3::Phrase &p)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "Phrase");

    writer.element("Title", p.title());
    write(writer, *p.displayParams());

    TSE3::File::XmlFileWriter::AutoElement ae2(writer, "Events");
    for (size_t n = 0; n < p.size(); ++n)
    {
        // This will be a stringstream
        std::ostringstream ev;
        ev << p[n].time << ":"
           << p[n].data.status << "/"
           << p[n].data.data1 << "/"
           << p[n].data.data2 << "/"
           << p[n].data.channel << "/"
           << p[n].data.port;
        if (p[n].data.status == MidiCommand_NoteOn)
        {
            ev << "-"
               << p[n].offTime << ":"
               << p[n].offData.status << "/"
               << p[n].offData.data1 << "/"
               << p[n].offData.data2 << "/"
               << p[n].offData.channel << "/"
               << p[n].offData.port;
        }
        writer.element("Event", ev.str());
    }
}
