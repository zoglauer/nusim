/* H+
 *
 *  Title        : OrbitalIntervals.h
 *  Author       : broberts
 *  Created      : Mon Apr 20 20:28:43 2009
 *  Synopsis     : base class for calculating orbital intervals
 *
 *  $LastChangedDate: 2009-05-14 08:37:46 -0700 (Thu, 14 May 2009) $
 *  $LastChangedRevision: 3323 $
 *  $LastChangedBy: broberts $
 *
 *  Revisions
 *  date   author   comment
 *  ------ -------- -----------------------------------------------------
 *
 * H-                                           
 */

#ifndef _OrbitalIntervals_H
#define _OrbitalIntervals_H

#include <GUtil.h>
#include <CalculateIntervals.h>
#include <boost/shared_ptr.hpp>


namespace vecutil {

class OrbitModel;

class OrbitalIntervals : public CalculateIntervals {
protected:
  /// orbit model
  OrbitModel *const om;

  /// Constructor
  OrbitalIntervals(OrbitModel *__om);
  };

}

#endif
