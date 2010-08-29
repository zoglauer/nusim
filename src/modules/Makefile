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
LOBJS = $(LB)/NModule.o \
	$(LB)/NModuleInterfaceEntry.o \
	$(LB)/NModuleInterfaceExit.o \
	$(LB)/NModuleInterfaceMerger.o \
	$(LB)/NModuleInterfaceEvent.o \
	$(LB)/NModuleInterfaceStarTracker.o \
	$(LB)/NModuleInterfaceMetrology.o \
	$(LB)/NModuleInterfaceObservatory.o \
	$(LB)/NModuleInterfaceOrbit.o \
	$(LB)/NModuleInterfacePointing.o \
	$(LB)/NModuleInterfaceOrientations.o \
	$(LB)/NModuleInterfacePhotonPropagation.o \
	$(LB)/NModuleInterfaceTime.o \
	$(LB)/NModuleInterfaceIO.o \
	$(LB)/NModuleInterfaceStopCriterion.o \
	$(LB)/NModuleInterfaceGeometryAndDetectorProperties.o \
	$(LB)/NModuleTemplate.o \
	$(LB)/NGUIOptionsSource.o \
	$(LB)/NGUIOptionsSourceDistribution.o \
	$(LB)/NGUIDiagnosticsSourceDistribution.o \
	$(LB)/NModuleSourceDistribution.o \
	$(LB)/NModuleOpticsEngine.o \
	$(LB)/NGUIOptionsOpticsEngine.o \
	$(LB)/NModuleOpticsEngineNone.o \
	$(LB)/NModuleOpticsEngineTrivial.o \
	$(LB)/NModuleApertureEngineNone.o \
	$(LB)/NModuleApertureEngineTrivial.o \
	$(LB)/NModuleDetectorSimulatorIdeal.o \
	$(LB)/NModuleDetectorSimulatorPropagatorOnly.o \
	$(LB)/NModuleDetectorSimulatorDetailed.o \
	$(LB)/NModuleDetectorSimulatorGeant4.o \
	$(LB)/NModuleBackgroundSimulatorTrivial.o \
	$(LB)/NGUIOptionsBackgroundSimulator.o \
	$(LB)/NModuleBackgroundSimulator.o \
	$(LB)/NModuleBackgroundSimulatorAperture.o \
	$(LB)/NModuleBackgroundSimulatorShieldOnly.o \
	$(LB)/NModuleBackgroundSimulatorDataBase.o \
	$(LB)/NModuleTriggerEngineIdeal.o \
	$(LB)/NModuleTriggerEnginePHE.o \
	$(LB)/NModuleTriggerEngineSciSim.o \
	$(LB)/NModuleTriggerEngineBackgroundSims.o \
	$(LB)/NGUIDiagnosticsDetectorEffectsEngine.o \
	$(LB)/NGUIOptionsDetectorEffectsEnginePHE.o \
	$(LB)/NModuleDetectorEffectsEngineSciSim.o \
	$(LB)/NModuleDetectorEffectsEngineIdeal.o \
	$(LB)/NModuleDetectorEffectsEnginePHE.o \
	$(LB)/NModuleDetectorEffectsEngineBackgroundSims.o \
	$(LB)/NModuleDetectorCalibratorSciSim.o \
	$(LB)/NModuleDetectorCalibratorIdeal.o \
	$(LB)/NModuleDetectorCalibratorBackgroundSims.o \
	$(LB)/NGUIOptionsEventSelector.o \
	$(LB)/NModuleEventSelector.o \
	$(LB)/NModuleEventSelectorBackgroundSims.o \
	$(LB)/NGUIDiagnosticsBackprojector.o \
	$(LB)/NModuleBackprojector.o \
	$(LB)/NGUIOptionsLoader.o \
	$(LB)/NModuleEventLoader.o \
	$(LB)/NGUIOptionsSaver.o \
	$(LB)/NModuleEventSaver.o \
	$(LB)/NModuleEventSaverCompressedEventFormat.o \
	$(LB)/NGUIOptionsMetrologyEngine.o \
	$(LB)/NModuleMetrologyEngine.o \
	$(LB)/NModuleMetrologyEngineTrivial.o \
	$(LB)/NModuleMetrologyCalibratorTrivial.o \
	$(LB)/NModuleMetrologyLoader.o \
	$(LB)/NModuleMetrologySaver.o \
	$(LB)/NGUIOptionsStarTrackerEngineTrivial.o \
	$(LB)/NModuleStarTrackerEngineTrivial.o \
	$(LB)/NModuleStarTrackerCalibratorTrivial.o \
	$(LB)/NModuleStarTrackerLoader.o \
	$(LB)/NModuleStarTrackerSaver.o \
	$(LB)/NObservatoryReconstructor.o \
	$(LB)/NModuleObservatoryReconstructor.o \
	$(LB)/NModuleObservatoryMerger.o \
	$(LB)/NModuleObservatoryLoader.o \
	$(LB)/NModuleObservatorySaver.o \
	$(LB)/NModuleOrbitStationary.o \
	$(LB)/NGUIOptionsSinglePointing.o \
	$(LB)/NGUIOptionsPointingPredefined.o \
	$(LB)/NModulePointingPredefined.o \
	$(LB)/NModulePointingDatabase.o \
	$(LB)/NModuleOrientationsDatabase.o \
	$(LB)/NGUIOptionsOrientationsDatabase.o \
	$(LB)/NModuleTimeIdeal.o \
	$(LB)/NModuleGeometryAndDetectorProperties.o \
	$(LB)/NGUIDiagnostics.o \
	$(LB)/NGUIDiagnosticsMain.o \
	$(LB)/NGUIDiagnosticsTemplate.o \
	$(LB)/NGUIOptions.o \
	$(LB)/NGUIOptionsTemplate.o \


SOBJ = $(LB)/libModules.$(DLL)

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

$(LOBJS): $(LB)/%.o: src/%.cxx inc/%.h
	@echo "Compiling $(subst src/,,$<) ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@


#
#----------------------------------------------------------------



