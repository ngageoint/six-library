//#############################################################################
//
//    FILENAME:          NitfIsd.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the NITF 2.0 and 2.1 ISD classes derived from the csm::Isd
//    base class.  This class provides portions of the NITF file that are
//    useful for initializing a model.
//
//    LIMITATIONS:       None
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     01-Jul-2003   LMT      Initial version.
//     06-Feb-2004   KRW      Incorporated changes approved by
//                             January and February configuration
//                             control board.
//     01-Oct-2004   KRW      October 2004 CCB
//     02-Mar-2012   SCM      Refactored interfaces.
//     29-Oct-2012   SCM      Fixed NitfIsd to return const vector references.
//     30-Oct-2012   SCM      Renamed to NitfIsd.h
//     06-Dec-2012   JPK      Made Filename parameter optional, replaced
//                            TRE and DES with Tre and Des for consistency
//     17-Dec-2012   BAH      Documentation updates.
//     10-Sep-2013   SCM      Added std namespace to atoi() calls.
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_NITFISD_H
#define __CSM_NITFISD_H

#include "Isd.h"

#include <vector>
#include <cstdlib>

namespace csm
{

//***
// CLASS: Des
//> NITF Data Extension Segment (DES) class.  DESs can be used to hold overflow
//  TREs that did not fit in a NITF image segment.
//<
//***
class CSM_EXPORT_API Des
{
public:
   Des() : theSubHeader(), theData() {}
   Des(const std::string& aSubHeader, const std::string& aData)
      : theSubHeader(aSubHeader), theData(aData) {}
   ~Des() {}

   void clear() { theSubHeader = theData = ""; }

   const std::string& subHeader() const { return theSubHeader; }
   const std::string& data() const { return theData; }

   void setSubHeader(const std::string& sh) { theSubHeader = sh; }
   void setData(const std::string& data) { theData = data; }

private:
   std::string theSubHeader;
   std::string theData;
      //> This contains the data in the DES.  It might contain overflow TREs.
      //<
};


//***
// CLASS: Tre
//> NITF Tagged Record Extension (TRE) class.  TREs contain most of the support
//  data needed to instantiate a model.
//<
//***
class CSM_EXPORT_API Tre
{
public:
   Tre() : theName(), theLength(0), theData() {}
   explicit Tre(const std::string& treData)
      : theName(), theLength(0), theData() { setTre(treData); }
   Tre(const std::string& aName, unsigned int aLength, const std::string& aData)
      : theName(aName), theLength(aLength), theData(aData) {}
   ~Tre() {}

   const std::string& name() const   { return theName; }
   const unsigned int length() const { return theLength; }
   const std::string& data() const   { return theData; }

   const void setName(const std::string& aName) { theName = aName; }
   const void setLength(unsigned int aLength)   { theLength = aLength; }
   const void setData(const std::string& aData) { theData = aData; }

   void setTre(const std::string& treData)
   {
      if (treData.length() < 11) return;
      theName = treData.substr(0, 6);
      theLength = std::atoi(treData.substr(6, 5).c_str());
      theData = treData.substr(11);
   }
      //> This method extracts the TRE's name, length and data from treData.
      //
      //  Note that this function extracts all six characters of the TRE name,
      //  which might include trailing spaces.
      //
      //  Note that theLength is set to the value in the TRE's CEL field,
      //  which should match the length of theData.
      //<

   void clear() { theName = ""; theLength = 0; theData = ""; }

private:
   std::string theName;
      //> This string contains the six-character TRE name.
      //<

   unsigned int theLength;
      //> This integer contains the TRE data length given by the TRE's CEL
      //  field, which should be same as the length of theData.
      //<

   std::string theData;
      //> This string contains the TRE data.
      //<
};

//***
// CLASS: Image
//> NITF image subheader class.  The image subheader contains the image support
//  data, including TREs.  The image data section of the NITF file consists of
// the image pixels and is not part of the CSM API.
//<
//***
class CSM_EXPORT_API Image
{
public:
   Image() : theSubHeader(), theImageTres() {}
   Image(const std::string& aSubHeader, const std::vector<Tre>& tres)
      : theSubHeader(aSubHeader), theImageTres(tres) {}
      //> This method constructs the Image object with the given image
      //  subheader data (including the TRE data) and list of parsed TREs.
      //<
   ~Image() {}

   const std::string& subHeader() const { return theSubHeader; }
      //> This method returns the entire image subheader, including a copy of
      //  the TRE data.
      //<

   const std::vector<Tre>& imageTres() const { return theImageTres; }
      //> This method returns the list of image subheader TREs.
      //<

   void setSubHeader(const std::string& sh) { theSubHeader = sh; }
      //> This method sets the entire image subheader, including a copy of
      //  the TRE data.  When using this method, it is important to also set
      //  the separated TREs as necessary using the methods below.
      //<


   void clearImageTres() { theImageTres.clear(); }
      //> This method removes all Tre objects from the list.
      //<

   void addImageTre(const Tre& tre) { theImageTres.push_back(tre); }
      //> This method adds the given Tre object to the list.
      //<

   void setImageTres(const std::vector<Tre>& tres) { theImageTres = tres; }
      //> This method sets the Tre list to the given vector.
      //<

private:
   std::string      theSubHeader;
      //> This string contains the entire image subheader, including a copy of
      //  the TRE data.
      //<

   std::vector<Tre> theImageTres;
      //> This contains the parsed TREs.
      //<
};


//***
// CLASS: NitfIsd
//> NITF ISD class.  This is an intermediary class that the NITF 2.0 and
//  NITF 2.1 ISD classes are derived from.  Do not construct this class
//  directly.  Construct the appropriate derived class to set the NITF format
//  that the calling application needs to parse the data.  Then this class
//  can be used to retrieve the format and access the data.
//<
//***
class CSM_EXPORT_API NitfIsd : public Isd
{
public:
   virtual ~NitfIsd();

   const std::string& fileHeader() const { return theFileHeader; }
      //> This method returns the entire file subheader, including a copy of
      //  the TRE data.
      //<

   const std::vector<Tre>& fileTres() const { return theFileTres; }
      //> This method returns the file header Tre objects.
      //<

   const std::vector<Des>& fileDess() const { return theFileDess; }
      //> This method returns the Des objects in this NITF.
      //<

   const std::vector<Image>& images() const { return theImages; }
      //> This method returns the Image objects in this NITF.
      //<


   void setFileHeader(const std::string& head) { theFileHeader = head; }
      //> This method sets the file header, which can include TRE data.
      //  When using this method, it is important to also set the separated
      //  file header TREs as necessary using the methods below.
      //<


   void clearFileTres() { theFileTres.clear(); }
      //> This method removes all file header Tre objects from the list.
      //<

   void addFileTre(const Tre& tre) { theFileTres.push_back(tre); }
      //> This method adds the given file header Tre object to the list.
      //<

   void setFileTres(const std::vector<Tre>& tres) { theFileTres = tres; }
      //> This method sets the file header Tre list to the given vector.
      //<


   void clearFileDess() { theFileDess.clear(); }
      //> This method removes all Des objects from the list.
      //<

   void addFileDes(const Des& des) { theFileDess.push_back(des); }
      //> This method adds the given Des object to the list.
      //<

   void setFileDess(const std::vector<Des>& dess) { theFileDess = dess; }
      //> This method sets the Des list to the given vector.
      //<


   void clearImages() { theImages.clear(); }
      //> This method removes all Image objects from the list.
      //<

   void addImage(const Image& image) { theImages.push_back(image); }
      //> This method adds the given Image object to the list.
      //<

   void setImages(const std::vector<Image>& images) { theImages = images; }
      //> This method sets the Image list to the given vector.
      //<

protected:
   NitfIsd(const std::string& format, const std::string& filename = "")
      : Isd(format,filename),theFileHeader(),theFileTres(),theFileDess(),theImages() {}

private:
   std::string theFileHeader;
      //> This string contains the full file header text, including a copy of
      //  the file level TRE data.
      //<

   std::vector<Tre>   theFileTres;
   std::vector<Des>   theFileDess;
   std::vector<Image> theImages;
};

class CSM_EXPORT_API Nitf20Isd : public NitfIsd
{
public:
   Nitf20Isd(const std::string& filename = "") : NitfIsd("NITF2.0", filename) {}
   virtual ~Nitf20Isd();
};

class CSM_EXPORT_API Nitf21Isd : public NitfIsd
{
public:
   Nitf21Isd(const std::string& filename = "") : NitfIsd("NITF2.1", filename) {}
   virtual ~Nitf21Isd();
};

} // namespace csm

#endif

