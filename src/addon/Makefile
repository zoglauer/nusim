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

ALLLIBS = $(shell $(BN)/alllibs $(LB) $(DLL) )



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
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(ALLLIBS) $(DEFINES) -I$(LHEASOFT)/include -lcfitsio -lm $< $(GLIBS) $(LIBS) -L$(LHEASOFT)/lib -o $@



#
#----------------------------------------------------------------



