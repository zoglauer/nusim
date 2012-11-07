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
	$(LB)/NUnfilteredEvents.o \
	$(LB)/NPhaFile.o \
	$(LB)/NBaseTool.o \
	$(LB)/NCheckRates.o \
	$(LB)/NQuickView.o \
	$(LB)/NQuickViewFiltered.o \
	$(LB)/NBackgroundModes.o \
	$(LB)/NBackgroundMode1.o \
	$(LB)/NBackgroundMode23.o \
	$(LB)/NBackgroundMode4.o \
	$(LB)/NBackgroundMode4DataBase.o \
	$(LB)/NLineFitter.o \
	$(LB)/NDetectorHealth.o \

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
ifeq ($(ARCH),macosx)
	$(LD) $(SOFLAGS) $(LOBJS) $(GLIBS) $(LIBS) -o $(SOBJ)
	$(LD) -bundle -undefined suppress -Wl,-x $(LDFLAGS) $^ $(GLIBS) $(LIBS) -o $(subst .$(DLL),.so,$@)
else
	@$(LD) $(LDFLAGS) $(SOFLAGS) $(LOBJS) $(GLIBS) $(LIBS) -o $(SOBJ)
endif


$(MOBJS): $(LB)/%.o: src/%.cxx inc/%.h
	@echo "Compiling $(subst src/,,$<) ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(LOBJS): $(LB)/%.o: src/%.cxx inc/%.h
	@echo "Compiling $(subst src/,,$<) ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(PRG): $(SOBJ) $(MOBJS)
	@echo "Linking $(subst $(BN)/,,$@) ..."
	@$(LD) $(LDFLAGS) $(MOBJS) -lNulyses -lMegalib -lGlobal $(GLIBS) $(LIBS) -o $(PRG)


#
#----------------------------------------------------------------



