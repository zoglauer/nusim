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

  //! Get the file name of the calibrated metrology system data base
  TString GetCalibratedMetrologyDBFileName() const { return m_CalibratedMetrologyDBFileName; }
  //! Set the file name of the calibrated metrology system data base
  void SetCalibratedMetrologyDBFileName(const TString CalibratedMetrologyDBFileName) { m_CalibratedMetrologyDBFileName = CalibratedMetrologyDBFileName; }

  //! Get the file name of the perturbed metrology system data base
  TString GetPerturbedMetrologyDBFileName() const { return m_PerturbedMetrologyDBFileName; }
  //! Set the file name of the perturbed metrology system data base
  void SetPerturbedMetrologyDBFileName(const TString PerturbedMetrologyDBFileName) { m_PerturbedMetrologyDBFileName = PerturbedMetrologyDBFileName; }

  //! Get the file name of the calibrated optics system data base
  TString GetCalibratedOpticsDBFileName() const { return m_CalibratedOpticsDBFileName; }
  //! Set the file name of the calibrated optics system data base
  void SetCalibratedOpticsDBFileName(const TString CalibratedOpticsDBFileName) { m_CalibratedOpticsDBFileName = CalibratedOpticsDBFileName; }

  //! Get the file name of the perturbed optics system data base
  TString GetPerturbedOpticsDBFileName() const { return m_PerturbedOpticsDBFileName; }
  //! Set the file name of the perturbed optics system data base
  void SetPerturbedOpticsDBFileName(const TString PerturbedOpticsDBFileName) { m_PerturbedOpticsDBFileName = PerturbedOpticsDBFileName; }

  //! Get the file name of the calibrated alignments data base
  virtual TString GetCalibratedAlignmentsDBFileName() const { return m_CalibratedAlignmentsDBFileName; }
  //! Set the file name of the calibrated alignments data base
  void SetCalibratedAlignmentsDBFileName(const TString CalibratedAlignmentsDBFileName) { m_CalibratedAlignmentsDBFileName = CalibratedAlignmentsDBFileName; }

  //! Get the file name of the pertubations data base
  virtual TString GetPerturbedAlignmentsDBFileName() const { return m_PerturbedAlignmentsDBFileName; }
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
   
  //! Read the calibrated metrology data base
  bool ReadCalibratedMetrologyDB(TString FileName);
  //! Read the perturbed metrology data base
  bool ReadPerturbedMetrologyDB(TString FileName);
  //! Read the calibrated optics data base
  bool ReadCalibratedOpticsDB(TString FileName);
  //! Read the perturbed optics data base
  bool ReadPerturbedOpticsDB(TString FileName);
  //! Read the calibrated alignments data base
  bool ReadCalibratedAlignmentsDB(TString FileName);
  //! Read the calibrated alignments data base
  bool ReadPerturbedAlignmentsDB(TString FileName);

  //! Find the line-delimeter of the file and rewind file afterwards!
  char FindDelimeter(ifstream& in);
  
  // private methods:
 private:



  // protected members:
 protected:

   // private members:
 private:
  //! File name of the metrology DB
  TString m_CalibratedMetrologyDBFileName;
  //! File name of the metrology DB
  TString m_PerturbedMetrologyDBFileName;
  //! File name of the optics DB
  TString m_CalibratedOpticsDBFileName;
  //! File name of the optics DB
  TString m_PerturbedOpticsDBFileName;
  //! File name of the calibrations DB
  TString m_CalibratedAlignmentsDBFileName;
  //! File name of the pertubations DB
  TString m_PerturbedAlignmentsDBFileName;

  //! Start your search for the perturbed alignments at this index...
  unsigned int m_StartIndexPerturbedAlignments;
  //! Time wrap for the perturbed alignments time index
  NTime m_TimeWrapPerturbedAlignments;
  //! The pertubed alignments in space as a function of time...
  vector<NAlignmentsDBEntry> m_PerturbedAlignments;   


  //! Start your search for the perturbed alignments at this index...
  unsigned int m_StartIndexPerturbedMetrologyUncertainties;
  //! Time wrap for the perturbed alignments time index
  NTime m_TimeWrapPerturbedMetrologyUncertainties;   
  //! The metrology uncertainties as a function of time
  vector<NMetrologyDBEntry> m_PerturbedMetrologyUncertainties;

  
  //! Start your search for the perturbed alignments at this index...
  unsigned int m_StartIndexPerturbedOpticsUncertainties;
  //! Time wrap for the perturbed alignments time index
  NTime m_TimeWrapPerturbedOpticsUncertainties;    
  //! The optics uncertainties as a function of time
  vector<NOpticsDBEntry> m_PerturbedOpticsUncertainties;

  
  
#ifdef ___CINT___
 public:
  ClassDef(NModuleOrientationsDatabase, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
