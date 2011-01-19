#include "nitf/Utils.hpp"

using namespace nitf;

bool Utils::isNumeric(std::string str)
{
    return (bool) nitf_Utils_isNumeric((char*) str.c_str());
}

bool Utils::isAlpha(std::string str)
{
    return (bool) nitf_Utils_isNumeric((char*) str.c_str());
}

void Utils::decimalToGeographic(double decimal, int* degrees, int* minutes,
                                double* seconds)
{
    nitf_Utils_decimalToGeographic(decimal, degrees, minutes, seconds);
}

double Utils::geographicToDecimal(int degrees, int minutes, double seconds)
{
    return nitf_Utils_geographicToDecimal(degrees, minutes, seconds);
}

char Utils::cornersTypeAsCoordRep(nitf::CornersType type)
{
    return nitf_Utils_cornersTypeAsCoordRep(type);
}
