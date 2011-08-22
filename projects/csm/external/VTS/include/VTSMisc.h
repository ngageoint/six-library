//#############################################################################
//
//    FILENAME:   VTSMisc.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the constants and other definitions used by the TSM VTS.
//
//    LIMITATIONS:       None
// 
//                       Date          Author      Comment    
//    SOFTWARE HISTORY:  13-Jan-2004   LMT         Initial version.
//                       22 Jul 2010   Don Leonard CCB Change add writeStateFile and readStateFile
//
//    NOTES:
//
//#############################################################################
#ifndef __VTSMISC_H
#define __VTSMISC_H
#ifdef _WIN32
#pragma warning( disable : 4290 )
#endif
#include "TSMMisc.h"
#include "TSMError.h"
#include "TSMWarning.h"
#include "TSMISDFilename.h"
#include "TSMISDByteStream.h"
#include "TSMISDNITF20.h"
#include "TSMISDNITF21.h"
#include "constants.h"

#include <vector>
#include <list>
#include <string>

class TSMPlugin;
class TargetPlugin;

#define MAXINPUTPARAMETERS	50
#define MAXOUTPUTPARAMETERS	18

extern std::list<std::string> commandQueue;
extern std::vector<std::string> menulist;
extern std::vector<MENU_TYPE> menutype;
extern bool debugFlag;
extern bool compareDebugFlag;
extern bool nitfDebugFlag;


class SegmentlengthInfo
{
 public:
  int seghdrlength;
  int segdatalength;
};
    
enum ISDType    
  {  NO_FILE_TYPE,
     NITF_20_TYPE,
     NITF_21_TYPE,
     BYTESTREAM_TYPE,
     FILENAME_TYPE
  }; 

typedef std::list < std::string >      NameList;

void initMenuList();
void trim(std::string& str);

TSMWarning * printList(const char *logFile);
// pre: None.
// post: The list of all currently-registered factories has been printed.
TSMWarning * printModelList(std::string pluginName);

TSMWarning * makeModelListFromISD(const std::string isd_name, const tsm_ISD * isd)
     throw (TSMError);
// pre: None.
// post: A subset of aList has been printed; the subset contains all
//    factories that can support the given isd.

TSMWarning * makeTargetListFromISD(const std::string isd_name, const tsm_ISD * isd)
     throw (TSMError);

TSMWarning * listSupportingModels(const std::string isd_name,
				  const tsm_ISD* isd,
				  NameList& names);
// pre: None.
// post: For each model currently registered, the model's name is
//    appended to aNameList if the model supports the given isd.

TSMWarning * listSupportingTargets(const std::string isd_name,
				  const tsm_ISD* isd,
				  NameList& names);

/*
void compareParam(std::vector<std::string> param_array,
		  char act_val[MAX_ACT][MAX_ACT_SIZE],
		  bool *pass_fail);

void compareParam(std::vector<std::string> param_array,
		  double *act_val,
		  double *comp_answer,
		  bool *pass_fail);
*/
void compareParam(std::vector<std::string> param_array,
		  std::vector<std::string> act_val,
		  std::vector<bool> &pass_fail);

void compareParam(std::vector<std::string> param_array,
		  std::vector<double> act_val,
		  std::vector<double> &comp_answer,
		  std::vector<bool> &pass_fail);

// recordLog II
void recordLog(std::string datafile,
               char* serialNum,
               char* timedata,
               std::string comment,
               std::string command,         
	       std::vector<std::string> param_array,
               std::vector<std::string> out_param_array);
//               char out_param_array[MAXOUTPUTPARAMETERS*3][MAX_OUTPARAM_LENGTH]);

// recordLog IV
void recordLog(std::string datafile,
               char* serialNum,
               char* timedata,
               std::string comment,
               std::string command,
               std::string text);

// recordLog VI
void recordLog(std::string datafile,
               char* serialNum,
               char* timedata,
               std::string comment,
               std::string command,
               std::vector<std::string> param_array,
               std::string text);

// recordLog VIII
void recordLog(std::string datafile,
               std::string  comment,
               std::string  text);

//recordLog X
void recordLog(std::string datafile,
               const std::string text);


void getLogfileName(const std::string dirName, std::string *name);

void reportWarning(TSMWarning* warning, const char* logFile);
void reportError (TSMError *err, const char *logFile);

// prototypes for ISD "constructors"
void initFilenameISD (filenameISD *isdfilename, 
		      std::string isd);

void initBytestreamISD(bytestreamISD *bytestream, std::string filename)  
     throw (TSMError);

TSMWarning *initNITF20ISD(NITF_2_0_ISD *isd,
                          std::string fname,
                          const int imageIndex = -1)
     throw (TSMError);

TSMWarning *initNITF21ISD(NITF_2_1_ISD *isd,
                          std::string fname,
                          const int imageIndex = -1)
     throw (TSMError);

FILE * fillBuff (std::string fname, struct stat &statbuf, char **buff) throw (TSMError);

std::string readStateFile( std::string fname) throw (TSMError);
void writeStateFile(std::string fname, std::string state) throw (TSMError);

TSMWarning *parseFile(NITF_2_0_ISD *isd,
                      FILE *ifile,
                      struct stat &statbuf,
                      char* buff,
                      const int imageIndex) throw (TSMError);

TSMWarning *parseFile(NITF_2_1_ISD *isd,
                      FILE *ifile,
                      struct stat &statbuf,
                      char* buff,
                      const int imageIndex) throw (TSMError);

void parseImages (NITF_2_0_ISD *isd, 
                  FILE *ifile,
		  const int hl, 
		  const std::vector <int> imagehdrlengths, 
		  const std::vector <int>imagelengths,
                  const int imageIndex);

void parseImages (NITF_2_1_ISD *isd, 
                  FILE *ifile,
		  const int hl, 
		  const std::vector <int> imagehdrlengths, 
		  const std::vector <int>imagelengths,
                  const int imageIndex);

void parseTRE(int tlen, // length of tre data
	      std::string treData,  //  input tre data
	      int &numTRE, // output numtres and file/image tre data
	      tre **Tre)   throw (TSMError);

void parseDES(NITF_2_0_ISD *isd, 
                  FILE *ifile,
		  int des_offset, 
		  const std::vector <int> hdrlengths, 
		  const std::vector <int>datalengths);

void parseDES(NITF_2_1_ISD *isd, 
                  FILE *ifile,
		  int des_offset, 
		  const std::vector <int> hdrlengths, 
	      const std::vector <int>datalengths);

char* getSegment( FILE *ifile,
                 const int offset,
		 const int bufferSize)  throw (TSMError);

void dumpHdr(NITF_2_0_ISD *isd);

void dumpHdr(NITF_2_1_ISD *isd);

int getCommandIndex(std::string command);
std::vector<std::string> loadHelpFile();
void set_keypress(void);
void reset_keypress(void);
std::string readDataLine();
std::vector<std::vector<std::string> > getCommands(std::string params);
#endif // VTSMISC_H

     
