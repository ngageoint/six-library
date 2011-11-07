/* =========================================================================
 * This file is part of the CSM SIDD Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2011, General Dynamics - Advanced Information Systems
 *
 * The CSM SIDD Plugin is free software; you can redistribute it and/or modify
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
#include "SIDDSensorModel.h"
#include "TSMWarning.h"
#include "TSMError.h"

using namespace sidd_sensor;

SIDDSensorModel::SIDDSensorModel(const std::string& filename,
                                 const std::string& sensorModel) :
    mSensorModelName(sensorModel)
{
    // create an XML registry
    // The reason to do this is to avoid adding XMLControlCreators to the
    // XMLControlFactory singleton - this way has more fine-grained control
    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::DERIVED, new six::XMLControlCreatorT<
            six::sidd::DerivedXMLControl>());

    // create a Reader registry (only NITF for now)
    six::ReadControlRegistry readerRegistry;
    readerRegistry.addCreator(new six::NITFReadControlCreator());

    // get the correct ReadControl for the given file
    std::auto_ptr < six::ReadControl
            > reader(readerRegistry.newReadControl(filename));
    reader->setXMLControlRegistry(&xmlRegistry);
    // load the file, passing in the optional registry, since we have one
    reader->load(filename);

    std::auto_ptr < six::Container > container(reader->getContainer());
    if (container->getDataType() != six::DataType::DERIVED
            || container->getNumData() != 1
            || container->getData(0)->getDataType() != six::DataType::DERIVED)
        throw except::Exception(Ctxt("Not a SIDD"));

    // get xml as string for sensor model state
    std::string xmlStr = six::toXMLString(container->getData(0), &xmlRegistry);
    mSensorModelState = mSensorModelName + " " + xmlStr;

    mData.reset((six::sidd::DerivedData*) container->getData(0));
    container->removeData(mData.get());
    mGrid.reset(six::sidd::Utilities::getGridGeometry(mData.get()));
}

SIDDSensorModel::SIDDSensorModel(NITF_2_1_ISD* isd,
                                 const std::string& sensorModel) :
    mSensorModelName(sensorModel)
{
    // Check for the first SIDD DES and parse it

    xml::lite::Document* siddXML = NULL;
    xml::lite::MinidomParser domParser;

    int numSIDD = 0;
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
                        == "SIDD")
                {
                    siddXML = domParser.getDocument();
                    numSIDD++;
                }
            }
            catch(except::Exception& ex)
            {
                // Couldn't parse DES as xml -- it's not a sidd so skip it
            }
        }
    }

    if (siddXML == NULL)
    {
        throw except::Exception(Ctxt("Not a SIDD"));
    }
    if (numSIDD > 1)
    {
        throw except::Exception(Ctxt("Too many SIDD image segments.  Only single segment SIDDs are supported."));
    }

    // get xml as string for sensor model state
    io::StringStream stringStream;
    siddXML->getRootElement()->print(stringStream);
    mSensorModelState = mSensorModelName + " " + stringStream.stream().str();

    six::XMLControlRegistry xmlRegistry;
    xmlRegistry.addCreator(six::DataType::DERIVED, new six::XMLControlCreatorT<
            six::sidd::DerivedXMLControl>());

    std::auto_ptr<six::XMLControl>
            control(xmlRegistry.newXMLControl(six::DataType::DERIVED));

    mData.reset((six::sidd::DerivedData*) control->fromXML(siddXML));
    mGrid.reset(six::sidd::Utilities::getGridGeometry(mData.get()));
}

SIDDSensorModel::SIDDSensorModel(const std::string& sensorModelState)
{
    std::string sensorModelXML = "";
    std::string name = "";
    std::string funcName = "SIDDSensorModel::SIDDSensorModel";

    int idx = sensorModelState.find(' ');
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
    xmlRegistry.addCreator(six::DataType::DERIVED, new six::XMLControlCreatorT<
            six::sidd::DerivedXMLControl>());

    std::auto_ptr<six::XMLControl>
            control(xmlRegistry.newXMLControl(six::DataType::DERIVED));

    // get xml as string for sensor model state
    mSensorModelState = sensorModelState;

    mData.reset((six::sidd::DerivedData*) control->fromXML(
            domParser.getDocument()));
    mGrid.reset(six::sidd::Utilities::getGridGeometry(mData.get()));
}

SIDDSensorModel::~SIDDSensorModel()
{
}

scene::RowCol<double> SIDDSensorModel::fromPixel(double l, double s)
{
    if (!mData->measurement->projection->isMeasurable())
    {
        throw except::Exception(Ctxt(
                "Image projection type is not measurable."));
    }

    six::sidd::MeasurableProjection* p =
            (six::sidd::MeasurableProjection*)mData->measurement->projection;

    scene::RowCol<int> ctrPt = p->referencePoint.rowCol;
    return scene::RowCol<double>(
            (l - ctrPt.row) * p->sampleSpacing.row,
                    (s - ctrPt.col) * p->sampleSpacing.col);
}

TSMWarning *SIDDSensorModel::groundToImage(const double &x, const double &y,
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

        // Project ground point into image grid and then
        // convert from ecef to RowCol
        scene::Vector3 gridPt = mGrid->sceneToGrid(groundPt);
	scene::RowCol<double> imagePt = mGrid->ecefToRowCol(gridPt);

	line = imagePt.row;
	sample = imagePt.col;

	// TODO not sure how to calculate achievedPrecision
	achievedPrecision = desiredPrecision;
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), "SIDDSensorModel::groundToImage");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::groundToImage(const double& x, const double& y,
                                           const double& z,
                                           const double groundCovariance[9],
                                           double& line, double& sample,
                                           double imageCovariance[4],
                                           double& achievedPrecision,
                                           const double& desiredPrecision)
                                                                           throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::groundToImage";

    // TODO need to compute the imageCovariance

    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::imageToGround(const double& line,
                                           const double& sample,
                                           const double& height, double& x,
                                           double& y, double& z,
                                           double& achievedPrecision,
                                           const double& desiredPrecision)
                                                                           throw (TSMError)
{
    try
    {
        // Convert line and sample to an ECEF point in the image grid and 
        // then project the grid point to the ground
	scene::Vector3 gridPt = mGrid->rowColToECEF(line, sample);
        scene::Vector3 groundPt = mGrid->gridToScene(gridPt, height);

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
                           e.getMessage(), "SIDDSensorModel::imageToGround");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::imageToGround(const double& line,
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
    std::string funcName = "SIDDSensorModel::imageToGround";

    // TODO need to compute the imageCovariance

    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::imageToProximateImagingLocus(
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
    std::string funcName("SIDDSensorModel::imageToProximateImagingLocus");
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::imageToRemoteImagingLocus(
                                                       const double& line,
                                                       const double& sample,
                                                       double locus[6],
                                                       double& achieved_precision,
                                                       const double& desired_precision)
                                                                                        throw (TSMError)
{
    std::string funcName("SIDDSensorModel::imageToRemoteImagingLocus");
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::computeGroundPartials(const double& x,
                                                   const double& y,
                                                   const double& z,
                                                   double partials[6])
                                                                       throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::computeGroundPartials";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::computeSensorPartials(
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
    std::string funcName = "SIDDSensorModel::computeSensorPartials";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::computeSensorPartials(
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
    std::string funcName = "SIDDSensorModel::computeSensorPartials";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning * SIDDSensorModel::getCurrentParameterCovariance(const int& index1,
                                                            const int& index2,
                                                            double &ret)
                                                                         throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::getCurrentParameterCovariance";
    ret = 0;

    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::setCurrentParameterCovariance(
                                                           const int& index1,
                                                           const int& index2,
                                                           const double& covariance)
                                                                                     throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::setCurrentParameterCovariance";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::setOriginalParameterCovariance(
                                                            const int& index1,
                                                            const int& index2,
                                                            const double& covariance)
                                                                                      throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::setOriginalParameterCovariance";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning * SIDDSensorModel::getOriginalParameterCovariance(const int& index1,
                                                             const int& index2,
                                                             double &ret)
                                                                          throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::getOriginalParameterCovariance";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getTrajectoryIdentifier(std::string& trajectoryId)
                                                                                throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::getTrajectoryIdentifier";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getReferenceDateAndTime(std::string& refDate)
                                                                           throw (TSMError)
{
    try
    {
        char buf[17];
        mData->exploitationFeatures->collections[0]->information->
                collectionDateTime.format("%Y%m%dT%H%M%.2SZ", buf, 17);
        refDate = std::string(buf);
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;         tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(),
                           "SIDDSensorModel::getReferenceDateAndTime");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::getImageTime(const double& line,
                                          const double& sample, double& time)
                                                                              throw (TSMError)
{
    try
    {
	if (!mData->measurement->projection->isMeasurable())
	{
	    throw except::Exception(Ctxt(
                    "Image projection type is not measurable."));
	}

        const scene::RowCol<double> imagePt = fromPixel(line, sample);
        time = ((six::sidd::MeasurableProjection*)mData->measurement->
                projection)->timeCOAPoly(imagePt.row, imagePt.col);
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(), "SIDDSensorModel::getImageTime");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::getSensorPosition(const double& line,
                                               const double& sample, double& x,
                                               double& y, double& z)
                                                                     throw (TSMError)
{
    try
    {
	if (!mData->measurement->projection->isMeasurable())
	{
	    throw except::Exception(Ctxt(
                    "Image projection type is not measurable."));
	}

        const scene::RowCol<double> imagePt = fromPixel(line, sample);
	double time = ((six::sidd::MeasurableProjection*)mData->measurement->
		projection)->timeCOAPoly(imagePt.row, imagePt.col);
	six::Vector3 pos = mData->measurement->arpPoly(time);
	x = pos[0];
	y = pos[1];
	z = pos[2];
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(),
                           "SIDDSensorModel::getSensorPosition");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::getSensorPosition(const double& time, double& x,
                                               double& y, double& z)
                                                                     throw (TSMError)
{
    try
    {
	six::Vector3 pos = mData->measurement->arpPoly(time);
	x = pos[0];
	y = pos[1];
	z = pos[2];
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(),
                           "SIDDSensorModel::getSensorPosition");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::getSensorVelocity(const double& line,
                                               const double& sample,
                                               double& vx, double& vy,
                                               double& vz) throw (TSMError)
{
    try
    {
	if (!mData->measurement->projection->isMeasurable())
	{
	    throw except::Exception(Ctxt(
                    "Image projection type is not measurable."));
	}

        const scene::RowCol<double> imagePt = fromPixel(line, sample);
	double time = ((six::sidd::MeasurableProjection*)mData->measurement->
		projection)->timeCOAPoly(imagePt.row, imagePt.col);
	six::PolyXYZ arpVelPoly = mData->measurement->arpPoly.derivative();
	six::Vector3 vel = arpVelPoly(time);
	vx = vel[0];
	vy = vel[1];
	vz = vel[2];
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(),
                           "SIDDSensorModel::getSensorVelocity");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::getSensorVelocity(const double& time, double& vx,
                                               double& vy, double& vz)
                                                                       throw (TSMError)
{
    try
    {
	six::PolyXYZ arpVelPoly = mData->measurement->arpPoly.derivative();
	six::Vector3 vel = arpVelPoly(time);
	vx = vel[0];
	vy = vel[1];
	vz = vel[2];
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(),
                           "SIDDSensorModel::getSensorVelocity");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::setCurrentParameterValue(const int& index,
                                                      const double& value)
                                                                           throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::setCurrentParameterValue";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning * SIDDSensorModel::getCurrentParameterValue(const int& index,
                                                       double &ret)
                                                                    throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::getCurrentParameterValue";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getParameterName(const int& index,
                                              std::string& name)
                                                                 throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::getParameterName";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning * SIDDSensorModel::getNumParameters(int &number) throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::getNumParameters";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::setOriginalParameterValue(const int& index,
                                                       const double& value)
                                                                            throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::setOriginalParameterValue";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getOriginalParameterValue(const int& index,
                                                       double &value)
                                                                      throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::getOriginalParameterValue";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getParameterType(
                                              const int& index,
                                              TSMMisc::Param_CharType &return_val)
                                                                                   throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::getParameterType";

    TSMMisc::Param_CharType value = TSMMisc::NONE;
    return_val = value;

    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::setParameterType(
                                              const int& index,
                                              const TSMMisc::Param_CharType &parameterType)
                                                                                            throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::setParameterType";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getPedigree(std::string& pedigree)
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
                           e.getMessage(), "SIDDSensorModel::getPedigree");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::getImageIdentifier(std::string& imageId)
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
                           "SIDDSensorModel::getImageIdentifier");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::setImageIdentifier(const std::string& imageId)
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
                           "SIDDSensorModel::setImageIdentifier");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::getSensorIdentifier(std::string& sensorId)
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
                           "SIDDSensorModel::getSensorIdentifier");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::getPlatformIdentifier(std::string& platformId)
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
                           "SIDDSensorModel::getPlatformIdentifier");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::getReferencePoint(double& x, double& y, double& z)
                                                                                throw (TSMError)
{
    try
    {
	six::Vector3 refPt = 
		mData->measurement->projection->referencePoint.ecef;
	x = refPt[0];
	y = refPt[1];
	z = refPt[2];
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(),
                           "SIDDSensorModel::getReferencePoint");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::setReferencePoint(const double& x,
                                               const double& y, const double& z)
                                                                                 throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::setReferencePoint";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getImageSize(int& height, int& width)
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
                           e.getMessage(), "SIDDSensorModel::getImageSize");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::getValidAltitudeRange(double& minHeight,
                                                   double& maxHeight)
                                                                      throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::validAltitudeRange";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getIlluminationDirection(const double& x,
                                                      const double& y,
                                                      const double& z,
                                                      double& direction_x,
                                                      double& direction_y,
                                                      double& direction_z)
                                                                           throw (TSMError)
{
    try
    {
	if (!mData->measurement->projection->isMeasurable())
	{
	    throw except::Exception(Ctxt(
                    "Image projection type is not measurable."));
	}

	scene::Vector3 groundPos;
	groundPos[0] = x;
	groundPos[1] = y;
	groundPos[2] = z;

	double time = ((six::sidd::MeasurableProjection*)mData->measurement->
		projection)->timeCOAPoly(0.0, 0.0);
	six::Vector3 arpPos = mData->measurement->arpPoly(time);

	scene::Vector3 illumVec = groundPos - arpPos;
	direction_x = illumVec[0];
	direction_y = illumVec[1];
	direction_z = illumVec[2];
    }
    catch(except::Exception& e)
    {
        TSMError tsmErr;
        tsmErr.setTSMError(TSMError::UNKNOWN_ERROR,
                           e.getMessage(),
                           "SIDDSensorModel::getIlluminationDirection");
        throw tsmErr;
    }

    return NULL;
}

TSMWarning *SIDDSensorModel::getNumSystematicErrorCorrections(int &number)
                                                                           throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::getNumSystematicErrorCorrections";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getSystematicErrorCorrectionName(const int& index,
                                                              std::string& name)
                                                                                 throw (TSMError)
{
    std::string funcName("SIDDSensorModel::getSystematicErrorCorrectionName");
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::setCurrentSystematicErrorCorrectionSwitch(
                                                                       const int& index,
                                                                       const bool& value,
                                                                       const TSMMisc::Param_CharType& parameterType)
                                                                                                                     throw (TSMError)
{
    std::string funcName =
            "SIDDSensorModel::setCurrentSystematicErrorCorrectionSwitch";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning * SIDDSensorModel::getCurrentSystematicErrorCorrectionSwitch(
                                                                        const int& index,
                                                                        bool& ret)
                                                                                   throw (TSMError)
{
    std::string funcName =
            "SIDDSensorModel::getCurrentSystematicErrorCorrectionSwitch";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getSensorModelName(std::string& name)
                                                                   throw (TSMError)
{
    name = mSensorModelName;
    return NULL;
}

TSMWarning *SIDDSensorModel::getSensorModelState(std::string &state)
                                                                     throw (TSMError)
{
    state = mSensorModelState;
    return NULL;
}

TSMWarning *SIDDSensorModel::getCovarianceModel(tsm_CovarianceModel*& covModel)
                                                                                throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::getCovarianceModel";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getUnmodeledError(const double line,
                                               const double sample,
                                               double covariance[4])
                                                                     throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::getUnmodeledError";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getUnmodeledCrossCovariance(
                                                         const double pt1Line,
                                                         const double pt1Sample,
                                                         const double pt2Line,
                                                         const double pt2Sample,
                                                         double crossCovariance[4])
                                                                                    throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::getUnmodeledCrossCovariance";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning *SIDDSensorModel::getCollectionIdentifier(std::string& collectionId)
                                                                                throw (TSMError)
{
//TODO see what this should be for SIDD
    return NULL;
}

TSMWarning * SIDDSensorModel::isParameterShareable(const int& index,
                                                   bool& shareableSwitch)
                                                                          throw (TSMError)
{
    std::string funcName = "SIDDSensorModel::isParameterShareable";
    // TODO not implemented
    TSMError tsmErr;
    tsmErr.setTSMError(TSMError::UNSUPPORTED_FUNCTION,
                       "Function not supported", funcName);
    throw tsmErr;
}

TSMWarning * SIDDSensorModel::getParameterSharingCriteria(
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
    std::string funcName = "SIDDSensorModel::getParameterSharingCriteria";

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
