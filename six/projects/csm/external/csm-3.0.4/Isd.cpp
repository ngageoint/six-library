//#############################################################################
//
//    FILENAME:          Isd.cpp
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
//
//    NOTES:
//
//#############################################################################

#ifndef CSM_LIBRARY
#define CSM_LIBRARY
#endif
#include "Isd.h"
#include "BytestreamIsd.h"
#include "NitfIsd.h"

csm::Isd::~Isd() {}
csm::BytestreamIsd::~BytestreamIsd() {}
csm::NitfIsd::~NitfIsd() {}
csm::Nitf20Isd::~Nitf20Isd() {}
csm::Nitf21Isd::~Nitf21Isd() {}
