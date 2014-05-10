

#include "tse3/file/Write.h"

#include "tse3/file/XML.h"
#include "tse3/MidiFilter.h"

void TSE3::File::write(XmlFileWriter &writer, TSE3::MidiFilter &mf)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "MidiFilter");

    writer.element("Status",        mf.status());
    unsigned int c_filter = 0, p_filter = 0;
    for (int n = 0; n < 16; n++) if (mf.channelFilter(n)) c_filter |= (1<<n);
    writer.element("ChannelFilter", c_filter);
    writer.element("PortFilter",    p_filter);
    writer.element("Channel",       mf.channel());
    writer.element("Port",          mf.port());
    writer.element("Offset",        mf.offset());
    writer.element("TimeScale",     mf.timeScale());
    writer.element("Quantise",      mf.quantise());
    writer.element("Transpose",     mf.transpose());
    writer.element("MinVelocity",   mf.minVelocity());
    writer.element("MaxVelocity",   mf.maxVelocity());
    writer.element("VelocityScale", mf.velocityScale());
}
