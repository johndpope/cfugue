bin_PROGRAMS = tse3play

tse3play_SOURCES = tse3play.h tse3play.cpp main.cpp
man_MANS = tse3play.1
tse3play_LDADD = $(top_builddir)/src/tse3/libtse3.la

DISTCLEANFILES = ./.deps/* ./.deps/.P

EXTRA_DIST = tse3play.1

INCLUDES = -I$(top_srcdir)/src
