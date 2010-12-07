/*
 * NOrientation.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NOrientation
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NOrientation.h"

// Standard libs:
#include <sstream>

// ROOT libs:
#include "TVector3.h"
#include "NQuaternion.h"

// MEGAlib libs:
#include "MStreams.h"
#include "MTokenizer.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NOrientation)
#endif


////////////////////////////////////////////////////////////////////////////////


double NOrientation::m_Unity[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};


////////////////////////////////////////////////////////////////////////////////


NOrientation::NOrientation()
{
  // Construct an instance of NOrientation

  m_Rotation.ResizeTo(3,3);

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NOrientation::~NOrientation()
{
  // Delete this instance of NOrientation
}


////////////////////////////////////////////////////////////////////////////////


void NOrientation::Clear() 
{
  // Reset the photon data to "NotDefined" values (polarization is zero)

  m_Translation.Clear();
  
  // The following has been optimized for exceution performance
  double* Array = m_Rotation.GetMatrixArray();
  for (unsigned int i = 0 ; i < 9; ++i) {
    Array[i] = m_Unity[i];
  }
  
  m_Q.SetRXYZ(1,0,0,0);
  
  m_IsRotated = false;

  m_Empty = true;
  
}


////////////////////////////////////////////////////////////////////////////////


void NOrientation::SetRotation(double x, double y, double z) 
{ 
  // Set the rotation as rotation around x, then y, then z

  m_Empty = false; 
  m_IsRotated = true;

  // Set the rotation of this volume
  // x,y,z are the counterclockwise rotations around x, y and z in rad
  // 
  // Then call the standard SetRotation() which handles all IDs

  double sinx, siny, sinz, cosx, cosy, cosz;
  //double q0, q1, q2, q3;
  sinx = sin(x);
  siny = sin(y);
  sinz = sin(z);
  cosx = cos(x);
  cosy = cos(y);
  cosz = cos(z);
  
  m_Rotation(0,0) =   cosz*cosy;
  m_Rotation(1,0) = - sinz*cosx + cosz*siny*sinx;
  m_Rotation(2,0) =   sinz*sinx + cosz*siny*cosx;
 
  m_Rotation(0,1) =   sinz*cosy;
  m_Rotation(1,1) =   cosz*cosx + sinz*siny*sinx;
  m_Rotation(2,1) = - cosz*sinx + sinz*siny*cosx;

  m_Rotation(0,2) = - siny;
  m_Rotation(1,2) =   cosy*sinx;
  m_Rotation(2,2) =   cosy*cosx;
  
  m_Q.SetDCM(MVector(cosz*cosy, -sinz*cosx + cosz*siny*sinx,sinz*sinx + cosz*siny*cosx),
			 MVector(sinz*cosy, cosz*cosx + sinz*siny*sinx,- cosz*sinx + sinz*siny*cosx),
			 MVector(- siny, cosy*sinx, cosy*cosx));
	
}

MVector NOrientation::GetEulerAngles()
{

  MVector EulerAngles(atan2(m_Rotation(1,2),m_Rotation(2,2)),-asin(m_Rotation(0,2)), atan2(m_Rotation(0,1),m_Rotation(0,0)));
  return EulerAngles;
}


////////////////////////////////////////////////////////////////////////////////


void NOrientation::SetRotation(double x, double y, double z, double r)
{
  //! Set the rotation as a quaternion

  m_Empty = false; 
  m_IsRotated = true;

  NQuaternion Q(r, x, y, z);
  m_Q=Q;

  MVector X(1, 0, 0);
  Q.Rotate(X);
  MVector Y(0, 1, 0);
  Q.Rotate(Y);
  MVector Z(0, 0, 1);
  Q.Rotate(Z);

  m_Rotation(0,0) = X.X();
  m_Rotation(1,0) = Y.X();
  m_Rotation(2,0) = Z.X();
 
  m_Rotation(0,1) = X.Y();
  m_Rotation(1,1) = Y.Y();
  m_Rotation(2,1) = Z.Y();

  m_Rotation(0,2) = X.Z();
  m_Rotation(1,2) = Y.Z();
  m_Rotation(2,2) = Z.Z();
}

////////////////////////////////////////////////////////////////////////////////

void NOrientation::SetRotation(TMatrixD Rotation) 
{ 
	m_Empty = false; 
	m_IsRotated = true; 
	m_Rotation = Rotation; 
	
	MVector x(m_Rotation(0,0), m_Rotation(1,0), m_Rotation(2,0));
	MVector y(m_Rotation(0,1), m_Rotation(1,1), m_Rotation(2,1));
	MVector z(m_Rotation(0,2), m_Rotation(1,2), m_Rotation(2,2));
	
    m_Q.SetDCM(x,y,z);
}


////////////////////////////////////////////////////////////////////////////////


void NOrientation::SetRotation(NQuaternion Q)
{
  //! Set the rotation as a quaternion
  m_Empty = false; 
  m_IsRotated = true;

  m_Q=Q;

  MVector X(1, 0, 0);
  Q.Rotate(X);
  MVector Y(0, 1, 0);
  Q.Rotate(Y);
  MVector Z(0, 0, 1);
  Q.Rotate(Z);

  m_Rotation(0,0) = X.X();
  m_Rotation(1,0) = Y.X();
  m_Rotation(2,0) = Z.X();
 
  m_Rotation(0,1) = X.Y();
  m_Rotation(1,1) = Y.Y();
  m_Rotation(2,1) = Z.Y();

  m_Rotation(0,2) = X.Z();
  m_Rotation(1,2) = Y.Z();
  m_Rotation(2,2) = Z.Z();
}


////////////////////////////////////////////////////////////////////////////////


void NOrientation::SetRotation(MVector x, MVector y, MVector z)
{

  m_Empty = false; 
  m_IsRotated = true;

  x.Unitize();
  y.Unitize();
  z.Unitize();
  // Make rotation matrix on the basis of 3 vectors x, y, z
  m_Rotation(0,0) = x[0];
  m_Rotation(1,0) = x[1];
  m_Rotation(2,0) = x[2];

  m_Rotation(0,1) = y[0];
  m_Rotation(1,1) = y[1];
  m_Rotation(2,1) = y[2];

  m_Rotation(0,2) = z[0];
  m_Rotation(1,2) = z[1];
  m_Rotation(2,2) = z[2];
  
  m_Q.SetDCM(x,y,z);
}

////////////////////////////////////////////////////////////////////////////////


NOrientation NOrientation::operator*(const NOrientation& O) const
{
  //! Multiply two orientations --- this does NOT modify this orientation

  NOrientation Combined;

  // Copy and paste from MEGAlib::MDVolume::RemoveVirtualVolumes
  Combined.m_Rotation = O.m_Rotation * m_Rotation;

  Combined.m_Q = m_Q*O.m_Q;
  
  //TMatrixD Inv = m_Rotation; 
  //Inv.Invert();
  Combined.m_Translation = m_Translation + m_Q.Rotation(O.m_Translation);
  
  Combined.m_Empty = false; 
  Combined.m_IsRotated = true;

  return Combined;
}

    

////////////////////////////////////////////////////////////////////////////////


void NOrientation::SetInterpolated(const NOrientation& A, const NOrientation& B, double Fraction)
{
  //! Set from a linear interpolation between the two values
  //! Fraction needs to be between ]0..1[

  SetTranslation(A.m_Translation + Fraction*(B.m_Translation - A.m_Translation));
  m_Q.SetInterpolated(A.m_Q, B.m_Q, Fraction);
  SetRotation(m_Q);

  // Set rest is set automatically by the called functions
}


////////////////////////////////////////////////////////////////////////////////


void NOrientation::TransformIn(NPhoton& Photon) const
{
  // Translate and rotate a photon into this coordinate system

  MVector Position = Photon.GetPosition();
  MVector Direction = Photon.GetDirection();
  MVector Polarization = Photon.GetPolarization();
  
  // Translate:
  //Direction -= m_Translation;
  //Polarization -= m_Translation;
  Position -= m_Translation;

  // Rotate:
  if (m_IsRotated == true) {
    //Position = m_Rotation * Position;
    //Direction = m_Rotation * Direction;
    //Polarization = m_Rotation * Polarization;

    Position = m_Q.Invert().Rotation(Position);  
    Direction = m_Q.Invert().Rotation(Direction);
    Polarization = m_Q.Invert().Rotation(Polarization);
  }
  
  Photon.SetPosition(Position);
  Photon.SetDirection(Direction);
  Photon.SetPolarization(Polarization);
}


////////////////////////////////////////////////////////////////////////////////



void NOrientation::TransformOut(NPhoton& Photon) const
{
  // Translate and rotate a photon into this coordinate system

  MVector Position = Photon.GetPosition();
  MVector Direction = Photon.GetDirection();
  MVector Polarization = Photon.GetPolarization();


  // Rotate:
  if (m_IsRotated == true) {
    //TMatrixD Inverted = m_Rotation; 
    //Inverted.Invert();
    
    //Position = Inverted * Position;
    //Direction = Inverted * Direction;
    //Polarization = Inverted * Polarization;
    
    Position = m_Q.Rotation(Position);   // again something odd is going on
    Direction = m_Q.Rotation(Direction);
    Polarization = m_Q.Rotation(Polarization);
   }

  // Translate:
  Position += m_Translation;
  //Direction += m_Translation;
  //Polarization += m_Translation;

  Photon.SetPosition(Position);
  Photon.SetDirection(Direction);
  Photon.SetPolarization(Polarization);
}
  

////////////////////////////////////////////////////////////////////////////////


void NOrientation::TransformIn(MVector& Position) const
{
  //! Translate and rotate a vector into this coordinate system
  // Translate:
  Position -= m_Translation;

  // Rotate:
  if (m_IsRotated == true) {
    //Position = m_Rotation * Position;
    Position = m_Q.Invert().Rotation(Position);
	}
}
   

////////////////////////////////////////////////////////////////////////////////



void NOrientation::TransformOut(MVector& Position) const
{
  //! Translate and rotate a vector into this coordinate system

  // Rotate
  if (m_IsRotated == true) {
    Position = m_Q.Rotation(Position);
  }  

  /*
  // Rotate:
  if (m_IsRotated == true) {
    TMatrixD Inverted = m_Rotation;
    Inverted.Invert();

    Position = Inverted * Position;
  }
  */

  // Translate:
  Position += m_Translation;
}
   

////////////////////////////////////////////////////////////////////////////////


void NOrientation::TransformIn(NQuaternion& Q) const
{
  //! Rotate a quaternion Q with m_Q and express in m_Q frame

  Q = Q*m_Q;
   
}


////////////////////////////////////////////////////////////////////////////////


void NOrientation::TransformOut(NQuaternion& Q) const
{
  //! Rotate m_Q with Q expressed in Q frame. 
  //  There is no translation
  //cout<<"m_Q "<<endl; 
  //cout<<m_Q.fRealPart<<" "<<m_Q.fVectorPart(0)<<" "<<m_Q.fVectorPart(1)<<" "<<m_Q.fVectorPart(2)<<endl;
  //cout<<Q.fRealPart<<" "<<Q.fVectorPart(0)<<" "<<Q.fVectorPart(1)<<" "<<Q.fVectorPart(2)<<endl;

  Q = m_Q*Q;

}


////////////////////////////////////////////////////////////////////////////////


TString NOrientation::ToString() const
{
  //! Print the content of this class as a string

  ostringstream Out;

  Out<<"Translation: "<<m_Translation<<endl; 
  Out<<"Rotation:    RM=("<<m_Rotation(0,0)<<", "<<m_Rotation(0,1)<<", "<<m_Rotation(0,2)
    <<", "<<m_Rotation(1,0)<<", "<<m_Rotation(1,1)<<", "<<m_Rotation(1,2)
    <<", "<<m_Rotation(2,0)<<", "<<m_Rotation(2,1)<<", "<<m_Rotation(2,2)
    <<"), Q=("<<m_Q(0)<<", "<<m_Q(1)<<", "<<m_Q(2)<<", "<<m_Q(3)<<")"<<endl;

  return Out.str().c_str();
}

  
////////////////////////////////////////////////////////////////////////////////


bool NOrientation::Stream(ofstream& S)
{
  //! Stream the content to an ASCII file 

  S<<m_Translation[0]<<" "<<m_Translation[1]<<" "<<m_Translation[2]<<" ";
  S<<m_Q(0)<<" "<<m_Q(1)<<" "<<m_Q(2)<<" "<<m_Q(3)<<"  ";

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NOrientation::Parse(TString& Line, int StartToken)
{
  //! Stream the content from a line of an ASCII file starting at Token StartToken

  MTokenizer T;
  T.Analyze(Line);
  
  if (T.GetNTokens() <= StartToken+6) {
    merr<<"Not enough tokens in string for parsing!"<<show;
  }
  
  SetTranslation(T.GetTokenAtAsDouble(StartToken), T.GetTokenAtAsDouble(StartToken+1), T.GetTokenAtAsDouble(StartToken+2));
  SetRotation(T.GetTokenAtAsDouble(StartToken+3), T.GetTokenAtAsDouble(StartToken+4), T.GetTokenAtAsDouble(StartToken+5), T.GetTokenAtAsDouble(StartToken+6));

  return true;
}


////////////////////////////////////////////////////////////////////////////////


std::ostream& operator<<(std::ostream& os, const NOrientation& O)
{
  os.precision(12);
  os<<"T: "<<O.GetTranslation()<<"  Q: "<<O.GetRotationQuaternion();
  return os;
}


// NOrientation.cxx: the end...
////////////////////////////////////////////////////////////////////////////////


