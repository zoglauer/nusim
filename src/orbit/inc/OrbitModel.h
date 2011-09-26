/* H+
 *      Title     : OrbitModel.h
 *      Author    : Bryce A. Roberts
 *      Date      : 24 October 1998
 *      Synopsis  : 
 *      SCCS      : %W% %G%
 *      Revisions :
 *      mm/dd/yy name   description
 * H-
 */

#ifndef _ORBITMODEL_H
#define _ORBITMODEL_H

#include <GUtil.h>
#include <TmTcTime.h>
#include <Dirvector.h>
#include <StateVector.h>

namespace vecutil {

class OrbitModel {
protected:
  bool boolIsValid;

public:
  /************************************************************
  ** Constructors
  ************************************************************/
  OrbitModel(void);
  virtual ~OrbitModel(void);

  /************************************************************
  ** Virtual functions
  ************************************************************/
  // read in the information that populates the orbit model
  virtual bool read(const string& fileName, string& errorString) = 0;

  // do any time invariant calculations once the orbit model parameters
  // are populated
  virtual bool init(void) = 0;

  // get state vector information at an instant in time
  virtual StateVector getStateVector(const tmtc::TmTcTime& st) const = 0;

  // get orbital parameter information
  virtual tmtc::TmTcTime getEpochTime(void) const = 0;

  bool isValid(void) const;

  operator bool() const;
  };

}

#endif

