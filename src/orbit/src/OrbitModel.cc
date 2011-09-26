/* H+
 *      Title     : OrbitModel.cc
 *      Author    : Bryce A. Roberts
 *      Date      : 24 October 1998
 *      Synopsis  : 
 *      SCCS      : %W% %G%
 *      Revisions :
 *      mm/dd/yy name   description
 * H-
 */

#include "OrbitModel.h"

/************************************************************
** Constructors
************************************************************/
vecutil::OrbitModel::OrbitModel(void) { ; }
vecutil::OrbitModel::~OrbitModel(void) { ; }

bool vecutil::OrbitModel::isValid(void) const { return(boolIsValid); }

vecutil::OrbitModel::operator bool() const { return(isValid()); }

