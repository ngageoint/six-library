//#############################################################################
//
//    FILENAME:          Version.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    This class represents a version designator.  This class consists of a
//    series of integers, e.g., "1.2.3".  Versions can be compared for
//    equality or for relative position (which version is most current).
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     19-Apr-2012   SCM      Initial creation
//     30-Oct-2012   SCM      Added version() accessors.
//     30-Oct-2012   SCM      Renamed to Version.h
//     17-Dec-2012   BAH      Documentation updates.
//     15-Sep-2020   bradh    Undef major / minor macros from <sys/types.h>
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_VERSION_H
#define __CSM_VERSION_H

#include "csm.h"

#include <string>
#include <iosfwd>
#include <vector>
#include <sstream>
#ifdef major
#undef major
#endif
#ifdef minor
#undef minor
#endif

namespace csm
{

class CSM_EXPORT_API Version
{
public:
   explicit Version(const std::string& version);
      //> This constructor makes a Version object with the given string version.
      //  The string should be a sequence of numbers separated by periods (".").
      //<

   Version(int major, int minor, int revision = -1);
      //> This constructor makes a Version object with the given major, minor,
      //  and revision version information.  The major and minor versions must
      //  be greater than or equal to 0, or an exception will be thrown.  If the
      //  revision is less than zero, it will be omitted from the object.
      //
      //  The resulting version string will look like "major.minor.revision".
      //<

   ~Version();
      //> This is the destructor.  This class has no virtual methods, and
      //  should not be derived from.
      //<

   std::ostream& print(std::ostream& os) const;
      //> This method renders the version as a string to the given output
      //  stream.
      //<

   std::string version() const { std::ostringstream s; print(s); return s.str(); }
      //> This method returns the version as a string.  If the result is being
      //  sent to an ostream, use the << operator or print method instead for
      //  efficiency.
      //<

   int version(unsigned int part) const
      { return (part < theVersions.size() ? theVersions[part] : -1); }
      //> This method returns the requested portion of the version.  If the
      //  requested part does not exist, this method returns -1.
      //<

   int major() const { return version(0); }
      //> This method returns the major version number.
      //<

   int minor() const { return version(1); }
      //> This method returns the minor version number.
      //<

   int revision() const { return version(2); }
      //> This method returns the revision version number, if it exists.
      //  If there is no revision in this version, this method returns -1.
      //<

   bool operator==(const Version& rhs);
      //> This method returns true if the two versions are equal.
      //<

   bool operator<(const Version& rhs);
      //> This method returns true if this version is earler than the given
      //  version.
      //<

private:
   std::vector<int> theVersions;
      //> This data member stores the major, minor, and revision components of
      //  the current Version object.
      //<
};

} // namespace csm

inline std::ostream& operator<<(std::ostream& os, const csm::Version& vsrn)
{
   return vsrn.print(os);
}

#endif
