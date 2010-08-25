/*
 * NModuleStarTrackerEngineTrivial.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleStarTrackerEngineTrivial
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleStarTrackerEngineTrivial.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptionsStarTrackerEngineTrivial.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleStarTrackerEngineTrivial)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleStarTrackerEngineTrivial::NModuleStarTrackerEngineTrivial(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceStarTracker(), NModuleInterfaceEntry()
{
  // Construct an instance of NModuleStarTrackerEngineTrivial

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Trivial";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagStarTrackerEngineTrivial";

  // Set the tool tip text
  m_ToolTip = "This is a trivial star tracker simulator.";

  // Set all types this modules handles
  m_ModuleType = c_StarTrackerEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsStarTrackerEngineTrivial)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsStarTrackerEngineTrivial();


  m_UpdateInterval = 10;
}


////////////////////////////////////////////////////////////////////////////////


NModuleStarTrackerEngineTrivial::~NModuleStarTrackerEngineTrivial()
{
  // Delete this instance of NModuleStarTrackerEngineTrivial
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleStarTrackerEngineTrivial::Initialize()
{
  // Initialize the module 

  m_Time = -10*m_UpdateInterval;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleStarTrackerEngineTrivial::AnalyzeStarTrackerData(NStarTrackerData& Data) 
{
  // Main data analysis routine, which updates the event to a new level 

  // We probably need to retrieve the current alignments from the satellite module

  NOrientation OS4 = m_Satellite.GetOrientationStarTracker(m_Time, 4);

  // Generate the initial star tracker data as measured by 

  NStarTrackerDataSet CH4;
  CH4.SetTime(m_Time);

  //MVector testaxis(0,0,1);
  
  NPointing P = m_Satellite.GetPointing(m_Time);
   
  NQuaternion Qstin = P.GetQuaternion()*OS4.GetRotationQuaternion().Invert();
  
  //cout<<Q<<endl;
  //cout<<"Before"<<Q.Rotation(testaxis)<<endl;
  //cout<<"RA "<<P.GetRa()<<" Dec "<<P.GetDec()<<endl;

  CH4.SetOriginalPointing(P);
  CH4.SetMeasuredTransformation(Qstin);

  Data.SetStarTrackerDataSet4(CH4);
  m_Time += m_UpdateInterval;
  //cout<<"ST-Time: "<<m_Time<<endl;

  // Test code to check that all orientation etc are read correctly and reach this point -- please don't remove - Andreas
  /*
  cout<<"OrientationSpaceCraftRelInertial: "<<m_Satellite.GetOrientationSpaceCraftRelInertial(0.0)<<endl;
  cout<<"OrientationFocalPlaneRelSC: "<<m_Satellite.GetOrientationFocalPlaneRelSC(0.0)<<endl;
  cout<<"OrientationOpticalBench: "<<m_Satellite.GetOrientationOpticalBench(0.0)<<endl; 
  cout<<"OrientationOptics 1: "<<m_Satellite.GetOrientationOptics(0.0, 1)<<endl;
  cout<<"OrientationOptics 2: "<<m_Satellite.GetOrientationOptics(0.0, 2)<<endl;
  cout<<"OrientationOpticsRelOpticalBench 1: "<<m_Satellite.GetOrientationOpticsRelOpticalBench(0.0, 1)<<endl; 
  cout<<"OrientationOpticsRelOpticalBench 2: "<<m_Satellite.GetOrientationOpticsRelOpticalBench(0.0, 2)<<endl; 
  cout<<"OrientationAperture 1: "<<m_Satellite.GetOrientationAperture(0.0, 1)<<endl; 
  cout<<"OrientationAperture 2: "<<m_Satellite.GetOrientationAperture(0.0, 2)<<endl; 
  cout<<"OrientationMetrologyLaserRelOpticalBench 1: "<<m_Satellite.GetOrientationMetrologyLaserRelOpticalBench(0.0, 1)<<endl;
  cout<<"OrientationMetrologyLaserRelOpticalBench 2: "<<m_Satellite.GetOrientationMetrologyLaserRelOpticalBench(0.0, 2)<<endl;
  cout<<"OrientationMetrologyDetector 1: "<<m_Satellite.GetOrientationMetrologyDetector(0.0, 1)<<endl; 
  cout<<"OrientationMetrologyDetector 2: "<<m_Satellite.GetOrientationMetrologyDetector(0.0, 2)<<endl; 
  cout<<"OrientationStarTrackerRelOpticalBench 4: "<<m_Satellite.GetOrientationStarTrackerRelOpticalBench(0.0, 4)<<endl; 
  cout<<"OrientationFocalPlaneModule 1: "<<m_Satellite.GetOrientationFocalPlaneModule(0.0, 1)<<endl; 
  cout<<"OrientationFocalPlaneModule 2: "<<m_Satellite.GetOrientationFocalPlaneModule(0.0, 2)<<endl; 

  cout<<"CalibratedPointingOpticalAxisRelOM 1: "<<m_Satellite.GetCalibratedPointingOpticalAxisRelOM(1)<<endl; 
  cout<<"CalibratedPointingOpticalAxisRelOM 2: "<<m_Satellite.GetCalibratedPointingOpticalAxisRelOM(2)<<endl; 
  cout<<"CalibratedPointingMetrologyLaserRelML 1: "<<m_Satellite.GetCalibratedPointingMetrologyLaserRelML(1)<<endl; 
  cout<<"CalibratedPointingMetrologyLaserRelML 2: "<<m_Satellite.GetCalibratedPointingMetrologyLaserRelML(2)<<endl; 

  cout<<"CalibratedOrientationFocalPlaneModule 1: "<<m_Satellite.GetCalibratedOrientationFocalPlaneModule(1) <<endl;
  cout<<"CalibratedOrientationFocalPlaneModule 2: "<<m_Satellite.GetCalibratedOrientationFocalPlaneModule(2) <<endl;
  cout<<"CalibratedOrientationMetrologyDetector 1: "<<m_Satellite.GetCalibratedOrientationMetrologyDetector(1)<<endl; 
  cout<<"CalibratedOrientationMetrologyDetector 2: "<<m_Satellite.GetCalibratedOrientationMetrologyDetector(2)<<endl; 
  cout<<"CalibratedOrientationOpticalBench: "<<m_Satellite.GetCalibratedOrientationOpticalBench()<<endl; 
  cout<<"CalibratedOrientationOpticsRelOpticalBench 1: "<<m_Satellite.GetCalibratedOrientationOpticsRelOpticalBench(1)<<endl; 
  cout<<"CalibratedOrientationOpticsRelOpticalBench 2: "<<m_Satellite.GetCalibratedOrientationOpticsRelOpticalBench(2)<<endl; 
  cout<<"CalibratedOrientationMetrologyLaserRelOpticalBench 1: "<<m_Satellite.GetCalibratedOrientationMetrologyLaserRelOpticalBench(1) <<endl;
  cout<<"CalibratedOrientationMetrologyLaserRelOpticalBench 2: "<<m_Satellite.GetCalibratedOrientationMetrologyLaserRelOpticalBench(2) <<endl;
  cout<<"CalibratedOrientationStarTrackerRelOpticalBench 4: "<<m_Satellite.GetCalibratedOrientationStarTrackerRelOpticalBench(4)<<endl;

  cout<<"CentroidingErrorMetrologyDetector 1: "<<m_Satellite.GetCentroidingErrorMetrologyDetector(1)<<endl; 
  cout<<"CentroidingErrorMetrologyDetector 2: "<<m_Satellite.GetCentroidingErrorMetrologyDetector(2)<<endl; 
  cout<<"PointingErrorStarTracker 4: "<<m_Satellite.GetPointingErrorStarTracker(4)<<endl;

  cout<<"ErrorXYZOrientationFocalPlaneModule 1: "<<m_Satellite.GetErrorXYZOrientationFocalPlaneModule(1)<<endl; 
  cout<<"ErrorXYZOrientationFocalPlaneModule 2: "<<m_Satellite.GetErrorXYZOrientationFocalPlaneModule(2)<<endl; 
  cout<<"ErrorXYZOrientationMetrologyDetector 1: "<<m_Satellite.GetErrorXYZOrientationMetrologyDetector(1)<<endl; 
  cout<<"ErrorXYZOrientationMetrologyDetector 2: "<<m_Satellite.GetErrorXYZOrientationMetrologyDetector(2)<<endl; 
  cout<<"ErrorXYZOrientationAperture 1: "<<m_Satellite.GetErrorXYZOrientationAperture(1)<<endl; 
  cout<<"ErrorXYZOrientationAperture 2: "<<m_Satellite.GetErrorXYZOrientationAperture(2)<<endl; 
  cout<<"ErrorXYZOrientationOpticalBench: "<<m_Satellite.GetErrorXYZOrientationOpticalBench()<<endl; 
  cout<<"ErrorXYZOrientationOpticsRelOpticalBench 1: "<<m_Satellite.GetErrorXYZOrientationOpticsRelOpticalBench(1) <<endl;
  cout<<"ErrorXYZOrientationOpticsRelOpticalBench 2: "<<m_Satellite.GetErrorXYZOrientationOpticsRelOpticalBench(2) <<endl;
  cout<<"ErrorXYZOrientationMetrologyLaserRelOpticalBench 1: "<<m_Satellite.GetErrorXYZOrientationMetrologyLaserRelOpticalBench(1)<<endl; 
  cout<<"ErrorXYZOrientationMetrologyLaserRelOpticalBench 2: "<<m_Satellite.GetErrorXYZOrientationMetrologyLaserRelOpticalBench(2)<<endl; 
  cout<<"ErrorXYZOrientationStarTrackerRelOpticalBench 4: "<<m_Satellite.GetErrorXYZOrientationStarTrackerRelOpticalBench(4)<<endl; 

  cout<<"ErrorDCMOrientationFocalPlaneModule 1: "<<m_Satellite.GetErrorDCMOrientationFocalPlaneModule(1)<<endl; 
  cout<<"ErrorDCMOrientationFocalPlaneModule 2: "<<m_Satellite.GetErrorDCMOrientationFocalPlaneModule(2)<<endl; 
  cout<<"ErrorDCMOrientationMetrologyDetector 1: "<<m_Satellite.GetErrorDCMOrientationMetrologyDetector(1)<<endl; 
  cout<<"ErrorDCMOrientationMetrologyDetector 2: "<<m_Satellite.GetErrorDCMOrientationMetrologyDetector(2)<<endl; 
  cout<<"ErrorDCMOrientationAperture 1: "<<m_Satellite.GetErrorDCMOrientationAperture(1)<<endl;
  cout<<"ErrorDCMOrientationAperture 2: "<<m_Satellite.GetErrorDCMOrientationAperture(2)<<endl;
  cout<<"ErrorDCMOrientationOpticalBench: "<<m_Satellite.GetErrorDCMOrientationOpticalBench()<<endl; 
  cout<<"ErrorDCMOrientationOpticsRelOpticalBench 1: "<<m_Satellite.GetErrorDCMOrientationOpticsRelOpticalBench(1)<<endl;
  cout<<"ErrorDCMOrientationOpticsRelOpticalBench 2: "<<m_Satellite.GetErrorDCMOrientationOpticsRelOpticalBench(2)<<endl;
  cout<<"ErrorDCMOrientationMetrologyLaserRelOpticalBench 1: "<<m_Satellite.GetErrorDCMOrientationMetrologyLaserRelOpticalBench(1)<<endl; 
  cout<<"ErrorDCMOrientationMetrologyLaserRelOpticalBench 2: "<<m_Satellite.GetErrorDCMOrientationMetrologyLaserRelOpticalBench(2)<<endl; 
  cout<<"ErrorDCMOrientationStarTrackerRelOpticalBench 4: "<<m_Satellite.GetErrorDCMOrientationStarTrackerRelOpticalBench(4)<<endl;
  */

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleStarTrackerEngineTrivial::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  NGUIOptionsStarTrackerEngineTrivial* Options = new NGUIOptionsStarTrackerEngineTrivial(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleStarTrackerEngineTrivial::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* UpdateIntervalNode = Node->GetNode("UpdateInterval");
  if (UpdateIntervalNode != 0) {
    m_UpdateInterval = UpdateIntervalNode->GetValueAsDouble();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleStarTrackerEngineTrivial::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "UpdateInterval", m_UpdateInterval);

  return Node;
}


// NModuleStarTrackerEngineTrivial.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
