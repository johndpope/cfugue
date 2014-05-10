tse3h_HEADERS = DisplayParams.h Error.h EventTrack.h FileBlockParser.h Filter.h TSE2MDL.h FlagTrack.h KeySigTrack.h Metronome.h Midi.h MidiCommandFilter.h MidiData.h MidiEcho.h MidiFile.h MidiFilter.h MidiMapper.h MidiParams.h Mixer.h Mutex.h Notifier.h Panic.h Part.h Phrase.h PhraseEdit.h PhraseList.h Playable.h Progress.h RepeatTrack.h MidiScheduler.h Serializable.h Song.h TempoTrack.h TimeSigTrack.h Track.h Transport.h TSE3.h TSE3MDL.h
tse3hdir = $(pkgincludedir)

lib_LTLIBRARIES = libtse3.la

libtse3_la_SOURCES = DisplayParams.cpp Error.cpp FileBlockParser.cpp Filter.cpp TSE2MDL.cpp FlagTrack.cpp KeySigTrack.cpp Metronome.cpp Midi.cpp MidiCommandFilter.cpp MidiData.cpp MidiEcho.cpp MidiFile.cpp MidiFilter.cpp MidiMapper.cpp MidiParams.cpp Mixer.cpp Mutex.cpp Notifier.cpp Panic.cpp Part.cpp Phrase.cpp PhraseEdit.cpp PhraseList.cpp Playable.cpp RepeatTrack.cpp MidiScheduler.cpp Serializable.cpp Song.cpp TempoTrack.cpp TimeSigTrack.cpp Track.cpp Transport.cpp TSE3.cpp TSE3MDL.cpp
man_MANS = tse3.3

EXTRA_DIST = tse3.3

#libtse3_la_CXXFLAGS =
#libtse3_la_LDFLAGS = --strip-unneeded
libtse3_la_LIBADD  =     \
	./app/libtse3app.la  \
	./cmd/libtse3cmd.la  \
	./file/libtse3file.la  \
	./ins/libtse3ins.la  \
	./plt/libtse3plt.la  \
	./util/libtse3util.la

DISTCLEANFILES = ./.deps/* ./.deps/.P
    
INCLUDES = -I$(top_srcdir)/src
SUBDIRS = util cmd plt app ins listen file

# Documentation building

KDOCDIR = $(top_builddir)/doc/api

tse3-kdoc-all: tse3-kdoc tse3-kdoc-libs
tse3-kdoc:
	@echo "**** Building TSE3 library documentation in 'doc/api' dir ****"
	mkdir -p $(KDOCDIR)
	kdoc --strip-h-path -d $(KDOCDIR) --name TSE3 --html-logo tse3logo.png $(top_srcdir)/src/tse3/*.h $(top_srcdir)/src/tse3/app/*.h $(top_srcdir)/src/tse3/cmd/*.h $(top_srcdir)/src/tse3/ins/*.h $(top_srcdir)/src/tse3/listen/*.h $(top_srcdir)/src/tse3/plt/*.h $(top_srcdir)/src/tse3/util/*.h
	cp $(top_srcdir)/doc/src/tse3logo.png.verbatim.in $(KDOCDIR)/tse3logo.png

tse3-kdoc-libs:
	@echo "**** Building TSE3 separate library documentation in 'doc/api/lib.*' dirs ****"
	mkdir -p $(KDOCDIR)
	kdoc --strip-h-path -d $(KDOCDIR)/lib.TSE3/ -n "TSE3 Namespace" --html-logo ../tse3logo.png $(top_srcdir)/src/tse3/*.h $(top_srcdir)/src/tse3/listen/*.h --libdir $(KDOCDIR)/.kdoc
	kdoc --strip-h-path -d $(KDOCDIR)/lib.Ins/ -n "Ins Namespace" --html-logo ../tse3logo.png $(top_srcdir)/src/tse3/ins/*.h --libdir $(KDOCDIR)/.kdoc --xref TSE3_namespace
	kdoc --strip-h-path -d $(KDOCDIR)/lib.Plt/ -n "Plt Namespace" --html-logo ../tse3logo.png $(top_srcdir)/src/tse3/plt/*.h --libdir $(KDOCDIR)/.kdoc --xref TSE3_namespace
	kdoc --strip-h-path -d $(KDOCDIR)/lib.Util/ -n "Util Namespace" --html-logo ../tse3logo.png $(top_srcdir)/src/tse3/util/*.h --libdir $(KDOCDIR)/.kdoc --xref TSE3_namespace
	kdoc --strip-h-path -d $(KDOCDIR)/lib.Cmd/ -n "Cmd Namespace" --html-logo ../tse3logo.png $(top_srcdir)/src/tse3/cmd/*.h --libdir $(KDOCDIR)/.kdoc --xref TSE3_namespace --xref Util_namespace
	kdoc --strip-h-path -d $(KDOCDIR)/lib.App/ -n "App Namespace" --html-logo ../tse3logo.png $(top_srcdir)/src/tse3/app/*.h --libdir $(KDOCDIR)/.kdoc --xref TSE3_namespace --xref Cmd_namespace --xref Ins_namespace --xref Util_namespace
	cp $(top_srcdir)/doc/src/tse3logo.png.verbatim.in $(KDOCDIR)/tse3logo.png

tse3-kdoc-clean:
	@echo "**** Cleaning TSE3 library documentation from 'doc/api' dir ****"
	rm -rf $(KDOCDIR)

# Test harnesses

noinst_PROGRAMS = test
test_SOURCES = test.cpp
test_LDADD = $(top_builddir)/src/tse3/libtse3.la

# McCabe .i file generation

tse3_i_FILES = $(patsubst %.cpp,i/%.i,$(libtse3_la_SOURCES))

tse3-i: $(tse3_i_FILES)
	@echo "**** Built TSE3 .i files in 'i' dir ****"

i/%.i: %.cpp
	@mkdir -p i
	gcc -E -C $(CXXFLAGS) $(INCLUDES) $< > $@
