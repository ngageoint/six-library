#include "six/Logger.h"

#include <std/memory>

six::Logger::Logger(logging::Logger*& log, bool& ownLog, std::nullptr_t)
    : mLog(log), mOwnLog(ownLog)
{
}
six::Logger::Logger() : Logger(pLog_, ownLog_, nullptr)
{
}
six::Logger::Logger(std::unique_ptr<logging::Logger>&& pLogger) : Logger()
{
    setLogger(std::move(pLogger));
}
six::Logger::Logger(logging::Logger& logger) : Logger()
{
    setLogger(logger);
}

logging::Logger* six::Logger::get(std::nothrow_t) const
{
    return mLog;
}
logging::Logger& six::Logger::get() const
{
    return *mLog;
}

void six::Logger::setLogger(logging::Logger* logger, bool ownLog)
{
    if (logger)
    {
        // Logger is passed in: set it and determine ownership
        if (ownLog)
        {
            setLogger(std::unique_ptr<logging::Logger>(logger)); // implicitly owns
        }
        else
        {
            mLogger.reset(); // for mOwnLog = true, mLog is turned over to a std::unique_ptr
            mLog = logger;
            mOwnLog = false;
        }
    }
    else
    {
        // No logger passed in: create a null logger
        setLogger(std::make_unique<logging::NullLogger>());
    }
}

void six::Logger::setLogger(std::unique_ptr<logging::Logger>&& logger)
{
    mLogger = std::move(logger);
    mLog = mLogger.get();
    mOwnLog = false; // managed by mLogger which is a std::unique_ptr
}

void six::Logger::setLogger(logging::Logger& logger)
{
    setLogger(&logger, false /*ownLog*/);
}

