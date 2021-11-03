#include "six/Logger.h"

six::Logger::Logger(logging::Logger*& log, bool& ownLog)
    : mLog(log), mOwnLog(ownLog)
{
}
six::Logger::Logger() : Logger(pLog_, ownLog_)
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

six::Logger::~Logger()
{
    if (mLog && mOwnLog)
    {
        delete mLog;
    }
}

six::Logger& six::Logger::operator=(Logger&&) noexcept = default;

logging::Logger* six::Logger::get()
{
    return mLog;
}

void six::Logger::deleteLogger(const logging::Logger* logger)
{
    // Delete the current logger if it exists and is owned
    if (mLog && mOwnLog && (logger != mLog))
    {
        delete mLog;
    }
    mLogger.reset();
}

void six::Logger::setLogger(logging::Logger* logger, bool ownLog)
{
    deleteLogger(logger);

    if (logger)
    {
        // Logger is passed in: set it and determine ownership
        if (ownLog)
        {
            setLogger(std::unique_ptr<logging::Logger>(logger)); // implicitly owns
        }
        else
        {
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
void six::Logger::setLogger(logging::Logger* logger)
{
    setLogger(logger, false /*ownLog*/);
}

void six::Logger::setLogger(std::unique_ptr<logging::Logger>&& logger)
{
    deleteLogger();

    // If no logger passed in, create a null logger
    mLogger = logger.get() != nullptr ? std::move(logger) : std::make_unique<logging::NullLogger>();
    mLog = mLogger.get();
    mOwnLog = false; // managed by mLogger which is a std::unique_ptr
}

void six::Logger::setLogger(logging::Logger& logger)
{
    deleteLogger();
    mLog = &logger;
    mOwnLog = false;
}

