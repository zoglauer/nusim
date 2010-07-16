/*
 * NModuleStarTrackerCalibratorTrivial.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleStarTrackerCalibratorTrivial__
#define __NModuleStarTrackerCalibratorTrivial__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceStarTracker.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleStarTrackerCalibratorTrivial : public NModule, public NModuleInterfaceStarTracker
{
  // public interface:
 public:
  //! Default constructor
  NModuleStarTrackerCalibratorTrivial(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleStarTrackerCalibratorTrivial();

  //! Initialize the module
  virtual bool Initialize();

  // Main data analysis routine, which updates the star tracker data
  virtual bool AnalyzeStarTrackerData(NStarTrackerData& Data);

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
  ClassDef(NModuleStarTrackerCalibratorTrivial, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
