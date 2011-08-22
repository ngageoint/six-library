//#############################################################################
//
//    FILENAME:   vts.cpp
//
//
//    DESCRIPTION:
//
//    This application is a test driver for the Community Sensor Model (CSM),
//    formerly Tactical Sensor Model (TSM) plugin manager, SMManager.
//
//    This version's primary function is to cause the TSM plugins to be loaded,
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
//                       Date          Author      Comment    
//    SOFTWARE HISTORY:  13-Jan-2004   LMT         Initial version.
//                       22 Jul 2010   Don Leonard CCB Change add vtsWriteStateToFile and vtsReadStateFromFile
//
//#############################################################################

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <ctime>
#include <list>
#include <vector>

// includes for CSM classes -- this code is configured by a global
// committee -- any changes must be approved by that committe.

#include "TSMError.h"
#include "TSMWarning.h"
#include "TSMPlugin.h"
#include "TSMSensorModel.h"
#include "TSMImageSupportData.h"
#include "TSMISDNITF20.h"
#include "TSMISDNITF21.h"
#include "TSMISDByteStream.h"
#include "TSMISDFilename.h"
#include "TSMCovarianceModel.h"
#include "constants.h"

// includes for VTS ancillary code
#include "VTSMisc.h"
#include "SMManager.h"


std::string VTS_VERSION ="vCSM2.0.3";

// global variables required by the two macros defined below
static long instructionCount = 0;
static clock_t start_clock = clock();  // Initialize the clock
static int repeat_flag = false;        // Repeat a command Flag
static int repeat_count = 1;           // The number of times to repeat

// a pair of 'macros' to factor out common code from switch cases
static inline void initCase(void);
static inline void initCase (void)
{
   instructionCount++;                  // Advance the command serial number
   start_clock = clock();               // Record the start time for call.
};
static inline void exitCase (void);
static inline void exitCase (void)
{
   repeat_flag = false;                 // Reset the repeat command to not repeat
   repeat_count = 1;                    // Clear the repeat count
};

std::string command;
std::string logFile;
std::vector<std::string> vHelpDataValues;
bool debugFlag = false;
int termEcho=0;  // The switch that determines what gets echoed to the terminal
//                   at the vtsPlugin and the vts command line.
//                   0= no output, 1= all output (same as what goes to
//                   the log file) and 2= just the returned values.

bool logCommand;                     // Use the default log command
bool logStringCommand;               // Use the default string log command

std::vector<double>      act_val;
std::vector<double>      comp_answer;
std::vector<bool>        pass_fail;
std::vector<std::string> act_val_string;
clock_t end_clock;
int param_array_index;               // Index into param_array where the
int maxReturnValuesToCompare;        // Maximum number of numeric
std::string errorName ="";
std::string text;

bool saveLogFile = true;
std::string comment = "";

//char imageId[MAX_NAME_LENGTH] = "";
char serialNum[MAX_NAME_LENGTH] = "";
char timedata[MAX_NAME_LENGTH] = "";
char tempStr[MAX_NAME_LENGTH] = "";

// Indicates what type of ISD:
ISDType ISDFileType=NO_FILE_TYPE;
// NITF_20_TYPE,
// NITF_21_TYPE. BYTESTREAM_TYPE,
// FILENAME_TYPE or NO_FILE_TYPE.

NITF_2_0_ISD *tsmnitf20 = NULL;
NITF_2_1_ISD *tsmnitf21 = NULL;
bytestreamISD *tsmbytestream = NULL;
filenameISD *tsmfilename = NULL;
std::string saved_state_file;

TSMPlugin* stubPlugin = NULL;
TSMSensorModel* sensorModel = NULL;

TSMWarning *tsmWarn = NULL;
TSMError global_err;
bool error_thrown = false;           // flag to indicate that an error

// flag to indicate that the ISD File has been read
bool ISDFileReadDone = false;
std::string returned_sensor_model_state = "";
// flag to indicate that the sensor model state has been saved
bool sensorModelStateSaved = false;

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
                       std::vector<std::string> param_array)
{
   int i, j;                               //----- Loop Variable
   bool command_found = true;              //----- Command Found Flag

   // for getList command
   TSMPlugin::TSMPluginList* pluginPtrList = NULL;
   TSMPlugin::TSMPluginList::const_iterator ii;

   int ret_int;
   int index;
   bool error_switch = false;
   bool return_bool1 = false;

   TSMMisc::Param_CharType parameterType = TSMMisc::NONE;

   switch (commandNumber)
   {
      //----- CASE # comment -- 0  NSetup
      case COMMENT:
         logCommand = false;            // do not log this command.

         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }
         exitCase();
         break;
         //----- CASE vtsSetLogFileName  NSetup
      case VTS_SET_LOG_FILENAME:
         logCommand = false;

         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }

         if(param_array.size() != 0)    // Do not use the default file name
            // and location (./vts_logfile.csv)or .\\vts_logfile.csv
            logFile = param_array[0];

         if(debugFlag)
         {
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      logFile=" << logFile << std::endl;
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
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }

         if(param_array.size() != 0)
           //set save-to-logfile" flag, else just display the value
           saveLogFile = (atoi(param_array[0].c_str()) == 1);

         if(debugFlag)
         {
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      saveLogFile=" << saveLogFile << std::endl;
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
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }

         comment = param_array[0];
         text.clear();

         for (i = 1; i < (int)param_array.size() ; i++)
            text += (param_array[i] + std::string(" "));

         if(debugFlag)
         {
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      comment=" << comment << std::endl;
            std::cout << "      text=" << text << std::endl;
         }

         if(saveLogFile )
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
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }

         text.clear();
         for (i = 0; i < (int)param_array.size(); i++)
            text += (param_array[i] + ' ');

         if(debugFlag)
         {
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      text=" << text << std::endl;
         }

         if(saveLogFile )
            recordLog(logFile,
               text);

         param_array_index = 1;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE vtsRepeat Set repeat count NSetup
      case VTS_REPEAT:
         logCommand = false;

         if(debugFlag)
         {
            std::cout
               << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }
         // save the number of repititions.
         repeat_count = atoi(param_array[0].c_str());

         initCase();
         if ( repeat_count <= 1)
         {
            std::cout << "      Incorrect vtsRepeat value." << std::endl;
            repeat_count = 1;
         }
         else
            repeat_flag = true;
         end_clock = clock();

         if(debugFlag)
         {
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      repeat_count=" << repeat_count << std::endl;
         }

         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         comment.clear();
         sprintf (tempStr, "%u", repeat_count);
         text = tempStr;

         if(saveLogFile )
            recordLog(logFile,
               serialNum,
               timedata,
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

         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }

         termEcho = atoi(param_array[0].c_str());
         initCase();
         if ( termEcho < 0 || termEcho > 2)
         {
            std::cout << "      Incorrect termEcho value." << std::endl;
            std::cout << "      Value of termEcho=" << termEcho
               << std::endl;
            termEcho = 2;
         }
         end_clock = clock();

         if(debugFlag)
         {
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      termEcho=" << termEcho << std::endl;
         }

         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         comment.clear();
         sprintf (tempStr, "%u\0", termEcho);
         text = tempStr;

         if(saveLogFile )
            recordLog(logFile,
               serialNum,
               timedata,
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
            std::cout << "Debugging has been enabeled." << std::endl;
         else
            std::cout << "Debugging has been disabeled." << std::endl;
         break;

         //----- CASE vtsToggleDebug NSetup
      case VTS_TOGGLE_COMPARE_DEBUG:
         logCommand = false;
         compareDebugFlag = !compareDebugFlag;

         if(compareDebugFlag)
            std::cout << "Compare debugging has been enabeled." << std::endl;
         else
            std::cout << "Compare debugging has been disabeled." << std::endl;
         break;

         //----- CASE vtsToggleDebug NSetup
      case VTS_TOGGLE_NITF_DEBUG:
         logCommand = false;
         nitfDebugFlag = !nitfDebugFlag;

         if(nitfDebugFlag)
            std::cout << "NITF debugging has been enabeled." << std::endl;
         else
            std::cout << "NITF debugging has been disabeled." << std::endl;
         break;

         //----- CASE printList NSetup
      case PRINT_LIST:
         logCommand = false;

         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }

         initCase();
         printList(logFile.c_str());
         end_clock = clock();           // Record the end time for call.

         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         comment.clear();
         text.clear();

         if(saveLogFile )
            recordLog(logFile,
               serialNum,
               timedata,
               comment,
               command,
               text);
         exitCase();
         break;

         //----- CASE printModelList NSetup
      case PRINT_MODEL_LIST:
         logCommand = false;

         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }

         initCase();
         if(param_array.size() > 0)
            printModelList(param_array[0]);
         end_clock = clock();           // Record the end time for call.

         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         comment.clear();
         text.clear();

         if(saveLogFile )
            recordLog(logFile,
               serialNum,
               timedata,
               comment,
               command,
               text);
         exitCase();
         break;

         //----- CASE status NSetup
      case STATUS:
         logCommand = false;

         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }

         initCase();

         if(tsmnitf20)
         {
            std::cout << "An NITF 2.0 file is currently loaded." << std::endl;
         }
         if(tsmnitf21)
         {
            std::cout << "An NITF 2.1 file is currently loaded." << std::endl;
         }
         if(tsmfilename)
         {
            std::cout << "An file is currently loaded." << std::endl;
         }
         if(tsmbytestream)
         {
            std::cout << "An byte stream is currently available." << std::endl;
         }

         if(stubPlugin)
         {
            std::string pluginName;

            try
            {
               tsmWarn = stubPlugin->getPluginName(pluginName);
            }
            catch (TSMError err)
            {
               error_thrown = true;
               global_err = err;
            }
            catch ( ... )
            {
               std::cout
                  << UNKNOWN_ERROR_STR << "getPluginName\n";
               error_thrown = true;
            }
            if (tsmWarn) break;

            std::cout << pluginName << " is currently loaded." << std::endl;
         }
         if(sensorModel)
         {
            try
            {
               //tsmWarn =
               //   sensorModel->getSensorModelName(smName);
            }
            catch (TSMError err)
            {
               error_thrown = true;
               global_err = err;
            }
            catch ( ... )
            {
               std::cout
                  << UNKNOWN_ERROR_STR << "getSensorModelName (SM47)\n";
               error_thrown = true;
            }
            if (tsmWarn) break;

            //std::cout << smName << " is currently loaded." << std::endl;
            std::cout << "A sensor model is currently loaded." << std::endl;
         }

         end_clock = clock();           // Record the end time for call.

         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         comment.clear();
         text.clear();

         if(saveLogFile )
            recordLog(logFile,
               serialNum,
               timedata,
               comment,
               command,
               text);
         exitCase();
         break;

		 //----- CASE vtsWriteStateToFile
      case VTS_WRITE_STATE_TO_FILE:

         logCommand = false;
         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }
         initCase();
         saved_state_file = param_array[0];
         try
         {
      		 writeStateFile(saved_state_file, returned_sensor_model_state);
         }
         catch (TSMError err)
         {
            error_thrown = true;
            global_err = err;
         }
         catch ( ... )
         {
            std::cout << UNKNOWN_ERROR_STR << "vtsWriteStateToFile\n";
            error_thrown = true;
         }
         if(error_thrown)
         {
            break;
         }
         std::cout << " Sensor Model State successfully written to file " 
			       << saved_state_file << " (" 
				   << returned_sensor_model_state.length() << " bytes)" 
				   << std::endl;
         end_clock = clock();           // Record the end time for call.
         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         comment.clear();
         text.clear();
         if(saveLogFile )
            recordLog(logFile,
               serialNum,
               timedata,
               comment,
               command,
               text);
         exitCase();
         break;

		 //----- CASE vtsReadStateFromFile
      case VTS_READ_STATE_FROM_FILE:
         logCommand = false;
         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }
         initCase();
         saved_state_file = param_array[0];
         try
         {
      		 returned_sensor_model_state = readStateFile(saved_state_file);
         }
         catch (TSMError err)
         {
            error_thrown = true;
            global_err = err;
         }
         catch ( ... )
         {
            std::cout << UNKNOWN_ERROR_STR << "vtsReadStateFromFile\n";
            error_thrown = true;
         }
         if(error_thrown)
         {
            break;
         }
         std::cout << " Sensor Model State successfully read from file " 
			       << saved_state_file << " (" 
				   << returned_sensor_model_state.length() << " bytes)" 
				   << std::endl;
         end_clock = clock();           // Record the end time for call.
         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         comment.clear();
         text.clear();
         if(saveLogFile )
            recordLog(logFile,
               serialNum,
               timedata,
               comment,
               command,
               text);
         exitCase();
         break;

         //----- CASE vtsRead20ISDFile NSetup
      case VTS_READ_20ISD_FILE:
         logCommand = false;

         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber]
               << std::endl;
         }

         initCase();

         //----- get the file and fill the object
         try
         {
            tsmnitf20 = new NITF_2_0_ISD ();

            if(param_array.size() > 1)
               tsmWarn = initNITF20ISD(tsmnitf20,
                                       param_array[0].c_str(),
                                       atoi(param_array[1].c_str()) - 1);
            else
               tsmWarn = initNITF20ISD(tsmnitf20,
                                       param_array[0].c_str());
         }
         catch (TSMError err)
         {
            error_thrown = true;
            global_err = err;
         }
         catch (...)
         {
            std::cout << UNKNOWN_ERROR_STR <<  "initNITF20ISD\n";
            error_thrown = true;
         }
         

         end_clock = clock();

         if (error_thrown)
         {
            ISDFileReadDone = false;
         }
         else
         {
            if(debugFlag)
            {
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      ISDFile=" << param_array[0].c_str()
                         << std::endl;
               if(param_array.size() > 1)
                  std::cout << "      imageIndex=" << param_array[1].c_str()
                            << std::endl;
            }

            // Add error check TBD

            ISDFileType = NITF_20_TYPE;
            ISDFileReadDone = true;
         }

         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         comment.clear();
         if(saveLogFile )
            recordLog(logFile,
               serialNum,
               timedata,
               comment,
               command,
               param_array[0].c_str());
         exitCase();
         break;

         //----- CASE vtsRead21ISDFile NSetup
      case VTS_READ_21ISD_FILE:
         logCommand = false;

         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }

         initCase();
         //----- get the file and fill the object
         try
         {
            tsmnitf21 = new NITF_2_1_ISD();
            if(param_array.size() > 1)
               tsmWarn = initNITF21ISD(tsmnitf21,
                                       param_array[0].c_str(),
                                       atoi(param_array[1].c_str()) - 1);
            else
               tsmWarn = initNITF21ISD(tsmnitf21,
                                       param_array[0].c_str());
         }
         catch (TSMError err)
         {
            error_thrown = true;
            global_err = err;
         }
         catch (...)
         {
            error_thrown = true;
            std::cout << UNKNOWN_ERROR_STR <<  "initNITF21ISD\n";
         }

         end_clock = clock();

         if (error_thrown)
         {
            ISDFileReadDone = false;
         }
         else
         {
            if(debugFlag)
            {
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      ISDFile=" << param_array[0].c_str()
                         << std::endl;
               if(param_array.size() > 1)
                  std::cout << "      imageIndex=" << param_array[1].c_str()
                            << std::endl;
            }

            ISDFileType = NITF_21_TYPE;
            ISDFileReadDone = true;
         }

         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         comment.clear();

         if(saveLogFile )
            recordLog(logFile,
               serialNum,
               timedata,
               comment,
               command,
               param_array[0].c_str());
         exitCase();
         break;

         //----- CASE vtsReadByteStream NSetup
      case VTS_READ_BYTESTREAM:
         logCommand = false;

         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }

         initCase();

         try
         {
            tsmbytestream = new bytestreamISD();
            initBytestreamISD (tsmbytestream, param_array[0].c_str());
         }
         catch (TSMError err)
         {
            error_thrown = true;
            global_err = err;
         }
         catch (...)
         {
            std::cout << UNKNOWN_ERROR_STR <<  "initBytestreamISD\n";
            error_thrown = true;
         }

         end_clock = clock();

         if (error_thrown ||
            tsmbytestream && tsmbytestream->_isd.length() == 0)
         {
            std::cout << "Unable to construct TSMByteStream object\n";
            ISDFileReadDone = false;
         }
         else
         {
            if(debugFlag)
            {
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      ISDFile=" << param_array[0].c_str()
                         << std::endl;
            }

            // Add error check TBD
            ISDFileType = BYTESTREAM_TYPE;
            ISDFileReadDone = true;
         }

         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         comment.clear();
         text = param_array[0].c_str();

         if(saveLogFile )
            recordLog(logFile,
               serialNum,
               timedata,
               comment,
               command,
               text);
         exitCase();
         break;

         //----- CASE vtsReadFilename NSetup
      case VTS_READ_FILENAME:
         logCommand = false;

         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }

         initCase();

         try
         {
            tsmfilename = new filenameISD;
            initFilenameISD (tsmfilename, param_array[0].c_str());
         }
         catch (TSMError err)
         {
            error_thrown = true;
            global_err = err;
         }
         catch (...)
         {
            std::cout << UNKNOWN_ERROR_STR <<  "initBytestreamISD\n";
            error_thrown = true;
         }

         end_clock = clock();           // Record the end time for call.

         if(error_thrown)
         {
            ISDFileReadDone = true;
         }
         else
         {
            if(debugFlag)
            {
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      ISDFile=" << param_array[0].c_str()
                         << std::endl;
            }

            // Add error check TBD
            ISDFileType = FILENAME_TYPE;
            ISDFileReadDone = true;
         }

         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         comment.clear();
         text = param_array[0].c_str();

         if(saveLogFile )
            recordLog(logFile,
               serialNum,
               timedata,
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
            std::cout << "   vts Commands:\n";

            bool someNotFound = false;
            for(j=0;j<MAX_NUMBER_COMMANDS;j++)
            {
               if(j == BEGIN_SP_NUMBER)
                  std::cout
                     << "\n\tThe following are Sensor Plugin commands:"
                     << std::endl;
               else if(j == BEGIN_SM_NUMBER)
                  std::cout
                        << "\n\tThe following are Sensor Model commands:"
                        << std::endl;
               else if(j == BEGIN_VTS_NUMBER)
                  std::cout
                        << "\n\tThe following are VTS internal commands:"
                        << std::endl;

               std::string footnote, helpText;
               helpText = vHelpDataValues[j];
               int item_index = helpText.find(" help is not defined");
               if (item_index > -1) {
                 someNotFound = true;
                 footnote = " * ";
               } else {
                 footnote = "   ";
               }
               std::cout << "\t" << footnote << menulist[j] << std::endl;
            }
            if(someNotFound)
               std::cout
                  << "\n\t * help text not available for this command"
                  << std::endl;
         }
         else
         {
            int index = getCommandIndex(param_array[0].c_str());

            if(index != MAX_NUMBER_COMMANDS)
            {
               std::cout << menulist[index] << ":" << std::endl;
               std::cout << std::endl;

               std::cout << vHelpDataValues[index] << std::ends;
            }
            else
            {
               std::cout << "Command " << param_array[0].c_str()
                  << " was not found." << std::endl;
            }
         }

         exitCase();                    // HELP
         break;

         //----- CASE EXIT        NSetup
      case EXIT1:                       // exit
      case EXIT2:                       // Exit

         if(debugFlag)
         {
            std::cout << "vts>    You requested routine "
               << menulist[commandNumber] << std::endl;
         }

         time_t aclock;
         aclock = time(NULL);
         text =  "\nProgram End Time = " + (std::string)ctime(&aclock)
            + "\n\n\n\n\n"; ;
         if(saveLogFile )
            recordLog(logFile,
               text);
         if (tsmfilename)
            delete (tsmfilename);
         if (tsmbytestream)
            delete (tsmbytestream);
         if (tsmnitf20)
            delete (tsmnitf20);
         if (tsmnitf21)
            delete (tsmnitf21);

         exit(0);
         //	       break; // case EXIT

         //----- CASE getList
      case SP_GET_LIST:
         logCommand = false;

         if(debugFlag)
         {
            std::cout << std::endl
               << "  vts - Calling getList"
               << std::endl;
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      NONE" << std::endl;
         }

         initCase();
         for(i=0; i < repeat_count; i++)
         {
            try
            {
               tsmWarn = TSMPlugin::getList(pluginPtrList);
            }
            catch (TSMError err)
            {
               error_thrown = true;
               global_err = err;
            }
            catch ( ... )
            {
               std::cout
                  << UNKNOWN_ERROR_STR << "TSMPlugin::getList (SM49)\n";
               error_thrown = true;
            }
            if (tsmWarn) break;
         }
         end_clock = clock();
         comment.clear();
         text.clear();
         if (!pluginPtrList)
         {
            std::cout << "plugin::getList returned a null list SM-49\n";
            break;
         }
         for (ii = pluginPtrList->begin();
            ii != pluginPtrList->end();
            ++ii)
         {
            std::string pluginName;
            try
            {
               tsmWarn = (*ii)->getPluginName(pluginName);
            }
            catch (TSMError err)
            {
               error_thrown = true;
               global_err = err;
            }
            catch ( ... )
            {
               std::cout
                  << UNKNOWN_ERROR_STR << "getPluginName (SM49)\n";
               error_thrown = true;
            }
            text += pluginName;
            text += " ";
         }
         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         if(saveLogFile )
            recordLog(logFile,
               (char*)serialNum,
               (char*)&timedata,
               comment,
               command,
               text);
         param_array_index = 0;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE getPluginName NSetup
      case SP_GET_PLUGIN_NAME:
         logCommand = false;

         if(debugFlag)
         {
            std::cout << std::endl
               << "  vts - Calling TargetPlugin::getPluginName"
               << std::endl;
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      NONE" << std::endl;
         }

         initCase();
         for(i=0; i < repeat_count; i++)
         {
            try
            {
               tsmWarn = TSMPlugin::getList(pluginPtrList);
            }
            catch (TSMError err)
            {
               error_thrown = true;
               global_err = err;
            }
            catch ( ... )
            {
               std::cout
                  << UNKNOWN_ERROR_STR << "TSMPlugin::getPluginName (SM67)\n";
               error_thrown = true;
            }
            if (tsmWarn) break;
         }
         end_clock = clock();
         comment.clear();
         text.clear();
         for (ii = pluginPtrList->begin();
            ii != pluginPtrList->end();
            ++ii)
         {
            std::string pluginName;
            try
            {
               tsmWarn = (*ii)->getPluginName(pluginName);
            }
            catch (TSMError err)
            {
               error_thrown = true;
               global_err = err;
            }
            catch ( ... )
            {
               std::cout
                  << UNKNOWN_ERROR_STR << "getPluginName (SM49)\n";
               error_thrown = true;
            }
            text += (pluginName + " ");
         }
         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         if(saveLogFile )
            recordLog(logFile,
               (char*)serialNum,
               (char*)&timedata,
               comment,
               command,
               text);
         param_array_index = 0;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE findPlugin
      case SP_FIND_PLUGIN:
         logCommand = false;

         if(debugFlag)
         {
            std::cout << std::endl
               << "  vts - Calling findPlugin"
               << std::endl;
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      Plugin="  << param_array[0] << std::endl;
         }

         initCase();
         for(i=0; i < repeat_count; i++)
         {
            try
            {
               tsmWarn = TSMPlugin::findPlugin( param_array[0],
                  (TSMPlugin*&)stubPlugin);
            }
            catch (TSMError err)
            {
               error_thrown = true;
               global_err = err;
            }
            catch ( ... )
            {
               std::cout
                  << UNKNOWN_ERROR_STR << "TSMPlugin::findPlugin (SM50)\n";
               error_thrown = true;
            }
            if (tsmWarn) break;
         }
         end_clock = clock();
         comment.clear();
         text.clear();
         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         if (tsmWarn)
         {
            std::cout << "TSMPlugin::findPlugin did not find "
               << param_array[0] << std::endl;
            text = "NOT Found";
         }
         else
         {
            std::cout << "TSMPlugin::findPlugin found for " << param_array[0]
               << std::endl;
            text = "Found";
         }

         if(saveLogFile )
            recordLog(logFile,
               (char*)serialNum,
               (char*)&timedata,
               comment,
               command,
               text);
         param_array_index = 0;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE removePlugin
      case SP_REMOVE_PLUGIN:
         logCommand = false;

         if(debugFlag)
         {
            std::cout << std::endl
               << "  vts - Calling removePlugin"
               << std::endl;
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      Plugin="  << param_array[0] << std::endl;
         }

         initCase();
         for (i = 0; i < repeat_count; i++)
         {
            std::string rem_name = param_array[0];
            try
            {
               tsmWarn = TSMPlugin::removePlugin(rem_name);
            }
            catch (TSMError err)
            {
               error_thrown = true;
               global_err = err;
            }
            catch ( ... )
            {
               std::cout
                  << UNKNOWN_ERROR_STR << "TSMPlugin::removePlugin (SM54)\n";
               error_thrown = true;
            }
            //if (tsmWarn) break;
         }
         end_clock = clock();
         comment.clear();
         text.clear();
         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         if(tsmWarn)
            text = "Not Found";
         else
            text = "Removed";
         if(saveLogFile )
            recordLog(logFile,
               (char*)serialNum,
               (char*)&timedata,
               comment,
               command,
               text);

         param_array_index = 0;
         maxReturnValuesToCompare = 0;
         exitCase();
         break;

         //----- CASE getNumSystematicErrorCorrections NSetup
      case SM_GET_NUM_SYSTEMATIC_ERROR_CORRECTIONS:
         if(debugFlag)
         {
            std::cout << std::endl
                      << "  vts - Calling StubSensorModel::getNumSystematicErrorCorrections"
                      << std::endl;
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      NONE" << std::endl;
         }

         initCase();
         for(i=0; i < repeat_count; i++) {
            try {
               tsmWarn = sensorModel->getNumSystematicErrorCorrections(ret_int);
            } catch (TSMError err) {
               error_thrown = true;
               global_err = err;
               break;
            } catch ( ... ) {
               std::cout << UNKNOWN_ERROR_STR
                         << "getNumSystematicErrorCorrections (SM43)\n";

               error_thrown = true;
               break;
            }
            if (tsmWarn || error_thrown) break;
         }
         end_clock = clock();

         if(debugFlag)
         {
            std::cout << "    vts - Returned values are: " << std::endl;
            std::cout << "      value=" << ret_int << std::endl;
            std::cout << std::endl;
         }

         param_array_index = 0;
         maxReturnValuesToCompare = 1;
         act_val[0]=(double)ret_int;
         exitCase();
         break;

         //-----  CASE getSystematicErrorCorrectionName Nsetup
      case SM_GET_SYSTEMATIC_ERROR_CORRECTION_NAME:
         logCommand = false;
         logStringCommand = true;
         index = atoi(param_array[0].c_str());

         if(debugFlag)
         {
            std::cout << std::endl
                      << "  vts - Calling StubSensorModel::getSystematicErrorCorrectionName"
                      << std::endl;
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      index=" << index << std::endl;
         }

         initCase();
         for(i=0; i < repeat_count; i++) {
            try {
               tsmWarn =
                  sensorModel->getSystematicErrorCorrectionName(index,errorName);
            } catch (TSMError err) {
               error_thrown = true;
               global_err = err;
            } catch ( ... ) {
               std::cout
                  << UNKNOWN_ERROR_STR << "getSystematicErrorCorrectionName (SM44)\n";
               error_thrown = true;
            }
            if (tsmWarn || error_thrown) break;
         }
         end_clock = clock();

         if(debugFlag)
         {
            std::cout << "    vts - Returned values are: " << std::endl;
            std::cout << "      errorName=" << errorName << std::endl;
            std::cout << std::endl;
         }

         act_val_string.push_back(errorName);

         param_array_index = 1;
         maxReturnValuesToCompare = 1;
         exitCase();
         break;

         //-- CASE setCurrentSystematicErrorCorrectionSwitch NSetup
      case SM_SET_CURRENT_SYSTEMATIC_ERROR_CORRECTION_SWITCH:
         logCommand = false;
         index = atoi(param_array[0].c_str());
         if (atoi(param_array[1].c_str()) == 0)
            error_switch = false;
         else
            error_switch = true;
         parameterType = (TSMMisc::Param_CharType)atoi(param_array[2].c_str());

         if(debugFlag)
         {
            std::cout << std::endl
                      << "  vts - Calling StubSensorModel::setCurrentSystematicErrorCorrectionSwitch"
                      << std::endl;
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      index=" << index << std::endl;
            std::cout << "      error_switch=" << error_switch
                      << std::endl;
         }

         initCase();
         for(i=0; i < repeat_count; i++) {
            try {
               tsmWarn =
                  sensorModel->setCurrentSystematicErrorCorrectionSwitch
                      (index,error_switch,parameterType);
            } catch (TSMError err) {
               error_thrown = true;
               global_err = err;
               break;
            } catch ( ... ) {
               std::cout << UNKNOWN_ERROR_STR
                    << "setCurrentSystematicErrorCorrectionSwitch (SM45)\n";
               error_thrown = true;
               break;
            }
            if (tsmWarn || error_thrown) break;
         }
         end_clock = clock();

         if(debugFlag)
         {
            std::cout << "    vts - Returned values are: "
                      << std::endl;
            std::cout << "      NONE" << std::endl;
            std::cout << std::endl;
         }

         param_array_index = 2;
         maxReturnValuesToCompare = 0;
         sprintf ((char*)serialNum, "%u", instructionCount);
         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         comment = "";
         text = "";

         if(saveLogFile)
            recordLog(logFile,
                      serialNum,
                      timedata,
                      comment,
                      command,
                      text);

         exitCase();
         break;

         //----- CASE getCurrentSystematicErrorCorrectionSwitch Nsetup
      case SM_GET_CURRENT_SYSTEMATIC_ERROR_CORRECTION_SWITCH:
         index = atoi(param_array[0].c_str());

         if(debugFlag)
         {
            std::cout << std::endl
                      << "  vts - Calling StubSensorModel::getCurrentSystematicErrorCorrectionSwitch"
                      << std::endl;
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      index=" << index << std::endl;
         }

         initCase();
         for (i=0; i < repeat_count; i++) {
            try {
               tsmWarn = sensorModel->getCurrentSystematicErrorCorrectionSwitch(
                              index, return_bool1);
            } catch (TSMError err) {
               error_thrown = true;
               global_err = err;
               break;
            } catch ( ... ) {
               std::cout << UNKNOWN_ERROR_STR
                       << "getCurrentSystematicErrorCorrectionSwitch (SM46)\n";

               error_thrown = true;
               break;
            }
            if (tsmWarn || error_thrown) break;
         }
         end_clock = clock();

         if(debugFlag)
         {
            std::cout << "    vts - Returned values are: "
                      << std::endl;
            std::cout << "      value=" << return_bool1 << std::endl;
            std::cout << std::endl;
         }

         param_array_index = 1;
         maxReturnValuesToCompare = 1;
         act_val[0]=(double)return_bool1;

         exitCase();
         break;

      default:
         command_found = false;
   } // switch

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
                       std::vector<std::string> param_array)
{
   int j;                                 //----- Loop Variable
   bool command_found = true;
   std::string dump;

   TSMPlugin::TSMPluginList* pluginPtrList = NULL;

   if( !ISDFileReadDone )
   {
      std::cout << "  vts - vtsReadISDFile has not been run yet! "
                << std::endl;
      logCommand = false;            // do not log this command.
   }
   else
   {
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
            // (MeanSensorModel) is not one of the example models
            // and, therefore, will not be found.
            //
            // The actual criteria, and the exact methods, used for
            // sensor model selection in the real world are yet to
            // be finalized.  //---

            initCase();
            std::string modelName  = param_array[0];
            std::string pluginName = param_array[1];
            logCommand = false;
            try
            {
               tsmWarn = TSMPlugin::getList(pluginPtrList);
            }
            catch (TSMError err)
            {
               error_thrown = true;
               global_err = err;
               break;
            }
            catch(...)
            {
               std::cout
                  << "Unknown exception thrown by TSMPlugin::getList.\n";
               error_thrown = true;
               break;
            }
            if (!tsmWarn)
            {                           // no warning returned
               end_clock = clock();
               j=0;
               // count loaded plugins
               if (!pluginPtrList)
                  std::cerr << "\a\n\a\n\a\t "
                     << "***** pluginPtrList is NULL *****"
                     << "\a\n\a\n\a\n";
               else
               {                        // have a pluginptr list
                  try
                  {
                     tsmWarn = printList((char *)logFile.c_str());
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                     break;
                  }
                  catch(...)
                  {
                     std::cout
                        << "Unknown exception thrown by TSMPlugin::printList.\n";
                     error_thrown = true;
                     break;
                  }
                  switch (ISDFileType)
                  {
                     case NITF_20_TYPE:
                        try
                        {
                           tsmWarn =
                              makeModelListFromISD(modelName,
                              (tsm_ISD*)tsmnitf20);
                        }
                        catch (TSMError err)
                        {
                           error_thrown = true;
                           global_err = err;
                        }
                        catch (...)
                        {
                           std::cout << UNKNOWN_ERROR_STR <<  "makeModelListFromISD\n";
                           error_thrown = true;
                        }
                        break;

                     case NITF_21_TYPE:
                        try
                        {
                           tsmWarn =
                              makeModelListFromISD(modelName,
                              (tsm_ISD*)tsmnitf21);
                        }
                        catch (TSMError err)
                        {
                           error_thrown = true;
                           global_err = err;
                        }
                        catch (...)
                        {
                           std::cout << UNKNOWN_ERROR_STR <<
                              "makeModelListFromISD\n";
                           error_thrown = true;
                        }
                        break;

                     case BYTESTREAM_TYPE:
                        try
                        {
                           tsmWarn =
                              makeModelListFromISD(modelName,
                              (tsm_ISD*)tsmbytestream);
                        }
                        catch (TSMError err)
                        {
                           error_thrown = true;
                           global_err = err;
                        }
                        catch (...)
                        {
                           std::cout << UNKNOWN_ERROR_STR <<
                              "makeModelListFromISD\n";
                           error_thrown = true;
                        }
                        break;

                     case FILENAME_TYPE:
                        try
                        {
                           tsmWarn =
                              makeModelListFromISD(modelName,
                              (tsm_ISD*)tsmfilename);
                        }
                        catch (TSMError err)
                        {
                           error_thrown = true;
                           global_err = err;
                        }
                        catch (...)
                        {
                           std::cout << UNKNOWN_ERROR_STR <<
                              "makeModelListFromISD\n";
                           error_thrown = true;
                        }
                        break;

                     default:
                        tsmWarn->setTSMWarning
                           (TSMWarning::UNKNOWN_WARNING,
                           "Invalid ISD type",
                           "vts");

                        std::cout << "  vts - ISD Type is unknown!\n";
                  }                     //  end  switch
                  try
                  {
                     tsmWarn = TSMPlugin::findPlugin(
                        pluginName,
                        (TSMPlugin*&)stubPlugin);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch (...)
                  {
                     std::cout << UNKNOWN_ERROR_STR <<  "findPlugin\n";
                     error_thrown = true;
                  }
                  if (tsmWarn)
                  {
                     text = "NO Plugin Found ";
                     comment.clear();

                     std::cout
                        << "TSMPlugin::findPlugin had an error."
                        << std::endl;
                  }
                  if (error_thrown)
                  {
                     text = "No Plugin Found ";
                  }
                  if (!error_thrown && !tsmWarn)
                  {
                     text = "Found a Model that matches";
                     comment.clear();
                  }
               }                        // end !pluginPtrList
            }                           // no warning from getList (!tsmWarn)
            else
               std::cout
                  << "\n\n ***** Plugins not loaded yet (case MAKE_MODEL_LIST_FROM_ISD) ***** \n\n";

            end_clock = clock();

            sprintf ((char*)serialNum, "%u", instructionCount);
            sprintf ((char*)timedata, "%u,%u",
               start_clock, end_clock);
            if(saveLogFile)
               recordLog(logFile,
                  (char*)serialNum,
                  (char*)&timedata,
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
               std::cout << "vts>    You requested routine "
                  << menulist[commandNumber] << std::endl;
            }

            switch (ISDFileType)
            {
               case NITF_20_TYPE:
                  std::cout << "\n!!!!! NITF 2.0 header:\n";
                  dumpHdr(tsmnitf20);
                  break;

               case NITF_21_TYPE:
                  std::cout << "\n!!!!! NITF 2.1 header:\n";
                  dumpHdr(tsmnitf21);
                  break;

               case BYTESTREAM_TYPE:
                  std::cout << "\nFORMAT= BYTESTREAM" << '\n';
                  dump.assign (tsmbytestream->_isd, 0, 100);
                  std::cout << "First 100 bytes of ISD: \n"
                     <<  dump << '\n';
                  break;

               case FILENAME_TYPE:
                  std::cout << "tsmfilename filename is "
                     << tsmfilename->_filename << '\n';
                  break;

               default:
                  std::cout << "  vts - ISDFileType is unknown! "
                     << std::endl;
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
                       std::vector<std::string> param_array)
{
   int i;                              //----- Loop Variable
   bool command_found = true;

   // flag to indicate that model may be constructable
   bool constructable = false;
   // flag to indicate that ISD may be converted to state
   bool convertible = false;

   int index = 0;
   int sensor_model_index = 0;
   int n_sensor_models = 0;

   int ret_int;
   std::string smName;
   std::string man_name;
   std::string relDate;

   bool return_bool1 = false;
   bool return_bool2 = false;
   bool return_bool3 = false;
   bool return_bool4 = false;
   bool return_bool5 = false;
   bool return_bool6 = false;

   //MMMMMMMMAKEMODELLISTDONE SECTION
   if( stubPlugin == NULL )
   {
      std::cout << NO_MODEL_LIST_ERROR << std::endl;
   }
   else
   {
      tsm_ISD *isd = NULL;
      command_found = true;
      switch (commandNumber)
      {
         //
         //
         //  MMLD
         case SP_CAN_ISD_BE_CONVERTED_TO_SENSOR_MODEL_STATE:
            logCommand = false;
            initCase();
            for(i=0; i < repeat_count; i++)
            {
               //tsm_ISD *isd;
               switch (ISDFileType)
               {
                  case NITF_20_TYPE:
                     isd = tsmnitf20;
                     break;

                  case NITF_21_TYPE:
                     isd = tsmnitf21;
                     break;
                  case BYTESTREAM_TYPE:
                     isd = tsmbytestream;
                     break;

                  case FILENAME_TYPE:
                     isd = tsmfilename;
                     break;
                  default:
                     std::cout << "  vts - ISDFileType is unknown! "
                        << std::endl;
               }                        //. end inner switch
               try
               {
                  tsmWarn = stubPlugin->canISDBeConvertedToSensorModelState
                     (*isd,
                     param_array[0],
                     convertible);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "stubPlugin->canISDBeConvertedToSensorModelState (SM52)\n";
                  error_thrown = true;
               }
            }                           // repeat count loop
            end_clock = clock();
            if (convertible)
               text = "Possibly Convertable";
            else
               text = "NOT Convertable";
            comment.clear();
            sprintf ((char*)serialNum, "%u", instructionCount);
            sprintf ((char*)timedata, "%u,%u",
               start_clock, end_clock);
            if(saveLogFile )
               recordLog(logFile,
                  (char*)serialNum,
                  (char*)&timedata,
                  comment,
                  command,
                  text);
            exitCase();
            break;

            //
            //
            //----- CASE canSensorModelBeConstructedFromISD  MMLD
         case SP_CAN_SENSOR_MODEL_BE_CONSTRUCTED_FROM_ISD:
            logCommand = false;

            if(debugFlag)
            {
               std::cout
                  << std::endl
                  << "  vts - Calling canSensorModelBeConstructedFromISD"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      Plugin="  << param_array[0] << std::endl;
            }

            initCase();
            switch (ISDFileType)
            {
               case NITF_20_TYPE:
                  isd = tsmnitf20;
                  break;

               case NITF_21_TYPE:
                  isd = tsmnitf21;
                  break;

               case BYTESTREAM_TYPE:
                  isd = tsmbytestream;
                  break;

               case FILENAME_TYPE:
                  isd = tsmfilename;
                  break;

               default:
                  std::cout << "  vts - ISDFileType is unknown!\n";
            }
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = stubPlugin->canSensorModelBeConstructedFromISD
                     (*isd,
                     param_array[0],
                     constructable);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "stubPlugin->canSensorModelBeConstructedFromISD (SM54)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if (tsmWarn)
            {
               std::cout
                  << "TSMPlugin::canSensorModelBeConstructedFromISD can not construct from ISD "
                  << param_array[0] << std::endl;
               text = "NOT Constructable";
            }
            else
            {
               std::cout
                  << "TSMPlugin::canSensorModelBeConstructedFromISD can possibly be constructed from ISD "
                  << param_array[0] << std::endl;
               text = "Possibly Constructable";
            }
            comment.clear();
            sprintf ((char*)serialNum, "%u", instructionCount);
            sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
            if(saveLogFile )
               recordLog(logFile,
                  (char*)serialNum,
                  (char*)&timedata,
                  comment,
                  command,
                  text);

            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE constructSensorModelFromISD  MMLD
         case SP_CONSTRUCT_SENSOR_MODEL_FROM_ISD:
            logCommand = false;

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling constructSensorModelFromISD"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            initCase();
            for (i=0; i < repeat_count; i++)
            {
               switch (ISDFileType)
               {
                  case NITF_20_TYPE:
                     isd = tsmnitf20;
                     break;

                  case NITF_21_TYPE:
                     isd = tsmnitf21;
                     break;

                  case BYTESTREAM_TYPE:
                     isd = tsmbytestream;
                     break;

                  case FILENAME_TYPE:
                     isd = tsmfilename;
                     break;

                  default:
                     std::cout << "  vts - ISDFileType is unknown! "
                        << std::endl;
               }                        // switch
               try
               {
                  tsmWarn = stubPlugin->constructSensorModelFromISD
                     (*isd,
                     std::string (param_array[0]),
                     (TSMSensorModel*&) sensorModel);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "stubPlugin->constructSensorModelFromISD (SM56)\n";
                  error_thrown = true;
               }
               end_clock = clock();
               if (!sensorModel)
               {
                  if(!tsmWarn)
                  {
                     tsmWarn = new TSMWarning(TSMWarning::UNKNOWN_WARNING,
                               "Sensor Model Not Constucted",
                               "VTS::processCommand");
                  }

                  text = "Not Constructed";
               }
               else
               {
                  text = "Constructed";
               }
               comment.clear();
               sprintf ((char*)serialNum, "%u", instructionCount);
               sprintf ((char*)timedata, "%u,%u",
                  start_clock, end_clock);
               if(saveLogFile )
                  recordLog(logFile,
                     (char*)serialNum,
                     (char*)&timedata,
                     comment,
                     command,
                     text);
               param_array_index = 0;
               maxReturnValuesToCompare = 0;
            }

            exitCase();
            break;

            //----- CASE convertISDToSensorModelState  MMLD
         case SP_CONVERT_ISD_TO_SENSOR_MODEL_STATE:
            logCommand = false;

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling convertISDToSensorModelState"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            initCase();
            for (i=0; i < repeat_count; i++)
            {
               switch (ISDFileType)
               {
                  case NITF_20_TYPE:
                     isd = tsmnitf20;
                     break;

                  case NITF_21_TYPE:
                     isd = tsmnitf21;
                     break;

                  case BYTESTREAM_TYPE:
                     isd = tsmbytestream;
                     break;

                  case FILENAME_TYPE:
                     isd = tsmfilename;
                     break;

                  default:
                     std::cout << "  vts - ISDFileType is unknown! "
                        << std::endl;
               }                        // end switch
               try
               {
                  tsmWarn = stubPlugin->convertISDToSensorModelState
                     (*isd,
                     std::string (param_array[0]),
                     returned_sensor_model_state);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "stubPlugin->convertISDToSensorModelState (SM57)\n";
                  error_thrown = true;
               }
//             if (tsmWarn) break;
//          }                           // end for
            end_clock = clock();

               // if no state returned
               if (returned_sensor_model_state == "" )
               {
                  if(!tsmWarn)
                  {
                     tsmWarn = new TSMWarning(TSMWarning::UNKNOWN_WARNING,
                               "Sensor Model Not Converted",
                               "VTS::processCommand");
                  }

                  text = "Not Converted";
               }
               else
               {
                  text = "Converted -  Returned State:  ";
                  text += returned_sensor_model_state;
                  sensorModelStateSaved = true;
               }

//          if (!tsmWarn)
//          {
//             text = "Converted -  Returned State:  ";
//             text += returned_sensor_model_state;
//             sensorModelStateSaved = true;
//          }
//          else
//          {
//             text = "Not Converted";
//             sensorModelStateSaved = false;
//          }

              comment.clear();
              sprintf ((char*)serialNum, "%u", instructionCount);
              sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
              if(saveLogFile )
                 recordLog(logFile,
                    (char*)serialNum,
                    (char*)&timedata,
                    comment,
                    command,
                    text);

              param_array_index = 0;
              maxReturnValuesToCompare = 0;
	    }
            exitCase();
            break;

            //----- CASE getManufacturer  MMLD
         case SP_GET_MANUFACTURER:
            logCommand = false;

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getManufacturer"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i = 0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = stubPlugin->getManufacturer(man_name);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "stubPlugin->getManufacturer (SM58)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: " << std::endl;
               std::cout << "        manufacturer_name=" << man_name
                         << std::endl;
               std::cout << std::endl;
            }

            comment.clear();
            sprintf ((char*)serialNum, "%u", instructionCount);
            sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
            text = man_name;
            if(saveLogFile )
               recordLog(logFile,
                  (char*)serialNum,
                  (char*)&timedata,
                  comment,
                  command,
                  text);

            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;
            /////////////////  SMSS section

            //----- CASE getNSensorModels  MMLD
         case SP_GET_N_SENSOR_MODEL:
            logCommand = false;

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getNSensorModels"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i = 0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = stubPlugin->getNSensorModels(n_sensor_models);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "stubPlugin->getNSensorModels (SM59)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: " << std::endl;
               std::cout << "        n_sensor_models=" << n_sensor_models
                  << std::endl;
               std::cout << std::endl;
            }

            comment.clear();
            sprintf ((char*)serialNum, "%u", instructionCount);
            sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
            sprintf (tempStr, "%u", n_sensor_models);
            text = tempStr;

            if(saveLogFile )
               recordLog(logFile,
                  (char*)serialNum,
                  (char*)&timedata,
                  comment,
                  command,
                  text);

            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE getReleaseDate  MMLD
         case SP_GET_RELEASE_DATE:
            logCommand = false;

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getReleaseDate"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i = 0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = stubPlugin->getReleaseDate(relDate);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "stubPlugin->getReleaseDate (SM60)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: " << std::endl;
               std::cout << "        release_date=" << relDate << std::endl;
               std::cout << std::endl;
            }

            comment.clear();
            sprintf ((char*)serialNum, "%u", instructionCount);
            sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
            text = relDate;
            if(saveLogFile )
               recordLog(logFile,
                  (char*)serialNum,
                  (char*)&timedata,
                  comment,
                  command,
                  text);

            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE getSensorModelName1  MMLD
         case SP_GET_SENSOR_MODEL_NAME1:
            logCommand = false;
            sensor_model_index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getSensorModelName1"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "        sensor_model_index="
                  << sensor_model_index << std::endl;
            }

            initCase();
            for(i = 0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = stubPlugin->getSensorModelName
                     (sensor_model_index,
                     smName);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "stubPlugin->getSensorModelName (SM61)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: " << std::endl;
               std::cout << "        sensor_model_name=" << smName << std::endl;
               std::cout << std::endl;
            }

            comment.clear();
            sprintf ((char*)serialNum, "%u", instructionCount);
            sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
            text = smName;
            if(saveLogFile )
               recordLog(logFile,
                  (char*)serialNum,
                  (char*)&timedata,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE getTargetModelVersion MTLD
         case SP_GET_SENSOR_MODEL_VERSION:
         {

            logCommand = false;
            std::string sensor_model = param_array[0];

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling "
                  << "TargetPlugin::TSMsensorModel::getsensorModelVersion"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      SensorModelName =" << sensor_model
                  << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i = 0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     stubPlugin->getSensorModelVersion(sensor_model,
                     ret_int);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
                  break;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "SensorPlugin::stubPlugin->getSensorModelVersion (SP78)\n";
                  error_thrown = true;
                  break;
               }
               if (tsmWarn || error_thrown) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: " << std::endl;
               std::cout << "        SensorModelVersion ="
                  << ret_int << std::endl;
               std::cout << std::endl;
            }

            comment.clear();
            sprintf ((char*)serialNum, "%u", instructionCount);
            sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
            sprintf (tempStr, "%u , \0", ret_int);
            text = tempStr;
            if(saveLogFile )
               recordLog(logFile,
                  serialNum,
                  timedata,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;

            exitCase();
            break;
         }

         //----- CASE getSensorModelNameFromSensorModelState MMLD
         case SP_GET_SENSOR_MODEL_NAME_FROM_SENSOR_MODEL_STATE:
            logCommand = false;

            if(debugFlag)
            {
               std::cout
                  << std::endl
                  << "  vts - Calling "
                  << "getSensorModelNameFromSensorModelState"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      sensor_model_state="
                  << param_array[0]
                  << std::endl;
            }
            initCase();
            for(i = 0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     stubPlugin->getSensorModelNameFromSensorModelState
                     (std::string(returned_sensor_model_state),
                     smName);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "stubPlugin->getSensorModelNameFromSensorModelState "
                     << "(SM62)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "        sensor_model_name="
                  << smName << std::endl;
               std::cout << std::endl;
            }

            comment.clear();
            sprintf ((char*)serialNum, "%u", instructionCount);
            sprintf ((char*)timedata, "%u,%u",
               start_clock, end_clock);
            text = smName;
            if(saveLogFile )
               recordLog(logFile,
                  (char*)serialNum,
                  (char*)&timedata,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE getCollectionIdentifier
         case SP_GET_COLLECTION_IDENTIFIER:
         {
            logCommand = false;
            logStringCommand = true;

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getCollectionIdentifier"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            std::string collectionId;
            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getCollectionIdentifier
                     (collectionId);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getCollectionIdentifier (SM64)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      collectionId=" << collectionId
                  << std::endl;
               std::cout << std::endl;
            }

            act_val_string.push_back(collectionId);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }                              // end case
         //----- CASE isParameterShareable
         case SP_IS_PARAMETER_SHAREABLE://{
            index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling isParameterShareable"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      index=" << index << std::endl;
            }

            initCase();
            for (i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->isParameterShareable
                     (index, return_bool1);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "isParameterShareable (SM65)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      value=" << return_bool1 << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 1;
            maxReturnValuesToCompare = 1;
            act_val[0]=(double)return_bool1;
            exitCase();
            break;

            //----- CASE getParameterSharingCriteria
         case SP_GET_PARAMETER_SHARING_CRITERIA:
            index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getParameterSharingCriteria"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      index=" << index << std::endl;
            }

            double allowableTimeDelta;

            initCase();
            for (i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getParameterSharingCriteria(
                     index,
                     return_bool1,
                     return_bool2,
                     return_bool3,
                     return_bool4,
                     return_bool5,
                     return_bool6,
                     allowableTimeDelta);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "getParameterSharingCriteria (SM66)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      requireModelNameMatch="
                  << return_bool1
                  << std::endl;
               std::cout << "      requireSensorIDMatch="
                  << return_bool2
                  << std::endl;
               std::cout << "      requirePlatformIDMatch="
                  << return_bool3
                  << std::endl;
               std::cout << "      requireCollectionIDMatch="
                  << return_bool4
                  << std::endl;
               std::cout << "      requireTrajectoryIDMatch="
                  << return_bool5
                  << std::endl;
               std::cout << "      requireDateTimeMatch="
                  << return_bool6
                  << std::endl;
               std::cout << "      allowableTimeDelta="
                  << allowableTimeDelta
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 1;
            maxReturnValuesToCompare = 7;
            act_val[0]=(double)return_bool1;
            act_val[1]=(double)return_bool2;
            act_val[2]=(double)return_bool3;
            act_val[3]=(double)return_bool4;
            act_val[4]=(double)return_bool5;
            act_val[5]=(double)return_bool6;
            act_val[6]=allowableTimeDelta;
            exitCase();
            break;
         default:
            command_found = false;
      }                                 // end switch makemodellist done
   }                                    // if MMLD . . .
   ///// END MMMMMAKEMODELLISTDONE SECTION

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
                       std::vector<std::string> param_array)
{
   int i;                              //----- Loop Variable
   bool command_found = true;
   int index;

   double x = 0.0, y = 0.0, z = 0.0;
   double line = 0.0, sample = 0.0, time_of_imaging = 0.0, height = 0.0;
   double groundAccuracy[9] = {0.0, 0.0, 0.0,   0.0, 0.0, 0.0,   0.0, 0.0, 0.0};
   double desired_precision = 0.0, achieved_precision = 0.0;

   double groundCovariance[9] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
   double imageCovariance[4] = {0.0, 0.0, 0.0, 0.0};
   double heightVariance = 0.0;
   double locus[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
   double partials[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
   double covarianceErr[4] = { 0.0, 0.0, 0.0, 0.0 }; // Covariance error
   //                                                   mensuration
   //                                                   matrix in
   //                                                   getUnmodeledError
   double pt1Line = 0.0, pt1Sample = 0.0, pt2Line = 0.0, pt2Sample = 0.0;
   double crossCovarianceErr[4] = { 0.0, 0.0, 0.0, 0.0 }; // Cross covariance
   //                                                        error mensuration
   //                                                        matrix in
   //                                                        getUnmodeledCrossCovariance

   double line_partial = 1.0;
   double sample_partial = 2.0;
   int index1 = 1, index2 = 2;
   int num_lines=0,  num_samples=0;
   //int n_sensor_models=0;
   std::vector<double> line_partials;
   std::vector<double> sample_partials;

   int ret_int;
   double return_double = 1.0;
   double covariance = 24.0;
   double time_count = 0.0;
   double vx = 0.0, vy = 0.0, vz = 0.0;
   double value = 17.0;
   std::vector<double> ret_list_double;

   std::string smName;
   std::string trajectoryId = "";
   std::string date_and_time = "";
   std::string paramName = "";

   int sensor_model_version = 0;
   int target_model_version = 0;
   int csm_version = 0;
   int NumParams = 0;                   // Output of getNumParameters.

   double direction_x = 0.0;
   double direction_y = 0.0;
   double direction_z = 0.0;

   double minAltitude = -99000.0;
   double maxAltitude =  99000.0;
   double minRow = -99000.0;
   double maxRow =  99000.0;
   double minCol = -99000.0;
   double maxCol =  99000.0;

   TSMMisc::Param_CharType parameterType = TSMMisc::NONE;
   TSMMisc::Param_CharType return_parameterType = TSMMisc::NONE;

   ///////////////////  CCCCCCCCCCONSTRUCTMODELDONE SECTION
   if( sensorModel == NULL )
   {
      std::cout << NO_SENSOR_MODEL_ERROR << std::endl;
   }
   else
   {
      command_found = true;
      switch (commandNumber)
      {
         //----- CASE groundToImage -- 1  MMLD CMD
         case SM_GROUND_TO_IMAGE1:
            x = atof(param_array[0].c_str());
            y = atof(param_array[1].c_str());
            z = atof(param_array[2].c_str());

            initCase();
            if ((param_array.size() < 4) ||
               (param_array[3] == "-c"))
            {
               // Use defaulted desired precision;
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn = sensorModel->groundToImage
                        (x,
                        y,
                        z,
                        line,
                        sample,
                        achieved_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << "groundToImage1 (SM1)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[3].c_str());
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn = sensorModel->groundToImage
                        (x,
                        y,
                        z,
                        line,
                        sample,
                        achieved_precision,
                        desired_precision );
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << "groundToImage1 (SM1)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }                        // else do not use default
               end_clock = clock();
            }

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      line=" << line << std::endl;
               std::cout << "      sample=" << sample << std::endl;
               std::cout << "      achieved_precision="
                  << achieved_precision << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 4;      //location of 1st CL switch
                                        // Maximum number of
            maxReturnValuesToCompare = 3;
            // comparisons switches

            act_val[0]=line;
            act_val[1]=sample;
            act_val[2]=achieved_precision;
            exitCase();
            break;

            //----- CASE groundToImage -- 2  MMLD CMD
         case SM_GROUND_TO_IMAGE2:
            x = atof(param_array[0].c_str());
            y = atof(param_array[1].c_str());
            z = atof(param_array[2].c_str());
            groundCovariance[0] = atof(param_array[3].c_str());
            groundCovariance[1] = atof(param_array[4].c_str());
            groundCovariance[2] = atof(param_array[5].c_str());
            groundCovariance[3] = atof(param_array[6].c_str());
            groundCovariance[4] = atof(param_array[7].c_str());
            groundCovariance[5] = atof(param_array[8].c_str());
            groundCovariance[6] = atof(param_array[9].c_str());
            groundCovariance[7] = atof(param_array[10].c_str());
            groundCovariance[8] = atof(param_array[11].c_str());

            initCase();
            if ((param_array.size() < 13) ||
               (param_array[12] == "-c"))
            {
               // Use defaulted desired precision;
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn = sensorModel->groundToImage
                        (x,
                        y,
                        z,
                        groundCovariance,
                        line,
                        sample,
                        imageCovariance,
                        achieved_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << "groundToImage2 (SM2)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[12].c_str());
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn = sensorModel->groundToImage
                        (x,
                        y,
                        z,
                        groundCovariance,
                        line,
                        sample,
                        imageCovariance,
                        achieved_precision,
                        desired_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << " groundToImage2 (SM2)\n";
                     error_thrown = true;
                  }
                  end_clock = clock();
                  if (tsmWarn) break;
               }
            }
            if(debugFlag)
            {
               std::cout << "      x,y,z=" << x  << "," << y << "," << z
                  << std::endl;
               std::cout << "      desired_precision="
                  << desired_precision
                  << std::endl;
               std::cout << "      groundCovariance="
                  << groundCovariance[0] << ","
                  << groundCovariance[1] << ","
                  << groundCovariance[2] << std::endl;;
               std::cout << "                       "
                  << groundCovariance[3]
                  << ","
                  << groundCovariance[4] << ","
                  << groundCovariance[5]
                  << std::endl;
               std::cout << "                       "
                  << groundCovariance[6]
                  << ","
                  << groundCovariance[7] << ","
                  << groundCovariance[8]
                  << std::endl;
            }

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      line=" << line << std::endl;
               std::cout << "      sample=" << sample << std::endl;
               std::cout << "      imageCovariance="
                  << imageCovariance[0]
                  << "," << imageCovariance[1]  << std::endl;
               std::cout << "                      "
                  << imageCovariance[2]
                  << "," << imageCovariance[3]  << std::endl;
               std::cout << "      achieved_precision="
                  << achieved_precision
                  << std::endl;
               std::cout << std::endl;
            }
            param_array_index = 13;     //location of first command line switch
                                        // Maximum number of
            maxReturnValuesToCompare = 7;
            // comparisons switches
            act_val[0]=line;
            act_val[1]=sample;
            act_val[2]=achieved_precision;
            act_val[3]=imageCovariance[0];
            act_val[4]=imageCovariance[1];
            act_val[5]=imageCovariance[2];
            act_val[6]=imageCovariance[3];
            exitCase();
            break;

            //----- CASE imageToGround form 1  MMLD CMD
         case SM_IMAGE_TO_GROUND1:
            line = atof(param_array[0].c_str());
            sample = atof(param_array[1].c_str());
            height = atof(param_array[2].c_str());
            initCase();
            if ((param_array.size() < 4) ||
               (param_array[3] == "-c"))
            {
               // Use defaulted desired precision;
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn = sensorModel->imageToGround(
                        line,
                        sample,
                        height,
                        x,
                        y,
                        z,
                        achieved_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << "imageToGround1 (SM3)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[3].c_str());
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn =
                        sensorModel->imageToGround(line, sample, height,
                        x,y, z,
                        achieved_precision,
                        desired_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << "imageToGround1 (SM3)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout
                  << std::endl
                  << "  vts - Calling imageToGround form 1"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      line=" << line << std::endl;
               std::cout << "      sample=" << sample << std::endl;
               std::cout << "      height=" << height << std::endl;
               std::cout << "      desired_precision=" << desired_precision
                  << std::endl;
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      x,y,z=" << x  << "," << y << ","
                  << z
                  << std::endl;
               std::cout << "      achieved_precision="
                  << achieved_precision << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 4;      //location of first command line switch
                                        // Maximum number of
            maxReturnValuesToCompare = 4;
            // comparisons switches
            act_val[0]=x;
            act_val[1]=y;
            act_val[2]=z;
            act_val[3]=achieved_precision;
            exitCase();
            break;

            //----- CASE imageToGround form 2  MMLD CMD
         case SM_IMAGE_TO_GROUND2:
            line = atof(param_array[0].c_str());
            sample = atof(param_array[1].c_str());
            imageCovariance[0] = atof(param_array[2].c_str());
            imageCovariance[1] = atof(param_array[3].c_str());
            imageCovariance[2] = atof(param_array[4].c_str());
            imageCovariance[3] = atof(param_array[5].c_str());
            height = atof(param_array[6].c_str());
            heightVariance = atof(param_array[7].c_str());
            initCase();

            if ((param_array.size() < 9) ||
               (param_array[8] == "-c"))
            {
               // Use defaulted desired precision;
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn =
                        sensorModel->imageToGround(line, sample,
                        imageCovariance,
                        height,
                        heightVariance,
                        x, y, z,
                        groundCovariance,
                        achieved_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << "imageToGround2 (SM4)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[8].c_str());
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn =
                        sensorModel->imageToGround(
                        line,
                        sample,
                        imageCovariance,
                        height,
                        heightVariance,
                        x,
                        y,
                        z,
                        groundCovariance,
                        achieved_precision,
                        desired_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR  << "imageToGround2 (SM4)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
               end_clock = clock();
            }
            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling imageToGround form 2"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      line=" << line << std::endl;
               std::cout << "      sample=" << sample << std::endl;
               std::cout << "      imageCovariance="
                  << imageCovariance[0]
                  << ","
                  << imageCovariance[1]  << std::endl;
               std::cout << "                      "
                  << imageCovariance[2]
                  << ","
                  << imageCovariance[3]  << std::endl;
               std::cout << "      height=" << height << std::endl;
               std::cout << "      heightVariance=" << heightVariance
                  << std::endl;
               std::cout << "      desired_precision="
                  << desired_precision
                  << std::endl;
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      x,y,z=" << x  << "," << y << ","
                  << z
                  << std::endl;
               std::cout << "      groundCovariance="
                  << groundCovariance[0] << ","
                  << groundCovariance[1]
                  << ","
                  << groundCovariance[2] << std::endl;;
               std::cout << "                       "
                  << groundCovariance[3]
                  << ","
                  << groundCovariance[4] << ","
                  << groundCovariance[5]  << std::endl;
               std::cout << "                       "
                  << groundCovariance[6]
                  << "," << groundCovariance[7] << ","
                  << groundCovariance[8]
                  << std::endl;
               std::cout << "      achieved_precision="
                  << achieved_precision
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 9;
            maxReturnValuesToCompare = 13;
            act_val[0]=x;
            act_val[1]=y;
            act_val[2]=z;
            act_val[3]=achieved_precision;
            act_val[4]=groundCovariance[0];
            act_val[5]=groundCovariance[1];
            act_val[6]=groundCovariance[2];
            act_val[7]=groundCovariance[3];
            act_val[8]=groundCovariance[4];
            act_val[9]=groundCovariance[5];
            act_val[10]=groundCovariance[6];
            act_val[11]=groundCovariance[7];
            act_val[12]=groundCovariance[8];
            exitCase();
            break;

            //----- CASE imageToProximateImagingLocus MMLD CMD
         case SM_IMAGE_TO_PROXIMATE_IMAGING_LOCUS:
            line = atof(param_array[0].c_str());
            sample = atof(param_array[1].c_str());
            x = atof(param_array[2].c_str());
            y = atof(param_array[3].c_str());
            z = atof(param_array[4].c_str());
            initCase();
            if ((param_array.size() < 6) ||
               (param_array[5] == "-c"))
            {
               // Use defaulted desired precision;
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn =
                        sensorModel->imageToProximateImagingLocus
                        (
                        line,
                        sample,
                        x,
                        y,
                        z,
                        locus,
                        achieved_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << " imageToProximateImagingLocus (SM5)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[5].c_str());
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn =
                        sensorModel->imageToProximateImagingLocus
                        (
                        line,
                        sample,
                        x,
                        y,
                        z,
                        locus,
                        achieved_precision,
                        desired_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << "imageToProximateImagingLocus (SM5)\n";

                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
               end_clock = clock();
            }
            if(debugFlag)
            {
               std::cout
                  << std::endl
                  << "  vts - Calling imageToProximateImagingLocus"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      line=" << line << std::endl;
               std::cout << "      sample=" << sample << std::endl;
               std::cout << "      x,y,z=" << x  << "," << y << ","
                  << z
                  << std::endl;
               std::cout << "      desired_precision="
                  << desired_precision
                  << std::endl;
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      locus=" << locus[0]  << ","
                  << locus[1]
                  << "," << locus[2] << std::endl;
               std::cout << "            " << locus[3]  << ","
                  << locus[4]
                  << "," << locus[5] << std::endl;
               std::cout << "      achieved_precision="
                  << achieved_precision
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 6;
            maxReturnValuesToCompare = 7;
            act_val[0]=achieved_precision;
            act_val[1]=locus[0];
            act_val[2]=locus[1];
            act_val[3]=locus[2];
            act_val[4]=locus[3];
            act_val[5]=locus[4];
            act_val[6]=locus[5];
            exitCase();
            break;

            //----- CASE imageToRemoteImagingLocus   MMLD CMD
         case SM_IMAGE_TO_REMOTE_IMAGING_LOCUS:
            line = atof(param_array[0].c_str());
            sample = atof(param_array[1].c_str());
            initCase();
            if ((param_array.size() < 3) ||
               (param_array[2] == "-c"))
            {
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn =
                        sensorModel->imageToRemoteImagingLocus(
                        line,
                        sample,
                        locus,
                        achieved_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << "imageToRemoteImagingLocus (SM6)\n";

                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[2].c_str());
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn =
                        sensorModel->imageToRemoteImagingLocus(
                        line,
                        sample,
                        locus,
                        achieved_precision,
                        desired_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << "groundToImage2 (SM2)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
               end_clock = clock();
            }

            if(debugFlag)
            {
               std::cout
                  << std::endl
                  << "  vts - Calling imageToRemoteImagingLocus"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      line=" << line << std::endl;
               std::cout << "      sample=" << sample << std::endl;
               std::cout << "      desired_precision=" << desired_precision
                  << std::endl;
               std::cout
                  << "    vts - Returned values are: "
                  << std::endl;
               std::cout
                  << "      locus=" << locus[0]  << ","
                  << locus[1]
                  << "," << locus[2] << std::endl;
               std::cout << "            " << locus[3]  << ","
                  << locus[4]
                  << "," << locus[5] << std::endl;
               std::cout << "      achieved_precision="
                  << achieved_precision << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 3;
            maxReturnValuesToCompare = 7;
            act_val[0]=achieved_precision;
            act_val[1]=locus[0];
            act_val[2]=locus[1];
            act_val[3]=locus[2];
            act_val[4]=locus[3];
            act_val[5]=locus[4];
            act_val[6]=locus[5];
            exitCase();
            break;

            //----- CASE computeGroundPartials MMLD CMD
         case SM_COMPUTE_GROUND_PARTIALS:
            x = atof(param_array[0].c_str());
            y = atof(param_array[1].c_str());
            z = atof(param_array[2].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling computeGroundPartials"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      x,y,z=" << x  << "," << y
                  << "," << z
                  << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = sensorModel->computeGroundPartials(
                     x,
                     y,
                     z,
                     partials);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "computeGroundPartials (SM7)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout
                  << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      partials=" << partials[0]
                  << ","
                  << partials[1]  << "," << partials[2]
                  << std::endl;
               std::cout << "               " << partials[3]
                  << ","
                  << partials[4]  << "," << partials[5]
                  << std::endl;
               std::cout << std::endl;
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

            //----- CASE computeSensorPartials form 1 MMLD CMD
         case SM_COMPUTE_SENSOR_PARTIALS1:

            index = atoi(param_array[0].c_str());
            x = atof(param_array[1].c_str());
            y = atof(param_array[2].c_str());
            z = atof(param_array[3].c_str());
            initCase();
            if ((param_array.size() < 5) ||
               (param_array[4] == "-c"))
            {
               // Use defaulted desired precision;
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn = sensorModel->computeSensorPartials
                        (
                        index,
                        x,
                        y,
                        z,
                        line_partial,
                        sample_partial,
                        achieved_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << "computeSensorPartials1 (SM8)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[4].c_str());
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn = sensorModel->computeSensorPartials
                        (
                        index,
                        x,
                        y,
                        z,
                        line_partial,
                        sample_partial,
                        achieved_precision,
                        desired_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << "computeSensorPartials1 (SM8)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
               end_clock = clock();
            }
            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling computeSensorPartials form 1"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      index=" << index << std::endl;
               std::cout << "      x,y,z=" << x  << ","
                  << y << "," << z
                  << std::endl;
               std::cout << "      desired_precision="
                  << desired_precision
                  << std::endl;
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      line_partial="
                  << line_partial
                  << std::endl;
               std::cout << "      sample_partial="
                  << sample_partial
                  << std::endl;
               std::cout << "      achieved_precision="
                  << achieved_precision << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 5;
            maxReturnValuesToCompare = 3;
            act_val[0]=line_partial;
            act_val[1]=sample_partial;
            act_val[2]=achieved_precision;
            exitCase();
            break;

            //----- CASE computeSensorPartials form 2  MMLD CMD
         case SM_COMPUTE_SENSOR_PARTIALS2:
            index = atoi(param_array[0].c_str());
            line = atof(param_array[1].c_str());
            sample = atof(param_array[2].c_str());
            x = atof(param_array[3].c_str());
            y = atof(param_array[4].c_str());
            z = atof(param_array[5].c_str());
            initCase();
            if ((param_array.size() < 7) ||
               (param_array[6] == "-c"))
            {
               // Use defaulted desired precision;
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn =
                        sensorModel->computeSensorPartials(
                        index,
                        line,
                        sample,
                        x,
                        y,
                        z,
                        line_partial,
                        sample_partial,
                        achieved_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR
                        << "computeSensorPartials (SM9)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
            }
            else
            {
               // Do not use defaulted desired precision;
               desired_precision = atof(param_array[6].c_str());
               for(i=0; i < repeat_count; i++)
               {
                  try
                  {
                     tsmWarn =
                        sensorModel->computeSensorPartials(
                        index,
                        line,
                        sample,
                        x,
                        y,
                        z,
                        line_partial,
                        sample_partial,
                        achieved_precision,
                        desired_precision);
                  }
                  catch (TSMError err)
                  {
                     error_thrown = true;
                     global_err = err;
                  }
                  catch ( ... )
                  {
                     std::cout
                        << UNKNOWN_ERROR_STR << "computeSensorPartials (SM9)\n";
                     error_thrown = true;
                  }
                  if (tsmWarn) break;
               }
               end_clock = clock();
            }
            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling computeSensorPartials form 2"
                  << std::endl;
               std::cout << "    Passing values are: " << std::endl;
               std::cout << "      index=" << index << std::endl;
               std::cout << "      line=" << line << std::endl;
               std::cout << "      sample=" << sample << std::endl;
               std::cout << "      x,y,z=" << x  << "," << y << "," << z
                  << std::endl;
               std::cout << "      desired_precision=" << desired_precision
                  << std::endl;
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      line_partial="
                  << line_partial
                  << std::endl;
               std::cout << "      sample_partial="
                  << sample_partial
                  << std::endl;
               std::cout << "      achieved_precision="
                  << achieved_precision << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 7;
            maxReturnValuesToCompare = 3;

            act_val[0]=line_partial;
            act_val[1]=sample_partial;
            act_val[2]=achieved_precision;
            exitCase();
            break;

            //----- CASE getCovarianceModel  MMLD CMD
         case SM_GET_COVARIANCE_MODEL:

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getCovarianceModel"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            tsm_CovarianceModel *covModel;
			covModel = NULL;
            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getCovarianceModel(covModel);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getCovarianceModel (SM10)\n";
                  error_thrown = true;
               }

               if (!covModel)
               {
                  std::cout
                     << "\ntsm_CovarianceModel object NOT found\n";
                  act_val[0]=0;         // Signal to log file to
                  // indicate failure.
                  exitCase();
                  break;
               }
               else
               {
                  std::cout
                     << "\ntsm_CovarianceModel object found\n";
                  act_val[0]=1;         // Signal to log file to
                  // indicate success.
               }
               if (tsmWarn) break;
            }

            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "     function returned (1=pointer, 0=NULL)="
                  << act_val[0]
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            // This is a special case because there is no real
            // return value. The return value is set to 1 for a
            // returned pointer and to 0 for a returned NULL that
            // indicates no corrolations between images. The
            // returned NULL is NOT a failure.

            exitCase();
            break;

            //----- CASE getUnmodeledError MMLD CMD
         case SM_GET_UNMODELED_ERROR:
            line = atof(param_array[0].c_str());
            sample = atof(param_array[1].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getUnmodeledError"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      line,sample="
                  << line  << "," << sample
                  << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = sensorModel->getUnmodeledError(
                     line,
                     sample,
                     covarianceErr);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getUnmodeledError (SM11)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout
                  << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      covarianceErr="
                  << covarianceErr[0]
                  << ","
                  << covarianceErr[1]  << ","
                  << covarianceErr[2]
                  << "," << covarianceErr[3]
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 2;
            maxReturnValuesToCompare = 4;
            act_val[0]=covarianceErr[0];
            act_val[1]=covarianceErr[1];
            act_val[2]=covarianceErr[2];
            act_val[3]=covarianceErr[3];

            exitCase();
            break;

            //----- CASE getUnmodeledCrossCovariance MMLD CMD
         case SM_GET_UNMODELED_CROSS_COVARIANCE:
            pt1Line = atof(param_array[0].c_str());
            pt1Sample = atof(param_array[1].c_str());
            pt2Line = atof(param_array[2].c_str());
            pt2Sample = atof(param_array[3].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getUnmodeledCrossCovariance"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      pt1Line,pt1Sample="
                  << pt1Line  << "," << pt1Sample
                  << std::endl;
               std::cout << "      pt2Line,pt2Sample="
                  << pt2Line  << "," << pt2Sample
                  << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getUnmodeledCrossCovariance(
                     pt1Line,
                     pt1Sample,
                     pt2Line,
                     pt2Sample,
                     crossCovarianceErr);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "getUnmodeledCrossCovariance (SM12)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      crossCovarianceErr="
                  << crossCovarianceErr[0]
                  << ","
                  << crossCovarianceErr[1]
                  << "," << crossCovarianceErr[2]
                  << "," << crossCovarianceErr[3]
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 4;
            maxReturnValuesToCompare = 4;
            act_val[0]=crossCovarianceErr[0];
            act_val[1]=crossCovarianceErr[1];
            act_val[2]=crossCovarianceErr[2];
            act_val[3]=crossCovarianceErr[3];

            exitCase();
            break;

            //----- CASE getCurrentParameterCovariance  MMLD CMD
         case SM_GET_CURRENT_PARAMETER_COVARIANCE:
            index1 = atoi(param_array[0].c_str());
            index2 = atoi(param_array[1].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getCurrentParameterCovariance"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      index1=" << index1 << std::endl;
               std::cout << "      index2=" << index2 << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getCurrentParameterCovariance(
                     index1,
                     index2,
                     return_double);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "getCurrentParameterCovariance (SM13)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "        covariance="
                  << return_double
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 2;
            maxReturnValuesToCompare = 1;
            act_val[0]=return_double;
            exitCase();
            break;

            //----- CASE setCurrentParameterCovariance  MMLD CMD
         case SM_SET_CURRENT_PARAMETER_COVARIANCE:
            index1 = atoi(param_array[0].c_str());
            index2 = atoi(param_array[1].c_str());
            covariance = atof(param_array[2].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling setCurrentParameterCovariance"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      index1=" << index1 << std::endl;
               std::cout << "      index2=" << index2 << std::endl;
               std::cout << "      covariance=" << covariance
                  << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->setCurrentParameterCovariance(
                     index1,
                     index2,
                     covariance);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "setCurrentParameterCovariance (SM14)\n";
                  error_thrown = true;
               }

               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      NONE" << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 3;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE setOriginalParameterCovariance MMLD CMD
         case SM_SET_ORIGINAL_PARAMETER_COVARIANCE:
            index1 = atoi(param_array[0].c_str());
            index2 = atoi(param_array[1].c_str());
            covariance = atof(param_array[2].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling setOriginalParameterCovariance"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      index1=" << index1 << std::endl;
               std::cout << "      index2=" << index2 << std::endl;
               std::cout << "      covariance="
                  << covariance << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->setOriginalParameterCovariance(
                     index1,
                     index2,
                     covariance);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "setOriginalParameterCovariance (SM15)\n";
                  error_thrown = true;
               }

               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      NONE" << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 3;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE getOriginalParameterCovariance  MMLD CMD
         case SM_GET_ORIGINAL_PARAMETER_COVARIANCE:
            index1 = atoi(param_array[0].c_str());
            index2 = atoi(param_array[1].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getOriginalParameterCovariance"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      index1=" << index1 << std::endl;
               std::cout << "      index2=" << index2 << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn  =
                     sensorModel->getOriginalParameterCovariance(
                     index1,
                     index2,
                     return_double);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "getOriginalParameterCovariance (SM16)\n";
                  error_thrown = true;
               }

               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      covariance=" << return_double
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 2;
            maxReturnValuesToCompare = 1;

            act_val[0]=return_double;

            exitCase();
            break;

            //----- CASE getTrajectoryIdentifier MMLD CMD
         case SM_GET_TRAJECTORY_IDENTIFIER:
            logCommand = false;
            logStringCommand = true;

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getTrajectoryIdentifier"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getTrajectoryIdentifier(
                     trajectoryId);

               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "getTrajectoryIdentifier\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      trajectoryId=" << trajectoryId
                  << std::endl;
               std::cout << std::endl;
            }

            act_val_string.push_back(trajectoryId);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;

            //----- CASE getReferenceDateAndTime MMLD CMD
         case SM_GET_REFERENCE_DATE_AND_TIME:
            logCommand = false;
            logStringCommand = true;

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getReferenceDateAndTime"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getReferenceDateAndTime(
                     date_and_time);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "getReferenceDateAndTime (SM18)\n";
                  error_thrown = true;
               }

               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      date_and_time="
                  << date_and_time << std::endl;
               std::cout << std::endl;
            }

            act_val_string.push_back(date_and_time);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;

            //----- CASE getImageTime MMLD CMD
         case SM_GET_IMAGE_TIME:
            line = atof(param_array[0].c_str());
            sample = atof(param_array[1].c_str());

            if(debugFlag)
            {
               std::cout
                  << std::endl
                  << "  vts - Calling getImageTime"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      line=" << line << std::endl;
               std::cout << "      sample=" << sample << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = sensorModel->getImageTime(
                     line,
                     sample,
                     time_count);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getImageTime (SM19)\n";
                  error_thrown = true;
               }

               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      time=" << time_count
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 2;
            maxReturnValuesToCompare = 1;
            act_val[0]=time_count;
            exitCase();
            break;

            //----- CASE getSensorPosition form 1 MMLD CMD
         case SM_GET_SENSOR_POSITION1:
            line = atof(param_array[0].c_str());
            sample = atof(param_array[1].c_str());

            if(debugFlag)
            {
               std::cout
                  << std::endl
                  << "  vts - Calling getSensorPosition1"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      line=" << line << std::endl;
               std::cout << "      sample=" << sample << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = sensorModel->getSensorPosition(
                     line,
                     sample,
                     x,
                     y,
                     z);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getSensorPosition1 (SM22)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      x,y,z=" << x  << "," << y
                  << "," << z
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 2;
            maxReturnValuesToCompare = 3;

            act_val[0]=x;
            act_val[1]=y;
            act_val[2]=z;
            exitCase();
            break;

            //----- CASE getSensorPosition form 2 MMLD CMD
         case SM_GET_SENSOR_POSITION2:
            time_of_imaging = atof(param_array[0].c_str());

            if(debugFlag)
            {
               std::cout
                  << std::endl
                  << "  vts - Calling getSensorPosition2"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      time_of_imaging="
                  << time_of_imaging << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getSensorPosition(
                     time_of_imaging,
                     x,
                     y,
                     z);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getSensorPosition2 (SM21)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      x,y,z=" << x  << "," << y
                  << "," << z
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 1;
            maxReturnValuesToCompare = 3;

            act_val[0]=x;
            act_val[1]=y;
            act_val[2]=z;
            exitCase();
            break;

            //----- CASE getSensorVelocity form 1 MMLD CMD
         case SM_GET_SENSOR_VELOCITY1:
            line = atof(param_array[0].c_str());
            sample = atof(param_array[1].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getSensorVelocity1"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      line=" << line << std::endl;
               std::cout << "      sample=" << sample << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = sensorModel->getSensorVelocity(
                     line,
                     sample,
                     vx,
                     vy,
                     vz);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getSensorVelocity1 (SM22)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      vx,vy,vz=" << vx  << ","
                  << vy << ","
                  << vz << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 2;
            maxReturnValuesToCompare = 3;
            act_val[0]=vx;
            act_val[1]=vy;
            act_val[2]=vz;

            exitCase();
            break;

            //----- CASE getSensorVelocity form 2 MMLD CMD
         case SM_GET_SENSOR_VELOCITY2:
            time_of_imaging = atof(param_array[0].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getSensorVelocity2"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      time_of_imaging="
                  << time_of_imaging << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = sensorModel->getSensorVelocity(
                     time_of_imaging,
                     vx,
                     vy,
                     vz);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getSensorVelocity2 (SM23)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      vx,vy,vz=" << vx  << ","
                  << vy << ","
                  << vz << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 1;
            maxReturnValuesToCompare = 3;
            act_val[0]=vx;
            act_val[1]=vy;
            act_val[2]=vz;
            exitCase();
            break;

            //----- CASE setCurrentParameterValue MMLD CMD
         case SM_SET_CURRENT_PARAMETER_VALUE:
            index = atoi(param_array[0].c_str());
            value = atof(param_array[1].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling setCurrentParameterValue"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      index=" << index << std::endl;
               std::cout << "      value=" << value << std::endl;
            }
            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->setCurrentParameterValue(
                     index,
                     value);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "setCurrentParameterValue (SM24)\n";
                  error_thrown = true;
               }

               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      NONE" << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE Calling getCurrentParameterValue MMLD CMD
         case SM_GET_CURRENT_PARAMETER_VALUE:
            index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getCurrentParameterValue"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      index=" << index << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getCurrentParameterValue(
                     index,
                     return_double);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "getCurrentParameterValue (SM25)\n";
                  error_thrown = true;
               }

               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      value=" << return_double
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 1;
            maxReturnValuesToCompare = 1;
            act_val[0]=return_double;

            exitCase();
            break;

            //----- CASE getParameterName MMLD CMD
         case SM_GET_PARAMETER_NAME:
            logCommand = false;
            logStringCommand = true;
            index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getParameterName"
                  << std::endl;
               std::cout << "    Passing values are:" << std::endl;
               std::cout << "      index=" << index << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =sensorModel->getParameterName
                     (index, paramName);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getParameterName (SM26)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      name = "
                  << paramName << std::endl;
               std::cout << std::endl;
            }

            act_val_string.push_back(paramName);

            param_array_index = 1;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE getNumParameters MMLD CMD
         case SM_GET_NUM_PARAMETERS:

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getNumParameters"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getNumParameters(ret_int);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getNumParameters (SM27)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      value=" << ret_int << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            NumParams = ret_int;
            act_val[0]=(double)ret_int;
            exitCase();
            break;

            //----- CASE setOriginalParameterValue  MMLD CMD
         case SM_SET_ORIGINAL_PARAMETER_VALUE:
            index = atoi(param_array[0].c_str());
            value = atof(param_array[1].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling setOriginalParameterValue"
                  << std::endl;
               std::cout << "    Passing values are: "
                  << std::endl;
               std::cout << "      index=" << index << std::endl;
               std::cout << "      value=" << value << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->setOriginalParameterValue(
                     index,
                     value);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "setOriginalParameterValue (SM28)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      NONE" << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE getOriginalParameterValue MMLD CMD
         case SM_GET_ORIGINAL_PARAMETER_VALUE:
            index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               std::cout
                  << std::endl
                  << "  vts - Calling getOriginalParameterValue"
                  << std::endl;
               std::cout << "    Passing values are:" << std::endl;
               std::cout << "      index=" << index << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getOriginalParameterValue(
                     index,
                     return_double);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getParameterName (SM29)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }

            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      value=" << return_double
                  << std::endl;
               std::cout << std::endl;
            }

            param_array_index = 1;
            maxReturnValuesToCompare = 1;
            act_val[0]=return_double;
            exitCase();
            break;

            //----- CASE setParameterType MMLD CMD
         case SM_SET_PARAMETER_TYPE:
            index = atoi(param_array[0].c_str());
            parameterType =
               (TSMMisc::Param_CharType)atoi(param_array[1].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling setParameterType"
                  << std::endl;
               std::cout << "    Passing values are:" << std::endl;
               std::cout << "      index=" << index << std::endl;
               std::cout << "      parameterType=" << parameterType
                  << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = sensorModel->setParameterType(
                     index,
                     parameterType);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "setParameterType (SM30)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE getParameterType MMLD CMD
         case SM_GET_PARAMETER_TYPE:
            index = atoi(param_array[0].c_str());

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getParameterType"
                  << std::endl;
               std::cout << "    Passing values are:" << std::endl;
               std::cout << "      index=" << index << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = sensorModel->getParameterType
                     (index,
                     return_parameterType);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getParameterType (SM31)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
               std::cout << "    vts - Returned values are: "
                  << std::endl;
               std::cout << "      parameterType="
                  << return_parameterType
                  << std::endl;
            }

            param_array_index = 1;
            maxReturnValuesToCompare = 1;
            act_val[0]=return_parameterType;
            exitCase();
            break;

            //----- CASE getPedigree  MMLD CMD
         case SM_GET_PEDIGREE:
         {
            std::string pedigree;
            logCommand = false;
            logStringCommand = true;

            if(debugFlag)
            {
               std::cout << std::endl
                  << "  vts - Calling getPedigree"
                  << std::endl;
               std::cout << "    Passing values are:" << std::endl;
               std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = sensorModel->getPedigree(pedigree);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getPedigree (SM32)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
            std::cout << "    vts - Returned values are: "
               << std::endl;
            std::cout << "      pedigree=" << pedigree
               << std::endl;
            std::cout << std::endl;
            }

            act_val_string.push_back(pedigree);

            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;
         }
         //----- CASE getImageIdentifier MMLD CMD
         case SM_GET_IMAGE_IDENTIFIER:
         {
            std::string imgid;
            logCommand = false;
            logStringCommand = true;

            if(debugFlag)
            {
            std::cout << std::endl
               << "  vts - Calling getImageIdentifier"
               << std::endl;
            std::cout << "    Passing values are:" << std::endl;
            std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getImageIdentifier(imgid);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getImageIdentifier (SM33)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }

            end_clock = clock();

            if(debugFlag)
            {
            std::cout << "    vts - Returned values are: "
               << std::endl;
            std::cout << "      imageId=" << imgid
               << std::endl;
            std::cout << std::endl;
            }

            act_val_string.push_back(imgid);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }
         //----- CASE setImageIdentifier MMLD CMD
         case SM_SET_IMAGE_IDENTIFIER:
            if(debugFlag)
            {
            std::cout << std::endl
               << "  vts - Calling setImageIdentifier"
               << std::endl;
            std::cout << "    Passing values are:" << std::endl;
            std::cout << "      imageId="
               << param_array[0] << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->setImageIdentifier(param_array[0]);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "setImageIdentifier (SM34)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
            std::cout << "    vts - Returned values are: "
               << std::endl;
            std::cout << "      NONE" << std::endl;
            std::cout << std::endl;
            }

            param_array_index = 1;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE getSensorIdentifier  MMLD CMD
         case SM_GET_SENSOR_IDENTIFIER:
         {
            std::string sensorId;
            logCommand = false;
            logStringCommand = true;

            if(debugFlag)
            {
            std::cout << std::endl
               << "  vts - Calling getSensorIdentifier"
               << std::endl;
            std::cout << "    Passing values are:" << std::endl;
            std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getSensorIdentifier(sensorId);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR <<
                     "getSensorIdentifier (SM35)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
            std::cout << "    vts - Returned values are: "
               << std::endl;
            std::cout << "      sensorId="
               << sensorId << std::endl;
            std::cout << std::endl;
            }

            act_val_string.push_back(sensorId);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }
         //----- CASE getPlatformIdentifier  MMLD CMD
         case SM_GET_PLATFORM_IDENTIFIER:
         {
            std::string platformId;
            // Calling getPlatformIdentifier.
            logCommand = false;
            logStringCommand = true;

            if(debugFlag)
            {
            std::cout << std::endl
               << "  vts - Calling getPlatformIdentifier"
               << std::endl;
            std::cout << "    Passing values are:" << std::endl;
            std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getPlatformIdentifier(platformId);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "getPlatformIdentifier (SM36)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
            std::cout << "    vts - Returned values are: "
               << std::endl;
            std::cout << "      platformId=" << platformId
               << std::endl;
            std::cout << std::endl;
            }

            act_val_string.push_back(platformId);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }
         //----- CASE getReferencePoint MMLD CMD
         case SM_GET_REFERENCE_POINT:
            // Calling getReferencePoint.
            if(debugFlag)
            {
            std::cout << std::endl
               << "  vts - Calling getReferencePoint"
               << std::endl;
            std::cout << "    Passing values are: "
               << std::endl;
            std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =sensorModel->getReferencePoint(
                     x,
                     y,
                     z);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getReferencePoint (SM37)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
            std::cout << "    vts - Returned values are:\n";
            std::cout << "      x,y,z=" << x  << ","
               << y << "," << z
               << std::endl;
            std::cout << std::endl;
            }

            param_array_index = 0;
            maxReturnValuesToCompare = 3;
            act_val[0]=x;
            act_val[1]=y;
            act_val[2]=z;
            exitCase();
            break;

            //----- CASE setReferencePoint MMLD CMD
         case SM_SET_REFERENCE_POINT:
            x = atof(param_array[0].c_str());
            y = atof(param_array[1].c_str());
            z = atof(param_array[2].c_str());

            if(debugFlag)
            {
            std::cout << std::endl
               << "  vts - Calling setReferencePoint"
               << std::endl;
            std::cout << "    Passing values are:" << std::endl;
            std::cout << "      x,y,z=" << x  << ","
               << y << "," << z
               << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->setReferencePoint(x, y, z);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "setReferencePoint (SM38)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
            std::cout << "    vts - Returned values are:" << std::endl;
            std::cout << "      NONE" << std::endl;
            std::cout << std::endl;
            }

            param_array_index = 3;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;

            //----- CASE getImageSize  MMLD CMD
         case SM_GET_IMAGE_SIZE:

            if(debugFlag)
            {
            std::cout << std::endl
               << "  vts - Calling getImageSize"
               << std::endl;
            std::cout << "    Passing values are:" << std::endl;
            std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getImageSize(
                     num_lines, num_samples);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getImageSize (SM39)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
            std::cout << "    vts - Returned values are:\n ";
            std::cout << "      num_lines="
               << num_lines << std::endl;
            std::cout << "      num_samples="
               << num_samples << std::endl;
            std::cout << std::endl;
            }

            param_array_index = 0;
            maxReturnValuesToCompare = 2;
            act_val[0]=num_lines;
            act_val[1]=num_samples;
            exitCase();
            break;

            //----- CASE getValidAltitudeRange
         case SM_GET_VALID_ALTITUDE_RANGE:

            if(debugFlag)
            {
            std::cout
               << std::endl
               << "  vts - Calling getValidAltitudeRange"
               << std::endl;
            std::cout << "    Passing values are: "
               << std::endl;
            std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = sensorModel->getValidAltitudeRange(
                     minAltitude,
                     maxAltitude);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "getValidAltitudeRange (SM43)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
            std::cout << "    vts - Returned values are: "
               << std::endl;
            std::cout << "      minAltitude="
               << minAltitude << std::endl;
            std::cout << "      maxAltitude="
               << maxAltitude << std::endl;
            std::cout << std::endl;
            }

            param_array_index = 0;
            maxReturnValuesToCompare = 2;
            act_val[0]=minAltitude;
            act_val[1]=maxAltitude;
            exitCase();
            break;

            //----- CASE getIlluminationDirection
         case SM_GET_ILLUMINATION_DIRECTION:

            x = atof(param_array[0].c_str());
            y = atof(param_array[1].c_str());
            z = atof(param_array[2].c_str());

            if(debugFlag)
            {
            std::cout << std::endl
               << "  vts - Calling getIlluminationDirection"
               << std::endl;
            std::cout << "    Passing values are:" << std::endl;
            std::cout << "      x,y,z=" << x  << ","
               << y << "," << z
               << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getIlluminationDirection(
                     x,
                     y,
                     z,
                     direction_x,
                     direction_y,
                     direction_z);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "getIlluminationDirection (SM42)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
            std::cout << "    vts - Returned values are: "
               << std::endl;
            std::cout << "      direction_x="
               << direction_x << std::endl;
            std::cout << "      direction_y="
               << direction_y << std::endl;
            std::cout << "      direction_z="
               << direction_z << std::endl;
            std::cout << std::endl;
            }

            param_array_index = 3;      //location of first command
            //                       line switch
                                        // Maximum
            maxReturnValuesToCompare = 3;
            //                               number of
            //                            comparisons
            //                            switches
            act_val[0]=direction_x;
            act_val[1]=direction_y;
            act_val[2]=direction_z;
            exitCase();
            break;

         case SM_GET_SENSOR_MODEL_STATE:
            logCommand = false;

            if(debugFlag)
            {
            std::cout << std::endl
               << "  vts - Calling getSensorModelState"
               << std::endl;
            std::cout << "    Passing values are: "<< std::endl;
            std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getSensorModelState
                     (returned_sensor_model_state);

               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "getSensorModelState (SM40)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();
            if (!tsmWarn )
            {
               text = "Saved -  Returned State:  " +
                  returned_sensor_model_state;
               sensorModelStateSaved = true;
            }
            else
            {
               text = "Not Saved";
               sensorModelStateSaved = false;
            }

            if(debugFlag)
            {
            std::cout << "    vts - Returned values are:\n";
            std::cout << "      SensorModelState="
               << returned_sensor_model_state
               << std::endl;
            std::cout << std::endl;
            }

            param_array_index = 0;
            maxReturnValuesToCompare = 0;

            sprintf ((char*)serialNum, "%u", instructionCount);
            sprintf ((char*)timedata, "%u,%u",
               start_clock, end_clock);
            comment.clear();
            if(saveLogFile )
               recordLog(logFile,
                  (char*)serialNum,
                  (char*)&timedata,
                  comment,
                  command,
                  param_array,
                  text);

            exitCase();
            break;

            //----- CASE getSensorModelName2
         case SM_GET_SENSOR_MODEL_NAME2:
         {
            logCommand = false;
            logStringCommand = true;

            if(debugFlag)
            {
            std::cout << std::endl
               << "  vts - Calling getSensorModelName2"
               << std::endl;
            std::cout << "    Passing values are:" << std::endl;
            std::cout << "      NONE" << std::endl;
            }

            std::string sensorId;
            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     sensorModel->getSensorModelName(smName);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR << "getSensorModelName (SM47)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();

            if(debugFlag)
            {
            std::cout << "    vts - Returned values are: "
               << std::endl;
            std::cout << "        sensor_model_name=" << smName
               << std::endl;
            std::cout << std::endl;
            }

            act_val_string.push_back(smName);

            param_array_index = 0;
            maxReturnValuesToCompare = 1;
            exitCase();
            break;
         }                              // end case
         default:
            command_found = false;
      }                                 // switch
   } ////// if constructModelDone section CMD
   /////////// end CCCCCCCCCCONSTRUCTMODELDONE SECTION

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
                       std::vector<std::string> param_array)
{
   int i;                              //----- Loop Variable
   bool command_found = true;

   // flag to indicate that model may be constructable
   bool constructable = false;

   if( !sensorModelStateSaved || stubPlugin == NULL )
   {
      if(sensorModelStateSaved)
         std::cout << "Sensor Model State Not Saved"
            << std::endl;
      else
         std::cout << NO_MODEL_LIST_ERROR << std::endl;
   }
   else
   {
      command_found = true;
      switch (commandNumber)
      {
         //----- CASE canSensorModelBeConstructedFromState SMSS MMLD
         case SP_CAN_SENSOR_MODEL_BE_CONSTRUCTED_FROM_STATE:
         {
            logCommand = false;

            if(debugFlag)
            {
            std::cout << std::endl
               << "  vts - Calling "
               << "canSensorModelBeConstructedFromState"
               << std::endl;
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      Plugin="  << param_array[0]
               << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn =
                     stubPlugin->canSensorModelBeConstructedFromState
                     (std::string(param_array[0]),
                     returned_sensor_model_state,
                     constructable);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "stubPlugin->canSensorModelBeConstructedFromState (SM53)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();
            text.clear();
            std::cout
               << "TSMPlugin::canSensorModelBeConstructedFromState";

            if (constructable)
            {
               std::cout << " can possibly be constructed from "
                  << param_array[0] << std::endl;
               text = "Possibly Constructable";
            }
            else
            {
               std::cout << " can not be constructed from "
                  << param_array[0] << std::endl;
               text = "NOT Constructable";
            }

            comment.clear();
            sprintf ((char*)serialNum, "%u", instructionCount);
            sprintf ((char*)timedata, "%u,%u",
               start_clock, end_clock);
            if(saveLogFile )
               recordLog(logFile,
                  (char*)serialNum,
                  (char*)&timedata,
                  comment,
                  command,
                  text);
            param_array_index = 0;
            maxReturnValuesToCompare = 0;
            exitCase();
            break;
         }                              // end case

         //----- CASE constructSensorModelFromState SMSS MMLD
         case SP_CONSTRUCT_SENSOR_MODEL_FROM_STATE:
            logCommand = false;

            if(debugFlag)
            {
            std::cout << std::endl
               << "  vts - Calling "
               << "constructSensorModelFromState"
               << std::endl;
            std::cout << "    Passing values are: " << std::endl;
            std::cout << "      NONE" << std::endl;
            }

            initCase();
            for(i=0; i < repeat_count; i++)
            {
               try
               {
                  tsmWarn = stubPlugin->constructSensorModelFromState
                     (std::string(returned_sensor_model_state),
                     (TSMSensorModel*&)sensorModel);
               }
               catch (TSMError err)
               {
                  error_thrown = true;
                  global_err = err;
               }
               catch ( ... )
               {
                  std::cout
                     << UNKNOWN_ERROR_STR
                     << "stubPlugin->constructSensorModelFromState (SM55)\n";
                  error_thrown = true;
               }
               if (tsmWarn) break;
            }
            end_clock = clock();
            if (!sensorModel)
            {
               if(!tsmWarn)
               {
                  tsmWarn = new TSMWarning(TSMWarning::UNKNOWN_WARNING,
                            "Sensor Model Not Constucted",
                            "VTS::processCommand");
               }

               text = "Not Constructed";
            }
            else
            {
               text = "Constructed";
            }
            comment.clear();
            sprintf ((char*)serialNum, "%u", instructionCount);
            sprintf ((char*)timedata, "%u,%u",
               start_clock, end_clock);
            if(saveLogFile )
               recordLog(logFile,
                  (char*)serialNum,
                  (char*)&timedata,
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
   //////////////////////  SMSS section

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
                    std::vector<std::string> param_array)
{
   int i;                               // Loop Variable
   int index = 1;                       // Number of values in param array

   int target_model_index = 0;
   double target_index = 0.0;

   std::string sensor_model_state = "Sensor Model State for Stub";

   bool command_found = false;

   logCommand = true;              // Use the default log command
   logStringCommand = false;       // Use the default string log command

   // Initialize compare parameters
   comp_answer.clear();
   pass_fail.clear();
   act_val.clear();
   act_val_string.clear();
   for(i=0; i<MAXOUTPUTPARAMETERS; i++)
   {
      comp_answer.push_back(0.0);
      pass_fail.push_back(true);
      act_val.push_back(0.0);
   }

   if (commandNumber >= BEGIN_SP_NUMBER && commandNumber <= END_SP_NUMBER)
      std::cout << "\ncommand number SP-" << commandNumber << std::endl;
   else if (commandNumber >= BEGIN_SM_NUMBER && commandNumber <= END_SM_NUMBER)
      std::cout << "\ncommand number SM-" << commandNumber << std::endl;
   else
      std::cout << "\ncommand number VTS-" << commandNumber << std::endl;

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
         command_found = processSensorModelStateCommand(commandNumber, param_array);
         break;
      default:
         command_found = false;
   }

   if(!command_found)
      std::cout << "Unable to locate command "
         << menulist[commandNumber] << std::endl;

   if( command_found && stubPlugin != NULL )
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
            std::cout << std::endl
               << "vts>    **** No parameter comparison switches"
               << std::endl;
            }
         }
         else
         {
            if(debugFlag)
            {
               std::cout << std::endl
                  << "vts>    **** Checking for parameter comparison switches "
                  << i << std::endl;
               // print the actual values for debug
               if(logStringCommand)
               {
                  for (i = 0; i< (int)act_val_string.size(); i++)
                     std::cout << "act_val_string[" << i << "] = "
                        << act_val_string[i] << std::endl;
               }
               else
               {
                  for (i = 0; i< (int)act_val.size(); i++)
                     std::cout << "act_val[" << i << "] = "
                        << act_val[i] << std::endl;
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

         sprintf ((char*)timedata, "%u,%u", start_clock, end_clock);
         sprintf ((char*)serialNum, "%u", instructionCount);
         comment.clear();
         std::vector< std::string > out_param_array;

         for (i = 0 ; i <  maxReturnValuesToCompare ; i++)
         {
            out_param_array.push_back("");
            out_param_array.push_back("");
            out_param_array.push_back("");

            if (!error_thrown)
            {
               if(logCommand)
               {
                  char stemp [1024];
                  sprintf (stemp, "%f", act_val[i]);
                  out_param_array[i*3] = stemp;

                  sprintf (stemp, "%f", comp_answer[i]);
                  out_param_array[i*3+1] = stemp;
               }
               else
                  out_param_array[i*3] = act_val_string[i];

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

         if(saveLogFile)
            recordLog(logFile,
               serialNum,
               timedata,
               comment,
               command,
               param_array,
               out_param_array);
      }                                 // End of if(logCommand)
   }                                    //  end check if makeModelListFromISD

   //---
   // Checking for TSMWarning other than NO_WARNING
   //---
   if(tsmWarn)
   {
      if(saveLogFile)
      {
         reportWarning(tsmWarn, logFile.c_str());
      }

      delete tsmWarn;
      tsmWarn = NULL;
   }

   if (error_thrown)
   {
      if(saveLogFile)
      {
         reportError(&global_err, logFile.c_str());
      }

      error_thrown = false;             // clear error indicator flag
   }
}

//*****************************************************************************
// main
//*****************************************************************************
int main(int argc, char** argv)
{
   std::string dirName;
   #ifdef _WIN32
   dirName = ".\\";
   logFile  =".\\vts_logfile.csv";
   #else
   dirName = "./";
   logFile ="./vts_logfile.csv";
   #endif

   #ifdef _DEBUG
      std::cout << "***** This code was compiled in DEBUG mode. *****"
                << std::endl;
   #endif

   int i = 0,j;
   std::string text = "";

   std::cout << std::endl;
   std::cout << "**********************************************"  << std::endl;
   std::cout << "**                                          **"  << std::endl;
   std::cout << "**        VTS version "   << VTS_VERSION;
   for(i=0;i<22-(int)VTS_VERSION.length();i++)
      std::cout << " ";
   std::cout << "**"  << std::endl;
   std::cout << "**                                          **"  << std::endl;
   std::cout << "**********************************************"  << std::endl;
   std::cout << std::endl;

   //---
   // Initialize the clock to record system clock tics from this point.
   //---

   initMenuList();
   vHelpDataValues = loadHelpFile();
   time_t aclock;
   aclock = time(NULL);

   std::cout << std::endl
      << "Program Start Time = " << ctime(&aclock)
      << std::endl;

   std::cout << std::endl;
   std::cout
      << "For this computer the number of tics-per-second (CLOCKS_PER_SEC) = "
      << CLOCKS_PER_SEC << std::endl;
   std::cout << std::endl;

   //---
   // Select the log file.
   //---
   // get logfile name
   std::string inputFile;
   getLogfileName(dirName, &inputFile);
   if(inputFile != "")
      logFile = inputFile;

   std::cout << "Output logged to file " << logFile << std::endl;

   #ifdef _WIN32
   for (std::string::iterator pos = dirName.begin();
        pos != dirName.end();
        pos ++)
      if (*pos == '/')
         *pos = '\\';
   #else
   for (std::string::iterator pos = dirName.begin();
        pos != dirName.end();
        pos ++)
      if (*pos == '\\')
         *pos = '/';
   #endif

   //---
   // Write a heading to the log file.
   //---
   text.assign ("TSM Verification Test System " + VTS_VERSION);
   recordLog (logFile, text);
   text.assign ("Program Start Time = " +  std::string(ctime(&aclock)));
   recordLog (logFile, text);

   std::ostringstream stext;
   stext << "For this computer the number of tics-per-second (CLOCKS_PER_SEC) = "
      <<  CLOCKS_PER_SEC << '\n';
   recordLog(logFile, stext.str());

   //  MAXINPUTPARAMETERS, MAXOUTPUTPARAMETERS are the number of param
   //  not to format the line to

   text.assign ("Start,End, Serial,,,");
   for(i=1; i<=MAXINPUTPARAMETERS;i++)
      text +=  "In,";

   for(i=1; i<=MAXOUTPUTPARAMETERS-1;  i++ )
   {
      char buf[5] = "";
      sprintf (buf, "%u", i);
      text += std::string(",Out ") + buf + std::string(",,");
   }
   recordLog(logFile, text);

   text.assign ("Time,Time,Number,Comment,Command Name, ");

   for(i=1; i<=MAXINPUTPARAMETERS;i++)
   {
      char buf[10] = "";
      sprintf(buf, "%u,",  i);
      text +=  buf;
   }

   for(i=1; i<= MAXOUTPUTPARAMETERS-1; i++ )
   {
      text += "Rtn,Dif,P/F,";
   }
   recordLog(logFile, text);

   termEcho=2;                          // Echo returns only to the terminal

   //---
   // Part 1: This calls the SMManager's loadLibraries function, which
   // ultimately results in all TSM plugin factories to self-register into a
   // list of TSM factories.
   //
   // Then, the printList method (defined below) is used to print the list of
   // all registered TSM factories. Note that only plugins are registered in
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
   std::cout << "Done with initialization!" << std::endl << std::endl;
   }

   std::cout << std::endl;              // Add a newline to make the vts prompt
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
      std::string targetCommand;
      std::vector<std::vector<std::string> > commandList;
      std::vector<std::string> param_array;
      int commandNumber;

      // get command line and parse it
      // targetCommand is the command line
      // subString is the token returned by parser
      std::cout << "\nvts> ";
      // remove all the leading white spaces
      //std::cin >> std::ws;

      targetCommand = readDataLine();

      if(debugFlag)
      {
      std::cout << "targetCommand: " << targetCommand << std::endl;
      }

      // Get the first parameter
      // ignore comments
      if (targetCommand[0] == '#')
         continue;
      // and empty lines
      if (targetCommand.empty())
         continue;

      commandList = getCommands(targetCommand);

      for(i=0;i<(int)commandList.size();i++)
      {
         command = commandList[i][0];
         commandNumber = getCommandIndex(command);
         param_array.clear();

         if(commandNumber < MAX_NUMBER_COMMANDS)
         {
            for(j=1;j<(int)commandList[i].size();j++)
               param_array.push_back(commandList[i][j]);

            processCommand(commandNumber, param_array);
         }
         else
         {  // command not found
            std::cout << std::endl
               << "vts>    **** Can not find the routine you request "
               << "in my list: " << command << std::endl;
         }
      }
   }
}
