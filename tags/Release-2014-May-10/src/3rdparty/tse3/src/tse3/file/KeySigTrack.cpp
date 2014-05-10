

#include "tse3/file/Write.h"

#include "tse3/file/XML.h"
#include "tse3/KeySigTrack.h"

#include <sstream>

void TSE3::File::write(XmlFileWriter &writer, TSE3::KeySigTrack &kst)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "KeySigTrack");

    writer.element("Status", kst.status());

    TSE3::File::XmlFileWriter::AutoElement ae2(writer, "Events");
    for (size_t n = 0; n < kst.size(); ++n)
    {
        std::ostringstream ev;
        ev << kst[n].time << ":" << kst[n].data.incidentals
           << "/" << kst[n].data.type;
        writer.element("Event", ev.str());
    }
}
