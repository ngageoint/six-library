//#############################################################################
//
//    FILENAME:          ModelIdentifier.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    This files defines various structs and classes to allow identification
//    of a specific Model within a "bundle" of related Models.
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//>
//     Date          Author   Comment
//     -----------   ------   -------
//     01-Mar-2017   JPK      Moved implementations her from ModelIdentifier.h
//                            Fixed include usage, added ability to compare
//                            across Data types as appropriate.
//     21-Mar-2017   JPK      Added missing default constructors.
//<
//#############################################################################

#define CSM_LIBRARY

#include "ModelIdentifier.h"
#include "Error.h"
#include <sstream>
#include <algorithm>

namespace csm
{
namespace MIC
{
//*****************************************************************************
// Data::Data
//*****************************************************************************
Data::Data(const std::string& argValue)
   :
      theType(STRING_TYPE),
      theLen (0),
      theStr (0)
{
   setValue(argValue,false);
}
//*****************************************************************************
// Data::Data
//*****************************************************************************
Data::Data(const Data& argData)
   :
      theType(INT_TYPE),
      theInt (0)
{
   switch(argData.type())
   {
   case STRING_TYPE  : setValue(argData.strValue(),false); break;
   case INT_TYPE     : setValue(argData.intValue(),false); break;
   case DOUBLE_TYPE  : setValue(argData.dblValue(),false); break;
   }
}
//*****************************************************************************
// Data::~Data
//*****************************************************************************
Data::~Data()
{
   clear();
}
//*****************************************************************************
// Data::strValue()
//*****************************************************************************
std::string Data::strValue() const
{
   if (theType == STRING_TYPE)
   {
      return std::string(theStr,theLen);
   }

   std::ostringstream strm;
   
   if (theType == INT_TYPE)
   {
      strm << theInt;
   }
   else // DOUBLE_TYPE
   {
      strm << theDbl;
   }
   
   return strm.str();
}
//*****************************************************************************
// Data::intValue()
//*****************************************************************************
int Data::intValue() const
{
   if (theType == INT_TYPE) return theInt;
   
   int retVal = 0;
   
   if (theType == DOUBLE_TYPE)
   {
      retVal = (int)theDbl;
   }
   else
   {
      std::string strVal(theStr,theLen);
      std::istringstream strm(strVal);
      
      if (!(strm >> retVal))
      {
         
         throw Error(Error::INVALID_USE,
                     "string value " + strVal +
                     " cannot be converted to an integer value",
                     "MIC::Data::intValue");
      }
   }
   
   return retVal;
}
//*****************************************************************************
// Data::dblValue()
//*****************************************************************************
double Data::dblValue() const
{
   if (theType == DOUBLE_TYPE) return theDbl;
   
   
   double retVal = 0;
   
   if (theType == INT_TYPE)
   {
      retVal = (double)theInt;
   }
   else
   {
      std::string strVal(theStr,theLen);
      std::istringstream strm(strVal);
      
      if (!(strm >> retVal))
      {
         throw Error(Error::INVALID_USE,
                     "string value " + strVal +
                     " cannot be converted to a double value.",
                     "MIC::Data::dblValue");
      }
   }
   
   return retVal;
}
//*****************************************************************************
// Data::setValue
//*****************************************************************************
void Data::setValue(const std::string& value,bool clearPrevious)
{
   if (clearPrevious) clear();
   
   theType = STRING_TYPE;
   theStr  = 0;
   theLen  = value.length();
   
   if (theLen)
   {
      theStr = new char[theLen + 1];
      strncpy(theStr,value.c_str(), theLen);
      theStr[theLen] = '\0';
   }
}
//*****************************************************************************
// Data::setValue
//*****************************************************************************   
void Data::setValue(int value,bool clearPrevious)
{
   if (clearPrevious) clear();
   
   theType = INT_TYPE;
   theInt  = value;
}
//*****************************************************************************
// Data::setValue
//*****************************************************************************   
void Data::setValue(double value,bool clearPrevious)
{
   if (clearPrevious) clear();
   
   theType = DOUBLE_TYPE;
   theDbl  = value;
}
//*****************************************************************************
// Data::operator<
//*****************************************************************************   
bool Data::operator<(const Data& argData) const
{
   //***
   // If types are the same, compare directly
   //***
   if (argData.type() == theType)
   {
      switch(theType)
      {
      case STRING_TYPE : return (strValue() < argData.strValue());
      case INT_TYPE    : return (intValue() < argData.intValue());
      case DOUBLE_TYPE : return (dblValue() < argData.dblValue());
      }
   }
   
   //***
   // Types are different.
   // avoid truncation and conversion "from" string which could fail..
   //***
   if (theType != STRING_TYPE)
   {
      switch (argData.type())
      {
      case STRING_TYPE : return (strValue() < argData.strValue());
      case INT_TYPE    : return (dblValue() < argData.intValue());
      case DOUBLE_TYPE : return (intValue() < argData.dblValue());
      } 
   }
   
   //***
   // Convert argument type to string and perform string comparison
   //***
   return strValue() < argData.strValue();
}
//*****************************************************************************
// Data::operator==
//*****************************************************************************   
bool Data::operator==(const Data& argData) const
{
   //***
   // If types are the same, compare directly
   //***
   if (argData.type() == theType)
   {
      switch(theType)
      {
      case STRING_TYPE : return (strValue() == argData.strValue());
      case INT_TYPE    : return (intValue() == argData.intValue());
      case DOUBLE_TYPE : return (dblValue() == argData.dblValue());
      }
   }
   
   //***
   // Types are different.
   // avoid truncation and conversion "from" string which could fail..
   //***
   if (theType != STRING_TYPE)
   {
      switch (argData.type())
      {
      case STRING_TYPE : return (strValue() == argData.strValue());
      case INT_TYPE    : return (dblValue() == (double)argData.intValue());
      case DOUBLE_TYPE : return ((double)intValue() == argData.dblValue()); 
      }
   }
   
   //***
   // Convert argument type to string and perform string comparison
   //***
   return (strValue() == argData.strValue());
}
//*****************************************************************************
// Data::operator=
//*****************************************************************************
Data& Data::operator=(const Data& argData)
{
   switch(argData.type())
   {
   case STRING_TYPE  : setValue(argData.strValue()); break;
   case INT_TYPE     : setValue(argData.intValue()); break;
   case DOUBLE_TYPE  : setValue(argData.dblValue()); break;
   }
   
   return *this;
}
//*****************************************************************************
// Data::clear()
//*****************************************************************************
void Data::clear()
{
   switch (theType)
   {
   case INT_TYPE    : theInt = 0;   break;
   case DOUBLE_TYPE : theDbl = 0.0; break;
   case STRING_TYPE :
   {
      if (theStr)
      {
         delete [] theStr;
         theStr = 0;
         theLen = 0;
      }
      break;
   }
   }
}
//*****************************************************************************
// RangeList::Range::Range
//*****************************************************************************
RangeList::Range::Range()
   :
      theMinimum      (0),
      theMaximum      (0),
      theStep         (0),
      theMinInclusive (false),
      theMaxInclusive (false)
{} 
//*****************************************************************************
// RangeList::Range::Range
//*****************************************************************************
 RangeList::Range::Range(const std::string& single)
   :
      theMinimum      (single),
      theMaximum      (single),
      theStep         (single),
      theMinInclusive (true),
      theMaxInclusive (true)
{}
//*****************************************************************************
// RangeList::Range::Range
//*****************************************************************************
RangeList::Range::Range(int  minValue,
                        int  maxValue,
                        int  stepValue)
   :
      theMinimum      (minValue),
      theMaximum      (maxValue),
      theStep         (stepValue),
      theMinInclusive (true),
      theMaxInclusive (true)
{
   if ((stepValue != 1) && ((maxValue == minValue) || (stepValue < 1)))
   {
      theStep.setValue(1,false);
   }
   
   adjustToStep();
}
//*****************************************************************************
// RangeList::Range::Range
//*****************************************************************************
RangeList::Range::Range(double minValue,
                        double maxValue,
                        double stepValue,
                        bool   includesMin,
                        bool   includesMax)
   :
      theMinimum      (minValue),
      theMaximum      (maxValue),
      theStep         (stepValue),
      theMinInclusive (includesMin),
      theMaxInclusive (includesMax)
{
   if ((stepValue != 0.0) && ((maxValue == minValue) || (stepValue < 0.0)))
   {
      theStep.setValue(0.0,false);
   }
   
   adjustToStep();
}
//*****************************************************************************
// RangeList::Range::contains
//*****************************************************************************
bool RangeList::Range::contains(const Data& argValue) const
{
   if (type() == argValue.type())
   {
      switch(type())
      {
      case Data::STRING_TYPE: return contains(argValue.strValue());
      case Data::INT_TYPE:    return contains(argValue.intValue());
      case Data::DOUBLE_TYPE: return contains(argValue.dblValue());
      }
   }
   
   if (type() != Data::STRING_TYPE)
   {
      switch(argValue.type())
      {
      case Data::INT_TYPE    : return contains((double)argValue.intValue());
      case Data::DOUBLE_TYPE :
      {
         //***
         // Range is integer and value is double.
         // If value is equal to an integer,
         // check range.  Otherwise, fail.
         //***
         double argDbl = argValue.dblValue();
         
         int    argInt = (int)argDbl;
         
         return ((argDbl == argInt) ? contains(argInt) : false);
      }
      case Data::STRING_TYPE :
      {
         if (type() == Data::INT_TYPE)
         {
            try
            {
               return contains(argValue.intValue());
            }
            catch(csm::Error&)
            {
               return false;
            }
         }
         else
         {
            try
            {
               return contains(argValue.dblValue());
            }
            catch(csm::Error&)
            {
               return false;
            }
         }
      }
      }
   }  
   return contains(argValue.strValue());
}
//*****************************************************************************
// RangeList::Range::contains
//*****************************************************************************
bool RangeList::Range::contains(const std::string& argValue) const
{
   const Data::Type& currentType = type();
   
   if (currentType == Data::STRING_TYPE)
   {
      return ((theMinimum.strValue() == argValue) ||
              (theMaximum.strValue() == argValue));
   }
   
   if (currentType == Data::INT_TYPE)
   {
      int valInt = 0;
      std::istringstream strm(argValue);
      
      if (strm >> valInt) return contains(valInt);
   }
   else // Data::DOUBLE_TYPE
   {
      double valDbl = 0;
      std::istringstream strm(argValue);
      
      if (strm >> valDbl) return contains(valDbl);
      
   }
   return false;
}
//*****************************************************************************
// RangeList::Range::contains
//*****************************************************************************
bool RangeList::Range::contains(int argVal) const
{
   const Data::Type& currentType = type();
   
   if (currentType == Data::INT_TYPE)
   {
      if ((theMinimum.intValue() == argVal) ||
          (theMaximum.intValue() == argVal))
      {
         return true;
      }
      
      if ((theMinimum.intValue() < argVal)  &&
          (theMaximum.intValue() > argVal))
      {  
         //***
         // The step should never < 1, but if so, ignore it.
         //***
         if ((theStep.intValue() <= 1))
         {
            return true;
         }
         
         //***
         // A step size other than 1 implies that the value must be a
         // discrete step between the minimum and maximum to be in the
         // range.
         //***
         int nbrSteps = (argVal - theMinimum.intValue())/
                        theStep.intValue();
         
         return (argVal == (theMinimum.intValue() +
                            theStep.intValue() * nbrSteps));
      }
   }
   else if (currentType == Data::DOUBLE_TYPE)
   {
      return contains((double)argVal);
   }
   else // Data::STRING_TYPE
   {
      std::ostringstream strm;
      strm << argVal;
      
      return contains(strm.str());
   }
   
   return false;
}
//*****************************************************************************
// RangeList::Range::contains
//*****************************************************************************
 bool RangeList::Range::contains(double argVal) const
{
   const Data::Type& currentType = type();
   
   if (currentType == Data::DOUBLE_TYPE)
   {
      if (theMinimum.dblValue() == argVal) return theMinInclusive;
      
      if (theMaximum.dblValue() == argVal) return theMaxInclusive;
      
      
      if ((theMinimum.dblValue() < argVal)  &&
          (theMaximum.dblValue() > argVal))
      {  
         //***
         // A step of 0.0 means the step size should be ignored.
         //***
         if (theStep.dblValue() <= 0.0) return true;
         
         
         //***
         // A step size other than 0 implies that the value must be a
         // discrete step between the minimum and maximum to be in the
         // range.
         //***
         int nbrSteps = (int)((argVal - theMinimum.dblValue())/
                              theStep.dblValue());
         
         return (argVal == (theMinimum.dblValue() +
                            theStep.dblValue() * nbrSteps));
      }
   }
   else if (currentType == Data::INT_TYPE)
   {
      int argInt = (int)argVal;
      
      if (argVal == argInt) return contains(argInt);
      
      return false;
   }
   else // Data::STRING_TYPE
   {
      std::ostringstream strm;
      strm << argVal;
      
      return contains(strm.str());
   }
   
   return false;
}
//*****************************************************************************
// RangeList::Range::adjustToStep
//*****************************************************************************
void RangeList::Range::adjustToStep()
{
   if (type() == Data::INT_TYPE)
   {
      //***
      // Make sure minimum is smaller than maximum
      //***
      if (theMinimum.intValue() > theMaximum.intValue())
      {
         Data tmp   = theMaximum;
         theMaximum = theMinimum;
         theMinimum = tmp;
      }
      
      if ((theStep.intValue() > 1) &&
          (theMinimum.intValue() != theMaximum.intValue()))
      {   
         //***
         // Make sure range is divisible by step... otherwise shrink it.
         //***
         int nbrSteps = (theMaximum.intValue() - theMinimum.intValue())/
                        theStep.intValue();
         
         theMaximum = MIC::Data(theMinimum.intValue() +
                                nbrSteps*theStep.intValue());
      }
   }
   else if (type() == Data::DOUBLE_TYPE)
   {
      //***
      // Make sure minimum is smaller than maximum
      //***
      if (theMinimum.dblValue() > theMaximum.dblValue())
      {
         Data tmp   = theMaximum;
         theMaximum = theMinimum;
         theMinimum = tmp;
      }
      
      //***
      // A step size of 0.0 means no step is defined.
      // Inclusion in the range simply implies value resides
      // between endpoints.
      // If a step size is provided, make sure range is adjusted to be
      // evenly divisible by step size.
      if ((theStep.dblValue() > 0.0) &&
          (theMinimum.dblValue() != theMaximum.dblValue()))
      {
         //***
         // Make sure range is divisible by step... otherwise shrink it.
         //***
         int nbrSteps = (int)((theMaximum.dblValue() -
                               theMinimum.dblValue())/
                              theStep.dblValue());
         
         theMaximum = MIC::Data(theMinimum.dblValue() +
                              nbrSteps*theStep.dblValue());
      } 
   }
}
//*****************************************************************************
// RangeList::Range::operator<
//*****************************************************************************   
bool RangeList::Range::operator<(const Range& argRange) const
{
   //***
   // For string types, minimum and maximum are the same value
   //***
   if (argRange.maximum().type() == Data::STRING_TYPE)
   {
      return (theMaximum < argRange.maximum());
   }
   
   if (type() == Data::STRING_TYPE)
   {
      return (argRange.minimum() < theMinimum);
   }
   
   //****
   // Types are either int or double
   //***
   if (theMinimum < argRange.minimum()) return false;
   
   if (argRange.maximum() < theMaximum) return false;
   
   
   bool minInside = (argRange.minimum() < theMinimum);
   
   bool maxInside = (theMaximum < argRange.maximum());
   
   //***
   // If both end points are inside argument range, the current range is
   // smaller.
   //***
   if (minInside && maxInside) return true;
   
   //***
   // If just the minimum is inside argument range, check maximum for
   // inclusion.
   //***
   if (minInside)
   {
      return (argRange.maxInclusive() || !theMaxInclusive);
   }
   
   //***
   // If just the maximum is inside argument range, check minimum for
   // inclusion.
   //***
   if (maxInside)
   {
      return (argRange.minInclusive() || !theMinInclusive);
   } 
   
   //***
   // Minimum and maximum are the same.
   // Check inclusion.
   //***
   bool sameMin = (argRange.minInclusive() == theMinInclusive);
   bool sameMax = (argRange.maxInclusive() == theMaxInclusive);
   
   //***
   // If both have same inclusion, check step.
   //***
   if (sameMin && sameMax)
   {
      return (argRange.step() < theStep);
   }
   
   return (argRange.minInclusive() && argRange.maxInclusive());
}
//*****************************************************************************
// RangeList::Range::operator==
//*****************************************************************************   
bool RangeList::Range::operator==(const Range& argRange) const
{
   //***
   // For string types, minimum and maximum are the same value
   //***
   if ((argRange.type() == Data::STRING_TYPE) ||
       (type()          == Data::STRING_TYPE))
      
   {
      return (argRange.minimum() == theMinimum);
   }
   
   return ((theMinimum      == argRange.minimum())      &&
           (theMaximum      == argRange.maximum())      &&
           (theStep         == argRange.step())         &&
           (theMinInclusive == argRange.minInclusive()) &&
           (theMaxInclusive == argRange.maxInclusive()));
}
//*****************************************************************************
// RangeList::RangeList
//*****************************************************************************
 RangeList::RangeList(const std::string& single)
   :
      theRanges(1,Range(single))
{}
//*****************************************************************************
// RangeList::RangeList
//*****************************************************************************
RangeList::RangeList(int minValue,
                     int maxValue,
                     int stepValue)
   :
      theRanges(1,Range(minValue,maxValue,stepValue))
{}

//*****************************************************************************
// RangeList::RangeList
//*****************************************************************************
RangeList::RangeList(double minValue,
                     double maxValue,
                     double stepValue,
                     bool   includesMin,
                     bool   includesMax)
   :
      theRanges(1,Range(minValue,
                        maxValue,
                        stepValue,
                        includesMin,
                        includesMax))
{}
//*****************************************************************************
// RangeList::RangeList
//*****************************************************************************
 RangeList::RangeList(const Range& argRange)
   :
      theRanges(1,argRange)
{}
//*****************************************************************************
// RangeList::RangeList
//*****************************************************************************
RangeList::RangeList(const std::vector<std::string>& argValues)
   :
      theRanges()
{
   //***
   // Since each value is assigned to its own Range, we want to remove any
   // redundant values.
   //***
   const size_t NUM_VALS = argValues.size();
   
   if (NUM_VALS)
   {
      std::vector<std::string> sortedValues = argValues;
      
      std::sort(sortedValues.begin(),
                sortedValues.end());
      
      
      theRanges.reserve(NUM_VALS);
      
      std::string lastUniqueString = sortedValues[0];
      
      theRanges.push_back(Range(lastUniqueString));
      
      for (size_t i = 1; i < NUM_VALS; ++i)
      {
         const std::string& currString = sortedValues[i];
         
         if (currString != lastUniqueString)
         {
            theRanges.push_back(Range(currString));
            lastUniqueString = currString;
         }
      }
   }
}
//*****************************************************************************
// RangeList::RangeList
//*****************************************************************************
RangeList::RangeList(const std::vector<int>& argValues)
   :
      theRanges()
{
   const size_t rSize = argValues.size();
   
   if (rSize == 1)
   {
      int single = argValues[0];
      
      theRanges.push_back(Range(single,single));
   }
   else if (rSize >= 2)
   {
      std::vector<int> sortedValues = argValues;
      
      std::sort(sortedValues.begin(),
                sortedValues.end());
      
      int minimum = sortedValues[0];
      int maximum = sortedValues[1];
      
      int diff = maximum - minimum;
      
      int step = (diff ? diff : 1);
      
      bool resetStep = false;
      
      if (rSize > 2)
      {
         //***
         // Establish step size between adjacent values
         //***
         for (size_t i = 2; i < rSize; ++i)
         {
            int currDiff = (sortedValues[i] - sortedValues[i-1]);
            
            //***
            // Ignore redundant values
            //***
            if (currDiff != 0)
            {
               if ((step != currDiff) && !resetStep)
               {  
                  Range range(minimum,
                              maximum,
                              step);
                  
                  theRanges.push_back(range);
                  
                  minimum = sortedValues[i];
                  
                  resetStep = true;     
               }
               else
               {
                  resetStep = false;
               }
               
               step = currDiff;
               maximum = sortedValues[i];
            }
         }
      }
      
      if (maximum == minimum) step = 1;
      
      Range range(minimum,maximum,step);
      
      theRanges.push_back(range);
   }
}
//*****************************************************************************
// RangeList::RangeList
//*****************************************************************************
RangeList::RangeList(const std::vector<double>& argValues)
   :
      theRanges()
{
   const size_t rSize = argValues.size();
   
   if (rSize == 1)
   {
      double single = argValues[0];
      
      theRanges.push_back(Range(single,single));
   }
   else if (rSize >= 2)
   {
      std::vector<double> sortedValues = argValues;
      
      std::sort(sortedValues.begin(),
                sortedValues.end());
      
      double minimum = sortedValues[0];
      double maximum = sortedValues[1];
      
      double step = maximum - minimum;
      
      bool resetStep = false;
      
      if (rSize > 2)
      {
         //***
         // Establish step size between adjacent values
         //***
         for (size_t i = 2; i < rSize; ++i)
         {
            double currDiff = (sortedValues[i] - sortedValues[i-1]);
            
            //***
            // Ignore redundant values
            //***
            if (currDiff != 0.0)
            {
               if ((step != currDiff) && !resetStep)
               {  
                  Range range(minimum,
                              maximum,
                              step);
                  
                  theRanges.push_back(range);
                  
                  minimum = sortedValues[i];
                  
                  resetStep = true;     
               }
               else
               {
                  resetStep = false;
               }
               
               step = currDiff;
               maximum = sortedValues[i];
            }
         }
      }
      
      if (maximum == minimum) step = 0.0;
      
      Range range(minimum,maximum,step);
      
      theRanges.push_back(range);
   }
}
//*****************************************************************************
// RangeList::range
//*****************************************************************************
const RangeList::Range& MIC::RangeList::range(size_t index) const
{
   const size_t NUM_RANGES = size();
   
   if (!NUM_RANGES)
   {
      throw Error(Error::BOUNDS,
                  "Current range list is empty!",
                  "MIC::RangeList::range");
      
   }
   
   if (index >= NUM_RANGES)
   {
      std::ostringstream err;
      err << "Specified index is outside valid "
          << " range [0-" << NUM_RANGES-1
          << "]";
      
      throw Error(Error::INDEX_OUT_OF_RANGE,err.str(),"MIC::RangeList::range");
   }
   
   return theRanges[index];
}

//*****************************************************************************
// RangeList::range
//*****************************************************************************
const Data::Type& MIC::RangeList::type(size_t index) const
{
   const size_t NUM_RANGES = size();
   
   if (!NUM_RANGES)
   {
      throw Error(Error::BOUNDS,
                  "Current range list is empty!",
                  "MIC::RangeList::type");
      
   }
   
   if (index >= NUM_RANGES)
   {
      std::ostringstream err;
      err << "Specified index is outside valid "
          << " range [0-" << NUM_RANGES-1
          << "]";
      
      throw Error(Error::INDEX_OUT_OF_RANGE,err.str(),"MIC::RangeList::type");
   }
   
   return theRanges[index].type();
}

//*****************************************************************************
// RangeList::contains
//*****************************************************************************
bool RangeList::contains(const MIC::Data& argValue) const
{
   const size_t NUM_RANGES = size();
   
   for (size_t i = 0; i < NUM_RANGES; ++i)
   {
      const Range& range = theRanges[i];
      
      if (range.contains(argValue)) return true;
   }
   
   return false;
}
//*****************************************************************************
// operator<<(std::ostream&,const Data&)
//*****************************************************************************
std::ostream& operator<<(std::ostream& os,const Data& argData)
{
   switch(argData.type())
   {
   case Data::STRING_TYPE : os <<  argData.strValue(); break;
   case Data::INT_TYPE    : os <<  argData.intValue(); break;
   case Data::DOUBLE_TYPE : os <<  argData.dblValue(); break;
   }
   
   return os;
}
//*****************************************************************************
// operator<<(std::ostream&,const RangeList::Range&)
//*****************************************************************************
std::ostream& operator<<(std::ostream& os,const RangeList::Range& range)
{
   os << (range.minInclusive() ? "[" : "(")
      << range.minimum()
      << ","
      << range.maximum()
      << (range.maxInclusive() ? "]" : ")");
   
   Data::Type rtype = range.minimum().type();
   
   if (rtype == Data::INT_TYPE)
   {
      if (range.step().intValue() > 1)
      {
         os << " (step " << range.step() << ")";
      }
   }
   else if (rtype == Data::DOUBLE_TYPE)
   {
      if (range.step().dblValue() != 0.0)
      {
         os << " (step " << range.step() << ")";
      }
   }
   
   return os;
}
//*****************************************************************************
// operator<<(std::ostream&,const RangeList::Range&)
//*****************************************************************************
std::ostream& operator<<(std::ostream& os,const RangeList& rangeList)
{
   const size_t NUM_RANGES = rangeList.size();
   
   for (size_t i = 0; i < NUM_RANGES; ++i)
   {
      os << rangeList.range(i);
      
      if (i < (NUM_RANGES -1)) os << " , ";
   }
   
   return os;
}
} // namespace MIC
//*****************************************************************************
// ModelIdentifier::ModelIdentifier
//*****************************************************************************
ModelIdentifier::ModelIdentifier()
   :
      theComponents()
{}
//*****************************************************************************
// ModelIdentifier::ModelIdentifier
//*****************************************************************************
 ModelIdentifier::ModelIdentifier(const std::string& argName,
                                  const std::string& argValue)
   :
      theComponents()
{
   theComponents.insert(MIComponent(argName,MIC::Data(argValue)));
}
//*****************************************************************************
// ModelIdentifier::ModelIdentifier
//*****************************************************************************
ModelIdentifier::ModelIdentifier(const std::string& argName,
                                 int                argValue)
   :
      theComponents()
{
   theComponents.insert(MIComponent(argName,MIC::Data(argValue)));
}
//*****************************************************************************
// ModelIdentifier::ModelIdentifier
//*****************************************************************************
ModelIdentifier::ModelIdentifier(const std::string& argName,
                                 double             argValue)
   :
      theComponents()
{
   theComponents.insert(MIComponent(argName,MIC::Data(argValue)));
}
//*****************************************************************************
// ModelIdentifier::ModelIdentifier
//*****************************************************************************
ModelIdentifier::ModelIdentifier(const MIComponent& argComponent)
   :
      theComponents()
{
   theComponents.insert(argComponent);
}
//*****************************************************************************
// ModelIdentifier::ModelIdentifier
//*****************************************************************************
ModelIdentifier::ModelIdentifier(const std::vector<MIComponent>& argComponents)
   :
      theComponents()
{
   const size_t NUM_VALUES = argComponents.size();
   
   for (size_t i = 0; i < NUM_VALUES; ++i)
   {
      theComponents.insert(argComponents[i]);
   }
}
//*****************************************************************************
// ModelIdentifier::addComponent
//*****************************************************************************
bool ModelIdentifier::addComponent(const std::string& argName,
                                   const std::string& argValue)
{
   MIComponent component(argName,MIC::Data(argValue));
   
   return addComponent(component);
}
//*****************************************************************************
// ModelIdentifier::addComponent
//*****************************************************************************
bool ModelIdentifier::addComponent(const std::string& argName,
                                   int                argValue)
{
   MIComponent component(argName,MIC::Data(argValue));
   
   return addComponent(component);
}
//*****************************************************************************
// ModelIdentifier::addComponent
//*****************************************************************************
bool ModelIdentifier::addComponent(const std::string& argName,
                                   double             argValue)
{
   MIComponent component(argName,MIC::Data(argValue));
   
   return addComponent(component);
}
//*****************************************************************************
// ModelIdentifier::addComponent
//*****************************************************************************
bool ModelIdentifier::addComponent(const MIComponent& argComponent)
{
   return (theComponents.insert(argComponent).second);
}
//*****************************************************************************
// ModelIdentifier::componentNames
//*****************************************************************************
std::vector<std::string> ModelIdentifier::componentNames() const
{
   std::vector<std::string> names;
   names.reserve(theComponents.size());
   
   MIComponentMap::const_iterator END = theComponents.end();
   
   for (MIComponentMap::const_iterator iter = theComponents.begin();
        iter != END; ++iter)
   {
      names.push_back(iter->first);
   }
   
   return names;
}
//*****************************************************************************
// ModelIdentifier::dataFor
//*****************************************************************************
const MIC::Data& ModelIdentifier::dataFor(const std::string& name) const
{
   MIComponentMap::const_iterator iter = theComponents.find(name);
   
   if (iter != theComponents.end())
   {
      return iter->second;
   }
   
   std::ostringstream err;
   
   err << "Specified name : " << name << "not found!";
   
   throw Error(Error::INDEX_OUT_OF_RANGE,err.str(),"ModelIdentifier::dataFor");
}

//*****************************************************************************
// MIDescription::MIDescription
//*****************************************************************************
MIDescription::MIDescription()
:
      theDescriptions()
{}
//*****************************************************************************
// MIDescription::MIDescription
//*****************************************************************************
MIDescription::MIDescription(const std::string&    argName,
                             const MIC::RangeList& argDesc)
   :
      theDescriptions()
{
   theDescriptions.insert(MIComponentDesc(argName,argDesc));
}
//*****************************************************************************
// MIDescription::MIDescription
//*****************************************************************************
MIDescription::MIDescription(const MIComponentDesc& argDesc)
   :
      theDescriptions()
{
   theDescriptions.insert(argDesc);
}
//*****************************************************************************
// MIDescription::MIDescription
//*****************************************************************************
MIDescription::MIDescription(const std::vector<MIComponentDesc>& argDescs)
   :
      theDescriptions()
{
   const size_t NUM_VALUES = argDescs.size();
   
   for (size_t i = 0; i < NUM_VALUES; ++i)
   {
      theDescriptions.insert(argDescs[i]);
   }
}
//*****************************************************************************
// MIDescription::addDescription
//*****************************************************************************
bool MIDescription::addDescription(const std::string&    argName,
                                   const MIC::RangeList& argDesc)
{
   MIComponentDesc componentDesc(argName,argDesc);
   return addDescription(componentDesc);
}
//*****************************************************************************
// MIDescription::addDescription
//*****************************************************************************
bool MIDescription::addDescription(const MIComponentDesc& argDesc)
{
   return (theDescriptions.insert(argDesc).second);
}
//*****************************************************************************
// MIDescription::componentNames
//*****************************************************************************
std::vector<std::string> MIDescription::componentNames() const
{
   std::vector<std::string> names;
   names.reserve(theDescriptions.size());
   
   MIComponentDescMap::const_iterator END = theDescriptions.end();
   
   for (MIComponentDescMap::const_iterator iter = theDescriptions.begin();
        iter != END; ++iter)
   {
      names.push_back(iter->first);
   }
   
   return names;
}
//*****************************************************************************
// MIDescription::rangesFor
//*****************************************************************************
const MIC::RangeList& MIDescription::rangesFor(const std::string& name) const
{
   MIComponentDescMap::const_iterator iter = theDescriptions.find(name);
   
   if (iter != theDescriptions.end())
   {
      return iter->second;
   }
   
   std::ostringstream err;
   
   err << "Specified name : " << name << "not found!";
   
   throw Error(Error::INDEX_OUT_OF_RANGE,err.str(),"MIDescription::rangesFor");
}
//*****************************************************************************
// MIDescription::inRange
//*****************************************************************************
bool MIDescription::inRange(const ModelIdentifier& id) const
{
   //***
   // Since the current set of descriptions are necessary to uniqely define
   // a model, make sure that the specified ModelIdentifier has a value for
   // each required component which falls in that Component's allowable range.
   //***
   if (id.size() < size())
   {
      return false;
   }
   
   const MIComponentMap& idComponents = id.components();
   MIComponentMap::const_iterator ID_END = idComponents.end();
   
   MIComponentDescMap::const_iterator END = theDescriptions.end();
   
   bool allInRange = true;
   
   for (MIComponentDescMap::const_iterator iter = theDescriptions.begin();
        iter != END && allInRange; ++iter)
   {
      const MIC::RangeList& rList = iter->second;
      
      MIComponentMap::const_iterator idIter = idComponents.find(iter->first);
      
      if (idIter != ID_END)
      {
         if (!rList.contains(idIter->second)) allInRange = false;
      }
      else
      {
         allInRange = false;
      }
   }
   
   return allInRange;
}
}  // namespace csm
