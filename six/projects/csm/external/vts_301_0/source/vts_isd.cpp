//#############################################################################
//
//    FILENAME:   vts_isd.cpp
//
//
//    DESCRIPTION:
//	This module contains functions to support CSMISD classes
//	These classes are bytestream, filename, NITF 2.0 and NITF 2.1
//
//  NOTES:
//  SOFTWARE HISTORY:
//      Date        Author Comment
//      13-Jan-2004  LMT   Initial version.
//      09 Mar 2010  DSL   CCB Change Deleted DATA_NOT_AVAILABLE Error
//      12 Aug 2010  DSL   CCB Change add writeStateFile and readStateFile
//      19 Jan 2011  DSL   changed parseTre to remove hardcoded length of vTreLengths vector
//                         changed "delete var" to "delete [] var" when var was an array
//      19 Jan 2011  DSL   changed getSegment calls in parseImages functions to only read
//                         image headers
//      25 Jan 2011  DSL   ensure closure of input file on exception
//      10 Feb 2011  DSL   Add capability to process large files (2G - 4G)
//      29 Aug 2011  DSL   Handle large pointers to DES records at the end of large files
//      10 Jan 2013  DSL   changes to reflect C++ reformat CRs
//      23 Jan 2013  DSL   In parseTre, changed pTreSegment from char* to string. This avoids 
//                         truncation when passing it to setTre if the the TRE contains embedded NULLs.
//      28 Jan 2013  DSL   VTS_VERSION CSM3.0.1.0


//#############################################################################

#ifdef _WIN32
#pragma warning( disable : 4290 )
#endif
#include "Error.h"
#include "Warning.h"
#include "VTSMisc.h"
#include "Isd.h"
#include "NitfIsd.h"
#include "BytestreamIsd.h"
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>

#ifdef IRIXN32
#include <stdio.h>
#else
#include <cstdio>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <sys/Path.h>

using namespace csm;
using namespace std;

extern std::string logFile;
bool nitfDebugFlag = false;        //----- NITF Debug Flag

/*
///////////////////////////////////////////////////////////////
//
//  Function: initFilenameISD
//
//  Description: Initialize a filename for reading
//
//  Inputs: isdfilename - File data structure
//          filename    - name of file to be opened
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void initFilenameISD( Isd *isdfilename,
                      std::string filename)
{
   isdfilename->setFilename(filename);
}
*/

///////////////////////////////////////////////////////////////
//
//  Function: initBytesteamISD
//
//  Description: Initialize a bytestream for reading
//
//  Inputs: bytesteamISD - Byte Stream data structure
//          filename     - name of byte stream to be opened
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void initBytestreamISD( BytestreamIsd *bytestream,
                        std::string filename)
   throw (Error)
{
   Error err;
   const size_t bufflen = 1048576;
   char *buff = new char[bufflen];
   size_t numread = 0;
   size_t totread = 0;

   FILE *ifp = NULL;
#ifdef _WIN32
   ifp = fopen (filename.c_str(), "rb");
#else
   ifp = fopen64 (filename.c_str(), "rb");
#endif
   
   bytestream->setData(""); // empty the string
   //bytestream->_isd = ""; // empty the string

   if (ifp == NULL)
   {
      err.setError( Error::FILE_READ,
		       "Unable to open file " + filename,
		       "initBytestreamISD" );
      throw err;
   }

   if (!buff)
   {
      err.setError( Error::MEMORY,
		       "Unable to allocate bytestream buffer",
		       "initBytestreamISD" );
	  if(ifp)
	  {
		  fclose(ifp);
	  }
      throw err;
   }

   while ((numread = fread ( buff, 1, bufflen, ifp)) == (int)bufflen)
   {
      totread += numread;
      bytestream->setData(bytestream->data() + std::string(buff, bufflen));
   }

   if (numread > 0)
   {
      totread += numread;
      bytestream->setData(bytestream->data() + std::string(buff, numread));
   }

   //bytestream->data.erase (totread);   //  todo: no access to erase last byte
   delete [] buff;
   if(ifp)
   {
      fclose(ifp);
   }

   return;
}

///////////////////////////////////////////////////////////////
//
//  Function: dumpbytestream
//
//  Description: Dump the loaded bytestream
//
//  Inputs: bytesteamISD - Byte Stream data structure
//          len          - Amount of data to be dumped
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void dumpbytestream( BytestreamIsd &bytestream,
                     int len)
{
	string bytestreamData = bytestream.data();
   std::cout << "\nLength of bytestream: "
             << bytestreamData.length() << std::endl;
   std::cout << "First " << len << " characters of ISD Bytestream are \n "
             <<  bytestreamData.substr(1,len) << std::endl;
//   std::cout << "\nLength of bytestream: "
//             << bytestream.data.length() << std::endl;
//   std::cout << "First " << len << " characters of ISD Bytestream are \n "
//             <<  bytestream.data.substr(1,len) << std::endl;
}

///////////////////////////////////////////////////////////////
//
//  Function: initNitf20ISD
//
//  Description: Initialize an NITF 2.0 file to be read
//
//  Inputs: isd        - An Nitf data structure
//          fname      - Filename to be loaded
//          imageIndex - Index of an image to be loaded
//
//  Returns: Warning if an error is found
//
///////////////////////////////////////////////////////////////
void initNitf20ISD( Nitf20Isd *isd,
                    std::string& fname,
                    const int imageIndex,
                    WarningList* warnings)
//   throw (Error)
{
   std::string ftype ("NITF20");
   FILE *ifile = NULL;

#ifdef _WIN32
   struct stat statbuf; // to check for file presence and size
#else
   struct stat64 statbuf; // to check for file presence and size
#endif

   char *buff = NULL;

   try {
      ifile = fillBuff(fname, statbuf, &buff);

      parseFile(isd, ifile, statbuf, buff, imageIndex, warnings);
   } catch (Error err) {
      delete [] buff;
	  if(ifile)
	  {
		  fclose(ifile);
	  }
      throw(err);
   }
   if(ifile)
   {
      fclose(ifile);
   }
   delete [] buff;
};

///////////////////////////////////////////////////////////////
//
//  Function: initNitf21ISD
//
//  Description: Initialize an NITF 2.1 file to be read
//
//  Inputs: isd        - An Nitf data structure
//          fname      - Filename to be loaded
//          imageIndex - Index of an image to be loaded
//
//  Returns: Warning if an error is found
//
///////////////////////////////////////////////////////////////
void initNitf21ISD( Nitf21Isd *isd,
                    std::string& fname,
                    const int imageIndex,
                    WarningList* warnings)

//   throw (Error)
{
   FILE *ifile = NULL;
   std::string ftype ("NITF21");
#ifdef _WIN32
   struct stat statbuf; // to check for file presence and size
#else
   struct stat64 statbuf; // to check for file presence and size
#endif
   char *buff = NULL;

   try {
      ifile = fillBuff(fname, statbuf, &buff);

      parseFile(isd, ifile, statbuf, buff, imageIndex, warnings);
   }
   catch (Error err)
   {
      delete [] buff;
      if(ifile)
      {
         fclose(ifile);
      }
      throw(err);
   }
   if(ifile)
   {
      fclose(ifile);
   }
   delete [] buff;
};

///////////////////////////////////////////////////////////////
//
//  Function: fillBuff
//
//  Description: Read in a data buffer
//
//  Inputs: fname   - Name of file to be opened
//          statbuf - File information for the OS
//          buff    - Data read from the requested file
//
//  Returns: File pointer
//
///////////////////////////////////////////////////////////////
static FILE * fillBuff_(const std::string& fname,
#ifdef _WIN32
                 struct stat &statbuf,
#else
				 struct stat64 &statbuf,
#endif
                 char** buff)
{
   Error csmerr;
   FILE *ifile = NULL;
   const size_t MAXNITFFILEHDRLEN = 999999;

#ifdef _WIN32
   off_t buffsize;     // st_size is defined as off_t
   if (stat(fname.c_str(), &statbuf))
#else
   off64_t buffsize;   // st_size is defined as off64_t
   if (stat64(fname.c_str(), &statbuf))
#endif
   {
      csmerr.setError (Error::FILE_READ,
			  "Unable to locate input file " + fname,
			  "CsmNitf20::fillBuff");
      throw (csmerr);
   }
   buffsize = (MAXNITFFILEHDRLEN > statbuf.st_size) ?
      statbuf.st_size : MAXNITFFILEHDRLEN;
   *buff = (char*) calloc (buffsize, sizeof (char));
   if (*buff == NULL)
   {
      csmerr.setError (Error::MEMORY,
			  "Unable to allocate file buffer",
			  "CsmNitf20::fillBuff");
      throw (csmerr);
   }

   // malloc ok
#ifdef _WIN32
   ifile = fopen (fname.c_str(), "rb");
#else
   ifile = fopen64 (fname.c_str(), "rb");
#endif
   if (! ifile)
   {
      std::string errstr ("Unable to open input file " + fname);
      csmerr.setError (Error::FILE_READ,
			  errstr,
			  "CsmNitf20::fillBuff");
      throw (csmerr);
   }
   // open ok
   size_t size = fread (*buff, 1, buffsize, ifile);
   if (size != buffsize)
   {
      std::string errstr ("failure reading input file " + fname);
      csmerr.setError (Error::FILE_READ,
			  errstr,
			  "CsmNitf20::fillBuff");
	  if(ifile)
	  {
		  fclose(ifile);
	  }
      throw (csmerr);
   }

   return ifile;
};
static FILE * fillBuff_expand_(std::string& fname,
#ifdef _WIN32
                 struct stat &statbuf,
#else
				 struct stat64 &statbuf,
#endif
                 char** buff)
{
    // If we're here, the orignal call to fillBuff_() failed; assume that's because
    // the path needs to be expanded.
    auto path = sys::Path::expandEnvironmentVariables(fname, sys::Filesystem::FileType::Regular);
    try
    {
        FILE* retval = fillBuff_(path, statbuf, buff);
        fname = std::move(path);
        return retval;
    }
    catch (const Error&)
    {
        return nullptr; // caller will throw original exception
    }
}

FILE * fillBuff( std::string& fname,
#ifdef _WIN32
                 struct stat &statbuf,
#else
				 struct stat64 &statbuf,
#endif
                 char** buff) throw (Error)
{
    try
    {
        return fillBuff_(fname, statbuf, buff);
    }
    catch (const Error&)
    {
        FILE* retval = fillBuff_expand_(fname, statbuf, buff);
        if (retval == nullptr)
        {
            throw; // original exception
        }
        return retval;
    }
}

///////////////////////////////////////////////////////////////
//
//  Function: GetBufferInt
//
//  Description: Convert a String from the buffer to an int
//
//  Inputs: pBuffer - The file buffer that contains the string
//          offset  - First character of the string
//          length  - The size of the string
//
//  Returns: The value of the converted string
//
///////////////////////////////////////////////////////////////
int GetBufferInt(const char* pBuffer, size_t offset, int length)
{
   char* value = new char[length + 1];
   size_t intValue = 0;

   memcpy(value, &pBuffer[offset], length);
   value[length] = '\0';
   intValue = atoi(value);

   delete [] value;

   return intValue;
}

///////////////////////////////////////////////////////////////
//
//  Function: GetBufferS
//
//  Description: Convert a String from the buffer to an size_t
//
//  Inputs: pBuffer - The file buffer that contains the string
//          offset  - First character of the string
//          length  - The size of the string
//
//  Returns: The value of the converted string
//
///////////////////////////////////////////////////////////////
size_t GetBufferS(const char* pBuffer, size_t offset, int length)
{
   char* value;
   size_t sValue = 0;

   value = (char*) (&pBuffer[offset]);
   char c[2];
   c[1] = '\0';
   for(int i=0;i<length;i++)
   {
     c[0] = value[i];
	 int val = atoi(c);
     sValue = sValue*10 + val;
   }

   return sValue;
}

///////////////////////////////////////////////////////////////
//
//  Function: DisplayValue
//
//  Description: Output a char string to the screen
//               If debug, log output to .csv log file
//
//  Inputs: str  - char* variable to be displayed
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void DisplayValue(const char* str)
{
   if(nitfDebugFlag)
   {
	   recordLog(logFile, std::string(str));
   }
}

///////////////////////////////////////////////////////////////
//
//  Function: DisplayValue
//
//  Description: Output an integer value to the screen
//
//  Inputs: pName  - Name of the variable to be displayed
//          value  - The integer value to be displayed
//          offset - Position of the value in the buffer
//          length - The length of the value
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void DisplayValue(const char* pName, int value, size_t offset, int length)
{
   char OutputString[200];
   char choffset[30];
   sprintf(choffset, "%llu", (unsigned long long)offset);
   sprintf(OutputString,
       "%s\t= %d from buffer at location %s with a length %d"
	   , pName, value, choffset, length);
   DisplayValue(OutputString);
}

///////////////////////////////////////////////////////////////
//
//  Function: DisplayValue
//
//  Description: Output a size_t value to the screen
//
//  Inputs: pName  - Name of the variable to be displayed
//          value  - The size_t value to be displayed
//          offset - Position of the value in the buffer
//          length - The length of the value
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void DisplayValue(const char* pName, size_t value, size_t offset, int length)
{
   char OutputString[200];
   char chvalue[30];
   sprintf(chvalue, "%llu", (unsigned long long)value);
   char choffset[30];
   sprintf(choffset, "%llu", (unsigned long long)offset);
   sprintf(OutputString,
       "%s\t= %s from buffer at location %s with a length %d"
	   , pName, chvalue, choffset, length);
   DisplayValue(OutputString);
}

///////////////////////////////////////////////////////////////
//
//  Function: DisplayValue
//
//  Description: Output an string value to the screen
//
//  Inputs: pName  - Name of the variable to be displayed
//          value  - The string value to be displayed
//          offset - Position of the value in the buffer
//          length - The length of the value
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void DisplayValue(const char* pName, std::string value, size_t offset, int length)
{
   char choffset[30];
   sprintf(choffset, "%llu", (unsigned long long)offset);
   char OutputString[200];
   std::string strValue = value.substr(0, length);
   sprintf(OutputString,
       "%s\t= %s from buffer at location %s with a length %d"
	   , pName, strValue.c_str(), choffset, length);
   DisplayValue(OutputString);
}

///////////////////////////////////////////////////////////////
//
//  Function: DisplayValue
//
//  Description: Output an starting position value to the screen
//
//  Inputs: pName  - Name of the variable to be displayed
//          start  - The position value to be displayed
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void DisplayValue(const char* pName, size_t start)
{
   char chstart[200];
   sprintf(chstart, "%llu", (unsigned long long)start);
   char OutputString[200];
   sprintf(OutputString, "%s start: %s",pName, chstart);
   DisplayValue(OutputString);
}

///////////////////////////////////////////////////////////////
//
//  Function: parseFile
//
//  Description: Parse an NITF 2.0 file header
//
//  Inputs: isd     - Structure to store results
//          ifile   - pointer to the file to be read
//          statbuf - system information from the data file
//          buff    - header of the data file
//          imageIndex - Index of an image to be loaded
//
//  Returns: Warning - warning if any occur
//
///////////////////////////////////////////////////////////////

void parseFile(Nitf20Isd *isd,
          FILE *ifile,
#ifdef _WIN32
          struct stat &statbuf,
#else
          struct stat64 &statbuf,
#endif
          char* buff,
          const int imageIndex,
		  WarningList* warnings)
{
   Error err;                      //----- Error class
   char OutputString[200];            //----- Output String
   size_t fl;                         //----- file length
   int hl;                            //----- header length
   size_t crypt_offset;               //----- Offset to encryption field
   int class_offset = 119;            //----- Offset to classification field
   int fsdwng_offset = 280;           //----- Offset to downgrade field

   if (!buff) // fillBuff failed -- should never get here
   {
      err.setError (Error::INVALID_USE,
  		       "No ISD data buffer: fillBuff failed",
		       "parseFile");
      throw (err);
   }

   if(strncmp(buff, "NITF02.00", 9))
   {
	   if(warnings)
	   {
		   Warning wn(Warning::UNKNOWN_WARNING,
			   "Not a valid NITF 2.0 file", "parseFile(nitf2.0)");
      //warning = new Warning( Warning::UNKNOWN_WARNING,
	  //	"Not a valid NITF 2.0 file", "parseFile(nitf2.0)");
		   warnings->push_back(wn);
	   }
   }
      
   strncpy(OutputString, buff, 9);
   OutputString[9] = '\0';
   if(nitfDebugFlag)
     std::cout << "File Version = " << OutputString << std::endl;

   // check for conditional fields
   // if file is classified at any level the class info is 40 bytes longer
   if(nitfDebugFlag)
      DisplayValue("FSCLAS", &buff[class_offset], class_offset, 1);

   //----- Check the FSDWNG Field
   crypt_offset = 296;
   if(nitfDebugFlag)
     DisplayValue("FSDWNG", &buff[fsdwng_offset], fsdwng_offset, 6);
   if(!strncmp(&buff[fsdwng_offset], "999998", 6))
      crypt_offset += 40;

   // location of file length field in 2.0 file
   size_t fl_offset = crypt_offset + 46;
   fl = GetBufferS(buff, fl_offset, 12);
   if(nitfDebugFlag)
      DisplayValue("FL", fl, fl_offset, 12);

   size_t filesize;

                           // this will happen in WIN32 if the file size is between 2G and 4G
   if(statbuf.st_size < 0) // because st_size is off_t (signed int) resulting with st_size = file size - 2G
   {
      int st_sizeBits = sizeof(statbuf.st_size) * 8;
      unsigned long long inMax = pow(2.0,st_sizeBits) + .5; // if st_size is unsigned int, inMax = 2G
      filesize = inMax + statbuf.st_size;
      if(nitfDebugFlag)
	  {
		  std::cout << "sizeof(st_size): " << sizeof(statbuf.st_size)
             << ", st_sizeBits: " <<  st_sizeBits
             << ", inMax: " <<  inMax << std::endl
             << "filesize: " <<  filesize
             << ", st_size: " << statbuf.st_size
             << std::endl;
	  }
   }
      else
   {
      filesize = statbuf.st_size;
   }

   if( filesize != fl)
   {
      char chfl[30];
      char chfilesize[30];
      sprintf(chfl, "%llu", (unsigned long long)fl);
      sprintf(chfilesize, "%llu", (unsigned long long)filesize);

      sprintf(OutputString,
        "The value of File length field %s, does not match actual file length %s"
		, chfl, chfilesize);
	  if(warnings)
	  {
		   Warning wn(Warning::UNKNOWN_WARNING,
               OutputString, "parseFile(nitf2.0)");
		   warnings->push_back(wn);
	  }
   }

   // get 2.0 file header length
   size_t hl_offset = fl_offset + 12;
   hl = GetBufferInt(buff, hl_offset, 6);
   if(nitfDebugFlag)
      DisplayValue("HL", hl, hl_offset, 6);

   if (filesize < (size_t)hl)
   {
	  if(warnings)
	  {
		   Warning wn(Warning::DATA_NOT_AVAILABLE,
			   "ISD file smaller than indicated header length",
			   "parseFile(nitf2.0)");
		   warnings->push_back(wn);
	  }
   }

   // Copy the file header
   //isd->fileHeader.assign (buff, hl);
   isd->setFileHeader(string(buff,hl));

////////////////////////////////////////////////////////////////////////////
//////////////////  This section goes through the file 2.0 header      /////
//////////////////  accumulating size information for the various      /////
//////////////////  segments of the file.  Althought at the present    /////
//////////////////  time only image info, DES info and User Defined    /////
//////////////////  Header data (tre) are required for the class, all  /////
//////////////////  of the section length info is retained for         /////
//////////////////  possible future use.                               /////
////////////////////////////////////////////////////////////////////////////
   int seg_index;
   size_t des_offset = hl; // This value will be used to point to the
                           // location in the 2.0 file of the DES data
   ////////////////////////////////////////////
   // process image segments length info  /////
   ////////////////////////////////////////////

   size_t numi_offset = hl_offset + 6;
   int NUMI = GetBufferInt(buff, numi_offset, 3);
   if(nitfDebugFlag)
      DisplayValue("NUMI", NUMI, numi_offset, 3);
   //isd->numImages = NUMI;                               //todo: numImages no longer in isd
   size_t seg_offset = numi_offset + 3;

   if (NUMI > 0)
   {
      std::vector <size_t> vImageHdrLengths (NUMI);
      std::vector <size_t> vImageLengths (NUMI);
      //for (seg_index = 0; seg_index < isd->numImages; seg_index++)
      for (seg_index = 0; seg_index < NUMI; seg_index++)
      {
         sprintf(OutputString, "LISH%03d", seg_index+1);
	 vImageHdrLengths.at(seg_index) = GetBufferS(buff, seg_offset, 6);
         if(nitfDebugFlag)
            DisplayValue(OutputString, vImageHdrLengths.at(seg_index),
                         seg_offset, 6);
	 des_offset += vImageHdrLengths.at(seg_index);
	 seg_offset += 6;

         sprintf(OutputString, "LI%03d", seg_index+1);
	 vImageLengths[seg_index] = GetBufferS(buff, seg_offset, 10);
         if(nitfDebugFlag)
            DisplayValue(OutputString, vImageLengths[seg_index],
                         seg_offset, 10);
	 des_offset += vImageLengths[seg_index];
	 seg_offset += 10;
      }

      //////////////////////////////////////////////////
      //// process 2.0 Symbols segments length info ////
      //////////////////////////////////////////////////

      int NUMS = GetBufferInt(buff, seg_offset, 3);
      if(nitfDebugFlag)
         DisplayValue("NUMS", NUMS, seg_offset, 3);
      seg_offset += 3;
      if (NUMS > 0)
      {
         std::vector <int> numshdrlengths (NUMS);
         std::vector <int> numslengths (NUMS);
         for (seg_index = 0; seg_index < NUMS; seg_index++)
         {
            sprintf(OutputString, "LSSH%03d", seg_index+1);
	    numshdrlengths.at(seg_index) = GetBufferInt(buff, seg_offset, 4);
            if(nitfDebugFlag)
               DisplayValue(OutputString, numshdrlengths.at(seg_index),
                            seg_offset, 12);
	    des_offset += numshdrlengths.at (seg_index);
	    seg_offset += 4;

            sprintf(OutputString, "LS%03d", seg_index+1);
	    numslengths.at(seg_index) = GetBufferInt(buff, seg_offset, 6);
            if(nitfDebugFlag)
               DisplayValue(OutputString, numslengths.at(seg_index),
                            seg_offset, 6);
	    des_offset += numslengths.at (seg_index);
	    seg_offset += 6;
         }
      }
      ///////////////////////////////////////////////////
      //// process 2.0  Label segments length info   ////
      ///////////////////////////////////////////////////
      int NUML = GetBufferInt(buff, seg_offset, 3);
      if(nitfDebugFlag)
         DisplayValue("NUML", NUML, seg_offset, 3);
      seg_offset += 3;
      if (NUML > 0)
      {
         std::vector <int> numLhdrlengths (NUML);
         std::vector <int> numLlengths (NUML);
         for (seg_index = 0; seg_index < NUML; seg_index++)
         {
            sprintf(OutputString, "LLSH%03d", seg_index+1);
	    numLhdrlengths.at(seg_index) = GetBufferInt(buff, seg_offset, 4);
            if(nitfDebugFlag)
               DisplayValue(OutputString, numLhdrlengths.at(seg_index),
                            seg_offset, 4);
	    des_offset += numLhdrlengths.at (seg_index);
	    seg_offset += 4;

            sprintf(OutputString, "LL%03d", seg_index+1);
	    numLlengths.at(seg_index) = GetBufferInt(buff, seg_offset, 3);
            if(nitfDebugFlag)
               DisplayValue(OutputString, numLlengths.at(seg_index),
                            seg_offset, 3);
	    des_offset += numLlengths.at(seg_index);
	    seg_offset += 3;
         }
      }

      ////////////////////////////////////////////////////////////
      //// process 2.0 Text Segments length info              ////
      ////////////////////////////////////////////////////////////
      int NUMT = GetBufferInt(buff, seg_offset, 3);
      if(nitfDebugFlag)
         DisplayValue("NUMT", NUMT, seg_offset, 3);
      seg_offset += 3;
      if (NUMT > 0 )
      {
         std::vector <int> numThdrlengths (NUMT);
         std::vector <int> numTlengths (NUMT);
         for (int t_index = 0; t_index < NUMT; t_index++)
         {
            sprintf(OutputString, "LTSH%03d", t_index+1);
	    numThdrlengths[t_index] = GetBufferInt(buff, seg_offset, 4);
            if(nitfDebugFlag)
               DisplayValue(OutputString, numThdrlengths[t_index],
                            seg_offset, 4);
	    des_offset += numThdrlengths [t_index];
	    seg_offset += 4;

            sprintf(OutputString, "LT%03d", t_index+1);
	    numTlengths[t_index] = GetBufferInt(buff, seg_offset, 5);
            if(nitfDebugFlag)
               DisplayValue(OutputString, numTlengths[t_index], seg_offset, 5);
	    des_offset += numTlengths [t_index];
	    seg_offset += 5;
         }
      }
      ////////////////////////////////////////////////////////////
      //// process 2.0 Data Extension Segments length info    ////
      ////////////////////////////////////////////////////////////

      int NUMD = GetBufferInt(buff, seg_offset, 3);
      if(nitfDebugFlag)
         DisplayValue("NUMDES", NUMD, seg_offset, 3);
      seg_offset += 3;
      std::vector <size_t> DesHdrlengths (NUMD);
      std::vector <size_t> DesLengths (NUMD);

      if (NUMD > 0)
         for (seg_index = 0; seg_index < NUMD; seg_index++)
         {
            sprintf(OutputString, "LDSH%03d", seg_index+1);
	    DesHdrlengths[seg_index] = GetBufferInt(buff, seg_offset, 4);
            if(nitfDebugFlag)
               DisplayValue(OutputString, DesHdrlengths[seg_index],
                            seg_offset, 4);
	    seg_offset += 4;

            sprintf(OutputString, "LD%03d", seg_index+1);
	    DesLengths[seg_index] = GetBufferInt(buff, seg_offset, 9);
            if(nitfDebugFlag)
               DisplayValue(OutputString, DesLengths[seg_index], seg_offset, 9);
	    seg_offset += 9;
         }

      /////////////////////////////////////////////////////////////
      //// process 2.0 Reserved Extension Segments length info ////
      /////////////////////////////////////////////////////////////
      int numR = GetBufferInt(buff, seg_offset, 3);
      if(nitfDebugFlag)
         DisplayValue("NUMRES", numR, seg_offset, 3);
      seg_offset += 3;
      std::vector <int> numRhdrlengths (numR);
      std::vector <int> numRlengths (numR);

      for (seg_index = 0; seg_index < numR; seg_index++)
      {
         sprintf(OutputString, "LRSH%03d", seg_index+1);
         numRhdrlengths[seg_index] = GetBufferInt(buff, seg_offset, 4);
         if(nitfDebugFlag)
            DisplayValue(OutputString, numRhdrlengths[seg_index],
                         seg_offset, 4);
         seg_offset += 4;

         sprintf(OutputString, "LR%03d", seg_index+1);
         numRlengths[seg_index] = GetBufferInt(buff, seg_offset, 7);
         if(nitfDebugFlag)
            DisplayValue(OutputString, numRlengths[seg_index], seg_offset, 7);
         seg_offset += 7;
      }

      ////////////////////////////////////////////////////////////
      //// process  2.0 UDHD/XHD info                         ////
      ////////////////////////////////////////////////////////////
      //std::cout << "seg_offset " << seg_offset << std::endl;
      std::string UDHD;
      int lenUDHD = GetBufferInt(buff, seg_offset, 5);
      if(nitfDebugFlag)
         DisplayValue("UDHDL", lenUDHD, seg_offset, 5);
      seg_offset += 5;
      if (lenUDHD > 0)
      {
         // store file header UD tre's in string
         UDHD.assign (&buff[seg_offset + 3], lenUDHD - 3);

         seg_offset += lenUDHD;
      }

      std::string XHD;
      int lenXHD = GetBufferInt(buff, seg_offset, 5);
      if(nitfDebugFlag)
         DisplayValue("XHDL", lenXHD, seg_offset, 5);
      seg_offset += 5;

      if (lenXHD > 0)
      {
         // store file header UD tre's in string
         XHD.assign (&buff[seg_offset + 3], lenXHD - 3);
         if(nitfDebugFlag)
            std::cout << "XHD = " << XHD << std::endl;

         seg_offset += lenXHD;  // cue up to begining of first image group
      }

      std::string HD = UDHD + XHD;
      size_t treLen = HD.length();
      if (treLen)
      {
         isd->setFileTres(parseTre(treLen,  HD));
      }

      ////////////////////////////////////////////
      /////// End of fileheader code  ////////////
      ////////////////////////////////////////////
      if (seg_offset != (size_t)hl)
      {
         std::cout << "Parser error in file header -- hl = " << hl
		   << " offset = " << seg_offset << std::endl;
         exit (666);
      }

      /////// Begin 2.0 image sub-header parsing
      if (NUMI > 0)
         parseImages(isd, ifile, hl, vImageHdrLengths, vImageLengths, imageIndex, NUMI);
      if (NUMD > 0)
         parseDes(isd, ifile, des_offset, DesHdrlengths, DesLengths, NUMD);
   }
}

///////////////////////////////////////////////////////////////
//
//  Function: parseFile
//
//  Description: Parse an NITF 2.1 file header
//
//  Inputs: isd     - Structure to store results
//          ifile   - pointer to the file to be read
//          statbuf - system information from the data file
//          buff    - header of the data file
//          imageIndex - Index of an image to be loaded
//
//  Returns: Warning - warning if any occur
//
///////////////////////////////////////////////////////////////
void parseFile(Nitf21Isd *isd,
               FILE *ifile,
#ifdef _WIN32
               struct stat &statbuf,
#else
               struct stat64 &statbuf,
#endif
               char* buff,
               const int imageIndex,
			   WarningList* warnings)
{
   Error err;                      //----- Error class
   Warning *warn = NULL;           //----- Warning class
   char OutputString[200];            //----- Output String
   size_t fl;		              //----- file length
   int hl;	                      //----- header length

   // locations and field lengths of the FL, HL, and NUMI fields are
   // all at fixed offsets in the 2.1 header.  These values are taken
   // from MIL-STD-2500B Notice 2
   const int FLOFFSET = 342;
   const int FLLEN = 12;
   const int HLOFFSET = 354;
   const int HLLEN = 6;
   const int NUMIOFFSET = 360;
   const int NUMILEN = 3;

   if (!buff) // fillBuff() failed or somebody called me before calling fillBuff() -- should never get here
   {
      err.setError (Error::INVALID_USE,
		       "No ISD data buffer: fillBuff failed",
		       "parseFile");
      throw (err);
   }

   if(strncmp(buff, "NITF02.10", 9))
   {
	  if(warnings)
	  {
		   Warning wn(Warning::UNKNOWN_WARNING,
			   "Not a valid NITF 2.1 file", 
			   "parseFile(nitf2.1)");
		   warnings->push_back(wn);
	  }
   }
   strncpy(OutputString, buff, 9);
   OutputString[9] = '\0';
   if(nitfDebugFlag)
     std::cout << "File Version = " << OutputString << std::endl;

   fl = GetBufferS(buff, FLOFFSET, FLLEN);
   if(nitfDebugFlag)
      DisplayValue("FL", fl, FLOFFSET, FLLEN);

   size_t filesize;

                           // this will happen in WIN32 if the file size is between 2G and 4G
   if(statbuf.st_size < 0) // because st_size is off_t (signed int) resulting with st_size = file size - 2G
   {
      int st_sizeBits = sizeof(statbuf.st_size) * 8;
      unsigned long long inMax = pow(2.0,st_sizeBits) + .5; // if st_size is unsigned int, inMax = 2G
      filesize = inMax + statbuf.st_size;
      if(nitfDebugFlag)
	  {
		  std::cout << "sizeof(st_size): " << sizeof(statbuf.st_size)
             << ", st_sizeBits: " <<  st_sizeBits
             << ", inMax: " <<  inMax << std::endl
             << "filesize: " <<  filesize
             << ", st_size: " << statbuf.st_size
             << std::endl;
	  }
   }
      else
   {
      filesize = statbuf.st_size;
   }
   if( filesize != fl)
   {
      char chfl[30];
      char chfilesize[30];
      sprintf(chfl, "%llu", (unsigned long long)fl);
      sprintf(chfilesize, "%llu", (unsigned long long)filesize);

      sprintf(OutputString,
        "The value of File length field %s, does not match actual file length %s"
	        , chfl, chfilesize);
	  if(warnings)
	  {
		   Warning wn(Warning::UNKNOWN_WARNING,
			   OutputString, 
			   "parseFile(nitf2.1)");
		   warnings->push_back(wn);
	  }
   }

   hl = GetBufferInt(buff, HLOFFSET, HLLEN);
   if(nitfDebugFlag)
      DisplayValue("HL", hl, HLOFFSET, HLLEN);
   if(filesize < (size_t)hl)
   {
	  if(warnings)
	  {
		   Warning wn(Warning::DATA_NOT_AVAILABLE,
			   "ISD file smaller than indicated header length", 
			   "parseFile(nitf2.1)");
		   warnings->push_back(wn);
	  }
   }

   // Copy the file header
   isd->setFileHeader(string(buff, hl));
   //isd->fileHeader.assign (buff, hl);

   size_t des_offset = hl;  // This value will be used to locate
				  // the DES data in the 2.1 file

////////////////////////////////////////////////////////////////////////////
//////////////////  This section goes through the 2.1 file header      /////
//////////////////  accumulating size information for the various      /////
//////////////////  segments of the file.  Althought at the present    /////
//////////////////  time only image info, DES info and User Defined    /////
//////////////////  Header data (tre) are required for the class, all  /////
//////////////////  of the section length info is retained for         /////
//////////////////  possible future use.                               /////
////////////////////////////////////////////////////////////////////////////
   int seg_index;
   size_t seg_offset = NUMIOFFSET;
   ////////////////////////////////////////////////
   // process 2.1 image segments length info  /////
   ////////////////////////////////////////////////

   int NUMI = GetBufferInt(buff, seg_offset, NUMILEN);
   if(nitfDebugFlag)
      DisplayValue("NUMI", NUMI, seg_offset, NUMILEN);
   // std::cout << "**** NUMI = " << NUMI << std::endl;
   //isd->numImages = NUMI;
   std::vector <size_t> vImageHdrLengths (NUMI);
   std::vector <size_t> vImageLengths (NUMI);

   seg_offset = seg_offset + 3;
   for (seg_index = 0; seg_index < NUMI; seg_index++)
   {
      sprintf(OutputString, "LISH%03d", seg_index + 1);
      vImageHdrLengths.at(seg_index) = GetBufferS(buff, seg_offset, 6);
      if(nitfDebugFlag)
         DisplayValue(OutputString, vImageHdrLengths.at(seg_index),
                      seg_offset, 6);
      des_offset += vImageHdrLengths.at(seg_index);
      seg_offset += 6;

      sprintf(OutputString, "LI%03d", seg_index + 1);
      vImageLengths[seg_index] = GetBufferS(buff, seg_offset, 10);
      if(nitfDebugFlag)
         DisplayValue(OutputString, vImageLengths[seg_index], seg_offset, 10);
      des_offset += vImageLengths[seg_index];
      seg_offset += 10;
   }

   // Because the number of entries in each of the 2.1 header/data
   // segment length fields varies as the number of segments there are
   // no fixed offsets.
   //////////////////////////////////////////////////
   //// process 2.1 Graphic segments length info ////
   //////////////////////////////////////////////////
   int NUMS = GetBufferInt(buff, seg_offset, 3);
   if(nitfDebugFlag)
      DisplayValue("NUMS", NUMS, seg_offset, 3);
   seg_offset += 3;
   if (NUMS > 0)
   {
      std::vector <int> numShdrlengths (NUMS);
      std::vector <int> numSlengths (NUMS);
      for (seg_index = 0; seg_index < NUMS; seg_index++)
      {
         sprintf(OutputString, "LSSH%03d", seg_index+1);
	 numShdrlengths.at(seg_index) = GetBufferInt(buff, seg_offset, 4);
         if(nitfDebugFlag)
            DisplayValue(OutputString, numShdrlengths.at(seg_index),
                         seg_offset, 4);
	 des_offset += numShdrlengths.at (seg_index);
	 seg_offset += 4;

         sprintf(OutputString, "LS%03d", seg_index+1);
	 numSlengths.at(seg_index) = GetBufferInt(buff, seg_offset, 3);
         if(nitfDebugFlag)
            DisplayValue(OutputString, numSlengths.at(seg_index),
                         seg_offset, 3);
	 seg_offset += 6;
      }
   }
   // There is a 3 space field called NUMX whixh is not currently used
   // by the 2500B standard
   seg_offset += 3;

   ////////////////////////////////////////////////////////////
   //// process 2.1 Text Segments length info               ////
   /////////////////////////////////////////////////////////////
   int NUMT = GetBufferInt(buff, seg_offset, 3);
   if(nitfDebugFlag)
      DisplayValue("NUMT", NUMT, seg_offset, 3);

   seg_offset += 3;
   if (NUMT > 0)
   {
      std::vector <int> numThdrlengths (NUMT);
      std::vector <int> numTlengths (NUMT);
      for (int t_index = 0; t_index < NUMT; t_index++)
      {
         sprintf(OutputString, "LTSH%03d", t_index+1);
	 numThdrlengths[t_index] = GetBufferInt(buff, seg_offset, 4);
         if(nitfDebugFlag)
            DisplayValue(OutputString, numThdrlengths[t_index],
                         seg_offset, 12);
	 des_offset += numThdrlengths [t_index];
	 seg_offset += 4;

         sprintf(OutputString, "LT%03d", t_index+1);
	 numTlengths[t_index] = GetBufferInt(buff, seg_offset, 5);
         if(nitfDebugFlag)
            DisplayValue(OutputString, numTlengths[t_index], seg_offset, 5);
	 des_offset += numTlengths [t_index];
	 seg_offset += 5;
      }
   }

   ////////////////////////////////////////////////////////////
   //// process 2.1 Data Extension Segments length info    ////
   ////////////////////////////////////////////////////////////
   int NUMD = GetBufferInt(buff, seg_offset, 3);
   if(nitfDebugFlag)
      DisplayValue("NUMDES", NUMD, seg_offset, 3);
   seg_offset += 3;
   std::vector <size_t> Dhdrlengths (NUMD);
   std::vector <size_t> Dlengths (NUMD);
   if (NUMD > 0)
   {
      for (seg_index = 0; seg_index < NUMD; seg_index++)
      {
         sprintf(OutputString, "LDSH%03d", seg_index+1);
	 Dhdrlengths[seg_index] = GetBufferInt(buff, seg_offset, 4);
         if(nitfDebugFlag)
            DisplayValue(OutputString, Dhdrlengths[seg_index], seg_offset, 4);
	 seg_offset += 4;

         sprintf(OutputString, "LD%03d", seg_index+1);
	 Dlengths[seg_index] = GetBufferInt(buff, seg_offset, 9);
         if(nitfDebugFlag)
            DisplayValue(OutputString, Dlengths[seg_index], seg_offset, 9);
	 seg_offset += 9;
      }
   }

   ////////////////////////////////////////////////////////////////
   //// process 2.1 Reserved Extension Segments length info    ////
   ////////////////////////////////////////////////////////////////
   int numR = GetBufferInt(buff, seg_offset, 3);
   if(nitfDebugFlag)
      DisplayValue("NUMRES", numR, seg_offset, 3);
   seg_offset += 3;
   std::vector <size_t> numRhdrlengths (numR);
   std::vector <size_t> numRlengths (numR);
   for (seg_index = 0; seg_index < numR; seg_index++)
   {
      sprintf(OutputString, "LRSH%03d", seg_index+1);
      numRhdrlengths[seg_index] = GetBufferInt(buff, seg_offset, 4);
      if(nitfDebugFlag)
         DisplayValue(OutputString, numRhdrlengths[seg_index],
                      seg_offset, 4);
      seg_offset += 4;

      sprintf(OutputString, "LR%03d", seg_index+1);
      numRlengths[seg_index] = GetBufferInt(buff, seg_offset, 7);
      if(nitfDebugFlag)
         DisplayValue(OutputString, numRlengths[seg_index], seg_offset, 7);
      seg_offset += 7;
   }

   ////////////////////////////////////////////////////////////
   //// process  2.1 UDHD/XHD (file tre) info              ////
   ////////////////////////////////////////////////////////////
   std::string UDHD;
   int lenUDHD = GetBufferInt(buff, seg_offset, 5);
   if(nitfDebugFlag)
      DisplayValue("UDHDL", lenUDHD, seg_offset, 5);
   seg_offset += 5;
   if (lenUDHD > 0)
   {
      // store file header UD tre's in string
      UDHD.assign (&buff[seg_offset + 3], lenUDHD - 3);

      seg_offset += lenUDHD;
   }

   int XHDL = GetBufferInt(buff, seg_offset, 5);
   if(nitfDebugFlag)
      DisplayValue("XHDL", XHDL, seg_offset, 5);
   seg_offset += 5;

   // store file header X UD tre's in string
   std::string XHD;
   std::string HD;
   if (XHDL > 0)  //----- Add Three Offset to skip unused field -RTB
   {
      XHD.assign (&buff[seg_offset + 3], XHDL - 3);

      seg_offset += XHDL;
   }
   if ((XHDL + lenUDHD) > 0)
      HD.assign (UDHD + XHD);

   size_t udidl = HD.length();
   // if any file tre data parse it
   if (udidl > 0)

   isd->setFileTres(parseTre(udidl, HD));
   if (NUMI > 0)
      parseImages(isd, ifile, hl, vImageHdrLengths, vImageLengths, imageIndex, NUMI);
   if (NUMD > 0)
      parseDes(isd, ifile, des_offset, Dhdrlengths, Dlengths, NUMD);
};

///////////////////////////////////////////////////////////////
//
//  Function: parseImages
//
//  Description: Parse an NITF 2.0 image sub header
//
//  Inputs: isd              - Structure to store results
//          ifile            - pointer to the file to be read
//          HeaderLength     - Start of the image segment
//          vImageHdrLengths - Vector of Image header lengths
//          vImageLengths    - Vector of Image lengths
//          imageIndex       - Index of an image to be loaded
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void parseImages(Nitf20Isd * isd,
                 FILE *ifile,
                 const int HeaderLength,
                 const std::vector<size_t> vImageHdrLengths,
                 const std::vector<size_t> vImageLengths,
                 const int imageIndex,
				 const int NUMI)
{
   int numComments;                     //----- Number cf comments
   int CommentLength = 80;              //----- Length of Comments
   int numBands;                        //----- Number of Bands
   int numLUTS;                         //----- Number of LUT
   int numLUTEnt;                       //----- Size of LUT entry
   size_t byteCount;                    //----- Current buffer position
   std::string pbuff;                   //----- Pointer Buffer
   char* pBuffer = NULL;                //----- Image data buffer
   int index, band;                     //----- Loop Variables
   int udidl;                           //----- User Defind data length
   int udofl;                           //----- User Overflow length
   int ixshdl;                          //----- Extended Data Length
   int ixsofl;                          //----- Extended Overflow length
   size_t imageStartByte = HeaderLength;   //----- Image Starting Byte
   std::string UHD;                     //----- TRE Data Segment
   std::string XHD;                     //----- TRE Data Segment
   std::string TreBuffer;               //----- TRE Data Segment

   //if(imageIndex == -1)
   //   isd->images = new image[NUMI];
   //else
   //   isd->images = new image[1];
   for(index = 0; index < NUMI; index++)
   {
	  Image img;
      if(nitfDebugFlag)
      {
         DisplayValue("Image Segment", imageStartByte);
      }

      pBuffer = getSegment(ifile,
                        imageStartByte,
                        vImageHdrLengths[index] // + vImageLengths[index]
	                                           );

      // copy image header including image tre's
	  img.setSubHeader(string(pBuffer,vImageHdrLengths[index]));
    //if(imageIndex == -1)
         //isd->images[index].imageSubHeader.assign(
         //  pBuffer,vImageHdrLengths[index]);
    //else if(imageIndex == index)
    //   isd->images[0].imageSubHeader.assign(                          =============================
    //      pBuffer,vImageHdrLengths[index]);

      //----- Get Classification
      byteCount = 123;
      if(nitfDebugFlag)
         DisplayValue("ISCLAS", &pBuffer[byteCount],
                      byteCount + imageStartByte, 1);

      //----- Check Image Downgrad field
      byteCount = 284;
      pbuff.assign(&pBuffer[byteCount], 6);
      if(nitfDebugFlag)
         DisplayValue("ISDWNG", pbuff, byteCount + imageStartByte, 6);
      if(pbuff == "999998")
         byteCount += 40;

      //---- Get Image Coordinates
      byteCount += 87;
      if(nitfDebugFlag)
         DisplayValue("ICORDS", &pBuffer[byteCount],
                      byteCount + imageStartByte, 1);
      if (pBuffer[byteCount] != 'N')
	 byteCount += 60; // There are image coordinates

      //----- Get Comments
      byteCount++;
      numComments = pBuffer[byteCount] - '0';
      if(nitfDebugFlag)
         DisplayValue("NICOM", numComments, byteCount + imageStartByte, 1);
      byteCount += (numComments * CommentLength + 1);

      //---- Get Image Compression
      if(nitfDebugFlag)
         DisplayValue("IC", &pBuffer[byteCount],
                      byteCount + imageStartByte, 2);
      if (pBuffer[byteCount] != 'N')
	 byteCount += 4; // image compression field required

      //----- Get the Number of commands
      byteCount += 2;
      numBands =  pBuffer[byteCount] - '0';
      if(nitfDebugFlag)
         DisplayValue("NBANDS", numBands, byteCount + imageStartByte, 1);
      byteCount++;

      //---- Get the Band information
      for (band=0; band < numBands; band++)
      {
	 byteCount += 12;

	 //----- Get the LUT data for each Band
	 numLUTS = pBuffer[byteCount] - '0';
         if(nitfDebugFlag)
         {
            char OutputString[200];
            sprintf(OutputString, "NLUTS%d", band+1);
            DisplayValue(OutputString, numLUTS, byteCount + imageStartByte, 1);
         }
	 byteCount++;

	 if (numLUTS)
	 {
            numLUTEnt = GetBufferInt(pBuffer, byteCount, 5);
	    byteCount += numLUTEnt * numLUTS + 5;
	 }
      }

      byteCount += 40;

      //----- Get User definded data section
      udidl = GetBufferInt(pBuffer, byteCount, 5);
      if(nitfDebugFlag)
         DisplayValue("UDIDL", udidl, byteCount + imageStartByte, 5);
      byteCount += 5;
      UHD.clear();
      XHD.clear();


      if(udidl > 0)
      {
         udofl = GetBufferInt(pBuffer, byteCount, 3);
         if(nitfDebugFlag)
            DisplayValue("UDOFL", udofl, byteCount + imageStartByte, 3);

         if(nitfDebugFlag)
         {
            DisplayValue("Image TRE data", imageStartByte);
         }
         UHD.assign(&pBuffer[byteCount + 3], udidl - 3);
         byteCount += udidl;
      }

      //----- Get TRE's from the Extended data segment
      ixshdl = GetBufferInt(pBuffer, byteCount, 5);
      if(nitfDebugFlag)
         DisplayValue("IXSHDL", ixshdl, byteCount + imageStartByte, 5);
      byteCount += 5;
      if(ixshdl > 0)
      {
         ixsofl = GetBufferInt(pBuffer, byteCount, 3);
         if(nitfDebugFlag)
            DisplayValue("IXSOFL", ixsofl, byteCount + imageStartByte, 3);

         // if image tre data available parse it
         if(nitfDebugFlag)
         {
            DisplayValue("Image TRE data", imageStartByte);
         }

         XHD.assign(&pBuffer[byteCount + 3], ixshdl - 3);
         byteCount += ixshdl;
      }

      TreBuffer = UHD + XHD;
      if(TreBuffer.length() > 0)
      {
         img.setImageTres(parseTre(TreBuffer.length(), TreBuffer));
	  }
	  TreBuffer.clear();

      // shift file pointer to next image header/data segment
      imageStartByte += vImageHdrLengths[index] + vImageLengths[index];
	  isd->addImage(img);

      delete [] pBuffer;
   }  //----- End of Image Loop

   //if(imageIndex != -1)
   //{
   //   if(imageIndex < NUMI)
   //   {
   //      NUMI = 1;
   //   }
   //   else
   //   {
   //      NUMI = 0;
   //   }
   //}
};

///////////////////////////////////////////////////////////////
//
//  Function: parseImages
//
//  Description: Parse an NITF 2.1 image sub header
//
//  Inputs: isd              - Structure to store results
//          ifile            - pointer to the file to be read
//          HeaderLength     - Start of the image segment
//          vImageHdrLengths - Vector of Image header lengths
//          vImageLengths    - Vector of Image lengths
//          imageIndex       - Index of an image to be loaded
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void parseImages(Nitf21Isd * isd,
                 FILE *ifile,
                 const int HeaderLength,
                 const std::vector <size_t> vImageHdrLengths,
                 const std::vector <size_t> vImageLengths,
                 const int imageIndex,
				 const int NUMI)
{
   const int ICORDS_OFFSET = 371;            //----- Offset to the Coords field
   const int ICORDS_LEN = 60;                //----- Length of the Coords field
   const int ICOM_LEN = 80;                  //----- Length of the Comment field

   // NITF data
   int numComments;                           //----- Number of Comments
   int numBands;                              //----- Number of Bands
   int numLUTS;                               //----- Number of LUTs
   int numLUTEnt;                             //----- Size of LUT
   int UDIDL;                                 //----- User Defined data length
   int UDOFL;                                 //----- User defined Overflow
   int IXSHDL;                                //----- Extended Data Length
   int IXSOFL;                                //----- Extended Data Overflow
   int index, bandIndex;                      //----- Loop Variable
   char* pBuffer = NULL;                      //----- Data Buffer
   size_t imageStartByte = HeaderLength;         //----- Image Starting Index
   std::string UHD;                           //----- TRE Data Segment
   std::string XHD;                           //----- TRE Data Segment
   std::string TreBuffer;                     //----- TRE Data Segment
   size_t byteCount;                             //----- Current Byte position

   //if(imageIndex == -1)
   //   isd->images = new image[NUMI];
   //else
   //   isd->images = new image[1];
   Image img;

   for (index = 0; index < NUMI; index ++)
   {
      byteCount = ICORDS_OFFSET + 1;

      if(nitfDebugFlag)
      {
          DisplayValue("Image Segment", imageStartByte);
      }
      pBuffer = getSegment(ifile,
                        imageStartByte,
                        vImageHdrLengths[index] // + vImageLengths[index]
	                                           );

      // copy image header including image tre's
      //if(imageIndex == -1)
      //   isd->images[index].imageSubHeader.assign(pBuffer,
      //       vImageHdrLengths[index]);
      //else if(imageIndex == index)
      //   isd->images[0].imageSubHeader.assign(pBuffer,
      //       vImageHdrLengths[index]);
      img.setSubHeader(string(pBuffer, vImageHdrLengths[index]));

      if(nitfDebugFlag)
         DisplayValue("ICOORDS", &pBuffer[ICORDS_OFFSET],
                      ICORDS_OFFSET + imageStartByte, 1);

      // now parse image looking for tre's
      if (pBuffer[ICORDS_OFFSET] != ' ') // 60 byte ICOORDS field present
	 byteCount += ICORDS_LEN;

      // get number of image comments;
      numComments = (int) pBuffer[byteCount] - '0';
      if(nitfDebugFlag)
         DisplayValue("NICOM", numComments, byteCount + imageStartByte, 5);
      byteCount++;
      byteCount += numComments *  ICOM_LEN;

      // evalute image compression type (IC) field to determine
      // presence of ICOM field
      if(nitfDebugFlag)
         DisplayValue("IC", &pBuffer[byteCount],
                      byteCount + imageStartByte, 2);
      if ((strncmp(&pBuffer[byteCount], "NM", 2))
	  && (strncmp(&pBuffer[byteCount], "NC", 2)))
	 byteCount += 4; // ICOM field is present if IC != NM or NC

      // check if XBANDS field is present
      byteCount += 2;
      numBands = GetBufferInt(pBuffer, byteCount, 1);
      if(nitfDebugFlag)
         DisplayValue("NBANDS", numBands, byteCount + imageStartByte, 5);
      byteCount++;

      if (numBands == 0)  // no NBANDS check XBANDS
      {
	 numBands = GetBufferInt(pBuffer, byteCount, 5);
         if(nitfDebugFlag)
            DisplayValue("XBANDS", numBands, byteCount + imageStartByte, 5);
	 byteCount += 5;
      }

      // image band parsing each image band contains required fields
      // plus an optional varying length LUT table.  The LUT
      // information can vary in length from band to band.
      for (bandIndex = 0; bandIndex < numBands; bandIndex++)
      {
         byteCount += 12;
         numLUTS = GetBufferInt(pBuffer, byteCount, 1);
         if(nitfDebugFlag)
         {
            char OutputString[200];
            sprintf(OutputString, "NLUTS%d", bandIndex+1);
            DisplayValue(OutputString, numLUTS, byteCount + imageStartByte, 1);
         }

         byteCount += 1;
         if (numLUTS) // there are LUTS
	 {
            numLUTEnt = GetBufferInt(pBuffer, byteCount, 5);
            if(nitfDebugFlag)
               DisplayValue("numLUTEnt", numLUTEnt, byteCount, 5);
            byteCount += 5;
            byteCount += (numLUTEnt * numLUTS);
	 }
      }
      byteCount += 40;

      UDIDL = GetBufferInt(pBuffer, byteCount, 5);   // image tre's
      if(nitfDebugFlag)
         DisplayValue("UDIDL", UDIDL, byteCount + imageStartByte, 5);
      byteCount += 5;

      UHD.clear();
      XHD.clear();

      if (UDIDL > 0)
      {
         UDOFL = GetBufferInt(pBuffer, byteCount, 3);
         if(nitfDebugFlag)
            DisplayValue("UDOFL", UDOFL, byteCount + imageStartByte, 3);

         if(nitfDebugFlag)
         {
            DisplayValue("Image TRE data", imageStartByte);
         }
         UHD.assign(&pBuffer[byteCount + 3], UDIDL - 3);

         byteCount += UDIDL;
      }

      // Added Reading of Extended Data Length
      IXSHDL = GetBufferInt(pBuffer, byteCount, 5);
      if(nitfDebugFlag)
         DisplayValue("IXSHDL", IXSHDL, byteCount + imageStartByte, 5);
      byteCount += 5;

      if (IXSHDL > 0) // tre's are present
      {
         IXSOFL = GetBufferInt(pBuffer, byteCount, 3);
         if(nitfDebugFlag)
            DisplayValue("IXSOFL", IXSOFL, byteCount + imageStartByte, 3);

         if(nitfDebugFlag)
         {
            DisplayValue("Image TRE data", imageStartByte);
         }
         XHD.assign(&pBuffer[byteCount + 3], IXSHDL - 3);
         byteCount += IXSHDL;
      }

      TreBuffer = UHD + XHD;
      if(TreBuffer.length() > 0)
      {
		  img.setImageTres(parseTre(TreBuffer.length(), TreBuffer));
	  }

      TreBuffer.clear();

      // shift file pointer to next image header/data segment
      imageStartByte += vImageHdrLengths[index] + vImageLengths[index];

      isd->addImage(img);
      delete [] pBuffer;
   } // end of image loop

   //if(imageIndex != -1)
   //{
   //   if(imageIndex < NUMI)
   //   {
   //      NUMI = 1;
   //   }
   //   else
   //   {
   //      NUMI = 0;
   //   }
   //}
};

///////////////////////////////////////////////////////////////
//
//  Function: printchar
//
//  Description: print out a tre
//
//  Inputs: data - tre data segment to be outputted
//          len  - the length of the image segment
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void printchar(const char *data, int len)
{
   for(int j = 0;j<len;j++)
      std::cout << data[j];

   std::cout << std::endl;
}

///////////////////////////////////////////////////////////////
//
//  Function: parseTre
//
//  Description: parse a TRE segment
//
//  Inputs: Trelength  - Length of the TRE segment
//          DataBuffer - The TRE data
//
//  Returns: pTreData   - The TRE data segment
//
///////////////////////////////////////////////////////////////
vector<Tre> parseTre(int Trelength, std::string DataBuffer)
{
   int parseLength = 0;               //----- Start of TRE
   std::vector<int> vTreLengths;      //----- vector of TRE lengths
   string pTreSegment;                //----- Name of the TRE
   int TreSize;                       //----- Lenth of TRE data
   int TreHeaderLength = 6 + 5;       //----- tre header length =
                                      //-----       6 label chars + 5 len chars
   int numTre = 0;                    //----- Number of TRE's
   int i;                             //----- Loop Variable

   vTreLengths.reserve(300);
   while (parseLength < Trelength)
   {
      TreSize = atoi(DataBuffer.substr(parseLength + 6, 5).c_str());
      vTreLengths.push_back(TreSize + TreHeaderLength);
      parseLength += vTreLengths[numTre];
      numTre++;
   }

   vector<Tre> pTreData;

   parseLength = 0;
   for (i = 0; i < numTre; i++)
   {
	  Tre tre;
      pTreSegment = string(DataBuffer, parseLength, vTreLengths[i]);
	  tre.setTre(pTreSegment);
      if(nitfDebugFlag)
      {
         std::string Trename = DataBuffer.substr(parseLength, 6);
         TreSize = atoi(DataBuffer.substr(parseLength + 6, 5).c_str());

         char OutputString[200];
         sprintf(OutputString,
              "TRE (%s) is at %d and has a length of %d bytes"
              ,Trename.c_str(), parseLength, TreSize);
		  DisplayValue(OutputString);
      }

      parseLength += vTreLengths[i];

	  pTreData.push_back(tre);
   }
   return pTreData;
};

///////////////////////////////////////////////////////////////
//
//  Function: parseDes
//
//  Description: parse a DES segment
//
//  Inputs: isd         - The file structure to store results
//          pFile       - The file pointer to be read
//          offset      - The offset to the DES data
//          hdrlengths  - Lengths of the DES header
//          datalengths - Lengths of the DES data
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void parseDes(Nitf20Isd * isd,
              FILE *pFile,
	          size_t offset,
	          const std::vector<size_t> hdrlengths,
	          const std::vector<size_t> datalengths,
			  const int NUMD)
{
   char* pHeader = NULL;
   char* pData = NULL;

   for (int index = 0; index < NUMD; index++)
   {

      pHeader = getSegment(pFile, offset, hdrlengths[index]);
      offset  += hdrlengths[index];
      pData   = getSegment(pFile, offset, datalengths[index]);
      offset  += datalengths[index];

      isd->addFileDes(Des(string(pHeader, hdrlengths[index]), string(pData, datalengths[index])));

      delete [] pHeader;
      delete [] pData;
   }
}

///////////////////////////////////////////////////////////////
//
//  Function: parseDes
//
//  Description: parse a DES segment
//
//  Inputs: isd         - The file structure to store results
//          pFile       - The file pointer to be read
//          offset      - The offset to the DES data
//          hdrlengths  - Lengths of the DES header
//          datalengths - Lengths of the DES data
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void parseDes(Nitf21Isd *isd,
              FILE *pFile,
	          size_t offset,
	          const std::vector<size_t> hdrlengths,
	          const std::vector<size_t> datalengths,
			  const int NUMD)
{
   parseDes((Nitf20Isd *)isd, pFile, offset, hdrlengths, datalengths, NUMD);
};

///////////////////////////////////////////////////////////////
//
//  Function: getSegment
//
//  Description: get a data segment from a file
//
//  Inputs: pFile      - File pointer to be read
//          offset     - Offset to data to be read
//          bufferSize - Lengths of the data to be read
//
//  Returns: char* - read data segment
//
///////////////////////////////////////////////////////////////
char* getSegment(FILE *pFile,
                 const size_t offset,
		 const size_t bufferSize)  throw (Error)
{
   char* buff = NULL;
   size_t bytesRead;                   //----- Number of Bytes read

   buff = new char[bufferSize];
   if (!buff)
      ; // handle error

#if defined (WIN32)
   fpos_t f_offset = offset;
   fsetpos(pFile, &f_offset);
#else
#if defined (__linux)
   off64_t f_offset = offset;
   fseeko64 (pFile, f_offset, SEEK_SET);
#else
   // use fsetpos instead of fseek for file larger than 2GB
   fpos64_t f_offset = offset;
   fsetpos64(pFile, &f_offset);
#endif
#endif

   bytesRead = fread (buff, 1, bufferSize, pFile);

   if (bytesRead != bufferSize)
   {
      delete [] buff;
      std::string errstr( "failure reading input file ");

      Error csmerr(Error::FILE_READ,
                      errstr,
                      "getSegment");
      throw (csmerr);
   }

   return buff;
}

///////////////////////////////////////////////////////////////
//
//  Function: dumpSubHdr
//
//  Description: Output a Subheader data segment
//
//  Inputs: buffer   - Sub header data segment
//          numStr   - Field name of the string
//          hdrStr   - Header of the string
//          hdrCount - Length of the header field
//          imgCount - Length of the data field
//
//  Returns: Number of bytes read
//
///////////////////////////////////////////////////////////////
int dumpSubHdr(std::string buffer,
               std::string numStr,
               std::string hdrStr,
               int hdrCount,
               int imgCount)
{
   int byteCount = 0;           //----- Count of the number of bytes
   char label[100];             //----- Output Label
   int num;                     //----- Integer value of a field
   int i;                       //----- Loop Variable

   num = atoi(buffer.substr(byteCount, 3).c_str());
   std::cout << numStr << "\t= " << buffer.substr(byteCount, 3) << std::endl;
   byteCount += 3;

   for(i=0;i<num;i++)
   {
      sprintf(label, "%sSH%03d\t= ", hdrStr.c_str(), i+1);
      std::cout << label << buffer.substr(byteCount, hdrCount) << std::endl;
      byteCount += hdrCount;
      sprintf(label, "%s%03d\t= ", hdrStr.c_str(), i+1);
      std::cout << label << buffer.substr(byteCount, imgCount) << std::endl;
      byteCount += imgCount;
   }

   return byteCount;
}

///////////////////////////////////////////////////////////////
//
//  Function: dumpNitf20Hdr
//
//  Description: Output the file header
//
//  Inputs: buffer - Header data segment
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void dumpNitf20Hdr(std::string buffer)
{
   int byteCount = 0;                 //----- Count of number of Bytes

   std::cout << "FHDR\t= " << buffer.substr(byteCount, 9) << std::endl;
   byteCount += 9;
   std::cout << "CLEVEL\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "STYPE\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "OSTAID\t= " << buffer.substr(byteCount, 10) << std::endl;
   byteCount += 10;
   std::cout << "FDT\t= " << buffer.substr(byteCount, 14) << std::endl;
   byteCount += 14;
   std::cout << "FTITLE\t= " << buffer.substr(byteCount, 80) << std::endl;
   byteCount += 80;
   std::cout << "FSCLAS\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "FSCODE\t= " << buffer.substr(byteCount, 40) << std::endl;
   byteCount += 40;
   std::cout << "FSCTLH\t= " << buffer.substr(byteCount, 40) << std::endl;
   byteCount += 40;
   std::cout << "FSREL\t= " << buffer.substr(byteCount, 40) << std::endl;
   byteCount += 40;
   std::cout << "FSCAUT\t= " << buffer.substr(byteCount, 20) << std::endl;
   byteCount += 20;
   std::cout << "FSCTLN\t= " << buffer.substr(byteCount, 20) << std::endl;
   byteCount += 20;
   std::cout << "FSDWNG\t= " << buffer.substr(byteCount, 6) << std::endl;
   if(buffer.substr(byteCount, 6) == "999998")
   {
      std::cout << "FSDEVT\t= " << buffer.substr(byteCount + 6, 40) << std::endl;
      byteCount += 40;
   }

   byteCount += 6;

   std::cout << "FSCOP\t= " << buffer.substr(byteCount, 5) << std::endl;
   byteCount += 5;
   std::cout << "FSCPYS\t= " << buffer.substr(byteCount, 5) << std::endl;
   byteCount += 5;
   std::cout << "ENCRYP\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "FBKGC\t= " << buffer.substr(byteCount, 3) << std::endl;
   byteCount += 3;
   std::cout << "ONAME\t= " << buffer.substr(byteCount, 24) << std::endl;
   byteCount += 24;
   std::cout << "OPHONE\t= " << buffer.substr(byteCount, 18) << std::endl;
   byteCount += 18;
   std::cout << "FL\t= " << buffer.substr(byteCount, 12) << std::endl;
   byteCount += 12;
   std::cout << "HL\t= " << buffer.substr(byteCount, 6) << std::endl;
   byteCount += 6;

   byteCount += dumpSubHdr(buffer.substr(byteCount), "NUMI", "LI", 6, 10);
   byteCount += dumpSubHdr(buffer.substr(byteCount), "NUMS", "LS", 4, 6);
   byteCount += dumpSubHdr(buffer.substr(byteCount), "NUML", "LL", 4, 3);
   byteCount += dumpSubHdr(buffer.substr(byteCount), "NUMT", "LT", 4, 5);
   byteCount += dumpSubHdr(buffer.substr(byteCount), "NUMDES", "LD", 4, 9);
   byteCount += dumpSubHdr(buffer.substr(byteCount), "NUMRES", "LR", 4, 7);

   int UDHDL = atoi(buffer.substr(byteCount, 5).c_str());
   std::cout << "UDHDL\t= " << buffer.substr(byteCount, 5) << std::endl;
   byteCount += 5;

   if(UDHDL > 0)
   {
      std::cout << "UDHOFL\t= " << buffer.substr(byteCount, 3) << std::endl;
      std::cout << "UDHD\t= " << buffer.substr(byteCount + 3, UDHDL - 3)
                << std::endl;
      byteCount += UDHDL;
   }

   int XHDL = atoi(buffer.substr(byteCount, 5).c_str());
   std::cout << "XHDL\t= " << buffer.substr(byteCount, 5) << std::endl;
   byteCount += 5;

   if(XHDL > 0)
   {
      std::cout << "XHDOFL\t= " << buffer.substr(byteCount, 3) << std::endl;
      std::cout << "XHD\t= " << buffer.substr(byteCount + 3, XHDL - 3)
                << std::endl;
      byteCount += XHDL;
   }
}

///////////////////////////////////////////////////////////////
//
//  Function: dumpNitf21Hdr
//
//  Description: Output the file header
//
//  Inputs: buffer - Header data segment
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void dumpNitf21Hdr(std::string buffer)
{
   int byteCount = 0;                   //----- Count of the number of bytes

   std::cout << "FHDR\t= " << buffer.substr(byteCount, 9) << std::endl;
   byteCount += 9;
   std::cout << "CLEVEL\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "STYPE\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "OSTAID\t= " << buffer.substr(byteCount, 10) << std::endl;
   byteCount += 10;
   std::cout << "FDT\t= " << buffer.substr(byteCount, 14) << std::endl;
   byteCount += 14;
   std::cout << "FTITLE\t= " << buffer.substr(byteCount, 80) << std::endl;
   byteCount += 80;
   std::cout << "FSCLAS\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "FSCLSY\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "FSCODE\t= " << buffer.substr(byteCount, 11) << std::endl;
   byteCount += 11;
   std::cout << "FSCTLH\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "FSREL\t= " << buffer.substr(byteCount, 20) << std::endl;
   byteCount += 20;
   std::cout << "FSDCTP\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "FSDCDT\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "FSDCXM\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "FSDG\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "FSDGDT\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "FSCLTX\t= " << buffer.substr(byteCount, 43) << std::endl;
   byteCount += 43;
   std::cout << "FSCATP\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "FSCAUT\t= " << buffer.substr(byteCount, 40) << std::endl;
   byteCount += 40;
   std::cout << "FSCRSN\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "FSSRDT\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "FSCTLN\t= " << buffer.substr(byteCount, 15) << std::endl;
   byteCount += 15;
   std::cout << "FSCOP\t= " << buffer.substr(byteCount, 5) << std::endl;
   byteCount += 5;
   std::cout << "FSCPYS\t= " << buffer.substr(byteCount, 5) << std::endl;
   byteCount += 5;
   std::cout << "ENCRYP\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "FBKGC\t= " << buffer.substr(byteCount, 3) << std::endl;
   byteCount += 3;
   std::cout << "ONAME\t= " << buffer.substr(byteCount, 24) << std::endl;
   byteCount += 24;
   std::cout << "OPHONE\t= " << buffer.substr(byteCount, 18) << std::endl;
   byteCount += 18;
   std::cout << "FL\t= " << buffer.substr(byteCount, 12) << std::endl;
   byteCount += 12;
   std::cout << "HL\t= " << buffer.substr(byteCount, 6) << std::endl;
   byteCount += 6;

   byteCount += dumpSubHdr(buffer.substr(byteCount), "NUMI", "LI", 6, 10);
   byteCount += dumpSubHdr(buffer.substr(byteCount), "NUMS", "LS", 4, 6);
   byteCount += dumpSubHdr(buffer.substr(byteCount), "NUMX", "LX", 4, 3);
   byteCount += dumpSubHdr(buffer.substr(byteCount), "NUMT", "LT", 4, 5);
   byteCount += dumpSubHdr(buffer.substr(byteCount), "NUMDES", "LD", 4, 9);
   byteCount += dumpSubHdr(buffer.substr(byteCount), "NUMRES", "LR", 4, 7);

   int UDHDL = atoi(buffer.substr(byteCount, 5).c_str());
   std::cout << "UDHDL\t= " << buffer.substr(byteCount, 5) << std::endl;
   byteCount += 5;

   if(UDHDL > 0)
   {
      std::cout << "UDHOFL\t= " << buffer.substr(byteCount, 3) << std::endl;
      std::cout << "UDHD\t= " << buffer.substr(byteCount + 3, UDHDL - 3)
                << std::endl;
      byteCount += UDHDL;
   }

   int XHDL = atoi(buffer.substr(byteCount, 5).c_str());
   std::cout << "XHDL\t= " << buffer.substr(byteCount, 5) << std::endl;
   byteCount += 5;

   if(XHDL > 0)
   {
      std::cout << "XHDOFL\t= " << buffer.substr(byteCount, 3) << std::endl;
      std::cout << "XHD\t= " << buffer.substr(byteCount + 3, XHDL - 3)
                << std::endl;
      byteCount += XHDL;
   }
}

///////////////////////////////////////////////////////////////
//
//  Function: dumpLUTS
//
//  Description: Output the Look up Table Information
//
//  Inputs: buffer - LUT data segment
//
//  Returns: Number of bytes in the LUT section
//
///////////////////////////////////////////////////////////////
int dumpLUTS(std::string buffer)
{
   int byteCount = 0;                  //----- Count of the number of bytes
   int num;                            //----- Integer value of a field
   int size;                           //----- Integer value of a field

   std::cout << "NLUTS\t= " << buffer.substr(byteCount, 1) << std::endl;
   num = atoi(buffer.substr(byteCount, 1).c_str());
   byteCount += 1;

   if(num > 0)
   {
      std::cout << "NELUTS\t= " << buffer.substr(byteCount, 5) << std::endl;
      size = atoi(buffer.substr(byteCount, 5).c_str());
      byteCount += 5;

      byteCount += num * size;
   }

   return byteCount;
}

///////////////////////////////////////////////////////////////
//
//  Function: dumpBands20
//
//  Description: Output the NITF 2.0 Band information
//
//  Inputs: buffer - Band data segment
//
//  Returns: Number of bytes in the Band section
//
///////////////////////////////////////////////////////////////
int dumpBands20(std::string buffer)
{
   int byteCount = 0;                    //----- Count of the number of bytes
   int num;                              //----- The integer value of a field
   int i;                                //----- Loop Variable
   char label[100];                      //----- Label string

   std::cout << "NBANDS\t= " << buffer.substr(byteCount, 1) << std::endl;
   num = atoi(buffer.substr(byteCount, 1).c_str());
   byteCount += 1;

   for(i=0;i<num;i++)
   {
      sprintf(label, "IREPBAND%d", i+1);
      std::cout << label << "\t= " << buffer.substr(byteCount, 2) << std::endl;
      byteCount += 2;

      sprintf(label, "ISUBCAT%d", i+1);
      std::cout << label << "\t= " << buffer.substr(byteCount, 6) << std::endl;
      byteCount += 6;

      sprintf(label, "IFC%d", i+1);
      std::cout << label << "\t= " << buffer.substr(byteCount, 1) << std::endl;
      byteCount += 1;

      sprintf(label, "IMFLT%d", i+1);
      std::cout << label << "\t= " << buffer.substr(byteCount, 3) << std::endl;
      byteCount += 3;

      byteCount += dumpLUTS(buffer.substr(byteCount));
   }

   return byteCount;
}

///////////////////////////////////////////////////////////////
//
//  Function: dumpBands21
//
//  Description: Output the NITF 2.1 Band information
//
//  Inputs: buffer - Band data segment
//
//  Returns: Number of bytes in the Band section
//
///////////////////////////////////////////////////////////////
int dumpBands21(std::string buffer)
{
   int byteCount = 0;                    //----- Current number of bytes
   int num;                              //----- Integer value of a field
   int i;                                //----- Loop Variable
   char label[100];                      //----- Label string

   std::cout << "NBANDS\t= " << buffer.substr(byteCount, 1) << std::endl;
   num = atoi(buffer.substr(byteCount, 1).c_str());
   byteCount += 1;

   if(num == 0)
   {
      std::cout << "XBANDS\t= " << buffer.substr(byteCount, 5) << std::endl;
      num = atoi(buffer.substr(byteCount, 5).c_str());
      byteCount += 5;
   }

   for(i=0;i<num;i++)
   {
      sprintf(label, "IREPBAND%d", i+1);
      std::cout << label << "\t= " << buffer.substr(byteCount, 2) << std::endl;
      byteCount += 2;

      sprintf(label, "ISUBCAT%d", i+1);
      std::cout << label << "\t= " << buffer.substr(byteCount, 6) << std::endl;
      byteCount += 6;

      sprintf(label, "IFC%d", i+1);
      std::cout << label << "\t= " << buffer.substr(byteCount, 1) << std::endl;
      byteCount += 1;

      sprintf(label, "IMFLT%d", i+1);
      std::cout << label << "\t= " << buffer.substr(byteCount, 3) << std::endl;
      byteCount += 3;

      byteCount += dumpLUTS(buffer.substr(byteCount));
   }

   return byteCount;
}

///////////////////////////////////////////////////////////////
//
//  Function: dumpImg20Hdr
//
//  Description: Output the NITF 2.0 Image information
//
//  Inputs: buffer - Band data segment
//
//  Returns: Number of bytes in the Image section
//
///////////////////////////////////////////////////////////////
void dumpImg20Hdr(std::string buffer)
{
   int byteCount = 0;                     //----- Count of Number of bytes
   int num;                               //----- Integer value of a field
   int i;                                 //----- Loop Variable
   char label[100];                       //----- Lable name for a field

   std::cout << "IM\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "IID\t= " << buffer.substr(byteCount, 10) << std::endl;
   byteCount += 10;
   std::cout << "IDATIM\t= " << buffer.substr(byteCount, 14) << std::endl;
   byteCount += 14;
   std::cout << "TGTID\t= " << buffer.substr(byteCount, 17) << std::endl;
   byteCount += 17;
   std::cout << "ITITLE\t= " << buffer.substr(byteCount, 80) << std::endl;
   byteCount += 80;
   std::cout << "ISCLAS\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "ISCODE\t= " << buffer.substr(byteCount, 40) << std::endl;
   byteCount += 40;
   std::cout << "ISCTLH\t= " << buffer.substr(byteCount, 40) << std::endl;
   byteCount += 40;
   std::cout << "ISREL\t= " << buffer.substr(byteCount, 40) << std::endl;
   byteCount += 40;
   std::cout << "ISCAUT\t= " << buffer.substr(byteCount, 20) << std::endl;
   byteCount += 20;
   std::cout << "ISCTLN\t= " << buffer.substr(byteCount, 20) << std::endl;
   byteCount += 20;
   std::cout << "ISDWNG\t= " << buffer.substr(byteCount, 6) << std::endl;

   if(buffer.substr(byteCount, 6) == "999998")
   {
      std::cout << "ISDEVT\t= " << buffer.substr(byteCount + 6, 40)
                << std::endl;
      byteCount += 40;
   }
   byteCount += 6;

   std::cout << "ENCRYP\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "ISORCE\t= " << buffer.substr(byteCount, 42) << std::endl;
   byteCount += 42;
   std::cout << "NROWS\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "NCOLS\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "PVTYPE\t= " << buffer.substr(byteCount, 3) << std::endl;
   byteCount += 3;
   std::cout << "IREP\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "ICAT\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "ABPP\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "PJUST\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "ICORDS\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;

   if(buffer.substr(byteCount-1, 1) != "N")
   {
      std::cout << "IGEOLO\t= " << buffer.substr(byteCount, 60) << std::endl;
      byteCount += 60;
   }

   std::cout << "NICOM\t= " << buffer.substr(byteCount, 1) << std::endl;
   num = atoi(buffer.substr(byteCount, 1).c_str());
   byteCount += 1;

   for(i=0;i<num;i++)
   {
      sprintf(label, "ICOM%d", i+1);
      std::cout << label << "\t= " << buffer.substr(byteCount, 80) << std::endl;
      byteCount += 80;
   }

   std::cout << "IC\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;

   if(buffer.substr(byteCount-2, 2) != "NC" &&
      buffer.substr(byteCount-2, 2) != "NM")
   {
      std::cout << "COMRAT\t= " << buffer.substr(byteCount, 4) << std::endl;
      byteCount += 4;
   }

   byteCount += dumpBands20(buffer.substr(byteCount));

   std::cout << "ISYNC\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "IMODE\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "NBPR\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "NBPC\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "NPPBH\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "NPPBV\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "NBPP\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "IDLVL\t= " << buffer.substr(byteCount, 3) << std::endl;
   byteCount += 3;
   std::cout << "IALVL\t= " << buffer.substr(byteCount, 3) << std::endl;
   byteCount += 3;
   std::cout << "ILOC\t= " << buffer.substr(byteCount, 10) << std::endl;
   byteCount += 10;
   std::cout << "IMAG\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "UDIDL\t= " << buffer.substr(byteCount, 5) << std::endl;
   int UDIDL = atoi(buffer.substr(byteCount, 5).c_str());
   byteCount += 5;

   if(UDIDL > 0)
   {
      std::cout << "UDOFL\t= " << buffer.substr(byteCount, 3) << std::endl;
      byteCount += 3;
      std::cout << "UDID\t= " << buffer.substr(byteCount, UDIDL - 3)
                << std::endl;
      byteCount += UDIDL - 3;
   }

   std::cout << "IXSHDL\t= " << buffer.substr(byteCount, 5) << std::endl;
   int IXSHDL = atoi(buffer.substr(byteCount, 5).c_str());
   byteCount += 5;

   if(IXSHDL > 0)
   {
      std::cout << "IXSOFL\t= " << buffer.substr(byteCount, 3) << std::endl;
      byteCount += 3;
      std::cout << "IXSHD\t= " << buffer.substr(byteCount, IXSHDL - 3)
                << std::endl;
      byteCount += IXSHDL - 3;
   }
}

///////////////////////////////////////////////////////////////
//
//  Function: dumpImg21Hdr
//
//  Description: Output the NITF 2.1 Image information
//
//  Inputs: buffer - Band data segment
//
//  Returns: Number of bytes in the Image section
//
///////////////////////////////////////////////////////////////
void dumpImg21Hdr(std::string buffer)
{
   int byteCount = 0;                     //----- Count of number of bytes
   int num;                               //----- Integer value of a field
   int i;                                 //----- Loop Variable
   char label[100];                       //----- Lable name for a field

   std::cout << "IM\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "IID1\t= " << buffer.substr(byteCount, 10) << std::endl;
   byteCount += 10;
   std::cout << "IDATIM\t= " << buffer.substr(byteCount, 14) << std::endl;
   byteCount += 14;
   std::cout << "TGTID\t= " << buffer.substr(byteCount, 17) << std::endl;
   byteCount += 17;
   std::cout << "IID2\t= " << buffer.substr(byteCount, 80) << std::endl;
   byteCount += 80;
   std::cout << "ISCLAS\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "ISCLSY\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "ISCODE\t= " << buffer.substr(byteCount, 11) << std::endl;
   byteCount += 11;
   std::cout << "ISCTLH\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "ISREL\t= " << buffer.substr(byteCount, 20) << std::endl;
   byteCount += 20;
   std::cout << "ISDCTP\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "ISDCDT\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "ISDCXM\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "ISDG\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "ISDGDT\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "ISCLTX\t= " << buffer.substr(byteCount, 43) << std::endl;
   byteCount += 43;
   std::cout << "ISCATP\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "ISCAUT\t= " << buffer.substr(byteCount, 40) << std::endl;
   byteCount += 40;
   std::cout << "ISCRSN\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "ISSRDT\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "ISCTLN\t= " << buffer.substr(byteCount, 15) << std::endl;
   byteCount += 15;
   std::cout << "ENCRYP\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "ISORCE\t= " << buffer.substr(byteCount, 42) << std::endl;
   byteCount += 42;
   std::cout << "NROWS\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "NCOLS\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "PVTYPE\t= " << buffer.substr(byteCount, 3) << std::endl;
   byteCount += 3;
   std::cout << "IREP\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "ICAT\t= " << buffer.substr(byteCount, 8) << std::endl;
   byteCount += 8;
   std::cout << "ABPP\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "PJUST\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "ICORDS\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;

   if(buffer.substr(byteCount-1, 1) != " ")
   {
      std::cout << "IGEOLO\t= " << buffer.substr(byteCount, 60) << std::endl;
      byteCount += 60;
   }

   std::cout << "NICOM\t= " << buffer.substr(byteCount, 1) << std::endl;
   num = atoi(buffer.substr(byteCount, 1).c_str());
   byteCount += 1;

   for(i=0;i<num;i++)
   {
      sprintf(label, "ICOM%d", i+1);
      std::cout << label << "\t= " << buffer.substr(byteCount, 80) << std::endl;
      byteCount += 80;
   }

   std::cout << "IC\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;

   if(buffer.substr(byteCount-2, 2) != "NC" &&
      buffer.substr(byteCount-2, 2) != "NM")
   {
      std::cout << "COMRAT\t= " << buffer.substr(byteCount, 4) << std::endl;
      byteCount += 4;
   }

   byteCount += dumpBands21(buffer.substr(byteCount));

   std::cout << "ISYNC\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "IMODE\t= " << buffer.substr(byteCount, 1) << std::endl;
   byteCount += 1;
   std::cout << "NBPR\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "NBPC\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "NPPBH\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "NPPBV\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "NBPP\t= " << buffer.substr(byteCount, 2) << std::endl;
   byteCount += 2;
   std::cout << "IDLVL\t= " << buffer.substr(byteCount, 3) << std::endl;
   byteCount += 3;
   std::cout << "IALVL\t= " << buffer.substr(byteCount, 3) << std::endl;
   byteCount += 3;
   std::cout << "ILOC\t= " << buffer.substr(byteCount, 10) << std::endl;
   byteCount += 10;
   std::cout << "IMAG\t= " << buffer.substr(byteCount, 4) << std::endl;
   byteCount += 4;
   std::cout << "UDIDL\t= " << buffer.substr(byteCount, 5) << std::endl;
   int UDIDL = atoi(buffer.substr(byteCount, 5).c_str());
   byteCount += 5;

   if(UDIDL > 0)
   {
      std::cout << "UDOFL\t= " << buffer.substr(byteCount, 3) << std::endl;
      byteCount += 3;
      std::cout << "UDID\t= " << buffer.substr(byteCount, UDIDL - 3)
                << std::endl;
      byteCount += UDIDL - 3;
   }

   std::cout << "IXSHDL\t= " << buffer.substr(byteCount, 5) << std::endl;
   int IXSHDL = atoi(buffer.substr(byteCount, 5).c_str());
   byteCount += 5;

   if(IXSHDL > 0)
   {
      std::cout << "IXSOFL\t= " << buffer.substr(byteCount, 3) << std::endl;
      byteCount += 3;
      std::cout << "IXSHD\t= " << buffer.substr(byteCount, IXSHDL - 3)
                << std::endl;
      byteCount += IXSHDL - 3;
   }
}

///////////////////////////////////////////////////////////////
//
//  Function: dumpHdr
//
//  Description: Output the NITF 2.0 data
//
//  Inputs: isd - structure of the data
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void dumpHdr(Nitf20Isd *isd)
{
   int i = 0;                        //----- Loop Variables

   dumpNitf20Hdr(isd->fileHeader());

   std::cout << "***************** END OF FILE HEADER *************\n";

   std::cout << isd->fileTres().size() << " fileTRE records\n";
   for (i = 0; i < isd->fileTres().size(); i++)
   {
      if ( isd->fileTres().size() > 0)
      {
	 std::cout << "\trecord |" << isd->fileTres()[i].data() << std::endl;
	 printchar(isd->fileTres()[i].data().c_str(), isd->fileTres()[i].length());
	 std::cout << "|" << std::endl;
	 std::cout << "\tname   " << isd->fileTres()[i].name() << std::endl;
	 std::cout << "\tlength " << isd->fileTres()[i].length() << "\n\n";
      }
   }
   int NUMI = isd->images().size();
   std::cout << NUMI << " image(s)\n";
   for (i = 0; i < NUMI; i++)
   {
      std::cout << "image sub-header for image " << i + 1 << std::endl;
      dumpImg20Hdr(isd->images()[i].subHeader());
      std::cout << "***************** END OF IMAGE HEADER *************\n";
      std::cout  << isd->images()[i].imageTres().size() << " TREs in image "
	         << i + 1 << std::endl;
      for (int j = 0; j < isd->images()[i].imageTres().size(); j++)
      {
	    std::cout << "\tname   " << isd->images()[i].imageTres()[j].name()
		      << std::endl;
	    std::cout << "\tlength "
			  << isd->images()[i].imageTres()[j].length() << std::endl;
		
	    std::cout << "\trecord |";
	    printchar(isd->images()[i].imageTres()[j].data().c_str(),
                      isd->images()[i].imageTres()[j].length());
	    std::cout << "|**\n";
	    std::cout << "\trecordlen "
		      << isd->images()[i].imageTres()[j].data().size()
		      << "\n\n";
      }
   }

   int NUMD = isd->fileDess().size();
   std::cout << NUMD << " DES(s)\n";
   for (i = 0; i < NUMD; i ++)
   {
      std::cout << "\n\tDES hdr " << i + 1 << std::endl;
      std::cout << "\theader length " << isd->fileDess()[i].subHeader().size()
                << std::endl;
      std::cout << "\theader: |" << isd->fileDess()[i].subHeader()<< "|\n\n";
      std::cout << "\tdata length " << isd->fileDess()[i].data().size()
                << std::endl;
   }
};

///////////////////////////////////////////////////////////////
//
//  Function: dumpHdr
//
//  Description: Output the NITF 2.1 data
//
//  Inputs: isd - structure of the data
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void dumpHdr(Nitf21Isd *isd)
{
   int i = 0;                          //----- Loop Variable

   dumpNitf21Hdr(isd->fileHeader());

   std::cout << "***************** END OF FILE HEADER *************\n";
   std::cout << isd->fileTres().size() << " fileTre records\n";
   for (i = 0; i < isd->fileTres().size(); i++)
   {
	 std::cout << "\trecord |";
	 printchar(isd->fileTres()[i].data().c_str(), isd->fileTres()[i].length());
	 std::cout << "|" << std::endl;
	 std::cout << "\tname   " << isd->fileTres()[i].name() << std::endl;
	 std::cout << "\tlength " << isd->fileTres()[i].length() << "\n\n";
   }
   int NUMI = isd->images().size();
   std::cout << NUMI << " image(s)\n";
   for (i = 0; i < NUMI; i++)
   {
      std::cout << "image sub-header for image " << i + 1 << std::endl;
      dumpImg21Hdr(isd->images()[i].subHeader());
      std::cout << "***************** END OF IMAGE HEADER *************\n";
      std::cout  << isd->images()[i].imageTres().size() << " TREs in image "
	         << i + 1 << std::endl;
      for (int j = 0; j < isd->images()[i].imageTres().size(); j++)
      {
	    std::cout << "\tname   " << isd->images()[i].imageTres()[j].name()
		      << std::endl;
	    std::cout << "\tlength "
		      << isd->images()[i].imageTres()[j].length() << std::endl;
	    std::cout << "\trecord |";
	    printchar(isd->images()[i].imageTres()[j].data().c_str(),
                      isd->images()[i].imageTres()[j].length());
	    std::cout << "|**\n";
	    std::cout << "\trecordlen "
		      << isd->images()[i].imageTres()[j].data().size()
		      << "\n\n";
         //}
      }
   }

   int NUMD = isd->fileDess().size();
   std::cout << NUMD << " DES(s)\n";
   for (i = 0; i < NUMD; i ++)
   {
      std::cout << "\n\tDES hdr " << i + 1 << std::endl;
      std::cout << "\theader length " << isd->fileDess()[i].subHeader().size()
                << std::endl;
      std::cout << "\theader: |" << isd->fileDess()[i].subHeader() << "|\n\n";
      std::cout << "\tdata length " << isd->fileDess()[i].data().size()
                << std::endl;
   }
}

///////////////////////////////////////////////////////////////
//
//     Function: writeStateFile
//
//  Description: Write sensor model state to a file
//
//       Inputs: fname - Name of file to write sensor model state
//               state - Sensor Model State Data to write
//
//      Returns: none
//
///////////////////////////////////////////////////////////////
void writeStateFile(std::string fname, std::string state) throw (Error)
{
   Error csmerr;
   size_t buffsize;
   FILE *ofile = NULL;

#ifdef _WIN32
   ofile = fopen (fname.c_str(), "w");
#else
   ofile = fopen64 (fname.c_str(), "w");
#endif

   if (!ofile)
   {
      std::string errstr ("Unable to open output file: (" + fname + ")");
      std::cerr << errstr << std::endl;
      csmerr.setError (Error::FILE_WRITE,
			  errstr,
			  "writeStateFile");
      throw (csmerr);
   }

   // open ok
   buffsize = state.length();
   size_t size = fwrite(state.c_str(), 1, buffsize, ofile);
   if (size != buffsize)
   {
      std::string errstr ("failure writing output file " + fname);
      std::cerr << errstr << std::endl;
      csmerr.setError (Error::FILE_WRITE,
			  errstr,
			  "writeStateFile");
	  if(ofile)
	  {
		  fclose(ofile);
	  }
      throw (csmerr);
   }
   if(ofile)
   {
      fclose(ofile);
   }
};

///////////////////////////////////////////////////////////////
//
//  Function: readStateFile
//
//  Description: Read in a sensor model state file
//
//  Inputs:          fname   - Name of file containing sensor model state
//
//  Returns: state_from_file - Data read from the requested file
//
///////////////////////////////////////////////////////////////
std::string readStateFile(std::string fname) throw (Error)
{
   Error csmerr;
   FILE *ifile = NULL;
   int buffsize;

#ifdef _WIN32
   off_t byte_size_of_file;
#else
   off64_t byte_size_of_file;
#endif

   char* buff;
   std::string state_from_file;

#ifdef _WIN32
   ifile = fopen (fname.c_str(), "rb");
#else
   ifile = fopen64 (fname.c_str(), "rb");
#endif

   if (! ifile)
   {
      std::string errstr ("Unable to open input file: (" + fname + ")");
      std::cerr << errstr << std::endl;
      csmerr.setError (Error::FILE_READ,
			  errstr,
			  "readStateFile");
      throw (csmerr);
   }

#ifdef _WIN32
   fseek(ifile, 0, SEEK_END);
   byte_size_of_file = ftell(ifile);
   fseek(ifile, 0, SEEK_SET);
#else
   fseeko64(ifile, 0, SEEK_END);
   byte_size_of_file = ftello64(ifile);
   fseeko64(ifile, 0, SEEK_SET);
#endif

   buffsize = (int)(byte_size_of_file);
   buff = (char*) calloc (buffsize, sizeof (char));
   if (buff == NULL)
   {
      csmerr.setError (Error::MEMORY,
			  "Unable to allocate file buffer",
			  "readStateFile");
      throw (csmerr);
   }

   // open ok
   size_t size = fread (buff, 1, buffsize, ifile);
   if (size != (size_t)buffsize)
   {
      std::string errstr ("failure reading input file " + fname);
      std::string state = std::string(buff);
	  std::cerr << errstr << " - size read = " << size
		        << ", size expected = " << buffsize
		        << ", state.length = (" << state.length() << ")"
				<< std::endl;
      csmerr.setError (Error::FILE_READ,
			  errstr,
			  "readStateFile");
      if(ifile)
      {
         fclose(ifile);
      }
      throw (csmerr);
   }

   state_from_file = std::string(buff, buffsize);
   delete [] buff;
   if(ifile)
   {
      fclose(ifile);
   }
   return state_from_file;
};
