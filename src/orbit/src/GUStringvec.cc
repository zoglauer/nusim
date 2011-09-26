/* H+
 *
 *      Title:     GUStringvec.cc
 *      Author:    Bryce A. Roberts
 *      Date:      Fri Aug 12 22:06:43 PDT 2005
 *      Synopsis:  
 *      Revisions:
 *      mm/dd/yy        name    description
 *
 * H-                                           
 */

#include "GUStringvec.h"

//using namespace genutil;


void genutil::GUStringvec::split(string str) {
  enum { SEPARATOR, TOKEN } whichState;

  // are we starting off in a token separator, or in a token?
  whichState = (inputTokenSep.find_first_of(str[0])!=string::npos) ? 
    SEPARATOR : TOKEN;

  bool inQuote=false, escape=false; string temp;
  string::const_iterator i;
  for (i=str.begin(); i<str.end(); i++) {
    // decide if this is white space or a character
    if (inputTokenSep.find_first_of(char(*i))!=string::npos) {
      if (whichState==TOKEN && !inQuote) {
	push_back(temp); temp=""; 
	whichState=SEPARATOR;
        }
      }
    else 
      whichState=TOKEN;

    switch (whichState) {
    case TOKEN:
      // what to do if the character is "
      if ((*i)=='"') {
	// " is not escaped, indicates some kind of quote boundary
	if (!escape) 
	  inQuote=!inQuote;
	// " is escaped, so store like a regular part of the string
	else {  
	  escape=false; temp+=char(*i);
	  }
        }
      else if ((*i)=='\\') escape=true;    // detect quote escape
      else {           // regular character; store
	temp+=char(*i);
	escape=false;
        }
      break;
    case SEPARATOR:
      break;
      }
    }

  // deal with the last token
  if (temp.size())
    push_back(temp);
  }


istream& genutil::GUStringvec::split(istream& is) {
  // get one line from the stream
  string str; getline(is, str, '\n');

  split(str);
  
  return(is);
  }


genutil::GUStringvec::GUStringvec(string s1, string s2, string s3,
				  string s4, string s5, string s6) {
  if (!s1.empty())
    push_back(s1);
  if (!s2.empty())
    push_back(s2);
  if (!s3.empty())
    push_back(s3);
  if (!s4.empty())
    push_back(s4);
  if (!s5.empty())
    push_back(s5);
  if (!s6.empty())
    push_back(s6);
	
  // set default token separators
  inputTokenSep=DEFAULT_INPUT_SEPARATOR; 
  outputTokenSep=DEFAULT_OUTPUT_SEPARATOR;
  }



genutil::GUStringvec& genutil::GUStringvec::setInputTokenSeparator(const string& inputTokenSep) {
  GUStringvec::inputTokenSep=inputTokenSep;
  return(*this);
  }

genutil::GUStringvec& 
genutil::GUStringvec::setOutputTokenSeparator(const string& outputTokenSep) {
  GUStringvec::outputTokenSep=outputTokenSep;
  return(*this);
  }

genutil::GUStringvec& genutil::GUStringvec::clear(void) {
  erase(begin(), end());
  return(*this);
  }

ostream& genutil::operator << (ostream& os, const genutil::GUStringvec& s) {
  for (genutil::GUStringvec::ci i=s.begin(); i!=s.end(); i++) {
    if (i!=s.begin())
      os << s.outputTokenSep;
    os << (*i);
    }

  return(os);
  }


istream& genutil::operator >> (istream& is, genutil::GUStringvec& s) {
  s.split(is);
  return(is);
  }

genutil::GUStringvec& genutil::operator >> (const string& str, genutil::GUStringvec& s) {
  s.split(str);
  return(s);
  }
