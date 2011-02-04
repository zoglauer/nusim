/*
 * NModuleMetrologyEngine.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleMetrologyEngine
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleMetrologyEngine.h"

// Standard libs:
#include "math.h"

// ROOT libs:
#include "TGClient.h"
#include "TCanvas.h"
#include "TH2.h"

// MEGAlib libs:
#include "TRandom.h"

// NuSTAR libs:
#include "NModule.h"
#include "NMetrologyData.h"
#include "NGUIOptionsMetrologyEngine.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleMetrologyEngine)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleMetrologyEngine::NModuleMetrologyEngine(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceMetrology(), NModuleInterfaceEntry()
{
  // Construct an instance of NModuleMetrologyEngine

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "NuSTAR default";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagMetrologyEngine";

  // Set the tool tip text
  m_ToolTip = "This is the default NuSTAR metrology engine.";

  // Set all types this modules handles
  m_ModuleType = c_MetrologyEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsMetrologyEngine)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiagnosticsMetrologyEngine();


  m_UpdateInterval = 0.01;
  m_BlurEnabled = true;
  
  m_PositionShiftFileName = "$(NUSIM)/resource/data/metcal_pert_002.dat";
}


////////////////////////////////////////////////////////////////////////////////


NModuleMetrologyEngine::~NModuleMetrologyEngine()
{
  // Delete this instance of NModuleMetrologyEngine
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologyEngine::Initialize()
{
  // Initialize the module 

  // We start before the actual time
  m_Time = -5*m_UpdateInterval;

  // Load the shift data from file
  MFile::ExpandFileName(m_PositionShiftFileName);
 
  if (MFile::FileExists(m_PositionShiftFileName) == false) {
    mgui<<"Metrology engine:\nPosition shift file not found: \""<<m_PositionShiftFileName<<"\""<<show;
    return false;
  }
  
  ifstream in;
  in.open(m_PositionShiftFileName);
  if (in.is_open() == false) {
    mgui<<"Metrology engine:\nUnable to open file: \""<<m_PositionShiftFileName<<"\""<<show;
    return false;
  }

  // Let's read until we find the first delimeter, then rewind
  char Delimeter = '\n'; //FindDelimeter(in);

  // Read the header
  TString Line;

  Line.ReadToDelim(in, Delimeter); // description

  
  // Read the data
  int LineCounter = 0;
  bool Found = false;
  while (!in.eof()) {
    Line.ReadToDelim(in, Delimeter);
    
    ++LineCounter;
    if (Line.Length() > 0) {
      
      NMetrologyDetectorShift M;
      if (M.ParseDB(Line) == false) {
        mgui<<"Metrology engine:\nParsing failed: Something is wrong with your metrology detector shifts"<<show;
        in.close();
        return false;            
      }
      m_MetrologyDetectorShifts.push_back(M);
      Found = true;
    }
  } 
  in.close();

  if (Found == false) {
    mgui<<"Metrology engine:\nNo entries found in your metrology detector shift data base"<<show;
    return false;
  }

  ShowDetectorShifts();
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


MVector NModuleMetrologyEngine::InterpolateShift(const MVector& Position)
{
  //! Interpolate a shift for the given position from the data set

  MVector Interpolation;

  // Find the clostest position in all 4 quadrants;
  double TestDistance = 10000.0;
  
  MVector pXpY(+TestDistance, +TestDistance, 0.0);
  MVector pXpY_Shift;
  double pXpY_Radial = pXpY.Mag();
  bool pXpY_Found = false;
  unsigned int pXpY_Index = 0;
  
  MVector pXmY(+TestDistance, -TestDistance, 0.0);
  MVector pXmY_Shift;
  double pXmY_Radial = pXmY.Mag();
  bool pXmY_Found = false;
  unsigned int pXmY_Index = 0;

  MVector mXpY(-TestDistance, +TestDistance, 0.0);
  MVector mXpY_Shift;
  double mXpY_Radial = mXpY.Mag();
  bool mXpY_Found = false;
  unsigned int mXpY_Index = 0;

  MVector mXmY(-TestDistance, -TestDistance, 0.0);
  MVector mXmY_Shift;
  double mXmY_Radial = mXmY.Mag();
  bool mXmY_Found = false;
  unsigned int mXmY_Index = 0;

  MVector PositionOffset;
  for (unsigned int s = 0; s < m_MetrologyDetectorShifts.size(); ++s) {
    PositionOffset = m_MetrologyDetectorShifts[s].GetPosition() - Position;
    if (PositionOffset[0] >= 0.0) {
      if (PositionOffset[1] >= 0.0) {
        if (PositionOffset.Mag() < pXpY_Radial) {
          pXpY_Radial = PositionOffset.Mag();
          pXpY = PositionOffset;
          pXpY_Found = true;
          pXpY_Index = s;
        }
      } else {
        if (PositionOffset.Mag() < pXmY_Radial) {
          pXmY_Radial = PositionOffset.Mag();
          pXmY = PositionOffset;
          pXmY_Found = true;
          pXmY_Index = s;
        }
      }
    } else {
      if (PositionOffset[1] >= 0.0) {
        if (PositionOffset.Mag() < mXpY_Radial) {
          mXpY_Radial = PositionOffset.Mag();
          mXpY = PositionOffset;
          mXpY_Found = true;
          mXpY_Index = s;
        }
      } else {
        if (PositionOffset.Mag() < mXmY_Radial) {
          mXmY_Radial = PositionOffset.Mag();
          mXmY = PositionOffset;
          mXmY_Found = true;
          mXmY_Index = s;
        }
      }
    }
  }
  
  if (pXpY_Found == false || pXmY_Found == false || mXpY_Found == false || mXmY_Found == false) {
    //cout<<"Error: Metrology detector position "<<Position<<" is outside calibrated area!"<<endl;
    //cout<<"Using shift of closest position!!"<<endl;
    double Distance = numeric_limits<double>::max();
    unsigned int Index = 0;
    if (pXpY_Found == true && pXpY_Radial < Distance) {
      Distance = pXpY_Radial;
      Index = pXpY_Index;
    }
    if (pXmY_Found == true && pXmY_Radial < Distance) {
      Distance = pXmY_Radial;
      Index = pXmY_Index;
    }
    if (mXpY_Found == true && mXpY_Radial < Distance) {
      Distance = mXpY_Radial;
      Index = mXpY_Index;
    }
    if (mXmY_Found == true && mXmY_Radial < Distance) {
      Distance = mXmY_Radial;
      Index = mXmY_Index;
    }
    return m_MetrologyDetectorShifts[Index].GetShift();
  }

  // Go back to the real thing (instead of offsets)
  pXpY = m_MetrologyDetectorShifts[pXpY_Index].GetPosition();
  pXmY = m_MetrologyDetectorShifts[pXmY_Index].GetPosition();
  mXpY = m_MetrologyDetectorShifts[mXpY_Index].GetPosition();
  mXmY = m_MetrologyDetectorShifts[mXmY_Index].GetPosition();

  pXpY_Shift = m_MetrologyDetectorShifts[pXpY_Index].GetShift();
  pXmY_Shift = m_MetrologyDetectorShifts[pXmY_Index].GetShift();
  mXpY_Shift = m_MetrologyDetectorShifts[mXpY_Index].GetShift();
  mXmY_Shift = m_MetrologyDetectorShifts[mXmY_Index].GetShift();

  //cout<<"Surrounding positions for "<<Position<<": "<<pXpY<<":"<<pXmY<<":"<<mXpY<<":"<<mXmY<<endl;
  //cout<<"Surrounding shifts for "<<Position<<": "<<pXpY_Shift<<":"<<pXmY_Shift<<":"<<mXpY_Shift<<":"<<mXmY_Shift<<endl;

  // Interpolate (twice):

  // S(x) = m*x + t
  // m = (S(x2)-S(x1))/(x2-x1)
  // t = S(x2) - m*x2
  // y0 = (y2-y1)/(x2-x1) * (x0-x1) + y1

  double m, t;

  // x-shift interpolation:
 
  // x shift at pY
  m = (pXpY_Shift.X() - mXpY_Shift.X())/(pXpY.X()-mXpY.X());
  t = mXpY_Shift.X() - m*mXpY.X();
  double xShift_pY = m*Position.X() + t;
  MVector pY(Position.X(), (pXpY.Y() - mXpY.Y())/(pXpY.X()-mXpY.X()) * (Position.X() - mXpY.X()) + mXpY.Y(), 0.0);
 
  //cout<<m<<":"<<t<<":"<<xShift_pY<<":"<<pY<<endl;
 
  // x shift at mY
  m = (pXmY_Shift.X()-mXmY_Shift.X())/(pXmY.X()-mXmY.X());
  t = mXmY_Shift.X() - m*mXmY.X();
  double xShift_mY = m*Position.X() + t;
  MVector mY(Position.X(), (pXmY.Y() - mXmY.Y())/(pXmY.X()-mXmY.X()) * (Position.X() - mXmY.X()) + mXmY.Y(), 0.0);

  //cout<<m<<":"<<t<<":"<<xShift_mY<<":"<<mY<<endl;
 
  // x shift at position
  
  m = (xShift_pY - xShift_mY)/(pY.Y() - mY.Y());
  t = xShift_mY - m*mY.Y();
  double xShift = m*Position.Y() + t;
  
  //cout<<m<<":"<<t<<":"<<xShift<<endl;


  // y-shift interpolation
   
  // y shift at pY
  m = (pXpY_Shift.Y() - mXpY_Shift.Y())/(pXpY.X()-mXpY.X());
  t = mXpY_Shift.Y() - m*mXpY.X();
  double yShift_pY = m*Position.X() + t;
  //MVector pY(Position.X(), (pXpY.Y() - mXpY.Y())/(pXpY.X()-mXpY.X()) * (Position.X() - mXpY.X()) + mXpY.Y(), 0.0);
 
  //cout<<m<<":"<<t<<":"<<xShift_pY<<":"<<pY<<endl;
 
  // y shift at mY
  m = (pXmY_Shift.Y()-mXmY_Shift.Y())/(pXmY.X()-mXmY.X());
  t = mXmY_Shift.Y() - m*mXmY.X();
  double yShift_mY = m*Position.X() + t;
  //MVector mY(Position.X(), (pXmY.Y() - mXmY.Y())/(pXmY.X()-mXmY.X()) * (Position.X() - mXmY.X()) + mXmY.Y(), 0.0);

  //cout<<m<<":"<<t<<":"<<xShift_mY<<":"<<mY<<endl;
 
  // y shift at position
  
  m = (yShift_pY - yShift_mY)/(pY.Y() - mY.Y());
  t = yShift_mY - m*mY.Y();
  double yShift = m*Position.Y() + t;



  return MVector(xShift, yShift, 0.0);
}


////////////////////////////////////////////////////////////////////////////////


void NModuleMetrologyEngine::ShowDetectorShifts()
{
  //! Show the detector shift histogram -- that's just for diagnosing the interpolation algorithm

  int Bins = 50;
  double xMin = -10.15;
  double xMax = 10.15;
  double yMin = -10.15;
  double yMax = 10.15;


  TH2D* XShift = new TH2D("XShift", "Interpolated x-shifts", Bins, xMin, xMax, Bins, yMin, yMax);
  TH2D* YShift = new TH2D("YShift", "Interpolated y-shifts", Bins, xMin, xMax, Bins, yMin, yMax);
  
  MVector Position, Shift;
  for (int bx = 1; bx <= XShift->GetXaxis()->GetNbins(); ++bx) {
    for (int by = 1; by <= XShift->GetYaxis()->GetNbins(); ++by) {
      Position.SetXYZ(XShift->GetXaxis()->GetBinCenter(bx), XShift->GetYaxis()->GetBinCenter(by), 0.0);
      Shift = InterpolateShift(Position);
      
      XShift->SetBinContent(bx, by, Shift[0]);
      YShift->SetBinContent(bx, by, Shift[1]);
    }
  }
 
  TH2D* XShiftMeasured = new TH2D("XShiftMeasured", "Measured x-shifts (att: bins without value are also colored)", Bins, xMin, xMax, Bins, yMin, yMax);
  TH2D* YShiftMeasured = new TH2D("YShiftMeasured", "Measured y-Shifts (att: bins without value are also colored)", Bins, xMin, xMax, Bins, yMin, yMax);

  for (unsigned int i = 0; i < m_MetrologyDetectorShifts.size(); ++i) {
    XShiftMeasured->Fill(m_MetrologyDetectorShifts[i].GetPosition().X(), m_MetrologyDetectorShifts[i].GetPosition().Y(), m_MetrologyDetectorShifts[i].GetShift().X());
    YShiftMeasured->Fill(m_MetrologyDetectorShifts[i].GetPosition().X(), m_MetrologyDetectorShifts[i].GetPosition().Y(), m_MetrologyDetectorShifts[i].GetShift().Y());
  }
    
  TCanvas* ShiftCanvas = new TCanvas("ShiftCanvas", "Shifts...", 800, 800);
  ShiftCanvas->Divide(2, 2);
  ShiftCanvas->cd(1);
  XShift->Draw("colz");
  ShiftCanvas->cd(2);
  YShift->Draw("colz");
  ShiftCanvas->cd(3);
  XShiftMeasured->Draw("colz");
  ShiftCanvas->cd(4);
  YShiftMeasured->Draw("colz");
  ShiftCanvas->Update();
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologyEngine::AnalyzeMetrologyData(NMetrologyData& Data) 
{
  // Main data analysis routine, which updates the metrology data to a new level 

  // We probably need to retrieve the current alignments from the satellite module

  NOrientation OML1 = m_Satellite.GetOrientationMetrologyLaser(m_Time, 1);
  NOrientation OML2 = m_Satellite.GetOrientationMetrologyLaser(m_Time, 2);

  NOrientation OMD1 = m_Satellite.GetOrientationMetrologyDetector(m_Time, 1);
  NOrientation OMD2 = m_Satellite.GetOrientationMetrologyDetector(m_Time, 2);
  NOrientation OB = m_Satellite.GetOrientationOpticalBench(m_Time);
  //cout<< m_Time <<endl;
  //cout << "OB =" << OB.ToString() << endl;
  //cout << "OMD1 =" << OMD1.ToString() << m_Time << endl;
  //cout << "OML1 =" << OML1.ToString() << endl;
  
  double MD1Err = m_Satellite.GetCentroidingErrorMetrologyDetector(m_Time, 1);
  double MD2Err = m_Satellite.GetCentroidingErrorMetrologyDetector(m_Time, 2);
  double Err;

  // ... and do something with them to generate to generate all three initial metrology data sets as measured

  NPhoton Laser;  // laser in ML coordinate frame
  MVector MDplane(0.0, 0.0, 0.0); // Plane in MD coordinate system
  MVector MDplaneNormal(0.0, 0.0, 1.0);
    
  NMetrologyDataSet Set1;
  Laser.SetPosition(m_Satellite.GetOriginMetrologyLaserRelML(m_Time,1));
  Laser.SetDirection(m_Satellite.GetPointingMetrologyLaserRelML(m_Time,1)); 
  OML1.TransformOut(Laser); // Transforms into global system
  //cout<<"1 "<<Laser.GetPosition()<<Laser.GetDirection()<<endl;
  OMD1.TransformIn(Laser); // Transforms from global to MD system
  //cout<<"2 "<<Laser.GetPosition()<<Laser.GetDirection()<<endl;
  PropagateToPlane(Laser, MDplane, MDplaneNormal); // where laser intersects MD plane
  MVector P1 = Laser.GetPosition(); // get new laser intersection in the MD coordinates
  //cout<<"3 "<<Laser.GetPosition()<<Laser.GetDirection()<<endl;
  Set1.SetOriginalLaserHit(P1); // updates the metrology set1 module with coordinates
  Set1.SetTime(m_Time); 
  Err = gRandom->Gaus(0,MD1Err);
  P1[0] = P1[0]-Err;
  Err = gRandom->Gaus(0,MD1Err);
  P1[1] = P1[1]-Err;
  Set1.SetCalibratedLaserHit(P1); 
  
  NMetrologyDataSet Set2;
  Laser.SetPosition(m_Satellite.GetOriginMetrologyLaserRelML(m_Time,2));
  Laser.SetDirection(m_Satellite.GetPointingMetrologyLaserRelML(m_Time,2));
  OML2.TransformOut(Laser);
  OMD2.TransformIn(Laser);
  PropagateToPlane(Laser, MDplane, MDplaneNormal);
  MVector P2 = Laser.GetPosition();
  Set2.SetOriginalLaserHit(P2);
  Set2.SetTime(m_Time);
  Err = gRandom->Gaus(0,MD2Err);
  P2[0] = P2[0]-Err;
  Err = gRandom->Gaus(0,MD2Err);
  P2[1] = P2[1]-Err;
  Set2.SetCalibratedLaserHit(P2); 
  // Done!

  // Finally update all data sets
  Data.SetMetrologyDataSet1(Set1);
  Data.SetMetrologyDataSet2(Set2);

  m_Time += m_UpdateInterval;

  //cout<<"ME-Time: "<<m_Time<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleMetrologyEngine::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  NGUIOptionsMetrologyEngine* Options = new NGUIOptionsMetrologyEngine(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologyEngine::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* UpdateIntervalNode = Node->GetNode("UpdateInterval");
  if (UpdateIntervalNode != 0) {
    m_UpdateInterval = UpdateIntervalNode->GetValueAsDouble();
  }
  MXmlNode* PositionShiftFileNameNode = Node->GetNode("PositionShiftFileName");
  if (PositionShiftFileNameNode != 0) {
    m_PositionShiftFileName = PositionShiftFileNameNode->GetValueAsString();
  }
  MXmlNode* BlurEnabledNode = Node->GetNode("BlurEnabled");
  if (BlurEnabledNode != 0) {
    m_BlurEnabled = BlurEnabledNode->GetValueAsBoolean();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleMetrologyEngine::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);  
  new MXmlNode(Node, "UpdateInterval", m_UpdateInterval);
  new MXmlNode(Node, "PositionShiftFileName", m_PositionShiftFileName);
  new MXmlNode(Node, "BlurEnabled", m_BlurEnabled);

  return Node;
}


// NModuleMetrologyEngine.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
