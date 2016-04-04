#include <six/Enums.h>
#include <six/Utilities.h>

int main(int, char**)
{
    six::PolarizationType pType;
    std::string unknownValue("UNKNOWN");

    // Testing Enum ctor
    try
    {
        pType = six::PolarizationType(unknownValue);
    }
    catch (except::InvalidFormatException&)
    {
        std::cerr << "Unable to construct PolarizationType with value \"" << unknownValue << "\"." << std::endl;
        return 1;
    }
    // Testing toType("UNKNOWN")
    try
    {
        pType = six::toType<six::PolarizationType>(unknownValue);
    }
    catch (except::Exception)
    {
        std::cerr << "Unable to convert \"" << unknownValue << "\" to six::PolarizationType" << std::endl;
        return 1;
    }

    //Testing toString(six::PolarizationType::UNKNOWN
    try
    {
        std::string pString = six::toString<six::PolarizationType>(pType);
        if (pString != unknownValue)
        {
            std::cerr << "Expecting six::toString<PolarzationType>(six::PolarizationType::UNKNOWN) "
                "to be " << unknownValue << ". Actually: " << pString << "." << std::endl;
            return 1;
        }
    }
    catch (except::Exception)
    {
        std::cerr << "Unable to convert six::PolarizationType::UNKNOWN to string" << std::endl;
        return 1;
    }
    
    std::cout << "six::PolarizationType conversion tests successful" << std::endl;
    return 0;
}