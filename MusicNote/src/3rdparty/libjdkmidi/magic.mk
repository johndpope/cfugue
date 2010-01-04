########################################################################
#
# The magic.makefile Copyright 2004-2008 
# by Jeff Koftinoff <jeffk@jdkoftinoff.com> 
# and J.D. Koftinoff Software Ltd.
#
# Version 7: http://opensource.jdkoftinoff.com/project/wiki/magicmake
#
# Simplifies the building of a c/c++ library, tests, tools, examples, 
# and documentation.
#
# based on v6: http://opensource.jdkoftinoff.com/project/wiki/autobuild
#
# for more information, including license information (GPL). 
# Note that this tool being GPL does NOT mean that it can only 
# be used to build GPL projects. You are free to use this software in 
# any way but this software itself is GPL
#

MAGICMAKE_DIR=$(PROJECT_TOP_DIR)

########################################################################
# magic-project.mk:
# default project settings. These are usually originally set in project.mk
# PROJECT is the single word describing this project.

PROJECT?=project

# PROJECT_NAME is a single line describing this project
PROJECT_NAME?=Project
PROJECT_VARIANT?=

ifneq ($(PROJECT_VARIANT),)
PROJECT_VARIANT_SUFFIX=-$(PROJECT_VARIANT)
endif

# PROJECT_VERSION defaults to the date, YYYYMMDD
PROJECT_VERSION_SHELL:=$(shell date +%Y%m%d)
PROJECT_VERSION?=$(PROJECT_VERSION_SHELL)
PROJECT_RELEASE?=1


PROJECT_MAINTAINER?=project_maintainer

PROJECT_COPYRIGHT?=project_copyright

PROJECT_COMPANY?=project_companyname

PROJECT_WEBSITE?=project_website

PROJECT_DESCRIPTION?=project_description

# We are not cross compiling by default
CROSS_COMPILING?=0

# PKG_CONFIG_PACKAGES contains lists of packages handled by pkg-config to be used
PKGCONFIG_PACKAGES?=

# Default to no packaging or install tool
PACKAGER?=tgz

PACKAGE_PROVIDES?=$(PROJECT_NAME)

# CONFIG_TOOLS are like PKGCONFIG_PACKAGES but not handled by 'pkg-config', they are called directly
CONFIG_TOOLS?=

# TOP_LIB_DIRS is a list of relative directories from the PROJECT_TOP_DIR that each contain
# an "include", "src", "tools", "tests" and other directories. Typically for a single project
# TOP_LIB_DIRS is set to '.' in project.mk

# LIB_DIRS is then calculated with the fully qualified directory name for each directory in TOP_LIB_DIRS
LIB_DIRS+=$(call add_top_dir_prefix,$(TOP_LIB_DIRS))

# by default do not build shared libraries
ENABLE_SHLIB?=0

# by default build static libraries
ENABLE_STATICLIB?=1



########################################################################
# magic-tools.mk

# RSYNC is the name of our rsync program.
RSYNC_SHELL:=$(shell which rsync)
RSYNC?=$(RSYNC_SHELL)

# RSYNC_OPTIONS are the rsync options we use to copy a tree of files/directories to another dir.
# We want ot exclude backup files and subversion and CVS control directories.
RSYNC_OPTIONS?=-a --exclude='*~' --exclude='.svn' --exclude='CVS'

TAR_EXCLUDE_LIST=--exclude '.svn' --exclude '*~' --exclude '.hg' --exclude 'CVS' --exclude '.git'

# When we build a config tool script, this is the file name we use. 
PROJECT_CONFIG_TOOL?=$(PROJECT)-config

SWIG:=$(shell which swig)
SWIG?=$(SWIG_SHELL)
SWIG_OPTIONS?=-python
OBJDUMP?=$(COMPILER_PREFIX)objdump
OBJDUMP_FLAGS?=-d -C
LIPO?=$(COMPILER_PREFIX)lipo
ZIP?=zip
ZIP_FLAGS=-rp
GZIP?=gzip
BZIP2?=bzip2
DPKG_SHELL:=$(shell which dpkg)
DPKG?=$(DPKG_SHELL)
FAKEROOT_SHELL:=$(shell which fakeroot)
FAKEROOT?=$(FAKEROOT_SHELL)
RUN_DPKG=$(FAKEROOT) $(DPKG)
M4_SHELL:=$(shell which m4)
M4?=$(M4_SHELL)
RANLIB_SHELL:=$(shell which ranlib)
RANLIB?=$(RANLIB_SHELL)
RPMBUILD_SHELL:=$(shell which rpmbuild)
RPMBUILD?=$(RPMBUILD_SHELL)
EPM_SHELL:=$(shell which epm)
EPM?=$(EPM_SHELL)
MKEPMLIST_SHELL:=$(shell which mkepmlist)
MKEPMLIST?=$(MKEPMLIST_SHELL)
TAR?=tar
DOXYGEN_SHELL:=$(shell which doxygen)
DOXYGEN?=$(DOXYGEN_SHELL)
VALGRIND_SHELL:=$(shell which valgrind)
VALGRIND?=$(VALGRIND_SHELL)
CP?=cp
CP_FLAGS=-rpv
SCP?=scp
SCP_OPTIONS?=-rpqC
SHIP_TO?=.
QMAKE_SHELL:=$(shell which qmake)
QMAKE?=$(QMAKE_SHELL)
MKDIR?=mkdir
MKDIRS?=mkdir -p
RMDIRS?=rm -r -f
RM?=rm -f
INSTALL?=install
INSTALL_FLAGS?=-d
SSH?=ssh
SUDO?=sudo
CHECKINSTALL_SHELL:=$(shell which checkinstall)
CHECKINSTALL?=$(CHEKCINSTALL_SHELL)
SUDO_CHECKINSTALL?=$(SUDO) $(CHECKINSTALL)
CHECKINSTALL_OPTIONS+=--install=no --fstrans -y 

########################################################################
#
# utility functions 

# add_top_dir_prefix is a function that takes every directory and or wildcard pattern 
# in arg1 and prefixes PROJECT_TOP_DIR to it, and then only expands the directories that actually exist.

add_top_dir_prefix=$(strip $(wildcard $(foreach lib,$(1),$(PROJECT_TOP_DIR)/$(lib))))


# subdirs_in_path is a function which takes 1 parameter:
#  $(1) is a single directory
# it expands to a list of all directories in $(1)

subdirs_in_path=$(strip $(foreach d,$(wildcard $(1)/*/.),$(dir $(d))))

bare_subdirs_in_path=$(subst /,,$(subst $(1),,$(call subdirs_in_path,$(1))))

# suffix_platform_dirs is a function that takes 3 parameters:
#  $(1) is a subdirectory name, like 'src'
#  $(2) is a list of platform directories, like 'win32' or 'posix'
#  $(3) is a list of main directory names, like '$(PROJECT_TOP_DIR)'
# 
# suffix_platform_dirs expands all existing directories that match any of $(3)/$(1) or $(3)/$(1)/$(2)


suffix_platform_dirs=$(strip $(foreach dir,$(addsuffix /$(1),$(3)) $(foreach platform,$(2),$(addsuffix /$(1)/$(platform),$(3))),$(wildcard $(dir))))


# target_suffix_platform_dirs is a function which takes one parameter:
#  $(1) is a subdirectory name, like 'src' or 'tool'
# it expands into all existing directories for the target platform sources
# including any variant suffix, like 'src-special' or 'tool-special'

target_suffix_platform_dirs=$(wildcard $(call suffix_platform_dirs,$(1),$(PLATFORM_DIRS),$(LIB_DIRS)))

# native_suffix_platform_dirs is a function which takes one parameter:
#  $(1) is a subdirectory name, like 'src' or 'tool'
# it expands into all existing directories for the native platform sources

native_suffix_platform_dirs=$(wildcard $(call suffix_platform_dirs,$(1),$(NATIVE_PLATFORM_DIRS),$(LIB_DIRS)))

# get_file_list is a function which takes two parameters:
#  $(1) a list of directory names
#  $(2) a file extension (without the dot)
# it expands into all the file names that are found in the listed directories which that file extension,
# and then removes all the directory names from the results.

get_file_list=$(strip $(notdir $(foreach dir,$(1),$(wildcard $(dir)/*.$(2)))))

# get_file_list_full is just like get_file_list but returns the full source path names instead of just the basenames

get_file_list_full=$(strip $(foreach dir,$(1),$(wildcard $(dir)/*.$(2))))


# get_src_file_list is a function which takes one parameter:
#  $(1) is a file extension
# it expands into a list of all source files for the target platform that of that type
# (with the directory names removed)

get_src_file_list=$(call get_file_list,$(sort $(LIB_SRC_DIR)),$(1))
get_native_src_file_list=$(call get_file_list,$(sort $(NATIVE_LIB_SRC_DIR)),$(1))

# get_include_file_list is a function which takes one parameter:
#  $(1) is a file extension
# it expands into a list of all include files for the target platform that of that type
# (with the directory names removed)

get_include_file_list=$(call get_file_list_full,$(call subdirs_in_path,$(LIB_INCLUDE_DIR)),$(1)) $(call get_file_list_full,$(LIB_INCLUDE_DIR),$(1))

# get_cpp_o_files is a function which takes one parameter:
#  $(1) is a list of cpp files with no directory names
#
# it returns the full path names for all the required object files which are generated by cpp files.
# get_cc_o_files, get_c_o_files, get_m_o_files, and get_mm_o_files are the same as get_cpp_o_files 
# except for C, Objective-C, and Objective-C++ respectively.

get_cpp_o_files=$(addprefix $(OUTPUT_OBJ_DIR)/,$(1:.cpp=.o))
get_cc_o_files=$(addprefix $(OUTPUT_OBJ_DIR)/,$(1:.cc=.o))
get_c_o_files=$(addprefix $(OUTPUT_OBJ_DIR)/,$(1:.c=.o))
get_m_o_files=$(addprefix $(OUTPUT_OBJ_DIR)/,$(1:.m=.o))
get_mm_o_files=$(addprefix $(OUTPUT_OBJ_DIR)/,$(1:.mm=.o))
get_rc_o_files=$(addprefix $(OUTPUT_OBJ_DIR)/,$(1:.rc=.o))

get_cpp_shlib_o_files=$(addprefix $(OUTPUT_SHOBJ_DIR)/,$(1:.cpp=.o))
get_cc_shlib_o_files=$(addprefix $(OUTPUT_SHOBJ_DIR)/,$(1:.cc=.o))
get_c_shlib_o_files=$(addprefix $(OUTPUT_SHOBJ_DIR)/,$(1:.c=.o))
get_m_shlib_o_files=$(addprefix $(OUTPUT_SHOBJ_DIR)/,$(1:.m=.o))
get_mm_shlib_o_files=$(addprefix $(OUTPUT_SHOBJ_DIR)/,$(1:.mm=.o))
get_rc_shlib_o_files=$(addprefix $(OUTPUT_SHOBJ_DIR)/,$(1:.rc=.o))

get_native_cpp_o_files=$(addprefix $(NATIVE_OUTPUT_OBJ_DIR)/,$(1:.cpp=.o))
get_native_cc_o_files=$(addprefix $(NATIVE_OUTPUT_OBJ_DIR)/,$(1:.cc=.o))
get_native_c_o_files=$(addprefix $(NATIVE_OUTPUT_OBJ_DIR)/,$(1:.c=.o))
get_native_m_o_files=$(addprefix $(NATIVE_OUTPUT_OBJ_DIR)/,$(1:.m=.o))
get_native_mm_o_files=$(addprefix $(NATIVE_OUTPUT_OBJ_DIR)/,$(1:.mm=.o))
get_native_rc_o_files=$(addprefix $(NATIVE_OUTPUT_OBJ_DIR)/,$(1:.rc=.o))



# calc_target_options is a function which takes 1 parameter:
#   $(1) is a target platform suffix, such as MINGW32, POSIX, or LINUX
# 
# it expands into makefile code which is to be ultimately eval'd - 
# this means that if $(1) is LINUX, then for instance, COMPILE_FLAGS_LINUX
# will get appended to the current COMPILE_FLAGS.
# the variables set in this fashion include:
#  LIB_SRC_DIR, DEFINES, COMPILE_FLAGS, LINK_FLAGS, LDLIBS,

define calc_target_options
# add the relevant top src dirs
LIB_SRC_DIR+=$$(call add_top_dir_prefix,$$(TOP_LIB_DIRS_$(1)))

# set the target platform macro definition
DEFINES+=TARGET_PLATFORM_$(1)=1

# set the platform specific project defines
DEFINES+=$$(DEFINES_$(1))

# set the platform specific project compile flags
COMPILE_FLAGS+=$$(COMPILE_FLAGS_$(1))

# set the platform specific project pre-process flags
PREPROCESS_FLAGS+=$$(PREPROCESS_FLAGS_$(1))

# set the platform specific project link flags
LINK_FLAGS+=$$(LINK_FLAGS_$(1))

# and for linking shlibs
SHLIB_LINK_FLAGS+=$$(SHLIB_LINK_FLAGS_$(1))

# set the platform specific project link libs
LDLIBS+=$$(LDLIBS_$(1))

endef


# calc_multi_target_options is a function which takes 1 parameter:
#  $(1) is a list of target platform suffixes, such as POSIX or LINUX
# it takes each one and expands them via the calc_target_options function.
# this expansion is to be eval'd.

calc_multi_target_options=$(foreach suffix,$(1),$(call calc_target_options,$(suffix)))


# calc_native_options is a function which takes 1 parameter:
#   $(1) is a native platform suffix, such as MINGW32, POSIX, or LINUX
# 
# it expands into makefile code which is to be ultimately eval'd - 
# this means that if $(1) is LINUX, then for instance, COMPILE_FLAGS_LINUX
# will get appended to the current NATIVE_COMPILE_FLAGS.
# the variables set in this fashion include:
#  NATIVE_LIB_SRC_DIR, NATIVE_DEFINES, NATIVE_COMPILE_FLAGS, 
#  NATIVE_LINK_FLAGS, NATIVE_LDLIBS

define calc_native_options
# add the relevant top src dirs
NATIVE_LIB_SRC_DIR+=$$(call add_top_dir_prefix,$$(TOP_LIB_DIRS_$(1)))

# set the target platform macro definition
NATIVE_DEFINES+=TARGET_PLATFORM_$(1)=1

# set the platform specific project defines
NATIVE_DEFINES+=$$(DEFINES_$(1))

# set the platform specific project compile flags
NATIVE_COMPILE_FLAGS+=$$(COMPILE_FLAGS_$(1))

# set the platform specific project preprocess flags
NATIVE_PREPROCESS_FLAGS+=$$(PREPROCESS_FLAGS_$(1))

# set the platform specific project link flags
NATIVE_LINK_FLAGS+=$$(LINK_FLAGS_$(1))

# set the platform specific project link libs
NATIVE_LDLIBS+=$$(LDLIBS_$(1))

endef


# calc_multi_native_options is a function which takes 1 parameter:
#  $(1) is a list of native platform suffixes, such as POSIX or LINUX
# it takes each one and expands them via the calc_native_options function.
# this expansion is to be eval'd.

calc_multi_native_options=$(foreach suffix,$(1),$(call calc_native_options,$(suffix)))


# search_program_group is a function which returns the makefile text required to find
# all source files in a specified directory class and construct the o file lists for them.
# Param $(1) is the class of program in capitals, for example TOOLS, TESTS, EXAMPLES
# the text returned by this function is then to be eval'd.  The appropriate lists of 
# source files and O files for this class are then created dynamically.

define search_program_group
LIB_$(1)_CPP_FILES=$$(call get_file_list,$$(LIB_$(1)_DIR),cpp)
LIB_$(1)_CC_FILES=$$(call get_file_list,$$(LIB_$(1)_DIR),cc)
LIB_$(1)_C_FILES=$$(call get_file_list,$$(LIB_$(1)_DIR),c)
LIB_$(1)_M_FILES=$$(call get_file_list,$$(LIB_$(1)_DIR),m)
LIB_$(1)_MM_FILES=$$(call get_file_list,$$(LIB_$(1)_DIR),mm)
LIB_$(1)_RC_FILES=$$(call get_file_list,$$(LIB_$(1)_DIR),rc)
LIB_$(1)_SH_FILES=$$(call get_file_list_full,$$(LIB_$(1)_DIR),sh)

LIB_$(1)_O_FILES=$$(call get_cpp_o_files,$$(LIB_$(1)_CPP_FILES)) \
	$$(call get_cc_o_files,$$(LIB_$(1)_CC_FILES)) \
	$$(call get_c_o_files,$$(LIB_$(1)_C_FILES)) \
	$$(call get_m_o_files,$$(LIB_$(1)_M_FILES)) \
	$$(call get_mm_o_files,$$(LIB_$(1)_MM_FILES)) \
	$$(call get_rc_o_files,$$(LIB_$(1)_RC_FILES))

LIB_$(1)_SHLIB_O_FILES=$$(call get_cpp_shlib_o_files,$$(LIB_$(1)_CPP_FILES)) \
	$$(call get_cc_shlib_o_files,$$(LIB_$(1)_CC_FILES)) \
	$$(call get_c_shlib_o_files,$$(LIB_$(1)_C_FILES)) \
	$$(call get_m_shlib_o_files,$$(LIB_$(1)_M_FILES)) \
	$$(call get_mm_shlib_o_files,$$(LIB_$(1)_MM_FILES)) \
	$$(call get_rc_shlib_o_files,$$(LIB_$(1)_RC_FILES))

LIB_$(1)_DISASM_FILES=$$(LIB_$(1)_O_FILES:.o=.disasm)

LIB_$(1)_ASM_FILES=$$(LIB_$(1)_O_FILES:.o=.asm)

LIB_$(1)_EXE_FILES=$$(addprefix $$(OUTPUT_$(1)_DIR)/,$$(notdir $$(LIB_$(1)_O_FILES:.o=$$(EXE))))

ifeq ($(CROSS_COMPILING),1)
NATIVE_LIB_$(1)_CPP_FILES=$$(call get_file_list,$$(NATIVE_LIB_$(1)_DIR),cpp)
NATIVE_LIB_$(1)_CC_FILES=$$(call get_file_list,$$(NATIVE_LIB_$(1)_DIR),cc)
NATIVE_LIB_$(1)_C_FILES=$$(call get_file_list,$$(NATIVE_LIB_$(1)_DIR),c)
NATIVE_LIB_$(1)_M_FILES=$$(call get_file_list,$$(NATIVE_LIB_$(1)_DIR),m)
NATIVE_LIB_$(1)_MM_FILES=$$(call get_file_list,$$(NATIVE_LIB_$(1)_DIR),mm)
NATIVE_LIB_$(1)_SH_FILES=$$(call get_file_list_full,$$(NATIVE_LIB_$(1)_DIR),sh)
NATIVE_LIB_$(1)_RC_FILES=$$(call get_file_list,$$(NATIVE_LIB_$(1)_DIR),rc)

NATIVE_LIB_TOOLS_O_FILES=$$(call get_native_cpp_o_files,$$(NATIVE_LIB_$(1)_CPP_FILES)) \
	$$(call get_native_cc_o_files,$$(NATIVE_LIB_$(1)_CC_FILES)) \
	$$(call get_native_c_o_files,$$(NATIVE_LIB_$(1)_C_FILES)) \
	$$(call get_native_m_o_files,$$(NATIVE_LIB_$(1)_M_FILES)) \
	$$(call get_native_mm_o_files,$$(NATIVE_LIB_$(1)_MM_FILES)) \
	$$(call get_native_rc_o_files,$$(NATIVE_LIB_$(1)_RC_FILES))

NATIVE_LIB_$(1)_EXE_FILES=$$(addprefix $$(NATIVE_OUTPUT_$(1)_DIR)/,$$(notdir $$(NATIVE_LIB_$(1)_O_FILES:.o=$$(NATIVE_EXE))))
endif
endef

# copy_file is a macro which copies a specified file to a new file name only if it originally exists

define copy_file
	if [ -f "$(1)" ]; then $(CP) -p $(1) $(2); fi
endef

# copy_files is a macro which recursively copies files only if they exist from $(1) to dir $(2), potentially creating dir $(2)
define copy_files
	for j in $(1); do $(INSTALL) $(INSTALL_FLAGS) "$$j" $(2); done
endef


define copy_dirs
	for i in $(wildcard $(1)); do if [ -d "$$i" ]; then s="$$i"/; else s=`dirname "$$i"`; fi; ( $(MKDIRS) $(2); cd "$$s" && $(TAR) -c $(TAR_EXCLUDE_LIST) . | $(TAR) -C $(2) -xf - ); done
endef

# erase_nondeployed_files is a macro which removes any files in dir $(1) and its subdirs that are not to be deployed
# such as .svn .hg .git and temp files
define erase_nondeployed_files
	@find $(1) -type f -name '.*' -exec $(RM) \{\} \;
	@find $(1) -type f -name '*~' -exec $(RM) \{\} \;
	@find $(1) -type f -name '.svn' -exec $(RM) \{\} \;
	@find $(1) -type f -name '.hg' -exec $(RM) \{\} \;
	@find $(1) -type f -name '.git' -exec $(RM) \{\} \;
endef


########################################################################
# magic-options.mk

RSYNC_OPTIONS+=

SHIP_TO_ACCOUNT = $(firstword $(subst :, ,$(SHIP_TO)))
SHIP_TO_DIR = $(lastword $(subst :, ,$(SHIP_TO)))


ifndef TARGET_FANCY_DIRS
TARGET_PLAIN_DIRS?=1
endif

# compile options

# RELEASE=1 means optimize no debug
ifeq ($(RELEASE),1)
RELEASE_FLAGS?=
OPTIMIZE?=1
DEBUG?=0
COMPILE_FLAGS+=$(RELEASE_FLAGS)
endif

# having OPTIMIZE set to 1 means we compile with -O2 but you can override this by settings OPTIMIZE_FLAGS
ifeq ($(OPTIMIZE),1)
OPTIMIZE_FLAGS?=-O2
COMPILE_FLAGS+=$(OPTIMIZE_FLAGS)
endif

# having DEBUG set to 1 means we compile with -g and add preprocessor define DEBUG=1

ifeq ($(DEBUG),1)
DEBUG_FLAGS?=-g
COMPILE_FLAGS+=$(DEBUG_FLAGS)
DEFINES+=DEBUG=1
endif

# having PROFILE set to 1 means we compile with -pg 

ifeq ($(PROFILE),1)
PROFILE_FLAGS?=-pg
COMPILE_FLAGS+=$(PROFILE_FLAGS)
endif

# haviing WARNINGS set to 0 means we do not compile with -Wall. Default is 1

WARNINGS?=1
ifeq ($(WARNINGS),1)
WARNINGS_FLAGS=-Wall
COMPILE_FLAGS+=$(WARNINGS_FLAGS)
endif

COMPILER?=gcc

ifeq ($(COMPILER),intel)
COMPILE_FLAGS+=
CXX=icc
CC=icc
AR=xiar
LINK.cpp=icc
LINK.c=icc
LINK_FLAGS+=-cxxlib -lstdc++ -lm -lsvml
endif

ifeq ($(DISABLE_SSE),1)
SSE_COMPILE_FLAGS=
endif

CONFIG_TOOLS_PATHS:=$(foreach pkg,$(CONFIG_TOOLS),$(shell which $(pkg)))

INCLUDES_PACKAGES:=$(sort $(filter-out -Wstrict-prototypes,$(foreach pkg,$(PKGCONFIG_PACKAGES),$(shell pkg-config $(pkg) --cflags)) $(foreach pkg,$(CONFIG_TOOLS),$(shell $(pkg) $(CONFIG_TOOLS_OPTIONS) --cflags))))

COMPILE_FLAGS+=$(COMPILE_FLAGS_PACKAGES)

# all our */include directories
INCLUDES+=$(LIB_INCLUDE_DIR)

# and any destination prefix directories afterwards

ifdef PREFIX
INCLUDES+="$(PREFIX)/include"
LDFLAGS+=-L$(PREFIX)/lib
endif

ifdef TARGET_INSTALL_DIR
INCLUDES+="$(TARGET_INSTALL_DIR)/include"
endif


# our compiler defines
DEFINES?=

# The preprocessor flags is initially comprised of -I option for each include dir, 
# and the -D option for each define
PREPROCESS_FLAGS+=$(addprefix -I,$(INCLUDES)) $(addprefix -D,$(DEFINES)) $(INCLUDES_PACKAGES) 

SORTED_PREPROCESS_FLAGS=$(PREPROCESS_FLAGS) $(COMPILE_FLAGS_PACKAGES)

# the additional linker flags:
LDFLAGS_PACKAGES:=$(foreach pkg,$(PKGCONFIG_PACKAGES),$(shell pkg-config $(pkg) --ldflags)) $(foreach pkg,$(CONFIG_TOOLS),$(shell $(pkg) $(CONFIG_TOOLS_OPTIONS) --ldflags))

# the addition linker libraries:

LDLIBS_PACKAGES:=$(foreach pkg,$(PKGCONFIG_PACKAGES),$(shell pkg-config $(pkg) --libs)) $(foreach pkg,$(CONFIG_TOOLS),$(shell $(pkg) $(CONFIG_TOOLS_OPTIONS) --libs))

LDLIBS+=

INHERITED_LIBS+=$(LINK_FLAGS) $(LDLIBS_PACKAGES) $(LDFLAGS_PACKAGES)

# The preprocessor flags needed to generate dependency information:
DEPENDENCY_OPTIONS?=-MM 


# NATIVE_RELEASE=1 means optimize no debug
ifeq ($(NATIVE_RELEASE),1)
NATIVE_OPTIMIZE?=1
NATIVE_DEBUG?=0
endif

# having OPTIMIZE set to 1 means we compile with -O2 but you can override this by settings OPTIMIZE_FLAGS
ifeq ($(NATIVE_OPTIMIZE),1)
NATIVE_OPTIMIZE_FLAGS?=-O2
NATIVE_COMPILE_FLAGS+=$(NATIVE_OPTIMIZE_FLAGS)
endif

# having DEBUG set to 1 means we compile with -g and add preprocessor define DEBUG=1

ifeq ($(NATIVE_DEBUG),1)
NATIVE_DEBUG_FLAGS?=-g
NATIVE_COMPILE_FLAGS+=$(NATIVE_DEBUG_FLAGS)
NATIVE_DEFINES+=DEBUG=1
endif

# having PROFILE set to 1 means we compile with -pg 

ifeq ($(NATIVE_PROFILE),1)
NATIVE_PROFILE_FLAGS?=-pg
NATIVE_COMPILE_FLAGS+=$(NATIVE_PROFILE_FLAGS)
endif

# haviing WARNINGS set to 0 means we do not compile with -Wall. Default is 1

NATIVE_WARNINGS?=1
ifeq ($(NATIVE_WARNINGS),1)
NATIVE_WARNINGS_FLAGS=-Wall
NATIVE_COMPILE_FLAGS+=$(NATIVE_WARNINGS_FLAGS)
endif



# The native platform preprocessor flags is initially comprised of -I option for each include dir, 
# and the -D option for each define
NATIVE_PREPROCESS_FLAGS=$(addprefix -I,$(INCLUDES)) $(addprefix -D,$(NATIVE_DEFINES)) 
SORTED_NATIVE_PREPROCESS_FLAGS=$(NATIVE_PREPROCESS_FLAGS)

# the additional linker flags:
NATIVE_LDFLAGS+=

# the addition linker libraries:
NATIVE_LDLIBS+=

LDLIBS_NO_OPTS=$(patsubst -%,,$(LDLIBS) $(LDLIBS_PACKAGES) $(PROJECT_LDLIB))

########################################################################
# magic-platform.mk

ifeq ($(QT4_MODE),1)
RAW_PLATFORM_DIRS+=qt4
endif

ifeq ($(WX_MODE),1)
RAW_PLATFORM_DIRS+=wx
endif

NATIVE_USE_AR?=1
NATIVE_USE_MACOSX_LIBTOOL?=0

# mingw32 is a subset of win32
ifeq ($(TARGET_PLATFORM_MINGW32),1)
PROJECT_ARCHITECTURE?=i386
TARGET_PLATFORM_NAME=win32-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=MINGW32 WIN32
EXE=.exe
RAW_PLATFORM_DIRS+=win32 mingw32
TARGET_PLATFORM_WIN32=1
# default to Windows XP target
WIN32_WINNT?=0x501
DEFINES+=_WIN32_WINNT=$(WIN32_WINNT)
endif

ifeq ($(TARGET_PLATFORM_MINGW32)$(RELEASE),11)
LDFLAGS+=-s
endif

# cygwin is really a subset of posix
ifeq ($(TARGET_PLATFORM_CYGWIN),1)
PROJECT_ARCHITECTURE?=i386
TARGET_PLATFORM_NAME=cygwin-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=CYGWIN POSIX
EXE=.exe
RAW_PLATFORM_DIRS+=cygwin posix
endif

# plain posix is just posix
ifeq ($(TARGET_PLATFORM_POSIX),1)
PROJECT_ARCHITECTURE?=unknown
TARGET_PLATFORM_NAME=posix-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX
RAW_PLATFORM_DIRS+=posix
endif

# linux is a subset of posix
ifeq ($(TARGET_PLATFORM_LINUX),1)
PROJECT_ARCHITECTURE_SHELL:=$(shell uname -m)
PROJECT_ARCHITECTURE?=$(PROJECT_ARCHITECTURE_SHELL)
TARGET_PLATFORM_NAME=linux-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX LINUX
RAW_PLATFORM_DIRS+=posix linux
TARGET_PLATFORM_POSIX=1
endif


# linux-i386 is a subset of linux, posix
ifeq ($(TARGET_PLATFORM_LINUX_I386),1)
PROJECT_ARCHITECTURE?=i386
TARGET_PLATFORM_NAME=linux-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX LINUX LINUX_I386
RAW_PLATFORM_DIRS+=posix linux linux-i386
TARGET_PLATFORM_LINUX?=1
TARGET_PLATFORM_POSIX=1
endif

# linux-x86_64 is a subset of linux, posix, linux-i386
ifeq ($(TARGET_PLATFORM_LINUX_X86_64),1)
PROJECT_ARCHITECTURE?=x86_64
TARGET_PLATFORM_NAME=linux-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX LINUX LINUX_I386  LINUX_X86_64
RAW_PLATFORM_DIRS+=posix linux linux-i386 linux-x86_64
LOCAL_COMPILE_FLAGS+=-m64
LOCAL_LINK_FLAGS+=-m64
TARGET_PLATFORM_LINUX_I386?=1
TARGET_PLATFORM_LINUX?=1
TARGET_PLATFORM_POSIX=1
endif

# linux-ppc is a subset of linux, posix
ifeq ($(TARGET_PLATFORM_LINUX_PPC),1)
PROJECT_ARCHITECTURE?=ppc
TARGET_PLATFORM_NAME=linux-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX LINUX LINUX_PPC
RAW_PLATFORM_DIRS+=posix linux linux-ppc
TARGET_PLATFORM_LINUX?=1
TARGET_PLATFORM_POSIX=1
endif


# linux-ppc64 is a subset of linux, linux-ppc posix
ifeq ($(TARGET_PLATFORM_LINUX_PPC64),1)
PROJECT_ARCHITECTURE?=ppc64
TARGET_PLATFORM_NAME=linux-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX LINUX LINUX_PPC LINUX_PPC64
RAW_PLATFORM_DIRS+=posix linux linux-ppc linux-ppc64
LOCAL_COMPILE_FLAGS+=-m64
LOCAL_LINK_FLAGS+=-m64
TARGET_PLATFORM_LINUX?=1
TARGET_PLATFORM_LINUX_PPC?=1
TARGET_PLATFORM_POSIX=1
endif

# xenomai is a subset of posix, linux
ifeq ($(TARGET_PLATFORM_XENOMAI),1)
PROJECT_ARCHITECTURE_SHELL:=$(shell uname -m)
PROJECT_ARCHITECTURE?=$(PROJECT_ARCHITECTURE_SHELL)
TARGET_PLATFORM_NAME=xenomai-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX LINUX XENOMAI
RAW_PLATFORM_DIRS+=posix linux xenomai
TARGET_PLATFORM_LINUX?=1
TARGET_PLATFORM_POSIX=1
endif


# xenomai-i386 is a subset of linux, posix
ifeq ($(TARGET_PLATFORM_XENOMAI_I386),1)
PROJECT_ARCHITECTURE?=i386
TARGET_PLATFORM_NAME=linux-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX LINUX LINUX_I386 XENOMAI XENOMAI_I386
RAW_PLATFORM_DIRS+=posix linux linux-i386 xenomai xenomai-i386
TARGET_PLATFORM_XENOMAI?=1
TARGET_PLATFORM_LINUX_I386?=1
TARGET_PLATFORM_LINUX?=1
TARGET_PLATFORM_POSIX=1
endif

# xenomai-x86_64 is a subset of linux, posix, linux-i386, xenomai xenomai-i386
ifeq ($(TARGET_PLATFORM_XENOMAI_X86_64),1)
PROJECT_ARCHITECTURE?=x86_64
TARGET_PLATFORM_NAME=xenomai-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX LINUX LINUX_X86_64 XENOMAI XENOMAI_I386 XENOMAI_X86_64
RAW_PLATFORM_DIRS+=posix linux linux-x86_64 xenomai xenomai-i386 xenomai-x86_64
LOCAL_COMPILE_FLAGS+=-m64
LOCAL_LINK_FLAGS+=-m64
TARGET_PLATFORM_XENOMAI_I386?=1
TARGET_PLATFORM_XENOMAI?=1
TARGET_PLATFORM_LINUX_X86_64?=1
TARGET_PLATFORM_LINUX_I386?=1
TARGET_PLATFORM_LINUX?=1
TARGET_PLATFORM_POSIX=1
endif

# xenomai-ppc is a subset of linux, posix, xenomai
ifeq ($(TARGET_PLATFORM_XENOMAI_PPC),1)
PROJECT_ARCHITECTURE?=ppc
TARGET_PLATFORM_NAME=xenomai-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX LINUX LINUX_PPC XENOMAI XENOMAI_PPC
RAW_PLATFORM_DIRS+=posix linux linux-ppc xenomai xenomai-ppc
TARGET_PLATFORM_XENOMAI_PPC?=1
TARGET_PLATFORM_XENOMAI?=1
TARGET_PLATFORM_LINUX_PPC?=1
TARGET_PLATFORM_LINUX?=1
TARGET_PLATFORM_POSIX=1
endif

# xenomai-ppc64 is a subset of linux, linux-ppc posix xenomai xenomai_ppc
ifeq ($(TARGET_PLATFORM_XENOMAI_PPC64),1)
PROJECT_ARCHITECTURE?=ppc64
TARGET_PLATFORM_NAME=xenomai-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX LINUX LINUX_PPC LINUX_PPC64 XENOMAI XENOMAI_PPC XENOMAI_PPC64
RAW_PLATFORM_DIRS+=posix linux linux-ppc linux-ppc64 xenomai xenomai-ppc xenomai-ppc64
LOCAL_COMPILE_FLAGS+=-m64
LOCAL_LINK_FLAGS+=-m64
TARGET_PLATFORM_XENOMAI_PPC?=1
TARGET_PLATFORM_XENOMAI?=1
TARGET_PLATFORM_LINUX_PPC64?=1
TARGET_PLATFORM_LINUX_PPC?=1
TARGET_PLATFORM_LINUX?=1
TARGET_PLATFORM_POSIX=1
endif


# bsd is a subset of posix
ifeq ($(TARGET_PLATFORM_BSD),1)
PROJECT_ARCHITECTURE?=unknown
TARGET_PLATFORM_NAME=bsd-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX BSD
RAW_PLATFORM_DIRS+=bsd posix
TARGET_PLATFORM_POSIX=1
endif

# freebsd is a subset of posix and bsd
ifeq ($(TARGET_PLATFORM_FREEBSD),1)
PROJECT_ARCHITECTURE?=unknown
TARGET_PLATFORM_NAME=freebsd-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX BSD FREEBSD
RAW_PLATFORM_DIRS+=bsd posix freebsd
TARGET_PLATFORM_POSIX=1
endif

# openbsd is a subset of posix and bsd
ifeq ($(TARGET_PLATFORM_OPENBSD),1)
PROJECT_ARCHITECTURE?=unknown
TARGET_PLATFORM_NAME=openbsd-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX BSD OPENBSD
RAW_PLATFORM_DIRS+=bsd posix openbsd
endif

# netbsd is a subset of posix and bsd
ifeq ($(TARGET_PLATFORM_NETBSD),1)
PROJECT_ARCHITECTURE?=unknown
TARGET_PLATFORM_NAME=netbsd-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX BSD NETBSD
RAW_PLATFORM_DIRS+=bsd posix netbsd
TARGET_PLATFORM_POSIX=1
endif

# solaris is a subset of posix 
ifeq ($(TARGET_PLATFORM_SOLARIS),1)
PROJECT_ARCHITECTURE?=unknown
TARGET_PLATFORM_NAME=solaris-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX SOLARIS
RAW_PLATFORM_DIRS+=solaris posix 
TARGET_PLATFORM_POSIX=1
endif

# macosx is a subset of posix
ifeq ($(TARGET_PLATFORM_MACOSX),1)
PROJECT_ARCHITECTURE_SHELL:=$(shell uname -p)
PROJECT_ARCHITECTURE?=$(PROJECT_ARCHITECTURE_SHELL)
TARGET_PLATFORM_NAME:=macosx-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX MACOSX
MACOSX_DEPLOYMENT_TARGET?=10.5
RAW_PLATFORM_DIRS+=posix macosx
DISASM=otool
DISASM_FLAGS=-t -v -V
COMPILE_FLAGS+=
LOCAL_LINK_FLAGS+=-mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
TARGET_PLATFORM_POSIX=1
endif

# macosx_ppc is a subset of macosx and posix
ifeq ($(TARGET_PLATFORM_MACOSX_PPC),1)
PROJECT_ARCHITECTURE?=ppc
TARGET_PLATFORM_NAME=macosx-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX MACOSX MACOSX_PPC
MACOSX_DEPLOYMENT_TARGET?=10.5
RAW_PLATFORM_DIRS+=posix macosx macosx-ppc
DISASM=otool
DISASM_FLAGS=-t -v -V
COMPILE_FLAGS+=
LOCAL_LINK_FLAGS+=-mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
TARGET_PLATFORM_MACOSX=1
TARGET_PLATFORM_POSIX=1
endif

# macosx_ppc64 is a subset of macosx and posix
ifeq ($(TARGET_PLATFORM_MACOSX_PPC64),1)
PROJECT_ARCHITECTURE?=ppc64
TARGET_PLATFORM_NAME=macosx-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX MACOSX MACOSX_PPC MACOSX_PPC64
MACOSX_DEPLOYMENT_TARGET?=10.5
RAW_PLATFORM_DIRS+=posix macosx macosx-ppc macosx-ppc64
DISASM=otool
DISASM_FLAGS=-t -v -V
LOCAL_COMPILE_FLAGS+=-m64
LOCAL_LINK_FLAGS+=-m64
LOCAL_LINK_FLAGS+=-mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
TARGET_PLATFORM_MACOSX_PPC=1
TARGET_PLATFORM_POSIX=1
endif

# macosx_i386 is a subset of macosx and posix
ifeq ($(TARGET_PLATFORM_MACOSX_I386),1)
PROJECT_ARCHITECTURE?=i386
TARGET_PLATFORM_NAME=macosx-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX MACOSX MACOSX_I386
MACOSX_DEPLOYMENT_TARGET?=10.5
RAW_PLATFORM_DIRS+=posix macosx macosx-i386
DISASM=otool
DISASM_FLAGS=-t -v -V
COMPILE_FLAGS+=
LOCAL_LINK_FLAGS+=-mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
TARGET_PLATFORM_MACOSX=1
TARGET_PLATFORM_POSIX=1
endif

# macosx_x86_64 is a subset of macosx and posix
ifeq ($(TARGET_PLATFORM_MACOSX_X86_64),1)
PROJECT_ARCHITECTURE?=x86_64
TARGET_PLATFORM_NAME=macosx-$(PROJECT_ARCHITECTURE)
SUFFIXES_TARGET_PLATFORM=POSIX MACOSX MACOSX_X86_64
MACOSX_DEPLOYMENT_TARGET?=10.5
RAW_PLATFORM_DIRS+=posix macosx macosx-x86_64
DISASM=otool
DISASM_FLAGS=-t -v -V
COMPILE_FLAGS+=
LOCAL_LINK_FLAGS+=-mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
LOCAL_COMPILE_FLAGS+=-m64
LOCAL_LINK_FLAGS+=-m64
TARGET_PLATFORM_MACOSX=1
TARGET_PLATFORM_MACOSX_I386=1
TARGET_PLATFORM_POSIX=1
endif


# macosx_universal is a subset of macosx and posix and uses mac libtool to generate fat binaries.
MACOSX_DEPLOYMENT_TARGET?=10.5
TARGET_MACOSX_SDK?=/Developer/SDKs/MacOSX$(MACOSX_DEPLOYMENT_TARGET).sdk
ifeq ($(TARGET_PLATFORM_MACOSX_UNIVERSAL),1)
TARGET_PLATFORM_NAME=macosx-universal
MACOSX_UNIVERSAL_ARCHS?=i386 ppc x86_64 ppc64
MACOSX_UNIVERSAL_ARCHS_PARAMS=$(foreach a,$(MACOSX_UNIVERSAL_ARCHS),-arch $(a))
SUFFIXES_TARGET_PLATFORM=POSIX MACOSX MACOSX_UNIVERSAL
RAW_PLATFORM_DIRS+=posix macosx macosx-ppc macosx-i386
LOCAL_COMPILE_FLAGS+=$(MACOSX_UNIVERSAL_ARCHS_PARAMS) -isysroot $(TARGET_MACOSX_SDK) 
LOCAL_PREPROCESS_FLAGS+=-isysroot $(TARGET_MACOSX_SDK) 
INCLUDES+=$(TARGET_MACOSX_SDK)/usr/include
LOCAL_LINK_FLAGS+=$(MACOSX_UNIVERSAL_ARCHS_PARAMS) -isysroot $(TARGET_MACOSX_SDK) -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
TARGET_USE_AR=0
TARGET_USE_MACOSX_LIBTOOL=1
MACOSX_LIBTOOL=libtool
MACOSX_LIBTOOLFLAGS?=-static
DISASM=otool
DISASM_FLAGS=-t -v -V
PROJECT_ARCHITECTURE?=$(MACOSX_UNIVERSAL_ARCHS)
TARGET_PLATFORM_MACOSX=1
endif

# cell_spu is not a posix platform
ifeq ($(TARGET_PLATFORM_CELL_SPU),1)
TARGET_PLATFORM_NAME=linux-cell_spu
SUFFIXES_TARGET_PLATFORM=CELL_SPU
RAW_PLATFORM_DIRS+=cell_spu
PROJECT_ARCHITECTURE?=cell_spu
endif

# linux_cell_ppu is a subset of linux-ppc, linux, posix
ifeq ($(TARGET_PLATFORM_LINUX_CELL_PPU),1)
TARGET_PLATFORM_NAME=linux-cell_ppu
SUFFIXES_TARGET_PLATFORM=POSIX LINUX LINUX_PPC LINUX_CELL_PPU CELL_PPU
RAW_PLATFORM_DIRS+=posix linux cell_ppu
PROJECT_ARCHITECTURE?=cell_ppu
TARGET_PLATFORM_POSIX=1
TARGET_PLATFORM_LINUX=1
endif

# default to objdump for disassembly
DISASM_FLAGS?=-d -S
DISASM?=$(OBJDUMP)

# if EXE suffix is not set then it ought to be blank.
EXE?=

# If we are to use normal gnu 'ar' to manipulate static libraries for the target platform,
# TARGET_USE_AR is to be set to 1. Defaults to 1.
TARGET_USE_AR?=1

# If we are to use mac os x libtool (tiger and beyond) to manipulate static libraries for the target
# platform, TARGET_USE_MACOSX_LIBTOOL is to be set to 1 instead. Defaults to 0.
TARGET_USE_MACOSX_LIBTOOL?=0

PLATFORM_DIRS=$(sort $(RAW_PLATFORM_DIRS))

########################################################################
# magic-dirs.mk
# Now we know all our target platform suffixes in SUFFIXES_TARGET_PLATFORM, so
# we eval the result of the calc_multi_target_options function. This effectively merges in
# the platform specific defines, compile flags, and link flags

$(eval $(call calc_multi_target_options,$(SUFFIXES_TARGET_PLATFORM)))

########################################################################
#
# Calculate all our source dirs for various things, for our target platform
#
LIB_INCLUDE_DIR+=$(call target_suffix_platform_dirs,include) 
LIB_SRC_DIR+=$(call target_suffix_platform_dirs,src) 
LIB_SWIG_DIR+=$(call target_suffix_platform_dirs,swig) 
LIB_TESTS_DIR+=$(call target_suffix_platform_dirs,tests) 

LIB_EXAMPLES_DIR+=$(call target_suffix_platform_dirs,examples) 

LIB_SHARE_DIR+=$(call target_suffix_platform_dirs,share) 
LIB_SHARE_DEV_DIR+=$(call target_suffix_platform_dirs,share-dev) 
LIB_ETC_DIR+=$(call target_suffix_platform_dirs,etc/common) 

LIB_TOOLS_DIR+=$(call target_suffix_platform_dirs,tools) 
LIB_TOOLS_DEV_DIR+=$(call target_suffix_platform_dirs,tools-dev) 

LIB_DOCS_DIR+=$(PROJECT_TOP_DIR)/docs
LIB_DOCS_DEV_DIR+=$(PROJECT_TOP_DIR)/docs-dev

ALL_SOURCES_DIRS=$(strip $(LIB_SRC_DIR) $(LIB_TESTS_DIR) $(LIB_EXAMPLES_DIR) $(LIB_TOOLS_DIR) $(LIB_TOOLS_DEV_DIR))

# calculate our output directories for our target platform results
OUTPUT_DIR=$(BUILD_DIR)/build
OUTPUT_LIB_DIR?=$(OUTPUT_DIR)/lib
OUTPUT_TESTS_DIR?=$(OUTPUT_DIR)/tests
OUTPUT_DOCS_DEV_DIR?=$(OUTPUT_DIR)/docs-dev
OUTPUT_DOCS_DIR?=$(OUTPUT_DIR)/docs
OUTPUT_TOOLS_DIR?=$(OUTPUT_DIR)/tools
OUTPUT_TOOLS_DEV_DIR?=$(OUTPUT_DIR)/tools-dev
OUTPUT_EXAMPLES_DIR?=$(OUTPUT_DIR)/examples
OUTPUT_SHARE_DIR?=$(OUTPUT_DIR)/share
OUTPUT_ETC_DIR?=$(OUTPUT_DIR)/etc
OUTPUT_OBJ_DIR?=$(OUTPUT_DIR)/obj
OUTPUT_SHOBJ_DIR?=$(OUTPUT_DIR)/shobj

# our output libraries name is simply our project name prefixed with lib in our output lib dir.

ifdef PREFIX
TARGET_INSTALL_DIR?=$(PREFIX)
TARGET_INSTALLDEV_DIR?=$(PREFIX)
else
TARGET_INSTALL_PREFIX?=/opt/local
TARGET_INSTALL_DIR?=$(TARGET_INSTALL_PREFIX)/$(PROJECT)-$(PROJECT_VERSION)
TARGET_INSTALLDEV_DIR?=$(TARGET_INSTALL_PREFIX)/$(PROJECT)-$(PROJECT_VERSION)-dev
endif

TARGET_BIN_DIR?=bin
TARGET_LIB_DIR?=lib

ifdef TARGET_PLAIN_DIRS
TARGET_INCLUDE_DIR?=include
TARGET_DOCS_DIR?=share/doc/$(PROJECT)
TARGET_SHARE_DIR?=share/$(PROJECT)
TARGET_ETC_DIR?=etc/$(PROJECT)
else
TARGET_INCLUDE_DIR?=include/$(PROJECT)-$(PROJECT_VERSION)
TARGET_DOCS_DIR?=share/doc/$(PROJECT)-$(PROJECT_VERSION)
TARGET_SHARE_DIR?=share/$(PROJECT)-$(PROJECT_VERSION)
TARGET_ETC_DIR?=etc/$(PROJECT)-$(PROJECT_VERSION)
endif

DOCS_DEV_SUBDIR?=.

TARGET_INSTALL_BIN_DIR?=$(TARGET_INSTALL_DIR)/$(TARGET_BIN_DIR)
TARGET_INSTALL_LIB_DIR?=$(TARGET_INSTALL_DIR)/$(TARGET_LIB_DIR)
TARGET_INSTALL_INCLUDE_DIR?=$(TARGET_INSTALL_DIR)/$(TARGET_INCLUDE_DIR)
TARGET_INSTALL_DOCS_DIR?=$(TARGET_INSTALL_DIR)/$(TARGET_DOCS_DIR)
TARGET_INSTALL_SHARE_DIR?=$(TARGET_INSTALL_DIR)/$(TARGET_SHARE_DIR)
TARGET_INSTALL_ETC_DIR?=$(TARGET_INSTALL_DIR)/$(TARGET_ETC_DIR)

LOCAL_INSTALL_DIR?=$(BUILD_DIR)/$(PROJECT)-$(PROJECT_VERSION)

LOCAL_INSTALL_BIN_DIR?=$(LOCAL_INSTALL_DIR)$(TARGET_INSTALL_BIN_DIR)
LOCAL_INSTALL_LIB_DIR?=$(LOCAL_INSTALL_DIR)$(TARGET_INSTALL_LIB_DIR)
LOCAL_INSTALL_INCLUDE_DIR?=$(LOCAL_INSTALL_DIR)$(TARGET_INSTALL_INCLUDE_DIR)
LOCAL_INSTALL_DOCS_DIR?=$(LOCAL_INSTALL_DIR)$(TARGET_INSTALL_DOCS_DIR)
LOCAL_INSTALL_SHARE_DIR?=$(LOCAL_INSTALL_DIR)$(TARGET_INSTALL_SHARE_DIR)
LOCAL_INSTALL_ETC_DIR?=$(LOCAL_INSTALL_DIR)$(TARGET_INSTALL_ETC_DIR)

TARGET_INSTALLDEV_BIN_DIR?=$(TARGET_INSTALLDEV_DIR)/$(TARGET_BIN_DIR)
TARGET_INSTALLDEV_LIB_DIR?=$(TARGET_INSTALLDEV_DIR)/$(TARGET_LIB_DIR)
TARGET_INSTALLDEV_INCLUDE_DIR?=$(TARGET_INSTALLDEV_DIR)/$(TARGET_INCLUDE_DIR)
TARGET_INSTALLDEV_DOCS_DIR?=$(TARGET_INSTALLDEV_DIR)/$(TARGET_DOCS_DIR)
TARGET_INSTALLDEV_SHARE_DIR?=$(TARGET_INSTALLDEV_DIR)/$(TARGET_SHARE_DIR)
TARGET_INSTALLDEV_ETC_DIR?=$(TARGET_INSTALLDEV_DIR)/$(TARGET_ETC_DIR)

LOCAL_INSTALLDEV_DIR?=$(BUILD_DIR)/$(PROJECT)-$(PROJECT_VERSION)-dev

LOCAL_INSTALLDEV_BIN_DIR?=$(LOCAL_INSTALLDEV_DIR)$(TARGET_INSTALLDEV_BIN_DIR)
LOCAL_INSTALLDEV_LIB_DIR?=$(LOCAL_INSTALLDEV_DIR)$(TARGET_INSTALLDEV_LIB_DIR)
LOCAL_INSTALLDEV_INCLUDE_DIR?=$(LOCAL_INSTALLDEV_DIR)$(TARGET_INSTALLDEV_INCLUDE_DIR)
LOCAL_INSTALLDEV_DOCS_DIR?=$(LOCAL_INSTALLDEV_DIR)$(TARGET_INSTALLDEV_DOCS_DIR)
LOCAL_INSTALLDEV_SHARE_DIR?=$(LOCAL_INSTALLDEV_DIR)$(TARGET_INSTALLDEV_SHARE_DIR)
LOCAL_INSTALLDEV_ETC_DIR?=$(LOCAL_INSTALLDEV_DIR)$(TARGET_INSTALLDEV_ETC_DIR)

LOCAL_INSTALLDOCSDEV_DIR?=$(BUILD_DIR)/$(PROJECT)-$(PROJECT_VERSION)-docs-dev
TARGET_INSTALLDOCSDEV_DOCS_DIR?=$(TARGET_INSTALLDEV_DOCS_DIR)
LOCAL_INSTALLDOCSDEV_DOCS_DIR?=$(LOCAL_INSTALLDOCSDEV_DIR)$(TARGET_INSTALLDOCSDEV_DOCS_DIR)

INSTALL_DIR?=$(TARGET_INSTALL_DIR)
INSTALL_BIN_DIR?=$(INSTALL_DIR)/$(TARGET_BIN_DIR)
INSTALL_LIB_DIR?=$(INSTALL_DIR)/$(TARGET_LIB_DIR)
INSTALL_INCLUDE_DIR?=$(INSTALL_DIR)/$(TARGET_INCLUDE_DIR)
INSTALL_DOCS_DIR?=$(INSTALL_DIR)/$(TARGET_DOCS_DIR)
INSTALL_SHARE_DIR?=$(INSTALL_DIR)/$(TARGET_SHARE_DIR)
INSTALL_ETC_DIR?=$(INSTALL_DIR)/$(TARGET_ETC_DIR)

INSTALLDEV_DIR=$(TARGET_INSTALLDEV_DIR)
INSTALLDEV_BIN_DIR?=$(INSTALLDEV_DIR)/$(TARGET_BIN_DIR)
INSTALLDEV_LIB_DIR?=$(INSTALLDEV_DIR)/$(TARGET_LIB_DIR)
INSTALLDEV_INCLUDE_DIR?=$(INSTALLDEV_DIR)/$(TARGET_INCLUDE_DIR)
INSTALLDEV_DOCS_DIR?=$(INSTALLDEV_DIR)/$(TARGET_DOCS_DIR)
INSTALLDEV_SHARE_DIR?=$(INSTALLDEV_DIR)/$(TARGET_SHARE_DIR)
INSTALLDEV_ETC_DIR?=$(INSTALLDEV_DIR)/$(TARGET_ETC_DIR)
INSTALLDOCSDEV_DOCS_DIR?=$(INSTALLDEV_DIR)/$(TARGET_DOCS_DIR)

PACKAGES_DIR?="$(BUILD_DIR)/packages"

ALL_OUTPUT_DIRS+=$(OUTPUT_LIB_DIR) $(OUTPUT_TOOLS_DIR) $(OUTPUT_TESTS_DIR) $(OUTPUT_DOCS_DIR) $(OUTPUT_DOCS_DEV_DIR) $(OUTPUT_EXAMPLES_DIR) $(OUTPUT_OBJ_DIR) $(OUTPUT_TOOLS_DEV_DIR) $(OUTPUT_SHARE_DIR) $(OUTPUT_ETC_DIR) $(PACKAGES_DIR)

########################################################################
# magic-vpaths.mk
#
# our vpaths
#

# all o files in $(OUTPUT_OBJ_DIR)
vpath %.o $(OUTPUT_OBJ_DIR)

# all source files in all of our src,tests,examples,tools,gui dirs

ALL_SRC_DIRS=$(LIB_SRC_DIR) $(LIB_TESTS_DIR) $(LIB_EXAMPLES_DIR) $(LIB_EXAMPLES_DEV_DIR) $(LIB_TOOLS_DIR) $(LIB_TOOLS_DEV_DIR) 

vpath %.tex $(LIB_DOCS_DIR)
vpath %.m $(ALL_SRC_DIRS)

vpath %.mm $(ALL_SRC_DIRS)
vpath %.cpp $(ALL_SRC_DIRS)
vpath %.cc $(ALL_SRC_DIRS)
vpath %.c $(ALL_SRC_DIRS)
vpath %.rc $(ALL_SRC_DIRS)

# all h files in our include dirs
vpath %.h .:$(LIB_INCLUDE_DIR)

# all libraries in our OUTPUT_LIB_DIR
vpath %.a $(OUTPUT_LIB_DIR)
vpath %.so $(OUTPUT_LIB_DIR)
vpath %.dylib $(OUTPUT_LIB_DIR)

# all testing shell scripts in our tests source dir
vpath %.sh $(LIB_TESTS_DIR)

# all object files are precious. Make should not delete them even if they are intermediate
.PRECIOUS : $(OUTPUT_OBJ_DIR)/%.o

# If we are cross compiling, then we must also look in our native dirs for objects and libraries.
ifeq ($(CROSS_COMPILING),1)
vpath %.o $(NATIVE_OUTPUT_OBJ_DIR)
vpath %.a $(NATIVE_OUTPUT_LIB_DIR)
.PRECIOUS : $(NATIVE_OUTPUT_OBJ_DIR)/%.o
endif

vpath %.a $(OUTPUT_LIB_DIR) $(PREFIX)/lib


########################################################################
# magic-rules.mk
#
# Make's rules.


# Remove make's built-in rules that we do not want

(%.o) : %.o

%.o : %.cpp

%.o : %.cc

%.o : %.rc

%.o : %.c

%.o : %.m

%.o : %.mm

% : %.in

%$(EXE) : %.cpp

%$(EXE) : %.cc

%$(EXE) : %.c

%$(EXE) : %.m

%$(EXE) : %.mm

COMPILE.cpp=$(CXX) $(CXXFLAGS) -c
COMPILE.cc=$(CXX) $(CXXFLAGS) -c
COMPILE.c=$(CC) $(CFLAGS) -c
COMPILE.mm=$(CXX) $(MMFLAGS) -c
COMPILE.m=$(CC) $(MFLAGS) -c
COMPILE.rc=$(WINDRES) 

LINK.cpp=$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $(LDLIBS_PACKAGES)
LINK.cc=$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $(LDLIBS_PACKAGES)
LINK.c=$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(LDLIBS_PACKAGES)
LINK.mm=$(CXX) $(MMFLAGS) $(LDFLAGS) $(LDLIBS) $(LDLIBS_PACKAGES)
LINK.m=$(CC) $(MFLAGS) $(LDFLAGS) $(LDLIBS) $(LDLIBS_PACKAGES)

NATIVE_COMPILE.cpp=$(NATIVE_CXX) $(NATIVE_CXXFLAGS) -c
NATIVE_COMPILE.cc=$(NATIVE_CX) $(NATIVE_CXXFLAGS) -c
NATIVE_COMPILE.c=$(NATIVE_CC) $(NATIVE_CFLAGS) -c
NATIVE_COMPILE.mm=$(NATIVE_CXX) $(NATIVE_MMFLAGS) -c
NATIVE_COMPILE.m=$(NATIVE_CC) $(NATIVE_MFLAGS) -c
NATIVE_COMPILE.rc=$(NATIVE_WINDRES)

NATIVE_LINK.cpp=$(NATIVE_CXX) $(NATIVE_CXXFLAGS) $(NATIVE_LDFLAGS) $(NATIVE_LDLIBS)
NATIVE_LINK.cc=$(NATIVE_CXX) $(NATIVE_CXXFLAGS) $(NATIVE_LDFLAGS) $(NATIVE_LDLIBS)
NATIVE_LINK.c=$(NATIVE_CC) $(NATIVE_CFLAGS) $(NATIVE_LDFLAGS) $(NATIVE_LDLIBS)
NATIVE_LINK.mm=$(NATIVE_CXX) $(NATIVE_MMFLAGS) $(NATIVE_LDFLAGS) $(NATIVE_LDLIBS)
NATIVE_LINK.m=$(NATIVE_CC) $(NATIVE_MFLAGS) $(NATIVE_LDFLAGS) $(NATIVE_LDLIBS)


########################################################################
# 
# Replace make's original rules  with our special rules, which place output objects 
# directly in $(OUTPUT_OBJ_DIR), and also perform include file dependancy file creation.

# For Objective C++:
$(OUTPUT_OBJ_DIR)/%.o $(OUTPUT_OBJ_DIR)/%.d : %.mm
	@echo "CXX mm : $(notdir $<)"
	@$(CXX) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $(MMFLAGS) $(DEPENDENCY_OPTIONS) -MT $(OUTPUT_OBJ_DIR)/$*.o -MF $(OUTPUT_OBJ_DIR)/$*.d $< &&	$(COMPILE.cpp) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $(COMPILE_FLAGS) $(LOCAL_COMPILE_FLAGS) -o $(OUTPUT_OBJ_DIR)/$*.o $< 

# For Objective C:
$(OUTPUT_OBJ_DIR)/%.o $(OUTPUT_OBJ_DIR)/%.d : %.m
	@echo "CC  m  : $(notdir $<)"
	@$(CC) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $(MFLAGS) $(DEPENDENCY_OPTIONS) -MT  $(OUTPUT_OBJ_DIR)/$*.o -MF $(OUTPUT_OBJ_DIR)/$*.d $< &&	$(COMPILE.c) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $(COMPILE_FLAGS) $(LOCAL_COMPILE_FLAGS) -o $(OUTPUT_OBJ_DIR)/$*.o $<

# For C++: (cpp)
$(OUTPUT_OBJ_DIR)/%.o $(OUTPUT_OBJ_DIR)/%.d : %.cpp
	@echo "CXX    : $(notdir $<)"
	@$(CXX) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS)  $(DEPENDENCY_OPTIONS) -MT $(OUTPUT_OBJ_DIR)/$*.o -MF $(OUTPUT_OBJ_DIR)/$*.d $< && $(COMPILE.cpp) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $(COMPILE_FLAGS) $(LOCAL_COMPILE_FLAGS) -o $(OUTPUT_OBJ_DIR)/$*.o $<

# Asm For C++: (cpp)
$(OUTPUT_OBJ_DIR)/%.asm : %.cpp
	@echo "CXX asm  : $(notdir $<)"
	@$(COMPILE.cc) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $(COMPILE_FLAGS) -S -o $(OUTPUT_OBJ_DIR)/$*.asm $<

# For C++: (cc)
$(OUTPUT_OBJ_DIR)/%.o $(OUTPUT_OBJ_DIR)/%.d : %.cc
	@echo "CXX    : $(notdir $<)"
	@$(CXX) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS)  $(DEPENDENCY_OPTIONS) -MT  $(OUTPUT_OBJ_DIR)/$*.o -MF $(OUTPUT_OBJ_DIR)/$*.d $< && $(COMPILE.cc) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $(COMPILE_FLAGS) $(LOCAL_COMPILE_FLAGS) -o $(OUTPUT_OBJ_DIR)/$*.o $<

# Asm For C++: (cc)
$(OUTPUT_OBJ_DIR)/%.asm : %.cc
	@echo "CXX asm  : $(notdir $<)"
	@$(COMPILE.cc) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $(COMPILE_FLAGS) -S -o $(OUTPUT_OBJ_DIR)/$*.asm $<

# For C:
$(OUTPUT_OBJ_DIR)/%.o $(OUTPUT_OBJ_DIR)/%.d : %.c
	@echo "CC     : $(notdir $<)"
	@$(CC) $(SORTED_PREPROCESS_FLAGS)  $(LOCAL_PREPROCESS_FLAGS) $(DEPENDENCY_OPTIONS) -MT  $(OUTPUT_OBJ_DIR)/$*.o -MF $(OUTPUT_OBJ_DIR)/$*.d $< && $(COMPILE.c) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $(COMPILE_FLAGS) $(LOCAL_COMPILE_FLAGS) -o $(OUTPUT_OBJ_DIR)/$*.o $<

# Asm For C:
$(OUTPUT_OBJ_DIR)/%.asm : %.c
	@echo "C asm  : $(notdir $<)"
	@$(COMPILE.c) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $(COMPILE_FLAGS) -S -o $(OUTPUT_OBJ_DIR)/$*.asm $<

# For RC (windows):
$(OUTPUT_OBJ_DIR)/%.o : %.rc
	@echo "WINDRES     : $(notdir $<)"
	@$(COMPILE.rc) -I$(dir $<) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $< $@

ifeq ($(TARGET_PLATFORM_MACOSX_UNIVERSAL),1)
# For disassembly of object files for mac 'fat' binaries, i386 and ppc archs
$(OUTPUT_OBJ_DIR)/%.disasm : $(OUTPUT_OBJ_DIR)/%.o
	@echo "DISASM (2)  : $(notdir $<)"
	@echo '# disassembly of ' $(notdir $<) >$(OUTPUT_OBJ_DIR)/$*.disasm
	@for i in $(MACOSX_UNIVERSAL_ARCHS); do \
		echo '#---------- ' $$i >>$(OUTPUT_OBJ_DIR)/$*.disasm; \
		$(LIPO) -thin $$i $< -o $(OUTPUT_OBJ_DIR)/$*.$$i.o && \
		$(DISASM) $(DISASM_FLAGS) $(OUTPUT_OBJ_DIR)/$*.$$i.o >>$(OUTPUT_OBJ_DIR)/$*.disasm; \
		$(RM) $(OUTPUT_OBJ_DIR)/$*.$$i.o; \
	done
else
# For disassembly of object files
$(OUTPUT_OBJ_DIR)/%.disasm : $(OUTPUT_OBJ_DIR)/%.o
	@echo "DISASM      : $(notdir $<)"
	@$(OBJDUMP) $(OBJDUMP_FLAGS) $< >$(OUTPUT_OBJ_DIR)/$*.disasm
endif


$(OUTPUT_OBJ_DIR)/% : %.in $(OUTPUT_OBJ_DIR)/m4_defs.txt
	@echo "M4 : $(notdir $<)"
	@$(M4) $(OUTPUT_OBJ_DIR)/m4_defs.txt $< $>$(OUTPUT_OBJ_DIR)/$


# For Objective C++:
$(OUTPUT_SHOBJ_DIR)/%.o $(OUTPUT_SHOBJ_DIR)/%.d : %.mm
	@echo "SHLIB CXX mm : $(notdir $<)"
	@$(CXX) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $(MMFLAGS) $(DEPENDENCY_OPTIONS) -MT $(OUTPUT_SHOBJ_DIR)/$*.o -MF $(OUTPUT_SHOBJ_DIR)/$*.d $< &&	$(COMPILE.cpp) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS) $(LOCAL_PREPROCESS_FLAGS) $(SHLIB_COMPILE_FLAGS) $(COMPILE_FLAGS) $(LOCAL_COMPILE_FLAGS) -o $(OUTPUT_SHOBJ_DIR)/$*.o $< 

# For Objective C:
$(OUTPUT_SHOBJ_DIR)/%.o $(OUTPUT_SHOBJ_DIR)/%.d : %.m
	@echo "SHLIB CC  m  : $(notdir $<)"
	@$(CC) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS)  $(LOCAL_PREPROCESS_FLAGS) $(MFLAGS) $(DEPENDENCY_OPTIONS) -MT  $(OUTPUT_SHOBJ_DIR)/$*.o -MF $(OUTPUT_SHOBJ_DIR)/$*.d $< &&	$(COMPILE.c) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS)  $(LOCAL_PREPROCESS_FLAGS) $(SHLIB_COMPILE_FLAGS) $(COMPILE_FLAGS)  $(LOCAL_COMPILE_FLAGS) -o $(OUTPUT_SHOBJ_DIR)/$*.o $<

# For C++: (cpp)
$(OUTPUT_SHOBJ_DIR)/%.o $(OUTPUT_SHOBJ_DIR)/%.d : %.cpp
	@echo "SHLIB CXX    : $(notdir $<)"
	@$(CXX) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS)  $(LOCAL_PREPROCESS_FLAGS)  $(DEPENDENCY_OPTIONS) -MT $(OUTPUT_SHOBJ_DIR)/$*.o -MF $(OUTPUT_SHOBJ_DIR)/$*.d $< && $(COMPILE.cpp) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS)  $(LOCAL_PREPROCESS_FLAGS) $(SHLIB_COMPILE_FLAGS) $(COMPILE_FLAGS)  $(LOCAL_COMPILE_FLAGS) -o $(OUTPUT_SHOBJ_DIR)/$*.o $<

# Asm For C++: (cpp)
$(OUTPUT_SHOBJ_DIR)/%.asm : %.cpp
	@echo "SHLIB CXX asm  : $(notdir $<)"
	@$(COMPILE.cc) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS)  $(LOCAL_PREPROCESS_FLAGS) $(SHLIB_COMPILE_FLAGS) $(COMPILE_FLAGS)  -S -o $(OUTPUT_SHOBJ_DIR)/$*.asm $<

# For C++: (cc)
$(OUTPUT_SHOBJ_DIR)/%.o $(OUTPUT_SHOBJ_DIR)/%.d : %.cc
	@echo "SHLIB CXX    : $(notdir $<)"
	@$(CXX) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS)  $(LOCAL_PREPROCESS_FLAGS)  $(DEPENDENCY_OPTIONS) -MT  $(OUTPUT_SHOBJ_DIR)/$*.o -MF $(OUTPUT_SHOBJ_DIR)/$*.d $< && $(COMPILE.cc) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS)  $(LOCAL_PREPROCESS_FLAGS) $(SHLIB_COMPILE_FLAGS) $(COMPILE_FLAGS)  $(LOCAL_COMPILE_FLAGS) -o $(OUTPUT_SHOBJ_DIR)/$*.o $<

# Asm For C++: (cc)
$(OUTPUT_SHOBJ_DIR)/%.asm : %.cc
	@echo "SHLIB CXX asm  : $(notdir $<)"
	@$(COMPILE.cc) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS)  $(LOCAL_PREPROCESS_FLAGS) $(SHLIB_COMPILE_FLAGS) $(COMPILE_FLAGS)  -S -o $(OUTPUT_SHOBJ_DIR)/$*.asm $<

# For C:
$(OUTPUT_SHOBJ_DIR)/%.o $(OUTPUT_SHOBJ_DIR)/%.d : %.c
	@echo "SHLIB CC     : $(notdir $<)"
	@$(CC) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS)   $(LOCAL_PREPROCESS_FLAGS) $(DEPENDENCY_OPTIONS) -MT  $(OUTPUT_SHOBJ_DIR)/$*.o -MF $(OUTPUT_SHOBJ_DIR)/$*.d $< && $(COMPILE.c) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS)  $(LOCAL_PREPROCESS_FLAGS) $(SHLIB_COMPILE_FLAGS) $(COMPILE_FLAGS)  $(LOCAL_COMPILE_FLAGS) -o $(OUTPUT_SHOBJ_DIR)/$*.o $<

# Asm For C:
$(OUTPUT_SHOBJ_DIR)/%.asm : %.c
	@echo "SHLIB C asm  : $(notdir $<)"
	@$(COMPILE.c) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS)  $(LOCAL_PREPROCESS_FLAGS) $(SHLIB_COMPILE_FLAGS) $(COMPILE_FLAGS)  -S -o $(OUTPUT_SHOBJ_DIR)/$*.asm $<

# For RC (windows):
$(OUTPUT_SHOBJ_DIR)/%.o : %.rc
	@echo "SHLIB WINDRES     : $(notdir $<)"
	@$(COMPILE.rc) -I$(dir $<) $(SHLIB_PREPROCESS_FLAGS) $(SORTED_PREPROCESS_FLAGS)  $(LOCAL_PREPROCESS_FLAGS) $< $@

ifeq ($(TARGET_PLATFORM_MACOSX_UNIVERSAL),1)
# For disassembly of object files for mac 'fat' binaries, i386 and ppc archs
$(OUTPUT_SHOBJ_DIR)/%.disasm : $(OUTPUT_SHOBJ_DIR)/%.o
	@echo "SHLIB DISASM (2)  : $(notdir $<)"
	@echo '# disassembly of ' $(notdir $<) >$(OUTPUT_SHOBJ_DIR)/$*.disasm
	@for i in $(MACOSX_UNIVERSAL_ARCHS); do \
		echo '#---------- ' $$i >>$(OUTPUT_SHOBJ_DIR)/$*.disasm; \
		$(LIPO) -thin $$i $< -o $(OUTPUT_SHOBJ_DIR)/$*.$$i.o && \
		$(DISASM) $(DISASM_FLAGS) $(OUTPUT_SHOBJ_DIR)/$*.$$i.o >>$(OUTPUT_SHOBJ_DIR)/$*.disasm; \
		$(RM) $(OUTPUT_SHOBJ_DIR)/$*.$$i.o; \
	done
else
# For disassembly of object files
$(OUTPUT_SHOBJ_DIR)/%.disasm : $(OUTPUT_SHOBJ_DIR)/%.o
	@echo "SHLIB DISASM      : $(notdir $<)"
	@$(OBJDUMP) $(OBJDUMP_FLAGS) $< >$(OUTPUT_SHOBJ_DIR)/$*.disasm
endif


$(OUTPUT_SHOBJ_DIR)/% : %.in $(OUTPUT_SHOBJ_DIR)/m4_defs.txt
	@echo "SHLIB M4 : $(notdir $<)"
	@$(M4) $(OUTPUT_SHOBJ_DIR)/m4_defs.txt $< $>$(OUTPUT_SHOBJ_DIR)/$



ifeq ($(CROSS_COMPILING),1)

$(NATIVE_OUTPUT_OBJ_DIR)/%.o $(NATIVE_OUTPUT_OBJ_DIR)/%.d : %.mm
	@echo "NATIVE_CXX mm : $(notdir $<)"
	@$(NATIVE_CXX) $(SORTED_NATIVE_PREPROCESS_FLAGS) $(LOCAL_NATIVE_PREPROCESS_FLAGS) $(DEPENDENCY_OPTIONS) $(NATIVE_OUTPUT_OBJ_DIR)/$*.o -MF $(NATIVE_OUTPUT_OBJ_DIR)/$*.d $< && $(NATIVE_COMPILE.mm) $(SORTED_NATIVE_PREPROCESS_FLAGS) $(LOCAL_NATIVE_PREPROCESS_FLAGS) $(NATIVE_COMPILE_FLAGS) $(LOCAL_NATIVE_COMPILE_FLAGS) -o $(NATIVE_OUTPUT_OBJ_DIR)/$*.o $<

$(NATIVE_OUTPUT_OBJ_DIR)/%.o $(NATIVE_OUTPUT_OBJ_DIR)/%.d : %.m
	@echo "NATIVE_CC  m  : $(notdir $<)"
	@$(NATIVE_CC) $(SORTED_NATIVE_PREPROCESS_FLAGS) $(LOCAL_NATIVE_PREPROCESS_FLAGS) $(DEPENDENCY_OPTIONS) -MT $(NATIVE_OUTPUT_OBJ_DIR)/$*.o -MF $(NATIVE_OUTPUT_OBJ_DIR)/$*.d $< && $(NATIVE_COMPILE.m) $(SORTED_NATIVE_PREPROCESS_FLAGS) $(LOCAL_NATIVE_PREPROCESS_FLAGS) $(NATIVE_COMPILE_FLAGS) $(LOCAL_NATIVE_COMPILE_FLAGS) -o $(NATIVE_OUTPUT_OBJ_DIR)/$*.o $<)

$(NATIVE_OUTPUT_OBJ_DIR)/%.o $(NATIVE_OUTPUT_OBJ_DIR)/%.d : %.cpp
	@echo "NATIVE_CXX    : $(notdir $<)"
	@$(NATIVE_CXX) $(SORTED_NATIVE_PREPROCESS_FLAGS)  $(DEPENDENCY_OPTIONS) -MT $(NATIVE_OUTPUT_OBJ_DIR)/$*.o -MF $(NATIVE_OUTPUT_OBJ_DIR)/$*.d $< && $(NATIVE_COMPILE.cpp) $(SORTED_NATIVE_PREPROCESS_FLAGS) $(LOCAL_NATIVE_PREPROCESS_FLAGS) $(NATIVE_COMPILE_FLAGS) $(LOCAL_NATIVE_COMPILE_FLAGS) -o $(NATIVE_OUTPUT_OBJ_DIR)/$*.o $<

$(NATIVE_OUTPUT_OBJ_DIR)/%.o $(NATIVE_OUTPUT_OBJ_DIR)/%.d : %.cc
	@echo "NATIVE_CXX    : $(notdir $<)"
	@$(NATIVE_CXX) $(SORTED_NATIVE_PREPROCESS_FLAGS) $(LOCAL_NATIVE_PREPROCESS_FLAGS)  $(DEPENDENCY_OPTIONS) -MT  $(NATIVE_OUTPUT_OBJ_DIR)/$*.o -MF $(NATIVE_OUTPUT_OBJ_DIR)/$*.d $< && $(NATIVE_COMPILE.cpp) $(SORTED_NATIVE_PREPROCESS_FLAGS) $(LOCAL_NATIVE_PREPROCESS_FLAGS) $(NATIVE_COMPILE_FLAGS) $(LOCAL_NATIVE_COMPILE_FLAGS) -o $(NATIVE_OUTPUT_OBJ_DIR)/$*.o $<

$(NATIVE_OUTPUT_OBJ_DIR)/%.o $(NATIVE_OUTPUT_OBJ_DIR)/%.d : %.c
	@echo "NATIVE_CC     : $(notdir $<)"
	@$(NATIVE_CC) $(SORTED_NATIVE_PREPROCESS_FLAGS) $(LOCAL_NATIVE_PREPROCESS_FLAGS)  $(DEPENDENCY_OPTIONS) -MT $(NATIVE_OUTPUT_OBJ_DIR)/$*.o -MF $(NATIVE_OUTPUT_OBJ_DIR)/$*.d $< && $(NATIVE_COMPILE.c)  $(SORTED_NATIVE_PREPROCESS_FLAGS) $(LOCAL_NATIVE_PREPROCESS_FLAGS) $(NATIVE_COMPILE_FLAGS) $(LOCAL_NATIVE_COMPILE_FLAGS) -o $(NATIVE_OUTPUT_OBJ_DIR)/$*.o $<

$(NATIVE_OUTPUT_OBJ_DIR)/%.o : %.rc
	@echo "NATIVE_WINDRES     : $(notdir $<)"
	@$(NATIVE_COMPILE.rc) -I$(dir $<) $(SORTED_NATIVE_PREPROCESS_FLAGS) $(LOCAL_NATIVE_PREPROCESS_FLAGS) $< $@
endif

$(OUTPUT_TOOLS_DIR)/%$(EXE) : $(OUTPUT_OBJ_DIR)/%.o $(LDLIBS_NO_OPTS)
	@echo "LINKING tool: $(notdir $<)"
	@$(LINK.cpp) $(LOCAL_LINK_FLAGS) $(LINK_FLAGS) $(LDFLAGS) -o $(OUTPUT_TOOLS_DIR)/$*$(EXE) $< -L$(OUTPUT_LIB_DIR) $(PROJECT_LDLIB) $(LDLIBS) $(LDFLAGS_PACKAGES) $(LDLIBS_PACKAGES) $(PROJECT_LDLIB) $(LDLIBS) $(LDFLAGS_PACKAGES) $(LDLIBS_PACKAGES)

$(OUTPUT_TOOLS_DEV_DIR)/%$(EXE) : $(OUTPUT_OBJ_DIR)/%.o $(LDLIBS_NO_OPTS)
	@echo "LINKING tool-dev: $(notdir $<)"
	@$(LINK.cpp) $(LOCAL_LINK_FLAGS) $(LINK_FLAGS) $(LDFLAGS) -o $(OUTPUT_TOOLS_DEV_DIR)/$*$(EXE) $< -L$(OUTPUT_LIB_DIR) $(PROJECT_LDLIB) $(LDLIBS) $(LDFLAGS_PACKAGES) $(LDLIBS_PACKAGES) $(PROJECT_LDLIB) $(LDLIBS) $(LDFLAGS_PACKAGES) $(LDLIBS_PACKAGES)

$(OUTPUT_EXAMPLES_DIR)/%$(EXE) : $(OUTPUT_OBJ_DIR)/%.o $(LDLIBS_NO_OPTS)
	@echo "LINKING example: $(notdir $<)"
	@$(LINK.cpp) $(LOCAL_LINK_FLAGS) $(LINK_FLAGS) $(LDFLAGS) -o $(OUTPUT_EXAMPLES_DIR)/$*$(EXE) $< -L$(OUTPUT_LIB_DIR) $(PROJECT_LDLIB) $(LDLIBS) $(LDFLAGS_PACKAGES) $(LDLIBS_PACKAGES) $(PROJECT_LDLIB) $(LDLIBS) $(LDFLAGS_PACKAGES) $(LDLIBS_PACKAGES)

$(OUTPUT_EXAMPLES_DEV_DIR)/%$(EXE) : $(OUTPUT_OBJ_DIR)/%.o $(LDLIBS_NO_OPTS)
	@echo "LINKING example dev: $(notdir $<)"
	@$(LINK.cpp) $(LOCAL_LINK_FLAGS) $(LINK_FLAGS) $(LDFLAGS) -o $(OUTPUT_EXAMPLES_DEV_DIR)/$*$(EXE) $< -L$(OUTPUT_LIB_DIR) $(PROJECT_LDLIB) $(LDLIBS) $(LDFLAGS_PACKAGES) $(LDLIBS_PACKAGES) $(PROJECT_LDLIB) $(LDLIBS) $(LDFLAGS_PACKAGES) $(LDLIBS_PACKAGES)

$(OUTPUT_TESTS_DIR)/%$(EXE) : $(OUTPUT_OBJ_DIR)/%.o $(LDLIBS_NO_OPTS)
	@echo "LINKING test: $(notdir $<)"
	@$(LINK.cpp) $(LOCAL_LINK_FLAGS) $(LINK_FLAGS) $(LDFLAGS) -o $(OUTPUT_TESTS_DIR)/$*$(EXE) $< -L$(OUTPUT_LIB_DIR) $(PROJECT_LDLIB) $(LDLIBS) $(LDFLAGS_PACKAGES) $(LDLIBS_PACKAGES) $(PROJECT_LDLIB) $(LDLIBS) $(LDFLAGS_PACKAGES) $(LDLIBS_PACKAGES)


ifeq ($(CROSS_COMPILING),1)

$(NATIVE_OUTPUT_TOOLS_DIR)/%$(NATIVE_EXE) : $(NATIVE_OUTPUT_OBJ_DIR)/%.o
	@echo "NATIVE_LINKING tool: $(notdir $<)"
	@$(NATIVE_LINK.cpp) $(NATIVE_LINK_FLAGS) $(NATIVE_LDFLAGS) -o $(NATIVE_OUTPUT_TOOLS_DIR)/$*$(NATIVE_EXE) $< -L$(NATIVE_OUTPUT_LIB_DIR) $(NATIVE_PROJECT_LDLIB) $(NATIVE_LDLIBS)

$(NATIVE_OUTPUT_TOOLS_DEV_DIR)/%$(NATIVE_EXE) : $(NATIVE_OUTPUT_OBJ_DIR)/%.o
	@echo "NATIVE_LINKING tool-dev: $(notdir $<)"
	@$(NATIVE_LINK.cpp) $(NATIVE_LINK_FLAGS) $(NATIVE_LDFLAGS) -o $(NATIVE_OUTPUT_TOOLS_DEV_DIR)/$*$(NATIVE_EXE) $< -L$(NATIVE_OUTPUT_LIB_DIR) $(NATIVE_PROJECT_LDLIB) $(NATIVE_LDLIBS)

$(NATIVE_OUTPUT_EXAMPLES_DIR)/%$(NATIVE_EXE) : $(NATIVE_OUTPUT_OBJ_DIR)/%.o
	@echo "NATIVE_LINKING example: $(notdir $<)"
	@$(NATIVE_LINK.cpp) $(NATIVE_LINK_FLAGS) $(NATIVE_LDFLAGS) -o $(NATIVE_OUTPUT_EXAMPLES_DIR)/$*$(NATIVE_EXE) $< -L$(NATIVE_OUTPUT_LIB_DIR) $(NATIVE_PROJECT_LDLIB) $(NATIVE_LDLIBS)

$(NATIVE_OUTPUT_TESTS_DIR)/%$(NATIVE_EXE) : $(NATIVE_OUTPUT_OBJ_DIR)/%.o
	@echo "NATIVE_LINKING test: $(notdir $<)"
	@$(NATIVE_LINK.cpp) $(NATIVE_LINK_FLAGS) $(NATIVE_LDFLAGS) -o $(NATIVE_OUTPUT_TESTS_DIR)/$*$(NATIVE_EXE) $< -L$(NATIVE_OUTPUT_LIB_DIR) $(NATIVE_PROJECT_LDLIB) $(NATIVE_LDLIBS)
endif


########################################################################
# magic-firsttarget.mk
# this first target, 'everything' is a placeholder which makes the required subdirectories and then
# calls make again with the required directories made. Since these subdirectories are part of the
# search paths, make must see them when invoked otherwise it gets confused.

.PHONY : everything


ifeq ($(CROSS_COMPILING),1)
everything : native-dirs dirs
	@$(MAKE) all
else
everything : dirs
	@$(MAKE) all
endif

########################################################################
# magic-configtool.mk
# config tool generation options

# CONFIG_TOOL_FILE is the full path name of the generated config tool script
CONFIG_TOOL_FILE=$(OUTPUT_TOOLS_DEV_DIR)/$(PROJECT_CONFIG_TOOL)

LOCAL_CONFIG_TOOL_FILE=$(PROJECT_CONFIG_TOOL)

# CONFIG_TOOL_PREPROCESS_FLAGS is the preprocessor flags that the config tool script 
# will output when given --cppflags
CONFIG_TOOL_PREPROCESS_FLAGS+=$(sort $(addprefix -I,$(TARGET_INSTALLDEV_INCLUDE_DIR)) $(addprefix -D,$(DEFINES)))

LOCAL_CONFIG_TOOL_PREPROCESS_FLAGS+=$(sort $(addprefix -I,$(LIB_INCLUDE_DIR)) $(addprefix -D,$(DEFINES)))


# CONFIG_TOOL_COMPILE_FLAGS is the full compile flags that the config tool script 
# will output when given --cflags or --cxxflags
CONFIG_TOOL_COMPILE_FLAGS+=$(WARNINGS_FLAGS) $(OPTIMIZE_FLAGS) $(CONFIG_TOOL_PREPROCESS_FLAGS) $(COMPILE_FLAGS) 

LOCAL_CONFIG_TOOL_COMPILE_FLAGS+=$(WARNINGS_FLAGS) $(OPTIMIZE_FLAGS) $(LOCAL_CONFIG_TOOL_PREPROCESS_FLAGS)  $(COMPILE_FLAGS)


PREPROCESS_FLAGS+=-DCONFIG_INSTALL_DIR=\"$(TARGET_INSTALL_DIR)\" \
	-DCONFIG_BIN_DIR=\"$(TARGET_BIN_DIR)\" \
	-DCONFIG_LIB_DIR=\"$(TARGET_LIB_DIR)\" \
	-DCONFIG_DOCS_DIR=\"$(TARGET_DOCS_DIR)\" \
	-DCONFIG_SHARE_DIR=\"$(TARGET_SHARE_DIR)\" \
	-DCONFIG_ETC_DIR=\"$(TARGET_ETC_DIR)\" \
	-DCONFIG_INSTALLDEV_DIR=\"$(TARGET_INSTALLDEV_DIR)\" 

ifeq ($(CROSS_COMPILING),1)
NATIVE_PREPROCESS_FLAGS+=-DCONFIG_INSTALL_DIR=\"$(NATIVE_OUTPUT_DIR)\" \
	-DCONFIG_BIN_DIR=\"bin\" \
	-DCONFIG_LIB_DIR=\"lib\" \
	-DCONFIG_DOCS_DIR=\"docs\" \
	-DCONFIG_SHARE_DIR=\"share\" \
	-DCONFIG_ETC_DIR=\"etc\" 
endif

.PHONY : config-tool

config-tool :  $(CONFIG_TOOL_FILE)

$(CONFIG_TOOL_FILE) : $(LIB_O_FILES)
	@-rm -f $(CONFIG_TOOL_FILE)
	@echo '#!/bin/bash' >$(CONFIG_TOOL_FILE)
	@echo 'for i in $$*; do' >>$(CONFIG_TOOL_FILE)
	@echo ' case $$i in ' >>$(CONFIG_TOOL_FILE)
	@echo '  (--help) echo "Usage: --help --version --prefix --compiler-prefix --includes --defines --libs --cflags --cxxflags --cppflags --mflags --mmflags --cc --cxx --ldflags --share-dir " ;;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--version) echo "$(PROJECT_NAME) version $(PROJECT_VERSION) config tool. " ;;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--prefix) echo -n "$(TARGET_INSTALLDEV_DIR) " ;;' >>$(CONFIG_TOOL_FILE)	
	@echo '  (--libs) echo -n "$(TARGET_INSTALLDEV_LIB_DIR)/lib$(PROJECT).a";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--defines) echo -n "$(addprefix -D,$(DEFINES)) ";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--includes) echo -n "$(addprefix -I,$(TARGET_INSTALLDEV_INCLUDE_DIR)) ";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--cflags) echo -n "$(CFLAGS) $(CONFIG_TOOL_COMPILE_FLAGS) ";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--cxxflags) echo -n "$(CXXFLAGS) $(CONFIG_TOOL_COMPILE_FLAGS) ";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--cppflags) echo -n "$(CONFIG_TOOL_PREPROCESS_FLAGS) ";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--mflags) echo -n "$(MFLAGS) $(CONFIG_TOOL_COMPILE_FLAGS) ";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--mmflags) echo -n "$(MMFLAGS) $(CONFIG_TOOL_COMPILE_FLAGS) ";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--cc) echo -n "$(CC) ";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--cxx) echo -n "$(CXX) ";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--ldflags) echo -n "$(LINK_FLAGS) ";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--compiler-prefix) echo -n "$(COMPILER_PREFIX)";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--share-dir) echo -n "$(TARGET_INSTALL_SHARE_DIR)";;' >>$(CONFIG_TOOL_FILE)
	@echo '  (--share-dev-dir) echo -n "$(TARGET_INSTALLDEV_SHARE_DIR)";;' >>$(CONFIG_TOOL_FILE)
	@echo ' esac' >>$(CONFIG_TOOL_FILE)
	@echo 'done' >>$(CONFIG_TOOL_FILE)
	@echo 'echo' >>$(CONFIG_TOOL_FILE)
	@chmod +x $(CONFIG_TOOL_FILE)

.PHONY : local-config-tool

local-config-tool :$(LOCAL_CONFIG_TOOL_FILE)

$(LOCAL_CONFIG_TOOL_FILE) : $(LIB_O_FILES)
	@-rm -f $(LOCAL_CONFIG_TOOL_FILE)
	@echo '#!/bin/bash' >$(LOCAL_CONFIG_TOOL_FILE)
	@echo 'for i in $$*; do' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo ' case $$i in ' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--help) echo "Usage: --help --version --prefix --compiler-prefix --includes --defines --libs --cflags --cxxflags --cppflags --mflags --mmflags --cc --cxx --ldflags --share-dir " ;;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--version) echo "$(PROJECT_NAME) version $(PROJECT_VERSION) config tool. " ;;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--prefix) echo -n "$(OUTPUT_DIR) " ;;' >>$(LOCAL_CONFIG_TOOL_FILE)	
	@echo '  (--libs) echo -n "$(OUTPUT_LIB_DIR)/lib$(PROJECT).a";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--defines) echo -n "$(addprefix -D,$(DEFINES)) ";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--includes) echo -n "$(addprefix -I,$(LIB_INCLUDE_DIR)) ";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--cflags) echo -n "$(CFLAGS) $(LOCAL_CONFIG_TOOL_COMPILE_FLAGS) ";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--cxxflags) echo -n "$(CXXFLAGS) $(LOCAL_CONFIG_TOOL_COMPILE_FLAGS) ";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--cppflags) echo -n "$(LOCAL_CONFIG_TOOL_PREPROCESS_FLAGS) ";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--mflags) echo -n "$(MFLAGS) $(LOCAL_CONFIG_TOOL_COMPILE_FLAGS) ";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--mmflags) echo -n "$(MMFLAGS) $(LOCAL_CONFIG_TOOL_COMPILE_FLAGS) ";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--cc) echo -n "$(CC) ";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--cxx) echo -n "$(CXX) ";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--ldflags) echo -n "$(LINK_FLAGS) ";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--compiler-prefix) echo -n "$(COMPILER_PREFIX)";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--share-dir) echo -n "$(PROJECT_TOP_DIR)/share";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo '  (--share-dev-dir) echo -n "$(PROJECT_TOP_DIR)/share-dev";;' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo ' esac' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo 'done' >>$(LOCAL_CONFIG_TOOL_FILE)
	@echo 'echo' >>$(LOCAL_CONFIG_TOOL_FILE)
	@chmod +x $(LOCAL_CONFIG_TOOL_FILE)

########################################################################
# magic-native.mk

ifeq ($(NATIVE_PLATFORM_MINGW32),1)
SUFFIXES_NATIVE_PLATFORM=MINGW32 WIN32
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=.exe
RAW_NATIVE_PLATFORM_DIRS+=win32 mingw32
endif

ifeq ($(NATIVE_PLATFORM_CYGWIN),1)
SUFFIXES_NATIVE_PLATFORM=CYGWIN POSIX
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=.exe
RAW_NATIVE_PLATFORM_DIRS+=cygwin posix
endif

ifeq ($(NATIVE_PLATFORM_POSIX),1)
SUFFIXES_NATIVE_PLATFORM=POSIX
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=
RAW_NATIVE_PLATFORM_DIRS+=posix
endif

ifeq ($(NATIVE_PLATFORM_LINUX),1)
SUFFIXES_NATIVE_PLATFORM=POSIX LINUX
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=
RAW_NATIVE_PLATFORM_DIRS+=posix linux
endif

ifeq ($(NATIVE_PLATFORM_LINUX_I386),1)
SUFFIXES_NATIVE_PLATFORM=POSIX LINUX LINUX_I386
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=
RAW_NATIVE_PLATFORM_DIRS+=posix linux
endif

ifeq ($(NATIVE_PLATFORM_LINUX_X86_64),1)
SUFFIXES_NATIVE_PLATFORM=POSIX LINUX LINUX_I386 LINUX_X86_64
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=
RAW_NATIVE_PLATFORM_DIRS+=posix linux
NATIVE_COMPILE_FLAGS+=-m64
NATIVE_LINK_FLAGS+=-m64
endif

ifeq ($(NATIVE_PLATFORM_LINUX_PPC),1)
SUFFIXES_NATIVE_PLATFORM=POSIX LINUX LINUX_PPC
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=
RAW_NATIVE_PLATFORM_DIRS+=posix linux
endif

ifeq ($(NATIVE_PLATFORM_LINUX_PPC64),1)
SUFFIXES_NATIVE_PLATFORM=POSIX LINUX LINUX_PPC LINUX_PPC64
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=
RAW_NATIVE_PLATFORM_DIRS+=posix linux
NATIVE_COMPILE_FLAGS+=-m64
NATIVE_LINK_FLAGS+=-m64
endif


# bsd is a subset of posix
ifeq ($(NATIVE_PLATFORM_BSD),1)
SUFFIXES_NATIVE_PLATFORM=POSIX BSD
RAW_NATIVE_PLATFORM_DIRS+=bsd posix
endif

# freebsd is a subset of posix and bsd
ifeq ($(NATIVE_PLATFORM_FREEBSD),1)
SUFFIXES_NATIVE_PLATFORM=POSIX BSD FREEBSD
RAW_NATIVE_PLATFORM_DIRS+=bsd posix freebsd
endif

# openbsd is a subset of posix and bsd
ifeq ($(NATIVE_PLATFORM_OPENBSD),1)
SUFFIXES_NATIVE_PLATFORM=POSIX BSD OPENBSD
RAW_NATIVE_PLATFORM_DIRS+=bsd posix openbsd
endif

# netbsd is a subset of posix and bsd
ifeq ($(NATIVE_PLATFORM_NETBSD),1)
SUFFIXES_NATIVE_PLATFORM=POSIX BSD NETBSD
RAW_NATIVE_PLATFORM_DIRS+=bsd posix netbsd
endif

# solaris is a subset of posix 
ifeq ($(NATIVE_PLATFORM_SOLARIS),1)
SUFFIXES_NATIVE_PLATFORM=POSIX SOLARIS
RAW_NATIVE_PLATFORM_DIRS+=solaris posix 
endif


ifeq ($(NATIVE_PLATFORM_MACOSX),1)
SUFFIXES_NATIVE_PLATFORM=POSIX MACOSX
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=
RAW_NATIVE_PLATFORM_DIRS+=posix macosx
NATIVE_DISASM=otool
NATIVE_DISASM_FLAGS=-t -v -V
endif

ifeq ($(NATIVE_PLATFORM_MACOSX_PPC),1)
SUFFIXES_NATIVE_PLATFORM=POSIX MACOSX MACOSX_PPC
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=
RAW_NATIVE_PLATFORM_DIRS+=posix macosx macosx-ppc
NATIVE_DISASM=otool
NATIVE_DISASM_FLAGS=-t -v -V
endif

ifeq ($(NATIVE_PLATFORM_MACOSX_PPC64),1)
SUFFIXES_NATIVE_PLATFORM=POSIX MACOSX MACOSX_PPC MACOSX_PPC64
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=
RAW_NATIVE_PLATFORM_DIRS+=posix macosx macosx-ppc macosx-ppc64
NATIVE_DISASM=otool
NATIVE_DISASM_FLAGS=-t -v -V
endif

ifeq ($(NATIVE_PLATFORM_MACOSX_I386),1)
SUFFIXES_NATIVE_PLATFORM=POSIX MACOSX MACOSX_I386
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=
RAW_NATIVE_PLATFORM_DIRS+=posix macosx macosx-i386
NATIVE_DISASM=otool
NATIVE_DISASM_FLAGS=-t -v -V
endif


ifeq ($(NATIVE_PLATFORM_MACOSX_X86_64),1)
SUFFIXES_NATIVE_PLATFORM=POSIX MACOSX MACOSX_I386 MACOSX_X86_64
NATIVE_PLATFORM_GENERIC=0
NATIVE_EXE=
RAW_NATIVE_PLATFORM_DIRS+=posix macosx macosx-i386 macosx-x86_64
NATIVE_DISASM=otool
NATIVE_DISASM_FLAGS=-t -v -V
endif

# macosx_universal is a subset of macosx and posix and uses mac libtool to generate fat binaries.
NATIVE_MACOSX_SDK?=/Developer/SDKs/MacOSX10.4u.sdk
ifeq ($(TARGET_PLATFORM_MACOSX_UNIVERSAL),1)
NATIVE_MACOSX_UNIVERSAL_ARCHS?=i386 ppc
NATIVE_MACOSX_UNIVERSAL_ARCHS_PARAMS=$(foreach a,$(NATIVE_MACOSX_UNIVERSAL_ARCHS),-arch $(a))
SUFFIXES_NATIVE_PLATFORM=POSIX MACOSX MACOSX_UNIVERSAL
NATIVE_EXE=
RAW_NATIVE_PLATFORM_DIRS+=posix macosx macosx-ppc macosx-i386
NATIVE_COMPILE_FLAGS+=-isysroot $(TARGET_MACOSX_SDK) $(NATIVE_MACOSX_UNIVERSAL_ARCHS)
NATIVE_LINK_FLAGS+=-isysroot $(TARGET_MACOSX_SDK) $(NATIVE_MACOSX_UNIVERSAL_ARCHS)
NATIVE_USE_AR=0
NATIVE_USE_MACOSX_LIBTOOL=1
NATIVE_MACOSX_LIBTOOL=libtool
NATIVE_MACOSX_LIBTOOLFLAGS?=-static
NATIVE_DISASM=otool
NATIVE_DISASM_FLAGS=-t -v -V
endif


# cell_spu 
ifeq ($(NATIVE_PLATFORM_CELL_SPU),1)
SUFFIXES_NATIVE_PLATFORM=CELL_SPU
RAW_NATIVE_PLATFORM_DIRS+=cell_spu
endif

# linux_cell_ppu is a subset of linux-ppc, linux, posix
ifeq ($(NATIVE_PLATFORM_LINUX_CELL_PPU),1)
SUFFIXES_NATIVE_PLATFORM=POSIX LINUX LINUX_PPC LINUX_CELL_PPU CELL_PPU
RAW_NATIVE_PLATFORM_DIRS+=posix linux cell_ppu
endif

# default to objdump for disassembly
NATIVE_DISASM_FLAGS?=-d -S
NATIVE_DISASM?=$(OBJDUMP)

$(eval $(call calc_multi_native_options,$(SUFFIXES_NATIVE_PLATFORM)))


# calculate our output directories for our native platform results
NATIVE_PLATFORM_DIRS=$(sort $(RAW_NATIVE_PLATFORM_DIRS))

NATIVE_OUTPUT_DIR=$(NATIVE_BUILD_DIR)/native
NATIVE_OUTPUT_LIB_DIR?=$(NATIVE_OUTPUT_DIR)/lib
NATIVE_OUTPUT_TESTS_DIR?=$(NATIVE_OUTPUT_DIR)/tests
NATIVE_OUTPUT_DOCS_DIR?=$(NATIVE_OUTPUT_DIR)/docs
NATIVE_OUTPUT_TOOLS_DIR?=$(NATIVE_OUTPUT_DIR)/tools
NATIVE_OUTPUT_EXAMPLES_DIR?=$(NATIVE_OUTPUT_DIR)/examples
NATIVE_OUTPUT_OBJ_DIR?=$(NATIVE_OUTPUT_DIR)/obj


NATIVE_LIB_SRC_DIR+=$(call native_suffix_platform_dirs,src) $(foreach i,$(SUBLIBS),$(call native_suffix_platform_dirs,src/$(i)))
NATIVE_LIB_TESTS_DIR+=$(call native_suffix_platform_dirs,tests) $(foreach i,$(SUBLIBS),$(call native_suffix_platform_dirs,tests/$(i)))
NATIVE_LIB_EXAMPLES_DIR+=$(call native_suffix_platform_dirs,examples) $(foreach i,$(SUBLIBS),$(call native_suffix_platform_dirs,examples/$(i)))
NATIVE_LIB_TOOLS_DIR+=$(call native_suffix_platform_dirs,tools) $(foreach i,$(SUBLIBS),$(call native_suffix_platform_dirs,tools/$(i)))
NATIVE_ALL_SOURCES_DIRS=$(strip $(NATIVE_LIB_SRC_DIR) $(NATIVE_LIB_TESTS_DIR) $(NATIVE_LIB_EXAMPLES_DIR) $(NATIVE_LIB_TOOLS_DIR))

NATIVE_ALL_OUTPUT_DIRS+=$(NATIVE_OUTPUT_LIB_DIR) $(NATIVE_OUTPUT_TOOLS_DIR) $(NATIVE_OUTPUT_TESTS_DIR) $(NATIVE_OUTPUT_DOCS_DIR) $(NATIVE_OUTPUT_EXAMPLES_DIR) $(NATIVE_OUTPUT_OBJ_DIR) 


########################################################################
# magic-filelist.mk

# get the list of library source files from the src directory
LIB_CPP_FILES+=$(call get_src_file_list,cpp)
LIB_CC_FILES+=$(call get_src_file_list,cc)
LIB_C_FILES+=$(call get_src_file_list,c)
LIB_M_FILES+=$(call get_src_file_list,m)
LIB_MM_FILES+=$(call get_src_file_list,mm)
LIB_RC_FILES+=$(call get_src_file_list,rc)
LIB_O_FILES+=$(call get_cpp_o_files,$(LIB_CPP_FILES)) \
	$(call get_cc_o_files,$(LIB_CC_FILES))  \
	$(call get_c_o_files,$(LIB_C_FILES)) \
	$(call get_m_o_files,$(LIB_M_FILES)) \
	$(call get_mm_o_files,$(LIB_MM_FILES)) \
	$(call get_rc_o_files,$(LIB_RC_FILES)) 

LIB_SHLIB_O_FILES+=$(call get_cpp_shlib_o_files,$(LIB_CPP_FILES)) \
	$(call get_cc_shlig1b_o_files,$(LIB_CC_FILES))  \
	$(call get_c_shlib_o_files,$(LIB_C_FILES)) \
	$(call get_m_shlib_o_files,$(LIB_M_FILES)) \
	$(call get_mm_shlib_o_files,$(LIB_MM_FILES)) \
	$(call get_rc_shlib_o_files,$(LIB_RC_FILES)) 

LIB_DISASM_FILES+=$(LIB_O_FILES:.o=.disasm)
LIB_ASM_FILES+=$(LIB_O_FILES:.o=.asm)

ifeq ($(CROSS_COMPILING),1)
NATIVE_LIB_CPP_FILES+=$(call get_native_src_file_list,cpp)
NATIVE_LIB_CC_FILES+=$(call get_native_src_file_list,cc)
NATIVE_LIB_C_FILE+=$(call get_native_src_file_list,c)
NATIVE_LIB_M_FILES+=$(call get_native_src_file_list,m)
NATIVE_LIB_MM_FILES+=$(call get_native_src_file_list,mm)
NATIVE_LIB_RC_FILES+=$(call get_native_src_file_list,rc)

NATIVE_LIB_O_FILES+=$(call get_native_cpp_o_files,$(NATIVE_LIB_CPP_FILES)) \
	$(call get_native_cc_o_files,$(NATIVE_LIB_CC_FILES)) \
	$(call get_native_c_o_files,$(NATIVE_LIB_C_FILES)) \
	$(call get_native_m_o_files,$(NATIVE_LIB_M_FILES)) \
	$(call get_native_mm_o_files,$(NATIVE_LIB_MM_FILES)) \
	$(call get_native_rc_o_files,$(NATIVE_LIB_RC_FILES)) 
endif

LIB_INCLUDE_FILES+=$(wildcard $(LIB_INCLUDE_DIR)/*.h) 


# get the list of tool program source files from the tools directories

$(eval $(call search_program_group,TOOLS))

# get the list of tool-dev program source files from the tools-dev directories

$(eval $(call search_program_group,TOOLS_DEV))

# get the list of test program source files from the tests directories

$(eval $(call search_program_group,TESTS))

# get the list of example program source files from the examples directories

$(eval $(call search_program_group,EXAMPLES))

# if there are no O files to build, do not build or link a lib!

ifeq ($(strip $(LIB_O_FILES)),)
DO_NOT_BUILD_LIB=1
else
DO_NOT_BUILD_LIB=0
endif

ifeq ($(strip $(NATIVE_LIB_O_FILES)),)
NATIVE_DO_NOT_BUILD_LIB=1
else
NATIVE_DO_NOT_BUILD_LIB=0
endif


ifeq ($(DO_NOT_BUILD_LIB),1)
PROJECT_LDLIB=
else
PROJECT_LDLIB=-l$(PROJECT)
endif

ifeq ($(NATIVE_DO_NOT_BUILD_LIB),1)
NATIVE_PROJECT_LDLIB=
else
NATIVE_PROJECT_LDLIB=-l$(PROJECT)
endif

ifeq ($(DO_NOT_BUILD_LIB),1)
OUTPUT_LIB=
else
OUTPUT_LIB?=$(OUTPUT_LIB_DIR)/lib$(PROJECT).a
endif

ifeq ($(DO_NOT_BUILD_LIB),1)
NATIVE_OUTPUT_LIB=
else
NATIVE_OUTPUT_LIB?=$(NATIVE_OUTPUT_LIB_DIR)/lib$(PROJECT).a
endif


ifeq ($(ENABLE_LATEX),1)
########################################################################
# magic-latex.mk

LATEX_DOCS_TEX=$(call get_file_list,$(LIB_DOCS_DIR),tex)
LATEX_DOCS_TEX_TMP=$(addprefix $(LATEX_BUILD_DIR)/,$(call get_file_list,$(LIB_DOCS_DIR),tex))
LATEX_DOCS_PDF=$(addprefix $(OUTPUT_DOCS_DIR)/,$(LATEX_DOCS_TEX:.tex=.pdf))
LATEX_DOCS_HTML=$(addprefix $(OUTPUT_DOCS_DIR)/,$(LATEX_DOCS_TEX:.tex=.html))
LATEX_DOCS_CSS=$(addprefix $(OUTPUT_DOCS_DIR)/,$(LATEX_DOCS_TEX:.tex=.css))
DOCS=$(LATEX_DOCS_TEX_TMP) $(LATEX_DOCS_PDF) #$(LATEX_DOCS_HTML) $(LATEX_DOCS_CSS)


LATEX?=latex
MAKEINDEX?=makeindex
PDFLATEX?=pdflatex
DVIPS?=dvips
PS2PDF?=ps2pdf
HTLATEX?=htlatex

LATEX_OPTS+= --shell-escape
MAKEINDEX_OPTS=
DVIPS_OPTS+=-Ppdf
PS2PDF_OPTS+=
HTLATEX_OPTS+=
PDFLATEX_OPTS+= --shell-escape
LATEX_BUILD_DIR=$(BUILD_DIR)/latex
ALL_OUTPUT_DIRS+=$(LATEX_BUILD_DIR)

$(LATEX_BUILD_DIR)/%.tex : %.tex
	$(CP) $(<) $(@)

$(OUTPUT_DOCS_DIR)/%.dvi : $(LATEX_BUILD_DIR)/%.dvi
	$(CP) $(<) $(@)

$(OUTPUT_DOCS_DIR)/%.ps : $(LATEX_BUILD_DIR)/%.ps
	$(CP) $(<) $(@)

$(OUTPUT_DOCS_DIR)/%.pdf : $(LATEX_BUILD_DIR)/%.pdf
	$(CP) $(<) $(@)

$(OUTPUT_DOCS_DIR)/%.html : $(LATEX_BUILD_DIR)/%.html
	$(CP) $(<) $(@)

$(OUTPUT_DOCS_DIR)/%.css : $(LATEX_BUILD_DIR)/%.css
	$(CP) $(<) $(@)

$(LATEX_BUILD_DIR)/%.dvi : $(LATEX_BUILD_DIR)/%.tex
	(cd $(LATEX_BUILD_DIR) && \
	$(LATEX) $(LATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(LATEX) $(LATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(MAKEINDEX) $(MAKEINDEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(LATEX) $(LATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(LATEX) $(LATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' \
	)

$(LATEX_BUILD_DIR)/%.pdf : $(LATEX_BUILD_DIR)/%.tex
	(cd $(LATEX_BUILD_DIR) && \
	$(PDFLATEX) $(PDFLATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(PDFLATEX) $(PDFLATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(MAKEINDEX) $* && \
	$(PDFLATEX) $(PDFLATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(PDFLATEX) $(PDFLATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' \
	)

$(LATEX_BUILD_DIR)/%.ps : $(LATEX_BUILD_DIR)/%.dvi
	(cd $(LATEX_BUILD_DIR) &&  \
	$(DVIPS) $(DVIPS_OPTS) $* \
	)

$(LATEX_BUILD_DIR)/%.html $(LATEX_BUILD_DIR)/%.css : $(LATEX_BUILD_DIR)/%.tex
	(cd $(LATEX_BUILD_DIR) &&   \
	$(HTLATEX) $(HTLATEX_OPTS) $* \
	)	




endif

.PHONY : shlib

shlib :

########################################################################
# magic-targets.mk

.PHONY : all

ifdef ALL
all : dirs $(ALL)
else
ifeq ($(CROSS_COMPILING)$(BUILD_NATIVE),11)
ALL_TARGETS += native-dirs dirs native-lib native-tools native-tools-dev native-examples native-tests dirs lib tools tools-dev tests examples local-config-tool 
else
ALL_TARGETS += dirs lib tools tools-dev tests examples local-config-tool 
endif

all : dirs $(ALL_TARGETS)

endif

.PHONY : dirs

dirs :
	-@$(MKDIR) -p $(ALL_OUTPUT_DIRS)

.PHONY : prelib

prelib : dirs

.PHONY : sharedlib

sharedlib : dirs shlib

.PHONY : lib

ifeq ($(DO_NOT_BUILD_LIB),0)

lib : dirs prelib staticlib shlib

.PHONY : staticlib

ifeq ($(ENABLE_STATICLIB),1)
staticlib : dirs $(OUTPUT_LIB)
else
staticlib : dirs
endif

$(OUTPUT_LIB) : $(LIB_O_FILES) 
ifeq ($(TARGET_USE_AR),1)
	@echo "AR     : $(notdir $@)($(notdir $?))"
	@$(AR) $(ARFLAGS) $@ $? >/dev/null
	@$(RANLIB) $@
endif
ifeq ($(TARGET_USE_MACOSX_LIBTOOL),1)
	@echo "LIBTOOL: $(notdir $@)($(notdir $?))"
#	@echo $(MACOSX_LIBTOOL) $(MACOSX_LIBTOOLFLAGS) -o $@ $^
	@$(MACOSX_LIBTOOL) $(MACOSX_LIBTOOLFLAGS) -o $@ $^
endif
else
lib : dirs
endif

$(LIB_O_FILES) : 

.PHONY : tools

tools : lib $(LIB_TOOLS_EXE_FILES)

$(LIB_TOOLS_EXE_FILES) : $(OUTPUT_LIB) $(CONFIG_TOOLS_PATHS)

#$(LDLIBS_NO_OPTS)

.PHONY : tools-dev

tools-dev : lib $(LIB_TOOLS_DEV_EXE_FILES) config-tool

$(LIB_TOOLS_DEV_EXE_FILES) : $(OUTPUT_LIB) $(CONFIG_TOOLS_PATHS) 
#$(LDLIBS_NO_OPTS)


.PHONY : examples

examples: lib $(LIB_EXAMPLES_EXE_FILES)

$(LIB_EXAMPLES_EXE_FILES) : $(OUTPUT_LIB) $(CONFIG_TOOLS_PATHS) $(LDLIBS_NO_OPTS)

.PHONY : tests

tests: lib $(LIB_TESTS_EXE_FILES) 

$(LIB_TESTS_EXE_FILES) : $(OUTPUT_LIB) $(CONFIG_TOOLS_PATHS) $(LDLIBS_NO_OPTS)

.PHONY : docs

docs : dirs $(DOCS)
	@echo "DOCS : in $(LIB_DOCS_DIR)"

.PHONY : docs-dev

docs-dev : dirs $(wildcard $(LIB_DOCS_DEV_DIR)/Doxyfile)
	@echo "DOCS-DEV :"
	@( \
  if [ -f $(LIB_DOCS_DEV_DIR)/Doxyfile ]; then \
	export TOP="$(PROJECT_TOP_DIR)"; \
	export PROJECT="$(PROJECT)"; \
	export PROJECT_VERSION="$(PROJECT_VERSION)"; \
	export ALL_SRC_DIRS="$(ALL_SRC_DIRS)"; \
	export LIB_EXAMPLES_DIRS="$(LIB_EXAMPLES_DIR)"; \
	cd "$(OUTPUT_DOCS_DEV_DIR)" && \
	for i in $(LIB_DOCS_DEV_DIR); do \
	  $(DOXYGEN) $$i/Doxyfile; done \
  fi \
	)

.PHONY : clean

clean :
	-@$(RM) -r -f $(LIB_TESTS_O_FILES) $(LIB_EXAMPLES_O_FILES) $(LIB_TOOLS_O_FILES) $(LIB_O_FILES) $(OUTPUT_OBJ_DIR)/*.d 2>/dev/null
	-@$(RM) -r -f $(LIB_SHLIB_O_FILES) $(OUTPUT_SHOBJ_DIR)/*.d $(OUTPUT_SHLIB) 2>/dev/null
	-@$(RM) -r -f $(LIB_TESTS_EXE_FILES) $(LIB_EXAMPLES_EXE_FILES) $(LIB_TOOLS_EXE_FILES) $(LIB_TOOLS_DEV_EXE_FILES) 2>/dev/null
	-@$(RM) -r -f $(OUTPUT_LIB) $(CLEAN_DIRS) $(CLEAN_FILES) $(PACKAGES) 2>/dev/null
ifeq ($(CROSS_COMPILING),1)
	-@$(RM) -r -f $(NATIVE_LIB_TESTS_O_FILES) $(NATIVE_LIB_EXAMPLES_O_FILES) $(NATIVE_LIB_TOOLS_O_FILES) $(NATIVE_LIB_O_FILES) $(NATIVE_OUTPUT_OBJ_DIR)/*.d 2>/dev/null
	-@$(RM) -r -f $(NATIVE_LIB_TESTS_EXE_FILES) $(NATIVE_LIB_EXAMPLES_EXE_FILES) $(NATIVE_LIB_TOOLS_EXE_FILES) 2>/dev/null
	-@$(RM) -r -f $(NATIVE_LIB_TOOLS_DEV_EXE_FILES) 2>/dev/null
	-@$(RM) -r -f $(NATIVE_OUTPUT_LIB) $(NATIVE_CLEAN_DIRS) $(NATIVE_CLEAN_FILES) 2>/dev/null
endif

.PHONY : realclean

realclean : distclean

.PHONY : distclean

distclean : clean
	-@$(RM) $(OUTPUT_LIB) 2>/dev/null
ifeq ($(CROSS_COMPILING),1)
	-@$(RM) $(NATIVE_OUTPUT_LIB) 2>/dev/null
endif

TAGS : $(LIB_CPP_FILES) $(LIB_CC_FILES) $(LIB_C_FILES) $(LIB_H_FILES)
	@echo "TAGS:"
	@etags --language c++ $(shell find $(LIB_INCLUDE_DIR) $(LIB_SRC_DIR) $(LIB_TESTS_DIR) $(LIB_EXAMPLES_DIR) $(LIB_TOOLS_DIR) \( -name "*.cpp" -or -name "*.cc" -or -name "*.h" \) )

tags : $(LIB_CPP_FILES) $(LIB_CC_FILES) $(LIB_C_FILES) $(LIB_H_FILES)
	@echo "tags:"
	@ctags --language c++ $(shell find $(LIB_INCLUDE_DIR) $(LIB_SRC_DIR) $(LIB_TESTS_DIR) $(LIB_EXAMPLES_DIR) $(LIB_TOOLS_DIR) \( -name "*.cpp" -or -name "*.cc" -or -name "*.h" \) )

ifeq ($(CROSS_COMPILING),1)
.PHONY : native-dirs

native-dirs :
	-@$(MKDIR) -p $(NATIVE_ALL_OUTPUT_DIRS)

.PHONY : native-lib

ifeq ($(NATIVE_DO_NOT_BUILD_LIB),0)
native-lib : native-dirs $(NATIVE_OUTPUT_LIB)


$(NATIVE_OUTPUT_LIB) : $(NATIVE_LIB_O_FILES) 
ifeq ($(NATIVE_USE_AR),1)
	@echo "NATIVE_AR     : $(notdir $@)($(notdir $?))"
	@$(NATIVE_AR) $(NATIVE_ARFLAGS) $@ $? >/dev/null
	@$(NATIVE_RANLIB) $@
endif
ifeq ($(NATIVE_USE_MACOSX_LIBTOOL),1)
	@echo "NATIVE_LIBTOOL: $(notdir $@)($(notdir $?))"
#	@echo $(NATIVE_MACOSX_LIBTOOL) $(NATIVE_MACOSX_LIBTOOLFLAGS) -o $@ $^
	@$(NATIVE_MACOSX_LIBTOOL) $(NATIVE_MACOSX_LIBTOOLFLAGS) -o $@ $^
endif
endif

.PHONY : native-tools

native-tools : native-lib $(NATIVE_LIB_TOOLS_EXE_FILES)

$(NATIVE_LIB_TOOLS_EXE_FILES) : $(NATIVE_OUTPUT_LIB)

.PHONY : native-tools-dev

native-tools-dev : native-lib $(NATIVE_LIB_TOOLS_DEV_EXE_FILES)

$(NATIVE_LIB_TOOLS_DEV_EXE_FILES) : $(NATIVE_OUTPUT_LIB)


.PHONY : native-examples

native-examples: native-lib $(NATIVE_LIB_EXAMPLES_EXE_FILES)

$(NATIVE_LIB_EXAMPLES_EXE_FILES) : $(NATIVE_OUTPUT_LIB)

.PHONY : native-tests

native-tests: native-lib $(NATIVE_LIB_TESTS_EXE_FILES)

$(NATIVE_LIB_TESTS_EXE_FILES) : $(NATIVE_OUTPUT_LIB)

.PHONY : native-test

native-test: native-lib native-tools $(LIB_TESTS_SH_FILES) $(NATIVE_LIB_TESTS_EXE_FILES)
	@cd "$(NATIVE_OUTPUT_TESTS_DIR)"; \
	for i in $(NATIVE_LIB_TESTS_EXE_FILES); \
	do \
	  n=$$(basename $$i); \
	  if $(VALGRIND) $(VALGRIND_OPTIONS) "./$${n}" $(TEST_ARGS) >"$${n}.out" 2>"$${n}.err"; then echo SUCCESS:$${n}; else echo  FAIL:$${n}; fi;\
	done; \
	for i in $(LIB_TESTS_SH_FILES); \
	do \
	  n=$$(basename $$i); \
	  if bash "$${i}" >"$${n}.out" 2>"$${n}.err"; then echo SUCCESS:$${n}; else echo  FAIL:$${n}; fi;\
	done

endif

.PHONY : disasm

disasm :  $(LIB_DISASM_FILES) $(LIB_EXAMPLES_DISASM_FILES) $(LIB_TOOLS_DISASM_FILES) $(LIB_TOOLS_DISASM_FILES) $(LIB_TESTS_DISASM_FILES)

.PHONY : asm

asm :  $(LIB_ASM_FILES) 


########################################################################
# magic-custom.mk

# read any custom mk files in the top dir of each library dir / project

CUSTOM_MAK_FILES := $(foreach dir,$(LIB_DIRS),$(dir)/$(dir).mk)

ifneq ($(CUSTOM_MAK_FILES),)
-include $(CUSTOM_MAK_FILES)
endif

########################################################################
# magic-install.mk

#INSTALL_MODE?=install-main install-docs
INSTALL_MODE?=install-main install-docs install-dev

.PHONY : install-all

install-all : install-main install-docs install-dev install-docs-dev

.PHONY : install

install : $(INSTALL_MODE)

.PHONY : preinstall-main

CLEAN_DIRS += $(LOCAL_INSTALL_DIR) $(LOCAL_INSTALLDEV_DIR) $(LOCAL_INSTALLDOCSDEV_DIR)

preinstall-main : all lib tools tests examples docs 
	@echo preinstall-main:
	@$(call copy_dirs,$(OUTPUT_TOOLS_DIR),$(LOCAL_INSTALL_BIN_DIR)) 
	@$(call copy_dirs,$(LIB_SHARE_DIR),$(LOCAL_INSTALL_SHARE_DIR))
	@$(call copy_dirs,$(LIB_ETC_DIR),$(LOCAL_INSTALL_ETC_DIR))


.PHONY : install-main

install-main : preinstall-main install-main-dirs
	@echo install-main:
	@$(call copy_dirs,$(LOCAL_INSTALL_BIN_DIR),$(INSTALL_BIN_DIR))
	@$(call copy_dirs,$(LOCAL_INSTALL_SHARE_DIR),$(INSTALL_SHARE_DIR))
	@$(call copy_dirs,$(LOCAL_INSTALL_ETC_DIR),$(INSTALL_ETC_DIR))
	@$(call copy_dirs,$(LOCAL_INSTALL_LIB_DIR),$(INSTALL_LIB_DIR))

.PHONY : install-main-dirs

install-main-dirs :
	-@$(MKDIR) -p $(INSTALL_BIN_DIR) $(INSTALL_SHARE_DIR) $(INSTALL_ETC_DIR) $(INSTALL_LIB_DIR)

.PHONY : preinstall-dev

preinstall-dev : preinstall-main tools-dev
	@echo preinstall-dev:
	@$(call copy_dirs,$(LIB_INCLUDE_DIR),$(LOCAL_INSTALLDEV_INCLUDE_DIR))
	@$(MKDIRS) $(LOCAL_INSTALLDEV_BIN_DIR)
	@$(INSTALL) $(CONFIG_TOOL_FILE) $(LOCAL_INSTALLDEV_BIN_DIR)
	@$(call copy_dirs,$(LIB_SHARE_DEV_DIR),$(LOCAL_INSTALLDEV_SHARE_DIR))
	@$(call copy_dirs,$(OUTPUT_LIB),$(LOCAL_INSTALLDEV_LIB_DIR))
	@$(call copy_dirs,$(OUTPUT_EXAMPLES_DIR) $(OUTPUT_TESTS_DIR) $(OUTPUT_TOOLS_DEV_DIR),$(LOCAL_INSTALLDEV_BIN_DIR))

.PHONY : install-dev

install-dev : preinstall-dev install-dev-dirs config-tool
	@$(call copy_dirs,$(LOCAL_INSTALLDEV_LIB_DIR),$(INSTALLDEV_LIB_DIR))
	@$(call copy_dirs,$(LOCAL_INSTALLDEV_BIN_DIR),$(INSTALLDEV_BIN_DIR))
	@$(call copy_dirs,$(LOCAL_INSTALLDEV_SHARE_DIR),$(INSTALLDEV_SHARE_DIR))
	@$(call copy_dirs,$(LOCAL_INSTALLDEV_INCLUDE_DIR),$(INSTALLDEV_INCLUDE_DIR))

.PHONY : install-dev-dirs

install-dev-dirs : preinstall-dev 
	@echo install-dev:

.PHONY : preinstall-docs-dev

preinstall-docs-dev : docs-dev 
	@echo preinstall-docs-dev:
	@$(call copy_dirs,$(OUTPUT_DOCS_DEV_DIR),$(LOCAL_INSTALLDOCSDEV_DOCS_DIR)/$(DOCS_DEV_SUBDIR))

.PHONY : install-docs-dev

install-docs-dev : preinstall-docs-dev 
	@echo install-docs-dev:
	@$(call copy_dirs,$(LOCAL_INSTALLDOCSDEV_DIR)/$(TARGET_INSTALLDOCSDEV_DOCS_DIR),$(TARGET_INSTALLDOCSDEV_DOCS_DIR))

.PHONY : preinstall-docs

preinstall-docs : docs
	@echo preinstall-docs:
	@$(call copy_dirs,$(LIB_DOCS_DIR),$(LOCAL_INSTALL_DOCS_DIR))

.PHONY : install-docs

install-docs : preinstall-docs 
	@echo install-docs:
	@$(call copy_dirs,$(LOCAL_INSTALL_DOCS_DIR),$(INSTALL_DOCS_DIR))


#####################################################################################
# magic-package.mk


ALL_OUTPUT_DIRS+=$(PACKAGES_DIR)

PACKAGE_PREINSTALL=$(PROJECT_TOP_DIR)/scripts/preinstall.sh
PACKAGE_POSTINSTALL=$(PROJECT_TOP_DIR)/scripts/postinstall.sh
PACKAGE_PREREMOVE=$(PROJECT_TOP_DIR)/scripts/preremove.sh
PACKAGE_POSTREMOVE=$(PROJECT_TOP_DIR)/scripts/postremove.sh

NORMAL_PACKAGE_NAMES?=0

ifeq ($(PACKAGER),zip)
NORMAL_PACKAGE_NAMES=1
PACKAGE_EXT=zip
endif
ifeq ($(PACKAGER),tgz)
NORMAL_PACKAGE_NAMES=1
PACKAGE_EXT=tgz
endif
ifeq ($(PACKAGER),dpkg)
NORMAL_PACKAGE_NAMES=0
PACKAGE_EXT=deb
endif
ifeq ($(PACKAGER),checkinstall-deb)
NORMAL_PACKAGE_NAMES=0
PACKAGE_EXT=deb
endif
ifeq ($(PACKAGER),checkinstall-rpm)
NORMAL_PACKAGE_NAMES=0
PACKAGE_EXT=rpm
endif
ifeq ($(PACKAGER),packagemaker-pkg)
NORMAL_PACKAGE_NAMES=0
PACKAGE_EXT=pkg
endif



PACKAGETESTRESULTS_BASENAME?=$(PROJECT)-testresults-$(PROJECT_VERSION)-$(TARGET_PLATFORM_NAME)$(PACKAGE_SUFFIX)

ifeq ($(NORMAL_PACKAGE_NAMES),1)
PACKAGE_BASENAME?=$(PROJECT)$(PROJECT_VARIANT_SUFFIX)-$(PROJECT_VERSION)-$(TARGET_PLATFORM_NAME)$(PACKAGE_SUFFIX)
PACKAGEDEV_BASENAME?=$(PROJECT)$(PROJECT_VARIANT_SUFFIX)-dev-$(PROJECT_VERSION)-$(TARGET_PLATFORM_NAME)$(PACKAGE_SUFFIX)
PACKAGEDOCSDEV_BASENAME?=$(PROJECT)$(PROJECT_VARIANT_SUFFIX)-docs-dev-$(PROJECT_VERSION)-$(TARGET_PLATFORM_NAME)$(PACKAGE_SUFFIX)
PACKAGE_FILE=$(PACKAGE_BASENAME).$(PACKAGE_EXT)
PACKAGE_DEV_FILE=$(PACKAGEDEV_BASENAME).$(PACKAGE_EXT)
PACKAGE_DOCSDEV_FILE=$(PACKAGEDOCSDEV_BASENAME).$(PACKAGE_EXT)
endif


ifeq ($(TARGET_PLATFORM_LINUX_PPC),1)
PACKAGE_ARCH=powerpc
endif

ifeq ($(TARGET_PLATFORM_LINUX_PPC64),1)
PACKAGE_ARCH=powerpc64
endif

ifeq ($(TARGET_PLATFORM_LINUX),1)
PACKAGE_ARCH=i386
endif

ifeq ($(TARGET_PLATFORM_LINUX_I386),1)
PACKAGE_ARCH=i386
endif

ifeq ($(TARGET_PLATFORM_LINUX_X86_64),1)
PACKAGE_ARCH=amd64
endif

ifeq ($(PACKAGE_EXT),deb)
PACKAGE_RELEASE?=$(PROJECT_RELEASE)
PACKAGE_BASENAME?=$(PROJECT)$(PROJECT_VARIANT_SUFFIX)$(PACKAGE_SUFFIX)
PACKAGEDEV_BASENAME?=$(PROJECT)$(PROJECT_VARIANT_SUFFIX)-dev$(PACKAGE_SUFFIX)
PACKAGEDOCSDEV_BASENAME?=$(PROJECT)$(PROJECT_VARIANT_SUFFIX)-docs-dev$(PACKAGE_SUFFIX)

PACKAGE_FILE=$(PACKAGE_BASENAME)_$(PROJECT_VERSION)-$(PACKAGE_RELEASE)_$(PACKAGE_ARCH).$(PACKAGE_EXT)
PACKAGE_DEV_FILE=$(PACKAGEDEV_BASENAME)_$(PROJECT_VERSION)-$(PACKAGE_RELEASE)_$(PACKAGE_ARCH).$(PACKAGE_EXT)
PACKAGE_DOCSDEV_FILE=$(PACKAGEDOCSDEV_BASENAME)_$(PROJECT_VERSION)-$(PACKAGE_RELEASE)_$(PACKAGE_ARCH).$(PACKAGE_EXT)
endif


ifneq ($(PACKAGE_PROVIDES),)
PACKAGE_PROVIDES_FLAGS=--provides $(PACKAGE_PROVIDES)
endif

ifneq ($(PACKAGE_REQUIRES),)
PACKAGE_REQUIRES_FLAGS=--requires $(PACKAGE_REQUIRES)
endif

ifneq ($(PACKAGEDEV__REQUIRES),)
PACKAGEDEV_REQUIRES_FLAGS=--requires $(PACKAGEDEV_REQUIRES)
endif


PACKAGE_PATH=$(PACKAGES_DIR)/$(PACKAGE_FILE)
PACKAGE_DEV_PATH=$(PACKAGES_DIR)/$(PACKAGE_DEV_FILE)
PACKAGE_DOCSDEV_PATH=$(PACKAGES_DIR)/$(PACKAGE_DOCSDEV_FILE)

package : preinstall-main

package-dev : preinstall-dev

package-docs-dev : preinstall-docs-dev

package : $(PACKAGE_PATH)

package-dev : $(PACKAGE_DEV_PATH)

package-docs-dev : $(PACKAGE_DOCSDEV_PATH)

ifeq ($(PACKAGER),tgz)

$(PACKAGE_PATH) :
	@echo "package-tgz:"
	@( cd $(LOCAL_INSTALL_DIR) && tar cvf - . | gzip >$(PACKAGE_PATH) )
	@echo "PACKAGE=$(PACKAGE_PATH)"

$(PACKAGE_DEV_PATH) :
	@echo "package-dev-tgz:"
	@( cd $(LOCAL_INSTALLDEV_DIR) && tar cvf - . | gzip >$(PACKAGE_DEV_PATH) )
	@echo "PACKAGE_DEV=$(PACKAGE_DEV_PATH)"

$(PACKAGE_DOCSDEV_PATH) :
	@echo "package-docs-dev-tgz:"
	@( cd $(LOCAL_INSTALLDOCSDEV_DIR) && tar cvf - . | gzip >$(PACKAGE_DOCSDEV_PATH) )
	@echo "PACKAGE_DOCSDEV=$(PACKAGE_DEV_PATH)"

endif

ifeq ($(PACKAGER),zip)

$(PACKAGE_PATH) :
	@echo "package-zip:"
	@( cd $(LOCAL_INSTALL_DIR) && $(ZIP) $(ZIP_FLAGS) $(PACKAGE_PATH) )
	@echo "PACKAGE=$(PACKAGE_PATH)"

$(PACKAGE_DEV_PATH) :
	@echo "package-dev-zip:"
	@( cd $(LOCAL_INSTALLDEV_DIR) && $(ZIP) $(ZIP_FLAGS) $(PACKAGE_DEV_PATH) )
	@echo "PACKAGE_DEV=$(PACKAGE_DEV_PATH)"

$(PACKAGE_DOCSDEV_PATH) :
	@echo "package-docs-dev-zip:"
	@( cd $(LOCAL_INSTALLDOCSDEV_DIR) && $(ZIP) $(ZIP_FLAGS) $(PACKAGE_DOCSDEV_PATH) )
	@echo "PACKAGE_DOCSDEV=$(PACKAGE_DEV_PATH)"

endif

ifeq ($(PACKAGER),checkinstall-deb)

$(PACKAGE_PATH) : 
	@echo "checkinstall-package-deb: "
	-@rm description-pak postinstall-pak preinstall-pak postremove-pak preremove-pak 2>/dev/null
	@echo "$(PROJECT_DESCRIPTION)" >description-pak
	@if [ -f "$(PACKAGE_PREINSTALL)"]; then cp -p "$(PACKAGE_PREINSTALL)" preinstall-pak; fi
	@if [ -f "$(PACKAGE_POSTINSTALL)"]; then cp -p "$(PACKAGE_POSTINSTALL)" postinstall-pak; fi
	@if [ -f "$(PACKAGE_PREREMOVE)"]; then cp -p "$(PACKAGE_PREREMOVE)" preremove-pak; fi
	@if [ -f "$(PACKAGE_POSTREMOVE)"]; then cp -p "$(PACKAGE_POSTREMOVE)" postremove-pak; fi
	$(SUDO_CHECKINSTALL) --type debian -D $(CHECKINSTALL_OPTIONS) --pkgname $(PACKAGE_BASENAME) --pkgversion $(PROJECT_VERSION) --pkgrelease $(PROJECT_RELEASE) --pkglicense $(PROJECT_LICENSE) --pakdir $(PACKAGES_DIR) --maintainer $(PROJECT_MAINTAINER) --install=no --reset-uids $(PACKAGE_PROVIDES_FLAGS) $(PACKAGE_REQUIRES_FLAGS) $(MAKE) install-main
	-@rm description-pak postinstall-pak preinstall-pak postremove-pak preremove-pak 2>/dev/null
	@echo "PACKAGE=$(PACKAGE_PATH)"


$(PACKAGE_DEV_PATH) : 
	@echo "checkinstall-package-dev-deb:"
	@echo "$(PROJECT_DESCRIPTION)" >description-pak
	@echo "" >>description-pak
	@echo "This package contains the development tools for the project." >>description-pak
	$(SUDO_CHECKINSTALL) --type debian -D $(CHECKINSTALL_OPTIONS) --pkgname $(PACKAGEDEV_BASENAME) --pkgversion $(PROJECT_VERSION) --pkgrelease $(PROJECT_RELEASE) --pkglicense $(PROJECT_LICENSE) --pakdir $(PACKAGES_DIR) --maintainer $(PROJECT_MAINTAINER) --install=no --reset-uids $(PACKAGEDEV_REQUIRES_FLAGS) $(MAKE) install-dev
	-@rm description-pak 2>/dev/null
	@echo "PACKAGE_DEV=$(PACKAGE_DEV_PATH)"

$(PACKAGE_DOCSDEV_PATH) : 
	@echo "checkinstall-package-docs-dev-deb:"
	@echo "$(PROJECT_DESCRIPTION)" >description-pak
	@echo "" >>description-pak
	@echo "This package contains the development documentation for the project." >>description-pak
	$(SUDO_CHECKINSTALL) --type debian -D $(CHECKINSTALL_OPTIONS) --pkgname $(PACKAGEDOCSDEV_BASENAME) --pkgversion $(PROJECT_VERSION) --pkgrelease $(PROJECT_RELEASE) --pkglicense $(PROJECT_LICENSE) --pakdir $(PACKAGES_DIR) --maintainer $(PROJECT_MAINTAINER) --install=no --reset-uids $(MAKE) install-docs-dev
	-@rm description-pak 2>/dev/null
	@echo "PACKAGE_DOCSDEV=$(PACKAGE_DEV_PATH)"

endif


ifeq ($(PACKAGER),dpkg)

PACKAGE_SECTION?=non-free/utils
PACKAGE_DEV_SECTION?=non-free/libdevel

DEBIAN_DIR=$(LOCAL_INSTALL_DIR)/DEBIAN

$(PACKAGE_PATH) : preinstall-main
	@echo "dpkg: "
	@rm -r -f $(DEBIAN_DIR)
	@$(MKDIRS) $(DEBIAN_DIR)
	@$(call copy_file,$(PACKAGE_PREINSTALL),$(DEBIAN_DIR)/preinst)
	@$(call copy_file,$(PACKAGE_POSTINSTALL),$(DEBIAN_DIR)/postinst)
	@$(call copy_file,copy_file $(PACKAGE_PREREMOVE),$(DEBIAN_DIR)/preremove)
	@$(call copy_file $(PACKAGE_POSTREMOVE),$(DEBIAN_DIR)/postremove)
	@echo "Package: $(PROJECT_NAME)$(PROJECT_VARIANT_SUFFIX)" >$(DEBIAN_DIR)/control
	@echo "Version: $(PROJECT_VERSION)-$(PROJECT_RELEASE)" >>$(DEBIAN_DIR)/control
	@echo "Section: $(PACKAGE_SECTION)" >>$(DEBIAN_DIR)/control
	@echo "Priority: optional"  >>$(DEBIAN_DIR)/control
	@echo "Architecture: $(PACKAGE_ARCH)" >>$(DEBIAN_DIR)/control
	@echo "Provides: $(PACKAGE_PROVIDES)" >>$(DEBIAN_DIR)/control
	@echo "Pre-Depends: $(PACKAGE_PREREQUIRES)" >>$(DEBIAN_DIR)/control
	@echo "Depends: $(PACKAGE_REQUIRES)" >>$(DEBIAN_DIR)/control
	@echo "Suggests: $(PACKAGE_SUGGESTS)" >>$(DEBIAN_DIR)/control
	@echo "Conflicts: $(PACKAGE_CONFLICTS)" >>$(DEBIAN_DIR)/control
	@echo "Replaces: $(PACKAGE_REPLACES)" >>$(DEBIAN_DIR)/control
	@echo "Maintainer: $(PROJECT_MAINTAINER)" >>$(DEBIAN_DIR)/control
	@echo "Description: $(PROJECT_DESCRIPTION)" >>$(DEBIAN_DIR)/control
	@$(RUN_DPKG) -b $(LOCAL_INSTALL_DIR) $(PACKAGE_PATH)
	@rm -r -f $(DEBIAN_DIR)
	@echo "PACKAGE=$(PACKAGE_PATH)"


DEBIAN_DEV_DIR=$(LOCAL_INSTALLDEV_DIR)/DEBIAN

$(PACKAGE_DEV_PATH) : preinstall-dev
	@echo "dpkg-dev: "
	@rm -r -f $(DEBIAN_DEV_DIR)
	@$(MKDIRS) $(DEBIAN_DEV_DIR)
	@echo "Package: $(PROJECT_NAME)$(PROJECT_VARIANT_SUFFIX)-dev" >$(DEBIAN_DEV_DIR)/control
	@echo "Version: $(PROJECT_VERSION)-$(PROJECT_RELEASE)" >>$(DEBIAN_DEV_DIR)/control
	@echo "Section: $(PACKAGE_DEV_SECTION)" >>$(DEBIAN_DEV_DIR)/control
	@echo "Priority: optional"  >>$(DEBIAN_DEV_DIR)/control
	@echo "Architecture: $(PACKAGE_ARCH)" >>$(DEBIAN_DEV_DIR)/control
	@echo "Provides: $(PACKAGE_DEV_PROVIDES)" >>$(DEBIAN_DEV_DIR)/control
	@echo "Pre-Depends: $(PACKAGE_DEV_PREREQUIRES)" >>$(DEBIAN_DEV_DIR)/control
	@echo "Depends: $(PACKAGE_DEV_REQUIRES)" >>$(DEBIAN_DEV_DIR)/control
	@echo "Suggests: $(PACKAGE_DEV_SUGGESTS)" >>$(DEBIAN_DEV_DIR)/control
	@echo "Conflicts: $(PACKAGE_DEV_CONFLICTS)" >>$(DEBIAN_DEV_DIR)/control
	@echo "Replaces: $(PACKAGE_DEV_REPLACES)" >>$(DEBIAN_DEV_DIR)/control
	@echo "Maintainer: $(PROJECT_MAINTAINER)" >>$(DEBIAN_DEV_DIR)/control
	@echo "Description: Development files - $(PROJECT_DESCRIPTION)" >>$(DEBIAN_DEV_DIR)/control
	@$(RUN_DPKG) -b $(LOCAL_INSTALLDEV_DIR) $(PACKAGE_DEV_PATH)
	@rm -r -f $(DEBIAN_DEV_DIR)
	@echo "PACKAGE_DEV=$(PACKAGE_DEV_PATH)"


DEBIAN_DOCS_DEV_DIR=$(LOCAL_INSTALLDOCSDEV_DIR)/DEBIAN

$(PACKAGE_DOCSDEV_PATH) : preinstall-docs-dev
	@echo "dpkg-docs-dev:"
	@rm -r -f $(DEBIAN_DOCS_DEV_DIR)
	@$(MKDIRS) $(DEBIAN_DOCS_DEV_DIR)
	@echo "Package: $(PROJECT_NAME)$(PROJECT_VARIANT_SUFFIX)-docsdev" >$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Version: $(PROJECT_VERSION)-$(PROJECT_RELEASE)" >>$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Section: $(PACKAGE_DEV_SECTION)" >>$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Priority: optional"  >>$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Architecture: $(PACKAGE_ARCH)" >>$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Essential: no" >>$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Provides: $(PACKAGE_PROVIDES)-docsdev" >>$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Pre-Depends: $(PACKAGE_DOCSDEV_PREREQUIRES)" >>$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Depends: $(PACKAGE_DOCSDEV_REQUIRES)" >>$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Suggests: $(PACKAGE_DOCSDEV_SUGGESTS)" >>$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Conflicts: $(PACKAGE_DOCSDEV_CONFLICTS)" >>$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Replaces: $(PACKAGE_DOCSDEV_REPLACES)" >>$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Maintainer: $(PROJECT_MAINTAINER)" >>$(DEBIAN_DOCS_DEV_DIR)/control
	@echo "Description: Development Files - $(PROJECT_DESCRIPTION)" >>$(DEBIAN_DOCS_DEV_DIR)/control
	@$(RUN_DPKG) -b $(LOCAL_INSTALLDOCSDEV_DIR) $(PACKAGE_DOCSDEV_PATH)
	@rm -r -f $(DEBIAN_DOCS_DEV_DIR)
	@echo "PACKAGE_DOCSDEV=$(PACKAGE_DEV_PATH)"

endif


CLEAN_FILES += $(PACKAGE_PATH) $(PACKAGE_DEV_PATH) $(PACKAGE_DOCSDEV_PATH) $(PACKAGE_TESTRESULTS_PATH)

.PHONY : packages

packages : package package-dev package-docs-dev package-testresults

.PHONY : package-testresults

ifeq ($(CROSS_COMPILING),1)

package-testresults :

else

package-testresults : $(PACKAGE_TESTRESULTS_PATH)

$(PACKAGE_TESTRESULTS_PATH) : test 
	@-( cd $(OUTPUT_TESTS_DIR) && $(ZIP) -r $(PACKAGE_TESTRESULTS_PATH) . >/dev/null )

$(PACKAGES_DIR)/$(PACKAGETESTRESULTS_BASENAME).tgz : test 
	@-( cd $(OUTPUT_TESTS_DIR) && $(TAR) cf - . | $(GZIP) >$(PACKAGES_DIR)/$(PACKAGETESTRESULTS_BASENAME).zip )

endif


########################################################################
# magic-ship.mk

.PHONY : ship-all

ship-all : ship ship-dev ship-docs-dev ship-testresults

.PHONY : ship

ship : package
	@echo SHIPPING: package to $(SHIP_TO)
	@$(SSH) $(SHIP_TO_ACCOUNT) mkdir -p $(SHIP_TO_DIR) && $(SCP) $(SCP_OPTIONS) $(PACKAGE_PATH) $(SHIP_TO)

.PHONY : ship-dev

ship-dev : package-dev 
	@echo SHIPPING: package-dev to $(SHIP_TO)
	@$(SSH) $(SHIP_TO_ACCOUNT) mkdir -p $(SHIP_TO_DIR) && $(SCP) $(SCP_OPTIONS) $(PACKAGE_DEV_PATH) $(SHIP_TO)

.PHONY : ship-docs-dev

ship-docs-dev : package-docs-dev 
	@echo SHIPPING: package-docs-dev to $(SHIP_TO)
	@$(SSH) $(SHIP_TO_ACCOUNT) mkdir -p $(SHIP_TO_DIR) && $(SCP) $(SCP_OPTIONS) $(PACKAGE_DOCSDEV_PATH) $(SHIP_TO)

.PHONY : ship-raw-docs-dev

ship-raw-docs-dev : docs-dev
	@-$(SSH) $(SHIP_TO_ACCOUNT) mkdir -p $(SHIP_TO_DIR)/$(PROJECT_NAME)-docs-dev && ( cd $(OUTPUT_DOCS_DIR) && $(SCP) $(SCP_OPTIONS) $(OUTPUT_DOCS_DIR) $(SHIP_TO)-docs-dev )


.PHONY : check

check : test

.PHONY : test

.PHONY : ship-testresults

ship-testresults : 

.PHONY : ship-raw-testresults

ship-raw-testresults : 

.PHONY : testresults

testresults : test


ifeq ($(CROSS_COMPILING),1)
test : native-test
else

CLEAN_FILES+=$(foreach f,$(LIB_TESTS_EXE_FILES),$(basename $(f)).err $(basename $(f)).out ) $(foreach f,$(notdir $(LIB_TESTS_SH_FILES)),$(OUTPUT_TESTS_DIR)/$(basename $(f)).sh.err $(OUTPUT_TESTS_DIR)/$(basename $(f)).sh.out )


test: lib tools $(LIB_TESTS_SH_FILES) $(LIB_TESTS_EXE_FILES)
	@cd "$(OUTPUT_TESTS_DIR)"; \
	for i in $(LIB_TESTS_EXE_FILES); \
	do \
	  n=$$(basename $$i); \
	  if $(VALGRIND) $(VALGRIND_OPTIONS) "./$${n}" $(TEST_ARGS) >"$${n}.out" 2>"$${n}.err"; then echo SUCCESS:$${n}; else echo  FAIL:$${n}; fi;\
	done; \
	for i in $(LIB_TESTS_SH_FILES); \
	do \
	  n=$$(basename $$i); \
	  if /bin/bash "$${i}" >"$${n}.out" 2>"$${n}.err"; then echo SUCCESS:$${n}; else echo  FAIL:$${n}; fi;\
	done

ship-raw-testresults : test
	@-( cd $(OUTPUT_TESTS_DIR) && $(SSH) $(SHIP_TO_ACCOUNT) mkdir -p $(SHIP_TO_DIR)/$(PROJECT_NAME)-tests && $(SCP) $(SCP_OPTIONS) $(OUTPUT_TESTS_DIR)/* $(SHIP_TO)/$(PROJECT_NAME)-tests/ )

ship-testresults : package-testresults 
	@echo SHIPPING: package-testresults to $(SHIP_TO)
	@$(SSH) $(SHIP_TO_ACCOUNT) mkdir -p $(SHIP_TO_DIR) && $(SCP) $(SCP_OPTIONS) $(PACKAGE_TESTRESULTS_PATH) $(SHIP_TO)
endif

########################################################################
# magic-help.mk

.PHONY : compile_info

compile_info :
	@echo "CC=$(CC)"
	@echo "CXX=$(CXX)"
	@echo "AR=$(AR)"
	@echo "RANLIB=$(RANLIB)"
	@echo "COMPILE_FLAGS=$(COMPILE_FLAGS)"
	@echo
	@echo "PREPROCESS_FLAGS=$(PREPROCESS_FLAGS)"
	@echo
	@echo "LINK_FLAGS=$(LINK_FLAGS)"
	@echo
	@echo "LDLIBS=$(LDLIBS)"
	@echo
	@echo "LDLIBS_NO_OPTS=$(LDLIBS_NO_OPTS)"
	@echo
ifneq ($(PKGCONFIG_PACKAGES),)
	@echo "PKGCONFIG_PACKAGES=$(PKGCONFIG_PACKAGES)"
	@echo
endif
ifneq ($(CONFIG_TOOLS),)
	@echo "CONFIG_TOOLS=$(CONFIG_TOOLS)"
	@echo
endif
ifneq ($(LDLIBS_PACKAGES),)
	@echo "LDLIBS_PACKAGES=$(LDLIBS_PACKAGES)"
	@echo
endif
	@echo "OUTPUT_LIB=$(OUTPUT_LIB)"
	@echo
	@echo "LIB_DIRS=$(LIB_DIRS)"
	@echo
ifneq ($(PLATFORM_DIRS),)
	@echo "PLATFORM_DIRS=$(PLATFORM_DIRS)"
	@echo
endif
	@echo "ALL_SRC_DIRS=$(ALL_SRC_DIRS)"
	@echo
	@echo "LIB_INCLUDE_DIR=$(LIB_INCLUDE_DIR)"
	@echo
	@echo "LIB_SRC_DIR=$(LIB_SRC_DIR)"
	@echo
ifneq ($(LIB_TESTS_DIR),)
	@echo "LIB_TESTS_DIR=$(LIB_TESTS_DIR)"
	@echo
endif
ifneq ($(LIB_TOOLS_DIR),)
	@echo "LIB_TOOLS_DIR=$(LIB_TOOLS_DIR)"
	@echo
endif
ifneq ($(LIB_TOOLS_DEV_DIR),)
	@echo "LIB_TOOLS_DEV_DIR=$(LIB_TOOLS_DEV_DIR)"
	@echo
endif
ifneq ($(LIB_EXAMPLES_DIR),)
	@echo "LIB_EXAMPLES_DIR=$(LIB_EXAMPLES_DIR)"
	@echo
endif
ifneq ($(LIB_EXMAPLES_DEV_DIR),)
	@echo "LIB_EXAMPLES_DEV_DIR=$(LIB_EXAMPLES_DEV_DIR)"
	@echo
endif
ifneq ($(LIB_CPP_FILES),)
	@echo "LIB_CPP_FILES=$(LIB_CPP_FILES)"
	@echo
endif
ifneq ($(LIB_CC_FILES),)
	@echo "LIB_CC_FILES=$(LIB_CC_FILES)"
	@echo
endif
ifneq ($(LIB_C_FILES),)
	@echo "LIB_C_FILES=$(LIB_C_FILES)"
	@echo
endif
ifneq ($(LIB_MM_FILES),)
	@echo "LIB_MM_FILES=$(LIB_MM_FILES)"
	@echo
endif
ifneq ($(LIB_M_FILES),)
	@echo "LIB_M_FILES=$(LIB_M_FILES)"
	@echo
endif
ifneq ($(LIB_RC_FILES),)
	@echo "LIB_RC_FILES=$(LIB_RC_FILES)"
	@echo
endif
ifneq ($(LATEX_DOCS_TEX),)
	@echo "LATEX_DOCS_TEX=$(LATEX_DOCS_TEX)"
	@echo
endif
ifneq ($(LATEX_DOCS_PDF),)
	@echo "LATEX_DOCS_PDF=$(LATEX_DOCS_PDF)"
	@echo
endif
	@echo "LIB_O_FILES=$(notdir $(LIB_O_FILES))"
	@echo
ifneq ($(LIB_TOOLS_EXE_FILES),)
	@echo "LIB_TOOLS_EXE_FILES=$(notdir $(LIB_TOOLS_EXE_FILES))"
	@echo
endif
ifneq ($(LIB_TOOLS_DEV_EXE_FILES),)
	@echo "LIB_TOOLS_DEV_EXE_FILES=$(notdir $(LIB_TOOLS_DEV_EXE_FILES))"
	@echo
endif
ifneq ($(LIB_EXAMPLES_EXE_FILES),)
	@echo "LIB_EXAMPLES_EXE_FILES=$(notdir $(LIB_EXAMPLES_EXE_FILES))"
	@echo
endif
ifneq ($(LIB_TESTS_EXE_FILES),)
	@echo "LIB_TESTS_EXE_FILES=$(notdir $(LIB_TESTS_EXE_FILES))"
	@echo
endif
	@echo "TEST_ARGS=$(TEST_ARGS)"
	@echo
	@echo "BUILD_DIR=$(OUTPUT_DIR)"
	@echo "OUTPUT_DIR=$(OUTPUT_DIR)"
	@echo "OUTPUT_LIB_DIR=$(OUTPUT_LIB_DIR)"
	@echo "OUTPUT_TESTS_DIR=$(OUTPUT_TESTS_DIR)"
	@echo "OUTPUT_DOCS_DIR=$(OUTPUT_DOCS_DIR)"
	@echo "OUTPUT_DOCS_DEV_DIR=$(OUTPUT_DOCS_DEV_DIR)"
	@echo "OUTPUT_TOOLS_DIR=$(OUTPUT_TOOLS_DIR)"
	@echo "OUTPUT_EXAMPLES_DIR=$(OUTPUT_EXAMPLES_DIR)"
	@echo "OUTPUT_EXAMPLES_DEV_DIR=$(OUTPUT_EXAMPLES_DEV_DIR)"
	@echo "OUTPUT_OBJ_DIR=$(OUTPUT_OBJ_DIR)"
	@echo "TARGET_INSTALL_DIR=$(TARGET_INSTALL_DIR)"
	@echo "INSTALL_DIR=$(TARGET_INSTALL_DIR)"
	@echo "INSTALL_BIN_DIR=$(INSTALL_DIR)/$(TARGET_BIN_DIR)"
	@echo "INSTALL_LIB_DIR=$(INSTALL_DIR)/$(TARGET_LIB_DIR)"
	@echo "INSTALL_INCLUDE_DIR=$(INSTALL_DIR)/$(TARGET_INCLUDE_DIR)"
	@echo "INSTALL_DOCS_DIR=$(INSTALL_DIR)/$(TARGET_DOCS_DIR)"
	@echo "INSTALL_SHARE_DIR=$(INSTALL_DIR)/$(TARGET_SHARE_DIR)"
	@echo "INSTALL_ETC_DIR=$(INSTALL_DIR)/$(TARGET_ETC_DIR)"
	@echo "INSTALLDEV_BIN_DIR=$(INSTALL_DIR)/$(TARGET_BIN_DIR)"
	@echo "INSTALLDEV_LIB_DIR=$(INSTALL_DIR)/$(TARGET_LIB_DIR)"
	@echo "INSTALLDEV_INCLUDE_DIR=$(INSTALL_DIR)/$(TARGET_INCLUDE_DIR)"
	@echo "INSTALLDEV_DOCS_DIR=$(INSTALL_DIR)/$(TARGET_DOCS_DIR)"
	@echo "INSTALLDEV_SHARE_DIR=$(INSTALL_DIR)/$(TARGET_SHARE_DIR)"
	@echo "INSTALLDEV_ETC_DIR=$(INSTALL_DIR)/$(TARGET_ETC_DIR)"
	@echo "INSTALLDOCSDEV_DOCS_DIR=$(INSTALL_DIR)/$(TARGET_DOCS_DIR)"
	@echo
	@echo "PACKAGER=$(PACKAGER)"
	@echo "PACKAGE_FILE=$(PACKAGE_FILE)"
	@echo "PACKAGE_DEV_FILE=$(PACKAGE_DEV_FILE)"
	@echo "PACKAGE_DOCSDEV_FILE=$(PACKAGE_DOCSDEV_FILE)"

ifeq ($(CROSS_COMPILING),1)
	@echo "NATIVE_CC=$(NATIVE_CC)"
	@echo "NATIVE_CXX=$(NATIVE_CXX)"
	@echo "NATIVE_AR=$(NATIVE_AR)"
	@echo "NATIVE_RANLIB=$(NATIVE_RANLIB)"
	@echo "NATIVE_COMPILE_FLAGS=$(NATIVE_COMPILE_FLAGS)"
	@echo
	@echo "NATIVE_LINK_FLAGS=$(NATIVE_LINK_FLAGS)"
	@echo
	@echo "NATIVE_LDLIBS=$(NATIVE_LDLIBS)"
	@echo
	@echo "NATIVE_OUTPUT_LIB=$(NATIVE_OUTPUT_LIB)"
	@echo
	@echo "NATIVE_LIB_SRC_DIR=$(NATIVE_LIB_SRC_DIR)"
	@echo
	@echo "NATIVE_LIB_TOOLS_DIR=$(NATIVE_LIB_TOOLS_DIR)"
	@echo
	@echo "NATIVE_PLATFORM_DIRS=$(NATIVE_PLATFORM_DIRS)"
	@echo
	@echo "NATIVE_LIB_O_FILES=$(NATIVE_LIB_O_FILES)"
	@echo
	@echo "NATIVE_LIB_TOOLS_EXE_FILES=$(NATIVE_LIB_TOOLS_EXE_FILES)"
	@echo
	@echo "NATIVE_LIB_TOOLS_DEV_EXE_FILES=$(NATIVE_LIB_TOOLS_DEV_EXE_FILES)"
	@echo
	@echo "NATIVE_LIB_TESTS_EXE_FILES=$(NATIVE_LIB_TESTS_EXE_FILES)"
	@echo
	@echo "NATIVE_LIB_CPP_FILES=$(NATIVE_LIB_CPP_FILES)"
	@echo
endif

.PHONY : help

help :
	@echo "TOP of source is $(PROJECT_TOP_DIR)"
	@echo "compile_info : show compile flags, directories, options"
	@echo "lib : build library $(OUTPUT_LIB)"
	@echo "TAGS : make etags for emacs"
	@echo "tags : make ctags for vi"
	@echo "docs : html docs"
	@echo "docs-dev : build doxygen docs"
	@echo "clean : clean intermediate files"
	@echo "distclean / realclean : clean all built files (except docs)"
	@echo "tools : build tool programs $(LIB_TOOLS_EXE_FILES)"
	@echo "examples : build example programs $(LIB_EXAMPLES_EXE_FILES)"
	@echo "tests : build test programs $(LIB_TESTS_EXE_FILES)"
	@echo "test : run tests, sh scripts and test programs"
	@echo "dirs : create output dirs $(OUTPUT_DIR)"
	@echo "preinstall : install tool executables files into $(LOCAL_INSTALL_DIR)"
	@echo "preinstall-docs : copy html docs into $(LOCAL_INSTALL_DIR)"
	@echo "preinstall-dev : install tool executables, include files and library files into $(LOCAL_INSTALLDEV_DIR)"
	@echo "preinstall-docs-dev : install tool executables, include files, library files, and doxygen docs into $(LOCAL_INSTALL_DIR)"
	@echo "package : make package of executables"
	@echo "package-testresults : make package of test results"
	@echo "package-dev : make package of development files"
	@echo "package-docs-dev : make package of development doxygen documentation"
	@echo "packages : make all packages"
	@echo "install : install tool executables files into $(INSTALL_DIR)"
	@echo "install-docs : copy html docs into $(INSTALL_DOCS_DIR)"
	@echo "install-dev : install tool executables, include files and library files into $(INSTALLDEV_DIR)"
	@echo "install-docs-dev : install tool executables, include files, library files, and doxygen docs into $(INSTALLDEVDOCS_DIR)"
	@echo "ship : ship main package to another computer via scp, use --ship-to configure option. Currently $(SHIP_TO)"
	@echo "ship-dev : ship dev package to another computer via scp"
	@echo "ship-docs-dev : ship dev documentation package to another computer via scp"
	@echo "ship-testresults : ship testresults package to another computer via scp"
	@echo "ship-raw-testresults : ship testresults files to another computer via scp"
	@echo "ship-raw-docs-dev : ship doxygen files to another computer via scp"
	@echo "ship-all : ship all packages to another computer via scp"
ifeq ($(CROSS_COMPILING),1)
	@echo "native-dirs : create output dirs for native build in $(NATIVE_OUTPUT_DIR)"
	@echo "native-lib : build native library $(NATIVE_OUTPUT_LIB)"
	@echo "native-tests : build native tests $(NATIVE_LIB_TESTS_EXE_FILES)"
	@echo "native-test : run native tests."
endif


ifeq ($(MAKECMDGOALS),clean)
GOALISCLEAN=1
endif
ifeq ($(MAKECMDGOALS),realclean)
GOALISCLEAN=1
endif
ifeq ($(MAKECMDGOALS),distclean)
GOALISCLEAN=1
endif

ifneq ($(GOALISCLEAN),1)
# include any dependencies created during the last make.
-include $(OUTPUT_OBJ_DIR)/*.d
-include $(NATIVE_OUTPUT_OBJ_DIR)/*.d
endif
