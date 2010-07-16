/*
 * NModuleStarTrackerLoader.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleStarTrackerLoader__
#define __NModuleStarTrackerLoader__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceStarTracker.h"
#include "NModuleInterfaceEntry.h"
#include "NModuleInterfaceIO.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleStarTrackerLoader : public NModule, public NModuleInterfaceStarTracker, public NModuleInterfaceEntry, public NModuleInterfaceIO
{
  // public interface:
 public:
  //! Default constructor
  NModuleStarTrackerLoader(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleStarTrackerLoader();

  //! Initialize the module
  virtual bool Initialize();

  // Main data analysis routine, which updates the metrology data
  virtual bool AnalyzeStarTrackerData(NStarTrackerData& Data);

  //! Finalize the module
  virtual bool Finalize();

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
  //! The input stream
  ifstream m_In;

  //! We buffer one data set, since we must know the next  time
  NStarTrackerData m_StarTrackerData;




#ifdef ___CINT___
 public:
  ClassDef(NModuleStarTrackerLoader, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
