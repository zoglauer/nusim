#----------------------------------------------------------------
#
#  Makefile for NuSIM
#
#  Author: Andreas Zoglauer
#
#----------------------------------------------------------------



#----------------------------------------------------------------
# Definitions:
#
LOBJS = $(LB)/NTime.o \
	$(LB)/NQuaternion.o \
	$(LB)/NSource.o \
	$(LB)/NEvent.o \
	$(LB)/NHit.o \
	$(LB)/NInteraction.o \
	$(LB)/NOrientation.o \
	$(LB)/NAlignmentsDBEntry.o \
	$(LB)/NOrbit.o \
	$(LB)/NPhoton.o \
	$(LB)/NPixelHit.o \
	$(LB)/NNinePixelHit.o \
	$(LB)/NShieldHit.o \
	$(LB)/NPHEFitsData.o \
	$(LB)/NPointing.o \
	$(LB)/NPointingJitterDBEntry.o \
	$(LB)/NStarTrackerDataSet.o \
	$(LB)/NStarTrackerData.o \
	$(LB)/NMetrologyDataSet.o \
	$(LB)/NMetrologyData.o \
	$(LB)/NMetrologyDBEntry.o \
	$(LB)/NMetrologyDetectorShift.o \
	$(LB)/NOpticsDBEntry.o \
	$(LB)/NObservatoryData.o \
	$(LB)/NExtractFitsImage.o \
	$(LB)/NDetectorMap.o \
	$(LB)/NExposureMap.o \
	$(LB)/NHotPixel.o \


SOBJ = $(LB)/libData.$(DLL)

#----------------------------------------------------------------
# Commands:
#

all: $(SOBJ) 

link:
		@$(LINK) $(shell pwd)/inc/*.h $(IN)
		@rm -f $(IN)/LinkDef.h

clean:
		@rm -f src/*~
		@rm -f src/\#*
		@rm -f inc/*~
		@rm -f inc/\#*
		@rm -f *~
		@rm -f \#*
		@rm -f $(LOBJS) $(SOBJ)


#----------------------------------------------------------------
# Dependencies:
#

$(SOBJ): $(LOBJS)
	@echo "Linking $(subst $(LB)/,,$@) ..."
	@$(LD) $(LDFLAGS) $(SOFLAGS) $(LOBJS) $(GLIBS) $(LIBS) -o $(SOBJ)

$(LOBJS): $(LB)/%.o: src/%.cxx inc/%.h
	@echo "Compiling $(subst src/,,$<) ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@


#
#----------------------------------------------------------------



