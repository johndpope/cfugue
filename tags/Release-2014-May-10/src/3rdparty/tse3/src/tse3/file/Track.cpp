

#include "tse3/file/Write.h"

#include "tse3/file/XML.h"
#include "tse3/Track.h"

void TSE3::File::write(XmlFileWriter &writer, TSE3::Track &t)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "Track");

    writer.element("Title", t.title());
    write(writer, *t.filter());
    write(writer, *t.params());
    write(writer, *t.displayParams());
    writer.element("NoParts", (int)t.size());

    for (size_t n = 0; n < t.size(); ++n)
    {
        write(writer, *t[n]);
    }
}
