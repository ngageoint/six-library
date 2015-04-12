///////////////////////////////////////////////////////////
//  LoggerFactory.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_LOGGER_FACTORY_H__
#define __LOGGING_LOGGER_FACTORY_H__

#include <map>
#include <import/sys.h>
#include <import/mt.h>
#include <mem/SharedPtr.h>
#include "logging/DefaultLogger.h"

namespace logging
{

/*!
 * \class LoggerManager
 * \brief The LoggerManager class maintains a static list of Loggers that can
 * be accessed from any context of your application simply by calling the
 * static getLogger() method. All Loggers are kept as pointers to heap memory,
 * and will get destructed at exit.
 */
class LoggerManager
{
private:
    std::map<std::string, mem::SharedPtr<Logger> > mLoggerMap; //! map for storing Loggers
    sys::Mutex mMutex; //! mutex used for locking the map

public:
    LoggerManager()
    {
    }

    /*!
     * Returns the Logger with the specified name. If a logger with the
     * given name does not exist, a new Logger is created, loaded with the
     * default values in the sytem. If the name is not supplied, the root logger
     * is used by default.
     */
    mem::SharedPtr<Logger> getLoggerSharedPtr(const std::string& name = "root");

    /*!
     * Returns the Logger with the specified name. If a logger with the
     * given name does not exist, a new Logger is created, loaded with the 
     * default values in the sytem. If the name is not supplied, the root logger
     * is used by default.
     */
    Logger* getLogger(const std::string& name = "root")
    {
        return getLoggerSharedPtr(name).get();
    }
};

/*!
 * This is where all the magic happens. The LoggerFactory is a LoggerManager
 * Singleton. Therefore, you can use a shared LoggerManager by using the
 * LoggerFactory.
 */
typedef mt::Singleton<LoggerManager, true> LoggerFactory;


//below are some shortcuts at the namespace level
//this allows you to call logging::warn("foo") from anywhere
//and it will use the root logger that is managed by the factory singleton

//! Logs a message at the DEBUG LogLevel to the 'root' logger
void debug(const std::string& msg);
//! Logs a message at the INFO LogLevel to the 'root' logger
void info(const std::string& msg);
//! Logs a message at the WARNING LogLevel to the 'root' logger
void warn(const std::string& msg);
//! Logs a message at the ERROR LogLevel to the 'root' logger
void error(const std::string& msg);
//! Logs a message at the CRITICAL LogLevel to the 'root' logger
void critical(const std::string& msg);

//! Logs an Exception Context at the DEBUG LogLevel to the 'root' logger
void debug(const except::Context& ctxt);
//! Logs an Exception Context at the INFO LogLevel to the 'root' logger
void info(const except::Context& ctxt);
//! Logs an Exception Context at the WARNING LogLevel to the 'root' logger
void warn(const except::Context& ctxt);
//! Logs an Exception Context at the ERROR LogLevel to the 'root' logger
void error(const except::Context& ctxt);
//! Logs an Exception Context at the CRITICAL LogLevel to the 'root' logger
void critical(const except::Context& ctxt);

//! Logs a Throwable at the DEBUG LogLevel to the 'root' logger
void debug(except::Throwable& t);
//! Logs a Throwable at the INFO LogLevel to the 'root' logger
void info(except::Throwable& t);
//! Logs a Throwable at the WARNING LogLevel to the 'root' logger
void warn(except::Throwable& t);
//! Logs a Throwable at the ERROR LogLevel to the 'root' logger
void error(except::Throwable& t);
//! Logs a Throwable at the CRITICAL LogLevel to the 'root' logger
void critical(except::Throwable& t);

//! Sets the LogLevel for the default 'root' logger
//TODO deprecate this
void setLogLevel(LogLevel level);

//! get a Logger of the given name
Logger* getLogger(const std::string& name = "root");
mem::SharedPtr<Logger> getLoggerSharedPtr(const std::string& name = "root");

}
#endif
