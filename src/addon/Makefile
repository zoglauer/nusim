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
	$(BN)/PulsationViewer \
	$(BN)/LightCurveViewer \
	$(BN)/FitsViewer \
	$(BN)/FitsCleaner \
	$(BN)/MakeARF \
	$(BN)/SAAComparer \


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
	$(LD) $(LDFLAGS) $(CXXFLAGS) $(DEFINES) -I$(LHEASOFT)/include $< -L$(LHEASOFT)/lib -lNulyses $(NUSIMLIBS) -lcfitsio -lm $(GLIBS) $(LIBS) -o $@



#
#----------------------------------------------------------------



