//#############################################################################
//
//    FILENAME:          Version.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//     This file provides implementation for methods declared in the
//     Version class.
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     19-Apr-2012   SCM      Initial creation
//     30-Oct-2012   SCM      Renamed to Version.cpp
//     17-Dec-2012   BAH      Documentation updates.
//     10-Sep-2013   SCM      Added std namespace to atoi() calls.
//
//    NOTES:
//     Refer to Version.h for more information.
//
//#############################################################################

#define CSM_LIBRARY

#include "Version.h"
#include "Error.h"

#include <ostream>
#include <cstdlib>

namespace csm
{

//*****************************************************************************
// Version::Version
//*****************************************************************************
Version::Version(const std::string& version)
   :
      theVersions()
{
   static const char* const MODULE = "csm::Version::Version";

   // split on periods
   std::string::size_type pos = 0;
   std::string::size_type dotPos = 0;
   while ( (dotPos = version.find('.', pos)) != std::string::npos )
   {
      const std::string v = version.substr(pos, dotPos - pos);
      if (v.find_first_not_of("1234567890") != std::string::npos)
      {
         throw Error(Error::INVALID_USE, "Invalid version component: " + v,
                     MODULE);
      }

      theVersions.push_back(std::atoi(v.c_str()));
      pos = dotPos + 1;
   }

   // add the final version
   const std::string v = version.substr(pos);
   if (v.find_first_not_of("1234567890") != std::string::npos)
   {
      throw Error(Error::INVALID_USE, "Invalid version component: " + v,
                  MODULE);
   }
   theVersions.push_back(std::atoi(v.c_str()));
}

//*****************************************************************************
// Version::Version
//*****************************************************************************
Version::Version(int major, int minor, int revision)
   :
      theVersions()
{
   static const char* const MODULE = "csm::Version::Version";

   if (major < 0)
   {
      throw Error(Error::BOUNDS, "Invalid major version number", MODULE);
   }
   if (minor < 0)
   {
      throw Error(Error::BOUNDS, "Invalid minor version number", MODULE);
   }

   theVersions.push_back(major);
   theVersions.push_back(minor);
   if (revision >= 0) theVersions.push_back(revision);
}

//*****************************************************************************
// Version::~Version
//*****************************************************************************
Version::~Version()
{
}

//*****************************************************************************
// Version::print
//*****************************************************************************
std::ostream& Version::print(std::ostream& os) const
{
   for(std::vector<int>::const_iterator it = theVersions.begin();
       it != theVersions.end(); ++it)
   {
      if (it != theVersions.begin()) os << ".";
      os << *it;
   }
   return os;
}

//*****************************************************************************
// Version::operator==
//*****************************************************************************
bool Version::operator==(const Version& rhs)
{
   return (theVersions == rhs.theVersions);
}

//*****************************************************************************
// Version::operator<
//*****************************************************************************
bool Version::operator<(const Version& rhs)
{
   for(size_t i = 0; i < theVersions.size() && i < rhs.theVersions.size(); ++i)
   {
      if (theVersions[i] < rhs.theVersions[i]) return true;
      if (theVersions[i] > rhs.theVersions[i]) return false;

      // must be equal, move to next value
   }

   //***
   // Equal up until one version was less than the other, so if our vector has
   // fewer elements than the RHS vector, then we are less.  Otherwise, we are
   // not less (note: that might mean we're equal, but that still means false)
   //***
   return (theVersions.size() < rhs.theVersions.size());
}

} // namespace csm
