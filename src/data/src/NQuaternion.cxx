/*
 * NQuaternion.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NQuaternion
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NQuaternion.h"

// Standard libs:
#include <cstdlib>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MAssert.h"
#include "MStreams.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NQuaternion)
#endif


////////////////////////////////////////////////////////////////////////////////

NQuaternion::NQuaternion(const NQuaternion& Q) : m_R(Q.m_R), m_V(Q.m_V) 
{
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion::NQuaternion(const MVector& V, double R) : m_R(R), m_V(V)  
{
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion::NQuaternion(double R, double X, double Y, double Z) : m_R(R), m_V(X, Y, Z) 
{
}

////////////////////////////////////////////////////////////////////////////////


NQuaternion::~NQuaternion()
{
  // Delete this instance of NQuaternion
}


////////////////////////////////////////////////////////////////////////////////


double NQuaternion::DotProduct(const NQuaternion& q2) const
{
  // calculates the dot product
  return m_V[0] * q2.m_V[0] + m_V[1] * q2.m_V[1] + m_V[2] * q2.m_V[2] + m_R * q2.m_R;
}
 

////////////////////////////////////////////////////////////////////////////////


void NQuaternion::SetInterpolated(NQuaternion q1, NQuaternion q2, double Fraction)
{
  double angle = q1.DotProduct(q2);
  
  if (angle < 0.0f) {
    q1 *= -1.0f;
    angle *= -1.0f;
  }
  
  double scale;
  double invscale;
  
  if (angle + 1.0f > 0.05f) {
    if (1.0f - angle >= 0.05f) { // spherical interpolation
      const double theta = acosf(angle);
      massert(sinf(theta) != 0);
      const double invsintheta = 1./(sinf(theta));
      scale = sinf(theta * (1.0f-Fraction)) * invsintheta;
      invscale = sinf(theta * Fraction) * invsintheta;
    } else { // linear interploation
      scale = 1.0f - Fraction;
      invscale = Fraction;
    }
  } else {
    q2.SetRXYZ(-q1.m_V[1], q1.m_V[0], -q1.m_R, q1.m_V[2]);  /// Can this be right ??? Andreas
    scale = sinf(c_Pi * (0.5f - Fraction));
    invscale = sinf(c_Pi * Fraction);
  }
  
  *this = (q1*scale) + (q2*invscale);
}
  

////////////////////////////////////////////////////////////////////////////////


void NQuaternion::SetDCM(const MVector& Vx, const MVector& Vy, const MVector& Vz)
{
  // matrix assignment operator

  double m00, m01, m02, m10, m11, m12, m20, m21, m22;
  double X, Y, Z, W;

  m00 = Vx[0];
  m10 = Vx[1];
  m20 = Vx[2];
	
  m01 = Vy[0];
  m11 = Vy[1];
  m21 = Vy[2];
		 
  m02 = Vz[0];
  m12 = Vz[1];
  m22 = Vz[2];

		 
  const double diag = m00 + m11 + m22 + 1;
 
  if( diag > 0.0f ) {
    const double scale = sqrtf(diag) * 2.0f; // get scale from diagonal
      
    // TODO: speed this up
    X = ( m12 - m21) / scale;
    Y = ( m20 - m02) / scale;
    Z = ( m01 - m10) / scale;
    W = 0.25f * scale;
  } else {
    if ( m00 > m11 && m00 > m22) {
      // 1st element of diag is greatest value
      // find scale according to 1st element, and double it
      const double scale = sqrtf( 1.0f + m00 - m11 - m22) * 2.0f;
      
      // TODO: speed this up
      X = 0.25f * scale;
      Y = (m10 + m01) / scale;
      Z = (m02 + m20) / scale;
      W = (m12 - m21) / scale;
    } else if ( m11 > m22) {
      // 2nd element of diag is greatest value
      // find scale according to 2nd element, and double it
      const double scale = sqrtf( 1.0f + m11 - m00 - m22) * 2.0f;
      
      // TODO: speed this up
      X = (m10 + m01 ) / scale;
      Y = 0.25f * scale;
      Z = (m21 + m12 ) / scale;
      W = (m20 - m02 ) / scale;
    } else {
      // 3rd element of diag is greatest value
      // find scale according to 3rd element, and double it
      const double scale = sqrtf( 1.0f + m22 - m00 - m11) * 2.0f;
      
      // TODO: speed this up
      X = (m20 + m02) / scale;
      Y = (m21 + m12) / scale;
      Z = 0.25f * scale;
      W = (m01 - m10) / scale;
    }
  }
  
  
  m_R = W;
  m_V[0] = X;
  m_V[1] = Y;
  m_V[2] = Z;
	
  Normalize();
}


////////////////////////////////////////////////////////////////////////////////



double NQuaternion::operator () (int i) const 
{
   //dereferencing operator const
   switch(i) {
      case 0:
      case 1:
      case 2:
         return m_V[i];
      case 3:
         return m_R;
      default:
         merr<<"bad index ("<<i<<") returning &m_R"<<show;
   }
   return 0.;
}


////////////////////////////////////////////////////////////////////////////////


double& NQuaternion::operator () (int i) 
{
   //dereferencing operator
   switch(i) {
      case 0:
      case 1:
      case 2:
         return m_V[i];
      case 3:
         return m_R;
      default:
        merr<<"bad index ("<<i<<") returning &m_R"<<show;
   }
   return m_R;
}


////////////////////////////////////////////////////////////////////////////////


double NQuaternion::GetQAngle() const 
{
   // Get angle of quaternion (rad)
   // N.B : this angle is half of the corresponding rotation angle

   if (m_R == 0) return 0;
   double denominator = m_V.Mag();
   return atan(denominator/m_R);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion& NQuaternion::SetQAngle(double angle) 
{
   // Set angle of quaternion (rad) - keep quaternion norm
   // N.B : this angle is half of the corresponding rotation angle
 
   double norm = Norm();
   double normSinV = m_V.Mag();
   if (normSinV != 0) m_V *= (sin(angle)*norm/normSinV);
   m_R = cos(angle)*norm;

   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion& NQuaternion::SetAxisQAngle(MVector& v,double QAngle) 
{
   // set quaternion from vector and angle (rad)
   // quaternion is set as unitary 
   // N.B : this angle is half of the corresponding rotation angle
        
   m_V = v;
   double norm = v.Mag();
   if (norm>0) m_V*=(1./norm);
   m_V*=sin(QAngle);
   m_R = cos(QAngle);

   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::operator+(double R) const 
{
   // sum of quaternion with a R

   return NQuaternion(m_V, m_R + R);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::operator-(double R) const 
{
   // substraction of R to quaternion

   return NQuaternion(m_V, m_R - R);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::operator*(double R) const 
{
   // product of quaternion with a R 

   return NQuaternion(m_R*R,m_V.X()*R,m_V.Y()*R,m_V.Z()*R);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::operator/(double R) const 
{
   // division by a R

   if (R !=0 ) {
      return NQuaternion(m_R/R,m_V.X()/R,m_V.Y()/R,m_V.Z()/R);
   } else {
     merr<<"bad value ("<<R<<") ignored"<<endl;
   }

   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion operator+(double r, const NQuaternion& q) 
{ 
  return (q+r); 
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion operator-(double r, const NQuaternion& q) 
{ 
  return (-q+r); 
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion operator*(double r, const NQuaternion& q) 
{ 
  return (q*r); 
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion operator/(double r, const NQuaternion& q) 
{ 
  return (q.Invert()*r); 
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::operator+(const MVector& V) const 
{
   // sum of quaternion with a R

   return NQuaternion(m_V + V, m_R);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::operator-(const MVector& V) const 
{
   // substraction of R to quaternion

   return NQuaternion(m_V - V, m_R);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion& NQuaternion::MultiplyLeft(const MVector& V) 
{
   // left multitplication

   double savedRPart = m_R;
   m_R = - (m_V * V);
   m_V = V.Cross(m_V);
   m_V += (V * savedRPart);

   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion& NQuaternion::operator*=(const MVector& V) 
{
   // right multiplication

   double savedRPart = m_R;
   m_R = -(m_V * V);
   m_V = m_V.Cross(V);
   m_V += (V * savedRPart );

   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::LeftProduct(const MVector& V) const 
{
   // left product

   return NQuaternion(V * m_R + V.Cross(m_V), -(m_V * V));
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::operator*(const MVector& V) const 
{
   // right product

   return NQuaternion(V * m_R + m_V.Cross(V), -(m_V * V));
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion& NQuaternion::DivideLeft(const MVector& V) 
{
   // left division

   double norm2 = V.Mag2();
   MultiplyLeft(V);
   if (norm2 > 0 ) {
      // use (1./nom2) to be numericaly compliant with LeftQuotient(const MVector &)
      (*this) *= -(1./norm2); // minus <- using conjugate of V
   } else {
     merr<<"bad norm2 ("<<norm2<<") ignored"<<endl;
   }
   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion& NQuaternion::operator/=(const MVector& V) 
{
   // right division

   double norm2 = V.Mag2();
   (*this) *= V;
   if (norm2 > 0 ) {
      // use (1./R) to be numericaly compliant with operator/(const MVector&)
      (*this) *= - (1./norm2); // minus <- using conjugate of V
   } else {
     merr<<"bad norm2 ("<<norm2<<") ignored"<<endl;
   }
   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::LeftQuotient(const MVector& V) const 
{
   // left quotient

   double norm2 = V.Mag2();

   if (norm2 > 0) {
      double invNorm2 = 1./norm2;
      return NQuaternion((V * -m_R - V.Cross(m_V))*invNorm2,
                         (m_V * V ) * invNorm2 );
   } else {
     merr<<"bad norm2 ("<<norm2<<") ignored"<<endl;
   }
   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::operator/(const MVector& V) const 
{
   //  right quotient

   double norm2 = V.Mag2();

   if (norm2 > 0) {
      double invNorm2 = 1./norm2;
      return NQuaternion((V * -m_R - m_V.Cross(V)) * invNorm2,
                         (m_V * V) * invNorm2 );
   } else {
     merr<<"bad norm2 ("<<norm2<<") ignored"<<endl;
   }
   return (*this);
}



////////////////////////////////////////////////////////////////////////////////


NQuaternion operator+(const MVector& V, const NQuaternion& Q) 
{ 
  return Q+V; 
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion operator-(const MVector& V, const NQuaternion& Q) 
{ 
  return -Q+V; 
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion operator*(const MVector& V, const NQuaternion& Q) 
{ 
  return Q.LeftProduct(V); 
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion operator/(const MVector& V, const NQuaternion& Q) 
{
   // divide operator

   NQuaternion res(V);
   res /= Q;
   return res;
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion& NQuaternion::operator*=(const NQuaternion& Q) 
{
   // right multiplication

   double saveRP = m_R;
   MVector cross(m_V.Cross(Q.m_V));

   m_R = m_R * Q.m_R - m_V * Q.m_V;

   m_V *= Q.m_R;
   m_V += Q.m_V * saveRP;
   m_V += cross;
   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion& NQuaternion::MultiplyLeft(const NQuaternion& Q) 
{
   // left multiplication

   double saveRP = m_R;
   MVector cross(Q.m_V.Cross(m_V));

   m_R = m_R * Q.m_R - m_V * Q.m_V;

   m_V *= Q.m_R;
   m_V += Q.m_V * saveRP;
   m_V += cross;

   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::LeftProduct(const NQuaternion& Q) const 
{
   // left product

   return NQuaternion( m_V*Q.m_R + Q.m_V*m_R
                                 + Q.m_V.Cross(m_V),
                                   m_R*Q.m_R - m_V*Q.m_V );
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::operator*(const NQuaternion& Q) const 
{
   // right product

   return NQuaternion(m_V*Q.m_R + Q.m_V*m_R
                    + m_V.Cross(Q.m_V),
                      m_R*Q.m_R - m_V*Q.m_V );
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion& NQuaternion::DivideLeft(const NQuaternion& Q) 
{
   // left division

   double norm2 = Q.Norm2();

   if (norm2 > 0 ) {
      MultiplyLeft(Q.Conjugate());
      (*this) *= (1./norm2);
   } else {
     merr<<"bad norm2 ("<<norm2<<") ignored"<<endl;
   }
   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion& NQuaternion::operator/=(const NQuaternion& Q) 
{
   // right division

   double norm2 = Q.Norm2();

   if (norm2 > 0 ) {
      (*this) *= Q.Conjugate();
      // use (1./norm2) top be numericaly compliant with operator/(const NQ&)
      (*this) *= (1./norm2);
   } else {
     merr<<"bad norm2 ("<<norm2<<") ignored"<<endl;
   }
   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::LeftQuotient(const NQuaternion& Q) const 
{
   // left quotient

   double norm2 = Q.Norm2();

   if (norm2 > 0 ) {
      double invNorm2 = 1./norm2;
      return NQuaternion(
             (m_V*Q.m_R - Q.m_V*m_R
                        - Q.m_V.Cross(m_V)) * invNorm2,
                        (m_R*Q.m_R + m_V*Q.m_V)*invNorm2 );
   } else {
     merr<<"bad norm2 ("<<norm2<<") ignored"<<endl;
   }
   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::operator/(const NQuaternion& Q) const 
{
   // right quotient

   double norm2 = Q.Norm2();

   if (norm2 > 0 ) {
      double invNorm2 = 1./norm2;
      return NQuaternion(
             (m_V*Q.m_R - Q.m_V*m_R
                        - m_V.Cross(Q.m_V)) * invNorm2,
                         (m_R*Q.m_R + m_V*Q.m_V) * invNorm2 );
   } else {
     merr<<"bad norm2 ("<<norm2<<") ignored"<<endl;
   }
   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


NQuaternion NQuaternion::Invert() const 
{
   // invert

   double norm2 = Norm2();
   if (norm2 > 0 ) {
      double invNorm2 = 1./norm2;
      return NQuaternion(m_V*(-invNorm2), m_R*invNorm2 );
   } else {
     merr<<"bad norm2 ("<<norm2<<") ignored"<<endl;
   }
   return (*this);
}


////////////////////////////////////////////////////////////////////////////////


void NQuaternion::Rotate(MVector& V) const 
{
   // rotate V by current quaternion

   V = Rotation(V);
}


////////////////////////////////////////////////////////////////////////////////


MVector NQuaternion::Rotation(const MVector& V) const 
{
   // rotation of V by current quaternion

   // Vres = (*this) * V * (this->Invert());
   double norm2 = Norm2();

   if (norm2>0) {
      NQuaternion quat(*this);
	  quat *= V;

      // only compute V part : (R part has to be 0 ) : 
      // V [ quat * ( this->Conjugate() ) ] = quat.m_R * -this->m_V
      // + this->m_R * quat.m_V
      // + quat.m_V X (-this->m_V)
      MVector cross(m_V.Cross(quat.m_V));
      quat.m_V *=  m_R;
      quat.m_V -= m_V * quat.m_R;
      quat.m_V += cross;

      return quat.m_V*(1./norm2);
   } else {
     merr<<"Bad norm2 ("<<norm2<<") ignored"<<endl;
     abort();
   }
   return V;
}
  
////////////////////////////////////////////////////////////////////////////////

MVector NQuaternion::ToEuler() const
{
         MVector Euler;

         /*
         double sqw = m_R*m_R;
         double sqx = m_V[0]*m_V[0];
         double sqy = m_V[1]*m_V[1];
         double sqz = m_V[2]*m_V[2];
         */
		 
		 double m11 = 2.0*m_R*m_R + 2.0*m_V[0]*m_V[0] - 1.0;
		 double m12 = 2.0*m_V[0]*m_V[1] + 2.0*m_R*m_V[2];
		 double m13 = 2.0*m_V[0]*m_V[2] + 2.0*m_R*m_V[1];
		 double m23 = 2.0*m_V[1]*m_V[2] + 2.0*m_R*m_V[0];
		 double m33 = 2.0*m_R*m_R + 2.0*m_V[2]*m_V[2];

		 Euler[0] = atan2(m12,m11);
		 Euler[1] = asin(-m13);
		 Euler[2] = atan2(m23,m33);

		 // (rotation about 1st axis) heading = rotation about z-axis 
         //Euler[0] = (atan2(2.0 * (m_V[0]*m_V[1] +m_V[2]*m_R),(sqx - sqy - sqz + sqw)));
 
         // (rotation about 2nd axis) bank = rotation about x-axis
         //Euler[2] = (atan2(2.0 * (m_V[1]*m_V[2] +m_V[0]*m_R),(-sqx - sqy + sqz + sqw)));
 
         // (rotation about 3rd axis) attitude = rotation about y-axis
         //Euler[1] = asin(-2.0f * (m_V[0]*m_V[2] - m_V[1]*m_R));
		
		 return Euler;
 }

////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const NQuaternion& Q)
{
  os<<"(x="<<Q.m_V.X()<<", y="<<Q.m_V.Y()<<", z="<<Q.m_V.Z()<<", r="<<Q.m_R<<")";
  return os;
}


// NQuaternion.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
