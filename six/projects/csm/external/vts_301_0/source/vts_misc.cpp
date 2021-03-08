//#############################################################################
//
//    FILENAME:   vts_misc.cpp
//
//    DESCRIPTION:
//      This module contains support function for the vts commands
//
//    NOTES:
//    SOFTWARE HISTORY:
//       Date       Author Comment    
//      09 Mar 2010  DSL   CCB Change Deleted DATA_NOT_AVAILABLE 
//      12 Aug 2010  DSL   CCB Change add writeStateFile and readStateFile
//      22 Oct 2010  DSL   CCB Change add getCurrentCrossCovarianceMatrix 
//                                    and getOriginalCrossCovarianceMatrix
//      25 Oct 2010  DSL   CCB Change add getNumGeometricCorrectionSwitches,
//                                        getGeometricCorrectionName,
//                                        getCurrentGeometricCorrectionSwitch,
//                                    and setCurrentGeometricCorrectionSwitch
//      29 Jun 2011  DSL   Add thread testing 
//      10 Jan 2013  DSL   changes to reflect C++ reformat CRs
//      28 Jan 2013  DSL   VTS_VERSION CSM3.0.1.0
//#############################################################################
#include "tokenizer.h"
#include "VTSMisc.h"
#include "Error.h"
#include "Warning.h"
#include "BytestreamIsd.h"
#include "Plugin.h"
#ifndef _WIN32
#include <termio.h>
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

list<string> commandQueue;
vector<string> menulist;
vector<MENU_TYPE> menutype;

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
   menulist[SM_IMAGE_TO_REMOTE_IMAGING_LOCUS]    = "imageToRemoteImagingLocus";
   menutype[SM_IMAGE_TO_REMOTE_IMAGING_LOCUS]    = SENSOR_MODEL_TYPE;
   menulist[SM_COMPUTE_GROUND_PARTIALS]  = "computeGroundPartials";
   menutype[SM_COMPUTE_GROUND_PARTIALS]  = SENSOR_MODEL_TYPE;
   menulist[SM_COMPUTE_SENSOR_PARTIALS1] = "computeSensorPartials1";
   menutype[SM_COMPUTE_SENSOR_PARTIALS1] = SENSOR_MODEL_TYPE;
   menulist[SM_COMPUTE_SENSOR_PARTIALS2] = "computeSensorPartials2";
   menutype[SM_COMPUTE_SENSOR_PARTIALS2] = SENSOR_MODEL_TYPE;
   menulist[SM_COMPUTE_ALL_SENSOR_PARTIALS1]   = "computeAllSensorPartials1";
   menutype[SM_COMPUTE_ALL_SENSOR_PARTIALS1]   = SENSOR_MODEL_TYPE;
   menulist[SM_COMPUTE_ALL_SENSOR_PARTIALS2]   = "computeAllSensorPartials2";
   menutype[SM_COMPUTE_ALL_SENSOR_PARTIALS2]   = SENSOR_MODEL_TYPE;
   menulist[SM_GET_COLLECTION_IDENTIFIER]      = "getCollectionIdentifier";
   menutype[SM_GET_COLLECTION_IDENTIFIER]      = SENSOR_MODEL_TYPE;
   menulist[SM_GET_COVARIANCE_MODEL]           = "getCovarianceModel";
   menutype[SM_GET_COVARIANCE_MODEL]           = SENSOR_MODEL_TYPE;
   menulist[SM_GET_NUM_SENSOR_MODEL_PARAMETERS] = "getNumSensorModelParameters";
   menutype[SM_GET_NUM_SENSOR_MODEL_PARAMETERS] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_NUM_CORRELATION_PARAMETER_GROUPS] = "getNumCorrelationParameterGroups";
   menutype[SM_GET_NUM_CORRELATION_PARAMETER_GROUPS] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_CORRELATION_PARAMETER_GROUP]      = "getCorrelationParameterGroup";
   menutype[SM_GET_CORRELATION_PARAMETER_GROUP]      = SENSOR_MODEL_TYPE;
   menulist[SM_GET_CORRELATION_COEFFICIENT]          = "getCorrelationCoefficient";
   menutype[SM_GET_CORRELATION_COEFFICIENT]          = SENSOR_MODEL_TYPE;
   menulist[SM_GET_CORRELATION_GROUP_PARAMETERS]     = "getCorrelationGroupParameters";
   menutype[SM_GET_CORRELATION_GROUP_PARAMETERS]     = SENSOR_MODEL_TYPE;
   menulist[SM_SET_CORRELATION_PARAMETER_GROUP]      = "setCorrelationParameterGroup";
   menutype[SM_SET_CORRELATION_PARAMETER_GROUP]      = SENSOR_MODEL_TYPE;
   menulist[SM_SET_CORRELATION_GROUP_PARAMETERS]     = "setCorrelationGroupParameters";
   menutype[SM_SET_CORRELATION_GROUP_PARAMETERS]     = SENSOR_MODEL_TYPE;
   menulist[SM_GET_CROSS_COVARIANCE_MATRIX]    = "getCrossCovarianceMatrix";
   menutype[SM_GET_CROSS_COVARIANCE_MATRIX]    =  SENSOR_MODEL_TYPE;
   menulist[SM_GET_COVARIANCE_MATRIX]          = "getCovarianceMatrix";
   menutype[SM_GET_COVARIANCE_MATRIX]          =  SENSOR_MODEL_TYPE;
   menulist[SM_GET_UNMODELED_ERROR]            = "getUnmodeledError";
   menutype[SM_GET_UNMODELED_ERROR]            = SENSOR_MODEL_TYPE;
   menulist[SM_GET_UNMODELED_CROSS_COVARIANCE] = "getUnmodeledCrossCovariance";
   menutype[SM_GET_UNMODELED_CROSS_COVARIANCE] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PARAMETER_COVARIANCE]    = "getParameterCovariance";
   menutype[SM_GET_PARAMETER_COVARIANCE]    = SENSOR_MODEL_TYPE;
   menulist[SM_SET_PARAMETER_COVARIANCE]    = "setParameterCovariance";
   menutype[SM_SET_PARAMETER_COVARIANCE]    = SENSOR_MODEL_TYPE;
   menulist[SM_GET_TRAJECTORY_IDENTIFIER]   = "getTrajectoryIdentifier";
   menutype[SM_GET_TRAJECTORY_IDENTIFIER]   = SENSOR_MODEL_TYPE;
   menulist[SM_GET_REFERENCE_DATE_AND_TIME] = "getReferenceDateAndTime";
   menutype[SM_GET_REFERENCE_DATE_AND_TIME] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_IMAGE_TIME]            = "getImageTime";
   menutype[SM_GET_IMAGE_TIME]            = SENSOR_MODEL_TYPE;
   menulist[SM_GET_SENSOR_POSITION1]      = "getSensorPosition1";
   menutype[SM_GET_SENSOR_POSITION1]      = SENSOR_MODEL_TYPE;
   menulist[SM_GET_SENSOR_POSITION2]      = "getSensorPosition2";
   menutype[SM_GET_SENSOR_POSITION2]      = SENSOR_MODEL_TYPE;
   menulist[SM_GET_SENSOR_VELOCITY1]      = "getSensorVelocity1";
   menutype[SM_GET_SENSOR_VELOCITY1]      = SENSOR_MODEL_TYPE;
   menulist[SM_GET_SENSOR_VELOCITY2]      = "getSensorVelocity2";
   menutype[SM_GET_SENSOR_VELOCITY2]      = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PARAMETERS]            = "getParameters";
   menutype[SM_GET_PARAMETERS]            = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PARAMETER_SET_INDICES] = "getParameterSetIndices";
   menutype[SM_GET_PARAMETER_SET_INDICES] = SENSOR_MODEL_TYPE;
   menulist[SM_SET_PARAMETER]             = "setParameter";
   menutype[SM_SET_PARAMETER]             = SENSOR_MODEL_TYPE;
   menulist[SM_SET_PARAMETER_VALUE]       = "setParameterValue";
   menutype[SM_SET_PARAMETER_VALUE]       = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PARAMETER_VALUE]       = "getParameterValue";
   menutype[SM_GET_PARAMETER_VALUE]       = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PARAMETER_NAME]        = "getParameterName";
   menutype[SM_GET_PARAMETER_NAME]        = SENSOR_MODEL_TYPE;
   menulist[SM_GET_NUM_PARAMETERS]        = "getNumParameters";
   menutype[SM_GET_NUM_PARAMETERS]        = SENSOR_MODEL_TYPE;
   menulist[SM_SET_PARAMETER_TYPE]        = "setParameterType";
   menutype[SM_SET_PARAMETER_TYPE]        = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PARAMETER_TYPE]        = "getParameterType";
   menutype[SM_GET_PARAMETER_TYPE]        = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PEDIGREE]              = "getPedigree";
   menutype[SM_GET_PEDIGREE]              = SENSOR_MODEL_TYPE;
   menulist[SM_GET_IMAGE_IDENTIFIER]      = "getImageIdentifier";
   menutype[SM_GET_IMAGE_IDENTIFIER]      = SENSOR_MODEL_TYPE;
   menulist[SM_SET_IMAGE_IDENTIFIER]      = "setImageIdentifier";
   menutype[SM_SET_IMAGE_IDENTIFIER]      = SENSOR_MODEL_TYPE;
   menulist[SM_GET_SENSOR_IDENTIFIER]     = "getSensorIdentifier";
   menutype[SM_GET_SENSOR_IDENTIFIER]     = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PLATFORM_IDENTIFIER]   = "getPlatformIdentifier";
   menutype[SM_GET_PLATFORM_IDENTIFIER]   = SENSOR_MODEL_TYPE;
   menulist[SM_GET_REFERENCE_POINT]       = "getReferencePoint";
   menutype[SM_GET_REFERENCE_POINT]       = SENSOR_MODEL_TYPE;
   menulist[SM_SET_REFERENCE_POINT]       = "setReferencePoint";
   menutype[SM_SET_REFERENCE_POINT]       = SENSOR_MODEL_TYPE;
   menulist[SM_GET_IMAGE_START]           = "getImageStart";
   menutype[SM_GET_IMAGE_START]           = SENSOR_MODEL_TYPE;
   menulist[SM_GET_IMAGE_SIZE]            = "getImageSize";
   menutype[SM_GET_IMAGE_SIZE]            = SENSOR_MODEL_TYPE;
   menulist[SM_GET_MODEL_STATE]           = "getModelState";
   menutype[SM_GET_MODEL_STATE]           = SENSOR_MODEL_TYPE;
   menulist[SM_REPLACE_MODEL_STATE]       = "replaceModelState";
   menutype[SM_REPLACE_MODEL_STATE]       = SENSOR_MODEL_TYPE;
   menulist[SM_GET_VALID_HEIGHT_RANGE]    = "getValidHeightRange";
   menutype[SM_GET_VALID_HEIGHT_RANGE]    = SENSOR_MODEL_TYPE;
   menulist[SM_GET_VALID_IMAGE_RANGE]     = "getValidImageRange";
   menutype[SM_GET_VALID_IMAGE_RANGE]     = SENSOR_MODEL_TYPE;
   menulist[SM_GET_ILLUMINATION_DIRECTION] = "getIlluminationDirection";
   menutype[SM_GET_ILLUMINATION_DIRECTION] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_NUM_GEOMETRIC_CORRECTION_SWITCHES] =
         "getNumGeometricCorrectionSwitches";
   menutype[SM_GET_NUM_GEOMETRIC_CORRECTION_SWITCHES] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_GEOMETRIC_CORRECTION_NAME] =
         "getGeometricCorrectionName";
   menutype[SM_GET_GEOMETRIC_CORRECTION_NAME] = SENSOR_MODEL_TYPE;
   menulist[SM_SET_GEOMETRIC_CORRECTION_SWITCH] =
         "setGeometricCorrectionSwitch";
   menutype[SM_SET_GEOMETRIC_CORRECTION_SWITCH] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_GEOMETRIC_CORRECTION_SWITCH] =
         "getGeometricCorrectionSwitch";
   menutype[SM_GET_GEOMETRIC_CORRECTION_SWITCH] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_MODEL_NAME2]   = "getModelName2";
   menutype[SM_GET_MODEL_NAME2]   = SENSOR_MODEL_TYPE;
   menulist[SM_IS_PARAMETER_SHAREABLE]   = "isParameterShareable";
   menutype[SM_IS_PARAMETER_SHAREABLE]   = SENSOR_MODEL_TYPE;
   menulist[SM_GET_VERSION]              = "getVersion";
   menutype[SM_GET_VERSION]              = SENSOR_MODEL_TYPE;
   menulist[SM_GET_FAMILY]               = "getFamily";
   menutype[SM_GET_FAMILY]               = SENSOR_MODEL_TYPE;


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
   menulist[SP_CAN_ISD_BE_CONVERTED_TO_MODEL_STATE] = "canISDBeConvertedToModelState";
   menutype[SP_CAN_ISD_BE_CONVERTED_TO_MODEL_STATE] = SENSOR_PLUGIN_TYPE;
   menulist[SP_CAN_MODEL_BE_CONSTRUCTED_FROM_STATE] = "canModelBeConstructedFromState";
   menutype[SP_CAN_MODEL_BE_CONSTRUCTED_FROM_STATE] = SENSOR_MODEL_STATE_TYPE;
   menulist[SP_CAN_MODEL_BE_CONSTRUCTED_FROM_ISD] = "canModelBeConstructedFromISD";
   menutype[SP_CAN_MODEL_BE_CONSTRUCTED_FROM_ISD] = SENSOR_PLUGIN_TYPE;
   menulist[SP_CONSTRUCT_MODEL_FROM_STATE] = "constructModelFromState";
   menutype[SP_CONSTRUCT_MODEL_FROM_STATE] = SENSOR_MODEL_STATE_TYPE;
   menulist[SP_CONSTRUCT_MODEL_FROM_ISD] = "constructModelFromISD";
   menutype[SP_CONSTRUCT_MODEL_FROM_ISD] = SENSOR_PLUGIN_TYPE;
   menulist[SP_CONVERT_ISD_TO_MODEL_STATE] =  "convertISDToModelState";
   menutype[SP_CONVERT_ISD_TO_MODEL_STATE] = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_MANUFACTURER]         = "getManufacturer";
   menutype[SP_GET_MANUFACTURER]         = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_NUM_MODELS]           = "getNumModels";
   menutype[SP_GET_NUM_MODELS]           = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_RELEASE_DATE]         = "getReleaseDate";
   menutype[SP_GET_RELEASE_DATE]         = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_CSM_VERSION]          = "getCsmVersion";
   menutype[SP_GET_CSM_VERSION]          = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_MODEL_NAME1]          = "getModelName1";
   menutype[SP_GET_MODEL_NAME1]          = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_MODEL_FAMILY]         = "getModelFamily";
   menutype[SP_GET_MODEL_FAMILY]         = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_MODEL_NAME_FROM_MODEL_STATE] = "getModelNameFromModelState";
   menutype[SP_GET_MODEL_NAME_FROM_MODEL_STATE] = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_MODEL_VERSION]        = "getModelVersion";
   menutype[SP_GET_MODEL_VERSION]        = SENSOR_PLUGIN_TYPE;
   menulist[SM_GET_PARAMETER_SHARING_CRITERIA] = "getParameterSharingCriteria";
   menutype[SM_GET_PARAMETER_SHARING_CRITERIA] = SENSOR_MODEL_TYPE;
   menulist[SM_HAS_SHAREABLE_PARAMETERS] = "hasShareableParameters";
   menutype[SM_HAS_SHAREABLE_PARAMETERS] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PARAMETER_UNITS] = "getParameterUnits";
   menutype[SM_GET_PARAMETER_UNITS] = SENSOR_MODEL_TYPE;
   menulist[SM_GET_PARAMETER] = "getParameter";
   menutype[SM_GET_PARAMETER] = SENSOR_MODEL_TYPE;
   menulist[SP_GET_SENSOR_TYPE] = "getSensorType";
   menutype[SP_GET_SENSOR_TYPE] = SENSOR_PLUGIN_TYPE;
   menulist[SP_GET_SENSOR_MODE] = "getSensorMode";
   menutype[SP_GET_SENSOR_MODE] = SENSOR_PLUGIN_TYPE;
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
   menulist[VTS_ADD_ISD_PARAM]           = "vtsAddIsdParam";
   menutype[VTS_ADD_ISD_PARAM]           = VTS_TYPE;
   menulist[VTS_GET_ISD_PARAM]           = "vtsGetIsdParam";
   menutype[VTS_GET_ISD_PARAM]           = VTS_TYPE;
   menulist[VTS_DUMP_ISD]                = "vtsDumpISD";
   menutype[VTS_DUMP_ISD]                = FILE_TYPE;
   menulist[VTS_TEST_THREADS]            = "vtsTestThreads";
   menutype[VTS_TEST_THREADS]            = VTS_TYPE;
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
void trim(string& str)
{
  string::size_type pos = str.find_last_not_of(" \t");
  if(pos != string::npos)
  {
    str.erase(pos + 1);
    pos = str.find_first_not_of(" \t");
    if(pos != string::npos)
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
string findFile(string filename)
{
   string fullFile = filename;
   string path = getenv("PATH");
   string location;

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
///////////////////////////////////////////////////////
// replace each embedded space within quotes with "%20"
///////////////////////////////////////////////////////
string checkQuotesInCommand(string command)
{
    char qu = '"';
    string out;
	string ch;
	string chs;
    for(int i = 0;i<command.length();i++)
    {
		ch = command[i];
		chs = "";
        if(ch[0] == qu)
        {
            for(int j=i+1;j<command.length();j++)
            {
                if(command[j]== qu)
                {
					if(command[j+1]== qu)
					{
						chs += qu;
						j++;
					}
					else
					{
						ch = "";
						out += chs;
						i = j;
						break;
					}
				}
				else if(command[j] == ' ')
				{
					chs += "%20";
				}
				else
				{
					chs += command[j];
					int jjj = 1;
				}
            }
        }
		out += ch;
		string xx = "2";
    }
    return out;
}

///////////////////////////////////////////////////////
// replace each "%20" with a space
///////////////////////////////////////////////////////
vector<string> checkSpacesInParams(vector<string> commandList)
{
    vector<string> cl1;
    for(int i=0;i<(int)commandList.size();i++)
    {
        string param = commandList[i];
        size_t start_pos = 1;
        while(start_pos != string::npos)
        {
            start_pos = param.find("%20");
            if(start_pos != string::npos)
            {
                param.replace(start_pos, 3, " ");
            }
        }
        cl1.push_back(param);
    }
    return cl1;
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
vector<string> getCommands(string params)
{
   params = checkQuotesInCommand(params); 
   vector<string> param_array;
   fstream fin;
   string line;

   // parse for parameters
   param_array.clear();
   if (!params.empty())
   {
      StringTokenizer strparams = StringTokenizer(params, " ");
      while (strparams.hasMoreTokens() &&
         param_array.size() < MAXINPUTPARAMETERS)
         param_array.push_back(strparams.nextToken());
   }

   param_array = checkSpacesInParams(param_array);
   return param_array;
}

///////////////////////////////////////////////////////////////
//
//  Function: reportWarning
//
//  Description: Place a warning into the log file
//
//  Inputs: warnings - WarningList: list of warnings returned from a command
//          logFile - file for error to be logged
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void reportWarning(WarningList warnings, const char* logFile)
{
   string comment = "W";
   string text;
   char stemp[TEMP_STRING_LENGTH];

   for (WarningList::iterator i = warnings.begin();i!=warnings.end();i++)
   {
	   Warning warning = *i;
	   switch(warning.getWarning())
	   {
	   case Warning::UNKNOWN_WARNING:
		   strcpy(stemp, "Unknown warning");
		   break;
	   case Warning::DATA_NOT_AVAILABLE:
		   strcpy(stemp, "Data not Available warning");
		   break;
	   case Warning::PRECISION_NOT_MET:
		   strcpy(stemp, "Precision not met warning");
		   break;
	   case Warning::NEGATIVE_PRECISION:
		   strcpy(stemp, "Negative Precision warning");
		   break;
	   case Warning::IMAGE_COORD_OUT_OF_BOUNDS:
		   strcpy(stemp, "Image Coordinates out of Bounds warning");
		   break;
	   case Warning::IMAGE_ID_TOO_LONG:         strcpy(stemp, "Image ID too Long warning");
		   break;
	   case Warning::NO_INTERSECTION:
		   strcpy(stemp, "No Intersection warning");
		   break;
	   case Warning::DEPRECATED_FUNCTION:
		   strcpy(stemp, "Deprecated Function warning");
		   break;
	   default:
		   sprintf(stemp, "Invalid Warning Id %i", warning.getWarning());
	   }
	}

   text =  stemp;
   text += " occured in function ";
   text += warnings.begin()->getFunction();
   text += " with the message: ";
   text += warnings.begin()->getMessage();

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
void reportError (Error* err, const char* logFile)
{
   string comment = "E";
   string text = "";
   char stemp[TEMP_STRING_LENGTH];

   switch(err->getError())
   {
      case Error::ALGORITHM:
         strcpy(stemp, "Algorithm error");
         break;
      case Error::BOUNDS:
         strcpy(stemp, "Bounds error");
         break;
      case Error::FILE_READ:
         strcpy(stemp, "Reading of file error");
         break;
      case Error::FILE_WRITE:
         strcpy(stemp, "Writting of file error");
         break;
      case Error::ILLEGAL_MATH_OPERATION:
         strcpy(stemp, "Illegal Math Operation error");
         break;
      case Error::INDEX_OUT_OF_RANGE:
         strcpy(stemp, "Index out of Range error");
         break;
      case Error::INVALID_SENSOR_MODEL_STATE:
         strcpy(stemp, "Invalid Sensor Model State error");
         break;
      case Error::INVALID_USE:
         strcpy(stemp, "Invalid Use error");
         break;
      case Error::ISD_NOT_SUPPORTED:
         strcpy(stemp, "ISD not Supported error");
         break;
      case Error::MEMORY:
         strcpy(stemp, "Memory error");
         break;
      case Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE:
         strcpy(stemp, "Sensor Model not Constructible error");
         break;
      case Error::SENSOR_MODEL_NOT_SUPPORTED:
         strcpy(stemp, "Sensor Model not Supported error");
         break;
      case Error::STRING_TOO_LONG:
         strcpy(stemp, "String too long error");
         break;
      case Error::UNKNOWN_ERROR:
         strcpy(stemp, "Unknown error");
         break;
      case Error::UNSUPPORTED_FUNCTION:
         strcpy(stemp, "Unsupported function error");
         break;
      case Error::UNKNOWN_SUPPORT_DATA:
         strcpy(stemp, "Unknown support data error");
         break;
      //case Error::DATA_NOT_AVAILABLE:                      // ***********************
      //   strcpy(stemp, "Data not Available error");
      //   break;
      default:
         sprintf(stemp, "Invalid Error Id %i", err->getError());
   };

/*
   sprintf(stemp, "%i", err->getError());
   text = "Error: function= "
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
//  Returns: N/A
//
///////////////////////////////////////////////////////////////
void printList(string logFile)
{
   PluginList pl = Plugin::getList();
   if(pl.size() > 0)
   {
       cout << "vts::printList():  Printing registered plugin names:" 
                    << endl;
  
       for (PluginList::const_iterator ii = pl.begin(); ii != pl.end(); ++ii) 
       {
	       string pluginName;
	       try {
               pluginName = (*ii)->getPluginName();
	           cout << "Sensor: " << pluginName << endl;
	       } catch (Error err) {
	           reportError(&err,logFile.c_str());
	       }
       }
   }
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
void printModelList(string pluginName)
{
   PluginList pluginList;
   int n_sensor_models=0;                        //----- Number of Sensor Models
   string currentPluginName;                //----- Current Plugin Name string
   string modelName;                        //----- Model Name string
   int i;                                        //----- Loop Variable
   const Plugin* pSensorPlugin = NULL;        //----- Sensor Plugin Pointer

   pluginList = Plugin::getList();
   if(pluginList.size()>0)
   {
	   for (PluginList::const_iterator ii = pluginList.begin(); 
		   ii != pluginList.end(); 
		   ++ii) 
	   {            
		   try
		   {
			   currentPluginName = (*ii)->getPluginName();
		   } catch (Error err) {
			   cout << err.getError() << endl;
			   cout << err.getMessage() << endl;
		   }

		   if(currentPluginName == pluginName)
			   pSensorPlugin = *ii;
	   }
   }

   if(pSensorPlugin)
   {
	   n_sensor_models = pSensorPlugin->getNumModels();
   }

   if(n_sensor_models > 0)
   {
      cout << "The plugin " << pluginName << " has " <<
         n_sensor_models << " models:" << endl;
      for (i=0; i<n_sensor_models; i++)
      {
		  modelName = pSensorPlugin->getModelName(i);
      }
   }
   else if(pSensorPlugin)
   {
      cout << "The plugin " << pluginName << " has " <<
         " no sensor models." << endl;
   }

   cout << endl;
   cout << endl;
 
   return;
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
//  Returns: N/A
//
///////////////////////////////////////////////////////////////
void makeModelListFromISD(const string isd_name,
                     const Isd* isd, 
					 WarningList* warnings)
{
   string myname("makeModelListFromISD");
   NameList names;
   Error csmErr;
   cout << endl 
	         << "makeModelListFromISD: Attempting to find models that match: "
			 << isd_name << endl;
 
   listSupportingModels(isd_name, isd, names, warnings);
   if (names.size() > 0)
   {
	   cout << "\nmakeModelListFromISD: Found the following models: " 
          		 << endl;

	   for (NameList::const_iterator j = names.begin();j != names.end();++j)
	   {
		   cout << "makeModelListFromISD:                          ->   " 
		             << (*j) << endl;
	   }
   }
   else
   {
	   cout << "makeModelListFromISD: No models found!" << endl;
       
	   csmErr.setError (
		   Error::ISD_NOT_SUPPORTED,
		   "No Models Found",
		   myname);
	   throw csmErr;

   }
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
void listSupportingModels(const string isd_name,
				  const Isd* image_support_data,
				  NameList& aNameList, WarningList* warnings)
{

   string myname("listSupportingModels");
   string smName;
   string pname;
   int n_sensor_models=0;
   int j=0;

   PluginList plugins = Plugin::getList();

   for (PluginList::const_iterator ii = plugins.begin();
        ii != plugins.end();++ii)
   {
	   pname = (*ii)->getPluginName();

	   cout << "\nComparing Sensor Model name " << isd_name
		   << " with Sensor Models in plugin named " << pname << endl;

	   bool isSupported = false;
	   n_sensor_models = (*ii)->getNumModels();
	   if (n_sensor_models > 0)
	   {
		   cout << "\n     Number of sensor models in plugin " << pname 
			   << " is: " << n_sensor_models << endl;
		   for (j=0; j<n_sensor_models; ++j)
		   {
			   WarningList warnings1;

			   smName = (*ii)->getModelName(j);

			   cout << "     Sensor model #" << j+1 << " in plugin " << pname 
				   << " is named: " << smName << endl;
			   isSupported = false;
			   try 
			   {
				   isSupported = (*ii)->canModelBeConstructedFromISD(
					   *image_support_data,
					   isd_name,
					   &warnings1);
			   }
			   catch (Error *err) 
			   {
				   cout << err->getError() << '\n';
				   cout << err->getMessage() << '\n';
			   }
			   catch (...) {
				   cout << "&&&&& UNKNOWN error thrown by canModelBeConstructedFromISD\n";
			   }

               for (WarningList::const_iterator ii = warnings1.begin();ii != warnings1.end();++ii) 
			   {
				   cout << "          !!!!!!!!!!!!! warning from canModelBeConstructedFromISD\n";
				   cout << "          Warning number:   " << (*ii).getWarning() << '\n';
				   cout << "          Warning function: " << (*ii).getFunction() << '\n';
				   cout << "          Warning message:  " << (*ii).getMessage() << '\n';
			   }
			   if (isSupported)
			   {
				   cout << "     Sensor model #" << j+1 << " in plugin " << pname 
					   << " named: " << smName << " is supported!!!!!!!!!!" << endl;
				   aNameList.push_back(smName);
			   }

		   }
	   }
	   else
	   {
		   cout << "ERROR: Plugin " << isd_name 
			   << " reports an incorrect number of sensor models of: "
			   << n_sensor_models << endl;

           warnings->push_back(Warning(Warning::UNKNOWN_WARNING,
			                          "No Sensor Model in plugin",
									  myname));
		   continue;
	   }

   } // end For loop
}
  
  
 void getLogfileName(const string dirName, string * name)
{
  cout << "VTS Specify Log File (" << dirName << "vts_logfile.csv)> ";

  // read characters from stdin until a newline
  getline(cin, *name);

  trim(*name);

  if(debugFlag)
     cout << "logFileCommand: " << *name << endl;
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
int getCommandIndex(string command)
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
//   Function: fitLines
//
//   Description: fits output lines to the screen's width
//
//   Input:
//       buff - (string) text to be displayed on the screen
//       del - (char) end of line character, normally '\n'
//       len - (int) screen width
//
//   Return Value:
//          (string) text to be displayed, fit to screen
//
///////////////////////////////////////////////////////
string fitLines(string buff, char del, int len) {
  int p = 0;
  if(len > (int)buff.length()) return buff;
  while (p+len < (int)buff.length()){
    int lastSpace = -1;
    for(int i=p;i<=p+len;i++){
      if(i > (int)buff.length()){
        break;
      }
      if(buff[i] == '\n') {
        lastSpace = -1;
        break;
      }
      if(buff[i] == ' ') {
         lastSpace = i;
      }
    }
    if(lastSpace > 0) {
      buff[lastSpace] = del;
      p = lastSpace+1;
    } else {
      p += len;
    }
  }
  return buff;
}

string fitLines(string buff) {
// fits output lines to a screen width
//    "del" - default end line char is '\n'
//    "len" - default screen width = 72
  return fitLines(buff, '\n', 72);
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
vector<string> loadHelpFile()
{
    vector<string> vHelpData;   //----- Help data array
    string helpData;                 //----- Help Data value
    int i;                                //----- Loop Variable
    fstream fin(HELP_FILENAME, ios_base::in);
                                          //----- Help file stream
    string buffer;                   //----- File buffer
    int index = MAX_NUMBER_COMMANDS;      //----- Command Index value
    int item_index = -1;                  //----- Index of }

    //----- Initialize the help data vector
    for(i=0;i<MAX_NUMBER_COMMANDS;i++)
    {
        vHelpData.push_back(string(menulist[i]) +
                           " help is not defined.\n");
    }

    if(fin.is_open())
    {
        //----- Read the help data file
        getline(fin, buffer, '\n');

        while(!fin.eof())
        {
           //cout << buffer << endl;

           if(buffer.substr(0, 5) == "Name=")
           { //----- Get the Command Name
              index = getCommandIndex(buffer.substr(5));
           }
           else if(buffer.substr(0, 6) == "Value=")
           { //---- Get the help to display
              helpData = "\n------------------------------------------\n";

              helpData += fitLines(buffer.substr(7));
              helpData += "\n";

              item_index = helpData.find("}");

              while(!fin.eof() && item_index == -1)
              {
                  getline(fin, buffer, '\n');
                  helpData += fitLines(buffer) + '\n';

                  item_index = helpData.find("}");
              }

              if(item_index != -1)
                  helpData[item_index] = '\n';

              helpData += "------------------------------------------\n";
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
        cout << "Help file \'" << HELP_FILENAME
                  << "\' was not found." << endl;
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
string readDataLine()
{
    string dataLine;     //----- Data String
    char ch;                  //----- Character that been inputted
    int index=0;              //----- Current Cursor Position
    int i;                    //----- Loop Variable
    list<string>::iterator listIter;
    list<string>::iterator lookIter;

    set_keypress();

    commandQueue.push_front("");
    lookIter = commandQueue.begin();
    listIter = lookIter;

    cin.get(ch);

    while(ch != '\n')
    {
        if(ch == BACKSPACE_KEY) //----- Deal with backspace key
        {
           if(index > 0)
           {
              dataLine.erase(--index, 1);
              cout << ch;

              for(i=index;i<(int)dataLine.length();i++)
                 cout << dataLine[i];
              cout << ' ';

              for(i=index;i<=(int)dataLine.length();i++)
              {
                 ARROW_LEFT;
              }
           }
        }
        else if(ch == 27) //----- Deal with arrow keys
        {
           cin.get(ch);
           if(ch == 91)
           {
              cin.get(ch);
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
                 if(index < (int)dataLine.length())
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
                    for(i=index;i<(int)dataLine.length();i++)
                       ARROW_RIGHT;
                    for(i=0;i<(int)dataLine.length();i++)
                       cout << BACKSPACE_KEY;

                    dataLine = *listIter;

                    for(i=0;i<(int)dataLine.length();i++)
                       cout << dataLine[i];

                    index = dataLine.length();
                 }
                 else
                    lookIter = listIter;
              }
              else if(ch == 66) //----- Down Arrow
              {
                 if(listIter != commandQueue.begin())
                 {
                    for(i=index;i<(int)dataLine.length();i++)
                       ARROW_RIGHT;
                    for(i=0;i<(int)dataLine.length();i++)
                       cout << BACKSPACE_KEY;

                    --listIter;
                    dataLine = *listIter;

                    for(i=0;i<(int)dataLine.length();i++)
                       cout << dataLine[i];

                    index = dataLine.length();
                 }

                 lookIter = listIter;
              }
           }
        }
        else if(ch == '\r')
        {
           cout << ch;
        }
        else  //----- Default responce
        {
           dataLine = dataLine.substr(0,index) + (char)ch +
                      dataLine.substr(index);

           for(i=index;i<(int)dataLine.length();i++)
              cout << dataLine[i];

           index++;

           for(i=index;i<(int)dataLine.length();i++)
           {
              ARROW_LEFT;
           }
        }

        cin.get(ch);
    }

    cout << ch;

    reset_keypress();
    //for(i=0;i<dataLine.length();i++)
    //   cout << "ch = |" << (int)dataLine[i] << "|" << endl;
    commandQueue.pop_front();

    index = dataLine.find_first_not_of(" \t");

    if(index != (int)string::npos)
       dataLine = dataLine.substr(index);

    if(dataLine.find_first_not_of(" \t") != string::npos)
       commandQueue.push_front(dataLine);
    if(commandQueue.size() > MAX_SAVE)
       commandQueue.pop_back();

    return dataLine;
}

///////////////////////////////////////////////////////
//   Function: ISDType2str
//
//   Description: return string version of ISD file type
//
//   Input: ISDType
//
//   Return Value:
//          string ISD file type
//
///////////////////////////////////////////////////////
string ISDType2str(ISDType ISDFileType)
{  
	string ret = "NO_FILE_TYPE";
    switch (ISDFileType)
    {
       case NITF_20_TYPE:
	      ret = "NITF_20_TYPE";
          break;

       case NITF_21_TYPE:
	      ret = "NITF_21_TYPE";
          break;

       case BYTESTREAM_TYPE:
	      ret = "BYTESTREAM_TYPE";
          break;

       case FILENAME_TYPE:
	      ret = "FILENAME_TYPE";
          break;
	}
	return ret;
} 
