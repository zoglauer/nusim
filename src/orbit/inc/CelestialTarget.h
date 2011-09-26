/*
 * H+
 *       Title     : CelestialTarget.h
 *       Author    : Bryce A. Roberts
 *       Date      : Tue Apr  5 16:35:36 PDT 2005
 *       Synopsis  : calculates the position of heavenly bodies
 *       Revisions :
 *       mm/dd/yy name   description
 * H-
 */

#ifndef _CELESTIAL_TARGET_H
#define _CELESTIAL_TARGET_H

#include <GUtil.h>
#include <TmTcTime.h>
#include <Dirvector.h>

namespace vecutil {

class CelestialTarget {
protected:
  // utility function: make sure an angle is between [0, TWOPI)
  static double reduceAngle(double ang);

public:
  // get the J2000 ECI unit position vector for the sun
  static Dirvector getSun(const tmtc::TmTcTime& st);

  /// get the angle between the sun and a target
  static double getSunAngle(const tmtc::TmTcTime& st,
			    const Dirvector& dvTarg);

  // get the J2000 ECI unit position vector for the moon
  static Dirvector getMoon(const tmtc::TmTcTime& st);

  /// get the angle between the moon and a target
  static double getMoonAngle(const tmtc::TmTcTime& st,
			     const Dirvector& dvTarg);
  };

}

#endif

