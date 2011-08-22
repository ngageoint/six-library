//#############################################################################
//
//    FILENAME:   constatns.h
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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <vector>
#include <string>

#define HELP_FILENAME "vts.hlp"
#define BUFF_SIZE 100

enum MENU_ENUM {
      // The following is the VTS internal comment command
      COMMENT,                                         //   0

      // The following are Sensor Model commands
      SM_GROUND_TO_IMAGE1,                             //   1
      SM_GROUND_TO_IMAGE2,                             //   2
      SM_IMAGE_TO_GROUND1,                             //   3
      SM_IMAGE_TO_GROUND2,                             //   4
      SM_IMAGE_TO_PROXIMATE_IMAGING_LOCUS,             //   5
      SM_IMAGE_TO_REMOTE_IMAGING_LOCUS,                //   6
      SM_COMPUTE_GROUND_PARTIALS,                      //   7
      SM_COMPUTE_SENSOR_PARTIALS1,                     //   8
      SM_COMPUTE_SENSOR_PARTIALS2,                     //   9
      SM_GET_COVARIANCE_MODEL,                         //  12
      SM_GET_UNMODELED_ERROR,                          //  13
      SM_GET_UNMODELED_CROSS_COVARIANCE,               //  14
      SM_GET_CURRENT_PARAMETER_COVARIANCE,             //  15
      SM_SET_CURRENT_PARAMETER_COVARIANCE,             //  16
      SM_SET_ORIGINAL_PARAMETER_COVARIANCE,            //  17
      SM_GET_ORIGINAL_PARAMETER_COVARIANCE,            //  18
      SM_GET_TRAJECTORY_IDENTIFIER,                    //  19
      SM_GET_REFERENCE_DATE_AND_TIME,                  //  20
      SM_GET_IMAGE_TIME,                               //  21
      SM_GET_SENSOR_POSITION1,                         //  22
      SM_GET_SENSOR_POSITION2,                         //  23
      SM_GET_SENSOR_VELOCITY1,                         //  24
      SM_GET_SENSOR_VELOCITY2,                         //  25
      SM_SET_CURRENT_PARAMETER_VALUE,                  //  26
      SM_GET_CURRENT_PARAMETER_VALUE,                  //  27
      SM_GET_PARAMETER_NAME,                           //  28
      SM_GET_NUM_PARAMETERS,                           //  29
      SM_SET_ORIGINAL_PARAMETER_VALUE,                 //  30
      SM_GET_ORIGINAL_PARAMETER_VALUE,                 //  31
      SM_SET_PARAMETER_TYPE,                           //  32
      SM_GET_PARAMETER_TYPE,                           //  33
      SM_GET_PEDIGREE,                                 //  34
      SM_GET_IMAGE_IDENTIFIER,                         //  35
      SM_SET_IMAGE_IDENTIFIER,                         //  36
      SM_GET_SENSOR_IDENTIFIER,                        //  37
      SM_GET_PLATFORM_IDENTIFIER,                      //  38
      SM_GET_REFERENCE_POINT,                          //  39
      SM_SET_REFERENCE_POINT,                          //  40
      SM_GET_IMAGE_SIZE,                               //  41
      SM_GET_SENSOR_MODEL_STATE,                       //  42
      SM_GET_VALID_ALTITUDE_RANGE,                     //  43
      SM_GET_ILLUMINATION_DIRECTION,                   //  45
      SM_GET_NUM_SYSTEMATIC_ERROR_CORRECTIONS,         //  46
      SM_GET_SYSTEMATIC_ERROR_CORRECTION_NAME,         //  47
      SM_SET_CURRENT_SYSTEMATIC_ERROR_CORRECTION_SWITCH, //  48
      SM_GET_CURRENT_SYSTEMATIC_ERROR_CORRECTION_SWITCH, //  49
      SM_GET_SENSOR_MODEL_NAME2,                       //  50

      // The following is a VTS convenience command made up
      // of Sensor Plugin Commands

      MAKE_MODEL_LIST_FROM_ISD,                        //  51

      // The following are Sensor Plugin commands

      SP_GET_LIST,                                     //  52
      SP_FIND_PLUGIN,                                  //  53
      SP_REMOVE_PLUGIN,                                //  54
      SP_CAN_ISD_BE_CONVERTED_TO_SENSOR_MODEL_STATE,   //  55
      SP_CAN_SENSOR_MODEL_BE_CONSTRUCTED_FROM_STATE,   //  56
      SP_CAN_SENSOR_MODEL_BE_CONSTRUCTED_FROM_ISD,     //  57
      SP_CONSTRUCT_SENSOR_MODEL_FROM_STATE,            //  58
      SP_CONSTRUCT_SENSOR_MODEL_FROM_ISD,              //  59
      SP_CONVERT_ISD_TO_SENSOR_MODEL_STATE,            //  60
      SP_GET_MANUFACTURER,                             //  61
      SP_GET_N_SENSOR_MODEL,                           //  62
      SP_GET_RELEASE_DATE,                             //  63
      SP_GET_SENSOR_MODEL_NAME1,                       //  65
      SP_GET_SENSOR_MODEL_NAME_FROM_SENSOR_MODEL_STATE, //  66
      SP_GET_SENSOR_MODEL_VERSION,                     //  67
      SP_GET_COLLECTION_IDENTIFIER,                    //  69
      SP_IS_PARAMETER_SHAREABLE,                       //  70
      SP_GET_PARAMETER_SHARING_CRITERIA,               //  71
      SP_GET_PLUGIN_NAME,                              //  72

      // The following are VTS internal commands

      VTS_SET_LOG_FILENAME,                            //  73
      VTS_SET_LOG_FILE,                                //  74
      VTS_WRITE_COMMENT_TO_LOG_FILE,                   //  75
      VTS_WRITE_TEXT_TO_LOG_FILE,                      //  76
      VTS_REPEAT,                                      //  77
      VTS_TERM_ECHO,                                   //  78
      VTS_TOGGLE_DEBUG,                                //  79
      VTS_TOGGLE_COMPARE_DEBUG,                        //  80
      VTS_TOGGLE_NITF_DEBUG,                           //
      PRINT_LIST,                                      //  81
      PRINT_MODEL_LIST,                                //  82
      STATUS,                                          //  83
      VTS_READ_20ISD_FILE,                             //  84
      VTS_READ_21ISD_FILE,                             //  85
      VTS_READ_BYTESTREAM,                             //  86
      VTS_READ_FILENAME,                               //  87
      VTS_DUMP_ISD,                                    //  88
	  VTS_WRITE_STATE_TO_FILE,                         //  89
      VTS_READ_STATE_FROM_FILE,                        //  90
      HELP1,                                           //  91
      HELP2,                                           //  92
      HELP3,                                           //  93
      EXIT1,                                           //  94
      EXIT2                                            //  95
};

enum MENU_TYPE {
      UNKNOWN_TYPE,
      VTS_TYPE,
      FILE_TYPE,
      SENSOR_PLUGIN_TYPE,
      SENSOR_MODEL_TYPE,
      SENSOR_MODEL_STATE_TYPE
};

//*****************************************************************
//**IMPORTANT:update these numbers when menulist changes
#define MAX_NUMBER_COMMANDS EXIT2+1   // Total number of commands
// First Sensor Plugin command
#define BEGIN_SP_NUMBER     SP_GET_LIST
// Last Sensor Plugin command
#define END_SP_NUMBER       SP_GET_PLUGIN_NAME
// First Sensor Model command
#define BEGIN_SM_NUMBER     SM_GROUND_TO_IMAGE1
// Last Sensor Model command
#define END_SM_NUMBER       MAKE_MODEL_LIST_FROM_ISD
// vts internal commands command
#define BEGIN_VTS_NUMBER    VTS_SET_LOG_FILENAME
//*****************************************************************

//----- Define the Keyboard codes
#define BACKSPACE_KEY (char)127
#define ARROW_LEFT std::cout << (char)27 << (char)91 << (char)68
#define ARROW_RIGHT std::cout << (char)27 << (char)91 << (char)67
#define MAX_SAVE 10

//----- Error Messages
#define NO_SENSOR_MODEL_ERROR    "  vts - constructSensorModelFrom... has not been run yet! "
#define NO_MODEL_LIST_ERROR      "  vts - makeModelListFromISD has not been run yet! "
#define UNKNOWN_ERROR_STR        "Unknown Error thrown by "
#endif
