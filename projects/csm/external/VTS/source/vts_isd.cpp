//#############################################################################
//
//    FILENAME:   vts_isd.cpp
//
//
//    DESCRIPTION:
//	This module contains functions to support TSMISD classes
//	These classes are bytestream, filename, NITF 2.0 and NITF 2.1    
//
//    NOTES:
//                       Date          Author      Comment    
//    SOFTWARE HISTORY:  13-Jan-2004   LMT         Initial version.
//                       10 Mar 2010   Don Leonard     CCB Change Removed DATA_NOT_AVAILABLE Error
//                       22 Jul 2010   Don Leonard     CCB Change add writeStateFile and readStateFile
//
//#############################################################################
#ifdef _WIN32
#pragma warning( disable : 4290 )
#endif
#include "TSMError.h"
#include "TSMWarning.h"
#include "VTSMisc.h"
#include "TSMImageSupportData.h"
#include "TSMISDNITF20.h"
#include "TSMISDNITF21.h"
#include "TSMISDByteStream.h"
#include "TSMISDFilename.h"

#ifdef _WIN32
# include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string.h>

#ifdef _WIN32
#include <sys\stat.h>
#else
#include <sys/stat.h>
#endif

bool nitfDebugFlag = false;        //----- NITF Debug Flag

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
void initFilenameISD( filenameISD *isdfilename,
                      std::string filename)
{
   isdfilename->_filename = filename;
}

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
void initBytestreamISD( bytestreamISD *bytestream,
                        std::string filename)
   throw (TSMError)
{
   TSMError err;
   const size_t bufflen = 1048576;
   char *buff = new char[bufflen];
   int numread = 0;
   int totread = 0;
   FILE * ifp = fopen (filename.c_str(), "rb");
   bytestream->_isd = ""; // empty the string

   if (ifp == NULL)
   {
      err.setTSMError( TSMError::FILE_READ, 
		       "Unable to open file " + filename, 
		       "initBytestreamISD" );
      throw err;
   }

   if (!buff)
   {
      err.setTSMError( TSMError::MEMORY, 
		       "Unable to allocate bytestream buffer", 
		       "initBytestreamISD" );
      throw err;
   }

   while ((numread = fread ( buff, 1, bufflen, ifp)) == bufflen)
   {
      totread += numread;
      bytestream->_isd += std::string(buff, bufflen);
   }

   if (numread > 0)
   {
      totread += numread;
      bytestream->_isd += std::string(buff, numread);
   }
  
   bytestream->_isd.erase (totread);
   delete [] buff;

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
void dumpbytestream( bytestreamISD &bytestream,
                     int len)
{
   std::cout << "\nLength of bytestream: " 
             << bytestream._isd.length() << std::endl;
   std::cout << "First " << len << " characters of ISD Bytestream are \n " 
             <<  bytestream._isd.substr(1,len) << std::endl;
}

///////////////////////////////////////////////////////////////
//
//  Function: initNITF20ISD
//
//  Description: Initialize an NITF 2.0 file to be read
//
//  Inputs: isd        - An Nitf data structure
//          fname      - Filename to be loaded
//          imageIndex - Index of an image to be loaded
//
//  Returns: TSMWarning if an error is found
//
///////////////////////////////////////////////////////////////
TSMWarning *initNITF20ISD( NITF_2_0_ISD *isd,
                           std::string fname,
                           const int imageIndex)
   throw (TSMError)
{
   TSMWarning *warn = NULL;
   std::string ftype ("NITF20");
   FILE *ifile = NULL;
   struct stat statbuf; // to check for file presence and size
   char *buff = NULL;

   try {
      ifile = fillBuff(fname, statbuf, &buff);

      warn = parseFile(isd, ifile, statbuf, buff, imageIndex);
   } catch (TSMError err) {
      delete buff;
      throw(err);
   }

   delete buff;

   return warn;
};

///////////////////////////////////////////////////////////////
//
//  Function: initNITF21ISD
//
//  Description: Initialize an NITF 2.1 file to be read
//
//  Inputs: isd        - An Nitf data structure
//          fname      - Filename to be loaded
//          imageIndex - Index of an image to be loaded
//
//  Returns: TSMWarning if an error is found
//
///////////////////////////////////////////////////////////////
TSMWarning *initNITF21ISD( NITF_2_1_ISD *isd,
                           std::string fname,
                           const int imageIndex)
   throw (TSMError)
{
   TSMWarning *warn = NULL;
   FILE *ifile = NULL;
   std::string ftype ("NITF21");
   struct stat statbuf; // to check for file presence and size
   char *buff = NULL;
 
   try {
      ifile = fillBuff(fname, statbuf, &buff);

      warn = parseFile(isd, ifile, statbuf, buff, imageIndex);
   } catch (TSMError err) {
      delete buff;
      throw(err);
   }

   delete buff;

   return warn;
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
FILE * fillBuff( std::string fname,
                 struct stat &statbuf,
                 char** buff) throw (TSMError)
{
   TSMError tsmerr;
   FILE *ifile = NULL;
   const int MAXNITFFILEHDRLEN = 999999;
   int buffsize;

   if (stat(fname.c_str(), &statbuf))
   {
      tsmerr.setTSMError (TSMError::FILE_READ, 
			  "Unable to locate input file " + fname, 
			  "TSMNITF20::fillBuff");
      throw (tsmerr);
   }
   buffsize = (MAXNITFFILEHDRLEN > statbuf.st_size) ? 
      statbuf.st_size : MAXNITFFILEHDRLEN;
   *buff = (char*) calloc (buffsize, sizeof (char));
   if (*buff == NULL)
   {
      tsmerr.setTSMError (TSMError::MEMORY, 
			  "Unable to allocate file buffer", 
			  "TSMNITF20::fillBuff");
      throw (tsmerr);
   }

   // malloc ok
   ifile = fopen (fname.c_str(), "rb");
   if (! ifile)
   {
      std::string errstr ("Unable to open input file " + fname);
      tsmerr.setTSMError (TSMError::FILE_READ, 
			  errstr, 
			  "TSMNITF20::fillBuff");
      throw (tsmerr);
   }
   // open ok
   off_t size = fread (*buff, 1, buffsize, ifile);
   if (size != buffsize)
   {
      std::string errstr ("failure reading input file " + fname);
      tsmerr.setTSMError (TSMError::FILE_READ, 
			  errstr, 
			  "TSMNITF20::fillBuff");
      throw (tsmerr);
   }

   return ifile;
};

///////////////////////////////////////////////////////////////
//
//  Function: GetBufferInt
//
//  Description: Convert a String from the buffer to an integer
//
//  Inputs: pBuffer - The file buffer that contains the string
//          offset  - First character of the string
//          length  - The size of the string
//
//  Returns: The value of the converted string
//
///////////////////////////////////////////////////////////////
int GetBufferInt(const char* pBuffer, int offset, int length)
{
   char* value = new char[length + 1];
   int intValue;

   memcpy(value, &pBuffer[offset], length);
   value[length] = '\0';
   intValue = atoi(value);

   delete value;

   return intValue;
}

///////////////////////////////////////////////////////////////
//
//  Function: DisplayValue
//
//  Description: Output an interger value to the screen
//
//  Inputs: pName  - Name of the variable to be displayed
//          value  - The interger value to be displayed
//          offset - Position of the value in the buffer
//          length - The length of the value
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void DisplayValue(const char* pName, int value, int offset, int length)
{
   std::cout << pName << "\t= " << value << " from buffer at location "
             << offset << " with a length " << length << std::endl;
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
void DisplayValue(const char* pName, std::string value, int offset, int length)
{
   std::string strValue = value.substr(0, length);
   std::cout << pName << "\t= " << strValue << " from buffer at location "
             << offset << " with a length " << length << std::endl;
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
//  Returns: TSMWarning - warning if any occur
//
///////////////////////////////////////////////////////////////
TSMWarning *parseFile( NITF_2_0_ISD *isd,
                       FILE *ifile,
                       struct stat &statbuf,
                       char* buff,
                       const int imageIndex) throw (TSMError)
{
   TSMError err;                      //----- Error class
   TSMWarning *warning = NULL;        //----- Warning class
   char OutputString[200];            //----- Output String
   int fl;		              //----- file length
   int hl;	                      //----- header length
   int crypt_offset;                  //----- Offset to encryption field
   int class_offset = 119;            //----- Offset to classification field
   int fsdwng_offset = 280;           //----- Offset to downgrade field

   if (!buff) // fillBuff failed -- should never get here
   {
      err.setTSMError (TSMError::INVALID_USE,
  		       "No ISD data buffer: fillBuff failed",
		       "parseFile");
      throw (err);
   }

   if(strncmp(buff, "NITF02.00", 9))
      warning = new TSMWarning( TSMWarning::UNKNOWN_WARNING, 
			"Not a valid NITF 2.0 file", "parseFile(nitf2.0)");

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
   int fl_offset = crypt_offset + 46;
   fl = GetBufferInt(buff, fl_offset, 12);
   if(nitfDebugFlag)
      DisplayValue("FL", fl, fl_offset, 12);

   if (statbuf.st_size != fl)
   {
      sprintf(OutputString,
        "The value of File length field %d, does not match actual file length %d", fl, statbuf.st_size);
      warning = new TSMWarning( TSMWarning::UNKNOWN_WARNING, 
			OutputString, "parseFile(nitf2.0)");
   }

   // get 2.0 file header length 
   int hl_offset = fl_offset + 12;
   hl = GetBufferInt(buff, hl_offset, 6);
   if(nitfDebugFlag)
      DisplayValue("HL", hl, hl_offset, 6);

   if (statbuf.st_size < hl)
   {
      //// choke
      //err.setTSMError(TSMError::DATA_NOT_AVAILABLE, 
		    // "ISD file smaller than indicated header length",
		    // "parseFile(nitf2.0)");
      //throw err;
      warning = new TSMWarning( TSMWarning::DATA_NOT_AVAILABLE, 
			"ISD file smaller than indicated header length", "parseFile(nitf2.0)");
   }

   // Copy the file header  
   isd->fileHeader.assign (buff, hl);

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
   unsigned int des_offset = hl; // This value will be used to point to
				 // the location in the 2.0 file of the
				 // DES data
   ////////////////////////////////////////////
   // process image segments length info  /////
   ////////////////////////////////////////////

   int numi_offset = hl_offset + 6;
   int NUMI = GetBufferInt(buff, numi_offset, 3);
   if(nitfDebugFlag)
      DisplayValue("NUMI", NUMI, numi_offset, 3);
   isd->numImages = NUMI;

   int seg_offset = numi_offset + 3;
  
   if (NUMI > 0)
   {
      std::vector <int> vImageHdrLengths (NUMI);
      std::vector <int> vImageLengths (NUMI);
      for (seg_index = 0; seg_index < isd->numImages; seg_index++)
      {
         sprintf(OutputString, "LISH%03d", seg_index+1);
	 vImageHdrLengths.at(seg_index) = GetBufferInt(buff, seg_offset, 6);
         if(nitfDebugFlag)
            DisplayValue(OutputString, vImageHdrLengths.at(seg_index),
                         seg_offset, 6);
	 des_offset += vImageHdrLengths.at(seg_index);
	 seg_offset += 6;

         sprintf(OutputString, "LI%03d", seg_index+1);
	 vImageLengths[seg_index] = GetBufferInt(buff, seg_offset, 10);
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
      isd->numDESs = NUMD;
      seg_offset += 3;
      std::vector <int> DEShdrlengths (NUMD);
      std::vector <int> DESlengths (NUMD);

      if (NUMD > 0)
         for (seg_index = 0; seg_index < NUMD; seg_index++)
         {
            sprintf(OutputString, "LDSH%03d", seg_index+1);
	    DEShdrlengths[seg_index] = GetBufferInt(buff, seg_offset, 4);
            if(nitfDebugFlag)
               DisplayValue(OutputString, DEShdrlengths[seg_index],
                            seg_offset, 4);
	    seg_offset += 4;

            sprintf(OutputString, "LD%03d", seg_index+1);
	    DESlengths[seg_index] = GetBufferInt(buff, seg_offset, 9);
            if(nitfDebugFlag)
               DisplayValue(OutputString, DESlengths[seg_index], seg_offset, 9);
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
      unsigned int treLen = HD.length();
      // parse file tre's
      if (treLen)
      {
         isd->numTREs = 0;
         parseTRE (treLen,  HD, isd->numTREs, &isd->fileTREs);
      }

      ////////////////////////////////////////////
      /////// End of fileheader code  ////////////
      ////////////////////////////////////////////
      if (seg_offset != hl)
      {
         std::cout << "Parser error in file header -- hl = " << hl 
		   << " offset = " << seg_offset << std::endl;
         exit (666);
      }

      /////// Begin 2.0 image sub-header parsing
      if (NUMI > 0)
         parseImages(isd, ifile, hl,
                     vImageHdrLengths, vImageLengths, imageIndex);
      if (NUMD > 0)
         parseDES(isd, ifile, des_offset, DEShdrlengths, DESlengths);
   }

   return warning;
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
//  Returns: TSMWarning - warning if any occur
//
///////////////////////////////////////////////////////////////
TSMWarning *parseFile( NITF_2_1_ISD *isd,
                       FILE *ifile,
                       struct stat &statbuf,
                       char* buff,
                       const int imageIndex) throw (TSMError)
{
   TSMError err;                      //----- Error class
   TSMWarning *warn = NULL;           //----- Warning class
   char OutputString[200];            //----- Output String
   int fl;		              //----- file length
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
      err.setTSMError (TSMError::INVALID_USE,
		       "No ISD data buffer: fillBuff failed",
		       "parseFile");
      throw (err);
   }

   if(strncmp(buff, "NITF02.10", 9))
      warn = new TSMWarning( TSMWarning::UNKNOWN_WARNING, 
			"Not a valid NITF 2.1 file", "parseFile(nitf2.1)");

   strncpy(OutputString, buff, 9);
   OutputString[9] = '\0';
   if(nitfDebugFlag)
     std::cout << "File Version = " << OutputString << std::endl;

   fl = GetBufferInt(buff, FLOFFSET, FLLEN);
   if(nitfDebugFlag)
      DisplayValue("FL", fl, FLOFFSET, FLLEN);
   if( statbuf.st_size != fl)
   {
      sprintf(OutputString,
        "The value of File length field %d, does not match actual file length %d", fl, statbuf.st_size);
      warn = new TSMWarning( TSMWarning::UNKNOWN_WARNING, 
			OutputString, "parseFile(nitf2.1)");
   }

   hl = GetBufferInt(buff, HLOFFSET, HLLEN);
   if(nitfDebugFlag)
      DisplayValue("HL", hl, HLOFFSET, HLLEN);
   if( statbuf.st_size < hl)
   {
      //// choke
      //err.setTSMError(TSMError::DATA_NOT_AVAILABLE, 
		    //  "ISD file smaller than indicated header length",
		    //  "parseFile(nitf2.1)");
      //throw err;
      warn = new TSMWarning( TSMWarning::DATA_NOT_AVAILABLE, 
			"ISD file smaller than indicated header length", "parseFile(nitf2.1)");
   }

   // Copy the file header  
   isd->fileHeader.assign (buff, hl);

   unsigned int des_offset = hl;  // This value will be used to locate
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
   int seg_offset = NUMIOFFSET;
   ////////////////////////////////////////////////
   // process 2.1 image segments length info  /////
   ////////////////////////////////////////////////

   int NUMI = GetBufferInt(buff, seg_offset, NUMILEN);
   if(nitfDebugFlag)
      DisplayValue("NUMI", NUMI, seg_offset, NUMILEN);
   // std::cout << "**** NUMI = " << NUMI << std::endl; 
   isd->numImages = NUMI;
   std::vector <int> vImageHdrLengths (NUMI);
   std::vector <int> vImageLengths (NUMI);

   seg_offset = seg_offset + 3;
   for (seg_index = 0; seg_index < isd->numImages; seg_index++)
   {
      sprintf(OutputString, "LISH%03d", seg_index + 1);
      vImageHdrLengths.at(seg_index) = GetBufferInt(buff, seg_offset, 6);
      if(nitfDebugFlag)
         DisplayValue(OutputString, vImageHdrLengths.at(seg_index),
                      seg_offset, 6);
      des_offset += vImageHdrLengths.at(seg_index);
      seg_offset += 6;

      sprintf(OutputString, "LI%03d", seg_index + 1);
      vImageLengths[seg_index] = GetBufferInt(buff, seg_offset, 10);
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
   isd->numDESs = NUMD;
   seg_offset += 3;
   std::vector <int> Dhdrlengths (NUMD);
   std::vector <int> Dlengths (NUMD);
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
  
   int udidl = HD.length();
   // if any file tre data parse it
   if (udidl > 0)
      parseTRE (udidl, HD, isd->numTREs, &(isd->fileTREs));
   if (NUMI > 0)
      parseImages(isd, ifile, hl, vImageHdrLengths, vImageLengths, imageIndex);
   if (NUMD > 0)
      parseDES(isd, ifile, des_offset, Dhdrlengths, Dlengths);

   return warn;
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
void parseImages(NITF_2_0_ISD * isd, 
                 FILE *ifile,
		 const int HeaderLength, 
		 const std::vector<int> vImageHdrLengths, 
		 const std::vector<int> vImageLengths,
                 const int imageIndex)
{
   int numComments;                     //----- Number cf comments
   int CommentLength = 80;              //----- Length of Comments 
   int numBands;                        //----- Number of Bands
   int numLUTS;                         //----- Number of LUT
   int numLUTEnt;                       //----- Size of LUT entry
   int byteCount;                       //----- Current buffer position
   std::string pbuff;                   //----- Pointer Buffer
   char* pBuffer = NULL;                //----- Image data buffer
   int index, band;                     //----- Loop Variables
   int udidl;                           //----- User Defind data length
   int udofl;                           //----- User Overflow length
   int ixshdl;                          //----- Extended Data Length
   int ixsofl;                          //----- Extended Overflow length 
   int imageStartByte = HeaderLength;   //----- Image Starting Byte
   std::string UHD;                     //----- TRE Data Segment
   std::string XHD;                     //----- TRE Data Segment
   std::string TREBuffer;               //----- TRE Data Segment

   if(imageIndex == -1)
      isd->images = new image[isd->numImages];
   else
      isd->images = new image[1];
   for(index = 0; index < isd->numImages; index++)
   {     
      if(nitfDebugFlag)
         std::cout << "Image Segment staring at " << imageStartByte
                   << std::endl;

      pBuffer = getSegment(ifile,
                        imageStartByte,
                        vImageHdrLengths[index] + vImageLengths[index]);

      // copy image header including image tre's
      if(imageIndex == -1)
         isd->images[index].imageSubHeader.assign(
            pBuffer,vImageHdrLengths[index]);
      else if(imageIndex == index)
         isd->images[0].imageSubHeader.assign(
            pBuffer,vImageHdrLengths[index]);

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
      
      //---- Get Image Corrdinates
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

      if(imageIndex == -1)
         isd->images[index].numTREs = 0;
      else if(imageIndex == index)
         isd->images[0].numTREs = 0;

      if(udidl > 0)
      {
         udofl = GetBufferInt(pBuffer, byteCount, 3);
         if(nitfDebugFlag)
            DisplayValue("UDOFL", udofl, byteCount + imageStartByte, 3);
                              
         // if image tre data available parse it
         if(nitfDebugFlag)
            std::cout << "Image TRE data starts at "
                      << byteCount + imageStartByte
                      << " bytes." << std::endl;
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
            std::cout << "Image TRE data starts at "
                      << byteCount + imageStartByte
                      << " bytes." << std::endl;
         //if(nitfDebugFlag)
         //   DisplayValue("IXSHD", &pBuffer[byteCount],
         //                byteCount + imageStartByte, ixshdl - 3);
         XHD.assign(&pBuffer[byteCount + 3], ixshdl - 3);
         byteCount += ixshdl;
      }
      
      TREBuffer = UHD + XHD;
      if(TREBuffer.length() > 0)
      {
         if(imageIndex == -1)
	    parseTRE(TREBuffer.length(), TREBuffer,
	             isd->images[index].numTREs, 
	             &(isd->images[index].imageTREs));
         else if(imageIndex == index)
	    parseTRE(TREBuffer.length(), TREBuffer,
	             isd->images[0].numTREs, 
	             &(isd->images[0].imageTREs));
      }
      TREBuffer.clear();

      // shift file pointer to next image header/data segment
      imageStartByte += vImageHdrLengths[index] + vImageLengths[index];

      delete pBuffer;
   }  //----- End of Image Loop

   if(imageIndex != -1)
      if(imageIndex < isd->numImages)
         isd->numImages = 1;
      else
         isd->numImages = 0;
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
void parseImages (NITF_2_1_ISD * isd, 
                  FILE *ifile,
		  const int HeaderLength, 
		  const std::vector <int> vImageHdrLengths, 
		  const std::vector <int> vImageLengths,
                  const int imageIndex)
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
   int imageStartByte = HeaderLength;         //----- Image Starting Index
   std::string UHD;                           //----- TRE Data Segment
   std::string XHD;                           //----- TRE Data Segment
   std::string TREBuffer;                     //----- TRE Data Segment
   int byteCount;                             //----- Current Byte position

   if(imageIndex == -1)
      isd->images = new image[isd->numImages];
   else
      isd->images = new image[1];

   for (index = 0; index < isd->numImages; index ++)
   {
      byteCount = ICORDS_OFFSET + 1;

      if(nitfDebugFlag)
         std::cout << "Image Segment staring at " << imageStartByte
                   << std::endl;
      pBuffer = getSegment(ifile,
                        imageStartByte,
                        vImageHdrLengths[index] + vImageLengths[index]);

      // copy image header including image tre's
      if(imageIndex == -1)
         isd->images[index].imageSubHeader.assign(pBuffer,
             vImageHdrLengths[index]);
      else if(imageIndex == index)
         isd->images[0].imageSubHeader.assign(pBuffer,
             vImageHdrLengths[index]);

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

      if(imageIndex == -1)
         isd->images[index].numTREs = 0;
      else if(imageIndex == index)
         isd->images[0].numTREs = 0;

      UHD.clear();
      XHD.clear();

      if (UDIDL > 0)
      {
	 UDOFL = GetBufferInt(pBuffer, byteCount, 3);
         if(nitfDebugFlag)
            DisplayValue("UDOFL", UDOFL, byteCount + imageStartByte, 3);

         if(nitfDebugFlag)
            std::cout << "Image TRE data starts at "
                      << byteCount + imageStartByte
                      << " bytes." << std::endl;
	 UHD.assign(&pBuffer[byteCount + 3], UDIDL - 3);

         byteCount += UDIDL;
      }

      // Added Readeing of Extended Data Length
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
            std::cout << "Image TRE data starts at "
                      << byteCount + imageStartByte
                      << " bytes." << std::endl;
	 XHD.assign(&pBuffer[byteCount + 3], IXSHDL - 3);
         byteCount += IXSHDL;
      }
      
      TREBuffer = UHD + XHD;
      if(TREBuffer.length() > 0)
      {
         if(imageIndex == -1)
	    parseTRE(TREBuffer.length(), TREBuffer, 
		     isd->images[index].numTREs, 
		     &isd->images[index].imageTREs);
         else
	    parseTRE(TREBuffer.length(), TREBuffer, 
		     isd->images[0].numTREs, 
		     &isd->images[0].imageTREs);
      }

      TREBuffer.clear();

      // shift file pointer to next image header/data segment
      imageStartByte += vImageHdrLengths[index] + vImageLengths[index];

      delete pBuffer;
   } // end of image loop

   if(imageIndex != -1)
      if(imageIndex < isd->numImages)
         isd->numImages = 1;
      else
         isd->numImages = 0;
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
//  Function: parseTRE
//
//  Description: parse a TRE segment
//
//  Inputs: TRElength  - Length of the TRE segment
//          DataBuffer - The TRE data
//          TREcount   - Number of TRE to be parsed
//          pTREData   - The TRE data segment
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void parseTRE(int TRElength,
	      std::string DataBuffer,
	      int &TREcount,
	      tre **pTREData)
   throw (TSMError)
{
   int parseLength = 0;               //----- Start of TRE
   std::vector<int> vTRELengths(100); //----- vector of TRE lengths
   char* pTREsegment;                 //----- Name of the TRE
   int TREsize;                       //----- Lenth of TRE data
   int TREHeaderLength = 6 + 5;       //----- tre header length =
                                      //-----       6 label chars + 5 len chars
   int numTRE = 0;                    //----- Number of TRE's
   int i;                             //----- Loop Variable

   TREcount = 0;

   while (parseLength < TRElength)
   {
      TREsize = atoi(DataBuffer.substr(parseLength + 6, 5).c_str());
      vTRELengths[numTRE] = TREsize + TREHeaderLength;
      parseLength += vTRELengths[numTRE];
      numTRE++;
   }

   *pTREData = new tre[numTRE];

   parseLength = 0;
   TREcount = numTRE;
   for (i = 0; i < numTRE; i++)
   {
      pTREsegment = new char[ vTRELengths[i] ];
      DataBuffer.copy(pTREsegment, vTRELengths[i], parseLength);
      (*pTREData)[i].setTRE(pTREsegment);

      if(nitfDebugFlag)
      {
         std::cout << "TRE (" << DataBuffer.substr(parseLength, 6)
                   << ") is at " << parseLength << " and has a length of "
                   << DataBuffer.substr(parseLength + 6, 5) << " bytes"
                   << std::endl;
      }

      parseLength += vTRELengths[i];

      delete pTREsegment;
   }
};

///////////////////////////////////////////////////////////////
//
//  Function: parseDES
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
void parseDES( NITF_2_0_ISD * isd, 
               FILE *pFile,
	       int offset,
	       const std::vector<int> hdrlengths, 
	       const std::vector<int> datalengths)
{
   isd->fileDESs = new des[isd->numDESs];
   char* pHeader = NULL;
   char* pData = NULL;

   for (int index = 0; index < isd->numDESs; index++)
   {
      pHeader = getSegment(pFile, offset, hdrlengths[index]);
      offset  += hdrlengths[index];
      pData   = getSegment(pFile, offset, datalengths[index]);
      offset  += datalengths[index];

      isd->fileDESs[index].setDES(hdrlengths[index], pHeader, 
				  datalengths[index], pData);

      delete pHeader;
      delete pData;
   }
}

///////////////////////////////////////////////////////////////
//
//  Function: parseDES
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
void parseDES(NITF_2_1_ISD *isd, 
              FILE *pFile,
	      int offset, 
	      const std::vector<int> hdrlengths, 
	      const std::vector<int> datalengths)
{
   parseDES((NITF_2_0_ISD *)isd, pFile, offset, hdrlengths, datalengths);
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
                 const int offset,
		 const int bufferSize)  throw (TSMError)
{
   char* buff = NULL;
   off_t bytesRead;                   //----- Number of Bytes read
   
   buff = new char[bufferSize];
   if (!buff)
      ; // handle error

   fseek (pFile, (long) offset, SEEK_SET);
   bytesRead = fread (buff, 1, bufferSize, pFile);

   if (bytesRead != bufferSize)
   {
      delete buff;
      std::string errstr( "failure reading input file ");

      TSMError tsmerr(TSMError::FILE_READ, 
                      errstr, 
                      "getSegment");
      throw (tsmerr);
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
   int i;                              //----- Loop Variable
   char label[100];                    //----- Label name string

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
void dumpHdr(NITF_2_0_ISD *isd)
{
   int i = 0;                        //----- Loop Variables

   dumpNitf20Hdr(isd->fileHeader);

   std::cout << "***************** END OF FILE HEADER *************\n";

   std::cout << isd->numTREs << " fileTRE records\n";
   for (i = 0; i < isd->numTREs; i++)
   {
      if ( isd->fileTREs != NULL)
      {
	 //std::cout << "\trecord " << isd->fileTREs[i].record << std::endl;
	 std::cout << "\trecord |" << isd->fileTREs[i].record << std::endl;
	 printchar(isd->fileTREs[i].record, isd->fileTREs[i].length);
	 std::cout << "|" << std::endl;
	 std::cout << "\tname   " << isd->fileTREs[i].name << std::endl;
	 std::cout << "\tlength " << isd->fileTREs[i].length << "\n\n";
      }
   }

   std::cout << isd->numImages << " image(s)\n";
   for (i = 0; i < isd->numImages; i++)
   {
      std::cout << "image sub-header for image " << i + 1 << std::endl;
      dumpImg20Hdr(isd->images[i].imageSubHeader);
      //std::cout << isd->images[i].imageSubHeader << std::endl;
      std::cout << "***************** END OF IMAGE HEADER *************\n";
      std::cout  << isd->images[i].numTREs << " TREs in image " 
	         << i + 1 << std::endl;
      for (int j = 0; j < isd->images[i].numTREs; j++)
      {
         if (isd->images[i].imageTREs != NULL)
         {
	    std::cout << "\tname   " << isd->images[i].imageTREs[j].name 
		      << std::endl;
	    std::cout << "\tlength " 
		      << isd->images[i].imageTREs[j].length << std::endl;
	    std::cout << "\trecord |";
	    printchar(isd->images[i].imageTREs[j].record,
                      isd->images[i].imageTREs[j].length);
	    std::cout << "|**\n";
	    std::cout << "\trecordlen " 
		      << strlen(isd->images[i].imageTREs[j].record)
		      << "\n\n";
         }
      }
   }

   std::cout << isd->numDESs << " DES(s)\n";
   for (i = 0; i < isd->numDESs; i ++)
   {
      std::cout << "\n\tDES hdr " << i + 1 << std::endl;
      std::cout << "\theader length " << isd->fileDESs[i].desShLength
                << std::endl;
      std::cout << "\theader: |" << isd->fileDESs[i].desSh << "|\n\n";
      std::cout << "\tdata length " << isd->fileDESs[i].desDataLength
                << std::endl;
      //std::cout << "\tdata: " << isd->fileDESs[i].desData << "\n\n";
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
void dumpHdr(NITF_2_1_ISD *isd)
{
   int i = 0;                          //----- Loop Variable

   dumpNitf21Hdr(isd->fileHeader);

   std::cout << "***************** END OF FILE HEADER *************\n";
   std::cout << isd->numTREs << " fileTRE records\n";
   for (i = 0; i < isd->numTREs; i++)
   {
      if ( isd->fileTREs != NULL)
      {
	 //std::cout << "\trecord " << isd->fileTREs[i].record << std::endl;
	 std::cout << "\trecord |";
	 printchar(isd->fileTREs[i].record, isd->fileTREs[i].length);
	 std::cout << "|" << std::endl;
	 std::cout << "\tname   " << isd->fileTREs[i].name << std::endl;
	 std::cout << "\tlength " << isd->fileTREs[i].length << "\n\n";
      }
   }

   std::cout << isd->numImages << " image(s)\n";
   for (i = 0; i < isd->numImages; i++)
   {
      std::cout << "image sub-header for image " << i + 1 << std::endl;
      dumpImg21Hdr(isd->images[i].imageSubHeader);
      //std::cout << isd->images[i].imageSubHeader << std::endl;
      std::cout << "***************** END OF IMAGE HEADER *************\n";
      std::cout  << isd->images[i].numTREs << " TREs in image " 
	         << i + 1 << std::endl;
      for (int j = 0; j < isd->images[i].numTREs; j++)
      {
         if (isd->images[i].imageTREs != NULL)
         {
	    std::cout << "\tname   " << isd->images[i].imageTREs[j].name 
		      << std::endl;
	    std::cout << "\tlength " 
		      << isd->images[i].imageTREs[j].length << std::endl;
	    std::cout << "\trecord |";
	    printchar(isd->images[i].imageTREs[j].record,
                      isd->images[i].imageTREs[j].length);
	    std::cout << "|**\n";
	    std::cout << "\trecordlen " 
		      << strlen(isd->images[i].imageTREs[j].record)
		      << "\n\n";
         }
      }
   }

   std::cout << isd->numDESs << " DES(s)\n";
   for (i = 0; i < isd->numDESs; i ++)
   {
      std::cout << "\n\tDES hdr " << i + 1 << std::endl;
      std::cout << "\theader length " << isd->fileDESs[i].desShLength
                << std::endl;
      std::cout << "\theader: |" << isd->fileDESs[i].desSh << "|\n\n";
      std::cout << "\tdata length " << isd->fileDESs[i].desDataLength
                << std::endl;
      //std::cout << "\tdata: " << isd->fileDESs[i].desData << "\n\n";
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
void writeStateFile(std::string fname, std::string state) throw (TSMError)
{
   TSMError tsmerr;
   int buffsize;
   FILE* ofile = fopen (fname.c_str(), "w");
   if (! ofile)
   {
	  std::string errstr ("Unable to open output file: (" + fname + ")");
	  std::cerr << errstr << std::endl;
      tsmerr.setTSMError (TSMError::FILE_WRITE, 
			  errstr, 
			  "writeStateFile");
      throw (tsmerr);
   }

   // open ok
   buffsize = state.length();
   off_t size = fwrite(state.c_str(), 1, buffsize, ofile);
   if (size != buffsize)
   {
      std::string errstr ("failure writing output file " + fname);
	  std::cerr << errstr << std::endl;
      tsmerr.setTSMError (TSMError::FILE_WRITE, 
			  errstr, 
			  "writeStateFile");
      throw (tsmerr);
   }
   fclose(ofile);
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
std::string readStateFile(std::string fname) throw (TSMError)
{
   TSMError tsmerr;
   FILE *ifile = NULL;
   int buffsize;
   long byte_size_of_file;
   char* buff;
   std::string state_from_file;
   ifile = fopen (fname.c_str(), "rb");
   if (! ifile)
   {
      std::string errstr ("Unable to open input file: (" + fname + ")");
	  std::cerr << errstr << std::endl;
      tsmerr.setTSMError (TSMError::FILE_READ, 
			  errstr, 
			  "readStateFile");
      throw (tsmerr);
   }

   fseek(ifile, 0, SEEK_END);
   byte_size_of_file = ftell(ifile);
   fseek(ifile, 0, SEEK_SET);
   buffsize = (int)(byte_size_of_file);
   buff = (char*) calloc (buffsize, sizeof (char));
   if (buff == NULL)
   {
      tsmerr.setTSMError (TSMError::MEMORY, 
			  "Unable to allocate file buffer", 
			  "readStateFile");
      throw (tsmerr);
   }

   // open ok
   off_t size = fread (buff, 1, buffsize, ifile);
   if (size != buffsize)
   {
      std::string errstr ("failure reading input file " + fname);
      std::string state = std::string(buff);
	  std::cerr << errstr << " - size read = " << size 
		        << ", size expected = " << buffsize
		        << ", state.length = (" << state.length() << ")"
				<< std::endl;
      tsmerr.setTSMError (TSMError::FILE_READ, 
			  errstr, 
			  "readStateFile");
      throw (tsmerr);
   }

   state_from_file = std::string(buff, buffsize);
   delete buff;
   fclose(ifile);
   return state_from_file;
};

