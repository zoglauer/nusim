/*
 * NModuleTimeEngine.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleTimeEngine__
#define __NModuleTimeEngine__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR:
#include "NTime.h"
#include "NModule.h"
#include "NModuleInterfaceTime.h"
#include "NOrbit.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Satellite module providing all time information.
//! This module is part of the satellite super-module and provides all relevant times,
//! the ideal time, the time the satellite bus measures, the time detector 1 and 2 measure 

class NModuleTimeEngine : public NModule, public NModuleInterfaceTime
{
  // public interface:
 public:
  //! Default constructor
  NModuleTimeEngine(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleTimeEngine();

  //! Initialize the module
  virtual bool Initialize();
  

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
  ClassDef(NModuleTimeEngine, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
