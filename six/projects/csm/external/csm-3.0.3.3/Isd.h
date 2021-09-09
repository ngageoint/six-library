//#############################################################################
//
//    FILENAME:          Isd.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the Isd base class. ISD is encapsulated in a C++ class for
//    transfer through the CSM (Plugin) interface. ISD is passed as the
//    base class and cast to the appropriate derived class for use.
//
//    LIMITATIONS:       None
//
//    SOFTWARE HISTORY:
//
//     Date          Author   Comment
//     -----------   ------   -------
//     01-Jul-2003   LMT      Initial version.
//     06-Feb-2004   KRW      Incorporates changes approved by
//                            January and February 2004 CCB.
//     02-Mar-2012   SCM      Refactored interfaces.
//     04-Jun-2012   SCM      Added generic parameters.
//     26-Sep-2012   JPK      Added some common parameter types
//     11-Oct-2012   SCM      Added clearAllParams() and clearParams().
//     30-Oct-2012   SCM      Renamed to Isd.h
//     06-Dec-2012   JPK      Used CSM_UNKNOWN instead of "UNKNOWN"
//     17-Dec-2012   BAH      Documentation updates.
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_ISD_H
#define __CSM_ISD_H

#include <string>
#include <map>

#include "csm.h"

namespace csm
{

class CSM_EXPORT_API Isd
{
public:
   Isd() : theFormat(CSM_UNKNOWN), theFilename() {}
      //> This constructor makes an "unknown" image support data object.  No
      //  information about the image will be available in this mode.
      //<

   explicit Isd(const std::string& filename)
      : theFormat("FILENAME"), theFilename(filename) {}
      //> This constructor makes a "filename" image support data object.  The
      //  only data available in this mode is the name of the file.
      //<

   virtual ~Isd();
      //> A virtual destructor is needed so derived class destructors will
      //  be called when the base class object is destroyed.
      //
      //  The impelmentations for the destructors for this class and all the
      //  derived ISD classes defined in CSM are in the Isd.cpp file.
      //<

   const std::string& format() const { return theFormat; }
      //> This method returns the format of the image support data.  Each
      //  derived class has a different format string.
      //<

   const std::string filename() const { return theFilename; }
      //> This method returns the filename associated with the image support
      //  data, if any.  If there is no filename, this string will be empty.
      //<

   inline void setFilename(const std::string& fn);
      //> This method sets the filename associated with the image support data.
      //  If there is no associated filename, an empty string should be used.
      //<

   inline std::string param(const std::string& name, int instance = 0) const;
      //> This method returns the value of the instance'th occurrance of the
      //  parameter named name in the parameter map.  If there are no
      //  parameters named name or instance is greater than the number of
      //  parameters named name, then an empty string ("") is returned.
      //<

   inline void addParam(const std::string& name, const std::string& value);
      //> This method adds a parameter named name with the value value to the
      //  parameter map.
      //<

   void clearAllParams() { theParameters.clear(); }
      //> This method removes all the parameters added to the parameter map.
      //<

   void clearParams(const std::string& name) { theParameters.erase(name); }
      //> This method removes all the instances of the given parameter name from
      //  the parameter map.
      //<

   const std::multimap<std::string, std::string>& parameters() const { return theParameters; }
      //> This method returns a reference to the parameter map.
      //<

protected:
   explicit Isd(const std::string& format, const std::string& filename)
      : theFormat(format), theFilename(filename) {}

   std::string theFormat;
   std::string theFilename;

   std::multimap<std::string, std::string> theParameters;
      //> Data member multimap that organizes additional parameters used to
      //  build a particular geometry.  It allows for multiple keys and
      //  multiple instances of the same key.  Though it can be accessed
      //  directly, it may be easier to access it through the param and
      //  addParam methods.
      //<
};

inline void Isd::setFilename(const std::string& fn)
{
   theFilename = fn;

   // if the format was unkown before set it to FILENAME now
   if (theFormat == CSM_UNKNOWN) theFormat = "FILENAME";
}

inline std::string Isd::param(const std::string& name, int instance) const
{
   std::multimap<std::string, std::string>::const_iterator it = theParameters.find(name);
   while (it != theParameters.end() && (instance || it->first != name))
   {
      // Unfortunately, the iterator is not limited to items whose key is named
      // name, so only decrement the instance variable if this matches
      if (it->first == name) --instance;

      ++it;
   }

   if (it == theParameters.end()) return "";

   return it->second;
}

inline void Isd::addParam(const std::string& name, const std::string& value)
{
   std::multimap<std::string, std::string>::value_type val(name, value);
   theParameters.insert(val);
}

#define IMAGE_ID_PARAM       "IMAGE_ID"
#define IMAGE_INDEX_PARAM    "IMAGE_INDEX"
#define LOGICAL_INDEX_PARAM  "LOGICAL_INDEX"
#define MODEL_NAME_PARAM     "MODEL_NAME"

} // namespace csm

#endif
