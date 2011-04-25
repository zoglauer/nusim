/*
 * NGUIOptionsSource.cxx
 *
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsSource
//
//
// This class is an elementary GUI-widget:
//
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsSource.h"

// Standard libs:

// ROOT libs:
#include <TGFont.h>
#include <TGResourcePool.h>

// MEGAlib libs:


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsSource)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsSource::NGUIOptionsSource(NSource* Source, const TGWindow* Parent) :
  TGCompositeFrame(Parent, 100, 100, kChildFrame), m_Source(Source)
{
  // Standard constructor

  c_BeamComboBox = 1;
  c_SpectralComboBox = 2;

  m_BeamOptionsSubFrame = 0;
  m_P1 = 0;
  m_P2 = 0;
  m_P3 = 0;
  m_P4 = 0;
  m_P5 = 0;
  m_P6 = 0;
  m_P7 = 0;
  m_PF = 0;
  m_SpectralOptionsSubFrame = 0;
  m_E1 = 0;
  m_E2 = 0;
  m_E3 = 0;
  m_E4 = 0;
  m_E5 = 0;
  m_EF = 0;
  m_EFunction = 0;
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsSource::~NGUIOptionsSource()
{
  // Destruct this instance of NGUIOptionsSource
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsSource::Create()
{
  // Create the label and the input-field.

  TGLayoutHints* Default = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsExpandX, 0, 0, 0, 0);
  TGLayoutHints* LayoutL1 = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsExpandX, 20, 20, 10, 10);
  TGLayoutHints* LayoutL1b = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsCenterX | kLHintsExpandX, 100, 20, 0, 10);
  TGLayoutHints* LayoutL2 = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY, 0, 20, 0, 10);
  TGLayoutHints* LayoutL2b = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsExpandX, 0, 0, 0, 10);

  m_Name = new MGUIEEntry(this, "Name: ", false, m_Source->GetName());
  m_Name->SetEntryFieldSize(200);
  AddFrame(m_Name, LayoutL1);



  TGCompositeFrame* BeamTypeFrame = new TGCompositeFrame(this, 100, 100, kHorizontalFrame);
  AddFrame(BeamTypeFrame, LayoutL1);

  TGLabel* BeamTypeLabel = new TGLabel(BeamTypeFrame, "Beam type:");
  BeamTypeLabel->SetWidth(120);
  BeamTypeFrame->AddFrame(BeamTypeLabel, LayoutL2);

  m_BeamTypes = new TGComboBox(BeamTypeFrame, c_BeamComboBox);
  for (int b = NSource::c_FirstBeamType; b <= NSource::c_LastBeamType; ++b) {
    m_BeamTypes->AddEntry(NSource::GetBeamTypeName(b), b);
  }
  m_BeamTypes->Associate(this);
  m_BeamTypes->SetHeight(20);
  m_BeamTypes->Select(m_Source->GetBeamType());
  BeamTypeFrame->AddFrame(m_BeamTypes, LayoutL2b);

  m_BeamOptionsFrame = new TGGroupFrame(this, "Beam options");
  AddFrame(m_BeamOptionsFrame, LayoutL1b);

  m_BeamOptionsSubFrame = new TGCompositeFrame(m_BeamOptionsFrame);
  m_BeamOptionsFrame->AddFrame(m_BeamOptionsSubFrame, Default);

  TGCompositeFrame* SpectralTypeFrame = new TGCompositeFrame(this, 100, 100, kHorizontalFrame);
  AddFrame(SpectralTypeFrame, LayoutL1);

  TGLabel* SpectralTypeLabel = new TGLabel(SpectralTypeFrame, "Spectral type:");
  SpectralTypeLabel->SetWidth(120);
  SpectralTypeFrame->AddFrame(SpectralTypeLabel, LayoutL2);

  m_SpectralTypes = new TGComboBox(SpectralTypeFrame, c_SpectralComboBox);
  for (int b = NSource::c_FirstSpectralType; b <= NSource::c_LastSpectralType; ++b) {
    m_SpectralTypes->AddEntry(NSource::GetSpectralTypeName(b), b);
  }
  m_SpectralTypes->Associate(this);
  m_SpectralTypes->SetHeight(20);
  m_SpectralTypes->Select(m_Source->GetSpectralType());
  SpectralTypeFrame->AddFrame(m_SpectralTypes, LayoutL2b);

  m_SpectralOptionsFrame = new TGGroupFrame(this, "Spectral options");
  AddFrame(m_SpectralOptionsFrame, LayoutL1b);

  m_SpectralOptionsSubFrame = new TGCompositeFrame(m_SpectralOptionsFrame);
  m_SpectralOptionsFrame->AddFrame(m_SpectralOptionsSubFrame, Default);

  m_Flux = new MGUIEEntry(this, "Flux [TBD]: ", false, m_Source->GetFlux());
  m_Flux->SetEntryFieldSize(200);
  AddFrame(m_Flux, LayoutL1);


  UpdateOptions();

  Resize(400, 500); 

  return;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsSource::ProcessMessage(long Message, long Parameter1, 
                                long Parameter2)
{
  // Process the messages for this application, mainly the scollbar moves:

  switch (GET_MSG(Message)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(Message)) {
    case kCM_CHECKBUTTON:
      break;
    case kCM_COMBOBOX:
      UpdateOptions();
      break;
    case kCM_BUTTON:
    case kCM_MENU:
      switch (Parameter1) {
      default:
        break;
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }

  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsSource::UpdateOptions()
{
  //UpdateSource();

  TGLayoutHints* Default = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsExpandX, 0, 0, 0, 0);
  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 0, 0, 0);
  
  if (m_BeamOptionsSubFrame != 0) {
    TGFrameElement* E;
    TIter Next(m_BeamOptionsSubFrame->GetList());
    while ((E = (TGFrameElement *) Next())) {
      m_BeamOptionsSubFrame->HideFrame(E->fFrame);
      m_BeamOptionsSubFrame->RemoveFrame(E->fFrame);
      delete E->fFrame;
    } 
    m_P1 = 0;
    m_P2 = 0;
    m_P3 = 0;
    m_P4 = 0;
    m_P5 = 0;
    m_P6 = 0;
    m_P7 = 0;
    m_PF = 0;
  }
  

  if (m_BeamTypes->GetSelected() == NSource::c_NearFieldPoint || 
      m_BeamTypes->GetSelected() == NSource::c_NearFieldRestrictedPoint) {
    m_P1 = new MGUIEEntry(m_BeamOptionsSubFrame, "Position X [mm]: ", false, m_Source->GetPositionParameter1());
    m_BeamOptionsSubFrame->AddFrame(m_P1, Default);
    m_P2 = new MGUIEEntry(m_BeamOptionsSubFrame, "Position Y [mm]: ", false, m_Source->GetPositionParameter2());
    m_BeamOptionsSubFrame->AddFrame(m_P2, Default);
    m_P3 = new MGUIEEntry(m_BeamOptionsSubFrame, "Position Z [mm]: ", false, m_Source->GetPositionParameter3());
    m_BeamOptionsSubFrame->AddFrame(m_P3, Default);
    m_Flux->SetLabel("Flux [ph/s]:");
    m_Flux->SetValue(m_Source->GetFlux());
  } else if (m_BeamTypes->GetSelected() == NSource::c_NearFieldBeam) {
    m_P1 = new MGUIEEntry(m_BeamOptionsSubFrame, "Center of start disk X [mm]: ", false, m_Source->GetPositionParameter1());
    m_BeamOptionsSubFrame->AddFrame(m_P1, Default);
    m_P2 = new MGUIEEntry(m_BeamOptionsSubFrame, "Center of start disk Y [mm]: ", false, m_Source->GetPositionParameter2());
    m_BeamOptionsSubFrame->AddFrame(m_P2, Default);
    m_P3 = new MGUIEEntry(m_BeamOptionsSubFrame, "Center of start disk Z [mm]: ", false, m_Source->GetPositionParameter3());
    m_BeamOptionsSubFrame->AddFrame(m_P3, Default);
    m_P4 = new MGUIEEntry(m_BeamOptionsSubFrame, "Start direction X: ", false, m_Source->GetPositionParameter4(), true, -1.0, 1.0);
    m_BeamOptionsSubFrame->AddFrame(m_P4, Default);
    m_P5 = new MGUIEEntry(m_BeamOptionsSubFrame, "Start direction Y: ", false, m_Source->GetPositionParameter5(), true, -1.0, 1.0);
    m_BeamOptionsSubFrame->AddFrame(m_P5, Default);
    m_P6 = new MGUIEEntry(m_BeamOptionsSubFrame, "Start direction Z: ", false, m_Source->GetPositionParameter6(), true, -1.0, 1.0);
    m_BeamOptionsSubFrame->AddFrame(m_P6, Default);
    m_P7 = new MGUIEEntry(m_BeamOptionsSubFrame, "Radius [mm]: ", false, m_Source->GetPositionParameter7(), true, 0.0000001);
    m_BeamOptionsSubFrame->AddFrame(m_P7, Default);
    m_Flux->SetLabel("Flux [ph/s]:");
    m_Flux->SetValue(m_Source->GetFlux());
  } else if (m_BeamTypes->GetSelected() == NSource::c_FarFieldPoint) {
    m_P1 = new MGUIEEntry(m_BeamOptionsSubFrame, "Declination [deg]: ", false, m_Source->GetPositionParameter1()/60.0, true, -90.0, 90.0);
    m_BeamOptionsSubFrame->AddFrame(m_P1, Default);
    m_P2 = new MGUIEEntry(m_BeamOptionsSubFrame, "Right ascension [deg]: ", false, m_Source->GetPositionParameter2()/60.0, true, 0.0, 360.0);
    m_BeamOptionsSubFrame->AddFrame(m_P2, Default);
    m_Flux->SetLabel("Flux [ph/s/cm2]:");
    m_Flux->SetValue(m_Source->GetFlux()*100); // go from the internal ph/s/mm2 to ph/s/cm2
  } else if (m_BeamTypes->GetSelected() == NSource::c_FarFieldDisk) {
    m_P1 = new MGUIEEntry(m_BeamOptionsSubFrame, "Declination [deg]: ", false, m_Source->GetPositionParameter1()/60.0, true, -90.0, 90.0);
    m_BeamOptionsSubFrame->AddFrame(m_P1, Default);
    m_P2 = new MGUIEEntry(m_BeamOptionsSubFrame, "Right ascension [deg]: ", false, m_Source->GetPositionParameter2()/60.0, true, 0.0, 360.0);
    m_BeamOptionsSubFrame->AddFrame(m_P2, Default);
    m_P3 = new MGUIEEntry(m_BeamOptionsSubFrame, "Extent [deg]: ", false, m_Source->GetPositionParameter3()/60.0, true, 0.00000001, 180.0);
    m_BeamOptionsSubFrame->AddFrame(m_P3, Default);
    m_Flux->SetLabel("Flux [ph/s/cm2]:");
    m_Flux->SetValue(m_Source->GetFlux()*100); // go from the internal ph/s/mm2 to ph/s/cm2
  } else if (m_BeamTypes->GetSelected() == NSource::c_FarFieldFitsFile) {
    m_PF = new MGUIEFileSelector(m_BeamOptionsSubFrame, "Choose a FITS file:", 
                                 m_Source->GetPositionFileName());
    m_PF->SetFileType("FITS file", "*.fits");
    m_BeamOptionsSubFrame->AddFrame(m_PF, Default);
    m_Flux->SetLabel("Flux [ph/s/cm2]:");
    m_Flux->SetValue(m_Source->GetFlux()*100); // go from the internal ph/s/mm2 to ph/s/cm2
  } else if (m_BeamTypes->GetSelected() == NSource::c_FarFieldNormalizedEnergyPositionFluxFunction) {
    m_PF = new MGUIEFileSelector(m_BeamOptionsSubFrame, "Choose a 3D-dat file:", 
                                 m_Source->GetNormalizedEnergyPositionFluxFileName());
    m_PF->SetFileType("3D dat", "*.3Ddat");
    m_BeamOptionsSubFrame->AddFrame(m_PF, Default);
    m_Flux->SetLabel("Flux [ph/s/cm2]:");
    m_Flux->SetValue(0.0);
  }

  
  if (m_BeamTypes->GetSelected() == NSource::c_FarFieldNormalizedEnergyPositionFluxFunction) {
    m_SpectralTypes->Select(NSource::c_NormalizedEnergyPositionFluxFunction);
    m_SpectralTypes->SetEnabled(false);
  } else {
    if (m_SpectralTypes->GetSelected() == NSource::c_NormalizedEnergyPositionFluxFunction) {
      m_SpectralTypes->Select(NSource::c_Monoenergetic);
    }
    m_SpectralTypes->SetEnabled(true);
  }


  if (m_SpectralOptionsSubFrame != 0) {
    TGFrameElement* E;
    TIter Next(m_SpectralOptionsSubFrame->GetList());
    while ((E = (TGFrameElement *) Next())) {
      m_SpectralOptionsSubFrame->HideFrame(E->fFrame);
      m_SpectralOptionsSubFrame->RemoveFrame(E->fFrame);
      delete E->fFrame;
    } 
    m_E1 = 0;
    m_E2 = 0;
    m_E3 = 0;
    m_E4 = 0;
    m_E5 = 0;
    m_EF = 0;
    m_EFunction = 0;
  }
  
  if (m_SpectralTypes->GetSelected() == NSource::c_Monoenergetic) {
    m_E1 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Energy [keV]: ", false, m_Source->GetEnergyParameter1(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E1, Default);
  } else if (m_SpectralTypes->GetSelected() == NSource::c_Linear) {
    m_E1 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Energy min [keV]: ", false, m_Source->GetEnergyParameter1(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E1, Default);
    m_E2 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Energy max [keV]: ", false, m_Source->GetEnergyParameter2(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E2, Default);
  } else if (m_SpectralTypes->GetSelected() == NSource::c_PowerLaw) {
    m_E1 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Energy min [keV]: ", false, m_Source->GetEnergyParameter1(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E1, Default);
    m_E2 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Energy max [keV]: ", false, m_Source->GetEnergyParameter2(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E2, Default);
    m_E3 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Photon index: ", false, m_Source->GetEnergyParameter3());
    m_SpectralOptionsSubFrame->AddFrame(m_E3, Default);
  } else if (m_SpectralTypes->GetSelected() == NSource::c_BrokenPowerLaw) {
    m_E1 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Energy min [keV]: ", false, m_Source->GetEnergyParameter1(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E1, Default);
    m_E2 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Energy max [keV]: ", false, m_Source->GetEnergyParameter2(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E2, Default);
    m_E3 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Energy break [keV]: ", false, m_Source->GetEnergyParameter3(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E3, Default);
    m_E4 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Photon index min: ", false, m_Source->GetEnergyParameter4());
    m_SpectralOptionsSubFrame->AddFrame(m_E4, Default);
    m_E5 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Photon index max: ", false, m_Source->GetEnergyParameter5());
    m_SpectralOptionsSubFrame->AddFrame(m_E5, Default);
  } else if (m_SpectralTypes->GetSelected() == NSource::c_BlackBody) {
    m_E1 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Energy min [keV]: ", false, m_Source->GetEnergyParameter1(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E1, Default);
    m_E2 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Energy max [keV]: ", false, m_Source->GetEnergyParameter2(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E2, Default);
    m_E3 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Temperature [keV]: ", false, m_Source->GetEnergyParameter3(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E3, Default);
  } else if (m_SpectralTypes->GetSelected() == NSource::c_NormalizedFunctionInPhPerCm2PerSPerKeV) {
    m_E1 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Energy min [keV]: ", false, m_Source->GetEnergyParameter1(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E1, Default);
    m_E2 = new MGUIEEntry(m_SpectralOptionsSubFrame, "Energy max [keV]: ", false, m_Source->GetEnergyParameter2(), true, 0.0);
    m_SpectralOptionsSubFrame->AddFrame(m_E2, Default);
    TGLabel* Label1 = new TGLabel(m_SpectralOptionsSubFrame, "Enter a normalized (ph/cm2/s/keV) spectrum, such as:");
    m_SpectralOptionsSubFrame->AddFrame(Label1, LabelLayout);
    TGLabel* Label2 = new TGLabel(m_SpectralOptionsSubFrame, "7.0e-6*pow(x/10, -1)*exp(-sqrt(x/2.2))");
    m_SpectralOptionsSubFrame->AddFrame(Label2, LabelLayout);
    //TGLabel* Label3 = new TGLabel(m_SpectralOptionsSubFrame, "Pay attention to the use of \"e\" and \"x\"!");
    //m_SpectralOptionsSubFrame->AddFrame(Label3, LabelLayout);
    m_EFunction = new TGTextEntry(m_SpectralOptionsSubFrame, m_Source->GetEnergyFunctionString());
    m_SpectralOptionsSubFrame->AddFrame(m_EFunction, Default);
  } else if (m_SpectralTypes->GetSelected() == NSource::c_FileDifferentialFlux) {
    m_EF = new MGUIEFileSelector(m_SpectralOptionsSubFrame, "Choose a file containing the spectrum:", 
                                 m_Source->GetEnergyFileName());
    m_EF->SetFileType("Spectrum", "*.spe");
    m_SpectralOptionsSubFrame->AddFrame(m_EF, Default);
  } else if (m_SpectralTypes->GetSelected() == NSource::c_NormalizedEnergyPositionFluxFunction) {
    TGLabel* EnergyLabel = new TGLabel(m_SpectralOptionsSubFrame, "All options included in beam!");
    m_SpectralOptionsSubFrame->AddFrame(EnergyLabel, Default);
  }

  if (m_SpectralTypes->GetSelected() == NSource::c_NormalizedFunctionInPhPerCm2PerSPerKeV ||
      m_SpectralTypes->GetSelected() == NSource::c_NormalizedEnergyPositionFluxFunction) {
    m_Flux->SetEnabled(false);
    m_Flux->SetValue(0.0);
  } else {
    m_Flux->SetEnabled(true);
  }

  MapSubwindows();
  MapWindow();  
  Layout();
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsSource::UpdateSource()
{
  //! Update the source data

  m_Source->SetName(m_Name->GetAsString());

  m_Source->SetBeamType(m_BeamTypes->GetSelected());
  double P1 = 0, P2 = 0, P3 = 0, P4 = 0, P5 = 0, P6 = 0, P7 = 0;
  if (m_P1 != 0) {
    P1 = m_P1->GetAsDouble();
    if (m_BeamTypes->GetSelected() == NSource::c_FarFieldPoint ||
        m_BeamTypes->GetSelected() == NSource::c_FarFieldDisk) {
      P1 *= 60;  // deg --> arcmin
    }
  }
  if (m_P2 != 0)  {
    P2 = m_P2->GetAsDouble();
    if (m_BeamTypes->GetSelected() == NSource::c_FarFieldPoint ||
        m_BeamTypes->GetSelected() == NSource::c_FarFieldDisk) {
      P2 *= 60;  // deg --> arcmin
    }
  } 
  if (m_P3 != 0)  {
    P3 = m_P3->GetAsDouble();
    if (m_BeamTypes->GetSelected() == NSource::c_FarFieldDisk) {
      P3 *= 60;  // deg --> arcmin
    }
  }
  if (m_P4 != 0) P4 = m_P4->GetAsDouble();
  if (m_P5 != 0) P5 = m_P5->GetAsDouble();
  if (m_P6 != 0) P6 = m_P6->GetAsDouble();
  if (m_P7 != 0) P7 = m_P7->GetAsDouble();
  m_Source->SetPosition(P1, P2, P3, P4, P5, P6, P7);
  if (m_PF != 0) {
    if (m_BeamTypes->GetSelected() == NSource::c_FarFieldNormalizedEnergyPositionFluxFunction) {
      m_Source->SetNormalizedEnergyPositionFluxFunction(m_PF->GetFileName());    
    } else {
      m_Source->SetPosition(m_PF->GetFileName());
    }
  }

  // Set the flux before the energy in order that the energy can update the flux
  if (m_BeamTypes->GetSelected() == NSource::c_NearFieldPoint ||
      m_BeamTypes->GetSelected() == NSource::c_NearFieldRestrictedPoint ||
      m_BeamTypes->GetSelected() == NSource::c_NearFieldBeam) {
    m_Source->SetFlux(m_Flux->GetAsDouble());
  } else if (m_BeamTypes->GetSelected() == NSource::c_FarFieldPoint ||
     m_BeamTypes->GetSelected() == NSource::c_FarFieldDisk ||
     m_BeamTypes->GetSelected() == NSource::c_FarFieldFitsFile) {
    m_Source->SetFlux(m_Flux->GetAsDouble()/100); // go from the external ph/s/cm2 to the external ph/s/mm2
  } else if (m_BeamTypes->GetSelected() == NSource::c_FarFieldNormalizedEnergyPositionFluxFunction) {
    // <-- Flux has been set during beam setting!!
  } else {
    cerr<<"NGUIOptionsSource::UpdateSource: Unknown beam type ("<<m_BeamTypes->GetSelected()<<")... You forgot to implement it..."<<endl;
  }


  m_Source->SetSpectralType(m_SpectralTypes->GetSelected());
  double E1 = 0, E2 = 0, E3 = 0, E4 = 0, E5 = 0;
  if (m_E1 != 0) E1 = m_E1->GetAsDouble(); 
  if (m_E2 != 0) E2 = m_E2->GetAsDouble(); 
  if (m_E3 != 0) E3 = m_E3->GetAsDouble(); 
  if (m_E4 != 0) E4 = m_E4->GetAsDouble(); 
  if (m_E5 != 0) E5 = m_E5->GetAsDouble(); 
  m_Source->SetEnergy(E1, E2, E3, E4, E5);
  if (m_EF != 0) m_Source->SetEnergy(m_EF->GetFileName());
  if (m_EFunction != 0) m_Source->SetEnergyFunctionString(m_EFunction->GetText());
  
}


// NGUIOptionsSource.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
