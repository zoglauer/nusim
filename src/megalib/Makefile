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
LOBJS = $(LB)/MAssert.o \
	$(LB)/MGlobal.o \
	$(LB)/MFile.o \
	$(LB)/MGUIDialog.o \
	$(LB)/MGUIElement.o \
	$(LB)/MGUIEEntry.o \
	$(LB)/MGUIEMinMaxEntry.o \
	$(LB)/MGUIEEntryList.o \
	$(LB)/MGUIEFileSelector.o \
	$(LB)/MGUIERBList.o \
	$(LB)/MGUIECBList.o \
	$(LB)/MGUIEText.o \
	$(LB)/MGUIProgressBar.o \
	$(LB)/MStreamBuffer.o \
	$(LB)/MStreams.o \
	$(LB)/MTime.o \
	$(LB)/MTimer.o \
	$(LB)/MSystem.o \
	$(LB)/MVector.o \
	$(LB)/MString.o \
	$(LB)/MFileManager.o \
	$(LB)/MTokenizer.o \
	$(LB)/MFunction.o \
	$(LB)/MFunction2D.o \
	$(LB)/MFunction3D.o \
	$(LB)/MFunction3DSpherical.o \
	$(LB)/MParser.o \
	$(LB)/MFileResponse.o \
	$(LB)/MResponseMatrix.o \
	$(LB)/MResponseMatrixO1.o \
	$(LB)/MResponseMatrixO2.o \
	$(LB)/MResponseMatrixO3.o \
	$(LB)/MXmlDocument.o \
	$(LB)/MXmlNode.o \
	$(LB)/MXmlData.o \
	$(LB)/MXmlAttribute.o \


SOBJ = $(LB)/libMegalib.$(DLL)

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



