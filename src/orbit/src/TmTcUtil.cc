/* H+
 *
 *      Title:     TmTcUtil.cc
 *      Author:    Bryce A. Roberts
 *      Date:      Tue Aug 23 13:09:14 PDT 2005
 *      Synopsis: 
 *      Keywords: 
 *      Revisions:
 *      mm/dd/yy        name    description
 *
 * H-                                           
 */

#include "TmTcUtil.h"
#include <bitset>

using namespace tmtc;

/// does this string represent a hex number? (i.e. 0x, 0X, x, or X)
bool tmtc::isHex(const string& str) {
  if (str.size()>=1) {
    if (str[0]=='x' || str[0]=='X')
      return(true);
    }
  if (str.size()>=2) {
    if ((str[0]=='0' && str[1]=='x') ||
        (str[0]=='0' && str[1]=='X'))
      return(true);
    }
  
  return(false);
  }

/// does this string represent a binary number? (i.e. 0b, 0B, b, or B)
bool tmtc::isBinary(const string& str) {
  if (str.size()>=1) {
    if (str[0]=='b' || str[0]=='B')
      return(true);
    }
  if (str.size()>=2) {
    if ((str[0]=='0' && str[1]=='b') ||
        (str[0]=='0' && str[1]=='B'))
      return(true);
    }
  
  return(false);
  }

/// convert a binary string to an integer
unsigned long tmtc::binaryToDecimal(const string& str) {
  bitset<32> b;
  stringstream ss; 
  
  // strip off the binary inidcator
  if (str.size()>=1 && stoupper(str.substr(0,1))=="B")
    ss << str.substr(1);
  // strip off the binary inidcator
  if (str.size()>=2 && stoupper(str.substr(0,2))=="0B")
    ss << str.substr(2);
  else
    ss << str;

  ss >> b;

  return(b.to_ulong());
  }
