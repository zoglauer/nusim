/* H+
 *
 *      Title:     GUStringvec.h
 *      Author:    Bryce A. Roberts
 *      Date:      Fri Aug 12 22:06:43 PDT 2005
 *      Synopsis:  
 *      Revisions:
 *      mm/dd/yy        name    description
 *
 * H-                                           
 */

#ifndef _GUStringvec_H
#define _GUStringvec_H

// use namespace std to get access to vector, string, stringstream, etc
using namespace std;

// include the bare minimum of header files here
#include <iostream>
#include <string>
#include <vector>


#define DEFAULT_INPUT_SEPARATOR    " \t"
#define DEFAULT_OUTPUT_SEPARATOR   ","


namespace genutil {
  /*! 
    \brief GUStringvec is a lightweight class for dealing conveniently with
    vectors of strings.

    It is little more than a class that inherits from vector<string>, 
    meaning that once strings are stored in a GUStringvec, all the 
    available methods and algorithms of the standard library's vector
    type are available.

    Additionally, GUStringvec implements >> and << operators which allow
    input streams (including cin, ifstream and stringstream) to be tokenized,
    and allow the instance to be written to an output stream.
  */
  class GUStringvec : public vector<string> {
    protected:
      /// token separators when receiving FROM a stream
      string inputTokenSep;
      
      /// token separators when outputing TO a stream
      string outputTokenSep;

      /// An internal function to tokenize an input stream, up until
      /// a newline is encountered
      /// \param is input stream passed by reference
      /// \return a reference to the istream that was passed
      istream& split(istream& is);
      void split(string str);

    public: 
      /// a public typedef to make accessing const_iterator easier
      typedef const_iterator ci; 

      /// a public typedef to make accessing iterator easier
      typedef iterator it; 

      /*!
	 This constructor takes a variable list of strings (up to six),
	 and stuffs them into the GUStringvec in order.  If no arguments
	 are provided, this just defaults to being a no-arg constructor.
      */
      GUStringvec(string s1="", string s2="", string s3="",
		  string s4="", string s5="", string s6="");

      /// Sets the list of characters which should be used to separate
      /// tokens when the GUSTringvec is being read IN from a stream.  NOTE:
      /// ANY of the characters in inputTokenSep constitute the start
      /// of a token.  For instance, " \t" (space tab) will separate
      /// tokens that are separated by either spaces OR tabs.
      /// \param inputTokenSep string containing input separator characters
      /// \return a reference to self
      GUStringvec& setInputTokenSeparator(const string& inputTokenSep);

      /// Sets the string which should be sued to separate tokens when
      /// the GUStringvec is being output TO a stream.  NOTE: the
      /// argument may be a single character string (traditionally " " or ",",
      /// but it can just as well be an arbitrary string, like " and ".
      /// \param outputTokenSep string containing output separator pattern
      /// \return a reference to self
      GUStringvec& setOutputTokenSeparator(const string& outputTokenSep);

      /// A convenience function to erase the contents of an instance
      /// of GUStringvec.
      GUStringvec& clear(void);

      //////////////////////////////////////////////////////////////////////
      // Friends
      //////////////////////////////////////////////////////////////////////
      friend ostream& operator << (ostream&, const GUStringvec&);
      friend istream& operator >> (istream&, GUStringvec&);
      friend GUStringvec& operator >> (const string&, GUStringvec&);
      };

  ostream& operator << (ostream& os, const GUStringvec& s);
  istream& operator >> (istream& is, GUStringvec& s);
  }

#endif
