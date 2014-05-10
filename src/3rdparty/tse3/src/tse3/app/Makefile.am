tse3apph_HEADERS = Application.h Choices.h Modified.h PartSelection.h PartDisplay.h Record.h TrackSelection.h
tse3apphdir = $(pkgincludedir)/app

noinst_LTLIBRARIES = libtse3app.la
libtse3app_la_SOURCES = Application.cpp Choices.cpp Modified.cpp PartSelection.cpp PartDisplay.cpp Record.cpp TrackSelection.cpp

DISTCLEANFILES = ./.deps/* ./.deps/.P

INCLUDES = -I$(top_srcdir)/src

#noinst_PROGRAMS = test
#test_SOURCES = test.cpp
#test_LDFLAGS = -L$(top_builddir)/src/tse3 -L$(top_builddir)/src/tse3/platform -L$(top_builddir)/src/tse3/application
#test_LDADD = -ltse3 -ltse3_OSS -ltse3_application
