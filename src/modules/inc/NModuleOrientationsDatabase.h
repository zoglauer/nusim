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
class NModuleOrientationsDatabase : public NModule, public NModuleInterfaceOrientations
{
  // public interface:
 public:
  //! Default constructor
  NModuleOrientationsDatabase(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleOrientationsDatabase();

  //! Initialize the module
  virtual bool Initialize();

  //! Get the file name of the metrology system error data base
  TString GetMetrologyDBFileName() const { return m_MetrologyDBFileName; }
  //! Set the file name of the metrology system error data base
  void SetMetrologyDBFileName(const TString MetrologyDBFileName) { m_MetrologyDBFileName = MetrologyDBFileName; }

  //! Get the file name of the optics system error data base
  TString GetOpticsDBFileName() const { return m_OpticsDBFileName; }
  //! Set the file name of the optics system error data base
  void SetOpticsDBFileName(const TString OpticsDBFileName) { m_OpticsDBFileName = OpticsDBFileName; }

  //! Get the file name of the calibrated alignments data base
  TString GetCalibratedAlignmentsDBFileName() const { return m_CalibratedAlignmentsDBFileName; }
  //! Set the file name of the calibrated alignments data base
  void SetCalibratedAlignmentsDBFileName(const TString CalibratedAlignmentsDBFileName) { m_CalibratedAlignmentsDBFileName = CalibratedAlignmentsDBFileName; }

  //! Get the file name of the pertubations data base
  TString GetPerturbedAlignmentsDBFileName() const { return m_PerturbedAlignmentsDBFileName; }
  //! Set the file name of the pertubations data base
  void SetPerturbedAlignmentsDBFileName(const TString PerturbedAlignmentsDBFileName) { m_PerturbedAlignmentsDBFileName = PerturbedAlignmentsDBFileName; }

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();


  // protected methods:
 protected:
  //! Determine orientations if they are not yet present for the given time t
  //! This function needs to be derived and is empty!
  virtual void AdvanceTime(const NTime& t);
   
  //! Read the metrology data base
  bool ReadMetrologyDB(TString FileName);
  //! Read the optics data base
  bool ReadOpticsDB(TString FileName);
  //! Read the calibrated alignments data base
  bool ReadCalibratedAlignmentsDB(TString FileName);
  //! Read the calibrated alignments data base
  bool ReadPerturbedAlignmentsDB(TString FileName);

  // private methods:
 private:



  // protected members:
 protected:

   // private members:
 private:
  //! File name of the metrology DB
  TString m_MetrologyDBFileName;
  //! File name of the optics DB
  TString m_OpticsDBFileName;
  //! File name of the calibrations DB
  TString m_CalibratedAlignmentsDBFileName;
  //! File name of the pertubations DB
  TString m_PerturbedAlignmentsDBFileName;

  //! Start your search for the perturbed alignments at this index...
  unsigned int m_StartIndexPerturbedAlignments;
  //! Time wrap for the perturbed alignments time index
  NTime m_TimeWrapPerturbedAlignments;
  //! The pertubed alignments in space as a function of time...
  vector<NAlignments> m_PerturbedAlignments;   


  //! Start your search for the perturbed alignments at this index...
  unsigned int m_StartIndexMetrologyUncertainties;
  //! Time wrap for the perturbed alignments time index
  NTime m_TimeWrapMetrologyUncertainties;   
  //! The metrology uncertainties as a function of time
  vector<NMetrologyUncertainties> m_MetrologyUncertainties;

  
  //! Start your search for the perturbed alignments at this index...
  unsigned int m_StartIndexOpticsUncertainties;
  //! Time wrap for the perturbed alignments time index
  NTime m_TimeWrapOpticsUncertainties;    
  //! The optics uncertainties as a function of time
  vector<NOpticsUncertainties> m_OpticsUncertainties;

  
  
#ifdef ___CINT___
 public:
  ClassDef(NModuleOrientationsDatabase, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
