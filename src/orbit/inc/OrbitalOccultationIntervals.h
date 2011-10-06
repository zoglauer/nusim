/* H+
 *
 *  Title        : OrbitalOccultationIntervals.h
 *  Author       : broberts
 *  Created      : Mon Apr 20 20:28:43 2009
 *  Synopsis     : class for calculating occultations of a target
 *
 *  $LastChangedDate: 2009-04-22 08:52:46 -0700 (Wed, 22 Apr 2009) $
 *  $LastChangedRevision: 3262 $
 *  $LastChangedBy: broberts $
 *
 *  Revisions
 *  date   author   comment
 *  ------ -------- -----------------------------------------------------
 *
 * H-                                           
 */

#ifndef _OrbitalOccultationIntervals_H
#define _OrbitalOccultationIntervals_H


#include <GUtil.h>
#include <OrbitalIntervals.h>
#include <Dirvector.h>


namespace vecutil {


class OrbitalOccultationIntervals : public vecutil::OrbitalIntervals {
protected:
  /// target vector
  vecutil::Dirvector dvTarg;

  /// earth limb angle
  double limbAngle;

public:
  /// find occultation status at an instant in time
  int findStatus(const tmtc::TmTcTime& t) const;

  /// Constructor
  OrbitalOccultationIntervals(vecutil::OrbitModel *__om);

  /// set the target direction vector
  void setTarget(const vecutil::Dirvector& dvTarg);

  /// set the limb angle for occultation
  void setLimbAngle(double limbAngle);
  };

}

#endif
