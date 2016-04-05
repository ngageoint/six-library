#include <six/Enums.h>
#include <six/Utilities.h>

int main(int, char**)
{
    six::DualPolarizationType pType;
    std::string unknownValue("UNKNOWN");

    // Testing Enum ctor
    try
    {
        pType = six::DualPolarizationType(unknownValue);
    }
    catch (except::InvalidFormatException&)
    {
        std::cerr << "Unable to construct DualPolarizationType with value \"" << unknownValue << "\"." << std::endl;
        return 1;
    }
    // Testing toType("UNKNOWN")
    try
    {
        pType = six::toType<six::DualPolarizationType>(unknownValue);
    }
    catch (except::Exception)
    {
        std::cerr << "Unable to convert \"" << unknownValue << "\" to six::DualPolarizationType" << std::endl;
        return 1;
    }

    //Testing toString(six::DualPolarizationType::UNKNOWN
    try
    {
        std::string pString = six::toString<six::DualPolarizationType>(pType);
        if (pString != unknownValue)
        {
            std::cerr << "Expecting six::toString<DualPolarzationType>(six::DualPolarizationType::UNKNOWN) "
                "to be " << unknownValue << ". Actually: " << pString << "." << std::endl;
            return 1;
        }
    }
    catch (except::Exception)
    {
        std::cerr << "Unable to convert six::DualPolarizationType::UNKNOWN to string" << std::endl;
        return 1;
    }
    
    std::cout << "six::DualPolarizationType conversion tests successful" << std::endl;
    return 0;
}