#ifndef __SIO_LITE_UNSUPPORTED_DATA_TYPE_EXCEPTION_H__
#define __SIO_LITE_UNSUPPORTED_DATA_TYPE_EXCEPTION_H__

#include <import/except.h>


namespace sio
{
namespace lite
{
class UnsupportedDataTypeException : public except::Exception
{
public:
    //!  Default constructor
    UnsupportedDataTypeException() {}

    /*!
     *  Constructor with a rhs message
     *  \param message The message to report
     *
     */
    UnsupportedDataTypeException(const char *message) :
            except::Exception(message) {}

    /*!
     *  Constructor with a rhs message
     *  \param message The message to report
     *
     */
    UnsupportedDataTypeException(const std::string& message)  :
            except::Exception(message) {}

    /*!
     *  Constructor with a context
     *  \param c The context with the message to report
     *
     */
    UnsupportedDataTypeException(const except::Context& c)  :
            except::Exception(c) {}
};
}
}
#endif
