/*
 * MResponseMatrix.h
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MResponseMatrix__
#define __MResponseMatrix__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
using namespace std;

// MEGAlib libs:
#include "MGlobal.h"
#include "MFileResponse.h"

// Forward declarations:
class MResponseMatrixO1;

////////////////////////////////////////////////////////////////////////////////


class MResponseMatrix
{
  // public interface:
 public:
  MResponseMatrix();
  MResponseMatrix(TString Name);
  virtual ~MResponseMatrix();

  void SetName(const TString& Name) { m_Name = Name; }
  TString GetName() const { return m_Name; }

  //! Set a hash value --- usually used to verify an outside object is still
  //! identical with that which has been used to create this response 
  void SetHash(const unsigned long& Hash) { m_Hash = Hash; }
  //! Return a hash value --- the has is not created internally, but has to be set
  unsigned long GetHash() const { return m_Hash; }

  unsigned int GetOrder() const { return m_Order; }

  virtual bool Read(TString FileName);
  virtual bool Write(TString FileName, bool Stream = false) = 0;

  virtual unsigned int GetNBins() const = 0;

  virtual float GetAxisContent(unsigned int b, unsigned int order = 0) const = 0;
  virtual vector<float> GetAxis(unsigned int order = 0) const = 0;
  virtual unsigned int GetAxisBins(unsigned int order = 0) const = 0;
  virtual TString GetAxisName(unsigned int order = 0) const = 0;
  virtual float GetAxisMinimum(unsigned int order = 0) const = 0;
  virtual float GetAxisMaximum(unsigned int order = 0) const = 0;
  virtual float GetAxisLowEdge(unsigned int b, unsigned int order = 0) const = 0;
  virtual float GetAxisHighEdge(unsigned int b, unsigned int order = 0) const = 0;

  bool IsIncreasing(vector<float> Axis) const;

  void SetValuesCenteredFlag(bool Centered) { m_ValuesCentered = Centered; }
  bool AreValuesCentered() const { return m_ValuesCentered; }

  virtual float GetMaximum() const = 0;
  virtual float GetMinimum() const = 0;
  virtual float GetSum() const = 0;
  virtual MResponseMatrixO1 GetSumMatrixO1(unsigned int order = 0) const = 0;

  static const float c_ShowX;
  static const float c_ShowY;
  static const float c_ShowZ;
  static const unsigned int c_UnusedAxis;

  // protected methods:
 protected:
  int FindBin(const vector<float>& Array, float Value) const;
  int FindBinCentered(const vector<float>& Array, float Value) const;
  bool AreIncreasing(unsigned int order, 
                     unsigned int a1 = c_UnusedAxis,
                     unsigned int a2 = c_UnusedAxis,
                     unsigned int a3 = c_UnusedAxis,
                     unsigned int a4 = c_UnusedAxis,
                     unsigned int a5 = c_UnusedAxis,
                     unsigned int a6 = c_UnusedAxis,
                     unsigned int a7 = c_UnusedAxis,
                     unsigned int a8 = c_UnusedAxis,
                     unsigned int a9 = c_UnusedAxis,
                     unsigned int a10 = c_UnusedAxis,
                     unsigned int a11 = c_UnusedAxis,
                     unsigned int a12 = c_UnusedAxis,
                     unsigned int a13 = c_UnusedAxis,
                     unsigned int a14 = c_UnusedAxis,
                     unsigned int a15 = c_UnusedAxis,
                     unsigned int a16 = c_UnusedAxis,
                     unsigned int a17 = c_UnusedAxis,
                     unsigned int a18 = c_UnusedAxis) const;

  // private methods:
 private:
  //! Read the class specific info from the file
  virtual bool ReadSpecific(MFileResponse&, const TString&, const int) { return true; };

  // protected members:
 protected:
  static const unsigned int c_SizeLimit;
  static const unsigned int c_Outside;

  //! True if the values correspond to bin centers
  bool m_ValuesCentered;
  //! Name of this response
  TString m_Name;

  //! Order/Dimension of this response 
  unsigned int m_Order;

  //! A hash value --- this value is not calculated but has to be set from outside or read in via file
  unsigned long m_Hash;

  // private members:
 private:

#ifdef ___CINT___
 public:
  ClassDef(MResponseMatrix, 1) // base class for all response matrices
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
