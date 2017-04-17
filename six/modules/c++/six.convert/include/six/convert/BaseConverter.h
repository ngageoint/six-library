#ifndef __CONVERT_BASE_CONVERTER_H__
#define __CONVERT_BASE_CONVERTER_H__

#include <string>
#include <six/sicd/ComplexData.h>
#include <six/XMLParser.h>
#include <xml/lite/Document.h>

namespace six
{
namespace convert
{
class BaseConverter : protected six::XMLParser
{
public:
    /*!
     * \class BaseConverter
     *
     * Base object for parsing vendor XML to create ComplexData
     */
    BaseConverter();

    /*!
     * Create a ComplexData object from source filetype
     *
     * \return ComplexData corresponding to given file
     */
    virtual std::auto_ptr<six::sicd::ComplexData> convert() const = 0;

protected:
    static std::auto_ptr<xml::lite::Document>
            readXML(const std::string& xmlPathname);

    XMLElem findUniqueElement(const XMLElem root,
            const std::string& xmlPath) const;
    std::vector<XMLElem> findElements(const XMLElem root,
            const std::string& xmlPath) const;
    size_t findIndex(const std::vector<XMLElem>& elements,
            const std::string& xmlPath, const std::string& value) const;
    size_t findIndexByAttributes(const std::vector<XMLElem>& elements,
            const std::vector<std::string>& attributes,
            const std::vector<std::string>& values) const;
    size_t findIndexByAttribute(const std::vector<XMLElem>& elements,
            const std::string& attribute, const std::string& value) const;

    std::string parseStringFromPath(const XMLElem root,
            const std::string& xmlPath) const;
    double parseDoubleFromPath(const XMLElem root,
            const std::string& xmlPath) const;
    size_t parseUIntFromPath(const XMLElem root,
            const std::string& xmlPath) const;
    six::DateTime parseDateTimeFromPath(const XMLElem root,
            const std::string& xmlPath) const;

    double sumOverElements(const std::vector<XMLElem>& elements,
            const std::string& xmlPath) const;
};

}
}
#endif

