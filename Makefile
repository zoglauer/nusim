
#------------------------------------------------------------------------------
#
#  Makefile for NuSim
#
#  Author: Andreas Zoglauer
#
#------------------------------------------------------------------------------



#------------------------------------------------------------------------------
# Program and directories

SHELL=/bin/sh

# Basic directories
TOP						= $(shell pwd)
SR						= $(TOP)/src
IN						= $(TOP)/include
LB						= $(TOP)/lib
BN            = $(TOP)/bin
HT            = $(TOP)/doc/html
CT						= $(TOP)/cint
CF						= $(TOP)/config


#------------------------------------------------------------------------------
# Included options (result of ./configure)

include $(TOP)/config/Makefile.options
include $(TOP)/config/Makefile.config

CXXFLAGS		+= -I$(IN)
LDFLAGS     += -L$(LB)


#------------------------------------------------------------------------------
# Commands:

#.NOPARALLEL:
.EXPORT_ALL_VARIABLES: all main megalib modules data orbit
.SILENT:
.NOTPARALLEL: info main megalib modules data orbit
.SUFFIXES:
.SUFFIXES: .cxx .h .o .so


#------------------------------------------------------------------------------
# NuSIM

NUSIMLIBS = \
	-lMain \
	-lModules \
	-lMegalib \
	-lOrbit \
	-lData \

all: info link modules data main megalib orbit
	@$(MAKE) all -C src

nusim: all
	@$(BN)/nusim

main: link megalib modules data orbit
	@$(MAKE) main -C src

megalib: link
	@$(MAKE) megalib -C src

modules: link megalib orbit
	@$(MAKE) modules -C src

data: link megalib orbit
	@$(MAKE) data -C src

orbit: link
	@$(MAKE) orbit -C src

debug: all
	@gdb $(BN)/nusim -x resource/gdb.ini

profile: all  
	@valgrind --tool=callgrind -v --dump-instr=yes $(BN)/nusim

#------------------------------------------------------------------------------
# Version check:

ROOTVERSIONOK   = $(shell bash $(CF)/configure_rootversiontest )
#GEANT4VERSIONOK = $(shell bash $(CF)/configure_geant4versiontest )

info:
	bash $(CF)/configure_revision
ifneq ($(strip $(ROOTVERSIONOK)),)
	echo "$(ROOTVERSIONOK)"
	exit 1;		
endif
	echo "NuSIM compilation mode: $(CMODE)"

#ifneq ($(strip $(GEANT4VERSIONOK)),)
#	echo "$(GEANT4VERSIONOK)"
#	exit 1;		
#endif



#------------------------------------------------------------------------------
# additional options:

link:
	$(MAKE) -s link -C src 

clean:
	@$(MAKE) -s clean -C src 
	@rm -f $(LB)/*.o
	@rm -f $(LB)/*.$(DLL)
ifneq ($(DLL),so)
	@rm -f $(LB)/*.so
endif
	@rm -f $(LB)/*Cint*
	@rm -rf $(LB)/*SunWS_cache*
	@rm -f *~
	@rm -f \#*
	@rm -f $(IN)/*.h

dist_clean: clean
	@rm -rf bin
	@rm -rf lib
	@rm -rf include

html: man
doxygen: man

man:
	@rm -rf $(HT)
	@sh resource/doxy
	@doxygen resource/Doxyfile

changelog:
	@sh bin/changelog.sh
	@less doc/ChangeLog

texdoc:
	@$(MAKE) -C doc 



#
#------------------------------------------------------------------------------


