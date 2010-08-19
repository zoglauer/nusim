/*
 * NModulePointingPredefined.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModulePointingPredefined
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModulePointingPredefined.h"

// Standard libs:

// ROOT libs:
#include "TRandom.h"
#include "TGClient.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NGUIOptionsPointingPredefined.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModulePointingPredefined)
#endif


////////////////////////////////////////////////////////////////////////////////


// make sure thos eare in sequence!
const int NModulePointingPredefined::c_MotionPatternPredefined  = 0;
const int NModulePointingPredefined::c_MotionPatternRandomWalk  = 1;
// Update those too!!
const int NModulePointingPredefined::c_MotionPatternMin         = 0;
const int NModulePointingPredefined::c_MotionPatternMax         = 1;


////////////////////////////////////////////////////////////////////////////////


NModulePointingPredefined::NModulePointingPredefined(NSatellite& Satellite) : NModule(Satellite), NModuleInterfacePointing()
{
  // Construct an instance of NModulePointingPredefined

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Predefined";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagModulePointingPredefined";

  // Set the tool tip text
  m_ToolTip = "Choose between a set of predefined pointing patterns (e.g. fixed, random walk).";

  // Set all types this modules handles
  m_ModuleType = c_PointingEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsPointing)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsPointing();

  // Some default initilizations:
  m_PointingCenter.SetRaDec(180*60, 45*60);
  m_MotionPattern = c_MotionPatternPredefined;
}


////////////////////////////////////////////////////////////////////////////////


NModulePointingPredefined::~NModulePointingPredefined()
{
  // Delete this instance of NModulePointingPredefined
}


////////////////////////////////////////////////////////////////////////////////


bool NModulePointingPredefined::Initialize()
{
  // Initialize the module 

  m_RWPointing = m_PointingCenter;
  m_RWTime.SetSeconds(-100);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


NPointing NModulePointingPredefined::GetPointing(NTime Time) 
{ 
  // Return the pointing of the satellite at a given time

  if (m_MotionPattern == c_MotionPatternRandomWalk) {
    double Move = 1.0/60/60/60; // Movement per sec -> 60 arcsec per hour
    double MoveTime = 1.0;

    double Ra = m_RWPointing.GetRa(); // ~12:00 hrs
    double Dec = m_RWPointing.GetDec(); // ~45.00 deg
    
    while (m_RWTime < Time) {
      if (Time - m_RWTime > MoveTime) { // larger than a sec
        Ra  += 2*(gRandom->Rndm() - 0.5) * Move;
        Dec += 2*(gRandom->Rndm() - 0.5) * Move;
        m_RWTime += MoveTime;
      } else {
        Ra  += 2*(gRandom->Rndm() - 0.5) * (Move / (Time - m_RWTime).GetSeconds());
        Dec += 2*(gRandom->Rndm() - 0.5) * (Move / (Time - m_RWTime).GetSeconds());
        m_RWTime = Time;
      }
    }

    m_RWPointing.SetRaDec(Ra, Dec);
    return m_RWPointing; 
  } else {
    return m_PointingCenter;
  }
}


////////////////////////////////////////////////////////////////////////////////


void NModulePointingPredefined::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptionsPointingPredefined* Options = new NGUIOptionsPointingPredefined(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


TString NModulePointingPredefined::GetMotionPatternName(int ID)
{
  //! Get the name of a certain motion pattern

  if (ID == c_MotionPatternPredefined) return "Predefined (no motion)";
  else if (ID == c_MotionPatternRandomWalk) return "Random walk";

  return "BUG: Unknown motion mattern";
}


////////////////////////////////////////////////////////////////////////////////


void NModulePointingPredefined::SetMotionPattern(int Pattern)
{
  //! Set the motion pattern

  if (Pattern >= c_MotionPatternMin && Pattern <= c_MotionPatternMax) {
    m_MotionPattern = Pattern;
  } else {
    merr<<"Unknown motion pattern with ID="<<Pattern<<endl;
    m_MotionPattern = c_MotionPatternPredefined;
  }
}


////////////////////////////////////////////////////////////////////////////////


bool NModulePointingPredefined::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  double Ra = 180*60, Dec = 45*60;

  MXmlNode* RaNode = Node->GetNode("Ra");
  if (RaNode != 0) {
    Ra = RaNode->GetValueAsDouble();
  }
  MXmlNode* DecNode = Node->GetNode("Dec");
  if (DecNode != 0) {
    Dec = DecNode->GetValueAsDouble();
  }
  m_PointingCenter.SetRaDec(Ra, Dec);
  MXmlNode* MotionPatternNode = Node->GetNode("MotionPattern");
  if (MotionPatternNode != 0) {
    m_MotionPattern = MotionPatternNode->GetValueAsDouble();
  }


  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModulePointingPredefined::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "Ra", m_PointingCenter.GetRa());
  new MXmlNode(Node, "Dec", m_PointingCenter.GetDec());
  new MXmlNode(Node, "MotionPattern", m_MotionPattern);

  return Node;
}


// NModulePointingPredefined.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
