/* H+
 *
 *      Title:     TmTcUtil.h
 *      Author:    Bryce A. Roberts
 *      Date:      Tue Aug 23 13:09:14 PDT 2005
 *      Synopsis: 
 *      Keywords: 
 *      Revisions:
 *      mm/dd/yy        name    description
 *
 * H-                                           
 */

#ifndef _TmTcUtil_H
#define _TmTcUtil_H

#include <GUtil.h>
#include <boost/lexical_cast.hpp>


using namespace genutil;


namespace tmtc {
  /// typedef some useful datatypes
  /// (NB: these may need to be adjusted on various architectures)
  typedef unsigned char    tmtc_uchar;
  typedef unsigned short   tmtc_ushort;
  typedef unsigned int     tmtc_ulong;
  typedef unsigned int     tmtc_time40;
  typedef signed char      tmtc_schar;
  typedef signed short     tmtc_sshort;
  typedef signed int       tmtc_slong;
  typedef float            tmtc_float;
  typedef double           tmtc_double;
  typedef tmtc_double      tmtc_time42;

  /// does this string represent a hex number? (i.e. 0x, 0X, x, or X)
  bool isHex(const string& str);

  /// does this string represent a binary number? (i.e. 0b, 0B, b, or B)
  bool isBinary(const string& str);

  /// convert a binary string to an integer
  unsigned long binaryToDecimal(const string& str);
    
  /// convert from string, autodetecting hex
  template<typename __T> __T fromString(const string& str) throw(GUException) {
    /// special logic to convert from binary
    if (isBinary(str)) 
      return((__T)binaryToDecimal(str));
    /// special logic to convert from hex
    else if (isHex(str)) {
      std::stringstream ss; __T t; 
      ss.flags(ios_base::hex);
      if (!(ss << str) || // failure storing string to stringstream
	  !(ss >> t) ||   // failure converting string to target datatype
	  !(ss >> std::ws).eof()) // unfinished conversion
	throw GUException("genutil::fromString()",
			  GUException::ERROR,
			  __FILE__, __LINE__,
			  "error converting string to streamable datatype",
			  str);
      return(t);
      }
    else {
      try {
	return(boost::lexical_cast<__T>(str));
        }
      catch (...) {
	throw GUException("genutil::fromString()",
			  GUException::ERROR,
			  __FILE__, __LINE__,
			  "error converting string to target datatype",
			  str);
        }
      }
    }
  }

#endif
