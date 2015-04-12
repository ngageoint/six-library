#include <import/plugin.h>
#include <import/mt.h>
#include <import/common/proc.h>
#include "common/proc/impl/Version.h"

using namespace plugin;
using namespace common::proc;

const char* __TASK_NAME_UC_QUAL___TASK_OP = "__TASK_NAME__";

__NAMESPACE_START__
class __TASK_NAME__Task : public AbstractTask
{
public:

    __TASK_NAME__Task(std::string& configDir) :
        mConfigDir(configDir)
    {
    }
    virtual ~__TASK_NAME__Task()
    {
    }

    void run(common::proc::Immutables_T& constants,
             common::proc::Mutables_T& data)
    {
        try
        {
            // YOUR CODE HERE
        }
        catch(except::Throwable& t)
        {
            getLogger()->error(t);
        }
    }

private:

    std::string mConfigDir;
};
__NAMESPACE_END__

class __TASK_NAME__TaskCreator : public TaskCreator
{
public:
    __TASK_NAME__TaskCreator()
    {
    }
    ~__TASK_NAME__TaskCreator()
    {

    }
    Task* createTask(std::string configDir)
    {
        return new __QUAL_NAME__Task(configDir);
    }
    void destroy(Task*& task)
    {
        delete task;
        task = NULL;
    }
};

class Ident : public PluginIdentity<TaskCreator>
{
public:
    Ident()
    {
    }
    ~Ident()
    {
    }
    const char** getOperations()
    {
        static const char* p[] =
        { __TASK_NAME_UC_QUAL___TASK_OP, NULL };
        return p;
    }
    int getMajorVersion()
    {
        return common::proc::impl::MAJOR_VERSION;
    }
    int getMinorVersion()
    {
        return common::proc::impl::MINOR_VERSION;
    }
    TaskCreator* spawnHandler()
    {
        return new __TASK_NAME__TaskCreator();
    }
    void destroyHandler(TaskCreator*& handler)
    {
        delete handler;
    }
};
PLUGIN_EXPOSE_IDENT(Ident, PluginIdentity<TaskCreator>)
