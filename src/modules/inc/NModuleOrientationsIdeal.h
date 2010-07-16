/*
 * NModuleOrientationsIdeal.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleOrientationsIdeal__
#define __NModuleOrientationsIdeal__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR:
#include "NModule.h"
#include "NModuleInterfaceOrientations.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


// Choose one:
class NModuleOrientationsIdeal : public NModule, public NModuleInterfaceOrientations
{
  // public interface:
 public:
  //! Default constructor
  NModuleOrientationsIdeal(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleOrientationsIdeal();

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
  //! Loads the default values
  virtual bool LoadDefaultValues();
  //! Determine orientations if they are not yet present for the given time t
  virtual void DetermineOrientations(const NTime& t);
  

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NModuleOrientationsIdeal, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
