///////////////////////////////////////////////////////////
//  DefaultLogger.cpp
///////////////////////////////////////////////////////////

#include "logging/DefaultLogger.h"

    
logging::LogLevel logging::DefaultLogger::defaultLogLevel = logging::LOG_WARN;


logging::DefaultLogger::DefaultLogger(std::string name)
    : logging::Logger(name)
{
    //TODO might be able to share just one amongst all DefaultLoggers -- just a thought
    mDefaultHandler = new logging::StreamHandler(defaultLogLevel);
    this->addHandler(mDefaultHandler);
}

logging::DefaultLogger::~DefaultLogger()
{
    if (mDefaultHandler)
    {
        this->removeHandler(mDefaultHandler);
        delete mDefaultHandler;
    }
}


void logging::DefaultLogger::setDefaultLogLevel(logging::LogLevel logLevel)
{
    logging::DefaultLogger::defaultLogLevel = logLevel;
}
