tse3comh_HEADERS = Command.h CommandGroup.h CommandHistory.h FlagTrack.h Song.h Track.h Part.h Phrase.h
tse3comhdir = $(pkgincludedir)/cmd

noinst_LTLIBRARIES = libtse3cmd.la
libtse3cmd_la_SOURCES = CommandGroup.cpp CommandHistory.cpp FlagTrack.cpp Song.cpp Track.cpp Part.cpp Phrase.cpp

DISTCLEANFILES = ./.deps/* ./.deps/.P

INCLUDES = -I$(top_srcdir)/src

#noinst_PROGRAMS = test
#test_SOURCES = test.cpp
#test_LDFLAGS = -L$(top_builddir)/src/tse3 -L$(top_builddir)/src/tse3/commands
#test_LDADD = -ltse3 -ltse3_commands
