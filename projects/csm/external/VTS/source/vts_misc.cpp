//#############################################################################
//
//    FILENAME:   vts_misc.cpp
//
//
//    DESCRIPTION:
//      This module contains support function for the vts commands
//
//    NOTES:
//                       Date          Author      Comment    
//    SOFTWARE HISTORY:  13-Jan-2004   LMT         Initial version.
//                       10 Mar 2010   Don Leonard     CCB Change Removed DATA_NOT_AVAILABLE Error
//                       11 May 2010   Don Leonard     Continue to construct a model if a warning is 
//                                                     returned from canSensorModelBeConstructedFromISD
//                       22 Jul 2010   Don Leonard     CCB Change add writeStateFile and readStateFile
//
//#############################################################################
#include "tokenizer.h"
#include "VTSMisc.h"
#include "TSMError.h"
#include "TSMWarning.h"
#include "TSMImageSupportData.h"
#include "TSMPlugin.h"
#ifndef _WIN32
#ifndef __APPLE__
#include <termio.h>
#else
#include <termios.h>
#endif
#include <unistd.h>
#else
#include <io.h>
#endif

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>

#ifndef _WIN32
struct termios stored_settings;
#endif

std::list<std::string> commandQueue;
std::vector<std::string> menulist;
std::vector<MENU_TYPE> menutype;

///////////////////////////////////////////////////////////////
//
//  Function: initMenuList
//
//  Description: Assign the command strings to their accociated
//               enumeration.
//
//  Inputs: None
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void initMenuList()
{
   menulist.resize(MAX_NUMBER_COMMANDS);
   menutype.resize(MAX_NUMBER_COMMANDS);

   for(int i=0;i<MAX_NUMBER_COMMANDS;i++)
   {
      menulist[i] = "";
      menutype[i] = UNKNOWN_TYPE;
   }

   // The following is the VTS internal comment command
   menulist[COMMENT] = "#";
   menutype[COMMENT] = VTS_TYPE;

   // The following are Sensor Model commands
   menulist[SM_GROUND_TO_IMAGE1]         = "groundToImage1";
   menutype[SM_GROUND_TO_IMAGE1]         = SENSOR_MODEL_TYPE;
   menulist[SM_GROUND_TO_IMAGE2]         = "groundToImage2";
   menutype[SM_GROUND_TO_IMAGE2]         = SENSOR_MODEL_TYPE;
   menulist[SM_IMAGE_TO_GROUND1]         = "imageToGround1";
   menutype[SM_IMAGE_TO_GROUND1]         = SENSOR_MODEL_TYPE;
   menulist[SM_IMAGE_TO_GROUND2]         = "imageToGround2";
   menutype[SM_IMAGE_TO_GROUND2]         = SENSOR_MODEL_TYPE;
   menulist[SM_IMAGE_TO_PROXIMATE_IMAGING_LOCUS] =
                                           "imageToProximateImagingLocus";
   menutype[SM_IMAGE_TO_PROXIMATE_IMAGING_LOCUS] = SENSOR_MODEL_TYPE;
   menulist[SM_IMAGE_TO_REMOTE_IMAGING_LOCUS] = 
                                           "imageToRemoteImagingLocus";
   menutype[SM_IMAGE_TO_REMOTE_IMAGING_LOCUS] = SENSOR_MODEL_TYPE;
   menulist[SM_COMPUTE_GROUND_PARTIALS]  = "computeGroundPartials";
   menutype[SM_COMPUTE_GROUND_PARTIALS]  = SENSOR_MODEL_TYPE;
   menulist[SM_COMPUTE_SENSOR_PARTIALS1] = "computeSensorPartials1";
   menutype[SM_COMPUTE_SENSOR_PARTIALS1] = SENSOR_MODEL_TYPE;
   menulist[SM_COMPUTE_SENSOR_PARTIALS2] = "computeSensorPartials2";
   menutype[SM_COMPUTE_SENSOR_PARTIALS2] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_COVARIANCE_MODEL]     = "getCovarianceModel";
   menutype[SM_GET_COVARIANCE_MODEL]     = SENSOR_MODEL_TYPE;
   menulist[SM_GET_UNMODELED_ERROR]      = "getUnmodeledError";
   menutype[SM_GET_UNMODELED_ERROR]      = SENSOR_MODEL_TYPE;
   menulist[SM_GET_UNMODELED_CROSS_COVARIANCE] = "getUnmodeledCrossCovariance";
   menutype[SM_GET_UNMODELED_CROSS_COVARIANCE] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_CURRENT_PARAMETER_COVARIANCE] =
                                           "getCurrentParameterCovariance";
   menutype[SM_GET_CURRENT_PARAMETER_COVARIANCE] = SENSOR_MODEL_TYPE;
   menulist[SM_SET_CURRENT_PARAMETER_COVARIANCE] =
                                           "setCurrentParameterCovariance";
   menutype[SM_SET_CURRENT_PARAMETER_COVARIANCE] = SENSOR_MODEL_TYPE;
   menulist[SM_SET_ORIGINAL_PARAMETER_COVARIANCE] = 
                                           "setOriginalParameterCovariance";
   menutype[SM_SET_ORIGINAL_PARAMETER_COVARIANCE] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_ORIGINAL_PARAMETER_COVARIANCE] =
                                           "getOriginalParameterCovariance";
   menutype[SM_GET_ORIGINAL_PARAMETER_COVARIANCE] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_TRAJECTORY_IDENTIFIER] = "getTrajectoryIdentifier";
   menutype[SM_GET_TRAJECTORY_IDENTIFIER] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_REFERENCE_DATE_AND_TIME] = "getReferenceDateAndTime";
   menutype[SM_GET_REFERENCE_DATE_AND_TIME] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_IMAGE_TIME]           = "getImageTime";
   menutype[SM_GET_IMAGE_TIME]           = SENSOR_MODEL_TYPE;
   menulist[SM_GET_SENSOR_POSITION1]     = "getSensorPosition1";
   menutype[SM_GET_SENSOR_POSITION1]     = SENSOR_MODEL_TYPE;
   menulist[SM_GET_SENSOR_POSITION2]     = "getSensorPosition2";
   menutype[SM_GET_SENSOR_POSITION2]     = SENSOR_MODEL_TYPE;
   menulist[SM_GET_SENSOR_VELOCITY1]     = "getSensorVelocity1";
   menutype[SM_GET_SENSOR_VELOCITY1]     = SENSOR_MODEL_TYPE;
   menulist[SM_GET_SENSOR_VELOCITY2]     = "getSensorVelocity2";
   menutype[SM_GET_SENSOR_VELOCITY2]     = SENSOR_MODEL_TYPE;
   menulist[SM_SET_CURRENT_PARAMETER_VALUE] = "setCurrentParameterValue";
   menutype[SM_SET_CURRENT_PARAMETER_VALUE] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_CURRENT_PARAMETER_VALUE] = "getCurrentParameterValue";
   menutype[SM_GET_CURRENT_PARAMETER_VALUE] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PARAMETER_NAME]       = "getParameterName";
   menutype[SM_GET_PARAMETER_NAME]       = SENSOR_MODEL_TYPE;
   menulist[SM_GET_NUM_PARAMETERS]       = "getNumParameters";
   menutype[SM_GET_NUM_PARAMETERS]       = SENSOR_MODEL_TYPE;
   menulist[SM_SET_ORIGINAL_PARAMETER_VALUE] = "setOriginalParameterValue";
   menutype[SM_SET_ORIGINAL_PARAMETER_VALUE] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_ORIGINAL_PARAMETER_VALUE] = "getOriginalParameterValue";
   menutype[SM_GET_ORIGINAL_PARAMETER_VALUE] = SENSOR_MODEL_TYPE;
   menulist[SM_SET_PARAMETER_TYPE]       = "setParameterType";
   menutype[SM_SET_PARAMETER_TYPE]       = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PARAMETER_TYPE]       = "getParameterType";
   menutype[SM_GET_PARAMETER_TYPE]       = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PEDIGREE]             = "getPedigree";
   menutype[SM_GET_PEDIGREE]             = SENSOR_MODEL_TYPE;
   menulist[SM_GET_IMAGE_IDENTIFIER]     = "getImageIdentifier";
   menutype[SM_GET_IMAGE_IDENTIFIER]     = SENSOR_MODEL_TYPE;
   menulist[SM_SET_IMAGE_IDENTIFIER]     = "setImageIdentifier";
   menutype[SM_SET_IMAGE_IDENTIFIER]     = SENSOR_MODEL_TYPE;
   menulist[SM_GET_SENSOR_IDENTIFIER]    = "getSensorIdentifier";
   menutype[SM_GET_SENSOR_IDENTIFIER]    = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PLATFORM_IDENTIFIER]  = "getPlatformIdentifier";
   menutype[SM_GET_PLATFORM_IDENTIFIER]  = SENSOR_MODEL_TYPE;
   menulist[SM_GET_REFERENCE_POINT]      = "getReferencePoint";
   menutype[SM_GET_REFERENCE_POINT]      = SENSOR_MODEL_TYPE;
   menulist[SM_SET_REFERENCE_POINT]      = "setReferencePoint";
   menutype[SM_SET_REFERENCE_POINT]      = SENSOR_MODEL_TYPE;
   menulist[SM_GET_IMAGE_SIZE]           = "getImageSize";
   menutype[SM_GET_IMAGE_SIZE]           = SENSOR_MODEL_TYPE;
   menulist[SM_GET_SENSOR_MODEL_STATE]   = "getSensorModelState";
   menutype[SM_GET_SENSOR_MODEL_STATE]   = SENSOR_MODEL_TYPE;
   menulist[SM_GET_VALID_ALTITUDE_RANGE] = "getValidAltitudeRange";
   menutype[SM_GET_VALID_ALTITUDE_RANGE] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_ILLUMINATION_DIRECTION] = "getIlluminationDirection";
   menutype[SM_GET_ILLUMINATION_DIRECTION] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_NUM_SYSTEMATIC_ERROR_CORRECTIONS] =
         "getNumSystematicErrorCorrections";              // Removed from 3.0
   menutype[SM_GET_NUM_SYSTEMATIC_ERROR_CORRECTIONS] = VTS_TYPE;
   menulist[SM_GET_SYSTEMATIC_ERROR_CORRECTION_NAME] =
         "getSystematicErrorCorrectionName";              // Removed from 3.0
   menutype[SM_GET_SYSTEMATIC_ERROR_CORRECTION_NAME] = VTS_TYPE;
   menulist[SM_SET_CURRENT_SYSTEMATIC_ERROR_CORRECTION_SWITCH] =
         "setCurrentSystematicErrorCorrectionSwitch";     // Removed from 3.0
   menutype[SM_SET_CURRENT_SYSTEMATIC_ERROR_CORRECTION_SWITCH] = VTS_TYPE;
   menulist[SM_GET_CURRENT_SYSTEMATIC_ERROR_CORRECTION_SWITCH] =
         "getCurrentSystematicErrorCorrectionSwitch";     // Removed from 3.0
   menutype[SM_GET_CURRENT_SYSTEMATIC_ERROR_CORRECTION_SWITCH] = VTS_TYPE;
   menulist[SM_GET_SENSOR_MODEL_NAME2]   = "getSensorModelName2";
   menutype[SM_GET_SENSOR_MODEL_NAME2]   = SENSOR_MODEL_TYPE;

   // The following is a VTS convenience command made up
   //  of Sensor Plugin Commands

   menulist[MAKE_MODEL_LIST_FROM_ISD]    = "makeModelListFromISD";
   menutype[MAKE_MODEL_LIST_FROM_ISD]    = FILE_TYPE;

   // The following are Sensor Plugin commands

   menulist[SP_GET_LIST]                 = "getList";
   menutype[SP_GET_LIST]                 = VTS_TYPE;
   menulist[SP_FIND_PLUGIN]              = "findPlugin";
   menutype[SP_FIND_PLUGIN]              = VTS_TYPE;
   menulist[SP_REMOVE_PLUGIN]            = "removePlugin";
   menutype[SP_REMOVE_PLUGIN]            = VTS_TYPE;
   menulist[SP_CAN_ISD_BE_CONVERTED_TO_SENSOR_MODEL_STATE] =
                                         "canISDBeConvertedToSensorModelState";
   menutype[SP_CAN_ISD_BE_CONVERTED_TO_SENSOR_MODEL_STATE] = SENSOR_PLUGIN_TYPE;
   menulist[SP_CAN_SENSOR_MODEL_BE_CONSTRUCTED_FROM_STATE] =
                                         "canSensorModelBeConstructedFromState";
   menutype[SP_CAN_SENSOR_MODEL_BE_CONSTRUCTED_FROM_STATE] = SENSOR_MODEL_STATE_TYPE;
   menulist[SP_CAN_SENSOR_MODEL_BE_CONSTRUCTED_FROM_ISD] =
                                         "canSensorModelBeConstructedFromISD";
   menutype[SP_CAN_SENSOR_MODEL_BE_CONSTRUCTED_FROM_ISD] = SENSOR_PLUGIN_TYPE;
   menulist[SP_CONSTRUCT_SENSOR_MODEL_FROM_STATE] =
                                         "constructSensorModelFromState";
   menutype[SP_CONSTRUCT_SENSOR_MODEL_FROM_STATE] = SENSOR_MODEL_STATE_TYPE;
   menulist[SP_CONSTRUCT_SENSOR_MODEL_FROM_ISD] = "constructSensorModelFromISD";
   menutype[SP_CONSTRUCT_SENSOR_MODEL_FROM_ISD] = SENSOR_PLUGIN_TYPE;
   menulist[SP_CONVERT_ISD_TO_SENSOR_MODEL_STATE] = 
                                         "convertISDToSensorModelState";
   menutype[SP_CONVERT_ISD_TO_SENSOR_MODEL_STATE] = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_MANUFACTURER]         = "getManufacturer";
   menutype[SP_GET_MANUFACTURER]         = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_N_SENSOR_MODEL]       = "getNSensorModels";
   menutype[SP_GET_N_SENSOR_MODEL]       = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_RELEASE_DATE]         = "getReleaseDate";
   menutype[SP_GET_RELEASE_DATE]         = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_SENSOR_MODEL_NAME1]   = "getSensorModelName1";
   menutype[SP_GET_SENSOR_MODEL_NAME1]   = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_SENSOR_MODEL_NAME_FROM_SENSOR_MODEL_STATE] =
                                      "getSensorModelNameFromSensorModelState";
   menutype[SP_GET_SENSOR_MODEL_NAME_FROM_SENSOR_MODEL_STATE] = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_SENSOR_MODEL_VERSION] = "getSensorModelVersion";
   menutype[SP_GET_SENSOR_MODEL_VERSION] = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_COLLECTION_IDENTIFIER] = "getCollectionIdentifier";
   menutype[SP_GET_COLLECTION_IDENTIFIER] = SENSOR_PLUGIN_TYPE;
   menulist[SP_IS_PARAMETER_SHAREABLE]   = "isParameterShareable";
   menutype[SP_IS_PARAMETER_SHAREABLE]   = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_PARAMETER_SHARING_CRITERIA] = "getParameterSharingCriteria";
   menutype[SP_GET_PARAMETER_SHARING_CRITERIA] = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_PLUGIN_NAME]          = "getPluginName";
   menutype[SP_GET_PLUGIN_NAME]          = VTS_TYPE;

   // The following are VTS internal commands

   menulist[VTS_WRITE_STATE_TO_FILE]     = "vtsWriteStateToFile";
   menutype[VTS_WRITE_STATE_TO_FILE]     = VTS_TYPE;
   menulist[VTS_READ_STATE_FROM_FILE]    = "vtsReadStateFromFile";
   menutype[VTS_READ_STATE_FROM_FILE]    = VTS_TYPE;
   menulist[VTS_SET_LOG_FILENAME]        = "vtsSetLogFileName";
   menutype[VTS_SET_LOG_FILENAME]        = VTS_TYPE;
   menulist[VTS_SET_LOG_FILE]            = "vtsSetLogFile";
   menutype[VTS_SET_LOG_FILE]            = VTS_TYPE;
   menulist[VTS_WRITE_COMMENT_TO_LOG_FILE] = "vtsWriteCommentToLogFile";
   menutype[VTS_WRITE_COMMENT_TO_LOG_FILE] = VTS_TYPE;
   menulist[VTS_WRITE_TEXT_TO_LOG_FILE]  = "vtsWriteTextToLogFile";
   menutype[VTS_WRITE_TEXT_TO_LOG_FILE]  = VTS_TYPE;
   menulist[VTS_REPEAT]                  = "vtsRepeat";
   menutype[VTS_REPEAT]                  = VTS_TYPE;
   menulist[VTS_TERM_ECHO]               = "vtsTermEcho";
   menutype[VTS_TERM_ECHO]               = VTS_TYPE;
   menulist[VTS_TOGGLE_DEBUG]            = "vtsToggleDebug";
   menutype[VTS_TOGGLE_DEBUG]            = VTS_TYPE;
   menulist[VTS_TOGGLE_COMPARE_DEBUG]    = "vtsToggleCompareDebug";
   menutype[VTS_TOGGLE_COMPARE_DEBUG]    = VTS_TYPE;
   menulist[VTS_TOGGLE_NITF_DEBUG]       = "vtsToggleNitfDebug";
   menutype[VTS_TOGGLE_NITF_DEBUG]       = VTS_TYPE;
   menulist[PRINT_LIST]                  = "printList";
   menutype[PRINT_LIST]                  = VTS_TYPE;
   menulist[PRINT_MODEL_LIST]            = "printModelList";
   menutype[PRINT_MODEL_LIST]            = VTS_TYPE;
   menulist[STATUS]                      = "status";
   menutype[STATUS]                      = VTS_TYPE;
   menulist[VTS_READ_20ISD_FILE]         = "vtsRead20ISDFile";
   menutype[VTS_READ_20ISD_FILE]         = VTS_TYPE;
   menulist[VTS_READ_21ISD_FILE]         = "vtsRead21ISDFile";
   menutype[VTS_READ_21ISD_FILE]         = VTS_TYPE;
   menulist[VTS_READ_BYTESTREAM]         = "vtsReadByteStream";
   menutype[VTS_READ_BYTESTREAM]         = VTS_TYPE;
   menulist[VTS_READ_FILENAME]           = "vtsReadFilename";
   menutype[VTS_READ_FILENAME]           = VTS_TYPE;
   menulist[VTS_DUMP_ISD]                = "vtsDumpISD";
   menutype[VTS_DUMP_ISD]                = FILE_TYPE;
   menulist[HELP1]                       = "help";
   menutype[HELP1]                       = VTS_TYPE;
   menulist[HELP2]                       = "Help";
   menutype[HELP2]                       = VTS_TYPE;
   menulist[HELP3]                       = "?";
   menutype[HELP3]                       = VTS_TYPE;
   menulist[EXIT1]                       = "exit";
   menutype[EXIT1]                       = VTS_TYPE;
   menulist[EXIT2]                       = "Exit";
   menutype[EXIT2]                       = VTS_TYPE;
}

///////////////////////////////////////////////////////////////
//
//  Function: trim
//
//  Description: remove white spaces and tabs from the ends of
//               a string
//
//  Inputs: str - string to be modified
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void trim(std::string& str)
{
  std::string::size_type pos = str.find_last_not_of(" \t");
  if(pos != std::string::npos)
  {
    str.erase(pos + 1);
    pos = str.find_first_not_of(" \t");
    if(pos != std::string::npos)
      str.erase(0, pos);
  }
  else
    str.erase(str.begin(), str.end());
}

///////////////////////////////////////////////////////////////
//
//  Function: findFile
//
//  Description: Find the full path of a file in the system path
//
//  Inputs: filename - Name of the file to be located
//
//  Returns: full path to the request file or the filename if not found
//
///////////////////////////////////////////////////////////////
std::string findFile(std::string filename)
{
   std::string fullFile = filename;
   std::string path = getenv("PATH");
   std::string location;

   if (!path.empty())
   {
      StringTokenizer strparams = StringTokenizer(path, ":");
      while (strparams.hasMoreTokens())
      {
         location = strparams.nextToken() + "/" + filename;
#ifdef _WIN32
         if(_access(location.c_str(), 04) == 0)
            fullFile = location;
#else
         if(access(location.c_str(), R_OK) == 0)
            fullFile = location;
#endif
      }
   }

   return fullFile;
}

///////////////////////////////////////////////////////////////
//
//  Function: getCommands
//
//  Description: Split the input data line into a command and a vector
//               of arguments
//
//  Inputs: params - Input line to be split up
//
//  Returns: Array of commands and arguments
//
///////////////////////////////////////////////////////////////
std::vector<std::vector<std::string> > getCommands(std::string params)
{
   std::vector<std::vector<std::string> > commandList1;
   std::vector<std::vector<std::string> > commandList2;
   std::vector<std::string> param_array;
   int i, j;
   int commandNumber;
   int num;
   std::fstream fin;
   std::string line;

   // parse for parameters
   param_array.clear();
   if (!params.empty())
   {
      StringTokenizer strparams = StringTokenizer(params, " ");
      while (strparams.hasMoreTokens() &&
         param_array.size() < MAXINPUTPARAMETERS)
         param_array.push_back(strparams.nextToken());
   }

   commandNumber = getCommandIndex(param_array[0]);

/*
   if(commandNumber >= MAX_NUMBER_COMMANDS)
   {
      fin.open(findFile(param_array[0]).c_str());

      if(fin.is_open())
      {
         getline(fin, line);

         while(!fin.eof())
         {
            commandList2 = getCommands(line);

            for(i=0;i<commandList2.size();i++)
            {
               for(j=0;j<commandList2[i].size();j++)
               {
                  num = 0;

                  if(commandList2[i][j][0] == '$')
                     num = atoi(commandList2[i][j].substr(1).c_str());

                  if(num > 0 && num < param_array.size())
                     commandList2[i][j] = param_array[num];
                  else if(num >= param_array.size())
                     commandList2[i][j] = "";
               }

               commandList1.push_back(commandList2[i]);
            }

            getline(fin, line);
         }

         fin.close();
      }
      else
         commandList1.push_back(param_array);
   }
   else
*/
      commandList1.push_back(param_array);

   return commandList1;
}

///////////////////////////////////////////////////////////////
//
//  Function: reportWarning
//
//  Description: Place a warning into the log file
//
//  Inputs: warning - Warning returned from a command
//          logFile - file for error to be logged
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void reportWarning(TSMWarning* warning, const char* logFile)
{
   std::string comment = "W";
   std::string text;
   char stemp[MAX_NAME_LENGTH];

   switch(warning->getWarning())
   {
      case TSMWarning::UNKNOWN_WARNING:
         strcpy(stemp, "Unknown warning");
         break;
      case TSMWarning::DATA_NOT_AVAILABLE:
         strcpy(stemp, "Data not Available warning");
         break;
      case TSMWarning::PRECISION_NOT_MET:
         strcpy(stemp, "Precision not met warning");
         break;
      case TSMWarning::NEGATIVE_PRECISION:
         strcpy(stemp, "Negative Precision warning");
         break;
      case TSMWarning::IMAGE_COORD_OUT_OF_BOUNDS:
         strcpy(stemp, "Image Coordinates out of Bounds warning");
         break;
      case TSMWarning::IMAGE_ID_TOO_LONG:
         strcpy(stemp, "Image ID too Long warning");
         break;
      case TSMWarning::NO_INTERSECTION:
         strcpy(stemp, "No Intersection warning");
         break;
      default:
         sprintf(stemp, "Invalid Warning Id %i", warning->getWarning());
   };

/*
   sprintf(stemp, "%i", warning->getWarning());
   text = "tsmWarning: function= "
        + warning->getFunction()
        + "  warning_type= "
        + stemp
        + "  message= "
        +  warning->getMessage();
*/

   text =  stemp;
   text += " occured in function ";
   text += warning->getFunction();
   text += " with the message: ";
   text += warning->getMessage();

   recordLog(logFile,
             comment,
             text);
};

///////////////////////////////////////////////////////////////
//
//  Function: reportError
//
//  Description: Place an error into the log file
//
//  Inputs: err     - Error returned from a command
//          logFile - file for error to be logged
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void reportError (TSMError *err, const char *logFile)
{
   std::string comment = "E";
   std::string text = "";
   char stemp[MAX_NAME_LENGTH] = "";

   switch(err->getError())
   {
      case TSMError::ALGORITHM:
         strcpy(stemp, "Algorithm error");
         break;
      case TSMError::BOUNDS:
         strcpy(stemp, "Bounds error");
         break;
      case TSMError::FILE_READ:
         strcpy(stemp, "Reading of file error");
         break;
      case TSMError::FILE_WRITE:
         strcpy(stemp, "Writting of file error");
         break;
      case TSMError::ILLEGAL_MATH_OPERATION:
         strcpy(stemp, "Illegal Math Operation error");
         break;
      case TSMError::INDEX_OUT_OF_RANGE:
         strcpy(stemp, "Index out of Range error");
         break;
      case TSMError::INVALID_SENSOR_MODEL_STATE:
         strcpy(stemp, "Invalid Sensor Model State error");
         break;
      case TSMError::INVALID_USE:
         strcpy(stemp, "Invalid Use error");
         break;
      case TSMError::ISD_NOT_SUPPORTED:
         strcpy(stemp, "ISD not Supported error");
         break;
      case TSMError::MEMORY:
         strcpy(stemp, "Memory error");
         break;
      case TSMError::SENSOR_MODEL_NOT_CONSTRUCTIBLE:
         strcpy(stemp, "Sensor Model not Constructible error");
         break;
      case TSMError::SENSOR_MODEL_NOT_SUPPORTED:
         strcpy(stemp, "Sensor Model not Supported error");
         break;
      case TSMError::STRING_TOO_LONG:
         strcpy(stemp, "String too long error");
         break;
      case TSMError::UNKNOWN_ERROR:
         strcpy(stemp, "Unknown error");
         break;
      case TSMError::UNSUPPORTED_FUNCTION:
         strcpy(stemp, "Unsupported function error");
         break;
      case TSMError::UNKNOWN_SUPPORT_DATA:
         strcpy(stemp, "Unknown support data error");
         break;
      //case TSMError::DATA_NOT_AVAILABLE:
      //   strcpy(stemp, "Data not Available error");
      //   break;
      default:
         sprintf(stemp, "Invalid Error Id %i", err->getError());
   };

/*
   sprintf ((char*)stemp, "%i", err->getError());
   text = "tsmError: function= "
        + err->getFunction()
        + "  error_type= "
        + stemp
        + "  message= "
        +  err->getMessage();
*/

   text =  stemp;
   text += " occured in function ";
   text += err->getFunction();
   text += " with the message: ";
   text += err->getMessage();

   recordLog(logFile,
             comment,
             text);
};

///////////////////////////////////////////////////////////////
//
//  Function: printList
//
//  Description: Display a list of availible plugins
//
//  Inputs: logFile - file for errors to be logged
//
//  Returns: Warning if one is produced
//
///////////////////////////////////////////////////////////////
TSMWarning * printList(const char *logFile)
{
   TSMPlugin::TSMPluginList* pluginPtrList = NULL;
   TSMWarning *tsmWarn;

   if (!(tsmWarn = TSMPlugin::getList(pluginPtrList)))
   { 
      if(pluginPtrList)
      {
          std::cout << "vts::printList():  Printing registered plugin names:" 
                    << std::endl;
  
          for (TSMPlugin::TSMPluginList::const_iterator ii = pluginPtrList->begin(); 
	           ii != pluginPtrList->end(); 
	           ++ii) 
          {
	         std::string pluginName;
	         try {
	            tsmWarn = (*ii)->getPluginName((pluginName));
	            std::cout << "Sensor: " << pluginName << std::endl;
	         } catch (TSMError err) {
	            reportError(&err,logFile);
	         }
          }
      }
   }

   return tsmWarn;
}

///////////////////////////////////////////////////////////////
//
//  Function: printModelList
//
//  Description: Get a list of model in a specified plugin
//
//  Inputs: pluginName - name of the plugin to get the list from
//
//  Returns: Warning if one is produced
//
///////////////////////////////////////////////////////////////
TSMWarning * printModelList(std::string pluginName)
{
   TSMPlugin::TSMPluginList* pluginPtrList = NULL;
   TSMWarning *tsmWarn = NULL;                   //----- Warning message
   int n_sensor_models=0;                        //----- Number of Sensor Models
   std::string currentPluginName;                //----- Current Plugin Name string
   std::string modelName;                        //----- Model Name string
   int i;                                        //----- Loop Variable
   const TSMPlugin* pSensorPlugin = NULL;        //----- Sensor Plugin Pointer

   if (!(tsmWarn = TSMPlugin::getList(pluginPtrList)))
   { 
      if(pluginPtrList)
      {
         for (TSMPlugin::TSMPluginList::const_iterator ii = pluginPtrList->begin(); 
            ii != pluginPtrList->end(); 
            ++ii) 
         {
            try {
               tsmWarn = (*ii)->getPluginName((currentPluginName));
            } catch (TSMError err) {
               std::cout << err.getError() << std::endl;
               std::cout << err.getMessage() << std::endl;
            }

            if(currentPluginName == pluginName)
               pSensorPlugin = *ii;
         }
      }
   }

   if(pSensorPlugin)
   {
      try {
         tsmWarn = pSensorPlugin->getNSensorModels(n_sensor_models);
      }
      catch (TSMError *err) {
         std::cout << err->getError() << std::endl;
         std::cout << err->getMessage() << std::endl;
      }
      catch (...) {
         std::cout << "&&&&& UNKNOWN error thrown by getNSensorModels\n";
      }
   }

   if(n_sensor_models > 0)
   {
      std::cout << "The plugin " << pluginName << " has " <<
         n_sensor_models << " models:" << std::endl;
      for (i=0; i<n_sensor_models; i++)
      {
         try {
            tsmWarn = pSensorPlugin->getSensorModelName(
                            i,
                            modelName);
         }
         catch (TSMError *err) {
            std::cout << err->getError() << std::endl;
            std::cout << err->getMessage() << std::endl;
         }
         catch (...) {
            std::cout << "&&&&& UNKNOWN error thrown by getSensorModelName\n";
         }
         std::cout << "\t" << modelName << std::endl;
      }
   }
   else if(pSensorPlugin)
   {
      std::cout << "The plugin " << pluginName << " has " <<
         " no sensor models." << std::endl;
   }

   std::cout << std::endl;
 
   return tsmWarn;
}

///////////////////////////////////////////////////////////////
//
//  Function: makeModelListFromISD
//
//  Description: Look up a model in the plugin
//
//  Inputs: isd_name - Name of model to be lookup
//          isd      - Loaded datafile
//
//  Returns: Warning if one is produced
//
///////////////////////////////////////////////////////////////
TSMWarning * makeModelListFromISD (const std::string isd_name,
                                   const tsm_ISD* isd)
                                  throw (TSMError)
{
    std::string myname("makeModelListFromISD");
    NameList names;
    TSMWarning *tsmWarn;
    TSMError tsmErr;
    std::cout << std::endl 
              << "makeModelListFromISD: Attempting to find models that match: "
              << isd_name << std::endl;
  
  //if (!(tsmWarn = listSupportingModels(isd_name, isd, names)))
  //{
        tsmWarn = listSupportingModels(isd_name, isd, names);
        if (names.size() > 0)
        {
            std::cout << "\nmakeModelListFromISD: Found the following models: " 
                      << std::endl;

            for(NameList::const_iterator j = names.begin();j!=names.end();++j)
            {
			    std::cout << "makeModelListFromISD:                          ->   " 
                          << (*j) << std::endl;
            }
		}
		else
		{
			std::cout << "makeModelListFromISD: No models found!" << std::endl;
			tsmErr.setTSMError (
				TSMError::ISD_NOT_SUPPORTED,
				"No Models Found",
				myname);
			throw tsmErr;

		}
  //}
    return NULL;
}

///////////////////////////////////////////////////////////////
//
//  Function: listSupportingModels
//
//  Description: List the models in the plugins
//
//  Inputs: isd_name           - Name of model to be lookup
//          image_support_data - Loaded datafile
//          aNameList          - List of Model Names
//
//  Returns: Warning if one is produced
//
///////////////////////////////////////////////////////////////
TSMWarning * listSupportingModels(const std::string isd_name,
				  const tsm_ISD* image_support_data,
				  NameList& aNameList)
{
	std::string myname("listSupportingModels");
	std::string smName;
	std::string pname;
	int n_sensor_models=0;
	int j=0;
	TSMWarning *tsmWarn;
	TSMPlugin::TSMPluginList* plugins = NULL;
	
	try 
	{
		tsmWarn = TSMPlugin::getList(plugins);
	}
	catch (TSMError *err) 
	{
		std::cout << err->getError() << '\n';
		std::cout << err->getMessage() << '\n';
	}
	catch (...) 
	{
		std::cout << "&&&&& UNKNOWN error thrown by getList\n";
	}
	if (tsmWarn)
	{
		std::cout << "     !!!!!!!!!!!!! warning from getList\n";
		std::cout << "     Warning number:   " << tsmWarn->getWarning() << '\n';
		std::cout << "     Warning function: " << tsmWarn->getFunction() << '\n';
		std::cout << "     Warning message:  " << tsmWarn->getMessage() << '\n';
	}

	if (!tsmWarn)
	{ 
		// tsmWarn = printList(NULL);
		for (TSMPlugin::TSMPluginList::const_iterator ii = plugins->begin();
			ii != plugins->end();
			++ii)
		{
			try 
			{
				tsmWarn = (*ii)->getPluginName (pname);
			}
			catch (TSMError *err) 
			{
				std::cout << err->getError() << '\n';
				std::cout << err->getMessage() << '\n';
			}
			catch (...) 
			{
				std::cout << "&&&&& UNKNOWN error thrown by getPluginName\n";
			}
			if (tsmWarn)
			{
				std::cout << "     !!!!!!!!!!!!! warning from getPluginName\n";
				std::cout << "     Warning number:   " << tsmWarn->getWarning() << '\n';
				std::cout << "     Warning function: " << tsmWarn->getFunction() << '\n';
				std::cout << "     Warning message:  " << tsmWarn->getMessage() << '\n';
				continue;
			}

			std::cout << "\nComparing Sensor Model name " << isd_name
				      << " with Sensor Models in plugin named " << pname << std::endl;

			bool isSupported = false;
			try 
			{
				tsmWarn = (*ii)->getNSensorModels(n_sensor_models);
			}
			catch (TSMError *err) 
			{
				std::cout << err->getError() << '\n';
				std::cout << err->getMessage() << '\n';
			}
			catch (...) 
			{
				std::cout << "&&&&& UNKNOWN error thrown by getNSensorModels\n";
			}
			if (tsmWarn)
			{
				std::cout << "     !!!!!!!!!!!!! warning from getNSensorModels\n";
				std::cout << "     Warning number:   " << tsmWarn->getWarning() << '\n';
				std::cout << "     Warning function: " << tsmWarn->getFunction() << '\n';
				std::cout << "     Warning message:  " << tsmWarn->getMessage() << '\n';
			}
			if (!tsmWarn)
				if (n_sensor_models > 0)
				{
					std::cout << "\n     Number of sensor models in plugin " << pname 
						<< " is: " << n_sensor_models << std::endl;
					for (j=0; j<n_sensor_models; ++j)
					{
						try 
						{
							tsmWarn = (*ii)->getSensorModelName(
								j,
								smName);
						}
						catch (TSMError *err) 
						{
							std::cout << err->getError() << '\n';
							std::cout << err->getMessage() << '\n';
						}
						catch (...) 
						{
							std::cout << "&&&&& UNKNOWN error thrown by getSensorModelName\n";
						}
						if (tsmWarn)
						{
							std::cout << "          !!!!!!!!!!!!! warning from getSensorModelName\n";
							std::cout << "          Warning number:   " << tsmWarn->getWarning() << '\n';
							std::cout << "          Warning function: " << tsmWarn->getFunction() << '\n';
							std::cout << "          Warning message:  " << tsmWarn->getMessage() << '\n';
						}

						std::cout << "     Sensor model #" << j+1 << " in plugin " << pname 
							<< " is named: " << smName << std::endl;
						isSupported = false;
						try 
						{
							tsmWarn = (*ii)->canSensorModelBeConstructedFromISD(
								*image_support_data,
								isd_name,
								isSupported);
						}
						catch (TSMError *err) 
						{
							std::cout << err->getError() << '\n';
							std::cout << err->getMessage() << '\n';
						}
						catch (...) 
						{
							std::cout << "&&&&& UNKNOWN error thrown by canSensorModelBeConstructedFromISD\n";
						}
						if (tsmWarn)
						{
							std::cout << "          !!!!!!!!!!!!! warning from canSensorModelBeConstructedFromISD\n";
							std::cout << "          Warning number:   " << tsmWarn->getWarning() << '\n';
							std::cout << "          Warning function: " << tsmWarn->getFunction() << '\n';
							std::cout << "          Warning message:  " << tsmWarn->getMessage() << '\n';
						}
						//if (!tsmWarn)
							if (isSupported)
							{
								std::cout << "     Sensor model #" << j+1 << " in plugin " << pname 
									<< " named: " << smName << " is supported!!!!!!!!!!" << std::endl;
								aNameList.push_back(smName);
							}

					}
				}
				else
				{
					std::cout << "ERROR: Plugin " << isd_name 
						<< " reports an incorrect number of sensor models of: "
						<< n_sensor_models << std::endl;

					tsmWarn = new TSMWarning(TSMWarning::UNKNOWN_WARNING,
						"No Sensor Model in plugin",
						myname);
					continue;

				}

	  } // end For loop
   }
   return tsmWarn; // kick it up the chain
}
 
///////////////////////////////////////////////////////////////
//
//  Function: getLogfileName
//
//  Description: prompt the user for the a log filename
//
//  Inputs: dirName - Current Directory
//          name    - Inputted filename
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void getLogfileName(const std::string dirName, std::string * name)
{
  std::cout << "VTS Specify Log File (" << dirName << "vts_logfile.csv)> ";

  // read characters from stdin until a newline
  getline(std::cin, *name);

  trim(*name);

  if(debugFlag)
     std::cout << "logFileCommand: " << *name << std::endl;
}

///////////////////////////////////////////////////////
//
//   Function: getCommandIndex
//
//   Description: get the Index of an instruction from
//                menulist array.
//
//   Input: command - name of the command to locate
//
//   Return Value:
//          Index of the array if found
//          MAX_NUMBER_COMMANDS if not found
//
///////////////////////////////////////////////////////
int getCommandIndex(std::string command)
{
    int i;                 //----- Loop Variable

    for(i=0; i < MAX_NUMBER_COMMANDS; i++) {
        if(command == menulist[i])
            break;
    }

    return i;
}

///////////////////////////////////////////////////////
//
//   Function: loadHelpFile
//
//   Description: load the help data file into a vector
//                of help responces.
//
//   Input: None
//
//   Return Value:
//          Vector of string containng help descriptions
//
///////////////////////////////////////////////////////
std::vector<std::string> loadHelpFile()
{
    std::vector<std::string> vHelpData;   //----- Help data array
    std::string helpData;                 //----- Help Data value
    int i;                                //----- Loop Variable
    std::fstream fin(HELP_FILENAME, std::ios_base::in);
                                          //----- Help file stream
    std::string buffer;                   //----- File buffer
    int index = MAX_NUMBER_COMMANDS;      //----- Command Index value
    int item_index = -1;                  //----- Index of }

    //----- Initialize the help data vector
    for(i=0;i<MAX_NUMBER_COMMANDS;i++)
    {
        vHelpData.push_back(std::string(menulist[i]) +
                           " help is not defined.\n");
    }

    if(fin.is_open())
    {
        //----- Read the help data file
        getline(fin, buffer, '\n');

        while(!fin.eof())
        {
           //std::cout << buffer << std::endl;

           if(buffer.substr(0, 5) == "Name=")
           { //----- Get the Command Name
              index = getCommandIndex(buffer.substr(5));
           }
           else if(buffer.substr(0, 6) == "Value=")
           { //---- Get the help to display
              helpData = "";

              helpData = buffer.substr(7);
              helpData += "\n";

              item_index = helpData.find("}");

              while(!fin.eof() && item_index == -1)
              {
                  std::getline(fin, buffer, '\n');
                  helpData += buffer + '\n';

                  item_index = helpData.find("}");
              }

              if(item_index != -1)
                  helpData[item_index] = '\n';

              if(index != MAX_NUMBER_COMMANDS)
                  vHelpData[index] = helpData;

              helpData = "";
              index = MAX_NUMBER_COMMANDS;
           }

           getline(fin, buffer, '\n');
        }

        fin.close();
    }
    else
    {
        std::cout << "Help file \'" << HELP_FILENAME
                  << "\' was not found." << std::endl;
    }

    return vHelpData;
};

///////////////////////////////////////////////////////
//
//   Function: set_keypress
//
//   Description: Disable terminal input buffereing and echo
//
//   Input: None
//
//   Return Value:
//          Nothing
//
///////////////////////////////////////////////////////
void set_keypress(void) {
#ifndef _WIN32
    struct termios new_settings;
    tcgetattr(0,&stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_lflag &= (~ECHO);
    new_settings.c_cc[VTIME] = 0;
    tcgetattr(0,&stored_settings);
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0,TCSANOW,&new_settings);
#endif
}

///////////////////////////////////////////////////////
//
//   Function: reset_keypress
//
//   Description: Restore default input functionality
//
//   Input: None
//
//   Return Value:
//          Nothing
//
///////////////////////////////////////////////////////
void reset_keypress(void) {
#ifndef _WIN32
    tcsetattr(0,TCSANOW,&stored_settings);
#endif
}

///////////////////////////////////////////////////////
//
//   Function: readDataLine
//
//   Description: read in the current command line
//                allow editing of instruction and recall
//                of previous instructions
//
//   Input: None
//
//   Return Value:
//          Command string to be parsed
//
///////////////////////////////////////////////////////
std::string readDataLine()
{
    std::string dataLine;     //----- Data String
    char ch;                  //----- Character that been inputted
    int index=0;              //----- Current Cursor Position
    int i;                    //----- Loop Variable
    std::list<std::string>::iterator listIter;
    std::list<std::string>::iterator lookIter;

    set_keypress();

    commandQueue.push_front("");
    lookIter = commandQueue.begin();
    listIter = lookIter;

    std::cin.get(ch);

    while(ch != '\n')
    {
        if(ch == BACKSPACE_KEY) //----- Deal with backspace key
        {
           if(index > 0)
           {
              dataLine.erase(--index, 1);
              std::cout << ch;

              for(i=index;i<dataLine.length();i++)
                 std::cout << dataLine[i];
              std::cout << ' ';

              for(i=index;i<=dataLine.length();i++)
              {
                 ARROW_LEFT;
              }
           }
        }
        else if(ch == 27) //----- Deal with arrow keys
        {
           std::cin.get(ch);
           if(ch == 91)
           {
              std::cin.get(ch);
              if(ch == 68)  //----- Left Arrow
              {
                 if(index > 0)
                 {
                    index--;
                    ARROW_LEFT;
                 }
              }
              else if(ch == 67) //----- Right Arrow
              {
                 if(index < dataLine.length())
                 {
                    index++;
                    ARROW_RIGHT;
                 }
              }
              else if(ch == 65) //----- Up Arrow
              {
                 ++lookIter;
                 if(lookIter != commandQueue.end())
                 {
                    ++listIter;
                    for(i=index;i<dataLine.length();i++)
                       ARROW_RIGHT;
                    for(i=0;i<dataLine.length();i++)
                       std::cout << BACKSPACE_KEY;

                    dataLine = *listIter;

                    for(i=0;i<dataLine.length();i++)
                       std::cout << dataLine[i];

                    index = dataLine.length();
                 }
                 else
                    lookIter = listIter;
              }
              else if(ch == 66) //----- Down Arrow
              {
                 if(listIter != commandQueue.begin())
                 {
                    for(i=index;i<dataLine.length();i++)
                       ARROW_RIGHT;
                    for(i=0;i<dataLine.length();i++)
                       std::cout << BACKSPACE_KEY;

                    --listIter;
                    dataLine = *listIter;

                    for(i=0;i<dataLine.length();i++)
                       std::cout << dataLine[i];

                    index = dataLine.length();
                 }

                 lookIter = listIter;
              }
           }
        }
        else if(ch == '\r')
        {
           std::cout << ch;
        }
        else  //----- Default responce
        {
           dataLine = dataLine.substr(0,index) + (char)ch +
                      dataLine.substr(index);

           for(i=index;i<dataLine.length();i++)
              std::cout << dataLine[i];

           index++;

           for(i=index;i<dataLine.length();i++)
           {
              ARROW_LEFT;
           }
        }

        std::cin.get(ch);
    }

    std::cout << ch;

    reset_keypress();
    //for(i=0;i<dataLine.length();i++)
    //   std::cout << "ch = |" << (int)dataLine[i] << "|" << std::endl;
    commandQueue.pop_front();

    index = dataLine.find_first_not_of(" \t");

    if(index != std::string::npos)
       dataLine = dataLine.substr(index);

    if(dataLine.find_first_not_of(" \t") != std::string::npos)
       commandQueue.push_front(dataLine);
    if(commandQueue.size() > MAX_SAVE)
       commandQueue.pop_back();

    return dataLine;
}
