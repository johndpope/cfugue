

#include "tse3/file/Write.h"

#include "tse3/file/XML.h"
#include "tse3/TempoTrack.h"

#include <sstream>

void TSE3::File::write(XmlFileWriter &writer, TSE3::TempoTrack &tt)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "TempoTrack");

    writer.element("Status", tt.status());

    TSE3::File::XmlFileWriter::AutoElement ae2(writer, "Events");
    for (size_t n = 0; n < tt.size(); ++n)
    {
        // This will be a stringstream
        std::ostringstream ev;
        ev << tt[n].time << ":" << tt[n].data.tempo;
        writer.element("Event", ev.str());
    }
}
