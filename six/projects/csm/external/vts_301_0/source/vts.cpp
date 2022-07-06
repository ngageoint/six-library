//#############################################################################
//
//    FILENAME:   vts.cpp
//
//
//    DESCRIPTION:
//
//    This application is a test driver for the Community Sensor Model (CSM),
//    formerly Tactical Sensor Model (CSM) plugin manager, SMManager.
//
//    This version's primary function is to cause the CSM plugins to be loaded,
//    and then to verify that they have loaded. It does this by by accessing
//    the list of all plugins currently registered, and then printing that
//    list.
//
//    NOTES:
//
//    This version of the test application experiments with using a string to
//    select a plugin from those in the registered list. This portion of the
//    application and plugins is temporary; it does not necessarily represent
//    an actual method for sensor model selection in the real world.
//
//    SOFTWARE HISTORY:
//       Date       Author Comment    
//      13-Jan-2004  LMT   Initial version.
//      13 Aug 2010  DSL  CCB Change add vtsWriteStateToFile and vtsReadStateFromFile
//      22 Oct 2010  DSL  CCB Change add getCurrentCrossCovarianceMatrix 
//                                    and getOriginalCrossCovarianceMatrix
//      25 Oct 2010  DSL  CCB Change add getNumGeometricCorrectionSwitches,
//                                        getGeometricCorrectionName,
//                                        getGeometricCorrectionSwitch,
//                                    and setGeometricCorrectionSwitch
//      29 Jun 2011  DSL   Add thread testing 
//      10 Jan 2013  DSL    changes to reflect C++ reformat CRs
//      23 Jan 2013  DSL    Created macros: _TRY _CATCH. Changed all try/catch code to 
//                         use macros.
//                         Added _TRY/_CATCHs around many more of the calls to handle 
//                         thrown errors from the called function.
//                         Changed getAllSensorPartials(1&2) and getCrossCovarianceMatrix 
//                         to add versitility to vector sizes of return arguments.
//      26 Jan 2013  DSL    added commands: vtsAddIsdParam, vtsGetIsdParam
//      28 Jan 2013  DSL   VTS_VERSION CSM3.0.1.0
//#############################################################################

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <ctime>
#include <list>
#include <vector>

// includes for CSM classes -- this code is configured by a global
// committee -- any changes must be approved by that committe.

#include "Error.h"
#include "Warning.h"
#include "Plugin.h"
#include "RasterGM.h"
#include "FourParameterCorrelationModel.h"
#include "LinearDecayCorrelationModel.h"
#include "Isd.h"
#include "NitfIsd.h"
#include "BytestreamIsd.h"
#include "constants.h"
#include <stdlib.h>

// includes for VTS ancillary code
#include "VTSMisc.h"
#include "SMManager.h"

#define _TRY                                                    \
      try                                                       \
      {
#define _CATCH                                                  \
      }                                                         \
      catch (const Error& err)                                         \
      {                                                         \
         error_thrown = true;                                   \
         global_err = err;                                      \
      }                                                         \
      catch ( ... )                                             \
      {                                                         \
         cout << UNKNOWN_ERROR_STR << menulist[commandNumber] << endl; \
         error_thrown = true;                                   \
      }

using namespace std;
using namespace csm;

WarningList warnings;
string VTS_VERSION ="CSM3.0.1.0";

// global variables required by the two macros defined below
static long instructionCountSt = 0;
static clock_t start_clockSt = clock();  // Initialize the clock
long instructionCount;
clock_t start_clock;
static int repeat_flag = false;       // Repeat a command Flag
static int repeat_count = 1;          // The number of times to repeat

string testThreads(string configfile);

// a pair of 'macros' to factor out common code from switch cases
static inline void initCase(void)
{
   instructionCountSt++;                  // Advance the command serial number
   start_clockSt = clock();               // Record the start time for call.
   instructionCount = instructionCountSt;
   start_clock = start_clockSt;
};
static inline void exitCase(void)
{
   repeat_flag = false;                 // Reset the repeat command
   repeat_count = 1;                    // Clear the repeat count
};

clock_t end_clock;

string command;
string logFile;
vector<string> vHelpDataValues;
bool debugFlag = false;
int termEcho=0;  // The switch that determines what gets echoed to the terminal
//                   at the vtsPlugin and the vts command line.
//                   0= no output, 1= all output (same as what goes to
//                   the log file) and 2= just the returned values.

bool logCommand;                     // Use the default log command
bool logStringCommand;               // Use the default string log command

map<int, double>      act_val;
//vector<double>      act_val;
map<int, double>      comp_answer;
map<int, bool>        pass_fail;
vector<string> act_val_string;
int param_array_index;               // Index into param_array where the
int maxReturnValuesToCompare;        // Maximum number of numeric
string errorName ="";
string text;
ostringstream stream; 

bool saveLogFile = true;
string comment = "";

char tempStr[TEMP_STRING_LENGTH] = "";

// Indicates what type of ISD:
ISDType ISDFileType=NO_FILE_TYPE;
// NITF_20_TYPE,
// NITF_21_TYPE. BYTESTREAM_TYPE,
// FILENAME_TYPE or NO_FILE_TYPE.

Nitf20Isd *csmnitf20 = NULL;
Nitf21Isd *csmnitf21 = NULL;
BytestreamIsd *csmbytestream = NULL;
string bytestreamData;
string fname;

//filenameISD *csmfilename = NULL;
Isd *csmfilename = NULL;
string saved_state_file;
string smName;
string configfile;
bool oneSMPerThread;

Plugin* stubPlugin = NULL;
RasterGM* rasterGM = NULL;
RasterGM* SM2 = NULL;
Version retver(0,0);

//Warning *csmWarn = NULL;
Error global_err;
bool error_thrown = false;           // flag to indicate that an error
EcefCoord groundPt;
EcefCoord sensorPos;
EcefVector sensorVel;
EcefVector illumDir;
ImageCoord imagePt;
ImageCoord ic;
ImageCoordCovar icCov;
EcefCoordCovar groundPtCovar;
string imageId;
// flag to indicate that the ISD File has been read
bool ISDFileReadDone = false;
string returned_sensor_model_state = "";
// flag to indicate that the sensor model state has been saved
bool sensorModelStateSaved = false;
//FourParameterCorrelationModel fpcvm(1,1);
csm::CorrelationModel* cvm;
bool fpcvm = false;
bool ldcvm = false;
//LinearDecayCorrelationModel ldcvm(1,1);

///////////////////////////////////////////////////////
//
//   Function: processVTSCommand
//
//   Description: process the inputted command
//
//   Input: commandNumber - Enumeration of the inputted
//                          command
//          param_array   - Argument to the command
//
//   Return Value:
//          bool - Command Found
//
///////////////////////////////////////////////////////
bool processVTSCommand(int commandNumber,
                       vector<string> param_array)
{
   int i, j;                               //----- Loop Variable
   bool command_found = true;              //----- Command Found Flag
   int index = 1;                          // Number of values in param array

   string pluginName;

   PluginList pluginList;             // for getList
   PluginList::const_iterator sensorIter;

   command_found = true;
   switch (commandNumber)
   {
      //----- CASE # comment -- 0  NSetup
      case COMMENT:
         logCommand = false;            // do not log this command.

         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }
         exitCase();
         break;
         //----- CASE vtsSetLogFileName  NSetup
      case VTS_SET_LOG_FILENAME:
         logCommand = false;

         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

         if(param_array.size() != 0)  // Do not use the default file name
                                      // and location (./vts_logfile.csv)or .\\vts_logfile.csv
            logFile = param_array[0];

         if(debugFlag)
         {
            cout << "    Passing values are: " << endl;
            cout << "      logFile=" << logFile << endl;
         }

         param_array_index = 1000;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE vtsSetLogFile  NSetup
      case VTS_SET_LOG_FILE:
         //   This command globally turns file logging on/off
         logCommand = false;

         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

         if(param_array.size() != 0)
           //set save-to-logfile" flag, else just display the value
           saveLogFile = (atoi(param_array[0].c_str()) == 1);

         if(debugFlag)
         {
            cout << "    Passing values are: " << endl;
            cout << "      saveLogFile=" << saveLogFile << endl;
         }

         param_array_index = 1;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE vtsWriteCommentToLogFile NSetup
      case VTS_WRITE_COMMENT_TO_LOG_FILE:
         logCommand = false;

         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

         comment = param_array[0];
         text.clear();

         for (i = 1; i < (int)param_array.size() ; i++)
            text += (param_array[i] + string(" "));

         if(debugFlag)
         {
            cout << "    Passing values are: " << endl;
            cout << "      comment=" << comment << endl;
            cout << "      text=" << text << endl;
         }

         recordLog(logFile,
            comment,
            text);

         param_array_index = 1;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE vtsWriteTextToLogFile NSetup
      case VTS_WRITE_TEXT_TO_LOG_FILE:
         logCommand = false;

         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

         text.clear();
         for (i = 0; i < (int)param_array.size(); i++)
            text += (param_array[i] + ' ');

         if(debugFlag)
         {
            cout << "    Passing values are: " << endl;
            cout << "      text=" << text << endl;
         }

         recordLog(logFile, text);

         param_array_index = 1;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE vtsRepeat Set repeat count NSetup
      case VTS_REPEAT:
         logCommand = false;
         initCase();

         if(debugFlag)
         {
            cout
               << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }
         // save the number of repititions.
         repeat_count = atoi(param_array[0].c_str());

         if ( repeat_count <= 1)
         {
            cout << "      Incorrect vtsRepeat value." << endl;
            repeat_count = 1;
         }
         else
         {
            repeat_flag = true;
         }
         end_clock = clock();

         if(debugFlag)
         {
            cout << "    Passing values are: " << endl;
            cout << "      repeat_count=" << repeat_count << endl;
         }

         comment.clear();
         sprintf (tempStr, "%u", repeat_count);
         text = tempStr;

         recordLog(logFile,
             comment,
             command,
             text);

         param_array_index = 1;
         maxReturnValuesToCompare = 0;

         break;

         //----- CASE vtsTermEcho NSetup
      case VTS_TERM_ECHO:
         // The switch that determines what gets echoed to the terminal
         // at the vtsPlugin and the vts command line.
         // 0= no output, 1= all output (same as what goes to
         // the log file) and 2= just the returned values(default).

         logCommand = false;
         initCase();

         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

         termEcho = atoi(param_array[0].c_str());
         if ( termEcho < 0 || termEcho > 2)
         {
            cout << "      Incorrect termEcho value." << endl;
            cout << "      Value of termEcho=" << termEcho
               << endl;
            termEcho = 2;
         }
         end_clock = clock();

         if(debugFlag)
         {
            cout << "    Passing values are: " << endl;
            cout << "      termEcho=" << termEcho << endl;
         }

         comment.clear();
         sprintf (tempStr, "%u", termEcho);
         text = tempStr;

         recordLog(logFile,
             comment,
             command,
             text);

         param_array_index = 1;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE vtsToggleDebug NSetup
      case VTS_TOGGLE_DEBUG:
         logCommand = false;
         debugFlag = !debugFlag;

         if(debugFlag)
            cout << "Debugging has been enabled." << endl;
         else
            cout << "Debugging has been disabled." << endl;
         break;

         //----- CASE vtsCompareToggleDebug NSetup
      case VTS_TOGGLE_COMPARE_DEBUG:
         logCommand = false;
         compareDebugFlag = !compareDebugFlag;

         if(compareDebugFlag)
            cout << "Compare debugging has been enabeled." << endl;
         else
            cout << "Compare debugging has been disabeled." << endl;
         break;

         //----- CASE vtsNitfToggleDebug NSetup
      case VTS_TOGGLE_NITF_DEBUG:
         logCommand = false;
         nitfDebugFlag = !nitfDebugFlag;

         if(nitfDebugFlag)
            cout << "NITF debugging has been enabeled." << endl;
         else
            cout << "NITF debugging has been disabeled." << endl;
         break;

         //----- CASE printList NSetup
      case PRINT_LIST:
         logCommand = false;
         initCase();
		 
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

         printList(logFile);
         end_clock = clock();           // Record the end time for call.

         comment.clear();
         text.clear();

         recordLog(logFile,
             comment,
             command,
             text);
         exitCase();
         break;

         //----- CASE printModelList NSetup
      case PRINT_MODEL_LIST:
         logCommand = false;
         initCase();
		 
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

         if(param_array.size() > 0)
            printModelList(param_array[0]);
         end_clock = clock();           // Record the end time for call.

         comment.clear();
         text.clear();

         recordLog(logFile,
             comment,
             command,
             text);
         exitCase();
         break;

         //----- CASE status NSetup
      case STATUS:
         logCommand = false;
         initCase();
		 
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

         if(csmnitf20)
         {
            cout << "An NITF 2.0 file is currently loaded." << endl;
         }
         if(csmnitf21)
         {
            cout << "An NITF 2.1 file is currently loaded." << endl;
         }
         if(csmfilename)
         {
            cout << "A 'Filename ISD' file is currently loaded."
                      << endl;
         }
         if(csmbytestream)
         {
            cout << "A 'byte stream' file is currently available."
                      << endl;
         }

         if(stubPlugin)
         {
            _TRY
               pluginName = (*sensorIter)->getPluginName();
            _CATCH
            cout  << endl << "Sensor plugin "
                      << pluginName << " is currently loaded." << endl;
         }
         if(rasterGM)
         {
            cout  << "A sensor model is currently loaded." << endl;
            _TRY
               smName = rasterGM->getModelName();
            _CATCH
         }

         end_clock = clock();           // Record the end time for call.

         comment.clear();
         text.clear();

         recordLog(logFile,
               comment,
               command,
               text);
         exitCase();
         break;

	  //----- CASE vtsWriteStateToFile
      case VTS_WRITE_STATE_TO_FILE:

         logCommand = false;
         initCase();
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

         saved_state_file = param_array[0];
         _TRY
      		 writeStateFile(saved_state_file, returned_sensor_model_state);
         _CATCH
         if(error_thrown)
         {
            break;
         }
         else
         {
			char tbuf[100];
			sprintf (tbuf, "%u", returned_sensor_model_state.length());

            text =  " Sensor Model State successfully written to file " 
                      + saved_state_file + " (" 
                      + tbuf + " bytes)\n"; 
            if(debugFlag)
            {
               cout << text;
            }
         }
         end_clock = clock();           // Record the end time for call.
         comment.clear();
         recordLog(logFile,
             comment,
             command,
             text);
         exitCase();
         break;

		 //----- CASE vtsReadStateFromFile
      case VTS_READ_STATE_FROM_FILE:
         logCommand = false;
         initCase();
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }
         saved_state_file = param_array[0];
         _TRY
      		 returned_sensor_model_state = readStateFile(saved_state_file);
         _CATCH
         if(error_thrown)
         {
            break;
         }
         else
         {
			char tbuf[100];
			sprintf (tbuf, "%u", returned_sensor_model_state.length());

            text =  " Sensor Model State successfully read from file " 
                      + saved_state_file + " (" 
                      + tbuf + " bytes)\n"; 
            if(debugFlag)
            {
               cout << text;
            }
         }
		 
		 end_clock = clock();           // Record the end time for call.
         comment.clear();
         recordLog(logFile,
               comment,
               command,
               text);
         exitCase();
         break;

         //----- CASE vtsRead20ISDFile NSetup
      case VTS_READ_20ISD_FILE:
         logCommand = false;
         initCase();
		 
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber]
               << endl;
         }

         //----- get the file and fill the object
         _TRY
            csmnitf20 = new Nitf20Isd(param_array[0].c_str());
            if(param_array.size() > 1)
			{
               initNitf20ISD(csmnitf20,
                     param_array[0],
                     atoi(param_array[1].c_str()) - 1,
					 &warnings);
			}
			else
			{
               initNitf20ISD(csmnitf20,
                     param_array[0],
					 -1,
					 &warnings);
			}
         _CATCH

         end_clock = clock();

         if (error_thrown)
         {
            ISDFileReadDone = false;
         }
         else
         {
            if(debugFlag)
            {
               cout << "    Passing values are: " << endl;
               cout << "      ISDFile=" << param_array[0]
                         << endl;
               if(param_array.size() > 1)
                  cout << "  Image Index=" << param_array[1]
                            << endl;
            }

            // Add error check TBD

            ISDFileType = NITF_20_TYPE;
            ISDFileReadDone = true;
         }

         comment.clear();
         recordLog(logFile,
               comment,
               command,
               param_array[0].c_str());
         exitCase();
         break;

         //----- CASE vtsRead21ISDFile NSetup
      case VTS_READ_21ISD_FILE:
         logCommand = false;
         initCase();
		 
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

         // get the file and fill the object
         _TRY
            csmnitf21 = new Nitf21Isd(param_array[0].c_str());
            if(param_array.size() > 1)
			{
               initNitf21ISD(csmnitf21,
                             param_array[0],
                             atoi(param_array[1].c_str()) - 1,
							 &warnings);
			}
            else
			{
               initNitf21ISD(csmnitf21,
                             param_array[0],
							 -1,
							 &warnings);
			}
         _CATCH

         end_clock = clock();

         if (error_thrown)
         {
            ISDFileReadDone = false;
         }
         else
         {
            if(debugFlag)
            {
               cout << "    Passing values are: " << endl;
               cout << "      ISDFile=" << param_array[0]
                         << endl;
               if(param_array.size() > 1)
                  cout << "  Image Index=" << param_array[1]
                            << endl;
            }

            ISDFileType = NITF_21_TYPE;
            ISDFileReadDone = true;
         }

         comment.clear();

         recordLog(logFile,
               comment,
               command,
               param_array[0].c_str());
         exitCase();
         break;

         //----- CASE vtsReadByteStream NSetup
      case VTS_READ_BYTESTREAM:
         logCommand = false;
         initCase();
		 
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

		 fname = param_array[0];
		 csmbytestream = new BytestreamIsd(bytestreamData, fname);
         _TRY
            initBytestreamISD (csmbytestream, fname);
         _CATCH
         end_clock = clock();

         if (error_thrown || (csmbytestream && (csmbytestream->data().length() == 0) ) )
         {
            cout << "Unable to construct CsmByteStream object\n";
            ISDFileReadDone = false;
         }
         else
         {
            if(debugFlag)
            {
               cout << "    Passing values are: " << endl;
               cout << "      ISDFile=" << param_array[0]
                         << endl;
            }

            // Add error check TBD
            ISDFileType = BYTESTREAM_TYPE;
            ISDFileReadDone = true;
         }

         comment.clear();
         text = param_array[0].c_str();

         recordLog(logFile,
               comment,
               command,
               text);
         exitCase();
         break;

		 //----- CASE vtsGetIsdParam
      case VTS_GET_ISD_PARAM:
      {
         logCommand = true;
         initCase();
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
                 << menulist[commandNumber] << endl;
            cout << "    Passing values are: " << endl;
            cout << "      param name = " << param_array[0] << endl 
                 << "      instance = " << param_array[1]   << endl;
         }
         if( !ISDFileReadDone )
         {
            cout << "  vts - vtsReadISDFile has not been run yet! " << endl;
			break;
         }
		 string pvalue = "";
		 string pname = param_array[0];
		 int instance = atoi(param_array[1].c_str());
         if(  pname != IMAGE_ID_PARAM &&      pname != IMAGE_INDEX_PARAM
           && pname != LOGICAL_INDEX_PARAM && pname != MODEL_NAME_PARAM)
		 {
            warnings.push_back(Warning(Warning::UNKNOWN_WARNING,
               "Invalid ISD Param name", "vts"));
            cout << "  vts - Invalid SD Param name!\n";
			break;
         }

         _TRY
            switch (ISDFileType)
            {
               case NITF_20_TYPE:
                  pvalue = csmnitf20->param(pname, instance);
                  break;

               case NITF_21_TYPE:
                  pvalue = csmnitf21->param(pname,instance);
                  break;

               case BYTESTREAM_TYPE:
                  pvalue = csmbytestream->param(pname,instance);
                  break;

               case FILENAME_TYPE:
                  pvalue = csmfilename->param(pname,instance);
                  break;

               default:
                  warnings.push_back(Warning(Warning::UNKNOWN_WARNING,
                                             "Invalid ISD type", "vts"));
                  cout << "  vts - ISD Type is unknown!\n";
            }                     //  end  switch
         _CATCH
         end_clock = clock();

         if (error_thrown)
         {
            cout << "Unable to get ISD Param\n";
            break;
         }
         if(debugFlag)
         {
            cout << "    Return values are: " << endl;
            cout << "      param value = " << pvalue << endl;
         }

         comment.clear();
         text = pvalue;
         recordLog(logFile,
               comment,
               command,
               text);
         exitCase();
         break;
      }

         //----- CASE vtsAddIsdParam
      case VTS_ADD_ISD_PARAM:
      {
         logCommand = true;
         initCase();
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }
         if( !ISDFileReadDone )
         {
            cout << "  vts - vtsReadISDFile has not been run yet! " << endl;
			break;
         }
		 string pname = param_array[0];
		 string pvalue = param_array[1];
         if(  pname != IMAGE_ID_PARAM &&      pname != IMAGE_INDEX_PARAM
           && pname != LOGICAL_INDEX_PARAM && pname != MODEL_NAME_PARAM)
		 {
            warnings.push_back(Warning(Warning::UNKNOWN_WARNING,
               "Invalid ISD Param name", "vts"));
            cout << "  vts - Invalid SD Param name!\n";
			break;
         }

         _TRY
            switch (ISDFileType)
            {
               case NITF_20_TYPE:
                  csmnitf20->addParam(pname, pvalue);
                  break;

               case NITF_21_TYPE:
                  csmnitf21->addParam(pname, pvalue);
                  break;

               case BYTESTREAM_TYPE:
                  csmbytestream->addParam(pname, pvalue);
                  break;

               case FILENAME_TYPE:
                  csmfilename->addParam(pname, pvalue);
                  break;

               default:
                  warnings.push_back(Warning(Warning::UNKNOWN_WARNING,
                                             "Invalid ISD type", "vts"));
                  cout << "  vts - ISD Type is unknown!\n";
            }                     //  end  switch
         _CATCH
         end_clock = clock();

         if (error_thrown)
         {
            cout << "Unable to add ISD Param\n";
            break;
         }
         if(debugFlag)
         {
            cout << "    vts - Returned values are: " << endl;
            cout << "      NONE" << endl << endl;
         }

         comment.clear();
         text = param_array[0] + ", " + param_array[1];

         recordLog(logFile,
               comment,
               command,
               text);
         exitCase();
         break;
      }
         //----- CASE vtsReadFilename NSetup
      case VTS_READ_FILENAME:
         logCommand = false;
         initCase();
		 
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

		 _TRY
			 string x = "1";
			 string filename = param_array[0].c_str();
             csmfilename = new Isd(filename);
             //initFilenameISD (csmfilename, param_array[0].c_str());
         _CATCH

         end_clock = clock();           // Record the end time for call.

         if(error_thrown)
         {
            ISDFileReadDone = true;
         }
         else
         {
            if(debugFlag)
            {
               cout << "    Passing values are: " << endl;
               cout << "      ISDFile=" << param_array[0]
                         << endl;
            }

            // Add error check TBD
            ISDFileType = FILENAME_TYPE;
            ISDFileReadDone = true;
         }

         comment.clear();
         text = param_array[0].c_str();

         recordLog(logFile,
               comment,
               command,
               text);
         exitCase();
         break;
         //----- CASE HELP           NSetup
      case HELP1:                       //help
      case HELP2:                       //Help
      case HELP3:                       //?

         logCommand = false;
         if(param_array.size() == 0)
         {
            cout << "   vts Commands:\n";

            bool someNotFound = false;
            for(j=0;j<MAX_NUMBER_COMMANDS;j++)
            {
               if(j == BEGIN_SP_NUMBER)
                  cout
                     << "\n\tThe following are Sensor Plugin commands:"
                     << endl;
               else if(j == BEGIN_SM_NUMBER)
                  cout
                        << "\n\tThe following are Sensor Model commands:"
                        << endl;
               else if(j == BEGIN_VTS_NUMBER)
                  cout
                        << "\n\tThe following are VTS internal commands:"
                        << endl;

               string footnote, helpText;
               helpText = vHelpDataValues[j];
               int item_index = helpText.find(" help is not defined");
               if (item_index > -1) {
                 someNotFound = true;
                 footnote = " * ";
               } else {
                 footnote = "   ";
               }
               cout << "\t" << footnote << menulist[j] << endl;
            }
            if(someNotFound)
               cout
                  << "\n\t * help text not available for this command"
                  << endl;
         }
         else
         {
            index = getCommandIndex(param_array[0].c_str());

            if(index != MAX_NUMBER_COMMANDS)
            {
               cout << menulist[index] << ":" << endl;
               cout << endl;

               cout << vHelpDataValues[index] << ends;
            }
            else
            {
               cout << "Command " << param_array[0]
                  << " was not found." << endl;
            }
         }

         exitCase();                    // HELP
         break;

         //----- CASE EXIT        NSetup
      case EXIT1:                       // exit
      case EXIT2:                       // Exit

		 
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

         time_t aclock;
         aclock = time(NULL);
         text =  "\nProgram End Time = " + (string)ctime(&aclock)
            + "\n\n\n\n\n"; ;
         recordLog(logFile, text);
         if (csmfilename)
            delete (csmfilename);
         if (csmbytestream)
            delete (csmbytestream);
         if (csmnitf20)
            delete (csmnitf20);
         if (csmnitf21)
            delete (csmnitf21);

         exit(0);
         //	       break; // case EXIT

         //----- CASE getList - tested
      case SP_GET_LIST:
         logCommand = false;
         initCase();
		 
         if(debugFlag)
         {
            cout << endl
               << "  vts - Calling getList"
               << endl;
            cout << "    Passing values are: " << endl;
            cout << "      NONE" << endl;
         }

         for(i=0; i < repeat_count; i++)
         {
            _TRY
               pluginList = Plugin::getList();
            _CATCH
         }
         end_clock = clock();
         comment.clear();
         text.clear();
         if (pluginList.size() == 0)
         {
            cout << "plugin::getList returned a null list SM-49\n";
            break;
         }
         for (sensorIter = pluginList.begin();
            sensorIter != pluginList.end();
            ++sensorIter)
         {
            _TRY
               pluginName = (*sensorIter)->getPluginName();
            _CATCH
			 text += pluginName;
			 text += " ";
         }
         recordLog(logFile,
               comment,
               command,
               text);
         param_array_index = 0;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE getPluginName NSetup - tested
      case SP_GET_PLUGIN_NAME:
         logCommand = false;
         initCase();
		 
         if(debugFlag)
         {
            cout << endl
               << "  vts - Calling Plugin::getPluginName"
               << endl;
            cout << "    Passing values are: " << endl;
            cout << "      NONE" << endl;
         }

         for(i=0; i < repeat_count; i++)
         {
			 pluginList = Plugin::getList();
         }
         end_clock = clock();
         comment.clear();
         text.clear();
         for (sensorIter = pluginList.begin();sensorIter != pluginList.end();++sensorIter)
         {
            _TRY
			   pluginName = (*sensorIter)->getPluginName();
            _CATCH
			 text += (pluginName + " ");
         }
         recordLog(logFile,
               comment,
               command,
               text);
         param_array_index = 0;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE findPlugin - tested
      case SP_FIND_PLUGIN:
         logCommand = false;
         initCase();
		 
         if(debugFlag)
         {
            cout << endl
               << "  vts - Calling findPlugin"
               << endl;
            cout << "    Passing values are: " << endl;
            cout << "      Plugin="  << param_array[0] << endl;
         }

         for(i=0; i < repeat_count; i++)
         {
            _TRY
                stubPlugin = (Plugin*)Plugin::findPlugin(param_array[0], &warnings);
            _CATCH
            if (warnings.size()>0) break;
         }
         end_clock = clock();
         comment.clear();
         text.clear();
         if (warnings.size()>0)
         {
            cout << "Plugin::findPlugin did not find "
               << param_array[0] << endl;
            text = "NOT Found";
         }
         else
         {
            cout << "Plugin::findPlugin found for " << param_array[0]
               << endl;
            text = "Found";
         }

         recordLog(logFile,
               comment,
               command,
               text);
         param_array_index = 0;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE removePlugin - tested
      case SP_REMOVE_PLUGIN:
         logCommand = false;
         initCase();
		 
         if(debugFlag)
         {
            cout << endl
               << "  vts - Calling removePlugin"
               << endl;
            cout << "    Passing values are: " << endl;
            cout << "      Plugin="  << param_array[0] << endl;
         }

         for (i = 0; i < repeat_count; i++)
         {
            string rem_name = param_array[0];
            _TRY
               Plugin::removePlugin(rem_name, &warnings);
            _CATCH
         }
         end_clock = clock();
         comment.clear();
         text.clear();
         if (warnings.size()>0)

            text = "Not Found";
         else
            text = "Removed";
         recordLog(logFile,
               comment,
               command,
               text);

         param_array_index = 0;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE vtsTestThreads
      case VTS_TEST_THREADS:
         logCommand = false;
         initCase();
		 
         if(debugFlag)
         {
            cout << "vts>    You requested routine "
               << menulist[commandNumber] << endl;
         }

         text.clear();
         for (i = 0; i < (int)param_array.size(); i++)
            text += (param_array[i] + ' ');

         if(debugFlag)
         {
            cout << "    Passing values are: " << endl;
            cout << "      text=" << text << endl;
         }

		 // initialize text in case threads don't work
         text =  " vtsTestThreads success unknown " ;

         param_array_index = 1; // = (int)param_array.size() ?;
         maxReturnValuesToCompare = 0;

		 configfile = param_array[0];

         _TRY
			text = testThreads(configfile);
         _CATCH
         if(error_thrown)
         {
            break;
         }
         else
         {
            if(debugFlag)
            {
               cout << text;
            }
         }
         end_clock = clock();           // Record the end time for call.
         comment.clear();
         recordLog(logFile,
             comment,
             command,
             text);
		 
		 exitCase();
         break;

         //----- CASE vtsSetEnv
      case VTS_SET_ENV:
      {
          logCommand = false;
          initCase();

          if (debugFlag)
          {
              cout << "vts>    You requested routine " << menulist[commandNumber] << "\n";
          }

          text.clear();
          for (i = 0; i < (int)param_array.size(); i++)
              text += (param_array[i] + ' ');

          if (debugFlag)
          {
              cout << "    Passing values are: \n" << "\ttext=" << text << "\n";
          }

          text = " vtsSetEnv success unknown ";

          param_array_index = 1; // = (int)param_array.size() ?;
          maxReturnValuesToCompare = 0;

          const auto name = param_array[0];
          const auto value = param_array[1];

          _TRY
              #if !_WIN32
	    setenv(name.c_str(), value.c_str(), true /*overwrite*/); // https://man7.org/linux/man-pages/man3/setenv.3.html
              #else
              const auto env = name + "=" + value;
              _putenv(env.c_str());
              #endif
	      text = name + "=" + getenv(name.c_str());
          _CATCH
              if (error_thrown)
              {
                  break;
              }
              else
              {
                  if (debugFlag)
                  {
                      cout << text;
                  }
              }
          end_clock = clock();           // Record the end time for call.
          comment.clear();
          recordLog(logFile, comment, command, text);

          exitCase();
          break;
      }

         //----- CASE getNumGeometricCorrectionSwitches NSetup
      case VTS_last_command_: // avoid compiler warning
      default:
         command_found = false;
   }                                    // switch

   return command_found;
}

///////////////////////////////////////////////////////
//
//   Function: processFileCommand
//
//   Description: This switch includes those cases which
//        require that an ISD File has been read
//
//   Input: commandNumber - Enumeration of the inputted
//                          command
//          param_array   - Argument to the command
//
//   Return Value:
//          bool - Command Found
//
///////////////////////////////////////////////////////
bool processFileCommand(int commandNumber,
                       vector<string> param_array)
{
   int j;                                 //----- Loop Variable
   bool command_found = true;

   string pluginName;
   string dump;

   PluginList pluginList;             // for getList
   //Plugin::PluginList::const_iterator sensorIter;
   PluginList::const_iterator sensorIter;

   if( !ISDFileReadDone )
   {
      cout << "  vts - vtsReadISDFile has not been run yet! "
                << endl;
      logCommand = false;            // do not log this command.
   }
   else                                 // file read not done
   {
      command_found = true;
      switch (commandNumber)
      {
         //----- CASE makeModelListFromISD  FRD
         case MAKE_MODEL_LIST_FROM_ISD:
         {
            // Part 2: This experiments with attempting to select
            // one of the registered models based on some
            // criteria. In this case, the criterion is a hard-
            // coded string with names of sensor models.
            //
            // In this example, three of the strings correspond to
            // example plugins that will be found. The fourth
            // (MeanRasterGM) is not one of the example models
            // and, therefore, will not be found.
            //
            // The actual criteria, and the exact methods, used for
            // sensor model selection in the real world are yet to
            // be finalized.  //---

            initCase();
            string modelName  = param_array[0];
            pluginName = param_array[1];
            logCommand = false;
            _TRY
               pluginList = Plugin::getList();
            _CATCH
            if (warnings.size() == 0)
            {                           // no warning returned
               end_clock = clock();
               j=0;
               // count loaded plugins
               if (pluginList.size() == 0)
                  cerr << "\a\n\a\n\a\t "
                     << "***** pluginList is NULL *****"
                     << "\a\n\a\n\a\n";
               else
               {                        // have a pluginptr list
                  _TRY
                      printList(logFile);
                  _CATCH
                  switch (ISDFileType)
                  {
                     case NITF_20_TYPE:
                        _TRY
							makeModelListFromISD(modelName, (Isd*)csmnitf20, &warnings);
                        _CATCH
                        break;

                     case NITF_21_TYPE:
                        _TRY
							makeModelListFromISD(modelName, (Isd*)csmnitf21, &warnings);
                        _CATCH
                        break;

                     case BYTESTREAM_TYPE:
                        _TRY
                            makeModelListFromISD(modelName, (Isd*)csmbytestream, &warnings);
                        _CATCH
                        break;

                     case FILENAME_TYPE:
                        _TRY
                            makeModelListFromISD(modelName, csmfilename, &warnings);
                        _CATCH
                        break;

                     default:
						 warnings.push_back(Warning(
							                    Warning::UNKNOWN_WARNING,
							                    "Invalid ISD type",
							                    "vts"));
                         cout << "  vts - ISD Type is unknown!\n";
                  }                     //  end  switch
                  _TRY
                     stubPlugin = (Plugin*)Plugin::findPlugin(pluginName, &warnings);
                  _CATCH
                  if (warnings.size()>0)
                  {
                     text = "NO Plugin Found ";
                     comment.clear();

                     cout
                        << "Plugin::findPlugin had an error."
                        << endl;
                  }
                  if (error_thrown)
                  {
                     text = "No Plugin Found ";
                  }
                  if (!error_thrown && warnings.size()==0)
                  {
                     text = "Found a Model that matches";
                     comment.clear();
                  }
               }                        // end if(pluginList.size() == 0)
            }                           // no warning from getList )
            else
               cout
                  << "\n\n ***** Plugins not loaded yet (case MAKE_MODEL_LIST_FROM_ISD) ***** \n\n";

            end_clock = clock();

            recordLog(logFile,
                  comment,
                  command,
                  text);
            exitCase();
            break;
         }                              // end case

         //----- CASE vtsDumpISD  FRD
         case VTS_DUMP_ISD:
            logCommand = false;
            
            if(debugFlag)
            {
               cout << "vts>    You requested routine "
                  << menulist[commandNumber] << endl;
            }

            switch (ISDFileType)
            {
               case NITF_20_TYPE:
                  cout << "\n!!!!! NITF 2.0 header:\n";
                  dumpHdr(csmnitf20);
                  break;

               case NITF_21_TYPE:
                  cout << "\n!!!!! NITF 2.1 header:\n";
                  dumpHdr(csmnitf21);
                  break;

               case BYTESTREAM_TYPE:
			   {
                  cout << "\nFORMAT= BYTESTREAM" << '\n';
//                dump.assign (csmbytestream->_isd, 0, 100);                 // todo: _isd -> data
				  string dump1 = csmbytestream->data();                      // i think this will work
				  dump.assign(dump1, 0, 100);
                  cout << "First 100 bytes of ISD: \n"
                     <<  dump << '\n';
			   }
                  break;

               case FILENAME_TYPE:
                  cout << "csmfilename filename is "
                     << csmfilename->filename() << '\n';
                  break;

               default:
                  cout << "  vts - ISDFileType is unknown! "
                     << endl;
            }                           // isd type switch

            param_array_index = 1;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;
         default:
            command_found = false;
      } // end of FILEREADDONE switch ISDFileReadDone ...
   }                                    // if (FRD) ...

   return command_found;
}

///////////////////////////////////////////////////////
//
//   Function: processSensorPluginCommand
//
//   Description: process the inputted command
//
//   Input: commandNumber - Enumeration of the inputted
//                          command
//          param_array   - Argument to the command
//
//   Return Value:
//          bool - Command Found
//
///////////////////////////////////////////////////////
bool processSensorPluginCommand(int commandNumber,
                       vector<string> param_array)
{
   int i;                              //----- Loop Variable
   bool command_found = true;

   bool constructable = false;         //----- Model may be constructable
   bool convertible = false;           //----- Can convert ISD to State

   string man_name;
   string relDate;
   string smName;

   int sensor_model_index = 0;
   int n_sensor_models=0;

   if(stubPlugin == NULL)
   {
      cout << NO_MODEL_LIST_ERROR << endl;
   }
   else
   {
      Isd *isd = NULL;
      command_found = true;
      switch (commandNumber)
      {

		 //----- CASE canISDBeConvertedToModelState - tested
         case SP_CAN_ISD_BE_CONVERTED_TO_MODEL_STATE:
            logCommand = false;
            initCase();
            for(i=0; i < repeat_count; i++)
            {
               switch (ISDFileType)
               {
                  case NITF_20_TYPE:
                     isd = csmnitf20;
                     break;

                  case NITF_21_TYPE:
                     isd = csmnitf21;
                     break;
                  case BYTESTREAM_TYPE:
                     isd = csmbytestream;
                     break;

                  case FILENAME_TYPE:
                     isd = csmfilename;
                     break;
                  default:
                     cout << "  vts - ISDFileType is unknown! "
                        << endl;
               }                        //. end inner switch
               _TRY
				   convertible = stubPlugin->canISDBeConvertedToModelState(
					   *isd,
					   param_array[0],
					   &warnings);
               _CATCH
            }                           // repeat count loop
            end_clock = clock();
            if (convertible)
               text = "Possibly Convertable";
            else
               text = "NOT Convertable";
            comment.clear();
            recordLog(logFile,
                  comment,
                  command,
                  text);
            exitCase();
            break;

         //----- CASE canModelBeConstructedFromISD  MMLD - tested
         case SP_CAN_MODEL_BE_CONSTRUCTED_FROM_ISD:
            logCommand = false;
            initCase();
            
            if(debugFlag)
            {
               cout
                  << endl
                  << "  vts - Calling canModelBeConstructedFromISD"
                  << endl;
               cout << "    Passing values are: " << endl;
               cout << "      Plugin="  << param_array[0] << endl;
            }

            switch (ISDFileType)
            {
               case NITF_20_TYPE:
                  isd = csmnitf20;
                  break;

               case NITF_21_TYPE:
                  isd = csmnitf21;
                  break;

               case BYTESTREAM_TYPE:
                  isd = csmbytestream;
                  break;

               case FILENAME_TYPE:
                  isd = csmfilename;
                  break;

               default:
                  cout << "  vts - ISDFileType is unknown!\n";
            }
            for(i=0; i < repeat_count; i++)
            {
               _TRY
			 	   constructable = stubPlugin->canModelBeConstructedFromISD(
					   *isd,
					   param_array[0],
					   &warnings);
               _CATCH
               if (warnings.size() > 0 || error_thrown) break;
            }
            end_clock = clock();

            if (warnings.size() > 0)
            {
               cout
                  << "Plugin::canModelBeConstructedFromISD can not construct from ISD "
                  << param_array[0] << endl;
               text = "NOT Constructable";
            }
            else
            {
               cout
                  << "Plugin::canModelBeConstructedFromISD can possibly be constructed from ISD "
                  << param_array[0] << endl;
               text = "Possibly Constructable";
            }
            comment.clear();
            recordLog(logFile,
                  comment,
                  command,
                  text);

            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE constructModelFromISD  MMLD - tested
         case SP_CONSTRUCT_MODEL_FROM_ISD:
            logCommand = false;
            initCase();
            
            if(debugFlag)
            {
               cout << endl
                  << "  vts - Calling constructModelFromISD"
                  << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            for (i=0; i < repeat_count; i++)
            {
               switch (ISDFileType)
               {
                  case NITF_20_TYPE:
                     isd = csmnitf20;
                     break;

                  case NITF_21_TYPE:
                     isd = csmnitf21;
                     break;

                  case BYTESTREAM_TYPE:
                     isd = csmbytestream;
                     break;

                  case FILENAME_TYPE:
                     isd = csmfilename;
                     break;

                  default:
                     cout << "  vts - ISDFileType is unknown! "
                        << endl;
               }                        // switch
               _TRY
                  rasterGM = (RasterGM*)stubPlugin->constructModelFromISD
					 ( *isd, 
					 string (param_array[0]),
					 &warnings);
               _CATCH
               end_clock = clock();
               if (!rasterGM)
               {
				   Warning wn(Warning::UNKNOWN_WARNING,
                               "Sensor Model Not Constructed",
                               "VTS::processCommand");
				   warnings.push_back(wn);

                  text = "Not Constructed";
               }
               else
               {
                  text = "Constructed";
               }
               comment.clear();
               recordLog(logFile,
                     comment,
                     command,
                     text);
               param_array_index = 0;
               maxReturnValuesToCompare = 0;
            }

            exitCase();
            break;

            //----- CASE convertISDToModelState  MMLD - tested
         case SP_CONVERT_ISD_TO_MODEL_STATE:
            logCommand = false;
            initCase();
            
            if(debugFlag)
            {
               cout << endl
                  << "  vts - Calling convertISDToModelState"
                  << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            for (i=0; i < repeat_count; i++)
            {
               switch (ISDFileType)
               {
                  case NITF_20_TYPE:
                     isd = csmnitf20;
                     break;

                  case NITF_21_TYPE:
                     isd = csmnitf21;
                     break;

                  case BYTESTREAM_TYPE:
                     isd = csmbytestream;
                     break;

                  case FILENAME_TYPE:
                     isd = csmfilename;
                     break;

                  default:
                     cout << "  vts - ISDFileType is unknown! "
                        << endl;
               }                        // end switch
               _TRY
				   returned_sensor_model_state = stubPlugin->convertISDToModelState(
					   *isd,
					   param_array[0],
					   &warnings);
               _CATCH
               end_clock = clock();

               // if no state returned
               if (returned_sensor_model_state == "" )
               {
				   Warning wn(Warning::UNKNOWN_WARNING,
					   "Sensor Model Not Converted",
					   "VTS::processCommand");
				   warnings.push_back(wn);

				   text = "Not Converted";
               }
               else
               {
                  text = "Converted -  Returned State:  ";
                  text += returned_sensor_model_state;
                  sensorModelStateSaved = true;
               }

              comment.clear();
              recordLog(logFile,
                    comment,
                    command,
                    text);

              param_array_index = 0;
              maxReturnValuesToCompare = 0;
            }
            exitCase();
            break;

            //----- CASE getManufacturer  MMLD - tested
         case SP_GET_MANUFACTURER:
            logCommand = false;
            initCase();
            
            if(debugFlag)
            {
               cout << endl
                  << "  vts - Calling getManufacturer"
                  << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            for(i = 0; i < repeat_count; i++)
            {
                _TRY
				   man_name = stubPlugin->getManufacturer();
                _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "        manufacturer_name=" << man_name
                         << endl;
               cout << endl;
            }

            comment.clear();
            text = man_name;
            recordLog(logFile,
                  comment,
                  command,
                  text);

            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;
            /////////////////  SMSS section

            //----- CASE getCsmVersion MMLD - tested
         case SP_GET_CSM_VERSION:
            logCommand = false;
            initCase();
            
            if(debugFlag)
            {
               cout << endl
                  << "  vts - Calling Plugin::getCsmVersion"
                  << endl;
               cout << "    Passing values are: "
                  << endl;
               cout << "      NONE" << endl;
            }

            for(i = 0; i < repeat_count; i++)
            {
               _TRY
				  retver = stubPlugin->getCsmVersion();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: "
                  << endl;
               cout << "        CsmVersion =" << retver
                  << endl;
               cout << endl;
            }

            comment.clear();
            stream << retver; 
            text = stream.str(); 

            recordLog(logFile,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;

            exitCase();
            break;

            //----- CASE getNumModels  MMLD - tested
         case SP_GET_NUM_MODELS:
            logCommand = false;
            initCase();
            
            if(debugFlag)
            {
               cout << endl
                  << "  vts - Calling getNumModels"
                  << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            for(i = 0; i < repeat_count; i++)
            {
               _TRY
			      n_sensor_models = stubPlugin->getNumModels();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "        n_sensor_models=" << n_sensor_models
                  << endl;
               cout << endl;
            }

            comment.clear();
            sprintf (tempStr, "%u", n_sensor_models);
            text = tempStr;

            recordLog(logFile,
                  comment,
                  command,
                  text);

            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE getReleaseDate  MMLD - tested
         case SP_GET_RELEASE_DATE:
            logCommand = false;
            initCase();
            
            if(debugFlag)
            {
               cout << endl
                  << "  vts - Calling getReleaseDate"
                  << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            for(i = 0; i < repeat_count; i++)
            {
               _TRY
			      relDate = stubPlugin->getReleaseDate();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "        release_date=" << relDate << endl;
               cout << endl;
            }

            comment.clear();
            text = relDate;
            recordLog(logFile,
                  comment,
                  command,
                  text);

            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

         //----- CASE getModelFamily  MMLD - tested
         case SP_GET_MODEL_FAMILY:
         {
            string family;
            logCommand = false;
            initCase();
            sensor_model_index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getModelFamily" << endl;
               cout << "    Passing values are: " << endl;
               cout << "        sensor_model_index=" << sensor_model_index << endl;
            }

            for(i = 0; i < repeat_count; i++)
            {
               _TRY
			      family = stubPlugin->getModelFamily(sensor_model_index);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "        Model Family=" << family << endl << endl;
            }

            comment.clear();
            text = family;
            recordLog(logFile,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;
         }

            //----- CASE getModelName1  MMLD - tested
         case SP_GET_MODEL_NAME1:
            logCommand = false;
            initCase();
            sensor_model_index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               cout << endl
                  << "  vts - Calling getModelName1"
                  << endl;
               cout << "    Passing values are: " << endl;
               cout << "        sensor_model_index="
                  << sensor_model_index << endl;
            }

            for(i = 0; i < repeat_count; i++)
            {
               _TRY
			      smName = stubPlugin->getModelName(sensor_model_index);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "        sensor_model_name=" << smName << endl;
               cout << endl;
            }

            comment.clear();
            text = smName;
            recordLog(logFile,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE getModelVersion MTLD - tested
         case SP_GET_MODEL_VERSION:
         {
			string verStr;
            logCommand = false;
            initCase();
            string sensor_model = param_array[0];

            if(debugFlag)
            {
               cout << endl
                  << "  vts - Calling "
                  << "SensorPlugin::RasterGM::getModelVersion"
                  << endl;
               cout << "    Passing values are: " << endl;
               cout << "      RasterGMName =" << sensor_model
                  << endl;
               cout << "      NONE" << endl;
            }

            for(i = 0; i < repeat_count; i++)
            {
               _TRY
                  Version version = stubPlugin->getModelVersion(sensor_model);
                  stringstream str;
                  str << version;
                  verStr = str.str();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "        RasterGM Version ="
                  << verStr << endl;
               cout << endl;
            }

            comment.clear();
            text = verStr;
            recordLog(logFile,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;

            exitCase();
            break;
         }

         //----- CASE getModelNameFromModelState MMLD - tested
         case SP_GET_MODEL_NAME_FROM_MODEL_STATE:
            logCommand = false;
            initCase();
            
            if(debugFlag)
            {
               cout
                  << endl
                  << "  vts - Calling "
                  << "getModelNameFromModelState"
                  << endl;
               cout << "    Passing values are: " << endl;
               cout << "      sensor_model_state="
                  << param_array[0]
                  << endl;
            }
            for(i = 0; i < repeat_count; i++)
            {
               _TRY
				   smName = stubPlugin->getModelNameFromModelState(
					   returned_sensor_model_state,
					   &warnings);
               _CATCH
               if (warnings.size()>0) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: "
                  << endl;
               cout << "        sensor_model_name="
                  << smName << endl;
               cout << endl;
            }

            comment.clear();
            text = smName;
            recordLog(logFile,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

         //----- CASE getSensorType
         case SP_GET_SENSOR_TYPE:
         {
            logCommand = false;
            logStringCommand = true;
            initCase();
            string type;

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getSensorType" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            for (i=0; i < repeat_count; i++)
            {
               _TRY
			      type = rasterGM->getSensorType();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      Sensor Type=" << type <<  endl << endl;
            }
            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            act_val_string.push_back(type);
            exitCase();
            break;
         }

         //----- CASE getSensorMode
         case SP_GET_SENSOR_MODE:
         {
            logCommand = false;
            logStringCommand = true;
            initCase();
            string mode;

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getSensorMode" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            for (i=0; i < repeat_count; i++)
            {
               _TRY
			      mode = rasterGM->getSensorMode();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      Sensor Mode=" << mode <<  endl << endl;
            }
            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            act_val_string.push_back(mode);
            exitCase();
            break;
         }

		 default:
            command_found = false;
      }                                 // end switch makemodellist done
   }                                    // if MMLD . . .

   return command_found;
}

///////////////////////////////////////////////////////
//
//   Function: processSensorModelCommand
//
//   Description: process the inputted command
//
//   Input: commandNumber - Enumeration of the inputted
//                          command
//          param_array   - Argument to the command
//
//   Return Value:
//          bool - Command Found
//
///////////////////////////////////////////////////////
bool processSensorModelCommand(int commandNumber,
                       vector<string> param_array)
{
   int i;                              //----- Loop Variable
   bool command_found = true;
   int index;
   bool error_switch = false;
   bool return_bool1 = false;
   ////vector<ParameterSharingCriteria> criteria;
   double allowableTimeDelta = 0.0;

   csm::param::Type parameterType = csm::param::NONE;

   double x = 0.0, y = 0.0, z = 0.0;
   double line = 0.0, sample = 0.0, time_of_imaging = 0.0, height = 0.0;
   double line1 = 0.0, sample1 = 0.0;
   RasterGM** SMs = NULL;
   double* lines = NULL;
   double* samples = NULL;
   double **crossCovarianceMatrix = NULL;

   //double groundAccuracy[9] = {0.0, 0.0, 0.0,   0.0, 0.0, 0.0,   0.0, 0.0, 0.0};
   double desired_precision = 0.0;
   double achieved_precision = 0.0;
   double groundCovariance[9] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
   double imageCovariance[4] = {0.0, 0.0, 0.0, 0.0};
   double heightVariance = 0.0;
   EcefLocus imgLocus;
   double locus[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
   vector<double> partials;
   double pt1Line = 0.0, pt1Sample = 0.0, pt2Line = 0.0, pt2Sample = 0.0;
   // Covariance error mensuration matrix in getUnmodeledError
   vector<double> covarianceErr;
   // Cross covariance error mensuration matrix in getUnmodeledCrossCovariance
   vector<double> crossCovarianceErr;

   RasterGM::SensorPartials sensorpartials;
   vector<RasterGM::SensorPartials> allSensorpartials;
   double line_partial = 1.0;
   double sample_partial = 2.0;
   int index1 = 1, index2 = 2;
   int num_lines=0,  num_samples=0;
   vector<double> line_partials;
   vector<double> sample_partials;

   int ret_int;
   double return_double = 1.0;
   vector<double> ret_list_double;
   string ptstr;
   double covariance = 24.0;
   double time_count = 0.0;
   double vx = 0.0, vy = 0.0, vz = 0.0;
   double value = 17.0;

   string trajectoryId = "";
   string date_and_time = "";
   string pluginName = "";
   string paramName = "";
   string smName;

   double direction_x = 0.0;
   double direction_y = 0.0;
   double direction_z = 0.0;

   pair<double,double> heightRange;
   double minHeight = -99000.0;
   double maxHeight =  99000.0;
   pair<ImageCoord,ImageCoord> imageRange;
   double minRow = -99000.0;
   double maxRow =  99000.0;
   double minCol = -99000.0;
   double maxCol =  99000.0;

   int csm_version = 0;

   if( rasterGM == NULL )
   {
      cout << NO_SENSOR_MODEL_ERROR << endl;
   }
   else
   {
      command_found = true;
	  //cout << "9998 switch ( " << commandNumber << endl;
      switch (commandNumber)
      {
         //----- CASE groundToImage -- 1  MMLD CMD - tested
         case SM_GROUND_TO_IMAGE1:
            initCase();
            
			groundPt.x = atof(param_array[0].c_str());
            groundPt.y = atof(param_array[1].c_str());
            groundPt.z = atof(param_array[2].c_str());

            if ((param_array.size() < 4) ||
               (param_array[3] == "-c"))
            {
               // Use defaulted desired precision;
               desired_precision = 0.001;
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[3].c_str());
			}

            for(i=0; i < repeat_count; i++)
            {
               _TRY
               ic = rasterGM->groundToImage(groundPt,
                                         desired_precision,
                                         &achieved_precision,
                                         &warnings);
               _CATCH
               if (warnings.size()>0) break;                                                                        \
            }

            if(!error_thrown)
            {
               if(debugFlag)
			   {
				   cout << "    vts - Returned values are: " << endl;
				   cout << "      line=" << ic.line << endl;
				   cout << "      sample=" << ic.samp << endl;
				   cout << "      achieved_precision=" << achieved_precision << endl;
				   cout << endl;
			   }
			   param_array_index = 4;        //location of 1st CL switch
			   maxReturnValuesToCompare = 3; //Maximum number of comparisons switches

			   act_val[0]=ic.line;
			   act_val[1]=ic.samp;
			   act_val[2]=achieved_precision;
			}
            exitCase();
            break;

            //----- CASE groundToImage -- 2  MMLD CMD - tested 
         case SM_GROUND_TO_IMAGE2:
            initCase();
			groundPtCovar.x = atof(param_array[0].c_str());
			groundPtCovar.y = atof(param_array[1].c_str());
			groundPtCovar.z = atof(param_array[2].c_str());
			groundPtCovar.covariance[1] = 2.0;
			groundPtCovar.covariance[2] = 2.0;
			groundPtCovar.covariance[0] = atof(param_array[3].c_str());
			groundPtCovar.covariance[1] = atof(param_array[4].c_str());
			groundPtCovar.covariance[2] = atof(param_array[5].c_str());
			groundPtCovar.covariance[3] = atof(param_array[6].c_str());
			groundPtCovar.covariance[4] = atof(param_array[7].c_str());
			groundPtCovar.covariance[5] = atof(param_array[8].c_str());
			groundPtCovar.covariance[6] = atof(param_array[9].c_str());
			groundPtCovar.covariance[7] = atof(param_array[10].c_str());
			groundPtCovar.covariance[8] = atof(param_array[11].c_str());

			if ((param_array.size() < 13) || (param_array[12] == "-c"))
			{
				 // Use defaulted desired precision;
				 desired_precision = 0.001;
			}
			else
			{
				 // Do not use defaulted desired precision;
				 desired_precision = atof(param_array[12].c_str());
			}
			for(i=0; i < repeat_count; i++)
			{
               _TRY
					icCov = rasterGM->groundToImage(groundPtCovar,
						desired_precision,
						&achieved_precision,
						&warnings);
               _CATCH
			   if (warnings.size() > 0 || error_thrown) break;
			}
			if(!error_thrown)
			{
				if(debugFlag)
				{
					cout << "      x,y,z=" << groundPtCovar.x << "," 
						<< groundPtCovar.y << "," 
						<< groundPtCovar.z << endl;
					cout << "      desired_precision=" << desired_precision << endl;
					cout << "      groundCovariance="
						<< groundPtCovar.covariance[0] << ","
						<< groundPtCovar.covariance[1] << ","
						<< groundPtCovar.covariance[2] << endl
						<< "                       "
						<< groundPtCovar.covariance[3] << ","
						<< groundPtCovar.covariance[4] << ","
						<< groundPtCovar.covariance[5]  << endl
						<< "                       "
						<< groundPtCovar.covariance[6] << ","
						<< groundPtCovar.covariance[7] << ","
						<< groundPtCovar.covariance[8] << endl;
				}

				if(debugFlag)
				{
					cout << "    vts - Returned values are: " << endl;
					cout << "      line=" << icCov.line << endl;
					cout << "      sample=" << icCov.samp << endl;
					cout << "      imageCovariance="
						<< icCov.covariance[0] << "," 
						<< icCov.covariance[1] << endl 
						<< icCov.covariance[3] << "," 
						<< icCov.covariance[4] << endl;
					cout << "      achieved_precision=" << achieved_precision << endl;
					cout << endl;
				}
				param_array_index = 13;     //location of first command line switch
				maxReturnValuesToCompare = 7; // Maximum number of comparisons switches
				act_val[0]=icCov.line;
				act_val[1]=icCov.samp;
				act_val[2]=achieved_precision;
				act_val[3]=icCov.covariance[0];
				act_val[4]=icCov.covariance[1];
				act_val[5]=icCov.covariance[2];
				act_val[6]=icCov.covariance[3];
			}
			exitCase();
			break;

            //----- CASE imageToGround form 1  MMLD CMD - tested
         case SM_IMAGE_TO_GROUND1:
            initCase();
            line = atof(param_array[0].c_str());
            sample = atof(param_array[1].c_str());
            height = atof(param_array[2].c_str());
            if ((param_array.size() < 4) ||
               (param_array[3] == "-c"))
            {
               // Use defaulted desired precision;
               for(i=0; i < repeat_count; i++)
               {
                  _TRY
                     groundPt = rasterGM->imageToGround(
						  ImageCoord(line, sample),
                          height,
					  	  .001,
						  &achieved_precision,
						  &warnings);
                  _CATCH
                  if (warnings.size() > 0 || error_thrown) break;
               }
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[3].c_str());
               for(i=0; i < repeat_count; i++)
               {
                  _TRY
                     groundPt = rasterGM->imageToGround(
						  ImageCoord(line, sample),
                          height,
					  	  desired_precision,
						  &achieved_precision,
						  &warnings);
                  _CATCH
                  if (warnings.size() > 0 || error_thrown) break;
               }
            }
            end_clock = clock();

            if(!error_thrown)
            {
				if(debugFlag)
				{
					cout << endl << "  vts - Calling imageToGround form 1" << endl;
					cout << "    Passing values are: " << endl;
					cout << "      line=" << line << endl;
					cout << "      sample=" << sample << endl;
					cout << "      height=" << height << endl;
					cout << "      desired_precision=" << desired_precision << endl;
					cout << "    vts - Returned values are: " << endl;
					cout << "      x,y,z=" << groundPt.x  << "," << groundPt.y << "," << groundPt.z << endl;
					cout << "      achieved_precision=" << achieved_precision << endl;
					cout << endl;
				}
				param_array_index = 4;      //location of first command line switch
				maxReturnValuesToCompare = 4;// Maximum number of comparisons switches
				act_val[0]=groundPt.x;
				act_val[1]=groundPt.y;
				act_val[2]=groundPt.z;
				act_val[3]=achieved_precision;
			}
			exitCase();
			break;

            //----- CASE imageToGround form 2  MMLD CMD - tested
         case SM_IMAGE_TO_GROUND2:
             initCase();
			 icCov.line = atof(param_array[0].c_str());
			 icCov.samp = atof(param_array[1].c_str());
			 icCov.covariance[0] = atof(param_array[2].c_str());
			 icCov.covariance[1] = atof(param_array[3].c_str());
			 icCov.covariance[2] = atof(param_array[4].c_str());
			 icCov.covariance[3] = atof(param_array[5].c_str());
			 height = atof(param_array[6].c_str());
			 heightVariance = atof(param_array[7].c_str());

            if ((param_array.size() < 9) ||
               (param_array[8] == "-c"))
            {
                // Use defaulted desired precision;
				desired_precision = .001;
			}
			else
			{
				// Do not use defaulted desired precision;
				desired_precision = atof(param_array[8].c_str());
			}
			for(i=0; i < repeat_count; i++)
			{
               _TRY
					groundPtCovar = rasterGM->imageToGround(icCov,
                						height,
										heightVariance,
										desired_precision,
										&achieved_precision,
										&warnings);
               _CATCH
			   if (warnings.size() > 0 || error_thrown) break;
			}
			end_clock = clock();

			if(!error_thrown)
            {
				if(debugFlag)
				{
					cout << endl << "  vts - Calling imageToGround form 2" << endl;
					cout << "    Passing values are: " << endl;
					cout << "      line=" << line << endl;
					cout << "      sample=" << sample << endl;
					cout << "      imageCovariance="
						<< icCov.covariance[0] << ","
						<< icCov.covariance[1] << endl
						<< "                      "
						<< icCov.covariance[2] << ","
						<< icCov.covariance[3]  << endl
						<< "      height=" << height << endl
						<< "      heightVariance=" << heightVariance
						<< endl
						<< "      desired_precision=" << desired_precision
						<< endl;
					cout << "    vts - Returned values are: " << endl;
					cout << "      x,y,z=" << groundPtCovar.x  << ","
						<< groundPtCovar.y << ","
						<< groundPtCovar.z
						<< endl;
					cout << "      groundCovariance="
						<< groundPtCovar.covariance[0] << ","
						<< groundPtCovar.covariance[1] << ","
						<< groundPtCovar.covariance[2]  << endl
						<< "                       "
						<< groundPtCovar.covariance[3] << ","
						<< groundPtCovar.covariance[4] << ","
						<< groundPtCovar.covariance[5]  << endl
						<< "                       "
						<< groundPtCovar.covariance[6] << ","
						<< groundPtCovar.covariance[7] << ","
						<< groundPtCovar.covariance[8] << endl;
					cout << "      achieved_precision="
						<< achieved_precision
						<< endl << endl;
				}
				param_array_index = 9;
				maxReturnValuesToCompare = 13;
				act_val[0] = groundPtCovar.x;
				act_val[1] = groundPtCovar.y;
				act_val[2] = groundPtCovar.z;
				act_val[3] = groundPtCovar.covariance[0];
				act_val[4] = groundPtCovar.covariance[1];
				act_val[5] = groundPtCovar.covariance[2];
				act_val[6] = groundPtCovar.covariance[3];
				act_val[7] = groundPtCovar.covariance[4];
				act_val[8] = groundPtCovar.covariance[5];
				act_val[9] = groundPtCovar.covariance[6];
				act_val[10] = groundPtCovar.covariance[7];
				act_val[11] = groundPtCovar.covariance[8];
				act_val[12] = achieved_precision;
			}
            exitCase();
            break;

            //----- CASE imageToProximateImagingLocus MMLD CMD - tested
         case SM_IMAGE_TO_PROXIMATE_IMAGING_LOCUS:
            initCase();
            imagePt.line = atof(param_array[0].c_str());
            imagePt.samp = atof(param_array[1].c_str());
            groundPt.x = atof(param_array[2].c_str());
            groundPt.y = atof(param_array[3].c_str());
            groundPt.z = atof(param_array[4].c_str());
            if ((param_array.size() < 6) ||
               (param_array[5] == "-c"))
            {
               // Use defaulted desired precision;
               desired_precision = 0.001;
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[5].c_str());
			}

            for(i=0; i < repeat_count; i++)
            {
               _TRY
				   imgLocus = rasterGM->imageToProximateImagingLocus(
					  imagePt,
					  groundPt,
					  desired_precision,
					  &achieved_precision,
					  &warnings);
               _CATCH
               if (warnings.size() > 0 || error_thrown) break;
            }
            if(!error_thrown)
            {
               if(debugFlag)
			   {
				   cout << endl << "  vts - Calling imageToProximateImagingLocus" << endl;
				   cout << "    Passing values are: " << endl;
				   cout << "      line=" << imagePt.line << endl;
				   cout << "      sample=" << imagePt.samp << endl;
				   cout << "      x,y,z=" << groundPt.x  << "," << groundPt.y << "," << groundPt.z << endl;
				   cout << "      desired_precision=" << desired_precision << endl;
				   cout << "    vts - Returned values are: " << endl;
				   cout << "      locus=" << imgLocus.direction.x  << "," 
			                              << imgLocus.direction.y << ","  
			                              << imgLocus.direction.z << endl
			            << "            " << imgLocus.point.x  << ","  
			                              << imgLocus.point.y << ","  
			                              << imgLocus.point.z << endl;
				   cout << "      achieved_precision=" << achieved_precision << endl << endl;
			   }
			   param_array_index = 1;
			   maxReturnValuesToCompare = 7;
			   act_val[0]=achieved_precision;
			   act_val[1]=imgLocus.direction.x;
			   act_val[2]=imgLocus.direction.y;
			   act_val[3]=imgLocus.direction.z;
			   act_val[4]=imgLocus.point.x;
			   act_val[5]=imgLocus.point.y;
			   act_val[6]=imgLocus.point.z;
			}
            exitCase();
            break;

            //----- CASE imageToRemoteImagingLocus   MMLD CMD - tested
         case SM_IMAGE_TO_REMOTE_IMAGING_LOCUS:
            initCase();
            
            imagePt.line = atof(param_array[0].c_str());
            imagePt.samp = atof(param_array[1].c_str());
			desired_precision = 0.001;
            if ((param_array.size() < 3) ||
               (param_array[2] == "-c"))
            {
               // Use defaulted desired precision;
               desired_precision = 0.001;
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[2].c_str());
			}
            for(i=0; i < repeat_count; i++)
            {
               _TRY
				   imgLocus = rasterGM->imageToRemoteImagingLocus(
					  imagePt,
					  desired_precision,
					  &achieved_precision,
					  &warnings);
               _CATCH
               if (warnings.size() > 0 || error_thrown) break;
			}

            if(!error_thrown)
            {
				if(debugFlag)
				{
					cout << endl << "  vts - Calling imageToRemoteImagingLocus" << endl;
					cout << "    Passing values are: " << endl;
					cout << "      line=" << imagePt.line << endl;
					cout << "      sample=" << imagePt.samp << endl;
					cout << "      desired_precision=" << desired_precision << endl;
					cout << "    vts - Returned values are: " << endl;
 				    cout << "      locus=" << imgLocus.direction.x  << "," 
			                               << imgLocus.direction.y << ","  
			                               << imgLocus.direction.z << endl
			             << "            " << imgLocus.point.x  << ","  
			                               << imgLocus.point.y << ","  
			                               << imgLocus.point.z << endl;
					cout << "      achieved_precision=" << achieved_precision << endl;
					cout << endl;
				}
				param_array_index = 3;
				maxReturnValuesToCompare = 7;
				act_val[0]=achieved_precision;
			    act_val[1]=imgLocus.direction.x;
			    act_val[2]=imgLocus.direction.y;
			    act_val[3]=imgLocus.direction.z;
			    act_val[4]=imgLocus.point.x;
			    act_val[5]=imgLocus.point.y;
			    act_val[6]=imgLocus.point.z;
			}
			exitCase();
            break;

            //----- CASE computeGroundPartials MMLD CMD - tested
         case SM_COMPUTE_GROUND_PARTIALS:
            initCase();
            
            groundPt.x = atof(param_array[0].c_str());
            groundPt.y = atof(param_array[1].c_str());
            groundPt.z = atof(param_array[2].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling computeGroundPartials" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      x,y,z=" << groundPt.x  << "," << groundPt.y << "," << groundPt.z << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
                _TRY
				   partials = rasterGM->computeGroundPartials(groundPt);
                _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      partials=" << partials[0] << "," << partials[1]  << ","
				                         << partials[2] << endl;
               cout << "               " << partials[3] << "," << partials[4]  << ","
				                         << partials[5] << endl << endl;
            }
            param_array_index = 3;
            maxReturnValuesToCompare = 6;
            act_val[0]=partials[0];
            act_val[1]=partials[1];
            act_val[2]=partials[2];
            act_val[3]=partials[3];
            act_val[4]=partials[4];
            act_val[5]=partials[5];
            exitCase();
            break;

            //----- CASE computeSensorPartials form 1 MMLD CMD - tested
         case SM_COMPUTE_SENSOR_PARTIALS1:
            initCase();
            
            index = atoi(param_array[0].c_str());
            groundPt.x = atof(param_array[1].c_str());
            groundPt.y = atof(param_array[2].c_str());
            groundPt.z = atof(param_array[3].c_str());
            if ((param_array.size() < 5) ||
               (param_array[4] == "-c"))
            {
               // Use defaulted desired precision;
               desired_precision = 0.001;
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[4].c_str());
			}
            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  sensorpartials = rasterGM->computeSensorPartials(
                     index,
                     groundPt,
                     desired_precision,
                     &achieved_precision,
                     &warnings);
                  line_partial = sensorpartials.first;
                  sample_partial = sensorpartials.second;
               _CATCH
               if (warnings.size() > 0 || error_thrown) break;
            }
            if(!error_thrown)
            {
				if(debugFlag)
				{
					cout << endl << "  vts - Calling computeSensorPartials form 1" << endl;
					cout << "    Passing values are: " << endl;
					cout << "      index=" << index << endl;
					cout << "      x,y,z=" << groundPt.x << "," << groundPt.y << "," << groundPt.z << endl;
					cout << "      desired_precision=" << desired_precision << endl;
					cout << "    vts - Returned values are: " << endl;
					cout << "      line_partial=" << line_partial << endl;
					cout << "      sample_partial=" << sample_partial << endl;
					cout << "      achieved_precision=" << achieved_precision << endl << endl;
				}
				param_array_index = 1;
				maxReturnValuesToCompare = 3;
				act_val[0]=line_partial;
				act_val[1]=sample_partial;
				act_val[2]=achieved_precision;
			}
            exitCase();
            break;

            //----- CASE computeSensorPartials form 2  MMLD CMD - tested
         case SM_COMPUTE_SENSOR_PARTIALS2:
            initCase();
            
            index = atoi(param_array[0].c_str());
            imagePt.line = atof(param_array[1].c_str());
            imagePt.samp = atof(param_array[2].c_str());
            groundPt.x = atof(param_array[3].c_str());
            groundPt.y = atof(param_array[4].c_str());
            groundPt.z = atof(param_array[5].c_str());
            if ((param_array.size() < 7) ||
               (param_array[6] == "-c"))
            {
               // Use defaulted desired precision;
               desired_precision = 0.001;
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[6].c_str());
			}
            for(i=0; i < repeat_count; i++)
            {
               _TRY
				   sensorpartials = rasterGM->computeSensorPartials(
					   index,
					   imagePt,
					   groundPt,
					   desired_precision,
					   &achieved_precision,
					   &warnings);
				   line_partial = sensorpartials.first;
				   sample_partial = sensorpartials.second;
               _CATCH
               if (warnings.size() > 0 || error_thrown) break;
            }
            if(!error_thrown)
            {
				if(debugFlag)
				{
					cout << endl << "  vts - Calling computeSensorPartials form 2" << endl;
					cout << "    Passing values are: " << endl;
					cout << "      index=" << index << endl;
					cout << "      line=" << imagePt.line << endl;
					cout << "      sample=" << imagePt.samp << endl;
					cout << "      x,y,z=" << groundPt.x  << "," << groundPt.y << "," << groundPt.z << endl;
					cout << "      desired_precision=" << desired_precision << endl;
					cout << "    vts - Returned values are: " << endl;
					cout << "      line_partial=" << line_partial << endl;
					cout << "      sample_partial=" << sample_partial << endl;
					cout << "      achieved_precision=" << achieved_precision << endl << endl;
				}
				param_array_index = 1;
				maxReturnValuesToCompare = 3;


				act_val[0]=line_partial;
				act_val[1]=sample_partial;
				act_val[2]=achieved_precision;
			}
			exitCase();
			break;

         //----- CASE computeAllSensorPartials form 1  MMLD
         case SM_COMPUTE_ALL_SENSOR_PARTIALS1:
         {
            initCase();
            
            groundPt.x = atof(param_array[0].c_str());
            groundPt.y = atof(param_array[1].c_str());
            groundPt.z = atof(param_array[2].c_str());
            desired_precision = 0.001;        //set default
			param::Set pSet = param::VALID;   //set default

            for(size_t i=3;i<5;i++)
            {              
               if(param_array.size() <= i )break;
               if(param_array[i] == "-c")break;

               if(param_array[i] == "ADJUSTABLE")
               {
                  pSet = param::ADJUSTABLE;
               }
               else if(param_array[i] == "FIXED")
               {
                  pSet = param::NON_ADJUSTABLE;
               }
               else
               {
                  // Do not use defaulted desired precision;
                  desired_precision = atof(param_array[i].c_str());
               }
            }

            for(int i=0; i < repeat_count; i++)
            {
               _TRY
				   allSensorpartials = rasterGM->computeAllSensorPartials(
					   groundPt,
					   pSet,
					   desired_precision,
					   &achieved_precision,
					   &warnings);
               _CATCH
               if (warnings.size() > 0 || error_thrown) break;

            }
            if(!error_thrown)
            {
               vector<RasterGM::SensorPartials>::const_iterator iter;
			   string txt = "";
               stringstream str;
               str << "      line_partials           sample_partials" << endl;

               maxReturnValuesToCompare = 0;
               for (iter = allSensorpartials.begin();iter != allSensorpartials.end();++iter)
               {
                  line_partial = (*iter).first ;
                  sample_partial = (*iter).second;
                  act_val[maxReturnValuesToCompare++] = line_partial;
                  act_val[maxReturnValuesToCompare++] = sample_partial;

  			      str << "      " << line_partial << "      " << sample_partial<< endl;
               }
               txt = str.str();
               if(debugFlag)
               {
                  cout << endl << "  vts - Calling computeAllSensorPartials form 1" << endl;
                  cout << "    Passing values are: " << endl;
                  cout << "      x,y,z=" << x  << "," << y << "," << z << endl;
                  cout << "      pSet=" << pSet << endl;
                  cout << "      desired_precision=" << desired_precision << endl;
                  cout << "    vts - Returned values are: " << endl;
                  cout << txt;
                  cout << "      achieved_precision=" << achieved_precision << endl;
                  cout << endl;
               }
               param_array_index = 1;
               act_val[maxReturnValuesToCompare++]=achieved_precision;
			}
            exitCase();
            break;
		 }

         //----- CASE computeAllSensorPartials form 2 MMLD
         case SM_COMPUTE_ALL_SENSOR_PARTIALS2:
         {
            initCase();
            
            imagePt.line   = atof(param_array[0].c_str());
            imagePt.samp = atof(param_array[1].c_str());
            groundPt.x      = atof(param_array[2].c_str());
            groundPt.y      = atof(param_array[3].c_str());
            groundPt.z      = atof(param_array[4].c_str());

			desired_precision = 0.001;        //set default
			param::Set pSet = param::VALID;   //set default

            // Use defaulted desired precision;
            for(size_t i=5;i<7;i++)
            {              
               if(param_array.size() <= i)break;
               if(param_array[i] == "-c")break;

               if(param_array[i] == "ADJUSTABLE")
               {
                  pSet = param::ADJUSTABLE;
               }
               else if(param_array[i] == "FIXED")
               {
                  pSet = param::NON_ADJUSTABLE;
               }
               else
               {
                  // Do not use defaulted desired precision;
                  desired_precision = atof(param_array[i].c_str());
               }
            }

            for(int i=0; i < repeat_count; i++)
            {
               _TRY
                  allSensorpartials = rasterGM->computeAllSensorPartials(
                     imagePt,
                     groundPt,
                     pSet,
                     desired_precision,
                     &achieved_precision,
                     &warnings);
               _CATCH
               if (warnings.size() > 0 || error_thrown) break;
            }

            if(!error_thrown)
            {
               vector<RasterGM::SensorPartials>::const_iterator iter;
			   string txt = "";
               stringstream str;
               str << "      line_partials           sample_partials" << endl;

               maxReturnValuesToCompare = 0;
               for (iter = allSensorpartials.begin();iter != allSensorpartials.end();++iter)
               {
                  line_partial = (*iter).first ;
                  sample_partial = (*iter).second;
                  act_val[maxReturnValuesToCompare++] = line_partial;
                  act_val[maxReturnValuesToCompare++] = sample_partial;

  			      str << "      " << line_partial << "      " << sample_partial<< endl;
               }
               txt = str.str();
               if(debugFlag)
               {
                  cout << endl << "  vts - Calling computeAllSensorPartials form 1" << endl;
                  cout << "    Passing values are: " << endl;
                  cout << "      x,y,z=" << x  << "," << y << "," << z << endl;
                  cout << "      pSet=" << pSet << endl;
                  cout << "      desired_precision=" << desired_precision << endl;
                  cout << "    vts - Returned values are: " << endl;
                  cout << txt;
                  cout << "      achieved_precision=" << achieved_precision << endl;
                  cout << endl;
               }
               param_array_index = 1;
               act_val[maxReturnValuesToCompare++]=achieved_precision;
			}
            exitCase();
            break;
         }

            //----- CASE getCollectionIdentifier - tested
         case SM_GET_COLLECTION_IDENTIFIER:
         {
            initCase();
            
            logCommand = false;
            logStringCommand = true;

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getCollectionIdentifier" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            string collectionId;
            for(i=0; i < repeat_count; i++)
            {
               _TRY
			      collectionId = rasterGM->getCollectionIdentifier();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      collectionId=" << collectionId << endl;
               cout << endl;
            }

            act_val_string.push_back(collectionId);
            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }                              // end case

         //----- CASE getCrossCovarianceMatrix   MMLD 
         case SM_GET_CROSS_COVARIANCE_MATRIX:
         {
            initCase();
            
			int matsize = 0;
            vector<double> crossCovarianceMatrix;

            param::Set pSet = param::VALID;   //set default
			if(param_array.size() > 0)
            {
               if(param_array[0] == "ADJUSTABLE")
               {
                  pSet = param::ADJUSTABLE;
               }
               else if(param_array[0] == "FIXED")
               {
                  pSet = param::NON_ADJUSTABLE;
               }
            }
 
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getCrossCovarianceMatrix" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      pSet=" << pSet << endl;
			}

            for(i=0; i < repeat_count; i++)
            {
               GeometricModel* comparisonModel = rasterGM;
			   GeometricModel::GeometricModelList otherModels;
                _TRY
                  crossCovarianceMatrix 
                     = rasterGM->getCrossCovarianceMatrix(*comparisonModel, pSet, otherModels);
                _CATCH
			}
            end_clock = clock();

			matsize = crossCovarianceMatrix.size();
            sprintf (tempStr, "matsize=%i, crossCovarianceMatrix=", matsize);
            text = tempStr;
            for(int nn=0;nn<matsize;nn++)
            {
               sprintf (tempStr, "%g, ",crossCovarianceMatrix[nn]);
               text += tempStr;
               text += "\n";
            }
			
            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl
                  << text << endl;
            }
			param_array_index = matsize;
			maxReturnValuesToCompare = matsize;
            for(int nn=0;nn<matsize;nn++)
			{
               act_val[nn] = crossCovarianceMatrix[nn];
			}

            exitCase();
            break;
		 }

		 //----- CASE getCovarianceModel
         case SM_GET_COVARIANCE_MODEL:
         {
            initCase();
            
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getCovarianceModel" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            _TRY
               cvm = (csm::CorrelationModel*)(&rasterGM->getCorrelationModel());
            _CATCH
			fpcvm = false;
			ldcvm = false;
			if(cvm->format() == "Four-parameter model (A, alpha, beta, tau)")
			{
				fpcvm = true;
			}
			else if(cvm->format() == "LinearDecayCorrelation")
			{
				ldcvm = true;
			}
			end_clock = clock();
			
            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl
                  <<   "          NONE" << endl;
            }

            exitCase();
            break;
		 }

         //----- CASE getNumSensorModelParameters
		 case SM_GET_NUM_SENSOR_MODEL_PARAMETERS:
         {
            initCase();
            
            logCommand = false;
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getNumSensorModelParameters" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

			int num = -1;
            _TRY
			   if(fpcvm)
			   {
			      num = ((FourParameterCorrelationModel*) (cvm))->getNumSensorModelParameters();
			   }
			   else if(ldcvm)
			   {
			      num = ((LinearDecayCorrelationModel*) (cvm))->getNumSensorModelParameters();
			   }
            _CATCH

			end_clock = clock();
			
            stringstream str;
            str << "      num=" << num << endl;
            text = str.str();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << text << endl;
            }

            comment.clear();
            recordLog(logFile,
                  comment,
                  command,
                  text);

            exitCase();
            break;
		 }

         //----- CASE getNumCorrelationParameterGroups
		 case SM_GET_NUM_CORRELATION_PARAMETER_GROUPS:
         {
            initCase();
            
            logCommand = false;
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getNumCorrelationParameterGroups" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            int numGr = -1;
            _TRY
			   if(fpcvm)
			   {
			      numGr = ((FourParameterCorrelationModel*) (cvm))->getNumCorrelationParameterGroups();
			   }
			   else if(ldcvm)
			   {
			      numGr = ((LinearDecayCorrelationModel*) (cvm))->getNumCorrelationParameterGroups();
			   }
            _CATCH
            
			end_clock = clock();
			
            stringstream str;
            str << "      numGr=" << numGr << endl;
            text = str.str();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << text << endl;
            }

            comment.clear();
            recordLog(logFile,
                  comment,
                  command,
                  text);

            exitCase();
            break;
		 }

         //----- CASE getCorrelationParameterGroup
		 case SM_GET_CORRELATION_PARAMETER_GROUP:
         {
            initCase();
            
            logCommand = false;
            int pIndex = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getCorrelationParameterGroup" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      pIndex=" << pIndex << endl;
            }

            int grIndex;
            _TRY
               grIndex = -1;
               if(fpcvm)
               {
                  grIndex = ((FourParameterCorrelationModel*) (cvm))->getCorrelationParameterGroup(pIndex);
               }
               else if(ldcvm)
               {
                  grIndex = ((LinearDecayCorrelationModel*) (cvm))->getCorrelationParameterGroup(pIndex);
               }
            _CATCH
            
			end_clock = clock();
            if(!error_thrown)
            {
               stringstream str;
               str << "      grIndex=" << grIndex << endl;
               text = str.str();

               if(debugFlag)
               {
                  cout << "    vts - Returned values are: " << endl;
                  cout << text << endl;
               }

               comment.clear();
               recordLog(logFile,
                     comment,
                     command,
                     text);
            }

            exitCase();
            break;
		 }

         //----- CASE getCorrelationCoefficient
		 case SM_GET_CORRELATION_COEFFICIENT:
         {
            initCase();
            
            logCommand = false;
            int GrIndex = atoi(param_array[0].c_str());
			double deltaTime = atof(param_array[1].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getCorrelationParameterGroup" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      GrIndex=" << GrIndex << ", deltaTime=" << deltaTime << endl;
            }

            double coeff = -1;
            _TRY
               if(fpcvm)
               {
                  coeff = ((FourParameterCorrelationModel*) (cvm))->getCorrelationCoefficient(GrIndex, deltaTime);
               }
               else if(ldcvm)
               {
                  coeff = ((LinearDecayCorrelationModel*) (cvm))->getCorrelationCoefficient(GrIndex, deltaTime);
               }
            _CATCH
            
			end_clock = clock();
			
            if(!error_thrown)
            {
               stringstream str;
               str << "      coeff=" << coeff << endl;
               text = str.str();

               if(debugFlag)
               {
                  cout << "    vts - Returned values are: " << endl;
                  cout << text << endl;
               }

               comment.clear();
               recordLog(logFile,
                     comment,
                     command,
                     text);
			}
            exitCase();
            break;
		 }

         //----- CASE getCorrelationGroupParameters
		 case SM_GET_CORRELATION_GROUP_PARAMETERS:
         {
            initCase();
            
            logCommand = false;
            FourParameterCorrelationModel::Parameters fpParams;
            LinearDecayCorrelationModel::Parameters ldParams;
			size_t grIndex = atoi(param_array[0].c_str());
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getParameters" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      grIndex=" << grIndex << endl;
            }

            _TRY
               if(fpcvm)
               {
                  fpParams = ((FourParameterCorrelationModel*) (cvm))->getCorrelationGroupParameters(grIndex);
               }
               else if(ldcvm)
               {
                  ldParams = ((LinearDecayCorrelationModel*) (cvm))->getCorrelationGroupParameters(grIndex);
               }
            _CATCH

			end_clock = clock();

            if(!error_thrown)
            {
               stringstream str;
               if(fpcvm)
               {
                  str << "      a=" << fpParams.a << ",";
                  str << "      alpha=" << fpParams.alpha << ",";
                  str << "      beta=" << fpParams.beta << ",";
                  str << "      tau=" << fpParams.tau << endl;
               }
               else if(ldcvm)
               {
                  str << "      theInitialCorrsPerSegment=";
                  for(size_t ii=0;ii<ldParams.theInitialCorrsPerSegment.size();ii++)
                  {
                     str << ldParams.theInitialCorrsPerSegment[ii] << ",";
                  }
                  str << endl;
                  str << "      theTimesPerSegment=";
                  for(size_t ii=0;ii<ldParams.theTimesPerSegment.size();ii++)
                  {
					 str << ldParams.theTimesPerSegment[ii] << ",";
                  }
                  str << endl;
               }
               text = str.str();

               if(debugFlag)
               {
                  cout << "    vts - Returned values are: " << endl;
                  cout << text << endl;
               }

               comment.clear();
               recordLog(logFile,
                     comment,
                     command,
                     text);
               maxReturnValuesToCompare = 0;
			}
            exitCase();
            break;
		 }

         //----- CASE setCorrelationParameterGroup
         case SM_SET_CORRELATION_PARAMETER_GROUP:
         {
            initCase();
            
			size_t pIndex = atoi(param_array[0].c_str());
			size_t grIndex = atoi(param_array[1].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling setCorrelationParameterGroup" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      pIndex=" << pIndex << endl;
               cout << "      grIndex=" << grIndex << endl;
            }

            _TRY
               if(fpcvm)
               {
                  ((FourParameterCorrelationModel*) (cvm))->setCorrelationParameterGroup(pIndex, grIndex);
               }
               else if(ldcvm)
               {
                  ((LinearDecayCorrelationModel*) (cvm))->setCorrelationParameterGroup(pIndex, grIndex);
               }
            _CATCH
            end_clock = clock();

            if(!error_thrown)
            {
               if(debugFlag)
               {
                  cout << "    vts - Returned values are: " << endl;
                  cout << "      NONE" << endl << endl;
               }
            }
            exitCase();
            break;
		 }

         //----- CASE setCorrelationGroupParameters
		 case SM_SET_CORRELATION_GROUP_PARAMETERS:
         {
            initCase();
            
            size_t cpGroupIndex = atoi(param_array[0].c_str());
            double a;
            double alpha;
            double beta;
            double tau;
			LinearDecayCorrelationModel::Parameters ldParams;
            if(fpcvm)
            {
               a = atof(param_array[1].c_str());
               alpha = atof(param_array[2].c_str());
               beta = atof(param_array[3].c_str());
               tau = atof(param_array[4].c_str());
            }
            else if(ldcvm)
            {
               int psz = (param_array.size()-1)/2;
               for(int ii=1;ii<=psz;ii++)
               {
                  double icps = atof(param_array[ii].c_str());
                  double tps = atof(param_array[ii+psz].c_str());

                  ldParams.theInitialCorrsPerSegment.push_back(icps);
				  ldParams.theTimesPerSegment.push_back(tps);
               }
            }

            if(debugFlag)
            {
               cout << endl << "  vts - Calling setCorrelationGroupParameters" << endl;
               cout << "    Passing values are: " << endl;
               if(fpcvm)
               {
                  cout << "      a=" << a << endl;
                  cout << "      alpha=" << alpha << endl;
                  cout << "      beta=" << beta << endl;
                  cout << "      tau" << tau << endl;
               }
               else if(ldcvm)
               {
                  cout << "      theInitialCorrsPerSegment=";
                  for(size_t ii=0;ii<ldParams.theInitialCorrsPerSegment.size();ii)
                  {
                     cout << ldParams.theInitialCorrsPerSegment[ii] << ",";
                  }
                  cout << endl;
                  cout << "      theTimesPerSegment=";
                  for(size_t ii=0;ii<ldParams.theTimesPerSegment.size();ii)
                  {
                     cout << ldParams.theTimesPerSegment[ii] << ",";
                  }
                  cout << endl;
               }
            }

            _TRY
               if(fpcvm)
               {
                  ((FourParameterCorrelationModel*) (cvm))->setCorrelationGroupParameters(cpGroupIndex, a, alpha, beta, tau);
               }
               else if(ldcvm)
               {
                  ((LinearDecayCorrelationModel*) (cvm))->setCorrelationGroupParameters(cpGroupIndex,ldParams);
               }
            _CATCH
            end_clock = clock();

            if(!error_thrown)
            {
               if(debugFlag)
               {
                  cout << "    vts - Returned values are: " << endl;
                  cout << "      NONE" << endl << endl;
               }
            }
            exitCase();
            break;
		 }

         //----- CASE getCovarianceMatrix
         case SM_GET_COVARIANCE_MATRIX:
         {
            initCase();
            
			int matsize = 0;
            vector<double> covarianceMatrix;
            param::Set pSet = param::VALID;   //set default
            bool pSetEntered = false;
			if(param_array.size() > 0)
            {
               pSetEntered = true;
               if(param_array[0] == "ADJUSTABLE")
               {
                  pSet = param::ADJUSTABLE;
               }
               else if(param_array[0] == "FIXED")
               {
                  pSet = param::NON_ADJUSTABLE;
               }
            }

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getCovarianceMatrix" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      pSet=" << pSet << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  if(pSetEntered)
                  {
                     covarianceMatrix = rasterGM->getCovarianceMatrix(pSet);
                  }
                  else
                  {
                     covarianceMatrix = rasterGM->getCovarianceMatrix();
                  }
               _CATCH
            }
            end_clock = clock();

			matsize = covarianceMatrix.size();
            sprintf (tempStr, "matsize=%i, covarianceMatrix=", matsize);
            text = tempStr;
            for(int nn=0;nn<matsize;nn++)
            {
               sprintf (tempStr, "%g, ",covarianceMatrix[nn]);
               text += tempStr;
               text += "\n";
            }
			
            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl
                  << text << endl;
            }
			param_array_index = matsize;
			maxReturnValuesToCompare = matsize;
            for(int nn=0;nn<matsize;nn++)
			{
               act_val[nn] = covarianceMatrix[nn];
			}

            exitCase();
            break;
		 }

         //----- CASE getUnmodeledError MMLD CMD - tested
         case SM_GET_UNMODELED_ERROR:
         {
            initCase();
            
            imagePt.line = atof(param_array[0].c_str());
            imagePt.samp = atof(param_array[1].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getUnmodeledError" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      line,sample=" << imagePt.line  << "," << imagePt.samp << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
                // note that: getUnmodeledError calls getUnmodeledCrossCovariance
				//            passing imagePt as both ImageCoord arguments
				//            (see getUnmodeledError in CsmRasterGM.h)
                // getUnmodeledError() now also calls getSIXUnmodeledError() to get
                // values from the SICD/SIDD XML.
                _TRY
				   covarianceErr = rasterGM->getUnmodeledError(imagePt);
                _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Refturned values are: " << endl;
               cout << "      covarianceErr="
                  << covarianceErr[0] << ","
                  << covarianceErr[1] << ","
                  << covarianceErr[2] << ","
				  << covarianceErr[3] << endl << endl;
            }
            param_array_index = 2;
            maxReturnValuesToCompare = 4;
            act_val[0]=covarianceErr[0];
            act_val[1]=covarianceErr[1];
            act_val[2]=covarianceErr[2];
            act_val[3]=covarianceErr[3];

            exitCase();
            break;
         }

            //----- CASE getUnmodeledCrossCovariance MMLD CMD - tested
         case SM_GET_UNMODELED_CROSS_COVARIANCE:
		 {
            initCase();
            
            ImageCoord pt1;
            ImageCoord pt2;
            pt1.line = atof(param_array[0].c_str());
            pt1.samp = atof(param_array[1].c_str());
            pt2.line = atof(param_array[2].c_str());
            pt2.samp = atof(param_array[3].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getUnmodeledCrossCovariance" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      pt1Line,pt1Sample=" << pt1.line  << "," << pt1.samp << endl;
               cout << "      pt2Line,pt2Sample=" << pt2.line  << "," << pt2.samp << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  crossCovarianceErr = rasterGM->getUnmodeledCrossCovariance(pt1,pt2);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: "
                  << endl;
               cout << "      crossCovarianceErr="
                  << crossCovarianceErr[0]
                  << ","
                  << crossCovarianceErr[1]
                  << "," << crossCovarianceErr[2]
                  << "," << crossCovarianceErr[3]
                  << endl;
               cout << endl;
            }
            param_array_index = 4;
            maxReturnValuesToCompare = 4;
            act_val[0]=crossCovarianceErr[0];
            act_val[1]=crossCovarianceErr[1];
            act_val[2]=crossCovarianceErr[2];
            act_val[3]=crossCovarianceErr[3];

            exitCase();
            break;
		 }

            //----- CASE getParameterCovariance  MMLD CMD - tested
         case SM_GET_PARAMETER_COVARIANCE:
            initCase();
            
            index1 = atoi(param_array[0].c_str());
            index2 = atoi(param_array[1].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getParameterCovariance" << endl;
               cout << "    Passing values are: "<< endl;
               cout << "      index1=" << index1 << endl;
               cout << "      index2=" << index2 << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
                _TRY
                   covariance = rasterGM->getParameterCovariance(index1, index2);
                _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "        covariance=" << return_double << endl << endl;
            }
            param_array_index = 2;
            maxReturnValuesToCompare = 1;
            act_val[0] = covariance;
            exitCase();
            break;

            //----- CASE setParameterCovariance  MMLD CMD - tested
         case SM_SET_PARAMETER_COVARIANCE:
            initCase();
            
            index1 = atoi(param_array[0].c_str());
            index2 = atoi(param_array[1].c_str());
            covariance = atof(param_array[2].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling setParameterCovariance" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      index1=" << index1 << endl;
               cout << "      index2=" << index2 << endl;
               cout << "      covariance=" << covariance << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
                _TRY
                   rasterGM->setParameterCovariance(index1, index2, covariance);
                _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      NONE" << endl << endl;
            }
            exitCase();
            break;

            //----- CASE getTrajectoryIdentifier MMLD CMD - tested
         case SM_GET_TRAJECTORY_IDENTIFIER:
            logCommand = false;
            logStringCommand = true;
            initCase();
            
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getTrajectoryIdentifier" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                   trajectoryId = rasterGM->getTrajectoryIdentifier();
                _CATCH
			}
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      trajectoryId=" << trajectoryId << endl << endl;
            }
            act_val_string.push_back(trajectoryId);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;

            //----- CASE getReferenceDateAndTime MMLD CMD - tested
         case SM_GET_REFERENCE_DATE_AND_TIME:
            logCommand = false;
            logStringCommand = true;
            initCase();
            
            if(debugFlag)
            {
               cout << endl
                  << "  vts - Calling getReferenceDateAndTime"
                  << endl;
               cout << "    Passing values are: "
                  << endl;
               cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
			      date_and_time = rasterGM->getReferenceDateAndTime();
               _CATCH
			}
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: "
                  << endl;
               cout << "      date_and_time="
                  << date_and_time << endl;
               cout << endl;
            }
            act_val_string.push_back(date_and_time);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;

            exitCase();
            break;

            //----- CASE getImageTime MMLD CMD - tested
         case SM_GET_IMAGE_TIME:
            imagePt.line = atof(param_array[0].c_str());
            imagePt.samp = atof(param_array[1].c_str());
            initCase();
            
            if(debugFlag)
            {
               cout
                  << endl
                  << "  vts - Calling getImageTime" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      line=" << imagePt.line << endl;
               cout << "      sample=" << imagePt.samp << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
			      time_count = rasterGM->getImageTime(imagePt);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      time=" << time_count << endl;
               cout << endl;
            }
            param_array_index = 2;
            maxReturnValuesToCompare = 1;
            act_val[0]=time_count;
            exitCase();
            break;

            //----- CASE getSensorPosition form 1 MMLD CMD - tested
         case SM_GET_SENSOR_POSITION1:
            imagePt.line = atof(param_array[0].c_str());
            imagePt.samp = atof(param_array[1].c_str());
            initCase();
            
            if(debugFlag)
            {
               cout
                  << endl
                  << "  vts - Calling getSensorPosition1" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      line=" << imagePt.line << endl;
               cout << "      sample=" << imagePt.samp << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                   sensorPos = rasterGM->getSensorPosition(imagePt);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      x,y,z=" << sensorPos.x  << "," << sensorPos.y << "," << sensorPos.z << endl;
               cout << endl;
            }
            param_array_index = 2;
            maxReturnValuesToCompare = 3;

            act_val[0]=sensorPos.x;
            act_val[1]=sensorPos.y;
            act_val[2]=sensorPos.z;
            exitCase();
            break;

            //----- CASE getSensorPosition form 2 MMLD CMD - tested
         case SM_GET_SENSOR_POSITION2:
            time_of_imaging = atof(param_array[0].c_str());
            initCase();
            
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getSensorPosition2" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      time_of_imaging=" << time_of_imaging << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                   sensorPos = rasterGM->getSensorPosition(time_of_imaging);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      x,y,z=" << sensorPos.x  << "," << sensorPos.y << "," << sensorPos.z << endl;
               cout << endl;
            }
            param_array_index = 1;
            maxReturnValuesToCompare = 3;

            act_val[0]=sensorPos.x;
            act_val[1]=sensorPos.y;
			act_val[2]=sensorPos.z;
            exitCase();
            break;

            //----- CASE getSensorVelocity form 1 MMLD CMD  - tested
         case SM_GET_SENSOR_VELOCITY1:
            imagePt.line = atof(param_array[0].c_str());
            imagePt.samp = atof(param_array[1].c_str());
            initCase();
            
            if(debugFlag)
            {
               cout << endl<< "  vts - Calling getSensorVelocity1"<< endl;
               cout << "    Passing values are: "<< endl;
               cout << "      line=" << imagePt.line << endl;
               cout << "      sample=" << imagePt.samp << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                   sensorVel = rasterGM->getSensorVelocity(imagePt);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      vx,vy,vz=" << sensorVel.x  << "," << sensorVel.y << "," << sensorVel.z << endl;
               cout << endl;
            }
            param_array_index = 2;
            maxReturnValuesToCompare = 3;
            act_val[0]=sensorVel.x;
            act_val[1]=sensorVel.y;
            act_val[2]=sensorVel.z;

            exitCase();
            break;

            //----- CASE getSensorVelocity form 2 MMLD CMD - tested
         case SM_GET_SENSOR_VELOCITY2:
            initCase();
            
            time_of_imaging = atof(param_array[0].c_str());

            if(debugFlag)
            {
               cout << endl
                  << "  vts - Calling getSensorVelocity2"
                  << endl;
               cout << "    Passing values are: "
                  << endl;
               cout << "      time_of_imaging="
                  << time_of_imaging << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                   sensorVel = rasterGM->getSensorVelocity(time_of_imaging);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: "
                  << endl;
               cout << "      vx,vy,vz=" << sensorVel.x  << "," << sensorVel.y << "," << sensorVel.z << endl;
               cout << endl;
            }
            param_array_index = 1;
            maxReturnValuesToCompare = 3;
            act_val[0]=sensorVel.x;
            act_val[1]=sensorVel.y;
            act_val[2]=sensorVel.z;
            exitCase();
            break;

         //----- CASE setParameter
         case SM_SET_PARAMETER:
         {
            initCase();
            
            index = atoi(param_array[0].c_str());
            string pname = param_array[1];
            double pvalue = atof(param_array[2].c_str());
            string punits = param_array[3];
            GeometricModel::Parameter p(pname,pvalue,punits,param::NONE,SharingCriteria());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling setParameter" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      index=" << index << endl;
               cout << "      pname=" << pname << endl;
               cout << "      pvalue=" << pvalue << endl;
               cout << "      punits=" << punits << endl;
            }
            for(i=0; i < repeat_count; i++)
            {
                _TRY
                   rasterGM->setParameter(index, p);
                _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      NONE" << endl;
               cout << endl;
            }
            exitCase();
            break;
		 }

         //----- CASE getParameters
         case SM_GET_PARAMETERS:
         {
            initCase();
            
            logCommand = false;
			vector<GeometricModel::Parameter> params;
            bool pSetEntered = false;
            param::Set pSet = param::VALID;   //set default
			if(param_array.size() > 0)
            {
               pSetEntered = true;
               if(param_array[0] == "ADJUSTABLE")
               {
                  pSet = param::ADJUSTABLE;
               }
               else if(param_array[0] == "FIXED")
               {
                  pSet = param::NON_ADJUSTABLE;
               }
            }
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getParameters" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      pSet=" << pSet << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  if(pSetEntered)
                  {
                     params = rasterGM->getParameters(pSet);
                  }
                  else
                  {
                     params = rasterGM->getParameters();
                  }
               _CATCH
            }
            end_clock = clock();

			vector<GeometricModel::Parameter>::const_iterator iter;
            stringstream str;
            for (iter = params.begin();iter != params.end();++iter)
            {
               str << "      pname=" << (*iter).name << ",";
               str << "      pval=" << (*iter).value << ",";
               str << "      punits=" << (*iter).units << endl;
            }
            str << endl;
            text = str.str();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << text << endl;
            }

            comment.clear();
            recordLog(logFile,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;
         }

         //----- CASE getParameterSetIndices
         case SM_GET_PARAMETER_SET_INDICES:
         {
            initCase();
            
            vector<int> indices;
            param::Set pSet = param::VALID;   //set default
            bool pSetEntered = false;
			if(param_array.size() > 0)
            {
               pSetEntered = true;
               if(param_array[0] == "ADJUSTABLE")
               {
                  pSet = param::ADJUSTABLE;
               }
               else if(param_array[0] == "FIXED")
               {
                  pSet = param::NON_ADJUSTABLE;
               }
            }
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getParameterSetIndices" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      pSet=" << pSet << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  if(pSetEntered)
                  {
                     indices = rasterGM->getParameterSetIndices(pSet);
                  }
                  else
                  {
                     indices = rasterGM->getParameterSetIndices();
                  }
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
            }
            int ii = 0;
			for (vector<int>::iterator iter=indices.begin();iter!=indices.end();iter++)
            {
               act_val[ii++] = *iter;
               if(debugFlag)
               {
                  cout << *iter << ",";
               }
			}
            if(debugFlag)
            {
               cout << endl;
            }

            param_array_index = 1;
            maxReturnValuesToCompare = ii;
            exitCase();
            break;
         }

         //----- CASE setParameterValue MMLD CMD - tested
         case SM_SET_PARAMETER_VALUE:
            initCase();
            
            index = atoi(param_array[0].c_str());
            value = atof(param_array[1].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling setParameterValue" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      index=" << index << endl;
               cout << "      value=" << value << endl;
            }
            for(i=0; i < repeat_count; i++)
            {
                _TRY
                   rasterGM->setParameterValue(index, value);
                _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      NONE" << endl;
               cout << endl;
            }
            exitCase();
            break;

            //----- CASE Calling getParameterValue MMLD CMD - tested
         case SM_GET_PARAMETER_VALUE:
            initCase();
            
            index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getParameterValue" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      index=" << index << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  return_double = rasterGM->getParameterValue(index);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      value=" << return_double << endl << endl;
            }
            param_array_index = 1;
            maxReturnValuesToCompare = 1;
            act_val[0] = return_double;

            exitCase();
            break;

            //----- CASE getParameterName MMLD CMD - tested
         case SM_GET_PARAMETER_NAME:
            initCase();
            
            logCommand = false;
            logStringCommand = true;
            index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getParameterName" << endl;
               cout << "    Passing values are:" << endl;
               cout << "      index=" << index << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  paramName = rasterGM->getParameterName(index);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      name = " << paramName << endl;
               cout << endl;
            }
            act_val_string.push_back(paramName);

            param_array_index = 1;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;

            //----- CASE getNumParameters MMLD CMD - tested
         case SM_GET_NUM_PARAMETERS:
            initCase();
            
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getNumParameters" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
			      ret_int = rasterGM->getNumParameters();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      value=" << ret_int << endl << endl;
            }
            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            act_val[0]=(double)ret_int;

            exitCase();
            break;


         //----- CASE setParameterType MMLD
         case SM_SET_PARAMETER_TYPE:
            initCase();
            
            index = atoi(param_array[0].c_str());
            parameterType = (param::Type)atoi(param_array[1].c_str());

            if(debugFlag)
            {
               cout << endl<< "  vts - Calling setParameterType" << endl;
               cout << "    Passing values are:" << endl;
               cout << "      index=" << index << endl;
               cout << "      parameterType=" << parameterType << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  rasterGM->setParameterType(index, parameterType);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      NONE" << endl;
            }
            exitCase();
            break;

         //----- CASE getParameterType MMLD
         case SM_GET_PARAMETER_TYPE:
         {
            initCase();
            
            csm::param::Type return_parameterType = csm::param::NONE;
            index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getParameterType" << endl;
               cout << "    Passing values are:" << endl;
               cout << "      index=" << index << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  return_parameterType = rasterGM->getParameterType(index);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      parameterType=" << return_parameterType << endl;
            }
            param_array_index = 1;
            maxReturnValuesToCompare = 1;
            act_val[0]=return_parameterType;
            exitCase();
            break;
         }
            //----- CASE getPedigree  MMLD CMD - tested
         case SM_GET_PEDIGREE:
         {
            string pedigree;
            logCommand = false;
            logStringCommand = true;
            initCase();
            
            if(debugFlag)
            {
               cout << endl
                         << "  vts - Calling getPedigree" << endl;
               cout << "    Passing values are:" << endl;
               cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
				  pedigree = rasterGM->getPedigree();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
            cout << "    vts - Returned values are: " << endl;
            cout << "      pedigree=" << pedigree << endl;
            cout << endl;
            }

            act_val_string.push_back(pedigree);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }
         //----- CASE getImageIdentifier MMLD CMD - tested
         case SM_GET_IMAGE_IDENTIFIER:
         {
            logCommand = false;
            logStringCommand = true;
            initCase();
            
            if(debugFlag)
            {
            cout << endl
               << "  vts - Calling getImageIdentifier"
               << endl;
            cout << "    Passing values are:" << endl;
            cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
                _TRY
                   imageId = rasterGM->getImageIdentifier();
                _CATCH
            }

            end_clock = clock();

            if(debugFlag)
            {
            cout << "    vts - Returned values are: " << endl;
            cout << "      imageId=" << imageId << endl;
            cout << endl;
            }
            act_val_string.push_back(imageId);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }
         //----- CASE setImageIdentifier MMLD CMD - tested
         case SM_SET_IMAGE_IDENTIFIER:
			imageId = param_array[0];
            initCase();
            
            if(debugFlag)
            {
				cout << endl
					      << "  vts - Calling setImageIdentifier" << endl;
				cout << "    Passing values are:" << endl;
				cout << "      imageId=" << imageId << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
				   rasterGM->setImageIdentifier(imageId, &warnings);
               _CATCH
               if (warnings.size() > 0 || error_thrown) break;
            }
            end_clock = clock();

            if(!error_thrown)
            {
				if(debugFlag)
				{
					cout << "    vts - Returned values are: " << endl;
					cout << "      NONE" << endl;
					cout << endl;
				}
			}
            exitCase();
            break;

            //----- CASE getSensorIdentifier  MMLD CMD - tested
         case SM_GET_SENSOR_IDENTIFIER:
         {
            string sensorId;
            logCommand = false;
            logStringCommand = true;
            initCase();
            
            if(debugFlag)
            {
            cout << endl
                      << "  vts - Calling getSensorIdentifier" << endl;
            cout << "    Passing values are:" << endl;
            cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
                _TRY
                   sensorId = rasterGM->getSensorIdentifier();
                _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
            cout << "    vts - Returned values are: " << endl;
            cout << "      sensorId=" << sensorId << endl;
            cout << endl;
            }
            act_val_string.push_back(sensorId);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }
         //----- CASE getPlatformIdentifier  MMLD CMD - tested
         case SM_GET_PLATFORM_IDENTIFIER:
         {
            string platformId;
            // Calling getPlatformIdentifier.
            logCommand = false;
            logStringCommand = true;
            initCase();
            
            if(debugFlag)
            {
            cout << endl
               << "  vts - Calling getPlatformIdentifier"
               << endl;
            cout << "    Passing values are:" << endl;
            cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
                _TRY
                   platformId =  rasterGM->getPlatformIdentifier();
                _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
            cout << "    vts - Returned values are: " << endl;
            cout << "      platformId=" << platformId << endl;
            cout << endl;
            }
            act_val_string.push_back(platformId);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }
         //----- CASE getReferencePoint MMLD CMD - tested
         case SM_GET_REFERENCE_POINT:
            initCase();
            
            if(debugFlag)
            {
				cout << endl
          				  << "  vts - Calling getReferencePoint" << endl;
				cout << "    Passing values are: " << endl;
				cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  groundPt = rasterGM->getReferencePoint();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are:\n";
               cout << "      x,y,z=" << groundPt.x  << "," << groundPt.y << "," << groundPt.z << endl;
			   cout << endl;
            }
            param_array_index = 0;
            maxReturnValuesToCompare = 3;
            act_val[0]=groundPt.x;
            act_val[1]=groundPt.y;
            act_val[2]=groundPt.z;
            exitCase();
            break;

            //----- CASE setReferencePoint MMLD CMD - tested
         case SM_SET_REFERENCE_POINT:
            initCase();
            
            groundPt.x = atof(param_array[0].c_str());
            groundPt.y = atof(param_array[1].c_str());
            groundPt.z = atof(param_array[2].c_str());

            if(debugFlag)
            {
				cout << endl << "  vts - Calling setReferencePoint" << endl;
				cout << "    Passing values are:" << endl;
				cout << "      x,y,z=" << groundPt.x  << "," << groundPt.y << "," << groundPt.z << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
                _TRY
				   rasterGM->setReferencePoint(groundPt);
                _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
				cout << "    vts - Returned values are:" << endl;
				cout << "      NONE" << endl;
				cout << endl;
            }

            param_array_index = 3;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;


            //----- CASE getImageStart - tested
         case SM_GET_IMAGE_START:
         {
            initCase();
            
            double line;
            double samp;
            if(debugFlag)
            {
				cout << endl
					      << "  vts - Calling getImageStart" << endl;
				cout << "    Passing values are:" << endl;
				cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
			      ImageCoord ic = rasterGM->getImageStart();
                  line = ic.line;
                  samp = ic.samp;
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
				cout << "    vts - Returned values are:\n ";
				cout << "      line=" << line << endl;
				cout << "      samp=" << samp << endl;
				cout << endl;
            }
            param_array_index = 0;
            maxReturnValuesToCompare = 2;
            act_val[0]=line;
            act_val[1]=samp;
            exitCase();
            break;
         }

         //----- CASE getImageSize  MMLD CMD - tested
         case SM_GET_IMAGE_SIZE:
         {
            initCase();
            
            double num_lines;
            double num_samples;
            if(debugFlag)
            {
				cout << endl
					      << "  vts - Calling getImageSize" << endl;
				cout << "    Passing values are:" << endl;
				cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  ImageVector iv = rasterGM->getImageSize();
                  num_lines = iv.line;
                  num_samples = iv.samp;
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
				cout << "    vts - Returned values are:\n ";
				cout << "      num_lines=" << num_lines << endl;
				cout << "      num_samples=" << num_samples << endl;
				cout << endl;
            }
            param_array_index = 0;
            maxReturnValuesToCompare = 2;
            act_val[0]=num_lines;
            act_val[1]=num_samples;
            exitCase();
            break;
         }
         //----- CASE getValidHeightRange - tested
         case SM_GET_VALID_HEIGHT_RANGE:
            initCase();
            
            if(debugFlag)
            {
				cout << endl << "  vts - Calling getValidHeightRange" << endl;
				cout << "    Passing values are: " << endl;
				cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
			      heightRange = rasterGM->getValidHeightRange();
			      minHeight = heightRange.first;
			      maxHeight = heightRange.second;
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
            cout << "    vts - Returned values are: " << endl;
            cout << "      minHeight=" << minHeight << endl;
            cout << "      maxHeight=" << maxHeight << endl;
            cout << endl;
            }
            param_array_index = 0;
            maxReturnValuesToCompare = 2;
            act_val[0]=minHeight;
            act_val[1]=maxHeight;
            exitCase();
            break;

            //----- CASE getValidImageRange - tested
         case SM_GET_VALID_IMAGE_RANGE:
            initCase();
            
            if(debugFlag)
            {
				cout << endl << "  vts - Calling getValidImageRange" << endl;
				cout << "    Passing values are: "<< endl;
				cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
				  imageRange = rasterGM->getValidImageRange();
				  ImageCoord minIC = imageRange.first;
				  ImageCoord maxIC = imageRange.second;
				  minRow = minIC.line;
				  minCol = minIC.samp;
				  maxRow = maxIC.line;
				  maxCol = maxIC.samp;
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
				cout << "    vts - Returned values are:\n";
				cout << "      minRow=" << minRow << endl;
				cout << "      maxRow=" << maxRow << endl;
				cout << "      minCol=" << minCol << endl;
				cout << "      maxCol=" << maxCol << endl << endl;
            }
            param_array_index = 0;
            maxReturnValuesToCompare = 4;
            act_val[0]=minRow;
            act_val[1]=maxRow;
            act_val[2]=minCol;
            act_val[3]=maxCol;
            exitCase();
            break;

         //----- CASE getIlluminationDirection - tested
         case SM_GET_ILLUMINATION_DIRECTION:
            initCase();
            
            groundPt.x = atof(param_array[0].c_str());
            groundPt.y = atof(param_array[1].c_str());
            groundPt.z = atof(param_array[2].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getIlluminationDirection" << endl;
			   cout << "    Passing values are:" << endl;
			   cout << "      x,y,z=" << groundPt.x  << "," << groundPt.y << "," << groundPt.z << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  illumDir = rasterGM->getIlluminationDirection(groundPt);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
				cout << "    vts - Returned values are: " << endl;
				cout << "      direction_x=" << illumDir.x << endl;
				cout << "      direction_y=" << illumDir.y << endl;
				cout << "      direction_z=" << illumDir.z << endl;
				cout << endl;
            }
            param_array_index = 3;        //location of first command line switch
            maxReturnValuesToCompare = 3; // Maximum number of comparisons switches
            act_val[0] = illumDir.x;
            act_val[1] = illumDir.y;
            act_val[2] = illumDir.z;
            exitCase();
            break;

         
         //----- CASE getNumGeometricCorrectionSwitches - tested
         case SM_GET_NUM_GEOMETRIC_CORRECTION_SWITCHES:
            initCase();
            
            if(debugFlag)
            {
               cout << endl
                    << "  vts - Calling StubSensorModel::getNumGeometricCorrectionSwitches" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  ret_int = rasterGM->getNumGeometricCorrectionSwitches();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned value is: " << endl;
               cout << "      value=" << ret_int << endl;
               cout << endl;
            }
            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            act_val[0]=(double)ret_int;
            exitCase();

            break;

            //-----  CASE getGeometricCorrectionName Nsetup - tested
         case SM_GET_GEOMETRIC_CORRECTION_NAME:
            logCommand = false;
            logStringCommand = true;
            initCase();
            
            index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling StubSensorModel::getGeometricCorrectionName" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      index=" << index << endl;
            }

            for(i=0; i < repeat_count; i++) 
			{
               _TRY
                  errorName = rasterGM->getGeometricCorrectionName(index);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      errorName=" << errorName << endl << endl;
            }
            act_val_string.push_back(errorName);

            param_array_index = 1;
            maxReturnValuesToCompare = 1;
            exitCase();

            break;

             //-- CASE setGeometricCorrectionSwitch - tested
         case SM_SET_GEOMETRIC_CORRECTION_SWITCH:
            initCase();
            
            index = atoi(param_array[0].c_str());
            if (atoi(param_array[1].c_str()) == 0)
               error_switch = false;
            else
               error_switch = true;
            parameterType = (param::Type)atoi(param_array[2].c_str());

            if(debugFlag)
            {
               cout << endl << "  vts - Calling setGeometricCorrectionSwitch" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      index=" << index << endl;
               cout << "      error_switch=" << error_switch << endl;
               cout << "      parameterType=" << parameterType << endl;
            }

            for(i=0; i < repeat_count; i++) 
			{
               _TRY
				  rasterGM->setGeometricCorrectionSwitch(index,error_switch,parameterType);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      NONE" << endl << endl;
            }

            param_array_index = 2;
            maxReturnValuesToCompare = 0;

            exitCase();
            break;

            //----- CASE getGeometricCorrectionSwitch - tested
         case SM_GET_GEOMETRIC_CORRECTION_SWITCH:
            initCase();
            
            index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               cout << endl
                    << "  vts - Calling StubSensorModel::getGeometricCorrectionSwitch" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      index=" << index << endl;
            }

            for (i=0; i < repeat_count; i++) 
			{
               _TRY
				  return_bool1 = rasterGM->getGeometricCorrectionSwitch(index);
               _CATCH
			}
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      value=" << return_bool1 << endl << endl;
            }

            param_array_index = 1;
            maxReturnValuesToCompare = 1;
            act_val[0]=(double)return_bool1;

            exitCase();

            break;

		 //----- CASE  getModelState - tested
         case SM_GET_MODEL_STATE:
            logCommand = false;
            initCase();
            
            if(debugFlag)
            {
				cout << endl
			              << "  vts - Calling getModelState" << endl;
				cout << "    Passing values are: "<< endl;
				cout << "      NONE" << endl;
			}

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  returned_sensor_model_state = rasterGM->getModelState();
               _CATCH
            }
            end_clock = clock();
            if (returned_sensor_model_state != "")
            {
               text = "Saved -  Returned State:  " + returned_sensor_model_state;
               sensorModelStateSaved = true;
            }
            else
            {
               text = "Not Saved";
               sensorModelStateSaved = false;
            }

            if(debugFlag)
            {
				cout << "    vts - Returned values are:\n";
				cout << "      SensorModelState=" << returned_sensor_model_state << endl;
				cout << endl;
            }

            comment.clear();
            recordLog(logFile,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

		 //----- CASE replaceModelState 
         case SM_REPLACE_MODEL_STATE:
         {
            logCommand = false;
            initCase();
            
            string state = "";
            if(param_array.size() > 0)
            {
               state = param_array[0];
            }
			else
            {
               state = returned_sensor_model_state;
            }

            if(debugFlag)
            {
               cout << endl << "  vts - Calling " << "replaceModelState" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      state="  << state << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
				   rasterGM->replaceModelState(state);
               _CATCH
            }
            end_clock = clock();

			if(debugFlag)
            {
				cout << "    vts - Returned values are:\n";
				cout << "      SensorModelState=" << returned_sensor_model_state << endl;
				cout << endl;
            }
            text.clear();

            comment.clear();
            recordLog(logFile,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;
         }                              // end case

		 //----- CASE getVersion - tested
         case SM_GET_VERSION:
         {
			string verStr;
            logCommand = false;
            logStringCommand = true;
            initCase();
            
            if(debugFlag)
            {
				cout << endl << "  vts - getVersion" << endl;
				cout << "    Passing values are:" << endl;
				cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
				  Version version = rasterGM->getVersion();
				  stringstream str;
				  str << version;
				  verStr = str.str();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
				cout << "    vts - Returned values are: " << endl;
				cout << "        version=" << verStr << endl << endl;
            }
            act_val_string.push_back(verStr);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }                              // end case

         //----- CASE getModelName2 - Tested
         case SM_GET_MODEL_NAME2:
         {
            logCommand = false;
            logStringCommand = true;
            initCase();
            
            if(debugFlag)
            {
				cout << endl
                     << "  vts - Calling getModelName2" << endl;
				cout << "    Passing values are:" << endl;
				cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
                  smName = rasterGM->getModelName();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
				cout << "    vts - Returned values are: " << endl;
				cout << "        sensor_model_name=" << smName << endl;
				cout << endl;
            }
            act_val_string.push_back(smName);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }                              // end case

         //----- CASE getFamily - tested
         case SM_GET_FAMILY:
         {
            string family;
            logCommand = false;
            initCase();
            
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getFamily" << endl;
               cout << "    Passing values are: " << endl;
               cout << "        None" << endl;
            }

            for(i = 0; i < repeat_count; i++)
            {
               _TRY
				  family = rasterGM->getFamily();
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "        Family=" << family << endl << endl;
            }

            comment.clear();
            text = family;
            recordLog(logFile,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;
         }

         //----- CASE isParameterShareable - tested
         case SM_IS_PARAMETER_SHAREABLE:
            index = atoi(param_array[0].c_str());
            initCase();
            
            if(debugFlag)
            {
               cout << endl << "  vts - Calling isParameterShareable" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      index=" << index << endl;
            }

            for (i=0; i < repeat_count; i++)
            {
               _TRY
                  return_bool1 = rasterGM->isParameterShareable(index);
               _CATCH
            }
            end_clock = clock();

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      value=" << return_bool1 << endl << endl;
            }
            param_array_index = 1;
            maxReturnValuesToCompare = 1;
            act_val[0]=(double)return_bool1;
            exitCase();
            break;

         //----- CASE getParameterUnits 
         case SM_GET_PARAMETER_UNITS:
         {
            logCommand = false;
            logStringCommand = true;
            initCase();
            
            index = atoi(param_array[0].c_str());
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getParameterUnits" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      index=" << index << endl;
            }

			string units;
            for (i=0; i < repeat_count; i++)
            {
               _TRY
                  units = rasterGM->getParameterUnits(index);
               _CATCH
            }
            end_clock = clock();
            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      units=" << units << endl << endl;
            }

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            act_val_string.push_back(units);
			
			exitCase();
            break;
         }

         //----- CASE hasShareableParameters
         case SM_HAS_SHAREABLE_PARAMETERS:
         {
            initCase();
            
            if(debugFlag)
            {
               cout << endl << "  vts - Calling hasShareableParameters" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      None" << endl;
            }

			bool hasSharableParams;
            for (i=0; i < repeat_count; i++)
            {
               _TRY
                  hasSharableParams = rasterGM->hasShareableParameters();
               _CATCH
            }
            end_clock = clock();
            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      hasSharableParams=" << hasSharableParams << endl << endl;
            }

            if (hasSharableParams)
            {
               text = "has Shareable Parameters";
            }
            else
            {
               text = "has no Shareable Parameters";
            }
            comment.clear();
            recordLog(logFile,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;
			
			exitCase();
            break;
         }

         //----- CASE getParameter
         case SM_GET_PARAMETER:
         {
            logCommand = false;
            logStringCommand = true;
            initCase();
            
            index = atoi(param_array[0].c_str());
			GeometricModel::Parameter p;

            if(debugFlag)
            {
               cout << endl << "  vts - Calling getParameter" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      index=" << index << endl;
            }

            for (i=0; i < repeat_count; i++)
            {
               _TRY
                  p = rasterGM->getParameter( index);
               _CATCH
            }
            end_clock = clock();
			text = "";

            if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << "      paramName=" << p.name << endl;
               cout << endl;
            }

            act_val_string.push_back(p.name);
            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }

         //----- CASE getParameterSharingCriteria
         case SM_GET_PARAMETER_SHARING_CRITERIA:
         {
            index = atoi(param_array[0].c_str());
			csm::SharingCriteria criteria;
            initCase();
            
            if(debugFlag)
            {
               cout << endl << "  vts - Calling getParameterSharingCriteria" << endl;
               cout << "    Passing values are: " << endl;
               cout << "      index=" << index << endl;
            }

            for (i=0; i < repeat_count; i++)
            {
               _TRY
                  criteria = rasterGM->getParameterSharingCriteria( index);
               _CATCH
            }
            end_clock = clock();
			text = "";
		    int v = 0;
            act_val[v++]=(double)criteria.matchesName;
            sprintf (tempStr, "type=%d, ", criteria.matchesName);
            text += tempStr;

			act_val[v++]=(double)criteria.matchesSensorID;
            sprintf (tempStr, "type=%d, ", criteria.matchesSensorID);
            text += tempStr;

            act_val[v++]=(double)criteria.matchesPlatformID;
            sprintf (tempStr, "type=%d, ", criteria.matchesPlatformID);
            text += tempStr;

            act_val[v++]=(double)criteria.matchesCollectionID;
            sprintf (tempStr, "type=%d, ", criteria.matchesCollectionID);
            text += tempStr;

            act_val[v++]=(double)criteria.matchesTrajectoryID;
            sprintf (tempStr, "type=%d, ", criteria.matchesTrajectoryID);
            text += tempStr;

            act_val[v++]=(double)criteria.matchesDateTime;
            sprintf (tempStr, "type=%d ", criteria.matchesDateTime);
            text += tempStr;

			if(criteria.matchesDateTime)
			{
                sprintf (tempStr, ", maxTimeDelta=%g ", criteria.maxTimeDelta);
                text += tempStr;
				act_val[v++]=criteria.maxTimeDelta;
			}

			
			text += "\n";

			if(debugFlag)
            {
               cout << "    vts - Returned values are: " << endl;
               cout << text << endl;
            }

            param_array_index = 1;
            maxReturnValuesToCompare = v;
			
			exitCase();
            break;
         }
		 default:
            command_found = false;
      }                                 // switch
   }                                    ////// if constructModelDone section CMD

   return command_found;
}

///////////////////////////////////////////////////////
//
//   Function: processSensorModelStateCommand
//
//   Description: process the inputted command
//
//   Input: commandNumber - Enumeration of the inputted
//                          command
//          param_array   - Argument to the command
//
//   Return Value:
//          bool - Command Found
//
///////////////////////////////////////////////////////
bool processSensorModelStateCommand(int commandNumber,
                       vector<string> param_array)
{
   int i;                              //----- Loop Variable
   bool command_found = true;

   bool constructable = false;         //----- Model may be constructable

   if (!sensorModelStateSaved || stubPlugin == NULL)
   {
      if(sensorModelStateSaved)
         cout << "Sensor Model State Not Saved"
            << endl;
      else
         cout << NO_MODEL_LIST_ERROR << endl;
   }
   else
   {
      command_found = true;
      switch (commandNumber)
      {
         //----- CASE canModelBeConstructedFromState SMSS MMLD - tested
         case SP_CAN_MODEL_BE_CONSTRUCTED_FROM_STATE:
         {
            logCommand = false;
            initCase();
            
            if(debugFlag)
            {
            cout << endl
               << "  vts - Calling "
               << "canModelBeConstructedFromState"
               << endl;
            cout << "    Passing values are: " << endl;
            cout << "      Plugin="  << param_array[0]
               << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
				   constructable = stubPlugin->canModelBeConstructedFromState(
					   param_array[0],
					   returned_sensor_model_state,
					   &warnings);
               _CATCH
               if (warnings.size() > 0 || error_thrown) break;
            }
            end_clock = clock();
            text.clear();
            cout
               << "Plugin::canModelBeConstructedFromState";

            if (constructable)
            {
               cout << " can possibly be constructed from "
                  << param_array[0] << endl;
               text = "Possibly Constructable";
            }
            else
            {
               cout << " can not be constructed from "
                  << param_array[0] << endl;
               text = "NOT Constructable";
            }
            comment.clear();
            recordLog(logFile,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;
         }                              // end case

         //----- CASE constructModelFromState SMSS MMLD - tested
         case SP_CONSTRUCT_MODEL_FROM_STATE:
            logCommand = false;
            initCase();
            
            if(debugFlag)
            {
            cout << endl
               << "  vts - Calling "
               << "constructModelFromState"
               << endl;
            cout << "    Passing values are: " << endl;
            cout << "      NONE" << endl;
            }

            for(i=0; i < repeat_count; i++)
            {
               _TRY
				   rasterGM = (RasterGM*)stubPlugin->constructModelFromState(
					   returned_sensor_model_state,
					   &warnings);
               _CATCH
               if (warnings.size() > 0 || error_thrown) break;
            }
            end_clock = clock();
            if (!rasterGM)
            {
				Warning wn(Warning::UNKNOWN_WARNING,
					"Sensor Model Not Constructed",
					"VTS::processCommand");
				warnings.push_back(wn);

				text = "Not Constructed";
            }
            else
            {
               text = "Constructed";
            }
            comment.clear();
            recordLog(logFile,
                  comment,
                  command,
                  text);

            param_array_index = 0;
            maxReturnValuesToCompare = 0;

            exitCase();
            break;
         default:
            command_found = false;
      }                                 // end of switch
   }                                    // end of sensormodelstatesaved

   return command_found;
}





///////////////////////////////////////////////////////
//
//   Function: processCommand
//
//   Description: process the inputted command
//
//   Input: commandNumber - Enumeration of the inputted
//                          command
//          param_array   - Argument to the command
//
//   Return Value:
//          Nothing
//
///////////////////////////////////////////////////////
void processCommand(int commandNumber,
                    vector<string> param_array)
{
   int i;                               // Loop Variable

   static bool saveLogFile = true;            //----- Save Log to a File Flag
   static string returned_sensor_model_state = "";

   char tempStr[TEMP_STRING_LENGTH] = "";

   //int target_model_version = 0;

   bool command_found = false;

   logCommand = true;              // Use the default log command
   logStringCommand = false;       // Use the default string log command

   // Initialize compare parameters
   comp_answer.clear();
   pass_fail.clear();
   act_val.clear();
   act_val_string.clear();
   text.clear();
   comment.clear();
   param_array_index = 1;
   maxReturnValuesToCompare = 0;
   for(i=0; i<MAXOUTPUTPARAMETERS; i++)
   {
      comp_answer[1] = 0.0;
      pass_fail[i] = true;
      act_val[1] = 0.0;
   }

   if (commandNumber >= BEGIN_SP_NUMBER && commandNumber <= END_SP_NUMBER)
      cout << "\ncommand number SP-" << commandNumber << endl;
   else if (commandNumber >= BEGIN_SM_NUMBER && commandNumber <= END_SM_NUMBER)
      cout << "\ncommand number SM-" << commandNumber << endl;
   else
      cout << "\ncommand number VTS-" << commandNumber << endl;

   switch( menutype[commandNumber] )
   {
      case VTS_TYPE:
         command_found = processVTSCommand(commandNumber, param_array);
         break;
      case FILE_TYPE:
         command_found = processFileCommand(commandNumber, param_array);
         break;
      case SENSOR_PLUGIN_TYPE:
         command_found = processSensorPluginCommand(commandNumber, param_array);
         break;
      case SENSOR_MODEL_TYPE:
         command_found = processSensorModelCommand(commandNumber, param_array);
         break;
      case SENSOR_MODEL_STATE_TYPE:
         command_found = processSensorModelStateCommand(commandNumber,
                                                        param_array);
         break;
      default:
         command_found = false;
   }

   if(!command_found)
      cout << "Unable to locate command "
         << menulist[commandNumber] << endl;

   if (command_found && (stubPlugin != NULL ))
   {
      if(logCommand || logStringCommand)
      {
         // Check if there are enough command line parameters to
         // request a comparison
         //
         if(param_array_index >= (int)param_array.size())
         {
            // Not enough parameters to do a comparison

            if(debugFlag)
            {
            cout << endl
               << "vts>    **** No parameter comparison switches"
               << endl;
            }
         }
         else
         {
            if(debugFlag)
            {
            cout << endl
               << "vts>    **** Checking for parameter comparison switches "
               << i << endl;
            // print the actual values for debug
            if(logStringCommand)
            {
               for (i = 0; i < (int)act_val_string.size(); i++)
                  cout << "act_val_string[" << i << "] = "
                     << act_val_string[i] << endl;
            }
            else
            {
               for (i = 0; i < (int)act_val.size(); i++)
                  cout << "act_val[" << i << "] = "
                     << act_val[i] << endl;
            }
            }

            if(logCommand)
               compareParam(param_array, act_val, comp_answer, pass_fail);
            else
               compareParam(param_array, act_val_string, pass_fail);

            if(debugFlag)
            {
               if (!error_thrown)
               {
                  printf("\n  Return to mainline ");
                  if(logStringCommand)
                  {
                     printf("\n     act_val_string");
                     for (i = 0; i<(int)act_val_string.size(); i++)
                        printf(" %s", act_val_string[i].c_str());
                  }
                  else
                  {
                     printf("\n     act_val");
                     for (i = 0; i<(int)act_val.size(); i++)
                        printf(" %14.4f", act_val[i]);
                     printf("\n comp_answer");
                     for (i = 0; i<(int)comp_answer.size(); i++)
                        printf(" %14.4f", comp_answer[i]);
                  }
                  printf("\n   pass_fail");
                  for (i = 0; i<(int)pass_fail.size(); i++)
                     printf(" %14d", (int)pass_fail[i]);
               }
            }
         }                              // if param_array_index ...

         comment.clear();
         vector< string > out_param_array;

         for (i = 0 ; i <  maxReturnValuesToCompare ; i++)
         {
            out_param_array.push_back("");
            out_param_array.push_back("");
            out_param_array.push_back("");

            if (!error_thrown)
            {
               if(logCommand)
               {
                  sprintf (tempStr, "%f", act_val[i]);
                  out_param_array[i*3] = tempStr;

                  sprintf (tempStr, "%f", comp_answer[i]);
                  out_param_array[i*3+1] = tempStr;
               }
               else
               {
                  out_param_array[i*3] = act_val_string[i];
               }

               if ( i < (int)pass_fail.size() && pass_fail[i] == false )
                  out_param_array[i*3+2] = "Fail";
               else
                  out_param_array[i*3+2] = "Pass";
            }
            else
            {
               out_param_array[i*3+2] = "N/A ";
            }
         }

         recordLog(logFile,
               comment,
               command,
               param_array,
               out_param_array);
      }                                 // End of if(logCommand)
   }                                    //  end check if makeModelListFromISD

   //---
   // Checking for Warning other than NO_WARNING
   //---
   if (warnings.size()>0)
   {
      if(saveLogFile)
      {
         reportWarning(warnings, logFile);
      }
	  warnings.clear();
   }

   if (error_thrown)
   {
      if(saveLogFile)
      {
         reportError(&global_err, logFile);
      }

      error_thrown = false;             // clear error indicator flag
   }
}

//string getStringParamType(param::Type pt) 
//{   
//   switch pt   
//   {   
//   case NONE: return "NONE";   
//   case FICTITIOUS: return "FICTITIOUS";   
//   case REAL: return "REAL";
//   case EXACT: return "EXACT";
//   default: return("Bad ParamType Value");   
//   } 
//} 


//*****************************************************************************
// main
//*****************************************************************************
int main(int argc, char** argv)
{
   string dirName;
   #ifdef _WIN32
   dirName = ".\\";
   logFile  =".\\vts_logfile.csv";
   #else
   dirName = "./";
   logFile ="./vts_logfile.csv";
   #endif

   #ifdef _DEBUG
      cout << "***** This code was compiled in DEBUG mode. *****"
                << endl;
   #endif

   int i = 0,j;
   string text = "";

   cout << endl;
   cout << "**********************************************"  << endl;
   cout << "**                                          **"  << endl;
   cout << "**        VTS version "   << VTS_VERSION;
   for(i=0;i<22-(int)VTS_VERSION.length();i++)
      cout << " ";
   cout << "**"  << endl;
   cout << "**                                          **"  << endl;
   cout << "**********************************************"  << endl;
   cout << endl;

   //---
   // Initialize the clock to record system clock tics from this point.
   //---

   initMenuList();
   vHelpDataValues = loadHelpFile();
   time_t aclock;
   aclock = time(NULL);

   cout << endl
      << "Program Start Time = " << ctime(&aclock)
      << endl;

   cout << endl;
   cout
      << "For this computer the number of tics-per-second (CLOCKS_PER_SEC) = "
      << CLOCKS_PER_SEC << endl;
   cout << endl;

   // Setup the input file
   std::istream* pInput = nullptr;
   if (argc != 2) // only support a single command-line argument, the input file
   {
       // Assume *nix-style stdin
       pInput = &std::cin;
   }
   else
   {
       if (*(argv[1]) == '-') // filename"is '-', which is stdin
       {
           pInput = &std::cin;
       }
       else
       {
           // Redirection from stdin is common on *nix, less-so on Windows
           static std::ifstream s(argv[1]); // note "static", stays in-scope
           pInput = &s;
       }
   }
   assert(pInput != nullptr);
   std::istream& cin = *pInput;

   //---
   // Select the log file.
   //---
   // get logfile name
   string inputFile;
   getLogfileName(dirName, cin, inputFile);
   if(inputFile != "")
      logFile = inputFile;

   cout << "Output logged to file " << logFile << endl;

   #ifdef _WIN32
   for (string::iterator pos = dirName.begin();
        pos != dirName.end();
        pos ++)
      if (*pos == '/')
         *pos = '\\';
   #else
   for (string::iterator pos = dirName.begin();
        pos != dirName.end();
        pos ++)
      if (*pos == '\\')
         *pos = '/';
   #endif

   //---
   // Write a heading to the log file.
   //---
   text.assign ("CSM Verification Test System " + VTS_VERSION);
   recordLog (logFile, text);
   text.assign ("Program Start Time = " +  string(ctime(&aclock)));
   recordLog (logFile, text);

   ostringstream stext;
   stext << "For this computer the number of tics-per-second (CLOCKS_PER_SEC) = "
      <<  CLOCKS_PER_SEC << '\n';
   recordLog(logFile, stext.str());

   //  MAXINPUTPARAMETERS, MAXOUTPUTPARAMETERS are the number of param
   //  not to format the line to

   text.assign ("Start,End, Serial,,,");
   for(i=1; i<=MAXINPUTPARAMETERS;i++)
      text +=  "In,";

   for(i=1; i<=MAXINPUTPARAMETERS-1;  i++ )
   {
      char buf[5] = "";
      sprintf (buf, "%u", i);
      text += string(",Out ") + buf + string(",,");
   }
   recordLog(logFile, text);

   text.assign ("Time,Time,Number,Comment,Command Name, ");

   for(i=1; i<=MAXINPUTPARAMETERS;i++)
   {
      char buf[10] = "";
      sprintf(buf, "%u,",  i);
      text +=  buf;
   }

   for(i=1; i<= MAXINPUTPARAMETERS-1; i++ )
   {
      text += "Rtn,Dif,P/F,";
   }
   recordLog(logFile, text);

   termEcho=2;                          // Echo returns only to the terminal

   //---
   // Part 1: This calls the SMManager's loadLibraries function, which
   // ultimately results in all CSM plugin factories to self-register into a
   // list of CSM factories.
   //
   // Then, the printList method (defined below) is used to print the list of
   // all registered CSM factories. Note that only plugins are registered in
   // this list; if any other shared objects/DLLs are found in this directory,
   // then they are ignored for purposes of populating the list.
   //---

   SMManager::instance().loadLibraries(dirName.c_str());

   // printList(logFile);

   /////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////
   ////////////////////// SETUP FOR MAIN LOOP //////////////////////////
   /////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////
   // first switch (-) is to be found
   // comparisons between return value
   // and desired value.
   // be used. Check TBD.

   //                    Array containing the calling parameters

   if(debugFlag)
   {
   cout << "Done with initialization!" << endl << endl;
   }

   cout << endl;              // Add a newline to make the vts prompt
   // look better.

   ///////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////
   /////////////////  Beginning of 'endless' for-loop (event loop)
   /////////////////  This loop polls stdin waiting for a command.
   /////////////////  When it gets one it parses it and steps to the
   /////////////////  appropriate command case in the switch
   /////////////////  statement.
   ///////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////

   while (true)
   {
      string targetCommand;
      vector<string> commandList;
      vector<string> param_array;
      int commandNumber;

      // get command line and parse it
      // targetCommand is the command line
      // subString is the token returned by parser
      cout << "\nvts> ";

      targetCommand = readDataLine(cin);
	  trim(targetCommand);

      if(debugFlag)
      {
      cout << "targetCommand: " << targetCommand << endl;
      }

      // Get the first parameter
      // ignore comments
      if (targetCommand[0] == '#')
         continue;
      // and empty lines
      if (targetCommand.empty())
         continue;

      commandList = getCommands(targetCommand);

      command = commandList[0];
      commandNumber = getCommandIndex(command);
      param_array.clear();

      if(commandNumber < MAX_NUMBER_COMMANDS)
      {
         for(j=1;j<(int)commandList.size();j++)
            param_array.push_back(commandList[j]);

         processCommand(commandNumber, param_array);
      }
      else
      {  // command not found
         stringstream str;
         str << endl
            << "vts>    **** Can not find the routine you request "
            << "in my list: " << command << endl;
         string msg = str.str();
         recordLog (logFile, msg);
      }
   }
}
