/*
 * NModuleObservatorySaver.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleObservatorySaver__
#define __NModuleObservatorySaver__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceObservatory.h"
#include "NModuleInterfaceExit.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleObservatorySaver : public NModule, public NModuleInterfaceObservatory, public NModuleInterfaceExit
{
  // public interface:
 public:
  //! Default constructor
  NModuleObservatorySaver(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleObservatorySaver();

  //! Initialize the module
  virtual bool Initialize();

  // Main data analysis routine, which updates the star tracker data
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
  ClassDef(NModuleObservatorySaver, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
