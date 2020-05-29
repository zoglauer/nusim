#----------------------------------------------------------------
#
#  Makefile for Mimrec
#
#  Author: Andreas Zoglauer
#
#----------------------------------------------------------------



#----------------------------------------------------------------
# Definitions:
#

PRG = $(BN)/nulyses

MOBJS     = $(LB)/NNulyses.o

LOBJS     = $(LB)/NBase.o \
	$(LB)/NAttitude.o \
	$(LB)/NOrbits.o \
	$(LB)/NEngineering.o \
	$(LB)/NFilteredEvents.o \
	$(LB)/NHousekeeping.o \
	$(LB)/NGTI.o \
	$(LB)/NUnfilteredEvents.o \
	$(LB)/NBadPixel.o \
	$(LB)/NPhaFile.o \
	$(LB)/NBaseTool.o \
	$(LB)/NCheckRates.o \
	$(LB)/NQuickView.o \
	$(LB)/NQuickViewFiltered.o \
	$(LB)/NBackgroundVariations.o \
	$(LB)/NBackgroundModes.o \
	$(LB)/NBackgroundMode1.o \
	$(LB)/NBackgroundMode23.o \
	$(LB)/NBackgroundMode4.o \
	$(LB)/NBackgroundMode4DataBase.o \
	$(LB)/NBackgroundTester.o \
	$(LB)/NLineFitter.o \
	$(LB)/NDetectorHealth.o \
	$(LB)/NApertureModel.o \
	$(LB)/NApertureTester.o \
	$(LB)/NPointSourceFitter.o \
	$(LB)/NMEGAlibExtract.o \

SOBJ = $(LB)/libNulyses.$(DLL)

#----------------------------------------------------------------
# Commands:
#

all: $(PRG)

lib: $(SOBJ)

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
	@rm -f $(PRG)
	@rm -f $(MOBJS) $(LOBJS) $(SOBJ) $(PRG)

#----------------------------------------------------------------
# Dependencies:
#


$(SOBJ): $(LOBJS)
	@echo "Linking $(subst $(LB)/,,$@) ..."
	@$(LD) $(LDFLAGS) $(SOFLAGS) $(LOBJS) $(GLIBS) $(LIBS) -o $(SOBJ)


$(MOBJS): $(LB)/%.o: src/%.cxx inc/%.h
	@echo "Compiling $(subst src/,,$<) ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(LOBJS): $(LB)/%.o: src/%.cxx inc/%.h
	@echo "Compiling $(subst src/,,$<) ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(PRG): $(SOBJ) $(MOBJS)
	@echo "Linking $(subst $(BN)/,,$@) ..."
	@$(LD) $(LDFLAGS) $(MOBJS) -lNulyses -lMegalib -lGlobal -lData $(GLIBS) $(LIBS) -o $(PRG)


#
#----------------------------------------------------------------



