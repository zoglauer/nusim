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
LOBJS = $(LB)/NGlobal.o \

SOBJ = $(LB)/libGlobal.$(DLL)

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



