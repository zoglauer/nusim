/*
 * NPointing.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NPointing
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NPointing.h"

// Standard libs:

// ROOT libs:
#include "NQuaternion.h"
#include "TVector3.h"
// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NPointing)
#endif


////////////////////////////////////////////////////////////////////////////////


NPointing::NPointing()
{
  // Construct an instance of NPointing

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NPointing::~NPointing()
{
  // Delete this instance of NPointing
}


////////////////////////////////////////////////////////////////////////////////


void NPointing::Clear() 
{
  // Reset the pointing data to default values 

  m_Ra = 0.0;
  m_Dec = 0.0;
  m_Yaw = 180*60;
  m_Time.Set(0.0);
  RaDecToQuaternion();

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


void NPointing::SetYaw(double Yaw)
{  
  //! Set a new yaw
  
  m_Empty = false;
  m_Yaw = Yaw;
  RaDecToQuaternion();  
}


////////////////////////////////////////////////////////////////////////////////


void NPointing::QuaternionToRaDec()
{
  // Rotate unite vector with quaternion to calculate Ra, Dec.
  // Then calculate the combined rotation around Z axis and 
  
  MVector Zhat(0.0,0.0,1.0);
  m_Q.Rotate(Zhat);
  
  m_Ra = 60*c_Deg*atan2(Zhat[1],Zhat[0]);
  m_Dec = 60*c_Deg*asin(Zhat[2]);
  
  // Next to find yaw:
  // Q.R = cyaw*cra*cdeg - syaw*sra*cdeg
  //       = cdeg*cos(yaw+ra)
  // Q.V[2] = cyaw*sra*cdeg - syaw*cra*cdeg
  //          = cdeg*sin(yaw+ra)
  // atan(Q.V[2]/Q.R) = cyaw/2+ra/2
  
  m_Yaw = 60 * c_Deg * atan(m_Q.m_V[2]/m_Q.m_R)*2.;
  m_Yaw -= m_Ra;
  if (m_Yaw < 0) m_Yaw+=360.0*60.0;
  if (m_Ra < 0) m_Ra+=360.0*60.0;

  //cout<<"RA:"<<m_Ra/60.<<" DEC: "<<m_Dec/60.<<" Yaw:"<<m_Yaw/60.<<endl;
    
 }


////////////////////////////////////////////////////////////////////////////////


void NPointing::RaDecToQuaternion()
{
   // Ra, Dec are in radians. Euler angles.

   //cout<<"RA/DEC sanity check before: RA:"<<m_Ra/60<<" DEC: "<<m_Dec/60<<endl;
   
   //Important thing to remember here is that the Z-axis of the space craft is what
   //needs to be pointing at the proper ra/deg. 
   
   double cyaw=cos(m_Yaw/60*c_Rad/2.);
   double syaw=sin(m_Yaw/60*c_Rad/2.);
   
   m_QR.m_R = cyaw;
   m_QR.m_V[0] = 0.0;
   m_QR.m_V[1] = 0.0; 
   m_QR.m_V[2] = syaw;
   
   double cr = cos(m_Ra/60*c_Rad/2.);
   double cd = cos((c_Pi/2.-m_Dec/60*c_Rad)/2.);
   double sr = sin(m_Ra/60*c_Rad/2.);
   double sd = sin((c_Pi/2.-m_Dec/60*c_Rad)/2.);
   
   m_Q.m_R = cr*cd;
   m_Q.m_V[0] = -sr*sd;
   m_Q.m_V[1] = cr*sd; 
   m_Q.m_V[2] = sr*cd;
   
   // First apply Ra, Dec, then yaw around new Z-axis (Z-Y-Z) rotation. Frame rotation!!
   m_Q = m_Q*m_QR;	 
   	 
   QuaternionToRaDec();
   //cout<<"m_q "<<m_Q<<endl;
   //cout<<"RA/DEC sanity check after: RA:"<<m_Ra/60.<<" DEC: "<<m_Dec/60.<<" Yaw:"<<m_Yaw/60.<<endl;
}


////////////////////////////////////////////////////////////////////////////////


bool NPointing::Stream(ofstream& S, TString Keyword)
{
  //! Stream the content to an ASCII file 

  S<<Keyword<<" ";
  if (m_Empty == true) {
    S<<"-"<<endl;
  } else {
    S<<m_Ra<<" "<<m_Dec<<endl;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


TString NPointing::ToString() const 
{
  //! Dump content to a string

  TString T("RD: RA=");
  T += m_Ra/60;
  T += " deg, DEC=";
  T += m_Dec/60;
  T += " deg, Yaw=";
  T += m_Yaw/60;
  T += " deg";
  if (m_Time.GetAsSeconds() > 0.0) {
    T += ", Time=";
    T += m_Time.GetAsSeconds();
    T += " sec";
  }

  return T;
}


////////////////////////////////////////////////////////////////////////////////


bool NPointing::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file  

  if (Line.Length() == 4 && Line[4] == '-') {
    Clear();
    return true;
  }

  char Key[2];
  double Time = 0;
  if (sscanf(Line.Data(), "%s %lf %lf %lf %lf", Key, &m_Ra, &m_Dec, &m_Yaw, &Time) != 5) {
    if (sscanf(Line.Data(), "%s %lf %lf %lf", Key, &m_Ra, &m_Dec, &Time) != 4) {
      if (sscanf(Line.Data(), "%s %lf %lf", Key, &m_Ra, &m_Dec) != 3) {
        Clear();
        return false;
      }
    }
    m_Yaw = 180*60; 
    m_Ra *= 60;
    m_Dec *= 60;
  } else {
    m_Ra *= 60;
    m_Dec *= 60;
    m_Yaw *= 60;
  }
  
  RaDecToQuaternion();
  m_Time.Set(Time);

  m_Empty = false;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NPointing::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  Clear();
  
  MXmlNode* RaNode = Node->GetNode("RA");
  if (RaNode != 0) {
    m_Ra = RaNode->GetValueAsDouble();
  }
  MXmlNode* DecNode = Node->GetNode("DEC");
  if (DecNode != 0) {
    m_Dec = DecNode->GetValueAsDouble();
  }
  MXmlNode* YawNode = Node->GetNode("Yaw");
  if (YawNode != 0) {
    m_Yaw = YawNode->GetValueAsDouble();
  }
  MXmlNode* TimeNode = Node->GetNode("Time");
  if (TimeNode != 0) {
    m_Time.Set(TimeNode->GetValueAsDouble());
  }

  // Do an official "set" to initialize all the other variables:
  SetRaDecYaw(m_Ra, m_Dec, m_Yaw);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NPointing::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, "Pointing");
  
  new MXmlNode(Node, "RA", m_Ra);
  new MXmlNode(Node, "DEC", m_Dec);
  new MXmlNode(Node, "Yaw", m_Yaw);
  new MXmlNode(Node, "Time", m_Time.GetAsSeconds());

  return Node;
}


// NPointing.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
