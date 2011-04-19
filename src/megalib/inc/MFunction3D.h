/*
 * MFunction3D.h
 *
 * Copyright (C) 2008-2010 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MFunction3D__
#define __MFunction3D__


////////////////////////////////////////////////////////////////////////////////


// ROOT libs:
#include <TROOT.h>
#include <TSpline.h>

// MEGAlib libs:
#include "MGlobal.h"
#include "MParser.h"

// Standard libs:
#include <vector>
using std::vector;

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MFunction3D
{
  // public interface:
 public:
  //! Default constructor
  MFunction3D();
  //! Copy constructor
  MFunction3D(const MFunction3D& Function3D);
  //! Default destructor
  virtual ~MFunction3D();

  //! Default assignment constructor
  const MFunction3D& operator=(const MFunction3D& F);

  //! Set the basic data, load the file and parse it
  virtual bool Set(const TString FileName,
           const TString KeyWord,
           const unsigned int InterpolationType = c_InterpolationLinear);

  //! Set the basic data from four vectors
  virtual bool Set(const vector<double>& X, const vector<double>& Y, const vector<double>& Z, const vector<double>& Values, unsigned int InterpolationType = c_InterpolationLinear);

  //! Multiply the x-axis by some value
  void ScaleX(double Scaler);

  //! Scale the x-axis to it's new min and max
  void RescaleX(double XMin, double XMax);

  //! Multiply the y-axis by some value
  void ScaleY(double Scaler);

  //! Scale the x-axis to it's new min and max
  void RescaleY(double YMin, double YMax);

  //! Multiply the z-axis by some value
  void ScaleZ(double Scaler);

  //! Scale the x-axis to it's new min and max
  void RescaleZ(double ZMin, double ZMax);

  //! Invert the z-axis content
  void InvertZ();

  //! Scale the value axis by some value
  void ScaleV(double Scaler);

  //! Evaluate the data for a specific x value 
  double Eval(double x, double y, double z) const;

  //! Return random numbers x, y distributed as the underlying function
  virtual void GetRandom(double& x, double& y, double& z);

  //! Get the minimum x-value
  double GetXMin() const;
  //! Get the maximum x-value
  double GetXMax() const;
  //! Get the minimum y-value
  double GetYMin() const;
  //! Get the maximum y-value
  double GetYMax() const;
  //! Get the minimum z-value
  double GetZMin() const;
  //! Get the maximum z-value
  double GetZMax() const;
  //! Get the minimum value
  double GetVMin() const;
  //! Get the maximum value
  double GetVMax() const;

  //! ID representing an unknown interpolation
  static const unsigned int c_InterpolationUnknown;
  //! ID representing a constant interpolation
  static const unsigned int c_InterpolationConstant;
  //! ID representing a linear interpolation (default)
  static const unsigned int c_InterpolationLinear;

  //! Plot the function in a Canvas (diagnostics only)
  virtual void Plot();

  // protected methods:
 protected:

  // private methods:
 private:

  // protected members:
 protected:
  //! Interpolation method used
  unsigned int m_InterpolationType;

  //! The x-axis data
  vector<double> m_X;
  //! The y-axis data
  vector<double> m_Y;
  //! The z-axis data
  vector<double> m_Z;
  //! The value-axis data
  vector<double> m_V;

  //! For random number generation store the maximum
  double m_Maximum;

  // private members:
 private:
  

#ifdef ___CINT___
 public:
  ClassDef(MFunction3D, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
