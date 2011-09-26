/* H+
 *
 *      Title:     GUtil.h
 *      Author:    Bryce A. Roberts
 *      Date:      Thu Aug  4 09:56:51 PDT 2005
 *      Synopsis:  
 *      Revisions:
 *      mm/dd/yy        name    description
 *
 * H-                                           
 */

#ifndef _GUtil_H
#define _GUtil_H



// include some very useful header files here
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <typeinfo>
#include <string.h>

#include <GUException.h>
#include <GUStringvec.h>

#include <math.h>
#include <strings.h>


/// Return the sign of any datatype that can be comared to 0
#define SIGN(x)     ((x)>=0 ? ((x)>0 ? 1:0) : -1)

/// a standard size to allocate for instances of char[]'s
#define MAXSTRING   (2048)

/// handy macros for dealing with angles
#ifndef PI
#define PI (3.141592653589793)
#endif

#ifndef TWOPI
#define TWOPI (2.0*PI)
#endif

#define PIBY2 (PI/2.0)

#define RAD2DEG(x) (((x)*180.0)/PI)
#define DEG2RAD(x) ((PI/180.0)*(x))

// use std namespace since it's so bloody common
using namespace std;

#include <boost/lexical_cast.hpp>

#define GU_DEBUG(x) (genutil::GUDebugPrivate(__FILE__, __LINE__, x))
#define GU_ENTER(x) (genutil::GUEnterPrivate(__FILE__, __LINE__, x))
#define GU_EXIT(x)  (genutil::GUExitPrivate(__FILE__, __LINE__, x))


static bool gutilDebugEnable = false;

/// A namespace for truly miscellaneous functions and classes
namespace genutil {
  template<typename __T>
  void GUDebugPrivate(const string& fileName,
		      int lineNumber,
		      __T obj) {
    // return if debugging not enabled
    if (!gutilDebugEnable)
      return;
    
    stringstream ssBase;
    GUStringvec svfile; svfile.setInputTokenSeparator("/"); fileName >> svfile;
    ssBase << "[" << svfile.back() << ", line " << lineNumber << "]";

    stringstream ss; ss << obj;
    GUStringvec sv; sv.setInputTokenSeparator("\n"); ss.str() >> sv;

    for (GUStringvec::const_iterator i=sv.begin(); i!=sv.end(); i++)
      cerr << ssBase.str() << " " << (*i) << endl;
    }

  void GUEnterPrivate(const string& fileName,
		      int lineNumber,
		      const string& funcName);

  void GUExitPrivate(const string& fileName,
		     int lineNumber,
		     const string& funcName);

  /// \brief Find and replace a substring in a string, a la 'sed'.
  /// \param arg the original string
  /// \param find the exact substring to be replaced
  /// \param replace the replacement for said substring
  /// \return the modified string
  string sed(const string& arg, const string& find, const string& replace);

  /// \brief Get an environment variable with NULL protection
  /// \param envVar the environment variable to be retrieved
  /// \return a string with the contents of the environment variable,
  /// or empty string if environment variable not defined
  string getEnv(const string& envVar);

  /// \brief Get the hostname from environment variable and return as
  /// an std::string
  /// \return the value of the environment variable, or "" if the
  /// environment variable wasn't specified
  string getHost(void);

  /// \brief Get user name from environment variable and return as an
  /// std::string 
  /// \return the value of the environment variable, or ""
  /// if the environment variable wasn't specified
  string getUser(void);

  /// \brief Get the system time
  /// \return the time in UTC in UNIX seconds
  double getTime(void);

  /// \brief Convert a string to all lowercase.
  /// \param arg the string to be converted to lower case
  /// \return a copy of the input argument, in all lowercase
  string stolower(const string& arg);

  /// \brief Convert a string to all uppercase.
  /// \param arg the string to be converted to upper case
  /// \return a copy of the input argument, in all uppercase
  string stoupper(const string& arg);
  
  /// \brief tests whether a string is purely numeric
  /// \param arg the string to be tested
  /// \param the allowed 'numeric' characters, in addition to 0-9
  bool isNumeric(const string& arg, string allowed="");

  /// \brief tests whether a string has only printing ASCII characters
  /// \param arg the string to be tested
  bool isAscii(const string& arg);

  /// \brief Generate a pseudo-random number in the range [0, 1)
  /// \return the random number
  double getRandomNumber(void);

  /// \brief Trim off leading and trailing whitespace.
  /// \param str the string to be depadded
  /// \return a copy of the input argument, with leading and trailing
  /// instances of ' ' and '\\t' stripped off.  This may well result in
  /// an empty string ("")
  string depad(string str);

  GUStringvec depad(const GUStringvec& sv);

  /// \brief Trim everything from a string following/including a
  /// comment character.
  /// \param str the input string with possible comments
  /// \param comment the list of characters that should be considered
  /// comment separators (typically ; or #)
  /// \return a copy of the input string with comments removed.  This may
  /// well result in an empty string ("")
  string decomment(const string& str, const string& comment);
  
  /// \brief Scan a directory and return of the GUStringvec of the
  /// non-directory entries
  GUStringvec lsFiles(string path);

  /// \brief Scan a directory and return of the GUStringvec of all
  /// the files, continuing to the lowest directory
  GUStringvec lsAllFiles(string path);

  /// \brief Get a time string in the common UNIX format
  /// \return a string containing the time
  string getTimeString(void);

  /// \brief Return true if the named file exists (as a regular file)
  bool statFile(const string& fileName);

  /// \brief Return the modification time of the file in UNIX seconds, 
  /// or -1 if the file didn't exist or couldn't be stat'd
  double fileModificationTime(const string& fileName);

  /// \brief Return the access time of the file in UNIX seconds, 
  /// or -1 if the file didn't exist or couldn't be stat'd
  double fileAccessTime(const string& fileName);

  /// \brief Return true if the named directory exists (as a directory)
  bool statDir(const string& dirName);

  /// \brief Return the size of the named file, or zero if the 
  /// file doesn't exist
  off_t fileSize(const string& fileName);

  /// \brief rename file or directory
  void renamePath(const string& oldfile, const string& newfile);

  /// \brief remove file
  void removePath(const string& fileName);

  /// \brief fork and start child process as a true daemon
  void daemonize(void);

  /// \brief a quick diff function, which returns trie if files are identical,
  /// false if not
  bool quickDiff(const string& one, const string& two);

  /// \brief copy a file 
  void copyFile(const string& fromFile, const string& toFile);

  /// \brief write a standardized lock file at a given filename
  void writeLock(const string& fileName);

  /// \brief check if a lock exists other than for this user/host/pid
  bool checkLock(const string& fileName);

  /// \brief read the parameters from a lock file
  bool readLock(const string& fileName,
		string& user,
		string& host,
		double& tLock,
		pid_t& pid);
  }

#endif

/* added comment */
