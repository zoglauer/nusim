/*
 * NModuleOrbitStationary.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleOrbitStationary__
#define __NModuleOrbitStationary__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR:
#include "NModule.h"
#include "NModuleInterfaceOrbit.h"
#include "NOrbit.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


// Choose one:
class NModuleOrbitStationary : public NModule, public NModuleInterfaceOrbit
{
  // public interface:
 public:
  //! Default constructor
  NModuleOrbitStationary(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleOrbitStationary();

  //! Initialize the module
  virtual bool Initialize();

  //! Return the orbit position at a given time
  virtual NOrbit GetOrbit(NTime Time);
  

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
  ClassDef(NModuleOrbitStationaryPosition, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
