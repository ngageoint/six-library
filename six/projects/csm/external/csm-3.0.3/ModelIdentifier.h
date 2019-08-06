//#############################################################################
//
//    FILENAME:          ModelIdentifier.h
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
//     20-Feb-2017   JPK      Initial creation
//     01-Mar-2017   JPK      Moved implementaitons to ModelIdentifier.cpp
//                            Fixed includes, namespace usage.
//<
//#############################################################################
#ifndef __CSM_MODEL_IDENTIFIER_H
#define __CSM_MODEL_IDENTIFIER_H

#include "csm.h"
#include <string>
#include <vector>
#include <map>
#include <ostream>

namespace csm
{
namespace MIC // Namespace for Model Identifier Components
{
//*****************************************************************************
// Data is a class for holding values of various types for a single
// component of a Model Identifier
//*****************************************************************************
class CSM_EXPORT_API Data
{
public:
   
   enum Type
   {
      STRING_TYPE,
      INT_TYPE,
      DOUBLE_TYPE
   };
      //> This enumeration indicated the data type for
      //  the Model Identifier
      //<

   
   Data()                : theType(INT_TYPE),theInt(0) {}
      //> The default constructor is declared private so that it cannot be
      //  invoked.
      //<
   
   Data(int argValue)    : theType(INT_TYPE), theInt(argValue) {}
      //> This constructor creates an MIData object from an integer
      //<
   
   Data(double argValue) : theType(DOUBLE_TYPE),theDbl(argValue){}
      //> This contructor creates an MIData object from a double
      //<
   
   Data(const std::string& argValue);
      //> This constructor creates an MIData object from a string
      //<
   
   Data(const Data& argData);
      //> This is the copy constructor
      //<
   
   ~Data();
      //> This is the destructor.
      //<
   
   std::string strValue() const;
      //> This method returns the current Data's value as a string
      //<
   int         intValue() const;
      //> This method returns the current Data's value as an integer
      //< 
   double      dblValue() const;
      //> This method returns the current Data's value as a double
      //<
   
   const Type& type() const {return theType;}
      //> This method returns the enumerated Type for the current Data
      //<
   
   void setValue(const std::string& value,bool clearPrevious = true);
      //> This method sets the value to the argument string.
      //  and sets the Type to STRING_TYPE.  If clearPrevious is set to
      //  true, any previous value is removed.
      //<
   
   void setValue(int value,bool clearPrevious = true);
      //> This method sets the value to the argument int.
      //  and sets the Type to INTEGER_TYPE  If clearPrevious is set to
      //  true, any previous value is removed.
      //<
   
   void setValue(double value,bool clearPrevious = true);
      //> This method sets the value to the argument double.
      //  and sets the Type to DOUBLE_TYPE  If clearPrevious is set to
      //  true, any previous value is removed.
      //<
   
   bool operator<(const Data& argData) const;
      //> This method returns "true" if the argument data is less than
      //  the current data, "false" otherwise.  Note that a STRING_TYPE
      //  is always less than an INT_TYPE or a DOUBLE_TYPE.  For any
      //  other comparisons, operator< on the actual types is returned.
      //<
   
   bool operator==(const Data& argData) const;
      //> This method returns "true" if argData is equal to the
      //  current Data.  Note that a STRING_TYPE is never
      //  equivalent to and INT_TYPE, but INT_TYPES and DOUBLE_TYPES
      //  can be equivalent if the values they hold are.
      //<
   
   Data& operator=(const Data&);
      //> This is the assignment operator
      //<
private:
   
   void clear();
   
   Type  theType;
      //> This data member is the enumerated Type
      //<
   union
   {
      struct{size_t theLen; char*  theStr;};
      int    theInt;
      double theDbl;
   };
};

//*****************************************************************************
// RangeList
//> This class represents the set of possible ranges for the value
//  associated with a single "component" of a Model Identifier.
//<
//*****************************************************************************
class CSM_EXPORT_API RangeList
{
public:
   
   //************************************************************************
   // Class Range
   //> This class represents a single range of possible values for a Data
   //<
   //************************************************************************
   class CSM_EXPORT_API Range
   {
   public:
      
      Range(); 
         //> This is the default constructor.
         //< 
      
      Range(const std::string& single);
         //> This constructor creates a range equal to a single "string"
         //  value.
         //<
      
      Range(int  minValue,
            int  maxValue,
            int  stepValue = 1);
         //> This constructor creates a range from specified minimum value
         //  to specified maximum value, given a specified step.
         //<
      
      Range(double minValue,
            double maxValue,
            double stepValue   = 0.0,
            bool   includesMin = true,
            bool   includesMax = true);
         //> This constructor creates a range from specified minimum value
         //  to specified maximum value, given a specified step.
         //<    
      
      ~Range() {};
         //> This is the destructor
         //<
      
      const Data::Type& type() const {return theMinimum.type();}
         //> This method returns the type associated with the data elements
         //  for this Range.
         //<
      const Data& minimum() const {return theMinimum;}
         //> This method returns the minimum value for the current range.
         //<
      const Data& maximum() const {return theMaximum;}
         //> This method returns the maximum value for the current range.
         //<
      const Data& step()    const {return theStep;}
         //> This method returns the step size for the current range.
         //  If the step is empty or 0 / 0.0, it is ignored. 
         //<
      bool minInclusive() const {return theMinInclusive;}
         //> This data member indicates if the minimum value is included
         //  in the range.
         //<
      bool maxInclusive() const {return theMaxInclusive;}
         //> This data member indicates if the maximum value is included
         //  in the range.
         //<
      bool contains(const Data& argValue) const;
         //> This method determines if the specified data is in the current
         //  range.
         //<
      bool contains(const std::string& argValue) const;
         //> This method determines if the specified string is in the current
         //  range, which will only be true if the current data type is
         //  of type STRING_TYPE
         //<
      bool contains(int argValue) const;
         //> This method determines if the specified integer is in the current
         //  range, which will only be true if the current data type is
         //  of type INT_TYPE
         //<
      bool contains(double argValue) const;
         //> This method determines if the specified integer is in the current
         //  range, which will only be true if the current data type is
         //  of type DOUBLE_TYPE
         //<
      bool operator<(const Range& argRange) const;
         //> This method returns "true" if the argument Range is less than
         //  the current Range, "false" otherwise.  Note that a STRING_TYPE
         //  Range is always less than an INT_TYPE or a DOUBLE_TYPE.  For any
         //  other comparisons, operator< on the actual types is returned.
         //<
      bool operator==(const Range& argRange) const;
         //> This method returns "true" if argRange is equal to the
         //  current Range.  Note that a STRING_TYPE is never
         //  equivalent to and INT_TYPE, but INT_TYPES and DOUBLE_TYPES
         //  can be equivalent if the values they hold are.
         //<
      
      private:
      
      void adjustToStep();
         //> This method adjusts the range to agree with the step size if a
         //  valid step size is specified.
         //<
      Data theMinimum;
         //> This data member represents the minimum value assoicated with the
         //  current range.
         //<
      Data theMaximum;
         //> This data member represents the maximum value assoicated with the
         //  current range.
         //<
      Data theStep;
         //> This data member represents the step size assoicated with the
         //  current range.
         //<
      bool theMinInclusive;
         //> This data member indicates of the current minimum value is
         //  included in the range.
         //<
      bool theMaxInclusive;
         //> This data member indicates of the current maximum value is
         //  included in the range.
         //<
   };
   
   RangeList() : theRanges() {}
      //> This is the default constructor
      //<
   RangeList(const std::string& single);
      //> This constructor creates a RangeList with a single "string"
      //  value.
      //<
   RangeList(int  minValue,
             int  maxValue,
             int  stepValue = 1);
      //> This constructor creates a RangeList with a single "integer"
      //  range from specified minimum value
      //  to specified maximum value, given a specified step.
      //<
   RangeList(double minValue,
             double maxValue,
             double stepValue   = 0.0,
             bool   includesMin = true,
             bool   includesMax = true);
      //> This constructor creates a RangeList from a single "double"
      //  range from specified minimum value
      //  to specified maximum value, given a specified step.
      //<    
   RangeList(const Range& argRange);
      //> This constructor creates an MICRangeList single range.
      //<
   RangeList(const std::vector<std::string>& argValues);
      //> This constructor creates an MICRangeList from the given list
      //  of possible "string" values.
      //<
   RangeList(const std::vector<int>& argValues);
      //> This constructor creates an MICRangeList from the given list
      //  of possible "integer" values.  The values are sorted and then
      //  assigned to ranges based on the distance between the adjacent
      //  values.
      //<
   RangeList(const std::vector<double>& argValues);
      //> This constructor creates an MICRangeList from the given list
      //  of possible "double" values.  The values are sorted and then
      //  assigned to ranges based on the distance between the adjacent
      //  values.
      //<

   size_t size() const {return theRanges.size();}
      //> This method returns the number of ranges.
      //<
   
   const Range& range(size_t index) const;
      //> This method returns the specified range.  If the passed in
      //  "index" is outside of the valid range, a csm::Error is thrown.
      //<
   const Data::Type& type(size_t index) const;
      //> This method returns the Type of the specified Range.
      //  Note that it is possible for different types of
      //  Ranges to be present for the same Data object.
      //  For example, a "Band ID" could be expressed as an
      //  integer index, a wavelength (non-integer number), or
      //  a name (Red,Green,Blue, etc).
      //<
   
   ~RangeList() {}
      //> This is the destuctor
      //<
   
   void addRange(const Range& argRange) {theRanges.push_back(argRange);}
      //> This method allows a range to be added to the current RangeList.
      //<
   bool contains(const Data& argValue) const;
      //> This method determined if any of the ranges in the list contain the
      //  specified value.
      //<
   
private:

   std::vector<Range> theRanges;
      //> this data member represents the set of Ranges own by the current
      //  MICRangeList.
      //<
};
std::ostream& operator<<(std::ostream& os,const Data& argData);
std::ostream& operator<<(std::ostream& os,const RangeList::Range& range); 
std::ostream& operator<<(std::ostream& os,const RangeList& rangeList);

} // namespace MIC

typedef std::map<std::string,MIC::Data> MIComponentMap;
typedef MIComponentMap::value_type MIComponent;

typedef std::map<std::string,MIC::RangeList> MIComponentDescMap;
typedef MIComponentDescMap::value_type  MIComponentDesc;

//*****************************************************************************
// ModelIdentifier
//> This class holds a set of "Components" which are intented to uniquely
//  identify a specific Model within a "bundle" of models.
//<
//*****************************************************************************
class CSM_EXPORT_API ModelIdentifier
{
public:   
   ModelIdentifier();
      //> This is the default constructor
      //<
   ModelIdentifier(const std::string& argName,
                   const std::string& argValue);
      //> This constructor creates a ModelIdentifier from the specified
      //  component name and string value.
      //<
   ModelIdentifier(const std::string& argName,
                   int                argValue);
      //> This constructor creates a ModelIdentifier from the specified
      //  component name and integer value.
      //<
   ModelIdentifier(const std::string& argName,
                   double             argValue);
      //> This constructor creates a ModelIdentifier from the specified
      //  component name and double value.
      //<
   ModelIdentifier(const std::string& argName,
                   const MIC::Data&   argValue);
      //> This constructor creates a ModelIdentifier from the specified
      //  component name and MIC::Data value.
      //<
   ModelIdentifier(const MIComponent& argComponent);
      //> This constructor creates a ModelIdentifier given a single
      //  MIComponent.
      //<
   ModelIdentifier(const std::vector<MIComponent>& argComponents);
      //> This constructor creates a ModelIdentifier from the supplied vector
      //  of MIComponents.
      //<

   ~ModelIdentifier() {}
      //> This is the destructor.
      //<

   bool addComponent(const std::string& argName,
                     const std::string& argValue);
      //> This method adds a new string component with name argName
      //  and value argValue to the current ModelIdentifier.
      //<
   bool addComponent(const std::string& argName,
                     int                argValue);
      //> This method adds a new integer component with name argName
      //  and value argValue to the current ModelIdentifier.
      //<
   bool addComponent(const std::string& argName,
                     double             argValue);
      //> This method adds a new double component with name argName
      //  and value argValue to the current ModelIdentifier.
      //<
   bool addComponent(const MIComponent& argComponent);
      //> This method allows a particular MIComponent to be added the this
      //  ModelIdentifier.  If the argument Component's name is equivalent
      //  to the name of a Component already owned by this ModelIdentifier,
      //  "false" is returned.  Otherwise, the argument Component is added
      //  to this ModelIdentifier, and "true" is returned.
      //<
   
   size_t size() const {return theComponents.size();}
      //> This method returns the current number of components
      //<
   
   std::vector<std::string> componentNames() const;
      //> This method returns the names for each Component.
      //<
   const MIC::Data& dataFor(const std::string& name) const;
      //> This method returns the Data associated with a specific
      //  Component.
      //<
   
   const MIComponentMap& components() const {return theComponents;}
      //> This method returns the current set of Components
      //<
   
   private:
   
   MIComponentMap theComponents;
      //> This data member stores the current set of Components.
      //<
};

//*****************************************************************************
// MIDescription
//> This class holds a set of "Component Descriptions" which define the valid
//  ranges for the "Components" of a ModelIdentifier.
//<
//*****************************************************************************
class CSM_EXPORT_API MIDescription
{
public:
   
   MIDescription();
      //> This is the default constructor
      //<
   MIDescription(const std::string& argName,
                 const MIC::RangeList& argDesc);
      //> This constructor creates an MIDescription given a single Component
      //  wth name argName and MIC::RangeList argDesc.
      //<
   MIDescription(const MIComponentDesc& argDesc);
      //> This constructor creates an MIDescription given a single Component
      //  description.
      //<
   MIDescription(const std::vector<MIComponentDesc>& argDescs);
      //> This constructor creates an MIDescription from the supplied
      //  vector of component descriptions
      //<

   ~MIDescription() {}
      //> This is the destructor
      //<
   
   bool addDescription(const std::string&    argName,
                       const MIC::RangeList& argDesc);
      //> This method allows a RangeList for the value named by argName
      //  to be added to the current MIDescription.
      //<
   bool addDescription(const MIComponentDesc& argDesc);
      //> This method allows a particular MIComponentDesc to be added the this
      //  MIDescription.  If the argument ComponentDesc's name is equivalent
      //  to the name of a Component already owned by this ModelIdentifier,
      //  "false" is returned.  Otherwise, the argument ComponentDesc is added
      //  to this MIDescription, and "true" is returned.
      //<

   size_t size() const {return theDescriptions.size();}
      //> This method returns the number of descriptions
      //<
   
   std::vector<std::string> componentNames() const;
    //> This method returns the names for each ComponentDesc.
    //<

   const MIC::RangeList& rangesFor(const std::string& name) const;
    //> This method returns the RangeList for each Component name..
    //<
   const MIComponentDescMap& descriptions() const {return theDescriptions;}
      //> This method returns the current set of ComponentDescs.
      //<

   bool inRange(const ModelIdentifier& id) const;
      //> This method returns "true" if the specified ModelIdentifier's
      //  components are each found in the current set of descritions and
      //  each Components's value is contained in its associated range.
      //<
   
private:
   
   MIComponentDescMap theDescriptions;
      //> This data member stores the current set of ComponentDescs.
      //<
};
}  // namespace csm

#endif
