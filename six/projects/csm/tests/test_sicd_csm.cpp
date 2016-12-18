#include <iostream>

#include <sys/DLL.h>
#include <sys/Conf.h>
#include <except/Exception.h>
#include <six/Utilities.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>

// CSM includes
#include <RasterGM.h>
#include <Plugin.h>
#include <NitfIsd.h>
/*
#include "Error.h"
#include "Warning.h"
#include "Plugin.h"
#include "RasterGM.h"
#include "FourParameterCorrelationModel.h"
#include "LinearDecayCorrelationModel.h"
#include "Isd.h"
#include "NitfIsd.h"
#include "BytestreamIsd.h"*/

namespace
{
std::string toString(nitf::FileSecurity security)
{
    const std::string str = security.getClassificationSystem().toString() +
            security.getCodewords().toString() +
            security.getControlAndHandling().toString() +
            security.getReleasingInstructions().toString() +
            security.getDeclassificationType().toString() +
            security.getDeclassificationDate().toString() +
            security.getDeclassificationExemption().toString() +
            security.getDowngrade().toString() +
            security.getDowngradeDateTime().toString() +
            security.getClassificationText().toString() +
            security.getClassificationAuthorityType().toString() +
            security.getClassificationAuthority().toString() +
            security.getClassificationReason().toString() +
            security.getSecuritySourceDate().toString() +
            security.getSecurityControlNumber().toString();

    return str;

}
}

int main(int argc, char** argv)
{
    try
    {
        sys::DLL dll("C:\\Users\\Adam\\Documents\\Code\\checkout\\six\\install\\share\\CSM\\plugins\\SIX_SAR_MDAIS_112_winVC140_CSM301.dll");

        const std::string sicdPathname("C:\\Users\\Adam\\Downloads\\sicd_example_RMA_RGZERO_RE32F_IM32F.nitf");

        const std::string confDir("C:\\Users\\Adam\\Documents\\Code\\checkout\\six\\install\\conf");

        const std::string schemaDir(sys::Path(confDir).join("schema").join("six"));

        // TODO: Fill me in!!
        std::vector<std::string> schemaDirs(1, schemaDir);

        /*
        std::auto_ptr<six::sicd::ComplexData> complexData(
                six::sicd::Utilities::getComplexData(sicdPathname, schemaDirs));*/

        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator(six::DataType::COMPLEX,
                new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

        six::NITFReadControl reader;
        reader.setXMLControlRegistry(&xmlRegistry);
        reader.load(sicdPathname, schemaDirs);
        std::auto_ptr<six::sicd::ComplexData> complexData(
                six::sicd::Utilities::getComplexData(reader));

        std::string xmlStr = six::toXMLString(complexData.get(), &xmlRegistry);

        //std::cout << "xHey: " << xmlStr << std::endl;

        csm::Plugin::setDataDirectory(confDir);

        csm::PluginList pluginList = csm::Plugin::getList();

        if (pluginList.size() != 1)
        {
            throw except::Exception(Ctxt("Expected 1 plugin but found " +
                    str::toString(pluginList.size())));
        }

        const csm::Plugin& plugin = **pluginList.begin();

        if (plugin.getPluginName() != "SIX")
        {
            throw except::Exception(Ctxt(
                    "Unexpected plugin name '" + plugin.getPluginName() + "'"));
        }

        std::cout << "Hey: " << plugin.getPluginName() << std::endl;
        std::cout << "Model: " << plugin.getModelName(0) << std::endl;


        // Set data directory!!

        // Try out the file ISD version
        //csm::Isd fileIsd(sicdPathname);
        csm::Isd fileIsd(sicdPathname);
        std::cout << "I constructed the ISD" << std::endl;
        std::cout << "It has a name of " << fileIsd.filename() << std::endl;

        if (plugin.canModelBeConstructedFromISD(fileIsd, "SICD_SENSOR_MODEL"))
        {
            std::cout << "Yep - can construct it!" << std::endl;
        }
        else
        {
            std::cout << "Nope - can't construct it" << std::endl;
        }

        std::auto_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
                plugin.constructModelFromISD(fileIsd, "SICD_SENSOR_MODEL")));

        //4839.5 4299.5 488.0295281264820
        csm::ImageCoord imagePt(4839.5, 4299.5);
        double height = 488.0295281264820;

        csm::EcefCoord coord = model->imageToGround(
                imagePt,
                height,
                0);
        std::cout << "I got " << coord.x << " " << coord.y << " " << coord.z << std::endl;

        // NITF ISD
        csm::Nitf21Isd nitfIsd(sicdPathname);

        csm::Des des;

        nitf::DESegment segment = static_cast<nitf::DESegment>(
                reader.getRecord().getDataExtensions().getFirst().getData());
        nitf::DESubheader subheader = segment.getSubheader();

        std::ostringstream ostr;
        ostr << subheader.getFilePartType().toString()
             << subheader.getTypeID().toString()
             << subheader.getVersion().toString()
             << subheader.getSecurityClass().toString()
             << toString(subheader.getSecurityGroup())
             << std::setw(4) << std::setfill('0')
             << static_cast<nitf::Uint32>(subheader.getSubheaderFieldsLength());

        nitf::TRE tre = subheader.getSubheaderFields();
        for (nitf::TRE::Iterator iter = tre.begin(); iter != tre.end(); ++iter)
        {
            nitf::Pair pair = *iter;
            std::cout << "Key:  '" << pair.getKey() << "'" << std::endl;
            std::cout << "Data: '" << tre.getField(pair.getKey()).toString() << "'" << std::endl;

            ostr << tre.getField(pair.getKey()).toString();
        }

        const std::string subheaderStr = ostr.str();
        std::cout << "Total str: '" << subheaderStr << "'  length " << subheaderStr.length() << std::endl;

        des.setSubHeader(subheaderStr);

        des.setData(xmlStr);

        nitfIsd.addFileDes(des);

        if (plugin.canModelBeConstructedFromISD(nitfIsd, "SICD_SENSOR_MODEL"))
        {
            std::cout << "Yep - can construct it!" << std::endl;
        }
        else
        {
            std::cout << "Nope - can't construct it" << std::endl;
        }

        {
        std::auto_ptr<csm::RasterGM> model(reinterpret_cast<csm::RasterGM*>(
                plugin.constructModelFromISD(nitfIsd, "SICD_SENSOR_MODEL")));

        //std::cout << "Hey: " << (void*)model << std::endl;

        csm::EcefCoord coord = model->imageToGround(
                imagePt,
                height,
                0);
        std::cout << "second time I got " << coord.x << " " << coord.y << " " << coord.z << std::endl;
        }

        // Try out the Nitf21Isd

        // See vts_isd.cpp's initNitf21ISD() for how to do this - you need to
        // fill it out yourself!
        // We can be lazy though - it's only the DES data that we need to populate!

    }
    catch (...)
    {

    }
}
