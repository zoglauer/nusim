/*
 * NModuleObservatoryLoader.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleObservatoryLoader__
#define __NModuleObservatoryLoader__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceObservatory.h"
#include "NModuleInterfaceEntry.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleObservatoryLoader : public NModule, public NModuleInterfaceObservatory, public NModuleInterfaceEntry
{
  // public interface:
 public:
  //! Default constructor
  NModuleObservatoryLoader(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleObservatoryLoader();

  //! Initialize the module
  virtual bool Initialize();

  // Main data analysis routine, which updates the metrology data
  virtual bool AnalyzeObservatoryData(NObservatoryData& Data);

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();


  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:




#ifdef ___CINT___
 public:
  ClassDef(NModuleObservatoryLoader, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
