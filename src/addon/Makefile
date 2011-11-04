#----------------------------------------------------------------
#
#  Makefile for MEGAlib - addon directory
#
#  Author: Andreas Zoglauer
#
#
#  All *.cpp and *.cxx files represent exactly one program!
#
#----------------------------------------------------------------



#----------------------------------------------------------------
# Definitions:
#

PRGS = \
	$(BN)/ConcatDat \
	$(BN)/MakeARF \

ALLLIBS = -lMain -lModules -lMegalib -lOrbit -lData


#----------------------------------------------------------------
# Commands:
#

all: $(PRGS) 

clean:
		@rm -f *~
		@rm -f \#*
		@rm -f *.o
		@rm -f $(PRGS)


#----------------------------------------------------------------
# Dependencies:
#

$(PRGS): $(BN)/%: %.cxx
	@echo "Compiling and linking $(subst $(BN)/,,$@) ..."
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(DEFINES) -I$(LHEASOFT)/include $< -L$(LHEASOFT)/lib $(ALLLIBS) -lcfitsio -lm $(GLIBS) $(LIBS) -o $@



#
#----------------------------------------------------------------



