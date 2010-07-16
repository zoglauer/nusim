/*
 * NModuleOrientationsDatabase.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleOrientationsDatabase__
#define __NModuleOrientationsDatabase__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR:
#include "NModuleOrientationsIdeal.h"
#include "NModuleInterfaceOrientations.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


// Choose one:
class NModuleOrientationsDatabase : public NModuleOrientationsIdeal
{
  // public interface:
 public:
  //! Default constructor
  NModuleOrientationsDatabase(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleOrientationsDatabase();

  //! Initialize the module
  virtual bool Initialize();

  //! Get the file name of the orientations data base
  TString GetFileName() const { return m_FileName; }
  //! Set the file name of the orientations data base
  void SetFileName(const TString FileName) { m_FileName = FileName; }

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();


  // protected methods:
 protected:
  //! Determine orientations if they are not yet present for the given time t
  virtual void DetermineOrientations(const NTime& t);
  

  // private methods:
 private:



  // protected members:
 protected:
  //! Start your search for the orientation at this index...
  unsigned int m_StartIndex;
  //! Time wrap for the orientation time index
  NTime m_TimeWrap;

  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NModuleOrientationsDatabase, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
