/*
 * NModuleOpticsEngineNone.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleOpticsEngineNone__
#define __NModuleOpticsEngineNone__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>





// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfacePhotonPropagation.h"


// Forward declarations:


////////////////////////////////////////////////////////////////////////////////



class NModuleOpticsEngineNone : public NModule, public NModuleInterfaceEvent, public NModuleInterfacePhotonPropagation
{
  // public interface:
 public:
  //! Default constructor
  NModuleOpticsEngineNone(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleOpticsEngineNone();

  //! Initialize the module
  virtual bool Initialize() { return true; }

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event) { return true; }

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
  ClassDef(NModuleOpticsEngineNone, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
