SUBDIRS = doc demos src

EXTRA_DIST = Version tse3.spec


###############################################################################
# Rule to create a local lib directory
# 
#   If you don't want to "make install", and just use your TSE3 library from
#   the build directory, then you can do a "make tse3-lib-local" after a "make"
#   to create a lib directory with symlinks to the objects.
#
#   It is suggested, however, that you initially do a
#   "./configure --prefix=SOMEWHERE_INOCULOUS" #   and "make install" to there
#   instead. This is only really intended for development work.

tse3-lib-local:
	@echo "**** Creating local lib directory from built sources ****"
	mkdir -p $(top_builddir)/lib
	cd $(top_builddir)/lib; for i in `find ../src -path */.libs/*.so*`; do echo "symbolic link to file: $$i"; ln -s -f $$i .; done

tse3-lib-local-clean:
	@echo "**** Cleaning local lib directory 'lib' ****"
	rm -rf lib

###############################################################################
# register rule

AUTHOR_EMAIL=pete@cthree.org
REG_TMP_FILE=/tmp/tse3-reg.txt

register:
	@echo "TSE3 Registration"                              > REG_TMP_FILE \
	&& echo "================="                           >> REG_TMP_FILE \
	&& echo                                               >> REG_TMP_FILE \
	&& echo "TSE3 info:    $(PACKAGE) $(VERSION)"         >> REG_TMP_FILE \
	&& echo "User info:    $$USER@$$HOSTNAME"             >> REG_TMP_FILE \
	&& echo "System:       $$(uname -m -r -s)"            >> REG_TMP_FILE \
	&& echo "Installed on: $$(date)"                      >> REG_TMP_FILE \
	&& mail $(AUTHOR_EMAIL) -s "$(PACKAGE) $(VERSION) Registration" < REG_TMP_FILE \
	&& rm REG_TMP_FILE \
    && echo "TSE3 registration email sent. Thankyou."

###############################################################################
# Documentation

if INSTALL_TSE3_DOC
docs_DATA = AUTHORS COPYING
docsdir = ${prefix}/doc/${PACKAGE}-${VERSION}
endif

