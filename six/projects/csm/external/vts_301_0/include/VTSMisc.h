//#############################################################################
//
//    FILENAME:   VTSMisc.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the constants and other definitions used by the CSM VTS.
//
//    LIMITATIONS:       None
// 
//    SOFTWARE HISTORY:
//       Date       Author Comment    
//      13-Jan-2004  LMT   Initial version.
//      22 Jul 2010  DSL   CCB Change add writeStateFile and readStateFile
//      08 Feb 2011  DSL   Add capability to process large files (2G - 4G)
//      29 Jun 2011  DSL   Add thread testing 
//      29 Aug 2011  DSL   Handle large pointers to DES records at the end of large files 
//      10 Jan 2013  DSL   changes to reflect C++ reformat CRs
//      28 Jan 2013  DSL   VTS_VERSION CSM3.0.1.0
//
//#############################################################################
#ifndef __VTSMISC_H
#define __VTSMISC_H
#ifdef _WIN32
#pragma warning( disable : 4290 )
#endif
#include "csm.h"
#include "Error.h"
#include "Warning.h"
#include "BytestreamIsd.h"
#include "NitfIsd.h"
#include "constants.h"

#include <vector>
#include <list>
#include <string>

using namespace csm; 
using namespace std; 
//class Plugin;

#define MAXINPUTPARAMETERS      30
#define MAXOUTPUTPARAMETERS     200
#define TEMP_STRING_LENGTH 2048

#ifdef _WIN32
#define STATTYPE stat
#else
#define STATTYPE stat64
#endif

extern list<string> commandQueue;
extern vector<string> menulist;
extern vector<MENU_TYPE> menutype;
extern bool debugFlag;
extern bool compareDebugFlag;
extern bool nitfDebugFlag;
extern WarningList warnings;

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

string ISDType2str(ISDType type);

typedef list < string >      NameList;

void initMenuList();
void trim(string& str);

void printList(string logFile);
// pre: None.
// post: The list of all currently-registered factories has been printed.
void printModelList(string pluginName);

void makeModelListFromISD(const string isd_name, const Isd * isd, WarningList* warning = NULL);

// pre: None.
// post: A subset of aList has been printed; the subset contains all
//    factories that can support the given isd.

void listSupportingModels(const string isd_name,
				  const Isd* isd,
				  NameList& names,
				  WarningList* warnings);

void compareParam(vector<string> param_array,
		  vector<string> act_val,
		  vector<bool> &pass_fail);

void compareParam(vector<string> param_array,
		  map<int, double> act_val,
		  map<int, double> &comp_answer,
		  map<int, bool> &pass_fail);

void compareParam(vector<string> param_array,
		  double *act_val,
		  double *comp_answer,
		  bool *pass_fail);

void compareParam(vector<string> param_array,
		  vector<string> act_val,
		  map<int, bool> &pass_fail);

void compareParam(vector<string> param_array,
		  map<int, double> act_val,
		  map<int, double> &comp_answer,
		  map<int, bool> &pass_fail);

// recordLog I
void recordLog(string datafile,
               string comment,
               string command,         
               vector<string> param_array, 
               vector<string> out_param_array);

// recordLog II
void recordLog(string datafile,
			   string serialNum,
               string timedata,
               string comment,
               string command,         
               vector<string> param_array,
               vector<string> out_param_array);

// recordLog III
void recordLog(string datafile,
               string comment,
               string command,
               string text);

// recordLog IV
void recordLog(string datafile,
               string serialNum,
               string timedata,
               string comment,
               string command,
               vector<string> param_array,
               string text);

// recordLog V
void recordLog(string datafile,
               string comment,
               string text);

// recordLog VI
void recordLog(string datafile,
               const string text);

void echo2Term(string comment,
               string command,
               string text);

void getLogfileName(const string dirName, string *name);

void reportWarning(WarningList warnings, const char* logFile);
void reportError(Error* err, const char* logFile);

// prototypes for ISD "constructors"
//void initFilenameISD (Isd *isdfilename, 
//		      string isd);

void initBytestreamISD(BytestreamIsd *bytestream, string filename)  
     throw (Error);

void initNitf20ISD(Nitf20Isd *isd,
                   string fname,
                   const int imageIndex = -1,
				   WarningList* warnings = NULL);

void initNitf21ISD(Nitf21Isd *isd,
                   string fname,
                   const int imageIndex = -1,
				   WarningList* warnings = NULL);

FILE * fillBuff (string fname,
#ifdef _WIN32
                    struct stat &statbuf, 
#else
                    struct stat64 &statbuf,
#endif
                    char **buff) throw (Error);

string readStateFile( string fname) throw (Error);
void writeStateFile(string fname, string state) throw (Error);

void parseFile(Nitf20Isd *isd,
               FILE *ifile,
#ifdef _WIN32
               struct stat &statbuf,
#else
               struct stat64 &statbuf,
#endif
               char* buff,
               const int imageIndex,
		       WarningList* warnings);

void parseFile(Nitf21Isd *isd,
               FILE *ifile,
#ifdef _WIN32
               struct stat &statbuf,
#else
               struct stat64 &statbuf,
#endif
               char* buff,
               const int imageIndex,
		       WarningList* warnings);

void parseImages (Nitf20Isd *isd, 
                  FILE *ifile,
                  const int hl, 
                  const vector <size_t> imagehdrlengths, 
                  const vector <size_t>imagelengths,
                  const int imageIndex,
				  const int NUMI);

void parseImages (Nitf21Isd *isd, 
                  FILE *ifile,
                  const int hl, 
                  const vector <size_t> imagehdrlengths, 
                  const vector <size_t>imagelengths,
                  const int imageIndex,
				  const int NUMI);

vector<Tre> parseTre(int tlen, // length of tre data
	      string treData);  //  input tre data

void parseDes(Nitf20Isd *isd, 
              FILE *ifile,
              size_t des_offset, 
              const vector <size_t> hdrlengths, 
              const vector <size_t>datalengths,
			  const int NUMD);

void parseDes(Nitf21Isd *isd, 
              FILE *ifile,
              size_t des_offset, 
              const vector <size_t> hdrlengths, 
              const vector <size_t>datalengths,
			  const int NUMD);

char* getSegment( FILE *ifile,
                  const size_t offset,
		  const size_t bufferSize)  throw (Error);

void dumpHdr(Nitf20Isd *isd);

void dumpHdr(Nitf21Isd *isd);

int getCommandIndex(string command);
vector<string> loadHelpFile();
void set_keypress(void);
void reset_keypress(void);
string readDataLine();
vector<string> getCommands(string params);
#endif // VTSMISC_H

