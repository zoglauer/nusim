/* H+
 *
 *  Title        : OrbitalIntervals.cc
 *  Author       : broberts
 *  Created      : Mon Apr 20 20:28:43 2009
 *  Synopsis     : base class for calculating orbital intervals
 *
 *  $LastChangedDate: 2009-04-22 14:29:04 -0700 (Wed, 22 Apr 2009) $
 *  $LastChangedRevision: 3264 $
 *  $LastChangedBy: broberts $
 *
 *  Revisions
 *  date   author   comment
 *  ------ -------- -----------------------------------------------------
 *
 * H-                                           
 */

#include <OrbitalIntervals.h>
#include <OrbitModel.h>

typedef vecutil::OrbitalIntervals __V;


/// Constructor
vecutil::OrbitalIntervals::OrbitalIntervals(vecutil::OrbitModel *__om) : 
  CalculateIntervals(), om(__om) { }
