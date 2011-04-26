/*
 * NModuleDetectorEffectsEngineSciSimCIE.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleDetectorEffectsEngineSciSimCIE__
#define __NModuleDetectorEffectsEngineSciSimCIE__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <string>
#include <vector>
using namespace std;

// ROOT libs:
#include "TH3.h"
#include "TFile.h"

// MEGAlib libs:
#include "MResponseMatrixO3.h"
#include "MFunction.h"

// NuSIM libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleDetectorEffectsEngineSciSimCIE : public NModule, public NModuleInterfaceEvent
{
  // public interface:
 public:
  //! Default constructor
  NModuleDetectorEffectsEngineSciSimCIE(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleDetectorEffectsEngineSciSimCIE();

  //! Get the file name of the charge induction efficiency (CIE) ROOT file
  TString GetCIEFileName() const { return m_ChargeInductionEfficiencyFileName; }
  //! Set the file name of the charge induction efficiency (CIE) ROOT file
  void SetCIEFileName(const TString CIEFileName)
  { m_ChargeInductionEfficiencyFileName = CIEFileName; }

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

  //! The charge induction efficiency file name
  TString m_ChargeInductionEfficiencyFileName;
  //! The charge induction efficiency root file
  TFile*  m_ChargeInductionEfficiencyROOTFile;
  //! The charge induction efficiency root histogram name
  TString m_ChargeInductionEfficiencyHistName;
  //! The charge induction efficiency root histogram
  TH3F*   m_ChargeInductionEfficiency;
  //! The scale factor in x-direction from pixel width in NuSim to the CIE histogram
  double  m_PixelWidthScaleFactorX;
  //! The scale factor in y-direction from pixel width in NuSim to the CIE histogram
  double  m_PixelWidthScaleFactorY;

  //! The correction factor for charge induction efficiency
  double m_CIECorrectionFactor;

  //! The energy resolution function
  MFunction m_EnergyResolution;

  //! The pixel center position in x-direction
  double *m_PixelCenterPositionX;
  //! The pixel center position in y-direction
  double *m_PixelCenterPositionY;

  //! Low veto trigger
  double m_LowTrigger;
  //! High veto trigger
  double m_HighTrigger;

  //! The charge sharing distance
  double m_ChargeSharingDistance;

  //! The trigger threshold
  double m_TriggerThreshold;

  // private members:
 private:

  //! Load the charge induction efficiency histogram
  bool LoadChargeInductionEfficiency();

  //! Interpolate values in 3-d histogram via trilinear interpolation
  double InterpolateTH3(TH3* Histogram, double& PosX, double& PosY, double& PosZ);

#ifdef ___CINT___
 public:
  ClassDef(NModuleDetectorEffectsEngineSciSimCIE, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
