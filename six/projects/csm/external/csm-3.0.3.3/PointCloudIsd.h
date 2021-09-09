//#############################################################################
//
//    FILENAME:          PointCloudIsd.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for LAS and BPF ISD classes derived from the csm::Isd
//    base class.  This class provides portions of the LAS file that are
//    useful for initializing a model.
//
//    LIMITATIONS:       None
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     01-DEC-2014   ISK      Initial version.
//     22-FEB-2018   JPK      Changed macro for inclusion into csm library.
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_POINT_CLOUD_ISD_H
#define __CSM_POINT_CLOUD_ISD_H

#include "Isd.h"
#include "csmPointCloud.h"

#include <vector>
#include <cstdlib>

namespace csm
{
//***
// CLASS: Vlr
//> Variable Length Record (VLR) class.  VLRs contain most of the support
//  data needed to instantiate a model.
//<
//***
class CSM_EXPORT_API Vlr
{
public:
   Vlr() : theUserId(),
           theRecordId(0),
           theRecordLength(0),
           theDescription(),
           theData() {}

   Vlr(
      const std::string& aUserId,
      unsigned short     aRecordId,
      unsigned long long aLength,
      const std::string& aDescription,
      const std::string& aData )
      : theUserId(aUserId),
        theRecordId(aRecordId),
        theRecordLength(aLength),
        theDescription(aDescription),
        theData(aData) {}

   ~Vlr();

   const std::string&        userId()      const { return theUserId; }
   const unsigned short&     recordId()    const { return theRecordId; }
   const unsigned long long& length()      const { return theRecordLength; }
   const std::string&        description() const { return theDescription; }
   const std::string&        data()        const { return theData; }

   void setUserId(const std::string& aUserId)
   { theUserId = aUserId; }

   void setRecordId(unsigned int aRecordId)
   { theRecordId = aRecordId; }

   void setLength(unsigned int aLength)
   { theRecordLength = aLength; }

   void setDescription(const std::string& aDescription ) 
   { theDescription = aDescription; }

   void setData(
      const std::string& aData )
   { theData = aData; }

   void set(
      const std::string& aUserId,
      unsigned short     aRecordId,
      unsigned long long aLength,
      const std::string& aDescription,
      const std::string& aData )
   {
      theUserId       = aUserId;
      theRecordId     = aRecordId;
      theRecordLength = aLength;
      theDescription  = aDescription;
      theData         = aData;
   }

   void clear()
   { 
      theUserId       = "";
      theRecordId     = 0;
      theRecordLength = 0;
      theDescription  = "";
      theData         = "";
   }

   // For BPF nomenclature
   void setFilename( const std::string& aFilename )
   { theDescription = aFilename; }

   const std::string& filename() const { return theDescription; }

   void setMagicNumber( const std::string& aMagicNumber )
   { theUserId = aMagicNumber; }

   const std::string& magicNumber() const { return theUserId; }


protected:

   std::string theUserId;
   //> This string contains the user id of the VLR.
   //<

   unsigned short theRecordId;
   //> This integer contains the record identifier.
   //<

   unsigned long long theRecordLength;
   //> This integer contains the record length. 
   //  It is of type long long to allow for extended VLRs.
   //<

   std::string theDescription;
   //> This string contains the VLR description.
   //<

   std::string theData;
   //> This string contains the VLR data.
   //<
};


//***
// CLASS: PointCloudIsd
//> LAS ISD class.  This is an intermediary class that the LAS and
//  BPF ISD classes are derived from.  Do not construct this class
//  directly.  Construct the appropriate derived class to set the format
//  that the calling application needs to parse the data.  Then this class
//  can be used to retrieve the format and access the data.
//<
//***
class CSM_EXPORT_API PointCloudIsd : public Isd
{
public:
   virtual ~PointCloudIsd();

   const std::string& fileHeader() const { return theFileHeader; }
   //> This method returns the entire file subheader.
   //<

   const std::vector<Vlr>& vlrList() const { return theVlrs; }
   //> This method returns the file header VLR objects.
   //<

   void addVlr( const Vlr &aVrl )
   { theVlrs.push_back( aVrl ); }
   //> This method adds a VLR to the VLR list.
   //<

   void setFileHeader(const std::string& head) { theFileHeader = head; }
   //> This method sets the file header. The interprtation of the header 
   //  depends upon the format setting.
   //<

protected:
   PointCloudIsd(
      const std::string& format,
      const std::string& filename = "")
      : Isd(format,filename), theFileHeader(), theVlrs() {}

private:
   std::string theFileHeader;
   //> This string contains the full file header text.
   //<

   std::vector<Vlr> theVlrs;
};

class CSM_EXPORT_API LasIsd : public PointCloudIsd
{
public:
   LasIsd(
      const std::string& filename = "")
      : PointCloudIsd("PointCloudLAS", filename) {}
   virtual ~LasIsd();
};

class CSM_EXPORT_API BpfIsd : public PointCloudIsd
{
public:
   BpfIsd(
      const std::string& filename = "")
      : PointCloudIsd("PointCloudBPF", filename) {}
   virtual ~BpfIsd();
};

} // namespace csm

#endif

