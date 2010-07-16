/*
 * NModuleMetrologyCalibratorTrivial.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleMetrologyCalibratorTrivial__
#define __NModuleMetrologyCalibratorTrivial__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceMetrology.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleMetrologyCalibratorTrivial : public NModule, public NModuleInterfaceMetrology
{
  // public interface:
 public:
  //! Default constructor
  NModuleMetrologyCalibratorTrivial(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleMetrologyCalibratorTrivial();

  //! Initialize the module
  virtual bool Initialize();

  // Main data analysis routine, which updates the star tracker data
  virtual bool AnalyzeMetrologyData(NMetrologyData& Data);

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
  ClassDef(NModuleMetrologyCalibratorTrivial, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
