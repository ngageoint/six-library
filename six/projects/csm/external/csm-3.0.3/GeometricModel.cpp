//#############################################################################
//
//    FILENAME:          GeometricModel.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    This file provides implementations for methods declared in the
//    GeometricModel class.
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//      29-Nov-2012   JPK  Initial Coding
//      06-Dec-2012   JPK  De-inlined destructor and getFamily() methods
//                         Added inline definition for convenience method
//                         getCovarianceMatrix().  Replaced ParamType
//                         and ParamSet with param::Type and param::Set
//                         respectively. Removed all getOriginal* and
//                         setOriginal* methods and renamed getCurrent*
//                         and setCurrent* to get* and set* respectively.
//                         Moved Parameter struct inside class definition to
//                         avoid name clashes.
//      17-Dec-2012   BAH  Documentation updates.
//      12-Feb-2013   JPK  Renamed EXACT to Parameter Type to FIXED,
//                         Renamed FIXED Parameter Set to NON_ADJUSTABLE
// 
//    NOTES:
//
//    Refer to GeometricModel.h for more information.
//
//#############################################################################

#define CSM_LIBRARY

#include "GeometricModel.h"

namespace csm {

//*****************************************************************************
// GeometricModel::~GeometricModel()
//*****************************************************************************
GeometricModel::~GeometricModel()
{}
   
//*****************************************************************************
// GeometricModel::getFamily()
//*****************************************************************************
std::string GeometricModel::getFamily() const
{
   return CSM_GEOMETRIC_MODEL_FAMILY;
}
   
//*****************************************************************************
// GeometricModel::getParameterSetIndices()
//*****************************************************************************
   std::vector<int> GeometricModel::getParameterSetIndices(param::Set pSet) const
{
   const int NUM_PARMS = getNumParameters();
   
   std::vector<int> indices;
   indices.reserve(NUM_PARMS);
   
   for (int i = 0; i < NUM_PARMS; ++i)
   {
      param::Type pType = getParameterType(i);
      if (pType == param::FIXED)
      {
         if (pSet != param::ADJUSTABLE) indices.push_back(i);
      }
      else if ((pType != param::NONE) && (pSet != param::NON_ADJUSTABLE))
      {
         indices.push_back(i);
      }
   }
   return indices;
}

//*****************************************************************************
// GeometricModel::getParameter()
//*****************************************************************************
GeometricModel::Parameter GeometricModel::getParameter(int index) const
{
   //***
   // Each of the individual "get" calls is responsible for range checking
   // the provided index, so no need to do so here.
   //*** 
   return Parameter(getParameterName(index),
                    getParameterValue(index),
                    getParameterUnits(index),
                    getParameterType(index),
                    getParameterSharingCriteria(index));
}
   
//*****************************************************************************
// GeometricModel::getParameter()
//*****************************************************************************
void GeometricModel::setParameter(int index,const Parameter& param)
{
   //***
   // Derived classes may have the ability to adjust all parameter attributes,
   // but no methods exist at this level to change anything but the value and
   // type.  Each of the individual "set" calls is responsible for range
   // checking the desired index, so no need to do that here.
   //***
   setParameterValue (index,param.value);
   setParameterType  (index,param.type);
}
   
//*****************************************************************************
// GeometricModel::getParameters()
//*****************************************************************************
std::vector<GeometricModel::Parameter>
GeometricModel::getParameters(param::Set pSet) const
{
   std::vector<Parameter> paramVctr;
   
   const std::vector<int>& indices = getParameterSetIndices(pSet);
   
   const int NUM_PARMS = indices.size();
   
   if (NUM_PARMS > 0)
   {
      paramVctr.resize(NUM_PARMS);
      
      for (int i = 0; i < NUM_PARMS; ++i)
      {
         paramVctr[i] = getParameter(indices[i]);
      }
   }
   
   return paramVctr;
}

} // namespace csm
