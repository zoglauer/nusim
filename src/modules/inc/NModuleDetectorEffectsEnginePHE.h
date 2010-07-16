/*
 * NModuleDetectorEffectsEnginePHE.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleDetectorEffectsEnginePHE__
#define __NModuleDetectorEffectsEnginePHE__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MResponseMatrixO3.h"
#include "MFunction.h"

// NuSIM libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NPHEFitsData.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleDetectorEffectsEnginePHE : public NModule, public NModuleInterfaceEvent
{
  // public interface:
 public:
  //! Default constructor
  NModuleDetectorEffectsEnginePHE(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleDetectorEffectsEnginePHE();
  
  //! Get the file name of the PHE fites file
  TString GetPHEFileName() const { return m_PHEFileName; }
  //! Set the file name of the PHE fits file
  void SetPHEFileName(const TString PHEFileName) { m_PHEFileName = PHEFileName; }
  
  //! Initialize the module
  virtual bool Initialize();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);

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
  //! The name of the PHE file
  TString m_PHEFileName;
  //! The PHE data sets
  vector<NPHEFitsData> m_PHEs;

#ifdef ___CINT___
 public:
  ClassDef(NModuleDetectorEffectsEnginePHE, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
