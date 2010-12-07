/*
 * NOrientation.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NOrientation__
#define __NOrientation__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <cmath>

// ROOT libs:
#include <TMatrixD.h>
#include <TQuaternion.h>

// MEGAlib libs:
#include "MVector.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NPhoton.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing an orientation object used for switching from one coordinate system to another
//! This class describes an orientation, consisting of a translation and a rotation (in this case 3x3 matrix). 
//! The class is used to rotate from one coordinate system to another

class NOrientation
{
  // public interface:
 public:
  //! Standard constructor
  NOrientation();
  //! Default destructor
  virtual ~NOrientation();

  //! Resets all data to default values
  virtual void Clear();

  //! Return true if the orientation is empty
  bool IsEmpty() const { return m_Empty; }

  //! Return the translation in mm
  MVector GetTranslation() const { return m_Translation; }
  //! Set the translation in mm
  void SetTranslation(MVector Translation) { m_Empty = false; m_Translation = Translation; }
  //! Set the translation in mm
  void SetTranslation(double x, double y, double z) { m_Empty = false; m_Translation.SetXYZ(x, y, z); }

	//! Return the rotation in matrix form
	TMatrixD GetRotationMatrix() const { return m_Rotation; }
	//! Return the rotation in matrix form
	NQuaternion GetRotationQuaternion() const { return m_Q; }
	//! Return euler angles
	MVector GetEulerAngles();

  //! Set the rotation as Rotation matrix
  void SetRotation(TMatrixD Rotation);
  //! Set the rotation as rotation around x, then y, then z
  void SetRotation(double x, double y = 0.0, double z = 0.0);
  //! Set the rotation as a quaternion
  void SetRotation(double x, double y, double z, double r);
  void SetRotation(NQuaternion Q);
  //! Set rotation as a basis of vectors x, y, z
  void SetRotation(MVector x, MVector y, MVector z);

  //! Multiply two orientations --- this does NOT modify this orientation
  //! Application: Outside = Mother*Daughter
  //! Daughter is an orientation inside mother, mother is an orientation in grandmother
  //! This operation gets the orientation of daughter in the grandmother coordiante system
  NOrientation operator*(const NOrientation& O) const;
  
  //! Set from an interpolation between the two values
  //! Fraction needs to be between ]0..1[
  void SetInterpolated(const NOrientation& A, const NOrientation& B, double Fraction);

  //! Translate and rotate a photon into this coordinate system
  void TransformIn(NPhoton& Photon) const;
  
  //! Translate and rotate a photon into this coordinate system
  void TransformOut(NPhoton& Photon) const;

  
  //! Translate and rotate a vector into this coordinate system
  void TransformIn(MVector& Vector) const;
  
  //! Translate and rotate a vector into this coordinate system
  void TransformOut(MVector& Vector) const;

  
  //! Rotate a quaternion into this coordinate system
  void TransformIn(NQuaternion& Q) const;
  
  //! Rotate a quaternion out of this coordinate system
  void TransformOut(NQuaternion& Q) const;


  //! Print the content of this class as a string
  TString ToString() const;

  
  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S);

  //! Stream the content from a line of an ASCII file starting at Token StartToken
  bool Parse(TString& Line, int StartToken);

  
  // protected methods:
 protected:

  // private methods:
 private:
  


  // protected members:
 protected:


  // private members:
 private:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;

  //! Translation of the corrdinate system relative to the mother coordinate system
  MVector m_Translation;
  //! Rotation of the coordinate system relative to the mother coordinate system as Matrix
  TMatrixD m_Rotation;
  //! Rotation of the coordinate system relative to the mother coordinate system as Quaternion
  NQuaternion m_Q;

  //! True if we really have a rotation
  bool m_IsRotated;

  //! An array to quickly set the rotation matrix to unity
  static double m_Unity[9];

#ifdef ___CINT___
 public:
  ClassDef(NOrientation, 0) // no description
#endif

};

// Very important ;-)
std::ostream& operator<<(std::ostream& os, const NOrientation& Orientation);

#endif


////////////////////////////////////////////////////////////////////////////////
