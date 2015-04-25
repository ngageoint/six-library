
#include <import/nitf.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_N,  2,  "Day of Dataset Collection",       "DAY" },
    {NITF_BCS_A,  3,  "Month of Dataset Collection",     "MONTH" },
    {NITF_BCS_N,  4,  "Year of Dataset Collection",      "YEAR" },
    {NITF_BCS_A,  2,  "Platform Identification",         "PLATFORM_CODE" },
    {NITF_BCS_N,  2,  "Vehicle Number",                  "VEHICLE_ID" },
    {NITF_BCS_N,  2,  "Pass Number",                     "PASS" },
    {NITF_BCS_N,  3,  "Operation Number",                "OPERATION" },
    {NITF_BCS_A,  2,  "Sensor ID",                       "SENSOR_ID" },  
    {NITF_BCS_A,  2,  "Product ID",                      "PRODUCT_ID" },  
    {NITF_BCS_A,  4,  "Reserved",                        "RESERVED_1" },  
    {NITF_BCS_N, 14,  "Image Start Time",                "TIME" },  
    {NITF_BCS_N, 14,  "Process Completion Time",         "PROCESS_TIME" },  
    {NITF_BCS_N,  2,  "Reserved",                        "RESERVED_2" },  
    {NITF_BCS_N,  2,  "Reserved",                        "RESERVED_3" },  
    {NITF_BCS_A,  1,  "Reserved",                        "RESERVED_4" },  
    {NITF_BCS_A,  1,  "Reserved",                        "RESERVED_5" },  
    {NITF_BCS_A, 10,  "Software Version Used",           "SOFTWARE_VERSION_NUMBER" },
    {NITF_END, 0, NULL, NULL}
};

NITF_DECLARE_SINGLE_PLUGIN(CSDIDA, description)

NITF_CXX_ENDGUARD
