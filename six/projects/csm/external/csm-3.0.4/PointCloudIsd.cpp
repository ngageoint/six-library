//#############################################################################
//
//    FILENAME:          PointCloudIsd.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    This file contains the implementations for the virtual destructors for
//    all the ISD classes defined in CSM.
//
//    LIMITATIONS:       None
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     20-Mar-2013   SCM      Initial Coding
//     26-Sep-2018   SCM      Added define of CSM_LIBRARY for Windows.
//
//    NOTES:
//
//#############################################################################

#ifndef CSM_LIBRARY
#define CSM_LIBRARY
#endif
#include "PointCloudIsd.h"

csm::Vlr::~Vlr() {}
csm::PointCloudIsd::~PointCloudIsd() {}
csm::LasIsd::~LasIsd() {}
csm::BpfIsd::~BpfIsd() {}
