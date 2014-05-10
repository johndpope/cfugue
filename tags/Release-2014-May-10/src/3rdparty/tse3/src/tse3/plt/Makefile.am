tse3plth_HEADERS = Alsa.h OSS.h RiscOS.h Factory.h Win32.h Arts.h
tse3plth_HEADERS += RiscOS.cpp Win32.cpp
tse3plthdir = $(pkgincludedir)/plt

noinst_LTLIBRARIES = libtse3plt.la
libtse3plt_la_SOURCES = OSS.cpp

EXTRA_DIST = Arts.cpp Win32.cpp RiscOS.cpp Factory_Unix.cpp Factory_Win32.cpp Alsa-0.5.cpp Alsa-0.9.cpp midiswis.h

DISTCLEANFILES = ./.deps/* ./.deps/.P

INCLUDES = -I$(top_srcdir)/src

###############################################################################
# Conditional on what interface support is being built in...

# Factory

if TSE3_WITH_WIN32
libtse3plt_la_SOURCES_factory = Factory_Win32.cpp
else
libtse3plt_la_SOURCES_factory = Factory_Unix.cpp
endif
libtse3plt_la_SOURCES += $(libtse3plt_la_SOURCES_factory)

# Alsa

if TSE3_WITH_ALSA
if TSE3_WITH_ALSA_0_5_X
libtse3plt_la_SOURCES_alsa  = Alsa-0.5.cpp
else
libtse3plt_la_SOURCES_alsa  = Alsa-0.9.cpp
endif
libtse3plt_la_LIBADD_alsa   = $(LIBASOUND)
else
libtse3plt_la_SOURCES_alsa  =
libtse3plt_la_LIBADD_alsa   =
endif

# Arts

if TSE3_WITH_ARTS
tse3plth_HEADERS_arts       = Arts.h
libtse3plt_la_SOURCES_arts  = Arts.cpp
libtse3plt_la_LIBADD_arts   = $(LIBARTS) -lartsmidi
INCLUDES_arts               = -I$(TSE3_ARTS_PREFIX)
else
tse3plth_HEADERS_arts       = 
libtse3plt_la_SOURCES_arts  = 
libtse3plt_la_LIBADD_arts   = 
INCLUDES_arts               =
endif

tse3plth_HEADERS      += $(tse3plth_HEADERS_arts)
libtse3plt_la_SOURCES += $(libtse3plt_la_SOURCES_alsa) $(libtse3plt_la_SOURCES_arts)
libtse3plt_la_LIBADD   = $(libtse3plt_la_LIBADD_alsa) $(libtse3plt_la_LIBADD_arts)
INCLUDES              += $(INCLUDES_arts)

# Win32 (obviously, under cygwin or mingw32)

if TSE3_WITH_WIN32
tse3plth_HEADERS_win32      = Win32.h
libtse3plt_la_SOURCES_win32 = Win32.cpp
libtse3plt_la_LIBADD_win32  =
INCLUDES_wind32             =
else
tse3plth_HEADERS_win32      = 
libtse3plt_la_SOURCES_win32 = 
libtse3plt_la_LIBADD_win32  = 
INCLUDES_win32              =
endif

tse3plth_HEADERS      += $(tse3plth_HEADERS_win32)
libtse3plt_la_SOURCES += $(libtse3plt_la_SOURCES_win32)
libtse3plt_la_LIBADD  += $(libtse3plt_la_LIBADD_win32)
INCLUDES              += $(INCLUDES_win32)

