/* H+
 *      Title     : NoradTle.cc
 *      Author    : Bryce A. Roberts
 *      Date      : 17 May 1998
 *      Synopsis  : propagates a satellite orbit based on a NORAD TLE
 *      SCCS      : %W% %G%
 *      Revisions :
 *      mm/dd/yy name   description
 * H-
 */

#include <NoradTle.h>
#include <GUtil.h>


tmtc::TmTcTime setTle(const string& str) {
  tmtc::TmTcTime t;

  // make sure the input makes sense
  if (str.length()<7 || str[5]!='.')
    return(t);
     
  // parse string
  int year, doy; double dayfrac;
  year=boost::lexical_cast<int>(depad(str.substr(0, 2)));
  if (year<57)
    year+=2000;
  else
    year+=1900;
  doy=boost::lexical_cast<int>(depad(str.substr(2, 3)));
  dayfrac=boost::lexical_cast<double>(depad(str.substr(5)));
  
  // set the year/day-of-year part
  if (!t.setGregorian(year, doy, 0, 0, 0.0)) {
    t.setInvalid();
    return(t);
    }

  t=t+tmtc::TmTcDuration().setDays(dayfrac);

  return(t);
  }


bool vecutil::NoradTle::checksum(const string& str, string& errorString) {
  if (str.size()<68) {
    errorString="invalid length TLE line";
    return(false);
    }

  int sum=0;
  for (int i=0; i<68; i++) {
    if (str[i]>=48 && str[i]<=57)
      sum+=int(str[i]-48);
    else if (str[i]=='-')
      sum++;
    }
      
  if (str.size()==69 && !((sum%10)==int(str[68]-48))) {
    errorString="TLE checksum failed";
    return(false);
    }
  else
    return(true);
  }

bool vecutil::NoradTle::read(const string& fileName, string& errorString) {
  ifstream ifs(fileName.c_str());
  if (ifs.bad() || ifs.fail()) {
    errorString="can't open '"+fileName+"' for reading";
    return(false);
    }
 
  if (!read(ifs, errorString)) {
    ifs.close();
    return(false);
    }

  ifs.close();
  return(true);
  }

bool vecutil::NoradTle::read(istream& is, string& errorString) {
  char buffer[MAXSTRING];
  string str;

  int lineCount=0;
  while (is.getline(buffer, MAXSTRING, '\n')) {
    str=buffer;

    // skip over short lines
    if (str.size()<2)
      continue;
    switch (lineCount) {
    // parse the zeroth line of the TLE
    case 0:
      break;
    // parse the first line of the TLE
    case 1:
      if (!checksum(str, errorString))
	return(false);
      
      // make sure line 1 is really line 1!
      if (buffer[0]!='1') {
	errorString="expected line 1 is not labelled as such";
	return(false);
        }

      // the satellite number (col 3-7)
      try {
	satNum=boost::lexical_cast<int>(depad(str.substr(2, 5)));
        }
      catch (...) {
	errorString="satellite number (line 1) is invalid";
	return(false);
        }

      // the international designation (col 10-17)
      designator=str.substr(9, 8);

      // set the epoch of the elements
      if (!(epoch=setTle(str.substr(18,14)))) {
	errorString="epoch time (line 1) is invalid";
	return(false);
        }
      
      // first time deriv of mean motion (col 34-43)
      try {
	n0dt=boost::lexical_cast<double>(depad(str.substr(33, 10)));
        }
      catch (...) {
	cout << str.substr(33, 10) << endl;
	errorString="first deriv of mean motion (line 1) is invalid";
	return(false);
        }

      // second time deriv of mean motion (col 45-52)
      try {
	n0dt2=boost::lexical_cast<double>(depad(str.substr(33, 10)));
        }
      catch (...) {
	errorString="second deriv of mean motion (line 1) is invalid";
	return(false);
        }

      n0dt*=(TWOPI/(MIN_PER_DAY*MIN_PER_DAY));
      n0dt2*=TWOPI/(MIN_PER_DAY*MIN_PER_DAY*MIN_PER_DAY);

      // bstar drag term (col 54-61)
      double bstarMantissa, bstarExponent;
      try {
	bstarMantissa=
	  boost::lexical_cast<double>(depad("0."+str.substr(54, 5)));
        }
      catch (...) {
	errorString="Bstar mantissa (line 1) is invalid";
	return(false);
        }

      try {
	bstarExponent=boost::lexical_cast<double>(depad(str.substr(59, 2)));
        }
      catch (...) {
	errorString="Bstar exponent (line 1) is invalid";
	return(false);
        }

      bstar=bstarMantissa*pow(10, bstarExponent);

      if (str.substr(53, 1)=="-")
	bstar*=-1;

      // ephemeris type (col 62)
      ephemType=str.substr(62, 1);

      // element set number
      elementNumber=str.substr(64,4);

      break;

    // parse the second line of the TLE
    case 2:
      if (!checksum(str, errorString))
	return(false);

      // make sure line 2 is really line 2!
      if (buffer[0]!='2') {
	errorString="expected line 2 is not labelled as such";
	return(false);
        }

      // inclination (col 9-16)
      try {
	i0=boost::lexical_cast<double>(depad(str.substr(8, 8)));
        }
      catch (...) {
	errorString="inclination is invalid";
	return(false);
        }
      i0=DEG2RAD(i0);
      
      // right ascension of the ascending node (col 18-25)
      try {
	raan=boost::lexical_cast<double>(depad(str.substr(17, 8)));
        }
      catch (...) {
	errorString="RAAN argument is invalid";
	return(false);
        }
      raan=DEG2RAD(raan);

      // eccentricity (col 27-33)
      try {
	e0=boost::lexical_cast<double>(depad("0."+str.substr(26, 7)));
        }
      catch (...) {
	errorString="eccentricity argument is invalid";
	return(false);
        }

      // argument of perigee (col 35-42)
      try {
	w0=boost::lexical_cast<double>(depad(str.substr(34, 8)));
        }
      catch (...) {
	errorString="argument of perigee argument is invalid";
	return(false);
        }
      w0=DEG2RAD(w0);

      // mean anomaly (col 44-51)
      try {
	M0=boost::lexical_cast<double>(depad(str.substr(43, 8)));
        }
      catch (...) {
	errorString="mean anomaly argument is invalid";
	return(false);
        }
      M0=DEG2RAD(M0);

      // mean motion (col 53-63)
      try {
	n0=boost::lexical_cast<double>(depad(str.substr(52, 11)));
        }
      catch (...) {
	errorString="mean motion argument is invalid";
	return(false);
        }
      n0*=(TWOPI/MIN_PER_DAY);

      // orbit number (col 64-68)
      try {
	orbitNumber=boost::lexical_cast<int>(depad(str.substr(63, 5)));
        }
      catch (...) {
	errorString="orbit number argument is invalid";
	return(false);
        }
      goto done;
      break;
      }
    
    lineCount++;
    }

 done:
  if (lineCount==0 && is.eof()) {
    boolIsValid=false;
    return(false);
    }
  else
    boolIsValid=true;

  // do range checking
  if (i0<0 || i0>PI) {
    errorString="inclination argument out of range";
    return(false);
    }
    
  if (raan<0 || raan>TWOPI) {
    errorString="RAAN argument out of range";
    return(false);
    }

  if (e0<0 || e0>=1.0) {
    errorString="eccentricity argument out of range";
    return(false);
    }

  if (w0<0 || w0>TWOPI) {
    errorString="argument of perigee out of range";
    return(false);
    }

  if (M0<0 || M0>TWOPI) {
    errorString="mean anomaly argument out of range";
    return(false);
    }

  if (!init()) {
    errorString="time invariant initializations faild";
    return(false);
    }

  return(true);
  }

// set/get the satellite name
string vecutil::NoradTle::getSatelliteName(void) const { return(satelliteName); }

void vecutil::NoradTle::setSatelliteName(const string& satelliteName) {
  this->satelliteName=satelliteName;
  }

// get the satellite catalog number
unsigned vecutil::NoradTle::getSatNumber(void) const { return(satNum); }

// get international designator
string vecutil::NoradTle::getDesignator(void) const { return(designator); }

// get epoch time
tmtc::TmTcTime vecutil::NoradTle::getEpochTime(void) const { return(epoch); }

// get first derivative of mean motion
double vecutil::NoradTle::getMeanDot(void) const { return(n0dt); }

// get second derivative of mean motion
double vecutil::NoradTle::getMeanDoubleDot(void) const { return(n0dt2); }

// get the Bstar drag term
double vecutil::NoradTle::getBstar(void) const { return(bstar); }

// get the ephemeris type
string vecutil::NoradTle::getEphemerisType(void) const { return(ephemType); }

// get the element set number
string vecutil::NoradTle::getElementNumber(void) const { return(elementNumber); }

// get the inclination
double vecutil::NoradTle::getInclination(void) const { return(i0); }

// get the RAAN
double vecutil::NoradTle::getRAAN(void) const { return(raan); }

// get the eccentricity
double vecutil::NoradTle::getEccentricity(void) const { return(e0); }

// get argument of perigee
double vecutil::NoradTle::getArgumentOfPerigee(void) const { return(w0); }

// get mean anomaly
double vecutil::NoradTle::getMeanAnomaly(void) const { return(M0); }

// get mean motion
double vecutil::NoradTle::getMeanMotion(void) const { return(n0); }

// get the orbit number
int vecutil::NoradTle::getOrbitNumber(void) const { return(orbitNumber); }

