/* =========================================================================
* This file is part of the CSM SICD Plugin
* =========================================================================
*
* (C) Copyright 2004 - 2011, General Dynamics - Advanced Information Systems
*
* The CSM SICD Plugin is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this program; if not,
* see <http://www.gnu.org/licenses/>.
*
*/

#include "Error.h"
#include <sys/OS.h>
#include <sys/Path.h>
#include <six/csm/SICDSensorModel.h>
#include <io/StringStream.h>
#include <logging/NullLogger.h>
#include <six/XMLControlFactory.h>
#include <six/NITFReadControl.h>
#include <six/sicd/ComplexXMLControl.h>
#include <six/sicd/Utilities.h>

namespace six
{
namespace csm
{
const ::csm::Version SICDSensorModel::VERSION(1, 0, 2);
const char SICDSensorModel::NAME[] = "SICD_SENSOR_MODEL";

SICDSensorModel::SICDSensorModel(const ::csm::Isd& isd,
                                 const std::string& dataDir)
{
    setSchemaDir(dataDir);

    const std::string& format(isd.format());

    if (format == "NITF2.1")
    {
        initializeFromISD(dynamic_cast<const ::csm::Nitf21Isd&>(isd));
    }
    else if (format == "FILENAME")
    {
        // Note: this case has not been tested
        initializeFromFile(isd.filename());
    }
    else
    {
        throw ::csm::Error(::csm::Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE,
                           "Unsupported ISD format " + format,
                           "SICDSensorModel::constructModelFromISD");
    }

}

SICDSensorModel::SICDSensorModel(const std::string& sensorModelState,
                                 const std::string& dataDir)
{
    setSchemaDir(dataDir);
    replaceModelStateImpl(sensorModelState);
}

void SICDSensorModel::initializeFromFile(const std::string& pathname)
{
    try
    {
        // create an XML registry
        // The reason to do this is to avoid adding XMLControlCreators to the
        // XMLControlFactory singleton - this way has more fine-grained control
        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator(six::DataType::COMPLEX,
                new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

        // create a reader and load the file
        six::NITFReadControl reader;
        reader.setXMLControlRegistry(&xmlRegistry);
        reader.load(pathname, mSchemaDirs);

        six::Container* const container = reader.getContainer();
        if (container->getDataType() != six::DataType::COMPLEX ||
            container->getNumData() != 1 ||
            container->getData(0)->getDataType() != six::DataType::COMPLEX)
        {
            throw ::csm::Error(::csm::Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE,
                               "Not a SICD",
                               "SICDSensorModel::initializeFromFile");
        }

        mData.reset(reinterpret_cast<six::sicd::ComplexData*>(
                container->getData(0)));
        container->removeData(mData.get());

        // get xml as string for sensor model state
        std::string xmlStr = six::toXMLString(mData.get(), &xmlRegistry);
        mSensorModelState = NAME + std::string(" ") + xmlStr;

        mGeometry.reset(six::sicd::Utilities::getSceneGeometry(mData.get()));
        mProjection.reset(six::sicd::Utilities::getProjectionModel(mData.get(),
                mGeometry.get()));
    }
    catch (const except::Exception& ex)
    {
        throw ::csm::Error(::csm::Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE,
                           ex.getMessage(),
                           "SICDSensorModel::initializeFromFile");
    }
}

void SICDSensorModel::initializeFromISD(const ::csm::Nitf21Isd& isd)
{
    try
    {
        // Check for the first SICD DES and parse it
        xml::lite::Document* sicdXML = NULL;
        xml::lite::MinidomParser domParser;

        const std::vector< ::csm::Des>& desList(isd.fileDess());
        for (size_t ii = 0; ii < desList.size(); ++ii)
        {
            const std::string& desData(desList[ii].data());

            if (!desData.empty())
            {
                try
                {
                    io::StringStream stream;
                    stream.write(desData.c_str(), desData.length());

                    domParser.clear();
                    domParser.parse(stream);

                    const std::string localName = domParser.getDocument()->
                            getRootElement()->getLocalName();
                    if (localName == "SICD")
                    {
                        sicdXML = domParser.getDocument();
                        break;
                    }
                    else if (localName == "SIDD")
                    {
                        // SIDD NITFs may also contain SICD DESs (that describe
                        // the SICD that this SIDD was derived from).  I guess
                        // technically we could build up a sensor model from
                        // this, but it seems more correct to error out so that
                        // the SIDDSensorModel will get used.  Note that a SIDD
                        // DES will always appear prior to a SICD DES, so if we
                        // do encounter a SICD DES before this point, we can
                        // safely say it's a SICD NITF.
                        throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
                                           "SIDD, not SICD, NITF",
                                           "SICDSensorModel::SICDSensorModel");
                    }
                }
                catch(const except::Exception& )
                {
                    // Couldn't parse DES as xml -- it's not a sicd so skip it
                }
            }
        }

        if (sicdXML == NULL)
        {
            throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
                               "Not a SICD",
                               "SICDSensorModel::SICDSensorModel");
        }

        // get xml as string for sensor model state
        io::StringStream stringStream;
        sicdXML->getRootElement()->print(stringStream);
        mSensorModelState = NAME + std::string(" ") + stringStream.stream().str();

        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator(six::DataType::COMPLEX,
                new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

        logging::NullLogger logger;
        std::auto_ptr<six::XMLControl> control(
                xmlRegistry.newXMLControl(six::DataType::COMPLEX, &logger));

        mData.reset(reinterpret_cast<six::sicd::ComplexData*>(control->fromXML(
                sicdXML, mSchemaDirs)));
        mGeometry.reset(six::sicd::Utilities::getSceneGeometry(mData.get()));
        mProjection.reset(six::sicd::Utilities::getProjectionModel(mData.get(),
                mGeometry.get()));
    }
    catch (const except::Exception& ex)
    {
        throw ::csm::Error(::csm::Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE,
                           ex.getMessage(),
                           "SICDSensorModel::initializeFromISD");
    }
}

bool SICDSensorModel::containsComplexDES(const ::csm::Nitf21Isd& isd)
{
    xml::lite::MinidomParser domParser;

    const std::vector< ::csm::Des>& desList(isd.fileDess());
    for (size_t ii = 0; ii < desList.size(); ++ii)
    {
        const std::string& desData(desList[ii].data());

        if (!desData.empty())
        {
            try
            {
                io::StringStream stream;
                stream.write(desData.c_str(), desData.length());

                domParser.clear();
                domParser.parse(stream);

                const std::string localName = domParser.getDocument()->
                        getRootElement()->getLocalName();
                if (localName == "SICD")
                {
                    return true;
                }
                else if (localName == "SIDD")
                {
                    // SIDD NITFs may also contain SICD DESs (that describe the
                    // SICD that this SIDD was derived from).  I guess
                    // technically we could build up a sensor model from this,
                    // but it seems more correct to return false so that the
                    // SIDDSensorModel gets picked.  Note that a SIDD DES will
                    // always appear prior to a SICD DES, so if we do encounter
                    // a SICD DES before this point, we can safely say it's a
                    // SICD NITF.
                    return false;
                }
            }
            catch(const except::Exception& )
            {
                // Couldn't parse DES as xml -- it's not a sicd so skip it
            }
        }
    }

    return false;
}

::csm::Version SICDSensorModel::getVersion() const
{
    return VERSION;
}

std::string SICDSensorModel::getModelName() const
{
    return NAME;
}

std::string SICDSensorModel::getPedigree() const
{
    return (mData->getSource() + "_" + NAME + "_SAR");
}

std::string SICDSensorModel::getImageIdentifier() const
{
    return mData->getName();
}

void SICDSensorModel::setImageIdentifier(const std::string& imageId,
                                         ::csm::WarningList* )

{
    mData->setName(imageId);
}

std::string SICDSensorModel::getSensorIdentifier() const
{
    return mData->getSource();
}

std::string SICDSensorModel::getPlatformIdentifier() const
{
    return mData->getSource();
}

std::string SICDSensorModel::getCollectionIdentifier() const
{
    // same as imageIdentifier right now...
    return mData->collectionInformation->coreName;
}

std::string SICDSensorModel::getSensorMode() const
{
    switch (mData->collectionInformation->radarMode)
    {
    case six::RadarModeType::SPOTLIGHT:
        return CSM_SENSOR_MODE_SPOT;
    case six::RadarModeType::STRIPMAP:
    case six::RadarModeType::DYNAMIC_STRIPMAP:
        return CSM_SENSOR_MODE_STRIP;
    default:
        return CSM_SENSOR_MODE_FRAME;
    }
}

std::string SICDSensorModel::getReferenceDateAndTime() const
{
    return mData->timeline->collectStart.format("%Y%m%dT%H%M%.2SZ");
}

std::string SICDSensorModel::getModelState() const
{
    return mSensorModelState;
}

void SICDSensorModel::replaceModelState(const std::string& argState)
{
    if (!argState.empty())
    {
        replaceModelStateImpl(argState);
    }
}

int SICDSensorModel::getNumParameters() const
{
    return scene::AdjustableParam::size;
}

std::string SICDSensorModel::getParameterName(int index) const
{
    if (index < 0 || index >= getNumParameters() )
    {
        throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SICDSensorModel::getParameterName");
    }
    return scene::AdjustableParam::name(index);
}

std::string SICDSensorModel::getParameterUnits(int index) const
{
    if (index < 0 || index >= getNumParameters())
    {
        throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SICDSensorModel::getParameterUnits");
    }
    return scene::AdjustableParam::units(index);
}

double SICDSensorModel::getParameterValue(int index) const
{
    if (index < 0 || index >= getNumParameters())
    {
        throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SICDSensorModel::getParameterValue");
    }
    return mProjection->getAdjustableParameter(index);
}

void SICDSensorModel::setParameterValue(int index, double value)
{
    if (index < 0 || index >= getNumParameters())
    {
        throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SICDSensorModel::setParameterValue");
    }
    mProjection->setAdjustableParameter( index, value);
}

::csm::param::Type SICDSensorModel::getParameterType(int index) const
{
    if (index < 0 || index >= getNumParameters())
    {
        throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SICDSensorModel::getParameterType");
    }
    return AdjustableTypes[index];
}

void SICDSensorModel::setParameterType(int index, ::csm::param::Type pType)
{
    if (index < 0 || index >= getNumParameters())
    {
        throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SICDSensorModel::setParameterType");
    }
    AdjustableTypes[index] = pType;
}

double SICDSensorModel::getParameterCovariance(int index1, int index2) const
{
    if (index1 < 0 || index1 >= getNumParameters() || index2 < 0 || index2 >= getNumParameters() )
    {
        throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SICDSensorModel::getParameterCovariance");
    }
    return mProjection->getErrorCovariance(index1, index2);
}

void SICDSensorModel::setParameterCovariance(int index1, int index2,
        double covariance)
{
    if (index1 < 0 || index1 >= getNumParameters() || index2 < 0 || index2 >= getNumParameters() )
    {
        throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SICDSensorModel::setParameterCovariance");
    }
    mProjection->setErrorCovariance(index1, index2, covariance);
}

::csm::EcefCoord SICDSensorModel::getReferencePoint() const
{
    const scene::Vector3 refPt = mGeometry->getReferencePosition();
    return ::csm::EcefCoord(refPt[0], refPt[1], refPt[2]);
}

types::RowCol<double>
SICDSensorModel::toPixel(const types::RowCol<double>& pos) const
{
    const six::sicd::ImageData& imageData(*mData->imageData);
    const types::RowCol<double> aoiOffset(imageData.firstRow,
                                          imageData.firstCol);

    const types::RowCol<double> offset(
            imageData.scpPixel.row - aoiOffset.row,
            imageData.scpPixel.col - aoiOffset.col);

    return types::RowCol<double>(
            (pos.row / mData->grid->row->sampleSpacing) + offset.row,
            (pos.col / mData->grid->col->sampleSpacing) + offset.col);
}

types::RowCol<double>
SICDSensorModel::fromPixel(const ::csm::ImageCoord& pos) const
{
    const six::sicd::ImageData& imageData(*mData->imageData);
    const types::RowCol<double> aoiOffset(imageData.firstRow,
                                          imageData.firstCol);

    const types::RowCol<double> adjustedPos(
            pos.line - (imageData.scpPixel.row - aoiOffset.row),
            pos.samp - (imageData.scpPixel.col - aoiOffset.col));

    return types::RowCol<double>(
            adjustedPos.row * mData->grid->row->sampleSpacing,
            adjustedPos.col * mData->grid->col->sampleSpacing);
}

::csm::ImageCoord SICDSensorModel::groundToImage(
        const ::csm::EcefCoord& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        ::csm::WarningList* ) const
{
    try
    {
        scene::Vector3 sceneGroundPt;
        sceneGroundPt[0] = groundPt.x;
        sceneGroundPt[1] = groundPt.y;
        sceneGroundPt[2] = groundPt.z;

        // TODO: Currently no way to specify desiredPrecision when calling
        //       sceneToImage()
        double timeCOA(0.0);
        const types::RowCol<double> imagePt =
                mProjection->sceneToImage(sceneGroundPt, &timeCOA);
        const types::RowCol<double> pixelPt = toPixel(imagePt);

        // TODO: Currently no way to determine the actual precision that was
        //       achieved, so setting it to the desired precision
        if (achievedPrecision)
        {
            *achievedPrecision = desiredPrecision;
        }

        return ::csm::ImageCoord(pixelPt.row, pixelPt.col);
    }
    catch (const except::Exception& ex)
    {
        throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "SICDSensorModel::groundToImage");
    }
}

::csm::EcefCoord SICDSensorModel::imageToGround(
        const ::csm::ImageCoord& imagePt,
        double height,
        double desiredPrecision,
        double* achievedPrecision,
        ::csm::WarningList* ) const
{
    try
    {
        const types::RowCol<double> imagePtMeters = fromPixel(imagePt);

        // TODO: imageToScene() supports specifying a height threshold in
        //       meters but it's not obvious how to convert that to a desired
        //       precision in pixels.  Likewise, not clear how to determine
        //       the achieved precision in pixels afterwards.
        const scene::Vector3 groundPt =
                mProjection->imageToScene(imagePtMeters, height);

        if (achievedPrecision)
        {
            *achievedPrecision = desiredPrecision;
        }

        return ::csm::EcefCoord(groundPt[0], groundPt[1], groundPt[2]);
    }
    catch (const except::Exception& ex)
    {
        throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "SICDSensorModel::imageToGround");
    }
}

::csm::ImageCoord SICDSensorModel::getImageStart() const
{
    return ::csm::ImageCoord(0.0, 0.0);
}

::csm::ImageVector SICDSensorModel::getImageSize() const
{
    return ::csm::ImageVector(mData->getNumRows(), mData->getNumCols());
}

::csm::EcefVector SICDSensorModel::getIlluminationDirection(
        const ::csm::EcefCoord& groundPt) const
{
    scene::Vector3 groundPos;
    groundPos[0] = groundPt.x;
    groundPos[1] = groundPt.y;
    groundPos[2] = groundPt.z;

    scene::Vector3 illumVec = groundPos - mGeometry->getARPPosition();
    illumVec.normalize();

    return ::csm::EcefVector(illumVec[0], illumVec[1], illumVec[2]);
}

double SICDSensorModel::getImageTime(const ::csm::ImageCoord& imagePt) const
{
    try
    {
        return mProjection->computeImageTime(fromPixel(imagePt));
    }
    catch (const except::Exception& ex)
    {
        throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "SICDSensorModel::getImageTime");
    }
}

::csm::EcefCoord
SICDSensorModel::getSensorPosition(const ::csm::ImageCoord& imagePt) const
{
    try
    {
        const double time = mProjection->computeImageTime(fromPixel(imagePt));
        const six::Vector3 pos = mProjection->computeARPPosition(time);
        return ::csm::EcefCoord(pos[0], pos[1], pos[2]);
    }
    catch (const except::Exception& ex)
    {
        throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "SICDSensorModel::getSensorPosition");
    }
}

::csm::EcefCoord SICDSensorModel::getSensorPosition(double time) const
{
    try
    {
        const six::Vector3 pos = mProjection->computeARPPosition(time);
        return ::csm::EcefCoord(pos[0], pos[1], pos[2]);
    }
    catch (const except::Exception& ex)
    {
        throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "SICDSensorModel::getSensorPosition");
    }
}

::csm::EcefVector
SICDSensorModel::getSensorVelocity(const ::csm::ImageCoord& imagePt) const
{
    try
    {
        const double time = mProjection->computeImageTime(fromPixel(imagePt));
        const six::Vector3 vel = mProjection->computeARPVelocity(time);
        return ::csm::EcefVector(vel[0], vel[1], vel[2]);
    }
    catch (const except::Exception& ex)
    {
        throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "SICDSensorModel::getSensorVelocity");
    }
}

::csm::EcefVector SICDSensorModel::getSensorVelocity(double time) const
{
    try
    {
        const six::Vector3 vel = mProjection->computeARPVelocity(time);
        return ::csm::EcefVector(vel[0], vel[1], vel[2]);
    }
    catch (const except::Exception& ex)
    {
        throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "SICDSensorModel::getSensorVelocity");
    }
}

void SICDSensorModel::replaceModelStateImpl(const std::string& sensorModelState)
{
    const size_t idx = sensorModelState.find(' ');
    if (idx == std::string::npos)
    {
        throw ::csm::Error(::csm::Error::INVALID_SENSOR_MODEL_STATE,
                           "Invalid sensor model state",
                           "SICDSensorModel::replaceModelStateImpl");
    }

    const std::string sensorModelName = sensorModelState.substr(0, idx);
    if (sensorModelName != NAME)
    {
        throw ::csm::Error(::csm::Error::INVALID_SENSOR_MODEL_STATE,
                           "Invalid sensor model state",
                           "SICDSensorModel::replaceModelStateImpl");
    }

    const std::string sensorModelXML = sensorModelState.substr(idx + 1);

    try
    {
        io::StringStream stream;
        stream.write(sensorModelXML.c_str(), sensorModelXML.length());

        xml::lite::MinidomParser domParser;
        domParser.parse(stream);

        six::XMLControlRegistry xmlRegistry;
        xmlRegistry.addCreator(six::DataType::COMPLEX,
                new six::XMLControlCreatorT<six::sicd::ComplexXMLControl>());

        logging::NullLogger logger;
        std::auto_ptr<six::XMLControl> control(
                xmlRegistry.newXMLControl(six::DataType::COMPLEX, &logger));

        // get xml as string for sensor model state
        mSensorModelState = sensorModelState;

        mData.reset(reinterpret_cast<six::sicd::ComplexData*>(control->fromXML(
                domParser.getDocument(), mSchemaDirs)));

        mGeometry.reset(six::sicd::Utilities::getSceneGeometry(mData.get()));
        mProjection.reset(six::sicd::Utilities::getProjectionModel(mData.get(),
                mGeometry.get()));
    }
    catch (const except::Exception& ex)
    {
        throw ::csm::Error(::csm::Error::INVALID_SENSOR_MODEL_STATE,
                           ex.getMessage(),
                           "SICDSensorModel::replaceModelStateImpl");
    }
}

int SICDSensorModel::getNumParameters() const
{
	return scene::AdjustableParam::size;
}

std::string SICDSensorModel::getParameterName(int index) const
{
	if (index < 0 || index >= getNumParameters() )
	{
		throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
				"Invalid index in call in function call",
				" SICDSensorModel::getParameterName");
	}
	return scene::AdjustableParam::name(index);
}

std::string SICDSensorModel::getParameterUnits(int index) const
{
	if (index < 0 || index >= getNumParameters())
	{
		throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
				"Invalid index in call in function call",
				" SICDSensorModel::getParameterUnits");
	}
	return scene::AdjustableParam::units(index);
}

double SICDSensorModel::getParameterValue(int index) const
{
	if (index < 0 || index >= getNumParameters())
	{
		throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
				"Invalid index in call in function call",
				" SICDSensorModel::getParameterValue");
	}
	return mProjection->getAdjustableParameter(index);
}

void SICDSensorModel::setParameterValue(int index, double value)
{
	if (index < 0 || index >= getNumParameters())
	{
		throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
				"Invalid index in call in function call",
				" SICDSensorModel::setParameterValue");
	}
	mProjection->setAdjustableParameter( index, value);
}

::csm::param::Type SICDSensorModel::getParameterType(int index) const
{
	if (index < 0 || index >= getNumParameters())
	{
		throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
				"Invalid index in call in function call",
				" SICDSensorModel::getParameterType");
	}
	return AdjustableTypes[index];
}

void SICDSensorModel::setParameterType(int index, ::csm::param::Type pType)
{
	if (index < 0 || index >= getNumParameters())
	{
		throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
				"Invalid index in call in function call",
				" SICDSensorModel::setParameterType");
	}
	AdjustableTypes[index] = pType;
}

double SICDSensorModel::getParameterCovariance(int index1, int index2) const
{
	if (index1 < 0 || index1 >= getNumParameters() || index2 < 0 || index2 >= getNumParameters() )
	{
		throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
				"Invalid index in call in function call",
				" SICDSensorModel::getParameterCovariance");
	}
	return mProjection->getErrorCovariance(index1, index2);
}

void SICDSensorModel::setParameterCovariance(int index1, int index2,
		double covariance)
{
	if (index1 < 0 || index1 >= getNumParameters() || index2 < 0 || index2 >= getNumParameters() )
	{
		throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
				"Invalid index in call in function call",
				" SICDSensorModel::setParameterCovariance");
	}
	mProjection->setErrorCovariance(index1, index2, covariance);
}

int SICDSensorModel::getNumGeometricCorrectionSwitches() const
{
	return 0;
}

std::vector<double> SICDSensorModel::computeGroundPartials(
		const ::csm::EcefCoord& groundPt) const
{
	double timeCOA(0.0);
	scene::Vector3 sceneGroundPt;
	sceneGroundPt[0] = groundPt.x;
	sceneGroundPt[1] = groundPt.y;
	sceneGroundPt[2] = groundPt.z;
	types::RowCol<double> imagePt = mProjection->sceneToImage(sceneGroundPt, &timeCOA);

	//sceneToImagePartials return value is in m/m, computeGroundPartials wants pixels/m
	math::linear::MatrixMxN<2, 3, double> groundpartials= mProjection->sceneToImagePartials(sceneGroundPt, imagePt);
	std::vector<double> return_value(6,0);
	return_value[0] = groundpartials[0][0]/mData->grid->row->sampleSpacing;
	return_value[1] = groundpartials[0][1]/mData->grid->row->sampleSpacing;
	return_value[2] = groundpartials[0][2]/mData->grid->row->sampleSpacing;
	return_value[3] = groundpartials[1][0]/mData->grid->col->sampleSpacing;
	return_value[4] = groundpartials[1][1]/mData->grid->col->sampleSpacing;
	return_value[5] = groundpartials[1][2]/mData->grid->col->sampleSpacing;
	return return_value;
}

//std::vector<double> SICDSensorModel::getUnmodeledCrossCovariance(
//		const ::csm::ImageCoord& pt1, const ::csm::ImageCoord& pt2) const
//{
//}

::csm::RasterGM::SensorPartials SICDSensorModel::computeSensorPartials(
		int index, const ::csm::EcefCoord& groundPt, double desiredPrecision,
		double* achievedPrecision, ::csm::WarningList* warnings) const
{
	try
	{
		scene::Vector3 sceneGroundPt;
		sceneGroundPt[0] = groundPt.x;
		sceneGroundPt[1] = groundPt.y;
		sceneGroundPt[2] = groundPt.z;
		double timeCOA(0.0);
		const types::RowCol<double> imagePt =
				mProjection->sceneToImage(sceneGroundPt, &timeCOA);
		const types::RowCol<double> pixelPt = toPixel(imagePt);
		const ::csm::ImageCoord imagecoordPt(pixelPt.row, pixelPt.col);
		return computeSensorPartials(index, imagecoordPt, groundPt, desiredPrecision, achievedPrecision, warnings);
	}
	catch (const except::Exception& ex)
	{
		throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
				ex.getMessage(),
				"SICDSensorModel::computeSensorPartials");
	}

}

::csm::RasterGM::SensorPartials SICDSensorModel::computeSensorPartials(
		int index, const ::csm::ImageCoord& imagePt,
		const ::csm::EcefCoord& groundPt, double desiredPrecision,
		double* achievedPrecision, ::csm::WarningList* warnings) const
{
	if (index < 0 || index >= getNumParameters())
	{
		throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
						"Invalid index in call in function call",
						" SICDSensorModel::computeSensorPartials");
	}
	try
	{
		scene::Vector3 sceneGroundPt;
		sceneGroundPt[0] = groundPt.x;
		sceneGroundPt[1] = groundPt.y;
		sceneGroundPt[2] = groundPt.z;

		types::RowCol<double> pixelPt = fromPixel(imagePt);

		math::linear::MatrixMxN<2, 7, double> sensorPartials = mProjection->sceneToImageSensorPartials(sceneGroundPt,
				pixelPt);

		// TODO: Currently no way to determine the actual precision that was
		//       achieved, so setting it to the desired precision
		if (achievedPrecision)
		{
			*achievedPrecision = desiredPrecision;
		}
		// Return value is in pixels/sensor units
		return ::csm::RasterGM::SensorPartials(sensorPartials[0][index]/mData->grid->row->sampleSpacing,
				sensorPartials[1][index]/mData->grid->col->sampleSpacing);
	}
	catch (const except::Exception& ex)
	{
		throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
				ex.getMessage(),
				"SICDSensorModel::computeSensorPartials");
	}
}

::csm::ImageCoordCovar SICDSensorModel::groundToImage(
		const ::csm::EcefCoordCovar& groundPt, double desiredPrecision,
		double* achievedPrecision, ::csm::WarningList* warnings) const
{
	::csm::ImageCoord imagePt = SICDSensorModel::groundToImage(groundPt, desiredPrecision, achievedPrecision, warnings);
	scene::Vector3 scenePt;
	scenePt[0] = groundPt.x;
	scenePt[1] = groundPt.y;
	scenePt[2] = groundPt.z;
	// m^2
	math::linear::MatrixMxN<3,3,double> userCovar(groundPt.covariance);
	math::linear::MatrixMxN<7,7,double> sensorCovar = mProjection->getErrorCovariance(scenePt);
	math::linear::MatrixMxN<2,7,double> sensorPartials = mProjection->sceneToImageSensorPartials(scenePt);
	math::linear::MatrixMxN<2,3,double> imagePartials = mProjection->sceneToImagePartials(scenePt);
	math::linear::MatrixMxN<2,2,double> unmodeledCovar = mProjection->getUnmodeledErrorCovariance();
	math::linear::MatrixMxN<2,2,double> errorCovar;
	errorCovar = unmodeledCovar + (imagePartials * userCovar * imagePartials.transpose()) +
			(sensorPartials * sensorCovar * sensorPartials.transpose());
	::csm::ImageCoordCovar returnValue;
	returnValue.line = imagePt.line;
	returnValue.samp = imagePt.samp;
	returnValue.covariance[0] = errorCovar[0][0] / pow(mData->grid->row->sampleSpacing,2);
	returnValue.covariance[1] = errorCovar[0][1] / (mData->grid->row->sampleSpacing * mData->grid->col->sampleSpacing);
	returnValue.covariance[2] = errorCovar[1][0] / (mData->grid->row->sampleSpacing * mData->grid->col->sampleSpacing);
	returnValue.covariance[3] = errorCovar[1][1] / pow(mData->grid->col->sampleSpacing,2);
	return returnValue;
}

::csm::EcefCoordCovar SICDSensorModel::imageToGround(
		const ::csm::ImageCoordCovar& imagePt, double height,
		double heightVariance, double desiredPrecision,
		double* achievedPrecision, ::csm::WarningList* warnings) const
{
	// ignoring heightVariance. Any error in the height should be translated to ephemeris error
	::csm::EcefCoord groundPt = SICDSensorModel::imageToGround((::csm::ImageCoord) imagePt, height,
			desiredPrecision, achievedPrecision, warnings);
	types::RowCol<double> imagePtmeters = fromPixel(imagePt);
	scene::Vector3 scenePt;
	scenePt[0] = groundPt.x;
	scenePt[1] = groundPt.y;
	scenePt[2] = groundPt.z;
	math::linear::MatrixMxN<2,2,double> userCovar(imagePt.covariance);
	math::linear::MatrixMxN<7,7,double> sensorCovar = mProjection->getErrorCovariance(scenePt);
	math::linear::MatrixMxN<2,2,double> unmodeledCovar = mProjection->getUnmodeledErrorCovariance();
	math::linear::MatrixMxN<3,2,double> groundPartials = mProjection->imageToScenePartials(imagePtmeters, height);
	math::linear::MatrixMxN<3,7,double> sensorPartials = mProjection->imageToSceneSensorPartials(imagePtmeters,height);
	math::linear::MatrixMxN<3,3,double> errorCovar;
	errorCovar = (groundPartials * (userCovar + unmodeledCovar) * groundPartials.transpose()) +
				(sensorPartials * sensorCovar * sensorPartials.transpose());
	::csm::EcefCoordCovar returnValue;
	returnValue.x = groundPt.x;
	returnValue.y = groundPt.y;
	returnValue.z = groundPt.z;
	for (int i =0; i < 3; i++)
	{
		for (int j=0; j < 3; j++)
		{
			returnValue.covariance[j*3+i] = errorCovar[i][j];
		}
	}

	return returnValue;

}

std::vector< ::csm::RasterGM::SensorPartials > SICDSensorModel::computeAllSensorPartials(
		const ::csm::EcefCoord& groundPt, ::csm::param::Set pSet,
		double desiredPrecision, double* achievedPrecision,
		::csm::WarningList* warnings) const
{
	try
	{
		scene::Vector3 sceneGroundPt;
		sceneGroundPt[0] = groundPt.x;
		sceneGroundPt[1] = groundPt.y;
		sceneGroundPt[2] = groundPt.z;
		double timeCOA(0.0);
		const types::RowCol<double> imagePt =
				mProjection->sceneToImage(sceneGroundPt, &timeCOA);
		const types::RowCol<double> pixelPt = toPixel(imagePt);
		const ::csm::ImageCoord imagecoordPt(pixelPt.row, pixelPt.col);
		return computeAllSensorPartials(imagecoordPt, groundPt, pSet, desiredPrecision, achievedPrecision, warnings);
	}
	catch (const except::Exception& ex)
	{
		throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
				ex.getMessage(),
				"SICDSensorModel::computeSensorPartials");
	}
}

std::vector< ::csm::RasterGM::SensorPartials > SICDSensorModel::computeAllSensorPartials(
		const ::csm::ImageCoord& imagePt, const ::csm::EcefCoord& groundPt,
		::csm::param::Set pSet, double desiredPrecision,
		double* achievedPrecision, ::csm::WarningList* warnings) const
{
	try
	{
		scene::Vector3 sceneGroundPt;
		sceneGroundPt[0] = groundPt.x;
		sceneGroundPt[1] = groundPt.y;
		sceneGroundPt[2] = groundPt.z;

		types::RowCol<double> pixelPt = fromPixel(imagePt);
		math::linear::MatrixMxN<2, 7, double> sensorPartials = mProjection->sceneToImageSensorPartials(sceneGroundPt,
				pixelPt);

		// TODO: Currently no way to determine the actual precision that was
		//       achieved, so setting it to the desired precision
		if (achievedPrecision)
		{
			*achievedPrecision = desiredPrecision;
		}
		// Return value is in pixels/sensor units
		std::vector< ::csm::RasterGM::SensorPartials > returnValue(7, ::csm::RasterGM::SensorPartials(0,0) );
		for (int idx = 0; idx < 7; idx++)
		{
			returnValue[idx].first = sensorPartials[0][idx]/mData->grid->row->sampleSpacing;
			returnValue[idx].second = sensorPartials[0][idx]/mData->grid->col->sampleSpacing;
		}
		return returnValue;
	}
	catch (const except::Exception& ex)
	{
		throw ::csm::Error(::csm::Error::UNKNOWN_ERROR,
				ex.getMessage(),
				"SICDSensorModel::computeSensorPartials");
	}
}

std::vector<double> SICDSensorModel::getUnmodeledError(
		const ::csm::ImageCoord& imagePt) const
{
	// Not sure what the hell I am supposed to do with the input argument since our
	// unmodeled error is not a function of image location, so we're just going to ignore
	// the input argument and return the unmodeled error
	math::linear::MatrixMxN<2, 2, double> unmodeledError = mProjection->getUnmodeledErrorCovariance();
	std::vector<double> returnValue(4,0);
	returnValue[0] = unmodeledError[0][0] / pow(mData->grid->row->sampleSpacing, 2);
	returnValue[1] = unmodeledError[0][1] / (mData->grid->row->sampleSpacing * mData->grid->col->sampleSpacing);
	returnValue[2] = unmodeledError[1][0] / (mData->grid->row->sampleSpacing * mData->grid->col->sampleSpacing);
	returnValue[3] = unmodeledError[1][1] / pow(mData->grid->col->sampleSpacing, 2);
	return returnValue;
}

void SICDSensorModel::setSchemaDir(const std::string& dataDir)
{
    sys::OS os;
    if (dataDir.empty())
    {
        mSchemaDirs.clear();

        // OK, but you better have your schema path set then
        std::string schemaPath;
        try
        {
            schemaPath = os.getEnv(six::SCHEMA_PATH);
        }
        catch(const except::Exception& )
        {
            throw ::csm::Error(::csm::Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE,
                    "Must specify SICD schema path via "
                    "Plugin::getDataDirectory() or " +
                    std::string(six::SCHEMA_PATH) + " environment variable",
                    "SICDSensorModel::setSchemaDir");
        }

        if (schemaPath.empty())
        {
            throw ::csm::Error(::csm::Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE,
                    std::string(six::SCHEMA_PATH) +
                    " environment variable is set but is empty",
                    "SICDSensorModel::setSchemaDir");
        }
    }
    else
    {
        const std::string schemaDir =
                sys::Path(dataDir).join("schema").join("six");
        if (!os.exists(schemaDir))
        {
            throw ::csm::Error(::csm::Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE,
                    "Schema directory '" + schemaDir + "' does not exist",
                    "SICDSensorModel::setSchemaDir");
        }

        mSchemaDirs.resize(1);
        mSchemaDirs[0] = schemaDir;
    }
}

}
}


