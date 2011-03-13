###############################################################################
# Install demo songs

SONG_FILES =      \
	Ambient.tse3  \
	Bry.tse3      \
	Demo.tse3     \
	Enigma.tse3   \
	FunkyJob.tse3 \
	Humphry.tse3  \
	Trip.tse3     \
	Demo.tse2

if INSTALL_TSE3_DOC
docs_DATA = $(SONG_FILES)
docsdir = ${prefix}/doc/${PACKAGE}-${VERSION}/songs
endif

###############################################################################
# Distribute them

EXTRA_DIST  = test.mid Standard.ins
EXTRA_DIST += $(SONG_FILES)

