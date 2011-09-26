/* H+
 *
 *      Title:     GUException.h
 *      Author:    Bryce A. Roberts
 *      Date:      Tue Aug  9 21:28:33 PDT 2005
 *      Synopsis:  class for reporting exceptions
 *      Keywords: 
 *      Revisions:
 *      mm/dd/yy        name    description
 *
 * H-                                           
 */

#ifndef _GUException_H
#define _GUException_H


#include <string>
#include <list>


using namespace std;


#define GU_THROW(_fcn_, _msg_, _data_) \
  throw(genutil::GUException(_fcn_, genutil::GUException::ERROR, \
  __FILE__, __LINE__, _msg_, _data_));

#define GU_ADD(_e_, _fcn_, _msg_, _data_) \
  _e_.add(genutil::GUException(_fcn_, GUException::ERROR, \
  __FILE__, __LINE__, _msg_, _data_)); 

#define GU_ADD_THROW(_e_, _fcn_, _msg_, _data_) \
  GU_ADD(_e_, _fcn_, _msg_, _data_); throw(_e_);

#define GU_CATCH_ADD_THROW(_e_, _fcn_, _msg_, _data_) \
  catch (GUException& e) { GU_ADD_THROW(_e_, _fcn_, _msg_, _data_) }


namespace genutil {
  /*! \brief Implements a generic exception class which can store
    messages from multiple exception levels
   */
class GUException {
public:
  /// levels of exception severity
  enum sev_enum { ERROR, WARNING, COMMENT };

protected:
  /// A list of subsequent levels of exceptions
  list<GUException> listEx;

  /// The name of the function in which the exception was thrown.
  string thrower;

  /// The severity of the exception.
  sev_enum severity;
  
  /// Filename where the exception was thrown.
  string file; 

  /// Approximate line number where the exception was thrown.
  int line;

  /// The user-specified message.
  string msg;

  /// Optional data to help diagnose the exception.
  string data;

public:
  /// No-arg constructor
  GUException(void) { ; }

  /// Constructor that populates all the useful variables
  GUException(const string& thrower,
	      sev_enum severity,
	      const string& file,
	      int line,
	      const string& msg,
	      string data="");

  /// Virtual destructor
  virtual ~GUException(void) { ; }

  /// (non-pure) virtual function to convert the GUException to a string; 
  /// programmers may reinherit this class and write a new virtual toString
  /// function of their own!
  virtual string toString(int level=0) const;

  /// extract just the exception message string
  string getMessage(void) const;

  /// extract just the program source string
  string getPgmSource(void) const;

  /// get the diagnostic data
  string getDiagData(void) const;

  /// extract a list of subsequent GUExceptions
  list<GUException> getList(void) const;

  /// Explicitly sets the function name that is throwing this exception.
  GUException& setThrower(const string& thrower);
  /// Explicitly sets the severity of this exception.
  GUException& setSeverity(sev_enum severity);
  /// Explicitly sets the filename where this exception was thrown.
  GUException& setFile(const string& file);
  /// Explicitly sets the line number where this exception was thrown.
  GUException& setLine(int line);
  /// Explicitly sets the exception message.
  GUException& setMessage(const string& msg);
  /// Explicitly sets example data (in the form of a string, already formatted
  /// by the thrower) to be included with the exception.
  GUException& setData(const string& data);
  /// Add a higher-level exception to the original exception.
  GUException& add(const GUException& e);
  };

}

#endif
