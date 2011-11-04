/* H+
 *
 *      Title:     GUtil.cc
 *      Author:    Bryce A. Roberts
 *      Date:      Thu Aug  4 09:56:51 PDT 2005
 *      Synopsis:  
 *      Revisions:
 *      mm/dd/yy        name    description
 *
 * H-                                           
 */

#include "GUtil.h"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

void genutil::GUEnterPrivate(const string& fileName,
			     int lineNumber,
			     const string& funcName) {
  GUDebugPrivate(fileName, lineNumber, "Entering "+funcName);
  }

void genutil::GUExitPrivate(const string& fileName,
			    int lineNumber,
			    const string& funcName) {
  GUDebugPrivate(fileName, lineNumber, "Exiting from "+funcName);
  }

string genutil::getEnv(const string& envVar) {
  if (getenv(envVar.c_str()))
    return(getenv(envVar.c_str()));
  else
    return("");
  }

string genutil::getHost(void) {
  /*
  if (getenv("HOST"))
    return(getenv("HOST"));
  else if (getenv("HOSTNAME"))
    return(getenv("HOSTNAME"));
  else
    return("");
  */
  char buffer[MAXSTRING];
  gethostname(buffer, MAXSTRING);
  return(buffer);
  }

string genutil::getUser(void) {
  if (getenv("USER"))
    return(getenv("USER"));
  else if (getenv("LOGNAME"))
    return(getenv("LOGNAME"));
  else
    return("");
  }

double genutil::getTime(void) {
  struct timeval tv;
  (void)gettimeofday(&tv, NULL);
  return(double(tv.tv_sec)+double(tv.tv_usec)/1.0E6);
  }

string genutil::sed(const string& arg, const string& find, 
		    const string& replace) {
  size_t start=0, findSize=find.size(), replaceSize=replace.size();

  string internal=arg;

  while ((start=internal.find(find, start))!=string::npos) {
    internal.replace(start, findSize, replace, 0, replaceSize);
    start+=replaceSize;
    }

  return(internal);
  }

string genutil::stolower(const string& arg) {
  string temp(arg);

  for (unsigned i=0; i<temp.size(); i++)
    temp[i]=tolower(temp[i]);

  return(temp);
  }

string genutil::stoupper(const string& arg) {
  string temp;
  string::const_iterator i;
  for (i=arg.begin(); i<arg.end(); temp+=toupper(*i), i++);
  return(temp);
  }

bool genutil::isNumeric(const string& arg, string allowed) {
  string temp=allowed;
  temp+="0123456789";
  return(arg.find_first_not_of(temp, 0)==string::npos);
  }

/// \brief tests whether a string has only printing ASCII characters
/// \param arg the string to be tested
bool genutil::isAscii(const string& arg) {
  for (string::const_iterator i=arg.begin(); i!=arg.end(); i++) {
    // non-ASCII character detected
    if ((unsigned char)(*i)>=127)
      return(false);
    }

  // made it to the end with no non-ASCII characters
  return(true);
  }

double genutil::getRandomNumber(void) { 
  return(double(rand())/double(RAND_MAX));
  }

string genutil::depad(string str) {
  size_t i1=str.find_first_not_of(" \t"), i2=str.find_last_not_of(" \t");
  
  // trim off leading and trailing whitespace
  if (i1!=string::npos && i2!=string::npos)
    str=str.substr(i1, (i2-i1)+1);
  // string is nothing but spaces, trims to empty
  else
    str="";

  return(str);
  }

genutil::GUStringvec genutil::depad(const genutil::GUStringvec& sv) {
  GUStringvec svnew=sv;
  svnew.erase(svnew.begin(), svnew.end());

  for (GUStringvec::ci i=sv.begin(); i!=sv.end(); i++)
    svnew.push_back(depad(*i));

  return(svnew);
  }



string genutil::decomment(const string& str, const string& comment) {
  size_t i=str.find_first_of(comment);
  if (i!=string::npos)
    return(str.substr(0, i));
  else
    return(str);
  }

/// \brief Scan a directory and return of the GUStringvec of the
/// non-directory entries
genutil::GUStringvec genutil::lsFiles(string path) {
  GUStringvec sv; 
  
  // make the path look pretty
  path=sed(path+"/", "//", "/");

  try {
    // open directory for scanning
    DIR *dp;
    if (!(dp=opendir(path.c_str()))) {
      GUException e("genutil::lsdir()", GUException::ERROR,
                    __FILE__, __LINE__,
                    "couldn't open directory for scanning");
      throw(e);
      }
  
    // scan through directory
    struct dirent *dirp;
    while ((dirp=readdir(dp))!=NULL) {
      // get the full pathname of the file
      string fullPath=path+dirp->d_name;

      // stat the file
      // store string if it's not a directory
      struct stat buf;
      if (stat(fullPath.c_str(), &buf)==0 &&
	  !S_ISDIR(buf.st_mode)) 
        sv.push_back(fullPath);
      }

    (void)closedir(dp);
    }
  catch (GUException e) {
    e.add(GUException("genutil::lsdir()", GUException::ERROR,
                      __FILE__, __LINE__,
                      "failed to make listing of directory '"+
                      path+"'"));
    throw(e);
    }

  return(sv);
  }

/// \brief Scan a directory and return of the GUStringvec of all
/// the files, continuing to the lowest directory
genutil::GUStringvec genutil::lsAllFiles(string path) {
  GUStringvec sv; 
  
  // make the path look pretty
  path=sed(path+"/", "//", "/");

  try {
    // open directory for scanning
    DIR *dp;
    if (!(dp=opendir(path.c_str()))) {
      GUException e("genutil::lsdir()", GUException::ERROR,
                    __FILE__, __LINE__,
                    "couldn't open directory for scanning");
      throw(e);
      }
  
    // scan through directory
    struct dirent *dirp;
    while ((dirp=readdir(dp))!=NULL) {
      // get the full pathname of the file
      string fullPath=path+dirp->d_name;

      // skip .
      if (strncmp(dirp->d_name, ".", 2)==0)
	continue;

      // skip ..
      if (strncmp(dirp->d_name, "..", 2)==0)
	continue;

      // stat the file
      struct stat buf;
      if (stat(fullPath.c_str(), &buf)==0 && !S_ISDIR(buf.st_mode)) 
        sv.push_back(fullPath);
      else if (stat(fullPath.c_str(), &buf)==0 && S_ISDIR(buf.st_mode)) {
	try {
	  GUStringvec _sv=lsAllFiles(fullPath);
	  sv.insert(sv.end(), _sv.begin(), _sv.end());
	}
	catch (...) {}
        }
      }

    (void)closedir(dp);
    }
  catch (GUException e) {
    e.add(GUException("genutil::lsdir()", GUException::ERROR,
                      __FILE__, __LINE__,
                      "failed to make listing of directory '"+
                      path+"'"));
    throw(e);
    }

  return(sv);
  }

// \brief Get a time string in the common UNIX format
// \return a string containing the time
string genutil::getTimeString(void) {
  // get the UNIX time
  time_t t=time(NULL);
    
  // break into a structure
  struct tm *ts=localtime(&t);
  if (!ts) 
    throw GUException("getTimeString()",
		      GUException::ERROR,
		      __FILE__, __LINE__,
		      "can't get local time structure");

  // format into a familiar ASCII string
  char *tp=asctime(ts);
  if (!tp) 
    throw GUException("getTimeString()",
		      GUException::ERROR,
		      __FILE__, __LINE__,
		      "can't get local time string");

  // return time as a string, with no \n at the end
  return(sed(tp, "\n", ""));
  }

/// \brief Return true if the named file exists
bool genutil::statFile(const string& fileName) {
  // stat the file
  struct stat buf;
  return(stat(fileName.c_str(), &buf)==0 && S_ISREG(buf.st_mode));
  }

/// \brief Return the size of the named file, or zero if the 
/// file doesn't exist
off_t genutil::fileSize(const string& fileName) {
  if (!statFile(fileName))
    return(0);

  // stat the file
  struct stat buf;
  if (stat(fileName.c_str(), &buf)!=0)
    return(0);

  return(buf.st_size);
  }

/// \brief Return the modification time of the file in UNIX seconds, 
/// or -1 if the file didn't exist or couldn't be stat'd
double genutil::fileModificationTime(const string& fileName) {
  if (!statFile(fileName))
    return(-1);

  // stat the file
  struct stat buf;
  if (stat(fileName.c_str(), &buf)!=0)
    return(-1);

  return(double(buf.st_mtime));
  }

/// \brief Return the access time of the file in UNIX seconds, 
/// or -1 if the file didn't exist or couldn't be stat'd
double genutil::fileAccessTime(const string& fileName) {
  if (!statFile(fileName))
    return(-1);

  // stat the file
  struct stat buf;
  if (stat(fileName.c_str(), &buf)!=0)
    return(-1);

  return(double(buf.st_atime));
  }

/// \brief Return true if the named directory exists (as a directory)
bool genutil::statDir(const string& dirName) {
  // stat the file
  struct stat buf;
  return(stat(dirName.c_str(), &buf)==0 && S_ISDIR(buf.st_mode));
  }

/// \brief rename file or directory
void genutil::renamePath(const string& oldpath, const string& newpath) {
  if (::rename(oldpath.c_str(), newpath.c_str())!=0)
    throw GUException("renamePath()",
		      GUException::ERROR,
		      __FILE__, __LINE__,
		      "failed to rename '"+oldpath+"' to '"+newpath+"'");
  }

/// \brief remove file
void genutil::removePath(const string& fileName) {
  if (remove(fileName.c_str())!=0) {
    throw GUException("renameFile()",
		      GUException::ERROR,
		      __FILE__, __LINE__,
		      "failed to remove '"+fileName+"'");
    }
  }

/// \brief fork and start child process as a true daemon
void genutil::daemonize(void) {
  // don't daemonize if debugging is enabled
  if (gutilDebugEnable)
    return;

  // clear file mode creation mask
  umask(0);

  pid_t pid;

  // try to fork; exit if fork failed
  if ((pid=fork())<0)
    exit(-1);
  // parent goes away
  else if (pid!=0)
    exit(0);

  // if we got this far, we're in the child process
  
  // child becomes session leader; exit if fails
  if (setsid()<0)
    exit(-1);

  // change to the root directory 
  if (chdir("/")<0)
    exit(-1);

  // close standard file handles
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  // attach standard filehandles to /dev/null
  /*
  int fd0, fd1, fd2;
  fd0=open("/dev/null", O_RDWR);
  fd1=dup(0);
  fd2=dup(0);
  */
}

/// \brief a quick diff function, which returns trie if files are identical,
/// false if not
bool genutil::quickDiff(const string& one, const string& two) {
  // return false if only one of the files exists or is readable
  if ((statFile(one) && !statFile(two)) ||
      (!statFile(one) && statFile(two)))
    return(false);

  // return true if both of the files don't exist or aren't readable
  if (!statFile(one) && !statFile(two))
    return(true);

  // get filesizes
  off_t size1=fileSize(one), size2=fileSize(two);

  // return false if files are different size
  if (size1!=size2)
    return(false);

  ifstream ifs1(one.c_str()); ifstream ifs2(two.c_str());  
  char buffer1[MAXSTRING], buffer2[MAXSTRING];

  // examine files byte by byte for differences
  while (ifs1.read(buffer1, MAXSTRING).gcount()>0 && 
	 ifs2.read(buffer2, MAXSTRING).gcount()>0) {
    // get the number of bytes read
    size_t len=ifs1.gcount();
    
    // return false if memcmp returns non-zero
    if (memcmp(buffer1, buffer2, len))
      return(false);
    }
	 
  // no mismatches
  return(true);
  }

/// \brief copy a file 
void genutil::copyFile(const string& fromFile, const string& toFile) {
  ifstream ifs(fromFile.c_str()); ofstream ofs(toFile.c_str());
  if (ifs.fail() || ifs.bad())
    GU_THROW("genutil::copyFile()",
             "source file '"+fromFile+
             "' couldn't be opened for reading", "");

  if (ofs.fail() || ofs.bad())
    GU_THROW("genutil::copyFile()",
             "target file '"+toFile+"' couldn't be opened for writing", "");

  char buffer[MAXSTRING];
  while (ifs.read(buffer, MAXSTRING).gcount()>0) {
    size_t len=ifs.gcount();
    ofs.write(buffer, len);
    }

  ofs.flush(); ifs.close(); ofs.close();
  }

/// \brief write a standardized lock file at a given filename
void genutil::writeLock(const string& fileName) {
  ofstream ofs(fileName.c_str());

  if (ofs.fail() || ofs.bad())
    GU_THROW("genutil::writeLock()",
	     "can't open lockfile '"+fileName+"' for writing", "");

  ofs << getUser() << " " << getHost() << " "
      << long(getTime()) << " " << getpid() << endl;

  ofs.flush();
  ofs.close();
  }

/// \brief check if a lock exists other than for this user/host/pid
bool genutil::checkLock(const string& fileName) {
  ifstream ifs(fileName.c_str());

  // file doesn't even exist, return false
  if (ifs.fail() || ifs.bad())
    return(false);

  string user, host, time, pid;

  ifs >> user >> host >> time >> pid;

  // locked by a different user
  if (user!=getUser())
    return(true);

  // locked by a different host
  if (host!=getHost())
    return(true);

  // locked by a different process
  if (pid!=boost::lexical_cast<string>(getpid()))
    return(true);

  // locked by identical user, host and process
  return(false);
  }


/// \brief read the parameters from a lock file
bool genutil::readLock(const string& fileName,
		       string& user,
		       string& host,
		       double& tLock,
		       pid_t& pid) {
  ifstream ifs(fileName.c_str());

  // file doesn't even exist, return false
  if (ifs.fail() || ifs.bad())
    return(false);

  ifs >> user >> host >> tLock >> pid;

  return(true);
  }
