//#############################################################################
//
//    FILENAME:   constants.h
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
//      12 Aug 2010  DSL   CCB Change add writeStateFile and readStateFile
//      25 Oct 2010  DSL   CCB Change add getNumGeometricCorrectionSwitches,
//                                        getGeometricCorrectionName,
//                                        getCurrentGeometricCorrectionSwitch,
//                                    and setCurrentGeometricCorrectionSwitch
//      29 Jun 2011  DSL   Add thread testing 
//      10 Jan 2013  DSL   changes to reflect C++ reformat CRs
//      26 Jan 2013  DSL   added commands: vtsAddIsdParam, vtsGetIsdParam
//      28 Jan 2013  DSL   VTS_VERSION CSM3.0.1.0
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
      SM_COMPUTE_ALL_SENSOR_PARTIALS1,                 //  10
      SM_COMPUTE_ALL_SENSOR_PARTIALS2,                 //  11
      SM_GET_COLLECTION_IDENTIFIER,                    //  73
      SM_GET_COVARIANCE_MODEL,                      //  12
      SM_GET_NUM_SENSOR_MODEL_PARAMETERS,
      SM_GET_NUM_CORRELATION_PARAMETER_GROUPS,
      SM_GET_CORRELATION_PARAMETER_GROUP,
      SM_GET_CORRELATION_COEFFICIENT,
      SM_GET_CORRELATION_GROUP_PARAMETERS,
      SM_SET_CORRELATION_PARAMETER_GROUP,
      SM_SET_CORRELATION_GROUP_PARAMETERS,
      SM_GET_CROSS_COVARIANCE_MATRIX,                  //  13
      SM_GET_COVARIANCE_MATRIX,
	  SM_GET_UNMODELED_ERROR,                          //  15
      SM_GET_UNMODELED_CROSS_COVARIANCE,               //  16
      SM_GET_PARAMETER_COVARIANCE,                     //  17
      SM_SET_PARAMETER_COVARIANCE,                     //  18
      SM_GET_TRAJECTORY_IDENTIFIER,                    //  21
      SM_GET_REFERENCE_DATE_AND_TIME,                  //  22
      SM_GET_IMAGE_TIME,                               //  23
      SM_GET_SENSOR_POSITION1,                         //  24
      SM_GET_SENSOR_POSITION2,                         //  25
      SM_GET_SENSOR_VELOCITY1,                         //  26
      SM_GET_SENSOR_VELOCITY2,                         //  27
      SM_GET_PARAMETERS,
      SM_GET_PARAMETER_SET_INDICES,
      SM_SET_PARAMETER,
      SM_SET_PARAMETER_VALUE,                          //  28
      SM_GET_PARAMETER_VALUE,                          //  29
      SM_GET_PARAMETER_NAME,                           //  30
      SM_GET_PARAMETER_UNITS,
      SM_GET_PARAMETER,
	  SM_GET_NUM_PARAMETERS,                           //  31
      SM_SET_PARAMETER_TYPE,                           //  34
      SM_GET_PARAMETER_TYPE,                           //  35
      SM_GET_PEDIGREE,                                 //  38
      SM_GET_IMAGE_IDENTIFIER,                         //  39
      SM_SET_IMAGE_IDENTIFIER,                         //  40
      SM_GET_SENSOR_IDENTIFIER,                        //  41
      SM_GET_PLATFORM_IDENTIFIER,                      //  42
      SM_GET_REFERENCE_POINT,                          //  43
      SM_SET_REFERENCE_POINT,                          //  44
      SM_GET_IMAGE_START,                              //  4x
      SM_GET_IMAGE_SIZE,                               //  45
      SM_GET_MODEL_STATE,                              //  46
      SM_REPLACE_MODEL_STATE,
      SM_GET_VALID_HEIGHT_RANGE,                       //  47
      SM_GET_VALID_IMAGE_RANGE,                        //  48
      SM_GET_ILLUMINATION_DIRECTION,                   //  49
      SM_GET_NUM_GEOMETRIC_CORRECTION_SWITCHES,        //  50
      SM_GET_GEOMETRIC_CORRECTION_NAME,                //  51
      SM_SET_GEOMETRIC_CORRECTION_SWITCH,              //  52
      SM_GET_GEOMETRIC_CORRECTION_SWITCH,              //  53
      SM_GET_MODEL_NAME2,                              //  54
      SM_IS_PARAMETER_SHAREABLE,                       //  74
      SM_HAS_SHAREABLE_PARAMETERS,
      SM_GET_PARAMETER_SHARING_CRITERIA,               //  75
	  SM_GET_VERSION,                                  //  5x
      SM_GET_FAMILY,                                   //  7x
      // The following is a VTS convenience command made up
      // of Sensor Plugin Commands

      MAKE_MODEL_LIST_FROM_ISD,                        //  55

      // The following are Sensor Plugin commands

      SP_GET_LIST,                                     //  56
      SP_FIND_PLUGIN,                                  //  57
      SP_REMOVE_PLUGIN,                                //  58
      SP_CAN_ISD_BE_CONVERTED_TO_MODEL_STATE,          //  59
      SP_CAN_MODEL_BE_CONSTRUCTED_FROM_STATE,          //  60
      SP_CAN_MODEL_BE_CONSTRUCTED_FROM_ISD,            //  61
      SP_CONSTRUCT_MODEL_FROM_STATE,                   //  62
      SP_CONSTRUCT_MODEL_FROM_ISD,                     //  63
      SP_CONVERT_ISD_TO_MODEL_STATE,                   //  64
      SP_GET_MANUFACTURER,                             //  65
      SP_GET_NUM_MODELS,                               //  66
      SP_GET_RELEASE_DATE,                             //  67
      SP_GET_CSM_VERSION,                              //  68
      SP_GET_MODEL_NAME1,                              //  69
      SP_GET_MODEL_NAME_FROM_MODEL_STATE,              // 70
      SP_GET_MODEL_VERSION,                            //  71
      SP_GET_SENSOR_TYPE,                              //  76
      SP_GET_SENSOR_MODE,                              //  7x
      SP_GET_PLUGIN_NAME,                              //  77
      SP_GET_MODEL_FAMILY,                             //  7x

      // The following are VTS internal commands

      VTS_SET_LOG_FILENAME,                            //  78
      VTS_SET_LOG_FILE,                                //  79
      VTS_WRITE_COMMENT_TO_LOG_FILE,                   //  80
      VTS_WRITE_TEXT_TO_LOG_FILE,                      //  81
      VTS_REPEAT,                                      //  82
      VTS_TERM_ECHO,                                   //  83
      VTS_TOGGLE_DEBUG,                                //  84
      VTS_TOGGLE_COMPARE_DEBUG,                        //  85
      VTS_TOGGLE_NITF_DEBUG,                           //  86
      PRINT_LIST,                                      //  87
      PRINT_MODEL_LIST,                                //  88
      STATUS,                                          //  89
      VTS_READ_20ISD_FILE,                             //  90
      VTS_READ_21ISD_FILE,                             //  91
      VTS_READ_BYTESTREAM,                             //  92
      VTS_READ_FILENAME,                               //  93
      VTS_ADD_ISD_PARAM,
      VTS_GET_ISD_PARAM,
	  VTS_DUMP_ISD,                                    //  94
      VTS_TEST_THREADS,                                //  95
      VTS_WRITE_STATE_TO_FILE,                         //  96
      VTS_READ_STATE_FROM_FILE,                        //  97
      HELP1,                                           //  98
      HELP2,                                           //  99
      HELP3,                                           // 100
      EXIT1,                                           // 101
      EXIT2,                                            // 102
      VTS_SET_ENV,
      
      VTS_last_command_
};

enum MENU_TYPE {
      UNKNOWN_TYPE,
      VTS_TYPE,
      FILE_TYPE,
      SENSOR_PLUGIN_TYPE,
      SENSOR_MODEL_TYPE,
      SENSOR_MODEL_STATE_TYPE
      //SENSOR_MODEL_STATE_TYPE,
      //TARGET_PLUGIN_TYPE,
      //TARGET_MODEL_TYPE
};

//*****************************************************************
//**IMPORTANT:update these numbers when menulist changes
#define MAX_NUMBER_COMMANDS VTS_last_command_   // Total number of commands
// First Sensor Plugin command
#define BEGIN_SP_NUMBER     SP_GET_LIST
// Last Sensor Plugin command
#define END_SP_NUMBER       SP_GET_PLUGIN_NAME
// First Sensor Model command
#define BEGIN_SM_NUMBER     SM_GROUND_TO_IMAGE1
// Last Sensor Model command
#define END_SM_NUMBER       MAKE_MODEL_LIST_FROM_ISD
// First Target Plugin command
//#define BEGIN_TP_NUMBER     MAKE_TARGET_LIST_FROM_ISD
// Last Target Plugin command
//#define END_TP_NUMBER       TP_CONSTRUCT_TARGET_MODEL_FROM_ISD
// First Target Model command
//#define BEGIN_TM_NUMBER     TM_FIND_TARGET
// Last Target Model command
//#define END_TM_NUMBER       TM_GET_TARGET_SENSOR_VELOCITY
// vts internal commands command
#define BEGIN_VTS_NUMBER    VTS_SET_LOG_FILENAME
//*****************************************************************

//----- Define the Keyboard codes
#define BACKSPACE_KEY (char)127
#define ARROW_LEFT cout << (char)27 << (char)91 << (char)68
#define ARROW_RIGHT cout << (char)27 << (char)91 << (char)67
#define MAX_SAVE 10

//----- Error Messages
#define NO_SENSOR_MODEL_ERROR    "  vts - constructSensorModelFrom... has not been run yet! "
//#define NO_TARGET_MODEL_ERROR    "  vts - constructTargetModelFrom... has not been run yet! " 
#define NO_MODEL_LIST_ERROR      "  vts - makeModelListFromISD has not been run yet! "
//#define NO_TARGET_LIST_ERROR     "  vts - makeTargetListFromISD has not been run yet! "
//#define NO_SAVED_STATE_ERROR     "  vts - A Sensor Model State has not been saved yet! "
//#define NO_NUMBER_PARAMETERS_ERROR "  vts - getNumParameters... has not been run yet! "
#define UNKNOWN_ERROR_STR        "Unknown Error thrown by "

//----- Length and Size Constants
#endif
