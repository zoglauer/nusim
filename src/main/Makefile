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

PRG = $(BN)/nusim

MOBJS     = $(LB)/NMain.o

LOBJS     = $(LB)/NGUIMain.o \
						$(LB)/NGUIModuleSelector.o \
						$(LB)/NGUISupervisor.o \
						$(LB)/NGUIAbout.o \
						$(LB)/NTextButton.o \
						$(LB)/NInterface.o \
						$(LB)/NSupervisor.o \
						$(LB)/NGlobal.o \

SOBJ = $(LB)/libMain.$(DLL)

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
	@$(LD) $(LDFLAGS) $(NUSIMLIBS) $(MOBJS) $(GLIBS) $(LIBS) -o $(PRG)


#
#----------------------------------------------------------------



