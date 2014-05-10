

#include "tse3/file/Write.h"

#include "tse3/file/XML.h"
#include "tse3/TimeSigTrack.h"

#include <sstream>

void TSE3::File::write(XmlFileWriter &writer, TSE3::TimeSigTrack &tst)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "TimeSigTrack");

    writer.element("Status", tst.status());

    TSE3::File::XmlFileWriter::AutoElement ae2(writer, "Events");
    for (size_t n = 0; n < tst.size(); ++n)
    {
        // This will be a stringstream
        std::ostringstream ev;
        ev << tst[n].time << ":" << tst[n].data.top
           << "/" << tst[n].data.bottom;
        writer.element("Event", ev.str());
    }
}
