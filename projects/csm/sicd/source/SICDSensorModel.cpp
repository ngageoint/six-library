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
#include <iostream>
#include <string>
#include <vector>
#include "SICDSensorModel.h"
#include "TSMWarning.h"
#include "TSMError.h"

using namespace sicd_sensor;

SICDSensorModel::SICDSensorModel(const std::string& filename,
                                 const std::string& sensorModel) :
    mSensorModelName(sensorModel)
{
    // create an XML registry
    // The reason to do this is to avoid adding XMLControlCreators to the
    // XMLControlFactory singleton - this way has more fine-grained control
    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX, new six::XMLControlCreatorT<
            six::sicd::ComplexXMLControl>());

    // create a Reader registry (only NITF for now)
    six::ReadControlRegistry readerRegistry;
    readerRegistry.addCreator(new six::NITFReadControlCreator());

    // get the correct ReadControl for the given file
    std::auto_ptr < six::ReadControl
            > reader(readerRegistry.newReadControl(filename));
    reader->setXMLControlRegistry(&xmlRegistry);
    // load the file, passing in the optional registry, since we have one
    reader->load(filename);

    six::Container* container = reader->getContainer();
    if (container->getDataType() != six::DataType::COMPLEX
            || container->getNumData() != 1
            || container->getData(0)->getDataType() != six::DataType::COMPLEX)
        throw except::Exception(Ctxt("Not a SICD"));

    // get xml as string for sensor model state
    std::string xmlStr = six::toXMLString(container->getData(0), &xmlRegistry);
    mSensorModelState = mSensorModelName + " " + xmlStr;

    mData.reset((six::sicd::ComplexData*) container->getData(0));
    container->removeData(mData.get());
    mGeometry.reset(six::sicd::Utilities::getSceneGeometry(mData.get()));
    mProjection.reset(six::sicd::Utilities::getProjectionModel(mData.get(), 
            mGeometry.get()));
}

SICDSensorModel::SICDSensorModel(NITF_2_1_ISD* isd,
                                 const std::string& sensorModel) :
    mSensorModelName(sensorModel)
{
    // Check for the first SICD DES and parse it

    xml::lite::Document* sicdXML = NULL;
    xml::lite::MinidomParser domParser;

    for (int i=0; i < isd->numDESs; i++)
    {
        if (isd->fileDESs && isd->fileDESs[i].desData)
        {
            try
            {
                io::ByteStream stream;
                stream.write(isd->fileDESs[i].desData, 
                             isd->fileDESs[i].desDataLength);

                domParser.clear();
                domParser.parse(stream);

                if (domParser.getDocument()->getRootElement()->getLocalName() 
                        == "SICD")
                {
                    sicdXML = domParser.getDocument();
                    break;
                }
            }
            catch(except::Exception& ex)
            {
                // Couldn't parse DES as xml -- it's not a sicd so skip it
            }
        }
    }

    if (sicdXML == NULL)
    {
        throw except::Exception(Ctxt("Not a SICD"));
    }

    // get xml as string for sensor model state
    io::StringStream stringStream;
    sicdXML->getRootElement()->print(stringStream);
    mSensorModelState = mSensorModelName + " " + stringStream.stream().str();

    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX, new six::XMLControlCreatorT<
            six::sicd::ComplexXMLControl>());

    std::auto_ptr<logging::Logger> log (new logging::NullLogger());
    std::auto_ptr<six::XMLControl>
            control(xmlRegistry.newXMLControl(
                    six::DataType::COMPLEX, log.get()));

    mData.reset((six::sicd::ComplexData*) control->fromXML(
            sicdXML, std::vector<std::string>()));
    mGeometry.reset(six::sicd::Utilities::getSceneGeometry(mData.get()));
    mProjection.reset(six::sicd::Utilities::getProjectionModel(mData.get(), 
            mGeometry.get()));
}

SICDSensorModel::SICDSensorModel(const std::string& sensorModelState)
{
    std::string sensorModelXML = "";
    std::string name = "";
    std::string funcName = "SICDSensorModel::SICDSensorModel";

    const size_t idx = sensorModelState.find(' ');
    if (idx == std::string::npos)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                   "Invalid sensor model state", funcName);
        throw tsmErr;
    }

    name = sensorModelState.substr(0, idx);
    sensorModelXML = sensorModelState.substr(idx + 1);

    mSensorModelName = name;

    io::ByteStream stream;
    stream.write(sensorModelXML.c_str(), sensorModelXML.length());

    xml::lite::MinidomParser domParser;
    domParser.parse(stream);

    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::COMPLEX, new six::XMLControlCreatorT<
            six::sicd::ComplexXMLControl>());

    std::auto_ptr<logging::Logger> log (new logging::NullLogger());
    std::auto_ptr<six::XMLControl> 
            control(xmlRegistry.newXMLControl(
                    six::DataType::COMPLEX, log.get()));

    // get xml as string for sensor model state
    mSensorModelState = sensorModelState;

    mData.reset((six::sicd::ComplexData*) control->fromXML(
            domParser.getDocument(), std::vector<std::string>()));

    mGeometry.reset(six::sicd::Utilities::getSceneGeometry(mData.get()));
    mProjection.reset(six::sicd::Utilities::getProjectionModel(mData.get(), 
            mGeometry.get()));
}

SICDSensorModel::~SICDSensorModel()
{
}

types::RowCol<double> SICDSensorModel::toPixel(double l, double s)
{
    types::RowCol<int> ctrPt = mData->imageData->scpPixel;
    return types::RowCol<double>(
            (l / mData->grid->row->sampleSpacing) + ctrPt.row,
                    (s / mData->grid->col->sampleSpacing) + ctrPt.col);
}

types::RowCol<double> SICDSensorModel::fromPixel(double l, double s)
{
    types::RowCol<int> ctrPt = mData->imageData->scpPixel;
    return types::RowCol<double>(
            (l - ctrPt.row) * mData->grid->row->sampleSpacing,
                    (s - ctrPt.col) * mData->grid->col->sampleSpacing);
}

TSMWarning *SICDSensorModel::groundToImage(const double &x, const double &y,
                                           const double &z, double& line,
                                           double& sample,
                                           double& achievedPrecision,
                                           const double &desiredPrecision)
                                                                           throw (TSMError)
{
    try
    {
	scene::Vector3 groundPt;
	groundPt[0] = x;
	groundPt[1] = y;
	groundPt[2] = z;

	double timeCOA(0.0);
	types::RowCol<double> imagePt = mProjection->sceneToImage(groundPt,
								  &timeCOA);
	types::RowCol<double> pixelPt = toPixel(imagePt.row, imagePt.col);
	line = pixelPt.row;
	sample = pixelPt.col;

	// TODO not sure how to calculate achievedPrecision
	achievedPrecision = desiredPrecision;
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), "SICDSensorModel::groundToImage");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::groundToImage(const double& x, const double& y,
                                           const double& z,
                                           const double groundCovariance[9],
                                           double& line, double& sample,
                                           double imageCovariance[4],
                                           double& achievedPrecision,
                                           const double& desiredPrecision)
                                                                           throw (TSMError)
{
    std::string funcName = "SICDSensorModel::groundToImage";

    // TODO need to compute the imageCovariance

    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::imageToGround(const double& line,
                                           const double& sample,
                                           const double& height, double& x,
                                           double& y, double& z,
                                           double& achievedPrecision,
                                           const double& desiredPrecision)
                                                                           throw (TSMError)
{
    try
    {
	// TODO handle the case where the height is non-zero
	const types::RowCol<double> imagePt = fromPixel(line, sample);
	double timeCOA(0.0);

	scene::Vector3 groundRefPoint = mGeometry->getReferencePosition();
	scene::Vector3 groundPlaneNormal(groundRefPoint);
	groundPlaneNormal.normalize();

	scene::Vector3 groundPt = mProjection->imageToScene(imagePt,
							    groundRefPoint,
							    groundPlaneNormal,
							    &timeCOA);

	x = groundPt[0];
	y = groundPt[1];
	z = groundPt[2];

	// TODO not sure how to calculate achievedPrecision
	achievedPrecision = desiredPrecision;
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), "SICDSensorModel::imageToGround");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::imageToGround(const double& line,
                                           const double& sample,
                                           const double imageCovariance[4],
                                           const double& height,
                                           const double& heightVariance,
                                           double& x, double& y, double& z,
                                           double groundCovariance[9],
                                           double& achieved_precision,
                                           const double& desired_precision)
                                                                            throw (TSMError)
{
    std::string funcName = "SICDSensorModel::imageToGround";

    // TODO need to compute the imageCovariance

    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::imageToProximateImagingLocus(
                                                          const double& line,
                                                          const double& sample,
                                                          const double& x,
                                                          const double& y,
                                                          const double& z,
                                                          double locus[6],
                                                          double& achieved_precision,
                                                          const double& desired_precision)
                                                                                           throw (TSMError)
{
    std::string funcName("SICDSensorModel::imageToProximateImagingLocus");
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::imageToRemoteImagingLocus(
                                                       const double& line,
                                                       const double& sample,
                                                       double locus[6],
                                                       double& achieved_precision,
                                                       const double& desired_precision)
                                                                                        throw (TSMError)
{
    std::string funcName("SICDSensorModel::imageToRemoteImagingLocus");
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::computeGroundPartials(const double& x,
                                                   const double& y,
                                                   const double& z,
                                                   double partials[6])
                                                                       throw (TSMError)
{
    std::string funcName = "SICDSensorModel::computeGroundPartials";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::computeSensorPartials(
                                                   const int& index,
                                                   const double& x,
                                                   const double& y,
                                                   const double& z,
                                                   double& line_partial,
                                                   double& sample_partial,
                                                   double& achieved_precision,
                                                   const double& desired_precision)
                                                                                    throw (TSMError)
{
    std::string funcName = "SICDSensorModel::computeSensorPartials";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::computeSensorPartials(
                                                   const int& index,
                                                   const double& line,
                                                   const double& sample,
                                                   const double& x,
                                                   const double& y,
                                                   const double& z,
                                                   double& line_partial,
                                                   double& sample_partial,
                                                   double& achieved_precision,
                                                   const double& desired_precision)
                                                                                    throw (TSMError)
{
    std::string funcName = "SICDSensorModel::computeSensorPartials";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning * SICDSensorModel::getCurrentParameterCovariance(const int& index1,
                                                            const int& index2,
                                                            double &ret)
                                                                         throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getCurrentParameterCovariance";
    ret = 0;

    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::setCurrentParameterCovariance(
                                                           const int& index1,
                                                           const int& index2,
                                                           const double& covariance)
                                                                                     throw (TSMError)
{
    std::string funcName = "SICDSensorModel::setCurrentParameterCovariance";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::setOriginalParameterCovariance(
                                                            const int& index1,
                                                            const int& index2,
                                                            const double& covariance)
                                                                                      throw (TSMError)
{
    std::string funcName = "SICDSensorModel::setOriginalParameterCovariance";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning * SICDSensorModel::getOriginalParameterCovariance(const int& index1,
                                                             const int& index2,
                                                             double &ret)
                                                                          throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getOriginalParameterCovariance";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getTrajectoryIdentifier(std::string& trajectoryId)
                                                                                throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getTrajectoryIdentifier";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getReferenceDateAndTime(std::string& refDate)
                                                                           throw (TSMError)
{
    try
    {
        char buf[17];
        mData->timeline->collectStart.format("%Y%m%dT%H%M%.2SZ", buf, 17);
        refDate = std::string(buf);
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), 
                           "SICDSensorModel::getReferenceDateAndTime");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::getImageTime(const double& line,
                                          const double& sample, double& time)
                                                                              throw (TSMError)
{
    try
    {
        types::RowCol<double> imagePt = fromPixel(line, sample);
        time = mProjection->computeImageTime(imagePt);
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), "SICDSensorModel::getImageTime");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::getSensorPosition(const double& line,
                                               const double& sample, double& x,
                                               double& y, double& z)
                                                                     throw (TSMError)
{
    try
    {
        types::RowCol<double> imagePt = fromPixel(line, sample);
        double time = mProjection->computeImageTime(imagePt);
        six::Vector3 pos = mProjection->computeARPPosition(time); 
        x = pos[0];
        y = pos[1];
        z = pos[2];
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), 
                           "SICDSensorModel::getSensorPosition");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::getSensorPosition(const double& time, double& x,
                                               double& y, double& z)
                                                                     throw (TSMError)
{
    try
    {
        six::Vector3 pos = mProjection->computeARPPosition(time); 
        x = pos[0];
        y = pos[1];
        z = pos[2];
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), 
                           "SICDSensorModel::getSensorPosition");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::getSensorVelocity(const double& line,
                                               const double& sample,
                                               double& vx, double& vy,
                                               double& vz) throw (TSMError)
{
    try
    {
        types::RowCol<double> imagePt = fromPixel(line, sample);
        double time = mProjection->computeImageTime(imagePt);
        six::Vector3 vel = mProjection->computeARPVelocity(time); 
        vx = vel[0];
        vy = vel[1];
        vz = vel[2];
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), 
                           "SICDSensorModel::getSensorVelocity");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::getSensorVelocity(const double& time, double& vx,
                                               double& vy, double& vz)
                                                                       throw (TSMError)
{
    try
    {
        six::Vector3 vel = mProjection->computeARPVelocity(time); 
        vx = vel[0];
        vy = vel[1];
        vz = vel[2];
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), 
                           "SICDSensorModel::getSensorVelocity");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::setCurrentParameterValue(const int& index,
                                                      const double& value)
                                                                           throw (TSMError)
{
    std::string funcName = "SICDSensorModel::setCurrentParameterValue";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning * SICDSensorModel::getCurrentParameterValue(const int& index,
                                                       double &ret)
                                                                    throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getCurrentParameterValue";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getParameterName(const int& index,
                                              std::string& name)
                                                                 throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getParameterName";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning * SICDSensorModel::getNumParameters(int &number) throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getNumParameters";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::setOriginalParameterValue(const int& index,
                                                       const double& value)
                                                                            throw (TSMError)
{
    std::string funcName = "SICDSensorModel::setOriginalParameterValue";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getOriginalParameterValue(const int& index,
                                                       double &value)
                                                                      throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getOriginalParameterValue";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getParameterType(
                                              const int& index,
                                              TSMMisc::Param_CharType &return_val)
                                                                                   throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getParameterType";

    TSMMisc::Param_CharType value = TSMMisc::NONE;
    return_val = value;

    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::setParameterType(
                                              const int& index,
                                              const TSMMisc::Param_CharType &parameterType)
                                                                                            throw (TSMError)
{
    std::string funcName = "SICDSensorModel::setParameterType";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getPedigree(std::string& pedigree)
                                                                throw (TSMError)
{
    try
    {
        pedigree = mData->getSource() + "_" + mSensorModelName;
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), "SICDSensorModel::getPedigree");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::getImageIdentifier(std::string& imageId)
                                                                      throw (TSMError)
{
    try
    {
        imageId = mData->getName();
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), 
                           "SICDSensorModel::getImageIdentifier");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::setImageIdentifier(const std::string& imageId)
                                                                            throw (TSMError)
{
    try
    {
        mData->setName(imageId);
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), 
                           "SICDSensorModel::setImageIdentifier");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::getSensorIdentifier(std::string& sensorId)
                                                                        throw (TSMError)
{
    try
    {
        sensorId = mData->getSource();
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), 
                           "SICDSensorModel::getSensorIdentifier");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::getPlatformIdentifier(std::string& platformId)
                                                                            throw (TSMError)
{
    try
    {
        platformId = mData->getSource();
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), 
                           "SICDSensorModel::getPlatformIdentifier");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::getReferencePoint(double& x, double& y, double& z)
                                                                                throw (TSMError)
{
    try
    {
        scene::Vector3 refPt = mGeometry->getReferencePosition();
        x = refPt[0];
        y = refPt[1];
        z = refPt[2];
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), 
                           "SICDSensorModel::getReferencePoint");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::setReferencePoint(const double& x,
                                               const double& y, const double& z)
                                                                                 throw (TSMError)
{
    std::string funcName = "SICDSensorModel::setReferencePoint";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getImageSize(int& height, int& width)
                                                                   throw (TSMError)
{
    try
    {
        height = mData->getNumRows();
        width = mData->getNumCols();
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), "SICDSensorModel::getImageSize");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::getValidAltitudeRange(double& minHeight,
                                                   double& maxHeight)
                                                                      throw (TSMError)
{
    std::string funcName = "SICDSensorModel::validAltitudeRange";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getIlluminationDirection(const double& x,
                                                      const double& y,
                                                      const double& z,
                                                      double& direction_x,
                                                      double& direction_y,
                                                      double& direction_z)
                                                                           throw (TSMError)
{
    try
    {
        scene::Vector3 groundPos;
        groundPos[0] = x;
        groundPos[1] = y;
        groundPos[2] = z;

        scene::Vector3 illumVec = groundPos - mGeometry->getARPPosition();
        illumVec.normalize();

        direction_x = illumVec[0];
        direction_y = illumVec[1];
        direction_z = illumVec[2];
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), 
                           "SICDSensorModel::getIlluminationDirection");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SICDSensorModel::getNumSystematicErrorCorrections(int &number)
                                                                           throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getNumSystematicErrorCorrections";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getSystematicErrorCorrectionName(const int& index,
                                                              std::string& name)
                                                                                 throw (TSMError)
{
    std::string funcName("SICDSensorModel::getSystematicErrorCorrectionName");
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::setCurrentSystematicErrorCorrectionSwitch(
                                                                       const int& index,
                                                                       const bool& value,
                                                                       const TSMMisc::Param_CharType& parameterType)
                                                                                                                     throw (TSMError)
{
    std::string funcName =
            "SICDSensorModel::setCurrentSystematicErrorCorrectionSwitch";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning * SICDSensorModel::getCurrentSystematicErrorCorrectionSwitch(
                                                                        const int& index,
                                                                        bool& ret)
                                                                                   throw (TSMError)
{
    std::string funcName =
            "SICDSensorModel::getCurrentSystematicErrorCorrectionSwitch";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getSensorModelName(std::string& name)
                                                                   throw (TSMError)
{
    name = mSensorModelName;
    return NULL;
}

TSMWarning *SICDSensorModel::getSensorModelState(std::string &state)
                                                                     throw (TSMError)
{
    state = mSensorModelState;
    return NULL;
}

TSMWarning *SICDSensorModel::getCovarianceModel(tsm_CovarianceModel*& covModel)
                                                                                throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getCovarianceModel";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getUnmodeledError(const double line,
                                               const double sample,
                                               double covariance[4])
                                                                     throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getUnmodeledError";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getUnmodeledCrossCovariance(
                                                         const double pt1Line,
                                                         const double pt1Sample,
                                                         const double pt2Line,
                                                         const double pt2Sample,
                                                         double crossCovariance[4])
                                                                                    throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getUnmodeledCrossCovariance";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SICDSensorModel::getCollectionIdentifier(std::string& collectionId)
                                                                                throw (TSMError)
{
    // same as imageIdentifier right now...
    collectionId = mData->collectionInformation->coreName;
    return NULL;
}

TSMWarning * SICDSensorModel::isParameterShareable(const int& index,
                                                   bool& shareableSwitch)
                                                                          throw (TSMError)
{
    std::string funcName = "SICDSensorModel::isParameterShareable";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning * SICDSensorModel::getParameterSharingCriteria(
                                                          const int& index,
                                                          bool& requireModelNameMatch,
                                                          bool& requireSensorIDMatch,
                                                          bool& requirePlatformIDMatch,
                                                          bool& requireCollectionIDMatch,
                                                          bool& requireTrajectoryIDMatch,
                                                          bool& requireDateTimeMatch,
                                                          double& allowableTimeDelta)
                                                                                      throw (TSMError)
{
    std::string funcName = "SICDSensorModel::getParameterSharingCriteria";

    requireModelNameMatch = false;
    requireSensorIDMatch = false;
    requirePlatformIDMatch = false;
    requireCollectionIDMatch = false;
    requireTrajectoryIDMatch = false;
    requireDateTimeMatch = false;

    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}
