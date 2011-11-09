/*
 * NMetrologyData.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NMetrologyData__
#define __NMetrologyData__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include <fstream>
using namespace std;

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NMetrologyDataSet.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing all 3 metrology data sets
//! This class represents all three metrology data sets, MD1, MD2, MD3

class NMetrologyData
{
  // public interface:
 public:
  //! Standard constructor
  NMetrologyData();
  //! Default destructor
  virtual ~NMetrologyData();

  //! Resets all data to default values
  virtual void Clear();

  //! \todo Verify that the metrology data sets are all read out at the same time:

  //! Get the measurement time --- I currently assume all metrology data sets are read out at the same time:
  NTime GetTime() const { return (m_Metrology1.GetTime() < m_Metrology2.GetTime()) ? m_Metrology1.GetTime() : m_Metrology2.GetTime(); }

  //! Return true if the orientation is empty
  bool IsEmpty() const { return (m_Empty && m_Metrology1.IsEmpty() && m_Metrology2.IsEmpty()); }

  //! Set data of the metrology detector MD1
  void SetMetrologyDataSet1(const NMetrologyDataSet& S) { m_Empty = false; m_Metrology1 = S; }
  //! Return a COPY OF the data of metrology detector MD1
  NMetrologyDataSet GetMetrologyDataSet1() const { return m_Metrology1; }
  //! Return a REFERENCE TO the data of metrology detector MD1
  NMetrologyDataSet& GetMetrologyDataSet1ByRef() { return m_Metrology1; }

  //! Set data of the metrology detector MD2
  void SetMetrologyDataSet2(const NMetrologyDataSet& S) { m_Empty = false; m_Metrology2 = S; }
  //! Return a COPY OF the data of metrology detector MD2
  NMetrologyDataSet GetMetrologyDataSet2() const { return m_Metrology2; }
  //! Return a REFERENCE TO the data of metrology detector MD2
  NMetrologyDataSet& GetMetrologyDataSet2ByRef() { return m_Metrology2; }

  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S, int Version);

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line, int Version);

  //! interpolate the metrology data
  void Interpolate(NMetrologyData A, NMetrologyData B, NTime t);

  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;

  //! The data measured by metrology detector MD1
  NMetrologyDataSet m_Metrology1;
  //! The data measured by metrology detector MD2
  NMetrologyDataSet m_Metrology2;


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NMetrologyData, 0) // no description
#endif

};


////////////////////////////////////////////////////////////////////////////////


//! \brief Simple structure allowing to compare NMetrologyData objects by their measurement time.
//! This structure enables comparing (and ultimately sorting) NMetrologyData objects by
//! their measurement time.
struct NMetrologyDataTimeComparator
{
  bool operator() (const NMetrologyData& D, NTime Time) const
  {
    return D.GetTime() < Time;
  }

  bool operator() (NTime Time, const NMetrologyData& D) const
  {
    return Time < D.GetTime();
  }
};


#endif


////////////////////////////////////////////////////////////////////////////////
