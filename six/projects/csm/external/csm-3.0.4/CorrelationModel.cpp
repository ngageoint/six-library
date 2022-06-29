//#############################################################################
//
//    FILENAME:          CorrelationModel.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    This class is used to compute the correlation between model
//    parameters in a community sensor model (CSM).
//
//    LIMITATIONS:       None
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     18-Feb-2013   JPK      Initial Coding
//     20-Sep-2019   JPK      Added setFormat() and
//                            setNumCorrelationParameterGroups()
//
//    NOTES:
//     Refer to CorrelationModel.h for more information.
//
//#############################################################################

#define CSM_LIBRARY
#include "CorrelationModel.h"
#include "Error.h"


namespace csm {
     
CorrelationModel::CorrelationModel(const std::string& format,
                                   size_t             numCPGroups)
   :
      theFormat                  (format),
      theDecorrelationEventTimes ()
{
   if (numCPGroups)
   {
      setNumCorrelationParameterGroups(numCPGroups);
   }
}

void CorrelationModel::setNumCorrelationParameterGroups(size_t numCPGroups)
{
   if (numCPGroups)
   {
     theDecorrelationEventTimes.resize(numCPGroups,"");
   }
   else
   {
      theDecorrelationEventTimes.clear();
   }
}

void CorrelationModel::setFormat(const std::string& format)
{
   theFormat = format;
}

const std::string&
CorrelationModel::getDecorrelationEventTime(size_t cpGroupIndex) const
{
   if (cpGroupIndex >= theDecorrelationEventTimes.size())
   {
      throw Error(Error::INDEX_OUT_OF_RANGE,
                  "Correlation parameter group index is out of range.",
                  "csm::CorrelationModel::getDecorrelationEventTime");
   }

   return theDecorrelationEventTimes[cpGroupIndex];
}
   
void
CorrelationModel::setDecorrelationEventTime(const std::string& decorrEventTime,
                                            size_t             cpGroupIndex)
{
   if (cpGroupIndex >= theDecorrelationEventTimes.size())
   {
      throw Error(Error::INDEX_OUT_OF_RANGE,
                  "Correlation parameter group index is out of range.",
                  "csm::CorrelationModel::setDecorrelationEventTime");
   }

   theDecorrelationEventTimes[cpGroupIndex] = decorrEventTime;
}

} // namespace csm
