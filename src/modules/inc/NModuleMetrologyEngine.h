/*
 * NModuleMetrologyEngine.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleMetrologyEngine__
#define __NModuleMetrologyEngine__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSIM libs:
#include "NGlobal.h"
#include "NMetrologyDetectorShift.h"
#include "NModule.h"
#include "NModuleInterfaceEntry.h"
#include "NModuleInterfaceMetrology.h"
#include "NModuleInterfacePhotonPropagation.h"
#include "NModuleInterfaceIO.h"
#include "NModuleInterfaceMetrologySaverLevel1Fits.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleMetrologyEngine : public NModule, public NModuleInterfaceMetrology, 
                               public NModuleInterfaceEntry,
                               public NModuleInterfacePhotonPropagation,
                               public NModuleInterfaceIO,
                               public NModuleInterfaceMetrologySaverLevel1Fits
{
  // public interface:
 public:
  //! Default constructor
  NModuleMetrologyEngine(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleMetrologyEngine();

  //! Initialize the module
  virtual bool Initialize();

  //! Main data analysis routine, which updates the metrology data
  virtual bool AnalyzeMetrologyData(NMetrologyData& Data);

  //! Finalize the module
  virtual bool Finalize();

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();

  //! Set the latitude
  void SetUpdateInterval(double UpdateInterval) { m_UpdateInterval = UpdateInterval; }
  //! Get the latitude
  double GetUpdateInterval() const { return m_UpdateInterval; }

  //! Enable the blur
  void EnableBlur(bool BlurEnabled = true) { m_BlurEnabled = BlurEnabled; }
  //! Enable the blur
  bool IsBlurEnabled() const { return m_BlurEnabled; }

  //! Enable saving the file as fits
  void SetSaveAsFits(bool SaveAsFits = true) { m_SaveAsFits = SaveAsFits; }
  //! Enable saving the file as fits
  bool GetSaveAsFits() const { return m_SaveAsFits; }

  //! Set the detector shifts
  void SetPositionShiftFileName(TString PositionShiftFileName) { m_PositionShiftFileName = PositionShiftFileName; }
  //! Get the detector shifts
  TString GetPositionShiftFileName() const { return m_PositionShiftFileName; }

  //! Show the detector shift histogram
  void ShowDetectorShifts();

  // protected methods:
 protected:
  //! Interpolate a shift for the given position from the data set
  MVector InterpolateShift(const MVector& Position);

  // private methods:
 private:



  // protected members:
 protected:
  double m_UpdateInterval;


  // private members:
 private:
  //! The name of the file containing the position shifts 
  TString m_PositionShiftFileName;
  //! Vector holding the position shift
  vector<NMetrologyDetectorShift> m_MetrologyDetectorShifts;

  //! Flag indicating whether of not to use the blur
  bool m_BlurEnabled;

  //! Flag indicating whether of not to save the data
  bool m_SaveAsFits;

  
#ifdef ___CINT___
 public:
  ClassDef(NModuleMetrologyEngine, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
