/*
 * NModuleBackgroundSimulatorAperture.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleBackgroundSimulatorAperture
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleBackgroundSimulatorAperture.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:
#include "MTokenizer.h"

// NuSTAR libs:


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleBackgroundSimulatorAperture)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulatorAperture::NModuleBackgroundSimulatorAperture(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceEntry()
{
  // Construct an instance of NModuleBackgroundSimulatorAperture

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Sample from data base";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagBackgroundSimulatorAperture";

  // Set the tool tip text
  m_ToolTip = "This module samples the detector background from a events retrieved from a data base.";

  // Set all types this modules handles
  m_ModuleType = c_BackgroundEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsBackgroundSimulatorAperture)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsBackgroundSimulatorAperture();

  // Additinal initializations
  m_FileNameSpectrum = "$(NUSIM)/resource/data/Background.ApertureSpectrum.dat";
  m_FileNameDistribution = "$(NUSIM)/resource/data/Background.ApertureFieldOfView.dat";
}


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulatorAperture::~NModuleBackgroundSimulatorAperture()
{
  // Delete this instance of NModuleBackgroundSimulatorAperture
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorAperture::Initialize()
{
  // Initialize the module - since this is an entry module determine the time of the first event

  if (m_Spectrum.Set(m_FileNameSpectrum, "DP") == false) return false;
  if (m_Distribution.Set(m_FileNameDistribution, "GR") == false) return false;

  // Switch to mm2 in the spectrum:
  m_Spectrum.ScaleY(0.01);

  //m_Spectrum.Plot();
  //m_Distribution.Plot();

  // Determine the rate:
  // Spectrum: is in ph/cm2/s/keV/sr
  // Distribution: sr
  // --> Integrating over both spectra gives: ph/sec
  
  m_Rate = 0.0;

  double s_size = 0.5;
  double x_size = 1.0;
  double y_size = 1.0;
  for (double s = m_Spectrum.GetXMin(); s < m_Spectrum.GetXMax(); s += s_size) {
    for (double x = m_Distribution.GetXMin(); x < m_Distribution.GetXMax(); x += x_size) {
      for (double y = m_Distribution.GetYMin(); y < m_Distribution.GetYMax(); y += y_size) { 
        m_Rate += 
          m_Spectrum.Eval(s+0.5*s_size)*s_size * 
          m_Distribution.Eval(x+0.5*x_size, y+0.5*y_size) * x_size*y_size;
      }
    }
  }

  // Multiply by two since the above is just for one detector
  m_Rate *= 2;

  // (Random) time of next event
  m_Time = gRandom->Exp(1.0/m_Rate);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorAperture::AnalyzeEvent(NEvent& Event) 
{
  // From known background distributions (energy, position, generation probability)
  // generate interactions and add them to the event
  // This is an entry module: In a simulation starting from the beginning

  
  Event.SetOrigin(NEvent::c_OriginBackground);

  // Determine a random telescope
  Event.SetTelescope(gRandom->Integer(2)+1);

  Event.SetTime(m_Satellite.GetTimeIdeal());

  NInteraction I;
  I.SetIsDetector(true);
  I.SetTelescope(Event.GetTelescope());

  I.SetEnergy(m_Spectrum.GetRandom());

  double x = 0;
  double y = 0;
  m_Distribution.GetRandom(x, y);

  // Fake the depth by determining a length based on TOTAL (not just photo) absorption cross-sections
  double z = gRandom->Exp(1.0/m_Satellite.GetCZTAbsorptionCoefficient(I.GetEnergy()));


  // The position depends on the telescope
  MVector Position;
  if (Event.GetTelescope() == 1) {
    Position.SetXYZ(x, y, 1.0-z);
  } else {
    Position.SetXYZ(-x, y, 1.0-z);    
  }

  // Check in which detector we are in (could be simplified, but that's safer this way)
  NOrientation DetectorOrientation;
  int Detector = 0;
  for (unsigned int d = 1; d <= 4; ++d) {
    DetectorOrientation = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetTelescope(), d);
    DetectorOrientation.TransformIn(Position);
    if (fabs(Position.X()) <= m_Satellite.GetDetectorHalfDimension().X() &&
        fabs(Position.Y()) <= m_Satellite.GetDetectorHalfDimension().Y() &&
        fabs(Position.Z()) <= m_Satellite.GetDetectorHalfDimension().Z()) {
      Detector = d;
      break;
    } else {
      DetectorOrientation.TransformOut(Position);
    }
  }

  if (Detector == 0) {
    // We did not hit the detector and can stop here
    Event.SetBlocked(true);
    return true;
  }
  I.SetDetector(Detector);
  I.SetPosition(Position);


  Event.AddInteraction(I);

  // Determine time of next event
  m_Time += gRandom->Exp(1.0/m_Rate);


  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleBackgroundSimulatorAperture::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorAperture::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleBackgroundSimulatorAperture::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);

  return Node;
}


// NModuleBackgroundSimulatorAperture.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
