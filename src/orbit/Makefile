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
LOBJS = $(LB)/CalculateIntervalsIntersection.o \
	$(LB)/CalculateIntervals.o \
	$(LB)/CelestialTarget.o \
	$(LB)/Quat.o \
	$(LB)/Dirvector.o \
	$(LB)/Euler.o \
	$(LB)/GUException.o \
	$(LB)/GUStringvec.o \
	$(LB)/GUtil.o \
	$(LB)/NoradTle.o \
	$(LB)/OrbitalIntervals.o \
	$(LB)/OrbitalNightIntervals.o \
	$(LB)/OrbitalOccultationIntervals.o \
	$(LB)/OrbitModel.o \
	$(LB)/SGP4.o \
	$(LB)/StateVector.o \
	$(LB)/TmTcDuration.o \
	$(LB)/TmTcInterval.o \
	$(LB)/TmTcIntervalSet.o \
	$(LB)/TmTcTime.o \
	$(LB)/TmTcUtil.o \


SOBJ = $(LB)/libOrbit.$(DLL)

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
ifeq ($(ARCH),macosx)
	$(LD) $(SOFLAGS) $(LOBJS) $(GLIBS) $(LIBS) -o $(SOBJ)
	$(LD) -bundle -undefined suppress -Wl,-x $(LDFLAGS) $^ $(GLIBS) $(LIBS) -o $(subst .$(DLL),.so,$@)
else
	@$(LD) $(LDFLAGS) $(SOFLAGS) $(LOBJS) $(GLIBS) $(LIBS) -o $(SOBJ)
endif

$(LOBJS): $(LB)/%.o: src/%.cc inc/%.h
	@echo "Compiling $(subst src/,,$<) ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@


#
#----------------------------------------------------------------



