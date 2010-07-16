/*
 * NModuleMetrologyEngineTrivial.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleMetrologyEngineTrivial
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleMetrologyEngineTrivial.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NModule.h"
#include "NMetrologyData.h"
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleMetrologyEngineTrivial)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleMetrologyEngineTrivial::NModuleMetrologyEngineTrivial(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceMetrology(), NModuleInterfaceEntry()
{
  // Construct an instance of NModuleMetrologyEngineTrivial

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Trivial";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagMetrologyEngineTrivial";

  // Set the tool tip text
  m_ToolTip = "This is a trivial star tracker simulator.";

  // Set all types this modules handles
  m_ModuleType = c_MetrologyEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsMetrologyEngineTrivial)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsMetrologyEngineTrivial();


  m_UpdateInterval = 1;
}


////////////////////////////////////////////////////////////////////////////////


NModuleMetrologyEngineTrivial::~NModuleMetrologyEngineTrivial()
{
  // Delete this instance of NModuleMetrologyEngineTrivial
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologyEngineTrivial::Initialize()
{
  // Initialize the module 

  // We start before the actual time
  m_Time = -5*m_UpdateInterval;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologyEngineTrivial::AnalyzeMetrologyData(NMetrologyData& Data) 
{
  // Main data analysis routine, which updates the metrology data to a new level 

  // We probably need to retrieve the current alignments from the satellite module

  NOrientation OML1 = m_Satellite.GetOrientationMetrologyLaser(m_Time, 1);
  NOrientation OML2 = m_Satellite.GetOrientationMetrologyLaser(m_Time, 2);

  NOrientation OMD1 = m_Satellite.GetOrientationMetrologyDetector(m_Time, 1);
  NOrientation OMD2 = m_Satellite.GetOrientationMetrologyDetector(m_Time, 2);
  
  // ... and do something with them to generate to generate all three initial metrology data sets as measured


  NPhoton Laser;  // laser in ML coordinate frame
  MVector MDplane(0.0, 0.0, 0.0); // Plane in MD coordinate system
  MVector MDplaneNormal(0.0, 0.0, 1.0);
  
  NMetrologyDataSet Set1;
  Laser.SetPosition(MVector(0.0, 0.0, 0.0)); // need to get right displacement (this will have thermal motion)
  Laser.SetDirection(MVector(0.0, 0.0, -1.0)); // always points down -z (this will have thermal motion)
  OML1.TransformOut(Laser); // Transforms into global system
  OMD1.TransformIn(Laser); // Transforms from global to MD system
  PropagateToPlane(Laser, MDplane, MDplaneNormal); // where laser intersects MD plane
  MVector P1 = Laser.GetPosition(); // get new laser intersection in the MD coordinates
  Set1.SetOriginalLaserHit(P1); // updates the metrology set1 module with coordinates
  Set1.SetTime(m_Time); 
  Data.SetMetrologyDataSet1(Set1); // update data set for the metrology
  // Introduce errors to the Original hit.
  // Call "something" (GetCentroidError) to get centroid error.
  // randomnize P1 with a gaussian.
  //  P1 = CentroidError*gRandom->Rndm();
  Set1.SetCalibratedLaserHit(P1); 
  
  cout << "Laser hit ="<<P1<<endl;
  
  
  NMetrologyDataSet Set2;
  Laser.SetPosition(MVector(0.0, 0.0, 0.0));
  Laser.SetDirection(MVector(0.0, 0.0, -1.0));
  OML2.TransformOut(Laser);
  OMD2.TransformIn(Laser);
  PropagateToPlane(Laser, MDplane, MDplaneNormal);
  MVector P2 = Laser.GetPosition();
  Set2.SetOriginalLaserHit(P2);
  Set2.SetTime(m_Time);
  Data.SetMetrologyDataSet2(Set2);
  // Introduce errors to the Original hit.
  // Call "something" (GetCentroidError) to get centroid error.
  // randomnize P1 with a gaussian.
  //  P1 = CentroidError*gRandom->Rndm();
  Set2.SetCalibratedLaserHit(P2); 


  // Done!

  m_Time += m_UpdateInterval;
  //cout<<"ME-Time: "<<m_Time<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleMetrologyEngineTrivial::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  NGUIOptions* Options = new NGUIOptions(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleMetrologyEngineTrivial::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* UpdateIntervalNode = Node->GetNode("UpdateInterval");
  if (UpdateIntervalNode != 0) {
    m_UpdateInterval = UpdateIntervalNode->GetValueAsDouble();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleMetrologyEngineTrivial::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);  
  new MXmlNode(Node, "UpdateInterval", m_UpdateInterval);


  return Node;
}


// NModuleMetrologyEngineTrivial.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
