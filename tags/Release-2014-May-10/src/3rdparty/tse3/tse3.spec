################################################################################
#
# Name:            tse3.spec
# Purpose:         Spec file to generate RPMs for TSE3 library
# Author:          Pete Goodliffe
# Date:            2000-01-30
#
# Library version: 0.0.21
#
# Usage:
#   To create a TSE3 RPM file, run a make dist and copy the tse3-X.tar.gz to
#   /usr/src/redhat/SOURCES. Then su to root and in the same directory as this
#   spec file rpm -ba tse3.spec
# 
###############################################################################

Summary:   TSE3 sequencer engine library
Name:      tse3
Version:   0.0.21
Release:   1
Copyright: GPL
Vendor:    Trax Software
Url:       http://tse3.sourceforge.net
Packager:  Pete Goodliffe
Group:     Applications/Multimedia
Source:    tse3-0.0.21.tar.gz
BuildRoot: /tmp/%{name}-%{version}-buildroot
Prefix:    /usr/local

%description
TSE3 [ Trax Sequencer Engine version 3 ]

The TSE3 library is a powerful third generation sequencer engine.
It provides core sequencer functionality with no form of fancy GUI interface.
The library comes with a sample playback program, tse3play

Install the TSE3 library if you want to use the tse3play program or
are using packages that rely on TSE3.

This RPM installation does not include Alsa library support, to get this you will have to build from source.

###############################################################################
# Preparing the build

%prep
rm -rf $RPM_BUILD_ROOT
%setup

###############################################################################
# Performing the build
#   - Currently, I don't package with alsa support, since it's not working.
#   - The docs aren't installed, otherwise the RPM gets two copies, one
#     installed in /usr/local/doc (thanks to the makefile) and one in
#     /usr/share/doc (thanks to rpm).
#   - I build in non debug mode to keep the library size down.

%build
CXXFLAGS=-O2 ./configure -prefix=$RPM_BUILD_ROOT/usr/local --with-install-root=$RPM_BUILD_ROOT --without-alsa --without-doc-install
make

###############################################################################
# Installation
#   - Performed into a temporary directory to not screw up the machine's setup

%install
make install

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.TSE3
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.TSE3
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.TSE3

###############################################################################
# Post install housekeeping

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_DIR/file.list.TSE3

###############################################################################
# RPM installation scripts

%post
ldconfig

%postun
ldconfig

###############################################################################
# The files to install

%files -f../file.list.TSE3
%doc doc/*.html doc/History doc/*.gif doc/*.png AUTHORS COPYING

