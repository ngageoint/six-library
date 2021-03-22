//#############################################################################
//
//    FILENAME:          BundleGM.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for abstract class that is to provide a common interface for
//    exploiting metadata comprising the information necessary to project
//    coordinates and provide error estimates within and between a related
//    set of csm::GeometricModel objects.
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//>
//     Date          Author   Comment
//     -----------   ------   -------
//     01-Mar-2017   JPK      Moved implmentations here from BundleGM.h
//     21-Mar-2017   JPK      Moved BundleGM constructor here.  Added missing
//                            body for ~BundleGM.
//<
//#############################################################################

#define CSM_LIBRARY

#include "BundleGM.h"

namespace csm
{
//*****************************************************************************
//  BundleGM::BundleGM
//*****************************************************************************
BundleGM::BundleGM()
:
      GeometricModel (),
      theDescription ()
{}

//*****************************************************************************
//  BundleGM::~BundleGM
//*****************************************************************************
BundleGM::~BundleGM()
{}

//*****************************************************************************
//  BundleGM::componentNames
//*****************************************************************************
std::vector<std::string> BundleGM::componentNames() const
{
   return theDescription.componentNames();
}

//*****************************************************************************
//  BundleGM::rangesFor
//*****************************************************************************
const MIC::RangeList& BundleGM::rangesFor(const std::string& name) const
{
   return theDescription.rangesFor(name);
}
} // namespace csm
