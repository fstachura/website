# See LICENSE.Batten for license details.
#
#=========================================================================
# Toplevel Makefile for the Modular C++ Build System
#=========================================================================
# Please read the documenation in 'mcppbs-doc.txt' for more details on
# how the Modular C++ Build System works. For most projects, a developer
# will not need to make any changes to this makefile. The key targets
# are as follows:
#
#  - default   : build all libraries and programs
#  - check     : build and run all unit tests
#  - install   : install headers, project library, and some programs
#  - clean     : remove all generated content (except autoconf files)
#  - dist      : make a source tarball
#  - distcheck : make a source tarball, untar it, check it, clean it
#  - distclean : remove everything
#

#-------------------------------------------------------------------------
# Basic setup
#-------------------------------------------------------------------------

# Remove all default implicit rules since they can cause subtle bugs
# and they just make things run slower
.SUFFIXES:
% : %,v
% : RCS/%,v
% : RCS/%
% : s.%
% : SCCS/s.%

# Default is to build the prereqs of the all target (defined at bottom)
default : all
.PHONY : default

project_name := riscv-pk
src_dir      := .
scripts_dir  := $(src_dir)/scripts

# If the version information is not in the configure script, then we
# assume that we are in a working directory. We use the vcs-version.sh
# script in the scripts directory to generate an appropriate version
# string. Currently the way things are setup we have to run this script
# everytime we run make so the script needs to be as fast as possible.

ifeq (?,?)
  project_ver:=$(shell $(scripts_dir)/vcs-version.sh $(src_dir))
else
  project_ver:=?
endif

# If --with-arch is not specified, it defaults to whatever the compiler's
# default is. The -with-abi is not necessary for this project. Unconditionally
# compile it with a no-float ABI. i.e., ilp32 for 32-bit and lp64 for 64-bit.

ifeq (,)
  march := -march=$(shell gcc -v 2>&1 | \
                    sed 's/ /\n/g' | grep with-arch | awk -F= '{print $$2}')
else
  march := -march=
endif
is_32bit := $(findstring 32,$(march))
mabi := -mabi=$(if $(is_32bit),ilp32,lp64)

# Installation directories

prefix       := /usr/local
enable_stow  := no

ifeq ($(enable_stow),yes)
  stow_pkg_dir := $(prefix)/pkgs
  INSTALLDIR ?= $(DESTDIR)/$(stow_pkg_dir)/$(project_name)-$(project_ver)
else
  ifeq ($(is_32bit),)
    install_subdir := riscv64-unknown-elf
  else
    install_subdir := $(shell echo riscv64-unknown-elf | sed -e 's/64/32/g')
  endif
  INSTALLDIR ?= $(DESTDIR)/$(prefix)/$(install_subdir)
endif

install_hdrs_dir := $(INSTALLDIR)/include/$(project_name)
install_libs_dir := $(INSTALLDIR)/lib/$(project_name)
install_exes_dir := $(INSTALLDIR)/bin

#-------------------------------------------------------------------------
# List of subprojects
#-------------------------------------------------------------------------

sprojs         :=  pk bbl softfloat dummy_payload machine util
sprojs_enabled :=  pk bbl softfloat dummy_payload machine util

sprojs_include := -I. $(addprefix -I$(src_dir)/, $(sprojs_enabled))
VPATH := $(addprefix $(src_dir)/, $(sprojs_enabled))

#-------------------------------------------------------------------------
# Programs and flags 
#-------------------------------------------------------------------------

# C++ compiler
#  - CPPFLAGS : flags for the preprocessor (eg. -I,-D)
#  - CXXFLAGS : flags for C++ compiler (eg. -Wall,-g,-O3)

CC            := gcc
READELF       := readelf
OBJCOPY       := objcopy
CFLAGS        := -Wall -Werror -D__NO_INLINE__ -mcmodel=medany -O2 -std=gnu99 -Wno-unused -Wno-attributes -fno-delete-null-pointer-checks -fno-PIE $(CFLAGS) $(march) $(mabi) -DBBL_PAYLOAD=\"bbl_payload\" -DBBL_LOGO_FILE=\"bbl_logo_file\"
BBL_PAYLOAD   := dummy_payload
COMPILE       := $(CC) -MMD -MP $(CFLAGS) \
                 $(sprojs_include)
# Linker
#  - LDFLAGS : Flags for the linker (eg. -L)
#  - LIBS    : Library flags (eg. -l)

LD            := $(CC)
LDFLAGS       :=  -Wl,--build-id=none -nostartfiles -nostdlib -static $(LDFLAGS) $(march) $(mabi)
LIBS          := -lgcc
LINK          := $(LD) $(LDFLAGS)

# Library creation

AR            := ar
RANLIB        := ranlib

# Host simulator

RUN           := @RUN@
RUNFLAGS      := @RUNFLAGS@

# Installation

MKINSTALLDIRS := $(scripts_dir)/mk-install-dirs.sh
INSTALL       := /usr/bin/install -c
INSTALL_HDR   := $(INSTALL) -m 444
INSTALL_LIB   := $(INSTALL) -m 644
INSTALL_EXE   := $(INSTALL) -m 555
STOW          := 

#-------------------------------------------------------------------------
# Include subproject makefile fragments
#-------------------------------------------------------------------------

sprojs_mk = $(addsuffix .mk, $(sprojs_enabled))

-include $(sprojs_mk)

dist_junk += $(sprojs_mk)

#-------------------------------------------------------------------------
# Reverse list helper function
#-------------------------------------------------------------------------
# This function is used by the subproject template to reverse the list
# of dependencies. It uses recursion to perform the reversal.
# 
# Arguments:
#  $(1)   : space separated input list
#  retval : input list in reverse order
#

reverse_list = $(call reverse_list_h,$(1),)
define reverse_list_h
  $(if $(strip $(1)),                        \
    $(call reverse_list_h,                   \
           $(wordlist 2,$(words $(1)),$(1)), \
           $(firstword $(1)) $(2)),          \
    $(2))
endef

#-------------------------------------------------------------------------
# Template for per subproject rules
#-------------------------------------------------------------------------
# The template is instantiated for each of the subprojects. It relies on
# subprojects defining a certain set of make variables which are all
# prefixed with the subproject name. Since subproject names can have
# dashes in them (and the make variables are assumed to only use
# underscores) the template takes two arguments - one with the regular 
# subproject name and one with dashes replaced with underscores.
#
# Arguments:
#  $(1) : real subproject name (ie with dashes)
#  $(2) : normalized subproject name (ie dashes replaced with underscores)
#

define subproject_template

# In some (rare) cases, a subproject might not have any actual object
# files. It might only include header files or program sources. To keep
# things consistent we still want a library for this subproject, so in
# this spectial case we create a dummy source file and thus the build
# system will create a library for this subproject with just the
# corresponding dummy object file.

ifeq ($$(strip $$($(2)_c_srcs)),)
$(2)_c_srcs += _$(1).c
$(2)_junk += _$(1).c
endif

_$(1).c :
	echo "int _$(2)( int arg ) { return arg; }" > $$@

# Build the object files for this subproject

$(2)_c_objs := $$(patsubst %.c, %.o, $$($(2)_c_srcs))
$(2)_asm_objs := $$(patsubst %.S, %.o, $$($(2)_asm_srcs))
$(2)_c_deps := $$(patsubst %.o, %.d, $$($(2)_c_objs))
$$($(2)_c_objs) : %.o : %.c
	$(COMPILE) -c $$<
$$($(2)_asm_objs) : %.o : %.S
	$(COMPILE) -c $$<

$(2)_junk += $$($(2)_c_objs) $$($(2)_c_deps) $$($(2)_asm_objs)

# Build a library for this subproject

lib$(1).a : $$($(2)_c_objs) $$($(2)_asm_objs)
	$(AR) rcv $$@ $$^
	$(RANLIB) $$@

$(2)_junk += lib$(1).a

# Reverse the dependency list so that a given subproject only depends on
# subprojects listed to its right. This is the correct order for linking
# the list of subproject libraries.

$(2)_reverse_deps   := $$(call reverse_list,$$($(2)_subproject_deps))

# Build unit tests

$(2)_test_objs      := $$(patsubst %.c, %.o, $$($(2)_test_srcs))
$(2)_test_deps      := $$(patsubst %.o, %.d, $$($(2)_test_objs))
$(2)_test_exes      := $$(patsubst %.t.c, %-utst, $$($(2)_test_srcs))
$(2)_test_outs      := $$(patsubst %, %.out, $$($(2)_test_exes))
$(2)_test_libs      := $(1) $$($(2)_reverse_deps) utst
$(2)_test_libnames  := $$(patsubst %, lib%.a, $$($(2)_test_libs))
$(2)_test_libarg    := -L. $$(patsubst %, -l%, $$($(2)_test_libs))

$$($(2)_test_objs) : %.o : %.c
	$(COMPILE) -c $$<

$$($(2)_test_exes) : %-utst : %.t.o $$($(2)_test_libnames)
	$(LINK) -o $$@ $$< $$($(2)_test_libarg) $(LIBS)

$(2)_c_deps += $$($(2)_test_deps)
$(2)_junk += \
	$$($(2)_test_objs) $$($(2)_test_deps) \
  $$($(2)_test_exes) *.junk-dat

# Run unit tests

$$($(2)_test_outs) : %.out : %
	$(RUN) $(RUNFLAGS) ./$$< default | tee $$@

$(2)_junk += $$($(2)_test_outs)

# Build programs

$(2)_prog_objs      := $$(patsubst %.c, %.o, $$($(2)_prog_srcs))
$(2)_prog_deps      := $$(patsubst %.o, %.d, $$($(2)_prog_objs))
$(2)_prog_exes      := $$(patsubst %.c, %, $$($(2)_prog_srcs))
$(2)_prog_libs      := $(1) $$($(2)_reverse_deps)
$(2)_prog_libnames  := $$(patsubst %, lib%.a, $$($(2)_prog_libs))
$(2)_prog_libarg    := -L. $$(patsubst %, -l%, $$($(2)_prog_libs))

$$($(2)_prog_objs) : %.o : %.c
	$(COMPILE) -c $$<

$$($(2)_prog_exes) : % : %.o $$($(2)_prog_libnames)
	$(LINK) -o $$@ $$< $$($(2)_prog_libarg) $(LIBS)

$(2)_c_deps += $$($(2)_prog_deps)
$(2)_junk += $$($(2)_prog_objs) $$($(2)_prog_deps) $$($(2)_prog_exes)

# Build programs which will be installed

$(2)_install_prog_objs := $$(patsubst %.c, %.o, $$($(2)_install_prog_srcs))
$(2)_install_prog_deps := $$(patsubst %.o, %.d, $$($(2)_install_prog_objs))
$(2)_install_prog_exes := $$(patsubst %.c, %, $$($(2)_install_prog_srcs))

$$($(2)_install_prog_objs) : %.o : %.c
	$(COMPILE) -c $$<

$$($(2)_install_prog_exes) : % : %.o $$($(2)_prog_libnames)
	$(LINK) -o $$@ $$< $$($(2)_prog_libarg) $(LIBS) -T $(src_dir)/$(2)/$(2).lds

$(2)_c_deps += $$($(2)_install_prog_deps)
$(2)_junk += \
  $$($(2)_install_prog_objs) $$($(2)_install_prog_deps) \
  $$($(2)_install_prog_exes)

# Subproject specific targets

all-$(1) : lib$(1).a $$($(2)_install_prog_exes)

check-$(1) : $$($(2)_test_outs)
	echo; grep -h -e'Unit Tests' -e'FAILED' -e'Segementation' $$^; echo

clean-$(1) :
	rm -rf $$($(2)_junk)

.PHONY : all-$(1) check-$(1) clean-$(1)

# Update running variables

libs += lib$(1).a
objs += $$($(2)_c_objs)
srcs += $$(addprefix $(src_dir)/$(1)/, $$($(2)_c_srcs)) 
hdrs += $$(addprefix $(src_dir)/$(1)/, $$($(2)_hdrs))
junk += $$($(2)_junk)
deps += $$($(2)_c_deps)

test_outs += $$($(2)_test_outs)

install_hdrs += $$(addprefix $(src_dir)/$(1)/, $$($(2)_hdrs))
install_libs += lib$(1).a
install_exes += $$($(2)_install_prog_exes)

endef

# Iterate over the subprojects and call the template for each one

$(foreach sproj,$(sprojs_enabled), \
  $(eval $(call subproject_template,$(sproj),$(subst -,_,$(sproj)))))

#-------------------------------------------------------------------------
# Autodependency files
#-------------------------------------------------------------------------

-include $(deps)

deps : $(deps)
.PHONY : deps

#-------------------------------------------------------------------------
# Check
#-------------------------------------------------------------------------

check : $(test_outs)
	echo; grep -h -e'Unit Tests' -e'FAILED' -e'Segementation' $^; echo

.PHONY : check

#-------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------

install-hdrs : $(install_hdrs)
	$(MKINSTALLDIRS) $(install_hdrs_dir)
	for file in $(install_hdrs); \
  do \
    $(INSTALL_HDR) $$file $(install_hdrs_dir); \
  done

install-libs : $(install_libs)
	$(MKINSTALLDIRS) $(install_libs_dir)
	for file in $(install_libs); \
  do \
    $(INSTALL_LIB) $$file $(install_libs_dir); \
  done

install-exes : $(install_exes)
	$(MKINSTALLDIRS) $(install_exes_dir)
	for file in $(install_exes); \
  do \
    $(INSTALL_EXE) $$file $(install_exes_dir); \
  done

install : install-hdrs install-libs install-exes
ifeq ($(enable_stow),yes)
	$(MKINSTALLDIRS) $(stow_pkg_dir)
	cd $(stow_pkg_dir) && \
    $(STOW) --delete $(project_name)-* && \
    $(STOW) $(project_name)-$(project_ver)
endif

.PHONY : install install-hdrs install-libs install-exes

#-------------------------------------------------------------------------
# Regenerate configure information
#-------------------------------------------------------------------------

config.status : $(src_dir)/configure
	./config.status --recheck

sprojs_mk_in = \
  $(join $(addprefix $(src_dir)/, $(sprojs_enabled)), \
         $(patsubst  %, /%.mk.in, $(sprojs_enabled)))

Makefile : $(src_dir)/Makefile.in $(sprojs_mk_in) config.status
	./config.status

dist_junk += config.status config.h Makefile config.log

#-------------------------------------------------------------------------
# Distribution
#-------------------------------------------------------------------------
# The distribution tarball is named project-ver.tar.gz and it includes
# both enabled and disabled subprojects. 

dist_files = \
  $(sprojs) \
  README \
  style-guide.txt \
  mcppbs-uguide.txt \
  scripts \
  configure.ac \
  aclocal.m4 \
  configure \
  config.h.in \
  Makefile.in \

dist_dir := $(project_name)-$(project_ver)
dist_tgz := $(project_name)-$(project_ver).tar.gz

# Notice that when we make the distribution we rewrite the configure.ac
# script with the current version and we rerun autoconf in the new
# source directory so that the distribution will have the proper version
# information. We also rewrite the "Version : " line in the README.

dist :
	rm -rf $(dist_dir)
	mkdir $(dist_dir)
	tar -C $(src_dir) -cf - $(dist_files) | tar -C $(dist_dir) -xpf -
	sed -i.bak 's/^\(# Version :\).*/\1 $(project_ver)/' $(dist_dir)/README
	sed -i.bak 's/\( proj_version,\).*/\1 [$(project_ver)])/' $(dist_dir)/configure.ac
	cd $(dist_dir) && \
    autoconf && autoheader && \
    rm -rf autom4te.cache configure.ac.bak README.bak
	tar -czvf $(dist_tgz) $(dist_dir)
	rm -rf $(dist_dir)

# You can use the distcheck target to try untarring the distribution and
# then running configure, make, make check, and make distclean. A
# "directory is not empty" error means distclean is not removing
# everything.

distcheck : dist
	rm -rf $(dist_dir)
	tar -xzvf $(dist_tgz)
	mkdir -p $(dist_dir)/build
	cd $(dist_dir)/build; ../configure; make; make check; make distclean
	rm -rf $(dist_dir)

junk += $(project_name)-*.tar.gz

.PHONY : dist distcheck

#-------------------------------------------------------------------------
# Default
#-------------------------------------------------------------------------

all : $(install_hdrs) $(install_libs) $(install_exes)
.PHONY : all

#-------------------------------------------------------------------------
# Makefile debugging
#-------------------------------------------------------------------------
# This handy rule will display the contents of any make variable by
# using the target debug-<varname>. So for example, make debug-junk will
# display the contents of the junk variable.

debug-% :
	@echo $* = $($*)

#-------------------------------------------------------------------------
# Clean up junk
#-------------------------------------------------------------------------

clean :
	rm -rf *~ \#* $(junk)

distclean :
	rm -rf *~ \#* $(junk) $(dist_junk)

.PHONY : clean distclean
