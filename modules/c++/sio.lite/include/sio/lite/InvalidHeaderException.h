#ifndef __SIO_LITE_INVALID_HEADER_EXCEPTION_H__
#define __SIO_LITE_INVALID_HEADER_EXCEPTION_H__

#include <import/except.h>


namespace sio
{
namespace lite
{
class InvalidHeaderException : public except::Exception
{
public:
    //!  Default constructor
    InvalidHeaderException() {}

    /*!
     *  Constructor with a rhs message
     *  \param message The message to report
     *
     */
    InvalidHeaderException(const char *message) :
            except::Exception(message) {}

    /*!
     *  Constructor with a rhs message
     *  \param message The message to report
     *
     */
    InvalidHeaderException(const std::string& message)  :
            except::Exception(message) {}

    /*!
     *  Constructor with a context
     *  \param c The context with the message to report
     *
     */
    InvalidHeaderException(const except::Context& c)  :
            except::Exception(c) {}
};
}
}

#endif
