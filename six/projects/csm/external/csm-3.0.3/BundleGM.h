//#############################################################################
//
//    FILENAME:          BundleGM.h
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
//     20-Feb-2017   JPK      Initial Coding
//     01-Mar-2017   JPK      Moved implementations to BundleGM.cpp
//     21-Mar-2017   JPK      Deinlined BundleGM default constructor
//<
//#############################################################################

#ifndef __CSM_BUNDLE_GM_H
#define __CSM_BUNDLE_GM_H

#include "GeometricModel.h"
#include "ModelIdentifier.h"

#define CSM_BUNDLE_GM_FAMILY "BundleGM"

namespace csm
{
//*****************************************************************************
// class BundleGM
//*****************************************************************************
class CSM_EXPORT_API BundleGM : public GeometricModel
{
public:
   BundleGM();
   
   virtual ~BundleGM();
   //> This is the destructor;
   //<
   virtual std::string getFamily() const {return CSM_BUNDLE_GM_FAMILY;}
   
   //> This method returns the Family ID
   //<
   
   std::vector<std::string> componentNames() const;
   //> This method returns the list of names of the "components" used to
   //  uniquely identify a GeometricModel in this "bundle".
   //<
   
   const MIC::RangeList& rangesFor(const std::string& argName) const;
   //> This method returns the valid set of ranges for the value associated
   //  with the specified component name.
   //<
   
   const MIDescription& idDescription() const {return theDescription;}
   //> This method returns all of the descriptions for all of the components
   //  associated with this "bundle".
   //<
   
   virtual csm::GeometricModel* geometricModel(const ModelIdentifier& id) = 0;
   //> This method returns a pointer to a GeometricModel, which is actually
   //  just a thin wrapper around this Bundle, corresponding to the
   //  provided ModelIdentifier. The pointer is only valid as long as this
   //  BundleGM stays in scope.
   //<
private:

  MIDescription theDescription;
};
} // namespace csm

#endif
