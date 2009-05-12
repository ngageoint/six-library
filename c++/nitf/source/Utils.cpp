#include "nitf/Utils.hpp"

using namespace nitf;

bool Utils::isNumeric(std::string str)
{
    return (bool)nitf_Utils_isNumeric((char*)str.c_str());
}

bool Utils::isAlpha(std::string str)
{
    return (bool)nitf_Utils_isNumeric((char*)str.c_str());
}

void Utils::decimalToGeographic(double decimal, 
                                int* degrees, 
                                int* minutes,
                                double* seconds)
{
    nitf_Utils_decimalToGeographic(decimal, 
                                   degrees, 
                                   minutes, 
                                   seconds);
}

double Utils::geographicToDecimal(int degrees, int minutes, double seconds)
{
    return nitf_Utils_geographicToDecimal(degrees, minutes, seconds);
}
/*
void Utils::parseGeographicString(std::string dms,
                                  int *degrees,
                                  int *minutes,
                                  int *seconds) throw(nitf::Exception)
{
    nitf_Error error;
    if (!nitf_Utils_parseGeographicString((char*)dms.c_str(),
                                          degrees,
                                          minutes,
                                          seconds,
                                          &error))
    {
        throw nitf::Exception(&error);
    }
}

void Utils::parseDecimalString(std::string d, double* decimal)
    throw (nitf::Exception)
{
    nitf_Error error;
    if (!nitf_Utils_parseDecimalString((char*)dms.c_str(),
                                       decimal
                                       &error))
    {
        throw nitf::Exception(&error);
    }
}
*/
char Utils::cornersTypeAsCoordRep(nitf::CornersType type)
{
    return nitf_Utils_cornersTypeAsCoordRep(type);
}
