/* H+
 *      Title     : SGP4.h
 *      Author    : Bryce A. Roberts
 *      Date      : 17 May 1998
 *      Synopsis  : propagates a satellite orbit based on a NORAD TLE
 *      SCCS      : %W% %G%
 *      Revisions :
 *      mm/dd/yy name   description
 * H-
 */

#ifndef _SGP4_H
#define _SGP4_H

#include <NoradTle.h>

namespace vecutil {

class SGP4 : public NoradTle {
protected:
  tmtc::TmTcTime st2000;

public:
  // perform time invariant calculations only once
  bool init(void);

  StateVector getStateVector(const tmtc::TmTcTime& st) const;
  };

}

#endif
