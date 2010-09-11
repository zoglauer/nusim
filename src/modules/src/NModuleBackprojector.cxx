/*
 * NModuleBackprojector.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleBackprojector
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleBackprojector.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptionsBackprojector.h"
#include "NGUIDiagnosticsBackprojector.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleBackprojector)
#endif


////////////////////////////////////////////////////////////////////////////////


  NModuleBackprojector::NModuleBackprojector(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleBackprojector

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Backprojector";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagBackprojector";

  // Set the tool tip text
  m_ToolTip = "Performs a simple backprojection of the event.";

  // Set all types this modules handles
  m_ModuleType = c_ScienceAnalyzer;

  // Set if this module has a diagnostics GUI
  // Overwrite ShowOptionsGUI() with the call to the GUI!
  m_HasDiagnosticsGUI = true;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsBackprojector)
  // and implement all your GUI options
  m_Diagnostics = 0;
  
  
  m_PixelSize = 0.2; // 12 arcsec
}


////////////////////////////////////////////////////////////////////////////////


NModuleBackprojector::~NModuleBackprojector()
{
  // Delete this instance of NModuleBackprojector

  delete m_Diagnostics;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackprojector::Initialize()
{
  // Initialize the module 

  delete m_Diagnostics;
  m_Diagnostics = new NGUIDiagnosticsBackprojector(m_PixelSize);
  dynamic_cast<NGUIDiagnosticsBackprojector*>(m_Diagnostics)->SetDetectorParameters(m_Satellite.GetOrientationDetectorRelFocalPlaneModule(0, 1, 1).GetTranslation(), 
                                                                                    m_Satellite.GetDetectorHalfDimension(), 
                                                                                    m_Satellite.GetDetectorPixelsX(),
                                                                                    m_Satellite.GetDetectorPixelsY());

  dynamic_cast<NGUIDiagnosticsBackprojector*>(m_Diagnostics)->SetInitialPointing(m_Satellite.GetPointing(0).GetRa(), 
                                                                                 m_Satellite.GetPointing(0).GetDec());
  dynamic_cast<NGUIDiagnosticsBackprojector*>(m_Diagnostics)->SetTime(m_Satellite.GetTimeIdeal());
                                                                   
                                                                                    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackprojector::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  // Backproject the event...

  // Do a trivial backprojection ;-)
  double Energy = 0.0;
  for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
    

    
    double RA = Event.GetHit(i).GetObservatoryData().GetRa();
    double DEC = Event.GetHit(i).GetObservatoryData().GetDec();
    
    MVector D = Event.GetHit(i).GetObservatoryData().GetDirectionEventInIS();
    
    //cout<<endl;
    //cout<<"RA/DEC hit:"<<Event.GetHit(i).GetObservatoryData().GetRa()/60<<":"<<Event.GetHit(i).GetObservatoryData().GetDec()/60<<endl;
    //cout<<"RA/DEC ini:"<<m_InitialRa/60<<" : "<<m_InitialDec/60<<endl;
    /*
    cout<<"RA/DEC dif:"<<RA<<" : "<<DEC<<endl;
    cout<<"RA/DEC evt:"<<Event.GetOriginalPhotonRa()*180/c_Pi*60<<":"<<Event.GetOriginalPhotonDec()*180/c_Pi*60<<endl;
    //cout<<"RA/DEC tst:"<<(atan(D[1]/D[0])-rr[0].rd_0)*!radeg*3600<<":"<<(asin(D[2])-rr[0].rd_1)*!radeg*3600.<<endl;
    cout<<"RA/DEC tst:"<<atan(D[1]/D[0])*180/c_Pi*60<<":"<<asin(D[2])*180/c_Pi*60<<endl;
    */
    
    Energy += Event.GetHit(i).GetEnergy();

    dynamic_cast<NGUIDiagnosticsBackprojector*>(m_Diagnostics)->AddBackprojection(RA, DEC);
  }
  dynamic_cast<NGUIDiagnosticsBackprojector*>(m_Diagnostics)->AddEnergy(Energy);
  dynamic_cast<NGUIDiagnosticsBackprojector*>(m_Diagnostics)->SetTime(Event.GetTime());

  //cout<<"Energy: "<<Energy<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackprojector::Finalize()
{
  // Finalize the module
  
  dynamic_cast<NGUIDiagnosticsBackprojector*>(m_Diagnostics)->SetTime(m_Satellite.GetTimeIdeal());
  
  return true;
}
  

////////////////////////////////////////////////////////////////////////////////


void NModuleBackprojector::ShowOptionsGUI()
{
  // Show the options GUI 

  NGUIOptionsBackprojector* Options = new NGUIOptionsBackprojector(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackprojector::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* PixelSizeNode = Node->GetNode("PixelSize");
  if (PixelSizeNode != 0) {
    m_PixelSize = PixelSizeNode->GetValueAsDouble();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleBackprojector::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  new MXmlNode(Node, "PixelSize", m_PixelSize);

  return Node;
}


// NModuleBackprojector.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
