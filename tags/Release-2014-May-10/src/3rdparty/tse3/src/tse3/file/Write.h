
#ifndef TSE3_FILE_SONG_H
#define TSE3_FILE_SONG_H

namespace TSE3
{
    class Song;
    class TempoTrack;
    class TimeSigTrack;
    class KeySigTrack;
    class FlagTrack;
    class PhraseList;
    class Phrase;
    class DisplayParams;
    class Track;
    class MidiFilter;
    class MidiParams;
    class Part;

    namespace File
    {
        class XmlFileWriter;

        void writeSong(XmlFileWriter &writer, TSE3::Song &song);

        void write(XmlFileWriter &writer, TSE3::Song          &song);
        void write(XmlFileWriter &writer, TSE3::TempoTrack    &tempoTrack);
        void write(XmlFileWriter &writer, TSE3::TimeSigTrack  &timeSigTrack);
        void write(XmlFileWriter &writer, TSE3::KeySigTrack   &keySigTrack);
        void write(XmlFileWriter &writer, TSE3::FlagTrack     &flagTrack);
        void write(XmlFileWriter &writer, TSE3::PhraseList    &phraseList);
        void write(XmlFileWriter &writer, TSE3::Phrase        &phrase);
        void write(XmlFileWriter &writer, TSE3::DisplayParams &displayParams);
        void write(XmlFileWriter &writer, TSE3::Track         &track);
        void write(XmlFileWriter &writer, TSE3::MidiFilter    &midiFilter);
        void write(XmlFileWriter &writer, TSE3::MidiParams    &midiParams);
        void write(XmlFileWriter &writer, TSE3::Part          &part);
    }
}

#endif
