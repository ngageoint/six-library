#include <except/Context.h>
#include <except/Exception.h>
#include <sys/Conf.h>
#include <NitfIsd.h>
#include <six/csm/SICDSensorModel.h>
#include <six/csm/SIDDSensorModel.h>

int main(int argc, char** argv)
{
    try
    {
        if (argc != 3)
        {
            const std::string usage = std::string(argv[0]) + 
                    " <NITF file> <SICD | SIDD>";
            throw except::Exception(Ctxt(usage));
        }
        const std::string pathname(argv[1]);
        const std::string type(argv[2]);

        std::auto_ptr<csm::Isd> nitfIsd(new csm::Nitf21Isd(pathname));
        std::auto_ptr<csm::Isd> fileIsd(new csm::Isd(pathname));

        std::auto_ptr<six::CSM::SIXSensorModel> nitfModel; (*nitfIsd, "");
        std::auto_ptr<six::CSM::SIXSensorModel> fileModel; (*fileIsd, "");

        if (type == "SICD")
        {
            nitfModel.reset(new six::CSM::SICDSensorModel(*nitfIsd, ""));
            fileModel.reset(new six::CSM::SICDSensorModel(*fileIsd, ""));
        }
        else if (type == "SIDD")
        {
            nitfModel.reset(new six::CSM::SIDDSensorModel(*nitfIsd, ""));
            fileModel.reset(new six::CSM::SIDDSensorModel(*fileIsd, ""));
        }
        if (nitfModel->getModelState() != fileModel->getModelState())
        {
            return 1;
        }
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
        return 1;
    }
}