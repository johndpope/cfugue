#include "tse3/file/Write.h"

#include "tse3/file/XML.h"
#include "tse3/Midi.h"

const int MajorVersion = 200;
const int MinorVersion = 0;
static const char *originator = "TSE3";

void TSE3::File::writeSong(XmlFileWriter &writer, TSE3::Song &song)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "TSE3");

    writer.element("Version-Major", MajorVersion);
    writer.element("Version-Minor", MinorVersion);
    writer.element("Originator",    originator);
    writer.element("PPQN",          Clock::PPQN);

    TSE3::File::write(writer, song);
}
