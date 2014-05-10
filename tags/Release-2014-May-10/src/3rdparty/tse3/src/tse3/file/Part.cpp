

#include "tse3/file/Write.h"

#include "tse3/file/XML.h"
#include "tse3/Part.h"
#include "tse3/Phrase.h"

void TSE3::File::write(XmlFileWriter &writer, TSE3::Part &p)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "Part");

    write(writer, *p.filter());
    write(writer, *p.params());
    write(writer, *p.displayParams());
    if (p.phrase())
    {
        writer.element("Phrase", p.phrase()->title());
    }
    else
    {
        writer.element("Phrase", "");
    }
    writer.element("Start",  p.start());
    writer.element("End",    p.end());
    writer.element("Repeat", p.repeat());
}
