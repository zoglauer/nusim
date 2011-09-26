/* H+
 *
 *      Title:     GUException.cc
 *      Author:    Bryce A. Roberts
 *      Date:      Tue Aug  9 21:31:38 PDT 2005
 *      Synopsis:  class for reporting GU exceptions
 *      Keywords: 
 *      Revisions:
 *      mm/dd/yy        name    description
 *
 * H-                                           
 */


#include "GUException.h"
#include <iomanip>
#include <sstream>


using namespace genutil;


//////////////////////////////////////////////////////////////////////
// Constructors
//////////////////////////////////////////////////////////////////////
GUException::GUException(const string& thrower,
			 sev_enum severity,
			 const string& file,
			 int line,
			 const string& msg,
			 string data) {
  GUException::thrower=thrower;
  GUException::severity=severity;
  GUException::file=file;
  GUException::line=line;
  GUException::msg=msg;
  GUException::data=data;
  }

//////////////////////////////////////////////////////////////////////
// Accessors
//////////////////////////////////////////////////////////////////////
string GUException::getPgmSource(void) const {
  stringstream ss;

  ss << "pgm source:  "+thrower+" (line " << line << 
    " of file "+file+")";

  return(ss.str());
  }

/// get the diagnostic data
string GUException::getDiagData(void) const {
  stringstream ss;
  if (data.size()>0 && data.size()<80)
    ss << "diag data: '" << data << "'";
  else if (data.size()>=80)
    ss << "diag data: '" << data.substr(0, 80) << "...'";

  return(ss.str());
  }

string GUException::toString(int level) const { 
  stringstream ss;

  ss << "#" << setw(2) << level << ": ";

  switch (severity) {
    case ERROR:   ss << "  error:  "; break;
    case WARNING: ss << "warning:  "; break;
    case COMMENT: ss << "comment:  "; break;
    default:      ss << "unknown:  "; break;
    }
  
  ss << msg;

  ss << "\n  pgm source:  "+thrower+" (line " << line << 
    " of file "+file+")" << endl;
  if (data.size()>0 && data.size()<80)
    ss << "   diag data:  '" << data << "'" << endl;
  else if (data.size()>=80)
    ss << "   diag data:  '" << data.substr(0, 80) << "...'" << endl;
  
  for (list<GUException>::const_iterator i=listEx.begin(); 
       i!=listEx.end(); i++) {
    level++;
    ss << endl << (*i).toString(level);
    }

  return(ss.str());
  }

string GUException::getMessage(void) const { 
  stringstream ss;

  switch (severity) {
    case ERROR:   ss << "error: "; break;
    case WARNING: ss << "warning: "; break;
    case COMMENT: ss << "comment: "; break;
    default:      ss << "unknown: "; break;
    }
  
  ss << msg;

  return(ss.str());
  }


list<GUException> GUException::getList(void) const { return(listEx); }

//////////////////////////////////////////////////////////////////////
// Mutators
//////////////////////////////////////////////////////////////////////
GUException& GUException::setThrower(const string& thrower) { 
  GUException::thrower=thrower; return(*this);
  }
GUException& GUException::setSeverity(sev_enum severity) { 
  GUException::severity=severity; return(*this);
  }
GUException& GUException::setFile(const string& file) { 
  GUException::file=file; return(*this);
  }
GUException& GUException::setLine(int line) { 
  GUException::line=line; return(*this); 
  }
GUException& GUException::setMessage(const string& msg) { 
  GUException::msg=msg; return(*this); 
  }
GUException& GUException::setData(const string& data) { 
  GUException::data=data; return(*this);
  }
GUException& GUException::add(const GUException& e) {
  listEx.push_back(e); return(*this);
  }
