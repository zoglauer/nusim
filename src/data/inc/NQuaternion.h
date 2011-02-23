/*
 * NQuaternion.h
 *
 * This a modified version of the ROOT TQuaternion class
 * (C) by ROOT team
 *
 */


#ifndef __NQuaternion__
#define __NQuaternion__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <cmath>

// ROOT libs:

// MEGAlib libs:
#include "MVector.h"

// NuSTAR libs:

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////

//! \brief Object describing a quaternion
//! This class represents a quaternoin
//! It is based on the ROOT class TQuaternion (C) by the ROOT team see http://root.cern.ch


class NQuaternion
{
  // public interface:
 public:
  //! Default destructor
  virtual ~NQuaternion();

  //! Converts the DCM into a quaternion.
  void SetDCM(const MVector& Vx, const MVector& Vy, const MVector& Vz);
  //! Set the quaternion from an interpolated quaternion given 2 quaternions
  //! Fraction needs to be between ]0..1[
  void SetInterpolated(NQuaternion Q1, NQuaternion Q2, double Fraction);
  //! Calculate the dot product
  double DotProduct(const NQuaternion& Q2) const;
  //! Normalize the quaternion
  //NQuaternion& Normalize();

   NQuaternion(double real = 0, double X = 0, double Y = 0, double Z = 0);
   NQuaternion(const MVector & vector, double real = 0);

   NQuaternion(const NQuaternion &);
   // The copy constructor.


   double operator () (int) const;
   inline double operator [] (int) const;
   // Get components by index. 0 to 2 = vector part, 3 = real part

   double & operator () (int);
   inline double & operator [] (int);
   // Set components by index. 0 to 2 = vector part, 3 = real part

   inline NQuaternion& SetRXYZ(double r,double x,double y,double z);
   inline NQuaternion& SetRV(double r, MVector& vect);
   // Sets components
   NQuaternion& SetAxisQAngle(MVector& v,double QAngle);
   // Set from vector direction and quaternion angle
   double GetQAngle() const;
   NQuaternion& SetQAngle(double angle);
   // set and get quaternion angle

   // ---------------  real to quaternion algebra
   inline NQuaternion& operator=(double r);
   inline bool operator == (double r) const;
   inline bool operator != (double r) const;
   inline NQuaternion& operator+=(double real);
   inline NQuaternion& operator-=(double real);
   inline NQuaternion& operator*=(double real);
   inline NQuaternion& operator/=(double real);
   NQuaternion operator*(double real) const;
   NQuaternion operator+(double real) const;
   NQuaternion operator-(double real) const;
   NQuaternion operator/(double real) const;

   // ---------------- vector to quaternion algebra
   inline NQuaternion& operator=(const MVector& );
   inline bool operator == (const MVector&) const;
   inline bool operator != (const MVector&) const;
   inline NQuaternion& operator+=(const MVector &vector);
   inline NQuaternion& operator-=(const MVector &vector);
   NQuaternion& MultiplyLeft(const MVector &vector);
   NQuaternion& operator*=(const MVector &vector);
   NQuaternion& DivideLeft(const MVector &vector);
   NQuaternion& operator/=(const MVector &vector);
   NQuaternion operator+(const MVector &vector) const;
   NQuaternion operator-(const MVector &vector) const;
   NQuaternion LeftProduct(const MVector &vector) const;
   NQuaternion operator*(const MVector &vector) const;
   NQuaternion LeftQuotient(const MVector &vector) const;
   NQuaternion operator/(const MVector &vector) const;

   // ----------------- quaternion algebra
   inline NQuaternion& operator=(const NQuaternion& );
   inline bool operator == (const NQuaternion&) const;
   inline bool operator != (const NQuaternion&) const;
   inline NQuaternion& operator+=(const NQuaternion &quaternion);
   inline NQuaternion& operator-=(const NQuaternion &quaternion);
   NQuaternion& MultiplyLeft(const NQuaternion &quaternion);
   NQuaternion& operator*=(const NQuaternion &quaternion);
   NQuaternion& DivideLeft(const NQuaternion &quaternion);
   NQuaternion& operator/=(const NQuaternion &quaternion);
   NQuaternion operator+(const NQuaternion &quaternion) const;
   NQuaternion operator-(const NQuaternion &quaternion) const;
   NQuaternion LeftProduct(const NQuaternion &quaternion) const;
   NQuaternion operator*(const NQuaternion &quaternion) const;
   NQuaternion LeftQuotient(const NQuaternion &quaternion) const;
   NQuaternion operator/(const NQuaternion &quaternion) const;

   // ------------------ general algebra
   inline double Norm() const; // quaternion magnitude
   inline double Norm2() const; // quaternion squared magnitude
   double QMag() const { return Norm(); } // quaternion magnitude
   double QMag2() const { return Norm2(); } // quaternion squared magnitude
   inline NQuaternion& Normalize();  // normalize quaternion
   inline NQuaternion operator - () const; // Unary minus.
   inline NQuaternion Conjugate() const;
   NQuaternion Invert() const;
   void Rotate(MVector& vect) const;
   MVector Rotation(const MVector& vect) const;
   MVector ToEuler() const;
   
   double m_R;          // Real part
   MVector m_V; // vector part

  // protected methods:
 protected:
  //NQuaternion() {};
  //NQuaternion(const NQuaternion& NCTHit) {};

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NQuaternion, 0) // no description
#endif

};

// Very important ;-)
std::ostream& operator<<(std::ostream& os, const NQuaternion& Q);


// getters / setters

inline NQuaternion& NQuaternion::SetRXYZ(double r,double x,double y,double z) {
   m_R = r;
   m_V.SetXYZ(x,y,z);
   return (*this); 
}

inline NQuaternion& NQuaternion::SetRV(double r, MVector& vect) {
   m_R = r;
   m_V= vect;
   return (*this); 
}


inline double & NQuaternion::operator[] (int i)       { return operator()(i); }
inline double   NQuaternion::operator[] (int i) const { return operator()(i); }

// ------------------ real to quaternion algebra

inline bool NQuaternion::operator == (double r) const {
   return (m_V.Mag2()==0 && m_R == r) ? kTRUE : kFALSE;
}

inline bool NQuaternion::operator != (double r) const {
   return (m_V.Mag2()!=0 || m_R != r) ? kTRUE : kFALSE;
}

inline NQuaternion& NQuaternion::operator=(double r) {
   m_R = r;
   m_V.SetXYZ(0,0,0);
   return (*this);
}

inline NQuaternion& NQuaternion::operator+=(double real) {
   m_R += real;
   return (*this);
}

inline NQuaternion& NQuaternion::operator-=(double real) { 
   m_R -= real; 
   return (*this); 
}

inline NQuaternion& NQuaternion::operator*=(double real) { 
   m_R *= real;
   m_V *= real;
   return (*this);
}

inline NQuaternion& NQuaternion::operator/=(double real) { 
   if (real!=0) {
      m_R /= real;
      m_V.SetX(m_V.X()/real); // keep numericaly compliant with operator/(double)
      m_V.SetY(m_V.Y()/real);
      m_V.SetZ(m_V.Z()/real);
   } else {
      Error("operator/=()(double)", "bad value (%f) ignored",real);
   }
   return (*this);
}

NQuaternion operator + (double r, const NQuaternion & q);
NQuaternion operator - (double r, const NQuaternion & q);
NQuaternion operator * (double r, const NQuaternion & q);
NQuaternion operator / (double r, const NQuaternion & q);

// ------------------- vector to quaternion algebra

inline bool NQuaternion::operator == (const MVector& V) const {
   return (m_V == V && m_R == 0) ? kTRUE : kFALSE;
}

inline bool NQuaternion::operator != (const MVector& V) const {
   return (m_V != V || m_R != 0) ? kTRUE : kFALSE;
}

inline NQuaternion& NQuaternion::operator=(const MVector& vect) {
   m_R = 0;
   m_V.SetXYZ(vect.X(),vect.Y(),vect.Z());
   return *this;
}

inline NQuaternion& NQuaternion::operator+=(const MVector &vect) {
   m_V += vect;
   return (*this);
}

inline NQuaternion& NQuaternion::operator-=(const MVector &vect) {
   m_V -= vect;
   return (*this);
}

NQuaternion operator + (const MVector &V, const NQuaternion &Q);
NQuaternion operator - (const MVector &V, const NQuaternion &Q);
NQuaternion operator * (const MVector &V, const NQuaternion &Q);
NQuaternion operator / (const MVector &V, const NQuaternion &Q);

// --------------- quaternion algebra

inline bool NQuaternion::operator == (const NQuaternion& Q) const {
   return (m_V == Q.m_V && m_R == Q.m_R) ? kTRUE : kFALSE;
}

inline bool NQuaternion::operator != (const NQuaternion& Q) const {
   return (m_V != Q.m_V || m_R != Q.m_R) ? kTRUE : kFALSE;
}

inline NQuaternion& NQuaternion::operator=(const NQuaternion& quat) {
   m_R = quat.m_R;
   m_V.SetXYZ(quat.m_V.X(),quat.m_V.Y(),quat.m_V.Z());
   return (*this);
}

inline NQuaternion& NQuaternion::operator+=(const NQuaternion &quaternion) {
   m_V += quaternion.m_V;
   m_R += quaternion.m_R;
   return (*this);
}

inline NQuaternion& NQuaternion::operator-=(const NQuaternion &quaternion) {
   m_V -= quaternion.m_V;
   m_R   -= quaternion.m_R;
   return (*this);
}

inline NQuaternion NQuaternion::operator+(const NQuaternion &quaternion) const {
        
   return NQuaternion(m_V+quaternion.m_V, m_R+quaternion.m_R);
}

inline NQuaternion NQuaternion::operator-(const NQuaternion &quaternion) const {

   return NQuaternion(m_V-quaternion.m_V, m_R-quaternion.m_R);
}

// ---------------- general
inline double NQuaternion::Norm() const {
   return sqrt(Norm2());
}

inline double NQuaternion::Norm2() const {
   return m_R*m_R + m_V.Mag2();
}

inline NQuaternion& NQuaternion::Normalize() {
        
   (*this) /= Norm();
   return (*this);
}

inline NQuaternion NQuaternion::operator - () const {
   return NQuaternion(-m_V,-m_R);
}

inline NQuaternion NQuaternion::Conjugate() const {
   return NQuaternion(-m_V,m_R);
}

#endif


////////////////////////////////////////////////////////////////////////////////
