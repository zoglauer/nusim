/*
 * NModuleDetectorEffectsEngineSciSim.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleDetectorEffectsEngineSciSim
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleDetectorEffectsEngineSciSim.h"

// Standard libs:
#include <cmath>
using namespace std;

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:
#include "MAssert.h"

// NuSTAR libs:
#include "NGUIOptions.h"
#include "NGUIDiagnosticsDetectorEffectsEngine.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleDetectorEffectsEngineSciSim)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorEffectsEngineSciSim::NModuleDetectorEffectsEngineSciSim(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleDetectorEffectsEngineSciSim

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "SciSim thread effects engine";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagDetectorEffectsEngineSciSim";

  // Set the tool tip text
  m_ToolTip = "This is the detector effects engine which belongs to the science simulator thread";

  // Set the module type
  m_ModuleType = c_DetectorEffectsEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = true;
  m_Diagnostics = 0;
}


////////////////////////////////////////////////////////////////////////////////


NModuleDetectorEffectsEngineSciSim::~NModuleDetectorEffectsEngineSciSim()
{
  // Delete this instance of NModuleDetectorEffectsEngineSciSim
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEngineSciSim::Initialize()
{
  // Initialize the module 

  if (gROOT->IsBatch() == false) {
    delete m_Diagnostics;
    m_Diagnostics = new NGUIDiagnosticsDetectorEffectsEngine();
    dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->SetDetectorParameters(m_Satellite.GetOrientationDetectorRelFocalPlaneModule(0, 1, 1).GetTranslation(), 
                                                                                              m_Satellite.GetDetectorHalfDimension(), 
                                                                                              m_Satellite.GetDetectorPixelsX(),
                                                                                              m_Satellite.GetDetectorPixelsY());
  }
  
  // Create the charge loss response
  m_ChargeLossFileName = "$(NUSIM)/resource/data/ChargeLossResponse.dat";
  MFile::ExpandFileName(m_ChargeLossFileName);
  m_ChargeLossInputEnergy = 122;
  if (GenerateChargeLossResponse() == false) {
    return false;
  }

  // Set up the energy resolution
  vector<double> X;
  X.push_back(0.0);
  X.push_back(30.0);
  X.push_back(60.0);
  X.push_back(80.0);
  vector<double> Y;
  Y.push_back(0.60/2.35);
  Y.push_back(0.60/2.35);
  Y.push_back(1.2/2.35);
  Y.push_back(1.6/2.35);
  m_EnergyResolution.Set(X, Y);

  m_LowTrigger = 50;
  m_HighTrigger = 5000;
  m_ChargeSharingDistance = 0.03;
  m_TriggerThreshold = 5;

  return true;
}


/******************************************************************************
 * Read an create the charge loss matrix
 */
bool NModuleDetectorEffectsEngineSciSim::GenerateChargeLossResponse()
{
  if (MFile::Exists(m_ChargeLossFileName) == false) {
    cout<<"ERROR: unable to find charge loss file \""<<m_ChargeLossFileName<<"\""<<endl;
    return false;
  }

  ifstream in;
  in.open(m_ChargeLossFileName);
  if (in.is_open() == false) {
    cout<<"ERROR: unable to open charge loss file \""<<m_ChargeLossFileName<<"\""<<endl;
    return false;
  }

  // Currently the file contains X rows of 6x6 energy matrices
  unsigned int XYCentralGrid = 5;

  m_ChargeLossXYGrid.clear();
  m_ChargeLossXYGrid.push_back(-0.250);
  m_ChargeLossXYGrid.push_back(-0.200);
  m_ChargeLossXYGrid.push_back(-0.150);
  m_ChargeLossXYGrid.push_back(-0.100);
  m_ChargeLossXYGrid.push_back(-0.050);
  m_ChargeLossXYGrid.push_back( 0.000);
  m_ChargeLossXYGrid.push_back( 0.050);
  m_ChargeLossXYGrid.push_back( 0.100);
  m_ChargeLossXYGrid.push_back( 0.150);
  m_ChargeLossXYGrid.push_back( 0.200);
  m_ChargeLossXYGrid.push_back( 0.250);

  m_ChargeLossZGrid.clear();
  for (int i = 10; i >= -10; --i) {
    m_ChargeLossZGrid.push_back(0.1*i);
  }

  unsigned int LineLength = 250;
  char* LineBuffer = new char[LineLength];

  MTokenizer T;
  T.AllowComposed(false);

  vector<vector<vector<double> > > ChargeLoss;

  // Prepare the first Grid:
  vector<double> xGridTemplate(m_ChargeLossXYGrid.size(), 0.0);
  vector<vector<double> > yGridTemplate;
  for (unsigned int i = 0; i < m_ChargeLossXYGrid.size(); ++i) {
    yGridTemplate.push_back(xGridTemplate);
  }

  unsigned int zGridID = 0;
  unsigned int yGridID = 5;
  ChargeLoss.push_back(yGridTemplate);
  while (in.getline(LineBuffer, LineLength, '\n')) {
    T.Analyze(LineBuffer);
    if (T.GetNTokens() == 0) {
      // Ignore
    } else if (T.GetNTokens() == 6) {
      for (unsigned int i = XYCentralGrid; i < XYCentralGrid+T.GetNTokens(); ++i) {
        ChargeLoss[zGridID][yGridID][i] = T.GetTokenAtAsDouble(i-XYCentralGrid);
        if (i > XYCentralGrid) {
          ChargeLoss[zGridID][yGridID][2*XYCentralGrid - i] = T.GetTokenAtAsDouble(i-XYCentralGrid);          
        }
      }
      if (yGridID > XYCentralGrid) {
        ChargeLoss[zGridID][2*XYCentralGrid - yGridID] = ChargeLoss[zGridID][yGridID];
      }

      // Prepare for next grid:
      yGridID++;
      //cout<<yGridID<<", "<<zGridID<<":"<<m_ChargeLossZGrid.size()<<endl;
      if (yGridID >= 11 && zGridID+1 < m_ChargeLossZGrid.size()) {
        //cout<<"--> "<<yGridID<<", "<<zGridID<<":"<<m_ChargeLossZGrid.size()<<endl;
        yGridID = 5;
        zGridID++;
        ChargeLoss.push_back(yGridTemplate);
      }
    } else {
      cout<<"ERROR: unable to parse charge loss file correctly: Number of tokens: "<<T.GetNTokens()<<endl;
      cout<<"Line: "<<LineBuffer<<endl;
      return false;    
    }
  }

  // Upgrade the table to differential loss
  for (unsigned int z = 0; z < ChargeLoss.size(); ++z) {
    for (unsigned int y = 0; y < ChargeLoss[z].size(); ++y) {
      for (unsigned int x = 0; x < ChargeLoss[z][y].size(); ++x) {
        ChargeLoss[z][y][x] /= m_ChargeLossInputEnergy;
      }
    }    
  }


//   // Sanity check -- dump table
//   for (unsigned int z = 0; z < ChargeLoss.size(); ++z) {
//     cout<<"Block "<<z<<": Depth = "<<m_ChargeLossZGrid[z]<<" cm"<<endl;
//     for (unsigned int y = 0; y < ChargeLoss[z].size(); ++y) {
//       for (unsigned int x = 0; x < ChargeLoss[z][y].size(); ++x) {
//         cout<<ChargeLoss[z][y][x]<<" ";
//       }
//       cout<<endl;
//     }    
//   }


  double Pitch = 2*m_Satellite.GetDetectorHalfDimension().X()/m_Satellite.GetDetectorPixelsX();

  // Now convert it to a response matrix 03:
  vector<float> xyAxis;
  xyAxis.push_back(-0.5*Pitch);
  for (unsigned int i = 0; i < m_ChargeLossXYGrid.size()-1; ++i) {
    xyAxis.push_back(0.5*(m_ChargeLossXYGrid[i]+m_ChargeLossXYGrid[i+1]));
  }
  xyAxis.push_back(0.5*Pitch);

  vector<float> zAxis;
  zAxis.push_back(m_ChargeLossZGrid.back());
  for (unsigned int i = m_ChargeLossZGrid.size()-1; i > 0; --i) {
    zAxis.push_back(0.5*(m_ChargeLossZGrid[i]+m_ChargeLossZGrid[i-1]));
  }
  zAxis.push_back(m_ChargeLossZGrid.front());

//   for (unsigned int i = 0; i < zAxis.size(); ++i) {
//     cout<<"z: "<<i<<":"<<zAxis[i]<<endl;
//   }

  m_ChargeLoss.Init();
  m_ChargeLoss.SetAxis(xyAxis, xyAxis, zAxis);
  for (unsigned int z = 0; z < ChargeLoss.size(); ++z) {
    for (unsigned int y = 0; y < ChargeLoss[z].size(); ++y) {
      for (unsigned int x = 0; x < ChargeLoss[z][y].size(); ++x) {
        m_ChargeLoss.SetBinContent(x, y, z, ChargeLoss[ChargeLoss.size()-z-1][y][x]);
        //m_ChargeLoss.SetBinContent(x, y, z, ChargeLoss[z][y][x]);
      }
    }    
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEngineSciSim::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  // Step 0: Create diagnostics window information
  for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
    if (Event.GetInteraction(i).IsDetector() == true && Event.GetTelescope() == 1) {
      // (a) Create the BEFORE info for the diagnostics window
      NOrientation O = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetInteraction(i).GetTelescope(), Event.GetInteraction(i).GetDetector());
      MVector Pos = Event.GetInteraction(i).GetPosition();
      O.TransformOut(Pos);
      if (gROOT->IsBatch() == false) {
        dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->AddBefore(Pos, Event.GetInteraction(i).GetEnergy());
      } 
    }
  }

  // All charge sharing, energy-loss, etc. is handled by the 9-pixel-RMF
  // It takes each interaction and generates NPixelHits in 3-by-3 square around
  // the energy deposit.
  // How this is handles interally in the 9-pixel-RMF is still TBD
 
  int NPixelsX = m_Satellite.GetDetectorPixelsX();
  int NPixelsY = m_Satellite.GetDetectorPixelsY();
 
  double IdealEnergy = 0.0;
  double NoisedEnergy = 0.0;
  
  int xPixel = 0;
  int yPixel = 0;
  
  vector<NPixelHit> PixelHits;
  
  // Step 1:
  // Translate into pixel hits
  // This is still a dummy since in reality the 9-pixel-RMF will be used.
  
  for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
    if (Event.GetInteraction(i).IsDetector() == true) {

      IdealEnergy = Event.GetInteraction(i).GetEnergy();
      NoisedEnergy = IdealEnergy*m_ChargeLoss.GetInterpolated(0.0, 0.0, Event.GetInteraction(i).GetPosition().Z(), true);
      NoisedEnergy = gRandom->Gaus(NoisedEnergy, m_EnergyResolution.Eval(NoisedEnergy));

      xPixel = int((Event.GetInteraction(i).GetPosition().X() + m_Satellite.GetDetectorHalfDimension().X()) / (2*m_Satellite.GetDetectorHalfDimension().X() / NPixelsX));
      yPixel = int((Event.GetInteraction(i).GetPosition().Y() + m_Satellite.GetDetectorHalfDimension().Y()) / (2*m_Satellite.GetDetectorHalfDimension().Y() / NPixelsY));

      massert(xPixel >= 0 && xPixel < NPixelsX);
      massert(yPixel >= 0 && yPixel < NPixelsY);

      // Create the central pixel hit
      NPixelHit P;
      P.SetTelescope(Event.GetInteraction(i).GetTelescope());
      P.SetDetector(Event.GetInteraction(i).GetDetector());
      P.SetXPixel(xPixel);
      P.SetYPixel(yPixel);

      P.SetPreTriggerSampleSum(0.0);
      P.SetPostTriggerSampleSum(NoisedEnergy);
      
      P.SetIdealAverageDepth(Event.GetInteraction(i).GetPosition().Z());
      P.SetIdealEnergy(IdealEnergy);
      
      P.SetNoisedAverageDepth(Event.GetInteraction(i).GetPosition().Z());
      P.SetNoisedEnergy(NoisedEnergy);
      
      PixelHits.push_back(P);
      
      // Create dummy pixel hits around central pixel hit
      
      for (int xp = xPixel-1; xp <= xPixel+1; ++xp) {
        for (int yp = yPixel-1; yp <= yPixel+1; ++yp) {
          if (xp == xPixel && yp == yPixel) continue;
          if (xp < 0 || xp >= NPixelsX) continue;
          if (yp < 0 || yp >= NPixelsY) continue;
          
          NPixelHit newP;
          newP.SetTelescope(Event.GetInteraction(i).GetTelescope());
          newP.SetDetector(Event.GetInteraction(i).GetDetector());
          newP.SetXPixel(xp);
          newP.SetYPixel(yp);

          newP.SetPreTriggerSampleSum(0.0);
          newP.SetPostTriggerSampleSum(0.0);
      
          // Zero values mean no trigger
          newP.SetIdealAverageDepth(0.0);
          newP.SetIdealEnergy(0.0);
      
          newP.SetNoisedAverageDepth(0.0);
          newP.SetNoisedEnergy(0.0);
          
          PixelHits.push_back(newP);
        }
      }
    } else {
      // Shield hits: 
      NShieldHit S;
      S.SetTelescope(Event.GetInteraction(i).GetTelescope());
      S.SetIdealEnergy(Event.GetInteraction(i).GetEnergy());
      Event.AddShieldHit(S);
    }
  }


  // Step 2:
  // Combine energy-deposits/pulse-heights in the same pixels:
  for (unsigned int p = 0; p < PixelHits.size(); ++p) {
    //cout<<p<<": "<<PixelHits[p].GetXPixel()<<":"<<PixelHits[p].GetYPixel()<<":"<<PixelHits[p].GetPostTriggerSampleSum()<<endl;
    // Check if a similar pixel hit is already in the event
    bool Merged = false;
    for (unsigned int e = 0; e < Event.GetNPixelHits(); ++e) {
      if (PixelHits[p].GetTelescope() == Event.GetPixelHit(e).GetTelescope() &&
        PixelHits[p].GetDetector() == Event.GetPixelHit(e).GetDetector() &&
        PixelHits[p].GetXPixel() == Event.GetPixelHit(e).GetXPixel() &&
        PixelHits[p].GetYPixel() == Event.GetPixelHit(e).GetYPixel()) {
        
        //cout<<"Duplicate pixel --- start merging: "<<endl;
        Event.GetPixelHitRef(e).AddPreTriggerSampleSum(PixelHits[p].GetPreTriggerSampleSum());
        Event.GetPixelHitRef(e).AddPostTriggerSampleSum(PixelHits[p].GetPostTriggerSampleSum());
        // Use energy weighted depths:
        if (Event.GetPixelHit(e).GetIdealEnergy() + PixelHits[p].GetIdealEnergy() > 0.0) {
          Event.GetPixelHitRef(e).SetIdealAverageDepth((Event.GetPixelHit(e).GetIdealAverageDepth()*Event.GetPixelHit(e).GetIdealEnergy() + PixelHits[p].GetIdealAverageDepth()*PixelHits[p].GetIdealEnergy())/(Event.GetPixelHit(e).GetIdealEnergy() + PixelHits[p].GetIdealEnergy()));
        }
        if (Event.GetPixelHit(e).GetNoisedEnergy() + PixelHits[p].GetNoisedEnergy() > 0.0) {
          Event.GetPixelHitRef(e).SetNoisedAverageDepth((Event.GetPixelHit(e).GetNoisedAverageDepth()*Event.GetPixelHit(e).GetNoisedEnergy() + PixelHits[p].GetNoisedAverageDepth()*PixelHits[p].GetNoisedEnergy())/(Event.GetPixelHit(e).GetNoisedEnergy() + PixelHits[p].GetNoisedEnergy()));
        }
        
        Event.GetPixelHitRef(e).SetIdealEnergy(Event.GetPixelHit(e).GetIdealEnergy() + PixelHits[p].GetIdealEnergy());
        Event.GetPixelHitRef(e).SetNoisedEnergy(Event.GetPixelHit(e).GetNoisedEnergy() + PixelHits[p].GetNoisedEnergy());
        Merged = true;
        break;
      }
    }
    if (Merged == false) {
     Event.AddPixelHit(PixelHits[p]); 
    }
  }



  // Step inf: Create the AFTER info for the diagnostics window
  for (unsigned int h = 0; h < Event.GetNPixelHits(); ++h) {
    //cout<<"N pixel hits: "<<Event.GetNPixelHits()<<endl;
    
    if (Event.GetTelescope() != 1) continue;
    
    NPixelHit& P = Event.GetPixelHitRef(h);
 
    if (P.GetNoisedEnergy() <= 0.0) continue;
    
    MVector Pos;
    Pos[0] = (0.5 + P.GetXPixel()) * (2*m_Satellite.GetDetectorHalfDimension().X()/NPixelsX) - m_Satellite.GetDetectorHalfDimension().X();
    Pos[1] = (0.5 + P.GetYPixel()) * (2*m_Satellite.GetDetectorHalfDimension().Y()/NPixelsY) - m_Satellite.GetDetectorHalfDimension().Y();
    Pos[2] = P.GetNoisedAverageDepth();
    if (Pos[2] > m_Satellite.GetDetectorHalfDimension().Z()) Pos[2] = 0.999*m_Satellite.GetDetectorHalfDimension().Z();
    if (Pos[2] < -m_Satellite.GetDetectorHalfDimension().Z()) Pos[2] = -0.999*m_Satellite.GetDetectorHalfDimension().Z();
    
    NOrientation O = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), P.GetTelescope(), P.GetDetector());
    O.TransformOut(Pos);
    if (gROOT->IsBatch() == false) {
      dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->AddAfter(Pos, P.GetPostTriggerSampleSum());
    } 
  }

  if (Event.GetNShieldHits() == 0 && Event.GetNPixelHits() == 0) {
    Event.SetBlocked(true);
  }


  /*

  // Charge sharing if hit is too close to edge
  vector<NInteraction> NewInteractions;
  for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
    if (Event.GetInteraction(i).IsDetector() == true) {
      if (m_Satellite.GetDistanceToClostestNeighborPixel(Event.GetInteraction(i).GetTelescope(), 
                                                         Event.GetInteraction(i).GetDetector(), 
                                                         Event.GetInteraction(i).GetPosition()) < m_ChargeSharingDistance) {
        NInteraction I = m_Satellite.CreateInteractionClostestNeighborPixel(Event.GetInteraction(i).GetTelescope(), 
                                                                            Event.GetInteraction(i).GetDetector(), 
                                                                            Event.GetInteraction(i).GetPosition());
        // Share energy 50%...
        double Energy = Event.GetInteraction(i).GetEnergy();
        Event.GetInteractionRef(i).SetEnergy(0.5*Energy);
        I.SetEnergy(0.5*Energy);
        I.SetDetector(Event.GetInteraction(i).GetDetector());
        I.SetTelescope(Event.GetInteraction(i).GetTelescope());
        I.SetIsDetector(true);

        NewInteractions.push_back(I);
      }
    }
  }

  // Test for duplicate interactions and merge
  for (unsigned int i = 0; i < NewInteractions.size(); ++i) {
    // Throw away charge sharing with imaginary pixels outside active detector region, add the rest
    if (fabs(NewInteractions[i].GetPosition().X()) <= m_Satellite.GetDetectorHalfDimension().X() &&
        fabs(NewInteractions[i].GetPosition().Y()) <= m_Satellite.GetDetectorHalfDimension().Y()) {
      Event.AddInteraction(NewInteractions[i]);
    } else {
      //cout<<"Out of bounds: "<<NewInteractions[i].ToString()<<endl;
    }
  }

  //for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
  //  cout<<Event.GetInteraction(i).ToString()<<endl;
  //}

  // Convert the interactions to pixel hits, i.e. pixel IDs and

  int NPixelsX = m_Satellite.GetDetectorPixelsX();
  int NPixelsY = m_Satellite.GetDetectorPixelsY();

  double Energy = 0.0;


  // Step 0: Check for veto:
  for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
    if (Event.GetInteraction(i).IsDetector() == false) {
      // Raise veto
      if (Event.GetInteraction(i).GetEnergy() > m_HighTrigger) {
        Event.SetVetoHigh();
      } else if (Event.GetInteraction(i).GetEnergy() > m_LowTrigger) {
        Event.SetVetoLow();        
      }
    }
  }


  // Step 1 : Create diagnostics window information
  for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
    if (Event.GetInteraction(i).IsDetector() == true) {
      // (a) Create the BEFORE info for the diagnostics window
      NOrientation O = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), Event.GetInteraction(i).GetTelescope(), Event.GetInteraction(i).GetDetector());
      MVector Pos = Event.GetInteraction(i).GetPosition();
      O.TransformOut(Pos);
      dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->AddBefore(Pos, Event.GetInteraction(i).GetEnergy());
    }
  }


  // Step 2: Combine pixels and apply depth dependent energy resolution:
  for (unsigned int i = 0; i < Event.GetNInteractions(); ++i) {
    if (Event.GetInteraction(i).IsDetector() == true) {
      
      // (b) Create the pixel hit(s)
      NPixelHit P;
      P.SetTelescope(Event.GetInteraction(i).GetTelescope());
      P.SetDetector(Event.GetInteraction(i).GetDetector());
      P.SetXPixel(int((Event.GetInteraction(i).GetPosition().X() + m_Satellite.GetDetectorHalfDimension().X()) / (2*m_Satellite.GetDetectorHalfDimension().X() / NPixelsX)));
      P.SetYPixel(int((Event.GetInteraction(i).GetPosition().Y() + m_Satellite.GetDetectorHalfDimension().Y()) / (2*m_Satellite.GetDetectorHalfDimension().Y() / NPixelsY)));
      P.SetIdealAverageDepth(Event.GetInteraction(i).GetPosition().Z());
      
      massert(P.GetXPixel() >= 0 && P.GetXPixel() < NPixelsX);
      massert(P.GetYPixel() >= 0 && P.GetYPixel() < NPixelsY);

      Energy = Event.GetInteraction(i).GetEnergy();
      Energy *= m_ChargeLoss.GetInterpolated(0.0, 0.0, Event.GetInteraction(i).GetPosition().Z(), true);

      P.SetADUnitsCathode(Energy);

      // Add it --- if there alreday exists a hit in the same pixel, just add it
      bool Found = false;
      for (unsigned int h = 0; h < Event.GetNPixelHits(); ++h) {
        NPixelHit& Hit = Event.GetPixelHitRef(h);
        if (Hit.GetTelescope() == P.GetTelescope() &&
            Hit.GetDetector() == P.GetDetector() &&
            Hit.GetXPixel() == P.GetXPixel() &&
            Hit.GetYPixel() == P.GetYPixel()) {
          // Energy weighted ideal depth
          Hit.SetIdealAverageDepth((Hit.GetADUnitsCathode()*Hit.GetIdealAverageDepth() + P.GetADUnitsCathode()*P.GetIdealAverageDepth())/(Hit.GetADUnitsCathode() + P.GetADUnitsCathode()));
          Hit.SetADUnitsCathode(Hit.GetADUnitsCathode() + P.GetADUnitsCathode());
          Hit.SetADUnitsAnode(Hit.GetADUnitsAnode() + P.GetADUnitsAnode());
          Found = true;
          break;
        }
      }
      if (Found == false) {
        Event.AddPixelHit(P);
      }
    }
  }
  
  // Step 3: Apply the electronics energy resolution and thresholds AFTER the hits have been combined into pixels:
  for (unsigned int h = 0; h < Event.GetNPixelHits(); ) { // auto for erase ++h) {
    NPixelHit& Hit = Event.GetPixelHitRef(h); 
    
       
    // Fake: encode the depth signal in the difference between anode and cathode signal
    double DepthScaler = (Hit.GetIdealAverageDepth() + m_Satellite.GetDetectorHalfDimension().Z())/ (2*m_Satellite.GetDetectorHalfDimension().Z());
    Hit.SetADUnitsAnode((0.5 + 0.5*DepthScaler) * (gRandom->Gaus(Hit.GetADUnitsCathode(), 4) + 8*(gRandom->Rndm()-0.5)));
    Hit.SetADUnitsCathode(gRandom->Gaus(Hit.GetADUnitsCathode(), m_EnergyResolution.Eval(Energy)));  

    // Remove the ideal depth information:
    Hit.SetIdealAverageDepth(g_DoubleNotDefined);

    // Throw away everything below the threshold of 5 keV...
    if (Hit.GetADUnitsCathode() < gRandom->Gaus(m_TriggerThreshold, 0.5)) {
      Event.RemovePixelHit(h);
    } else {
      ++h;
    }
  }


  // Step 4: Create the AFTER info for the diagnostics window
  for (unsigned int h = 0; h < Event.GetNPixelHits(); ++h) {
    NPixelHit& P = Event.GetPixelHitRef(h);
 
    MVector Pos;
    Pos[0] = (0.5 + P.GetXPixel()) * (2*m_Satellite.GetDetectorHalfDimension().X()/NPixelsX) - m_Satellite.GetDetectorHalfDimension().X();
    Pos[1] = (0.5 + P.GetYPixel()) * (2*m_Satellite.GetDetectorHalfDimension().Y()/NPixelsY) - m_Satellite.GetDetectorHalfDimension().Y();
    Pos[2] = m_Satellite.GetDetectorHalfDimension().Z()*(4*P.GetADUnitsAnode()/P.GetADUnitsCathode() - 3);
    if (Pos[2] > m_Satellite.GetDetectorHalfDimension().Z()) Pos[2] = 0.999*m_Satellite.GetDetectorHalfDimension().Z();
    if (Pos[2] < -m_Satellite.GetDetectorHalfDimension().Z()) Pos[2] = -0.999*m_Satellite.GetDetectorHalfDimension().Z();
    
    NOrientation O = m_Satellite.GetOrientationDetectorRelFocalPlaneModule(Event.GetTime(), P.GetTelescope(), P.GetDetector());
    O.TransformOut(Pos);
    dynamic_cast<NGUIDiagnosticsDetectorEffectsEngine*>(m_Diagnostics)->AddAfter(Pos, P.GetADUnitsCathode());
  }


  // If there are no hits & there is no veto, the event got blocked
  if (Event.GetVeto() == false && Event.GetNPixelHits() == 0) {
    Event.SetBlocked(true);
  }

*/

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleDetectorEffectsEngineSciSim::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptions* Options = new NGUIOptions(this);

  // with something like:
  // NGUIOptionsTemplate* Options = new NGUIOptionsTemplate(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleDetectorEffectsEngineSciSim::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  /*
  MXmlNode* SomeTagNode = Node->GetNode("SomeTag");
  if (SomeTagNode != 0) {
    m_SomeTagValue = SomeTagNode.GetValue();
  }
  */

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleDetectorEffectsEngineSciSim::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleDetectorEffectsEngineSciSim.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
