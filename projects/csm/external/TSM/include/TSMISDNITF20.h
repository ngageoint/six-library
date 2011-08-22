//#############################################################################
//
//    FILENAME:   tsm_ISDNITF20.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the NITF 2.0 ISD class derived from the tsm_ISD base class. 
//    ISD is encapsulated in a C++ class for transfer through the TSM 
//    interface. ISD is passed as a pointer to a simple ISD base class 
//    (for example, tsm_ISD *isd).
//    
//    LIMITATIONS:       None
// 
//                       Date          Author   Comment    
//    SOFTWARE HISTORY:  01-Jul-2003   LMT      Initial version.
//                       06-Feb-2004   KRW      Incorporated changes approved by 
//                                               January and February configuration
//                                               control board.
//                       01-Oct-2004   KRW      October 2004 CCB
//    NOTES:
//
//#############################################################################
#ifndef __tsm_ISDNITF20_H
#define __tsm_ISDNITF20_H

#include "TSMImageSupportData.h"
#include "TSMMisc.h"

class TSM_EXPORT_API des
{
  public:

   des()  
      { 
	 desShLength   = 0; 
	 desSh         = NULL; 
	 desDataLength = 0; 
	 desData       = NULL; 
      }

   ~des() 
      { 
	 clear();
      }

   void setDES
      ( 
	 int   des_sh_length,
	 char *des_sh,
	 int   des_data_length,
	 char *des_data )
      {
	 int i;
            
	 clear();

	 desShLength = des_sh_length;
	 desSh       = new char[desShLength+1];

	 for( i = 0; i < desShLength; i++ )
	    desSh[i] = des_sh[i];

	 desSh[desShLength] = '\0'; // in case NULL termination is needed

	 desDataLength = des_data_length;
	 desData       = new char[desDataLength+1];

	 for( i = 0; i < desDataLength; i++ )
	    desData[i] = des_data[i];

	 desData[desDataLength] = '\0';  // in case NULL termination is needed
      }

   void clear() 
      { 
	 delete [] desSh; 
	 delete [] desData; 
	 desShLength   = 0; 
	 desDataLength = 0; 
      }

   int    desShLength;
   char  *desSh;
   long   desDataLength;
   char  *desData;
};

 
class TSM_EXPORT_API tre
{
  public:
   tre()  { record = NULL; length = 0; name[0] = '\0'; }
   ~tre() { delete [] record; }

   void setTRE( char *tre)  // tre includes TRE name, length and data
      {
	 int i;
	 char lengthString[6];
      
	 clear();
	 for( i = 0; i < 6; i++ )
	    name[i] = tre[i];

	 // in case, NULL termination is needed
	 name[6]     = '\0';

	 for( i = 6; i < 11; i++ )
	    lengthString[i-6] = tre[i];

	 // in case, NULL termination is needed
	 lengthString[5]     = '\0';
      
      
	 length = atoi(lengthString);
      
	 record = new char[length+1];
	 for( i = 11; i < length+11; i++ )
	    record[i-11] = tre[i];

	 // in case, NULL termination is needed
	 record[length] = '\0';
      }

   void clear() 
      { delete [] record; length = 0; name[0] = '\0'; }

   char *record;
   char  name[7];
   int   length;
};


class TSM_EXPORT_API image
{
  public:
   image() { numTREs = 0; imageTREs = NULL; }
   ~image() { delete [] imageTREs; }

   std::string  imageSubHeader;
   tre    *imageTREs;
   int     numTREs;
};


class TSM_EXPORT_API NITF_2_0_ISD : public tsm_ISD
{
  public:
   NITF_2_0_ISD() 
      { _format = "NITF2.0"; numTREs = 0; numImages = 0; 
      fileTREs = NULL; images = NULL; numDESs = 0; fileDESs = NULL; }
   ~NITF_2_0_ISD() 
      { delete [] images; delete [] fileTREs; delete[] fileDESs; }

   std::string  fileHeader;
   int     numTREs;
   tre    *fileTREs;
   int     numDESs;
   des     *fileDESs;
   int     numImages;
   image  *images;
};

#endif
