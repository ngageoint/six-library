//#############################################################################
//
//    FILENAME:          MultiNitfIsd.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    This file contains the implementations for the functions defined in CSM.
//
//    LIMITATIONS:       None
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     28-NOV-2018   JPK      Initial Version.
//     29-NOV-2018   SCM      Cleanup formatting and added include of Error.h.
//
//    NOTES:
//
//#############################################################################

#ifndef CSM_LIBRARY
#define CSM_LIBRARY
#endif

#include "MultiNitfIsd.h"
#include "Error.h"

namespace csm
{

//*****************************************************************************
// MultiNitfIsd::MultiNitfIsd
//*****************************************************************************
MultiNitfIsd::MultiNitfIsd(const std::vector<csm::NitfIsd>& isdList)
   :
      Isd("MULTINITF", ""),
      theISDs(isdList)
{
}

//*****************************************************************************
// MultiNitfIsd::addIsd
//*****************************************************************************
void MultiNitfIsd::addIsd(const csm::NitfIsd &isd)
{
   theISDs.push_back(isd);
}
   
//*****************************************************************************
// MultiNitfIsd::isdList
//*****************************************************************************
const std::vector<csm::NitfIsd>& MultiNitfIsd::isdList() const
{
   return theISDs;
}

//*****************************************************************************
// MultiNitfIsd::isdList
//*****************************************************************************
std::vector<csm::NitfIsd>& MultiNitfIsd::isdList()
{
   return theISDs;
}

//*****************************************************************************
// MultiNitfIsd::isd
//*****************************************************************************
const csm::NitfIsd& MultiNitfIsd::isd(std::size_t index) const
{
   if (index < theISDs.size()) return theISDs[index];

   throw Error(Error::INDEX_OUT_OF_RANGE,
               "ISD Index is out of range.",
               "csm::MultiNitfIsd::isd");
}
   
} //namespace csm
