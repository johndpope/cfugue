

#include "tse3/file/Write.h"

#include "tse3/file/XML.h"
#include "tse3/DisplayParams.h"

#include <sstream>

void TSE3::File::write(XmlFileWriter &writer, TSE3::DisplayParams &dp)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "DisplayParams");

    writer.element("Style", dp.style());
    {
        // This will be a stringstream
        std::ostringstream ev;
        int r, g, b;
        dp.colour(r, g, b);
        ev << r << "," << g << "," << b;
        writer.element("Colour", ev.str());
    }
    if (dp.style() == TSE3::DisplayParams::PresetColour)
    {
        writer.element("Preset",
                       TSE3::DisplayParams::presetColourString(dp.presetColour()));
    }
}


