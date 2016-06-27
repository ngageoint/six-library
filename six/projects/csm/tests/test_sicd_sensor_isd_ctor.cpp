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
            throw except::Exception(Ctxt("Usage: " + usage));
        }
        const std::string pathname(argv[1]);
        const std::string type(argv[2]);
        const std::string desid = "XML_DATA_CONTENT";
        const std::string desSubHeader = "DE" + desid +
                std::string("", NITF_DESTAG_SZ - desid.size()) + "01U";
        const std::string desData = "XML_DA77399999XML00000"
            "2016-06-01T00:00:00Z1234567890123456789012345678901234567890"
            "SICD Volume 1 Design & Implementation Description Document  "
            "Version 102016-06-01T00:00:00Z" + std::string("", 120) +
            std::string("1", 125) + std::string("", 360);
        csm::Des des(desSubHeader, desData);
        std::auto_ptr<csm::NitfIsd> nitfIsd(new csm::Nitf21Isd(pathname));
        nitfIsd->addFileDes(des);
        //std::auto_ptr<csm::Isd> fileIsd(new csm::Isd(pathname));

        std::auto_ptr<six::CSM::SIXSensorModel> nitfModel; (*nitfIsd, "");
        //std::auto_ptr<six::CSM::SIXSensorModel> fileModel; (*fileIsd, "");

        if (type == "SICD")
        {
            nitfModel.reset(new six::CSM::SICDSensorModel(*nitfIsd, ""));
            //fileModel.reset(new six::CSM::SICDSensorModel(*fileIsd, ""));
        }
        else if (type == "SIDD")
        {
            nitfModel.reset(new six::CSM::SIDDSensorModel(*nitfIsd, ""));
            //fileModel.reset(new six::CSM::SIDDSensorModel(*fileIsd, ""));
        }
        std::cerr << "Made it!\n";
        //if (nitfModel->getModelState() != fileModel->getModelState())
        //{
        //    return 1;
        //}
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