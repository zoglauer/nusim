/*
 * NModuleOrbitEngineTLE.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleOrbitEngineTLE
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleOrbitEngineTLE.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptionsOrbitEngineTLE.h"
#include "NGUIDiagnosticsOrbitEngine.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleOrbitEngineTLE)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleOrbitEngineTLE::NModuleOrbitEngineTLE(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceOrbit()
{
  // Construct an instance of NModuleOrbitEngineTLE

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "TLE-based orbit engine";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagModuleOrbitEngineTLE";

  // Set the tool tip text
  m_ToolTip = "This is a orbit module which propagates the orbit based on a TLE";

  // Set all types this modules handles
  m_ModuleType = c_OrbitEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  m_Diagnostics = 0;    
}


////////////////////////////////////////////////////////////////////////////////


NModuleOrbitEngineTLE::~NModuleOrbitEngineTLE()
{
  // Delete this instance of NModuleOrbitEngineTLE
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrbitEngineTLE::Initialize()
{
  // Initialize the module 

  if (gROOT->IsBatch() == false) {
    delete m_Diagnostics;
    m_Diagnostics = new NGUIDiagnosticsOrbitEngine();
  }
  
  if (m_TLEFileName != "") {
    MFile::ExpandFileName(m_TLEFileName);
    if (MFile::Exists(m_TLEFileName) == false) {
      cerr<<"Error: NModuleOrbitEngineTLE: TLE file does not exist: "<<m_TLEFileName<<endl;
      return false;
    }
  }

  m_LimbAngle = 10*deg;


  // read the file nustar.tle in to the SGP4 model
  string errorString;
  if (!m_SGP4.read(m_TLEFileName.Data(), errorString)) {
    cerr<<"Error: NModuleOrbitEngineTLE::Initialize: error reading TLE: "<<errorString<<endl;
    return false;
  }
     
  cout<<"Pre-calculating a first set of occultation times & day-night cycles..."<<endl;
  // Advance time to zero to make sure we are out of initialization troubles...
  m_Time.Set(-4*60*60+1.0);
  m_BeginOccultationTime.push_back(NTime(-4*60*60.0-100.0));
  m_EndOccultationTime.push_back(NTime(-4*60*60.0-1.0));
  m_BeginNightTime.push_back(NTime(-4*60*60.0-100.0));
  m_EndNightTime.push_back(NTime(-4*60*60.0-1.0));

  AdvanceTime(NTime(4*60*60));
  
  if (m_BeginOccultationTime.size() < 2 || m_EndOccultationTime.size() < 2 || 
      m_BeginNightTime.size() < 2 || m_EndNightTime.size() < 2) {
    cerr<<"Error: NModuleOrbitEngineTLE::Initialize: Unable to initialize times correctly..."<<endl;
    return false;
  }
  
  m_BeginOccultationTime.erase(m_BeginOccultationTime.begin());
  m_EndOccultationTime.erase(m_EndOccultationTime.begin());
  m_BeginNightTime.erase(m_BeginNightTime.begin());
  m_EndNightTime.erase(m_EndNightTime.begin());
  
  cout<<"Initial night times: "<<m_BeginNightTime.size()<<endl;
  unsigned int e = 0;
  for (unsigned int i = 0; i < m_BeginNightTime.size(); ++i) {
    cout<<m_Satellite.ConvertToAbsoluteTime(m_BeginNightTime[i]).GetDateInString();
    while (m_EndNightTime[e] < m_BeginNightTime[i] && e < m_EndNightTime.size()) ++e;
    if (e < m_EndNightTime.size()) {
      cout<<" - "<<m_Satellite.ConvertToAbsoluteTime(m_EndNightTime[e]).GetDateInString()<<endl;
    } else {
      cout<<endl;
    }
  }
  
  cout<<"Initial occultation times: "<<m_BeginOccultationTime.size()<<endl;
  e = 0;
  for (unsigned int i = 0; i < m_BeginOccultationTime.size(); ++i) {
    cout<<m_Satellite.ConvertToAbsoluteTime(m_BeginOccultationTime[i]).GetDateInString();
    while (m_EndOccultationTime[e] < m_BeginOccultationTime[i] && e < m_EndOccultationTime.size()) ++e;
    if (e < m_EndOccultationTime.size()) {
      cout<<" - "<<m_Satellite.ConvertToAbsoluteTime(m_EndOccultationTime[e]).GetDateInString()<<endl;
    } else {
      cout<<endl;
    }
  }
  
  return true;
}

  
////////////////////////////////////////////////////////////////////////////////


bool NModuleOrbitEngineTLE::AdvanceTime()
{
  //! Advance the time by one hour
  
  return AdvanceTime(m_Time + 1*hour);
}

  
////////////////////////////////////////////////////////////////////////////////


bool NModuleOrbitEngineTLE::AdvanceTime(const NTime& Time)
{
  //! Advance the time until "Time" in 1 sec intervals
    
 
  while (m_Time < Time) {
    m_Time += 1*s;
    //cout<<"Time: "<<m_Time<<endl;

    NPointing P = m_Satellite.GetPointing(m_Time);
    
    // target direction vector -- initialize from celestial coordinates
    vecutil::Dirvector ObjectDirection;
    ObjectDirection.fromCelestial(P.GetRa()/rad, P.GetDec()/rad);

    // create an occultation interval object, set with orbit model
    vecutil::OrbitalOccultationIntervals ooi(&m_SGP4);
    // set target in occultation object
    ooi.setTarget(ObjectDirection);
    // set desired critical limb angle
    ooi.setLimbAngle(m_LimbAngle/rad);
  
    // create an orbital night interval object, set with orbit model
    vecutil::OrbitalNightIntervals oni(&m_SGP4);



    // calculate orbital occultation intervals
    tmtc::TmTcTime tStart, tEnd;
    
    NTime T = m_Satellite.ConvertToAbsoluteTime(m_Time);
    T -= 4*hour;
    tStart.setGregorian(T.GetYears(), T.GetMonths(), T.GetDays(), T.GetHours(), T.GetMinutes(), T.GetSeconds());
    T += 4*hour;
    tEnd.setGregorian(T.GetYears(), T.GetMonths(), T.GetDays(), T.GetHours(), T.GetMinutes(), T.GetSeconds());
    //cout<<tStart.getEngineering()<<endl;
    //cout<<tEnd.getEngineering()<<endl;
    
    
    tmtc::TmTcIntervalSet isOcc = ooi.getIntervals(tStart, tEnd);
    
    bool PreviouslyOcculted = true;
    if (m_BeginOccultationTime.back() < m_EndOccultationTime.back()) {
      PreviouslyOcculted = false;
    }
    
    bool NowOcculted = false;
    for (tmtc::TmTcIntervalSet::const_iterator i = isOcc.begin(), iEnd = isOcc.end(); i != iEnd; i++) {
      // get the ingress and egress times of this interval
      // (in other words, target occultation start and end)
      const tmtc::TmTcTime tIngress = i->getStart();
      const tmtc::TmTcTime tEgress  = i->getEnd();
      NTime Start;
      Start.Set(tIngress.getYear(), tIngress.getMonth(), tIngress.getDay(), tIngress.getHour(), tIngress.getMinute(), tIngress.getSecond());
      Start = m_Satellite.ConvertFromAbsoluteTime(Start);
      NTime Stop;
      Stop.Set(tEgress.getYear(), tEgress.getMonth(), tEgress.getDay(), tEgress.getHour(), tEgress.getMinute(), tEgress.getSecond());
      Stop = m_Satellite.ConvertFromAbsoluteTime(Stop);
    
      if (m_Time >= Start && m_Time <= Stop) {
        NowOcculted = true;
        break;
      }
    }
    if (PreviouslyOcculted == false && NowOcculted == true) {
      //cout<<"New occultation begin: "<<m_Time<<endl;
      m_BeginOccultationTime.push_back(m_Time);
    } else if (PreviouslyOcculted == true && NowOcculted == false) {
      //cout<<"New occultation end: "<<m_Time<<endl;
      m_EndOccultationTime.push_back(m_Time);
    }
    
    
    // Calculated the day and night times:
    tmtc::TmTcIntervalSet isNight = oni.getIntervals(tStart, tEnd);
    //cout<<"IsNight: "<<isNight<<endl;

    bool PreviouslyNight = true;
    if (m_BeginNightTime.back() < m_EndNightTime.back()) {
      PreviouslyNight = false;
    }
    
    bool NowNight = false;
    for (tmtc::TmTcIntervalSet::const_iterator i = isNight.begin(), iEnd = isNight.end(); i != iEnd; i++) {
      // get the ingress and egress times of this interval
      // (in other words, target occultation start and end)
      const tmtc::TmTcTime tIngress = i->getStart();
      const tmtc::TmTcTime tEgress  = i->getEnd();
      NTime Start;
      Start.Set(tIngress.getYear(), tIngress.getMonth(), tIngress.getDay(), tIngress.getHour(), tIngress.getMinute(), tIngress.getSecond());
      Start = m_Satellite.ConvertFromAbsoluteTime(Start);
      NTime Stop;
      Stop.Set(tEgress.getYear(), tEgress.getMonth(), tEgress.getDay(), tEgress.getHour(), tEgress.getMinute(), tEgress.getSecond());
      Stop = m_Satellite.ConvertFromAbsoluteTime(Stop);

      if (m_Time >= Start && m_Time <= Stop) {
        NowNight = true;
        break;
      }
    }
    //cout<<"Prev. night: "<<((PreviouslyNight == true) ? "true" : "false")<<" now night: "<<((NowNight == true) ? "true" : "false")<<endl;
    if (PreviouslyNight == false && NowNight == true) {
      //cout<<"New night begin: "<<m_Time<<endl;
      m_BeginNightTime.push_back(m_Time);
    } else if (PreviouslyNight == true && NowNight == false) {
      //cout<<"New night end: "<<m_Time<<endl;
      m_EndNightTime.push_back(m_Time);
    }
    if (m_Time >= 0 && gROOT->IsBatch() == false) {
      dynamic_cast<NGUIDiagnosticsOrbitEngine*>(m_Diagnostics)->AddOrbitStatus(m_Time, NowOcculted, NowNight);
    }
  }
  

  
  return true;
}

  
////////////////////////////////////////////////////////////////////////////////


NOrbit NModuleOrbitEngineTLE::GetOrbit(NTime Time)
{
  //! Return the orbit position at a given time

  // Convert the time
  NTime T = m_Satellite.ConvertToAbsoluteTime(Time);
  tmtc::TmTcTime t; // set the time
  t.setGregorian(T.GetYears(), T.GetMonths(), T.GetDays(), T.GetHours(), T.GetMinutes(), T.GetSeconds());

  // Get the ECI position at this instant
  vecutil::StateVector sv = m_SGP4.getStateVector(t);
  vecutil::Dirvector dvPos = sv.getPosition();

  // Translate the vector from ECI to ECEF
  dvPos = dvPos.eci2ecef(t);
        
  // Get the latitude and longitude (NB: lng and lat will be in radians)
  double lat, lng; 
  dvPos.toCelestial(lng, lat);
  
  NOrbit Orbit;
  Orbit.SetAltitude((sv.getAltitude()-vecutil::KM_PER_EARTH)*km);
  Orbit.SetLatitude(lat*rad);
  Orbit.SetLongitude(lng*rad);

  return Orbit;
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleOrbitEngineTLE::StartOfNextBlackout(NTime t)
{
  //! Get the start time of the next black-out
  
  // Make sure we have a next blackout...
  while (m_BeginOccultationTime.back() < t) AdvanceTime();
  
  // Find it going from the back...
  for (unsigned int i = m_BeginOccultationTime.size() - 2; i < m_BeginOccultationTime.size(); --i) {
    if (m_BeginOccultationTime[i] < t) return m_BeginOccultationTime[i+1];
  }
 
  merr<<"By design, we sould never reach this part of the code..."<<fatal;
 
  return NTime(0);
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleOrbitEngineTLE::EndOfNextBlackout(NTime t)
{
  //! Get the end time of the next black-out

  // Make sure we have a next blackout...
  while (m_EndOccultationTime.back() < t) AdvanceTime();
  
  // Find it going from the back...
  for (unsigned int i = m_EndOccultationTime.size() - 2; i < m_EndOccultationTime.size(); --i) {
    if (m_EndOccultationTime[i] < t) return m_EndOccultationTime[i+1];
  }
 
  merr<<"By design, we sould never reach this part of the code..."<<fatal;

  return NTime(0);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrbitEngineTLE::IsBlackout(NTime t)
{
  //! Return true if we are within blackout

  // Make sure we have a complete blackout...
  while (m_BeginOccultationTime.back() < t) AdvanceTime();
  while (m_EndOccultationTime.back() < t || m_EndOccultationTime.back() < m_BeginOccultationTime.back()) AdvanceTime();

  // Find the first start which is smaller than this one:
  NTime Begin;
  for (unsigned int i = m_BeginOccultationTime.size() - 2; i < m_BeginOccultationTime.size(); --i) {
    if (m_BeginOccultationTime[i] < t) {
      Begin = m_BeginOccultationTime[i+1];
      break;
    }
  }
  // Find the first End after begin
  NTime End;
  for (unsigned int i = m_EndOccultationTime.size() - 2; i < m_EndOccultationTime.size(); --i) {
    if (m_EndOccultationTime[i] < Begin) End = m_EndOccultationTime[i+1];
  }

  if (Begin >= t && End <= t) return true; 
  
  return false;
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleOrbitEngineTLE::GetBlackoutDuration(NTime t1, NTime t2)
{
  //! Get the duration of the sum of all blackouts between t1 and t2

  if (t1 == t2) {
    return NTime(0.0);
  } else if (t1 > t2) {
    merr<<"GetBlackoutDuration: t1 >= t2: "<<t1<<" vs. "<<t2<<endl;
    return NTime(0.0);
  }

  while (m_BeginOccultationTime.back() < t2) AdvanceTime();
  while (m_EndOccultationTime.back() < t2) AdvanceTime();

  unsigned int FirstStart = 0;
  unsigned int FirstEnd = 0;
  
  // Find the first start which is larger than this one:
  // x---||||||
  //     ^
  for (unsigned int i = m_BeginOccultationTime.size() - 2; i < m_BeginOccultationTime.size(); --i) {
    if (m_BeginOccultationTime[i] < t1) {
      FirstStart = i+1;
      break;
    }
  }
  // Find the first End after begin
  // x---||||||
  //          ^
  for (unsigned int i = m_EndOccultationTime.size() - 2; i < m_EndOccultationTime.size(); --i) {
    if (m_EndOccultationTime[i] < t1) {
      FirstEnd = i+1;
      break;
    }
  }

  NTime CurrentTime = t1;
  NTime BlackoutDuration = NTime(0.0);

  while (CurrentTime < t2) {
    if (m_BeginOccultationTime[FirstStart] < m_EndOccultationTime[FirstEnd]) {
      CurrentTime = m_BeginOccultationTime[FirstStart];
      ++ FirstStart;
    } else {
      if (t2 < m_EndOccultationTime[FirstEnd]) {
        BlackoutDuration += t2 - CurrentTime;        
        CurrentTime = t2;
        break;
      } else {
        BlackoutDuration += m_EndOccultationTime[FirstEnd] - CurrentTime;
        CurrentTime = m_EndOccultationTime[FirstEnd];
        ++FirstEnd;
      }
    }
  }
  
  return BlackoutDuration;
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleOrbitEngineTLE::GetEffectiveObservationTime(NTime t1, NTime t2) 
{ 
  // Get the effective observation time between times

  return t2 - t1 - GetBlackoutDuration(t1, t2);
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleOrbitEngineTLE::FindIdealTime(NTime ObservationTime)
{
  //! Get the ideal time as a function of observation time starting from 0
  
  NTime T(0.0);
  NTime Accum(0.0);
  
  while (m_BeginOccultationTime.back() < T) AdvanceTime();
  while (m_EndOccultationTime.back() < T) AdvanceTime();
  
  unsigned int FirstStart = 0;
  unsigned int FirstEnd = 0;
  
  // Find the first start which is larger than this one:
  for (unsigned int i = m_BeginOccultationTime.size() - 2; i < m_BeginOccultationTime.size(); --i) {
    if (m_BeginOccultationTime[i] < T) {
      FirstStart = i+1;
      break;
    }
  }
  // Find the first End after begin
  for (unsigned int i = m_EndOccultationTime.size() - 2; i < m_EndOccultationTime.size(); --i) {
    if (m_EndOccultationTime[i] < T) {
      FirstEnd = i+1;
      break;
    }
  }
  
  
  while (T < ObservationTime) {
    while (m_BeginOccultationTime.back() < T) AdvanceTime();
    while (m_EndOccultationTime.back() < T) AdvanceTime();
    
    if (m_BeginOccultationTime[FirstStart] < m_EndOccultationTime[FirstEnd]) {
      if (Accum + (m_BeginOccultationTime[FirstStart] - T) >= ObservationTime) {
        T += ObservationTime - Accum;
        Accum = ObservationTime;
        break;
      } else {
        Accum += m_BeginOccultationTime[FirstStart] - T;
        T = m_BeginOccultationTime[FirstStart];
        ++FirstStart;
      }
    } else {
      T = m_EndOccultationTime[FirstEnd];
      ++FirstEnd;
    }
  }
  
  return T;
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleOrbitEngineTLE::StartOfPreviousBlackout(NTime t)
{  
  //! Get the start time of the previous black-out

  // Make sure we have a next blackout...
  while (m_BeginOccultationTime.back() < t) AdvanceTime();
  
  // Find it going from the back...
  for (unsigned int i = m_BeginOccultationTime.size() - 2; i < m_BeginOccultationTime.size(); --i) {
    if (m_BeginOccultationTime[i] < t) return m_BeginOccultationTime[i];
  }
 
  merr<<"By design, we sould never reach this part of the code..."<<fatal;
 
  return NTime(0);
}


////////////////////////////////////////////////////////////////////////////////


void NModuleOrbitEngineTLE::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptionsOrbitEngineTLE* Options = new NGUIOptionsOrbitEngineTLE(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrbitEngineTLE::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* TLEFileNameNode = Node->GetNode("TLEFileName");
  if (TLEFileNameNode != 0) {
    m_TLEFileName = TLEFileNameNode->GetValueAsString();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleOrbitEngineTLE::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);

  new MXmlNode(Node, "TLEFileName", m_TLEFileName);

  return Node;
}


// NModuleOrbitEngineTLE.cxx: the end...
////////////////////////////////////////////////////////////////////////////////