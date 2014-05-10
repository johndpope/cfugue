

#include "tse3/file/Write.h"

#include "tse3/file/XML.h"
#include "tse3/MidiParams.h"

void TSE3::File::write(XmlFileWriter &writer, TSE3::MidiParams &mp)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "MidiParams");

    writer.element("BankLSB", mp.bankLSB());
    writer.element("BankMSB", mp.bankMSB());
    writer.element("Program", mp.program());
    writer.element("Pan",     mp.pan());
    writer.element("Reverb",  mp.reverb());
    writer.element("Chorus",  mp.chorus());
    writer.element("Volume",  mp.volume());
}
