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
  //m_Diagnostics = new MGUIDiognosticsMetrologyEngine();


  m_UpdateInterval = 0.01;
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

  return true;
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
  
  double MD1Err=m_Satellite.GetCentroidingErrorMetrologyDetector(1);
  double MD2Err=m_Satellite.GetCentroidingErrorMetrologyDetector(2);
  double Err;

  // ... and do something with them to generate to generate all three initial metrology data sets as measured

  NPhoton Laser;  // laser in ML coordinate frame
  MVector MDplane(0.0, 0.0, 0.0); // Plane in MD coordinate system
  MVector MDplaneNormal(0.0, 0.0, 1.0);
    
  NMetrologyDataSet Set1;
  Laser.SetPosition(MVector(0.0, 0.0, 0.0)); // need to get right displacement (this will have thermal motion)
  Laser.SetDirection(MVector(0.0, 0.0, -1.0)); // always points down -z (this will have thermal motion)
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
  Laser.SetPosition(MVector(0.0, 0.0, 0.0));
  Laser.SetDirection(MVector(0.0, 0.0, -1.0));
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

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleMetrologyEngine::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);  
  new MXmlNode(Node, "UpdateInterval", m_UpdateInterval);


  return Node;
}


// NModuleMetrologyEngine.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
