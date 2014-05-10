

#include "tse3/file/Write.h"

#include "tse3/file/XML.h"
#include "tse3/FlagTrack.h"

#include <sstream>

void TSE3::File::write(XmlFileWriter &writer, TSE3::FlagTrack &ft)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "FlagTrack");

    TSE3::File::XmlFileWriter::AutoElement ae2(writer, "Events");
    for (size_t n = 0; n < ft.size(); ++n)
    {
        // This will be a stringstream
        std::ostringstream ev;
        ev << ft[n].time << ":" << ft[n].data.title();
        writer.element("Event", ev.str());
    }
}
