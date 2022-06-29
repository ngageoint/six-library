//#############################################################################
//
//    FILENAME:          MultiNitfIsd.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the MultiNitfIsd base class. MultiNitfIsd is encapsulated in a
//    C++ class for transfer through the CSM (Plugin) interface. MultiNitfIsd
//    holds a vector of csm::NitfIsds
//
//    LIMITATIONS:       None
//
//    SOFTWARE HISTORY:
//
//     Date          Author   Comment
//     -----------   ------   -------
//     28-NOV-2018   JPK      Initial Version.
//     29-NOV-2018   SCM      Cleanup formatting.  Use std::size_t for size.
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_MULTINITFISD_H
#define __CSM_MULTINITFISD_H

#include <string>
#include <vector>

#include "csm.h"
#include "NitfIsd.h"

namespace csm
{

class CSM_EXPORT_API MultiNitfIsd : public Isd
{
public:
   MultiNitfIsd() : Isd("MULTINITF", ""), theISDs() {}
      //> This constructor makes an empty image support data list.
      //<

   explicit MultiNitfIsd(const std::vector<csm::NitfIsd>& isdList);
      //> This constructor stores one or more image support data objects.
      //<

   const std::string& format() const { return theFormat; }
      //> This method returns the format of the image support data (MULTINITF).
      //<

   void addIsd(const csm::NitfIsd& isd);
      //> This method adds a csm::NitfIsd to the MultiNitfIsd.
      //<

   const std::vector<csm::NitfIsd>& isdList() const;
      //> This method allows const retrieval of the entire
      //  vector of available csm::NitfIsd objects.
      //<

   std::vector<csm::NitfIsd>& isdList();
      //> This method allows non-const retrieval of the
      //  vector of available csm::NitfIsd objects, allowing the
      //  vector to be modified (added to / deleted from).
      //<

   void clearAllIsds() { theISDs.clear(); }
      //> This method removes all the ISDs added to the vector.
      //<
         
   std::size_t numberOfIsds() const {return theISDs.size();}
      //> This method returns the number of available ISDs owned
      //  by this class.
      //<

   const csm::NitfIsd& isd(std::size_t index) const;
      //> This method returns the csm::NitfIsd at the specified
      //  index.
      //<

protected:
   std::vector<csm::NitfIsd> theISDs;
};

} // namespace csm

#endif
