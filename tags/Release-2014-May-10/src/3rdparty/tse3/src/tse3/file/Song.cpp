
#include "tse3/file/Write.h"

#include "tse3/file/XML.h"
#include "tse3/Song.h"



void TSE3::File::write(XmlFileWriter &writer, TSE3::Song &song)
{
    TSE3::File::XmlFileWriter::AutoElement ae(writer, "Song");

    writer.comment("General information");
    writer.element("Title",     song.title());
    writer.element("Author",    song.author());
    writer.element("Copyright", song.copyright());
    writer.element("Date",      song.date());
    writer.element("NoTracks",  (int)song.size());

    writer.comment("Master tracks");
    write(writer, *song.tempoTrack());
    write(writer, *song.timeSigTrack());
    write(writer, *song.keySigTrack());
    write(writer, *song.flagTrack());

    writer.comment("Playback information");
    writer.element("SoloTrack", song.soloTrack());
    writer.element("Repeat",    song.repeat());
    writer.element("From",      song.from());
    writer.element("To",        song.to());

    writer.comment("Phrase information");
    write(writer, *song.phraseList());

    writer.comment("Track information");
    for (size_t n = 0; n < song.size(); ++n)
    {
        write(writer, *song[n]);
    }
}
