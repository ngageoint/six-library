/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include <cphd03/CPHDXMLControl.h>

#include <string>
#include <std/memory>

#include <io/StringStream.h>
#include <logging/NullLogger.h>
#include <str/EncodedStringView.h>
#include <six/Utilities.h>
#include <six/XmlLite.h>

// CPHD Spec is not enforced
#define ENFORCESPEC 0

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace cphd03
{
const char CPHDXMLControl::CPHD03_URI[] = "urn:CPHD:0.3";

CPHDXMLControl::CPHDXMLControl() :
    six::XMLParser(CPHD03_URI, false, new logging::NullLogger(), true),
    mCommon(CPHD03_URI, false, CPHD03_URI, log())
{
}

CPHDXMLControl::CPHDXMLControl(logging::Logger* log, bool ownLog) :
    six::XMLParser(CPHD03_URI, false, log, ownLog),
    mCommon(CPHD03_URI, false, CPHD03_URI, log)
{
}

std::string CPHDXMLControl::getDefaultURI() const
{
    return CPHD03_URI;
}

std::string CPHDXMLControl::getSICommonURI() const
{
    return CPHD03_URI;
}

std::u8string CPHDXMLControl::toXMLString(const Metadata& metadata)
{
    std::unique_ptr<xml::lite::Document> doc(toXML( metadata));
    io::U8StringStream ss;
    doc->getRootElement()->print(ss);

    return str::EncodedStringView("<?xml version=\"1.0\"?>").u8string() + ss.stream().str();
}
std::string CPHDXMLControl::toXMLString_(const Metadata& metadata)
{
    return str::EncodedStringView(toXMLString(metadata)).native();
}

size_t CPHDXMLControl::getXMLsize(const Metadata& metadata)
{
    return toXMLString(metadata).size();
}

mem::auto_ptr<xml::lite::Document> CPHDXMLControl::toXML(const Metadata& metadata)
{
    auto doc = std::make_unique<xml::lite::Document>();

    XMLElem root = newElement("CPHD");
    doc->setRootElement(root);

    // Fill in the rest...
    mCommon.convertCollectionInformationToXML(&metadata.collectionInformation,
            root);
    toXML(metadata.data, root);
    toXML(metadata.global, root);
    toXML(metadata.channel, root);
    toXML(metadata.srp, root);
    if (metadata.antenna.get())
    {
        toXML(*metadata.antenna, root);
    }
    toXML(metadata.vectorParameters, root);

    //set the XMLNS
    root->setNamespacePrefix("", getDefaultURI());

    return mem::auto_ptr<xml::lite::Document>(doc.release());
}

XMLElem CPHDXMLControl::createLatLonAltFootprint(const std::string& name,
                                                 const std::string& cornerName,
                                                 const cphd::LatLonAltCorners& corners,
                                                 XMLElem parent) const
{
    XMLElem footprint = newElement(name, parent);

    // Write the corners in CW order
    XMLElem vertex =
        mCommon.createLatLonAlt(cornerName, corners.upperLeft, footprint);
    setAttribute(vertex, "index", "1");

    vertex = mCommon.createLatLonAlt(cornerName, corners.upperRight, footprint);
    setAttribute(vertex, "index", "2");

    vertex = mCommon.createLatLonAlt(cornerName, corners.lowerRight, footprint);
    setAttribute(vertex, "index", "3");

    vertex = mCommon.createLatLonAlt(cornerName, corners.lowerLeft, footprint);
    setAttribute(vertex, "index", "4");

    return footprint;
}

XMLElem CPHDXMLControl::toXML(const Data& data, XMLElem parent)
{
    XMLElem dataXML = newElement("Data", parent);

    createString("SampleType", data.sampleType, dataXML);

    createInt("NumCPHDChannels", data.numCPHDChannels, dataXML);
    createInt("NumBytesVBP", data.numBytesVBP, dataXML);
    for (size_t ii = 0; ii < data.numCPHDChannels; ++ii)
    {
        XMLElem arrsizeXML = newElement("ArraySize", dataXML);
        createInt("NumVectors", data.arraySize.at(ii).numVectors, arrsizeXML);
        createInt("NumSamples", data.arraySize.at(ii).numSamples, arrsizeXML);
        setAttribute(arrsizeXML, "index", ii + 1);
    }

    return dataXML;
}

XMLElem CPHDXMLControl::toXML(const Global& global, XMLElem parent)
{
    XMLElem globalXML = newElement("Global", parent);

    createString("DomainType", global.domainType, globalXML);
    createString("PhaseSGN", global.phaseSGN, globalXML);

    if (!six::Init::isUndefined(global.refFrequencyIndex))
    {
        createInt("RefFreqIndex", global.refFrequencyIndex, globalXML);
    }

    createDateTime("CollectStart", global.collectStart, globalXML);
    createDouble("CollectDuration", global.collectDuration, globalXML);
    createDouble("TxTime1", global.txTime1, globalXML);
    createDouble("TxTime2", global.txTime2, globalXML);

    // ImageArea is required (SICD RadarCollection:Area is optional)
    XMLElem areaXML = newElement("ImageArea", globalXML);

    const ImageArea& area = global.imageArea;

    XMLElem cornersXML =
            createLatLonAltFootprint("Corners", "ACP", area.acpCorners, areaXML);

    // The "size" attribute isn't used in cphd03
    cornersXML->getAttributes().remove("size");

    // "Plane" is optional
    if (area.plane.get())
    {
        const AreaPlane& plane = *area.plane;

        XMLElem planeXML = newElement("Plane", areaXML);
        XMLElem refPtXML = newElement("RefPt", planeXML);

        six::ReferencePoint refPt = plane.referencePoint;
        if (!refPt.name.empty())
        {
            setAttribute(refPtXML, "name", refPt.name);
        }

        mCommon.createVector3D("ECF", refPt.ecef, refPtXML);
        createDouble("Line",   refPt.rowCol.row, refPtXML);
        createDouble("Sample", refPt.rowCol.col, refPtXML);

        areaLineDirectionParametersToXML  ("XDir", plane.xDirection, planeXML);
        areaSampleDirectionParametersToXML("YDir", plane.yDirection, planeXML);

        // Within the optional Plane, DwellTime is itself optional
        if (plane.dwellTime.get())
        {
            XMLElem dwellTimeXML = newElement("DwellTime", planeXML);
            mCommon.createPoly2D("CODTimePoly",
                                 plane.dwellTime->codTimePoly,
                                 dwellTimeXML);
            mCommon.createPoly2D("DwellTimePoly",
                                 plane.dwellTime->dwellTimePoly,
                                 dwellTimeXML);
        }
    }

    return globalXML;
}

static void set_index_attribute(xml::lite::Element& elem, size_t value)
{
    elem.attribute("index") = std::to_string(value);
}

XMLElem CPHDXMLControl::toXML(const Channel& channel, XMLElem parent)
{
    XMLElem channelXML = newElement("Channel", parent);

    // There is a Parameters entry for each channel

    for (size_t ii = 0; ii < channel.parameters.size(); ++ii)
    {
        XMLElem chanParamsXML = newElement("Parameters", channelXML);
        set_index_attribute(*chanParamsXML, ii + 1);

        ChannelParameters cp = channel.parameters[ii];
        createInt("SRP_Index", cp.srpIndex, chanParamsXML);
        createDouble("NomTOARateSF", cp.nomTOARateSF, chanParamsXML);
        createDouble("FxCtrNom", cp.fxCtrNom, chanParamsXML);
        createDouble("BWSavedNom", cp.bwSavedNom, chanParamsXML);
        createDouble("TOASavedNom", cp.toaSavedNom, chanParamsXML);

        // optional
        if (!six::Init::isUndefined(cp.txAntIndex))
        {
            createInt("TxAnt_Index", cp.txAntIndex, chanParamsXML);
        }

        if (!six::Init::isUndefined(cp.rcvAntIndex))
        {
            createInt("RcvAnt_Index", cp.rcvAntIndex, chanParamsXML);
        }

        if (!six::Init::isUndefined(cp.twAntIndex))
        {
            createInt("TWAnt_Index", cp.twAntIndex, chanParamsXML);
        }
    }
    return channelXML;
}

XMLElem CPHDXMLControl::toXML(const SRP& srp, XMLElem parent)
{
    XMLElem srpXML = newElement("SRP", parent);

    createString("SRPType", srp.srpType, srpXML);
    createInt("NumSRPs", srp.numSRPs, srpXML);

    switch ((int)srp.srpType)
    {
    case cphd::SRPType::FIXEDPT:
        if (srp.srpPT.size() != srp.numSRPs)
        {
            throw except::Exception(Ctxt(
                    "SRP: number of FIXEDPT entries must match NumSRPs"));
        }
        for (size_t ii = 0; ii < srp.srpPT.size(); ++ii)
        {
            XMLElem fixedptXML = newElement("FIXEDPT", srpXML);
            set_index_attribute(*fixedptXML, ii + 1);
            mCommon.createVector3D("SRPPT", srp.srpPT[ii], fixedptXML);
        }

        break;

    case cphd::SRPType::PVTPOLY:
        if (srp.srpPVTPoly.size() != srp.numSRPs)
        {
            throw except::Exception(Ctxt(
                    "SRP: number of PVTPOLY entries must match NumSRPs"));
        }
        for (size_t ii = 0; ii < srp.srpPVTPoly.size(); ++ii)
        {
            XMLElem pvtpolyXML = newElement("PVTPOLY", srpXML);
            set_index_attribute(*pvtpolyXML, ii + 1);
            mCommon.createPolyXYZ("SRPPVTPoly", srp.srpPVTPoly[ii], pvtpolyXML);
        }
        break;

    case cphd::SRPType::PVVPOLY:
        if (srp.srpPVVPoly.size() != srp.numSRPs)
        {
            throw except::Exception(Ctxt(
                    "SRP: number of PVVPOLY entries must match NumSRPs"));
        }
        for (size_t ii = 0; ii < srp.srpPVVPoly.size(); ++ii)
        {
            XMLElem pvvpolyXML = newElement("PVVPOLY", srpXML);
            set_index_attribute(*pvvpolyXML, ii + 1);
            mCommon.createPolyXYZ("SRPPVVPoly", srp.srpPVVPoly[ii], pvvpolyXML);
        }
        break;

    case cphd::SRPType::STEPPED:
        // No SRP type for this
        if (srp.numSRPs != 0)
        {
            throw except::Exception(Ctxt(
                    "SRP: SRPType of STEPPED must have NumSRPs equal zero"));
        }
        break;

    default:
        throw except::Exception(Ctxt(
                "Invalid SRPType: " + srp.srpType.toString()));
        break;
    }

    return srpXML;
}

XMLElem CPHDXMLControl::toXML(const Antenna& antenna, XMLElem parent)
{
    XMLElem antennaXML = newElement("Antenna", parent);

    createInt("NumTxAnt",  antenna.numTxAnt,  antennaXML);
    createInt("NumRcvAnt", antenna.numRcvAnt, antennaXML);
    createInt("NumTWAnt",  antenna.numTWAnt,  antennaXML);

    for (size_t ii = 0; ii < antenna.tx.size(); ++ii)
    {
         XMLElem txXML = toXML("Tx", antenna.tx[ii], antennaXML);
         set_index_attribute(*txXML, ii + 1);
    }

    for (size_t ii = 0; ii < antenna.rcv.size(); ++ii)
    {
        XMLElem rcvXML = toXML("Rcv", antenna.rcv[ii], antennaXML);
        set_index_attribute(*rcvXML, ii + 1);
    }

    for (size_t ii = 0; ii < antenna.twoWay.size(); ++ii)
    {
        XMLElem twXML = toXML("TwoWay", antenna.twoWay[ii], antennaXML);
        set_index_attribute(*twXML, ii + 1);
    }

    return antennaXML;
}


XMLElem CPHDXMLControl::toXML(const std::string& name,
                              const AntennaParameters& params,
                              XMLElem parent)
{
    XMLElem apXML = newElement(name, parent);

    mCommon.createPolyXYZ("XAxisPoly", params.xAxisPoly, apXML);
    mCommon.createPolyXYZ("YAxisPoly", params.yAxisPoly, apXML);
    createDouble("FreqZero", params.frequencyZero, apXML);

    if (params.electricalBoresight.get())
    {
        XMLElem ebXML = newElement("EB", apXML);
        mCommon.createPoly1D("DCXPoly", params.electricalBoresight->dcxPoly, ebXML);
        mCommon.createPoly1D("DCYPoly", params.electricalBoresight->dcyPoly, ebXML);
    }
    if (params.halfPowerBeamwidths.get())
    {
        XMLElem hpXML = newElement("HPBW", apXML);
        createDouble("DCX", params.halfPowerBeamwidths->dcx, hpXML);
        createDouble("DCY", params.halfPowerBeamwidths->dcy, hpXML);
    }
    if (params.array.get())
    {
        XMLElem arrXML = newElement("Array", apXML);
        mCommon.createPoly2D("GainPoly", params.array->gainPoly, arrXML);
        mCommon.createPoly2D("PhasePoly", params.array->phasePoly, arrXML);
    }
    if (params.element.get())
    {
        XMLElem elemXML = newElement("Elem", apXML);
        mCommon.createPoly2D("GainPoly", params.element->gainPoly, elemXML);
        mCommon.createPoly2D("PhasePoly", params.element->phasePoly, elemXML);
    }
    if (!params.gainBSPoly.empty())
    {
        mCommon.createPoly1D("GainBSPoly", params.gainBSPoly, apXML);
    }

    createBooleanType("EBFreqShift", params.electricalBoresightFrequencyShift,
                      apXML);
    createBooleanType("MLFreqDilation", params.mainlobeFrequencyDilation,
                      apXML);

    return apXML;
}

XMLElem CPHDXMLControl::toXML(const VectorParameters& vp, XMLElem parent)
{
    XMLElem vectorParametersXML = newElement("VectorParameters", parent);

    createInt("TxTime", vp.txTime, vectorParametersXML);
    createInt("TxPos", vp.txPos, vectorParametersXML);
    createInt("RcvTime", vp.rcvTime, vectorParametersXML);
    createInt("RcvPos", vp.rcvPos, vectorParametersXML);

    if (!six::Init::isUndefined(vp.srpTime))
    {
        createInt("SRPTime", vp.srpTime, vectorParametersXML);
    }

    createInt("SRPPos", vp.srpPos, vectorParametersXML);

    if (!six::Init::isUndefined(vp.ampSF))
    {
        createInt("AmpSF", vp.ampSF, vectorParametersXML);
    }

    if (!six::Init::isUndefined(vp.tropoSRP))
    {
        createInt("TropoSRP", vp.tropoSRP, vectorParametersXML);
    }

    // Error check the conditional elements
    if (vp.fxParameters.get() == nullptr && vp.toaParameters.get() == nullptr)
    {
        throw except::Exception(Ctxt(
                "VectorParameters: either FxParameters or TOAParameters must "
                "be present"));
    }

    if (vp.fxParameters.get() != nullptr  && vp.toaParameters .get()!= nullptr)
    {
        throw except::Exception(Ctxt(
                "VectorParameters: FxParameters and TOAParameters cannot both "
                "be present"));
    }

    if (vp.fxParameters.get() != nullptr)
    {
        XMLElem fxParametersXML = newElement("FxParameters", vectorParametersXML);
        createInt("Fx0", vp.fxParameters->Fx0, fxParametersXML);
        createInt("Fx_SS", vp.fxParameters->FxSS, fxParametersXML);
        createInt("Fx1", vp.fxParameters->Fx1, fxParametersXML);
        createInt("Fx2", vp.fxParameters->Fx2, fxParametersXML);
    }

    if (vp.toaParameters.get() != nullptr)
    {
        XMLElem toaParametersXML = newElement("TOAParameters", vectorParametersXML);
        createInt("DeltaTOA0", vp.toaParameters->deltaTOA0, toaParametersXML);
        createInt("TOA_SS", vp.toaParameters->toaSS, toaParametersXML);
    }

    return vectorParametersXML;
}

XMLElem CPHDXMLControl::areaLineDirectionParametersToXML(
    const std::string& name,
    const AreaDirectionParameters& adp,
    XMLElem parent)
{
    XMLElem adpXML = newElement(name, parent);
    mCommon.createVector3D("UVectECF", adp.unitVector, adpXML);
    createDouble("LineSpacing", adp.spacing, adpXML);
    createInt("NumLines", adp.elements, adpXML);
    createInt("FirstLine", adp.first, adpXML);
    return adpXML;
}

XMLElem CPHDXMLControl::areaSampleDirectionParametersToXML(
    const std::string& name,
    const AreaDirectionParameters& adp,
    XMLElem parent)
{
    XMLElem adpXML = newElement(name, parent);
    mCommon.createVector3D("UVectECF", adp.unitVector, adpXML);
    createDouble("SampleSpacing", adp.spacing, adpXML);
    createInt("NumSamples", adp.elements, adpXML);
    createInt("FirstSample", adp.first, adpXML);
    return adpXML;
}

mem::auto_ptr<Metadata> CPHDXMLControl::fromXML(const std::string& xmlString)
{
    auto result = fromXML(str::EncodedStringView(xmlString).u8string());
    return mem::auto_ptr<Metadata>(result.release());
}
std::unique_ptr<Metadata> CPHDXMLControl::fromXML(const std::u8string& xmlString)
{
    io::U8StringStream stringStream;
    stringStream.write(xmlString);
    six::MinidomParser parser;
    parser.parse(stringStream);
    return fromXML(&parser.getDocument());
}

mem::auto_ptr<Metadata> CPHDXMLControl::fromXML(const xml::lite::Document* doc)
{
    auto cphd03 = std::make_unique<Metadata>();

    const auto root = doc->getRootElement();

    XMLElem collectionInfoXML   = getFirstAndOnly(root, "CollectionInfo");
    XMLElem dataXML             = getFirstAndOnly(root, "Data");
    XMLElem globalXML           = getFirstAndOnly(root, "Global");
    XMLElem channelXML          = getFirstAndOnly(root, "Channel");
    XMLElem srpXML              = getFirstAndOnly(root, "SRP");
    XMLElem antennaXML          = getOptional(root, "Antenna");
    XMLElem vectorParametersXML = getFirstAndOnly(root, "VectorParameters");

    // Parse XML for each section
    mCommon.parseCollectionInformationFromXML(collectionInfoXML,
            &cphd03->collectionInformation);
    fromXML(dataXML, cphd03->data);
    fromXML(globalXML, cphd03->global);
    fromXML(channelXML, cphd03->channel);
    fromXML(srpXML, cphd03->srp);

    if (antennaXML != nullptr)
    {
        cphd03->antenna.reset(new Antenna());
        fromXML(antennaXML, *cphd03->antenna);
    }

    fromXML(vectorParametersXML, cphd03->vectorParameters);

    return mem::auto_ptr<Metadata>(cphd03.release());
}
Metadata CPHDXMLControl::fromXML(const xml::lite::Document& doc)
{
    return *(fromXML(&doc));
}


void CPHDXMLControl::fromXML(const xml::lite::Element* dataXML, Data& data)
{
    data.sampleType = cphd::SampleType::toType(getFirstAndOnly(dataXML, "SampleType")->getCharacterData());

    parseUInt(getFirstAndOnly(dataXML, "NumCPHDChannels"), data.numCPHDChannels);
    parseUInt(getFirstAndOnly(dataXML, "NumBytesVBP"), data.numBytesVBP);

    std::vector<XMLElem> arraySizeXML;
    dataXML->getElementsByTagName("ArraySize", arraySizeXML);

    if (arraySizeXML.empty())
    {
        throw except::Exception(Ctxt("Expected at least one ArraySize"));
    }

    for (auto it = arraySizeXML.begin();
         it != arraySizeXML.end();
         ++it)
    {
        ArraySize as;
        parseUInt(getFirstAndOnly(*it, "NumVectors"), as.numVectors);
        parseUInt(getFirstAndOnly(*it, "NumSamples"), as.numSamples);
        data.arraySize.push_back(as);
    }
}

void CPHDXMLControl::fromXML(const xml::lite::Element* globalXML, Global& global)
{
    XMLElem tmpElem = nullptr;

    global.domainType = cphd::DomainType::toType(getFirstAndOnly(globalXML, "DomainType")->getCharacterData());
    global.phaseSGN   = cphd::PhaseSGN::toType(getFirstAndOnly(globalXML, "PhaseSGN")->getCharacterData());

    parseOptionalInt(globalXML, "RefFreqIndex", global.refFrequencyIndex);

    parseDateTime(getFirstAndOnly(globalXML, "CollectStart"), global.collectStart);

    parseDouble(getFirstAndOnly(globalXML, "CollectDuration"), global.collectDuration);


#if ENFORCESPEC
    /* This is the correct way to parse the Global TxTimeN values.
     * However, some old producers created CPHD files create files that don't match spec
     */
    parseDouble(getFirstAndOnly(globalXML, "TxTime1"), global.txTime1);
    parseDouble(getFirstAndOnly(globalXML, "TxTime2"), global.txTime2);

#else
    /*
     * Instead look for the incorrect Fields as optional
     * TODO: Is this still an ongoing problem or can this be removed?
     */
    tmpElem = getOptional(globalXML, "Tx1Time");
    if (tmpElem)
    {
        // Bad file:
        log()->warn(Ctxt("Found field Tx1Time, should be TxTime1. CPHD file does not meet spec."));
        parseDouble(tmpElem, global.txTime1);
    }
    else
        parseDouble(getFirstAndOnly(globalXML, "TxTime1"), global.txTime1);

    tmpElem = getOptional(globalXML, "Tx2Time");
    if (tmpElem)
    {
        // Bad file:
        log()->warn(Ctxt("Found field Tx2Time, should be TxTime2. CPHD file does not meet spec."));
        parseDouble(tmpElem, global.txTime2);
    }
    else
        parseDouble(getFirstAndOnly(globalXML, "TxTime2"), global.txTime2);
#endif

    XMLElem imageAreaXML = getFirstAndOnly(globalXML, "ImageArea");
    XMLElem cornersXML = getFirstAndOnly(imageAreaXML, "Corners");
    mCommon.parseFootprint(cornersXML, "ACP", global.imageArea.acpCorners);

    XMLElem planeXML = getOptional(imageAreaXML, "Plane");
    if (planeXML)
    {
        // Optional section
        global.imageArea.plane.reset(new AreaPlane());

        // All elements are required

        // Reference Point, "name" not required
        XMLElem refPtXML = getFirstAndOnly(planeXML, "RefPt");
        if (refPtXML->getAttributes().contains("name"))
        {
            global.imageArea.plane->referencePoint.name =
                    refPtXML->getAttributes().getValue("name");
        }

        mCommon.parseVector3D(getFirstAndOnly(refPtXML, "ECF"),
                              global.imageArea.plane->referencePoint.ecef);
        parseDouble(getFirstAndOnly(refPtXML, "Line"),
                    global.imageArea.plane->referencePoint.rowCol.row);
        parseDouble(getFirstAndOnly(refPtXML, "Sample"),
                    global.imageArea.plane->referencePoint.rowCol.col);

        // XDir info
        XMLElem dirXML = getFirstAndOnly(planeXML, "XDir");
        mCommon.parseVector3D(getFirstAndOnly(dirXML, "UVectECF"),
                      global.imageArea.plane->xDirection.unitVector);
        parseDouble(getFirstAndOnly(dirXML, "LineSpacing"),
                    global.imageArea.plane->xDirection.spacing);

#if ENFORCESPEC
        /*
         * This is the correct way to parse the Global NumLines/FirstLine values.
         * However, some old producers generated CPHD files create files that don't match spec
         */
        parseUInt(getFirstAndOnly(dirXML, "NumLines"),
                  global.imageArea.plane->xDirection.elements);
        parseUInt(getFirstAndOnly(dirXML, "FirstLine"),
                  global.imageArea.plane->xDirection.first);
#else
        // TODO: Is this still needed?
        double dtmp;
        size_t uitmp;
        tmpElem = getFirstAndOnly(dirXML, "NumLines");
        parseDouble(tmpElem, dtmp);
        parseUInt(tmpElem, uitmp);
        if (dtmp != uitmp)
        {
            log()->warn(Ctxt(
                    "Field XDir::NumLines is a double, should be an integer. "
                    "CPHD file does not meet spec."));
        }
        global.imageArea.plane->xDirection.elements = static_cast<size_t>(dtmp);

        tmpElem = getFirstAndOnly(dirXML, "FirstLine");
        parseDouble(tmpElem, dtmp);
        parseUInt(tmpElem, uitmp);
        if (dtmp != uitmp)
        {
            log()->warn(Ctxt(
                    "Field XDir::FirstLine is a double, should be an integer. "
                    "CPHD file does not meet spec."));
        }
        global.imageArea.plane->xDirection.first = static_cast<size_t>(dtmp);

#endif

        // YDir info
        dirXML = getFirstAndOnly(planeXML, "YDir");
        mCommon.parseVector3D(getFirstAndOnly(dirXML, "UVectECF"),
                      global.imageArea.plane->yDirection.unitVector);
        parseDouble(getFirstAndOnly(dirXML, "SampleSpacing"),
                    global.imageArea.plane->yDirection.spacing);
#if ENFORCESPEC
        /*
         * This is the correct way to parse the Global NumSamples/FirstSample values.
         * However, some old producers generated CPHD files create files that don't match spec
         */
        parseUInt(getFirstAndOnly(dirXML, "NumSamples"),
                  global.imageArea.plane->yDirection.elements);
        parseUInt(getFirstAndOnly(dirXML, "FirstSample"),
                  global.imageArea.plane->yDirection.first);
#else
        // TODO: Is this still needed?
        tmpElem = getFirstAndOnly(dirXML, "NumSamples");
        parseDouble(tmpElem, dtmp);
        parseUInt (tmpElem, uitmp);
        if (dtmp != uitmp)
        {
            log()->warn(Ctxt(
                    "Field YDir::NumSamples is a double, should be an integer. "
                    "CPHD file does not meet spec."));
        }
        global.imageArea.plane->yDirection.elements = static_cast<size_t>(dtmp);

        tmpElem = getFirstAndOnly(dirXML, "FirstSample");
        parseDouble(tmpElem, dtmp);
        parseUInt (tmpElem, uitmp);
        if (dtmp != uitmp)
        {
            log()->warn(Ctxt(
                    "Field YDir::FirstSample is a double, should be an "
                    "integer. CPHD file does not meet spec."));
        }
        global.imageArea.plane->yDirection.first = static_cast<size_t>(dtmp);
#endif

        // Dwell Time Polys optional
        XMLElem dwellTimeXML = getOptional(planeXML, "DwellTime");
        if (dwellTimeXML)
        {
            global.imageArea.plane->dwellTime.reset(new DwellTimeParameters());

            XMLElem codTimePolyXML = getFirstAndOnly(dwellTimeXML, "CODTimePoly");
            mCommon.parsePoly2D(codTimePolyXML,
                                global.imageArea.plane->dwellTime->codTimePoly);

            XMLElem dwellTimePolyXML = getFirstAndOnly(dwellTimeXML, "DwellTimePoly");
            mCommon.parsePoly2D(dwellTimePolyXML,
                                global.imageArea.plane->dwellTime->dwellTimePoly);
        }
    }
}

void CPHDXMLControl::fromXML(const xml::lite::Element* channelXML, Channel& channel)
{
    std::vector<XMLElem> chanParametersXML;
    channelXML->getElementsByTagName("Parameters", chanParametersXML);
    if (chanParametersXML.empty())
    {
        throw except::Exception(Ctxt("Expected at least one Parameters "
                                     "element"));
    }

    for (std::vector<XMLElem>::const_iterator it = chanParametersXML.begin();
         it != chanParametersXML.end();
         ++it)
    {
        ChannelParameters chanParam;
        parseInt(getFirstAndOnly(*it, "SRP_Index"), chanParam.srpIndex);
        parseDouble(getFirstAndOnly(*it, "NomTOARateSF"), chanParam.nomTOARateSF);
        parseDouble(getFirstAndOnly(*it, "FxCtrNom"), chanParam.fxCtrNom);
        parseDouble(getFirstAndOnly(*it, "BWSavedNom"), chanParam.bwSavedNom);
        parseDouble(getFirstAndOnly(*it, "TOASavedNom"), chanParam.toaSavedNom);

        parseOptionalInt(*it, "TxAnt_Index", chanParam.txAntIndex);
        parseOptionalInt(*it, "RcvAnt_Index", chanParam.rcvAntIndex);
        parseOptionalInt(*it, "TWAnt_Index", chanParam.twAntIndex);

        channel.parameters.push_back(chanParam);
    }
}

void CPHDXMLControl::fromXML(const xml::lite::Element* srpXML, SRP& srp)
{
#if ENFORCESPEC
    srp.srpType = cphd::SRPType(getFirstAndOnly(srpXML, "SRPType")->getCharacterData());
#else
    std::string s(getFirstAndOnly(srpXML, "SRPType")->getCharacterData());
    str::upper(s);
    srp.srpType = cphd::SRPType::toType(s);
#endif
    parseInt(getFirstAndOnly(srpXML, "NumSRPs"), srp.numSRPs);

    std::vector< XMLElem > tmpXMLs;
    switch ((int)srp.srpType)
    {
    case cphd::SRPType::FIXEDPT:

        srpXML->getElementsByTagName("FIXEDPT", tmpXMLs);
        for (std::vector<XMLElem>::const_iterator it = tmpXMLs.begin();
             it != tmpXMLs.end();
             ++it)
        {
            cphd::Vector3 srppt;
            mCommon.parseVector3D(getFirstAndOnly(*it, "SRPPT"), srppt);
            srp.srpPT.push_back(srppt);
        }
        if (srp.srpPT.size() != srp.numSRPs)
        {
            throw except::Exception(Ctxt("SRP: number of FIXEDPT entries must match NumSRPs"));
        }
        break;

    case cphd::SRPType::PVTPOLY:
        srpXML->getElementsByTagName("PVTPOLY", tmpXMLs);
        for (std::vector<XMLElem>::const_iterator it = tmpXMLs.begin();
             it != tmpXMLs.end();
             ++it)
        {
            cphd::PolyXYZ pvtPoly;
            mCommon.parsePolyXYZ(getFirstAndOnly(*it, "SRPPVTPoly"), pvtPoly);
            srp.srpPVTPoly.push_back(pvtPoly);
        }
        if (srp.srpPVTPoly.size() != srp.numSRPs)
        {
            throw except::Exception(Ctxt("SRP: number of PVTPOLY entries must match NumSRPs"));
        }
        break;

    case cphd::SRPType::PVVPOLY:
        srpXML->getElementsByTagName("PVVPOLY", tmpXMLs);
        for (std::vector<XMLElem>::const_iterator it = tmpXMLs.begin();
             it != tmpXMLs.end();
             ++it)
        {
            cphd::PolyXYZ pvvPoly;
            mCommon.parsePolyXYZ(getFirstAndOnly(*it, "SRPPVVPoly"), pvvPoly);
            srp.srpPVVPoly.push_back(pvvPoly);
        }
        if (srp.srpPVVPoly.size() != srp.numSRPs)
        {
            throw except::Exception(Ctxt("SRP: number of PVVPOLY entries must match NumSRPs"));
        }
        break;

    case cphd::SRPType::STEPPED:
        // No SRP type for this
        if (srp.numSRPs != 0)
        {
            throw except::Exception(Ctxt("SRP: SRPType of STEPPED must have NumSRPs equal zero"));
        }
        break;

    default:
        throw except::Exception(Ctxt(std::string("Invalid SRPType: ")
                + srp.srpType.toString()));

        break;
    }
}

void CPHDXMLControl::fromXML(const xml::lite::Element* vectorParametersXML,
                             VectorParameters& vp)
{
    parseInt(getFirstAndOnly(vectorParametersXML, "TxTime"), vp.txTime);
    parseInt(getFirstAndOnly(vectorParametersXML, "TxPos"), vp.txPos);
    parseInt(getFirstAndOnly(vectorParametersXML, "RcvTime"), vp.rcvTime);
    parseInt(getFirstAndOnly(vectorParametersXML, "RcvPos"), vp.rcvPos);

    parseOptionalInt(vectorParametersXML, "SRPTime", vp.srpTime);

    parseInt(getFirstAndOnly(vectorParametersXML, "SRPPos"), vp.srpPos);

    parseOptionalInt(vectorParametersXML, "AmpSF", vp.ampSF);

    parseOptionalInt(vectorParametersXML, "TropoSRP", vp.tropoSRP);

    // We must have either FxParameters or TOAParameters
    XMLElem FxParametersXML = getOptional(vectorParametersXML, "FxParameters");
    XMLElem TOAParametersXML = getOptional(vectorParametersXML, "TOAParameters");

    if (FxParametersXML == nullptr  && TOAParametersXML == nullptr)
    {
        throw except::Exception(Ctxt(
                "VectorParameters: either FxParameters or TOAParameters must "
                "be present"));
    }

    if (FxParametersXML != nullptr  && TOAParametersXML != nullptr)
    {
        throw except::Exception(Ctxt(
                "VectorParameters: FxParameters and TOAParameters cannot both "
                "be present"));
    }

    if (FxParametersXML != nullptr)
    {
        vp.fxParameters.reset(new FxParameters());
        parseInt(getFirstAndOnly(FxParametersXML, "Fx0"), vp.fxParameters->Fx0);
        parseInt(getFirstAndOnly(FxParametersXML, "Fx_SS"), vp.fxParameters->FxSS);
        parseInt(getFirstAndOnly(FxParametersXML, "Fx1"), vp.fxParameters->Fx1);
        parseInt(getFirstAndOnly(FxParametersXML, "Fx2"), vp.fxParameters->Fx2);
    }

    if (TOAParametersXML != nullptr)
    {
        vp.toaParameters.reset(new TOAParameters());
        parseInt(getFirstAndOnly(TOAParametersXML, "DeltaTOA0"), vp.toaParameters->deltaTOA0);
        parseInt(getFirstAndOnly(TOAParametersXML, "TOA_SS"), vp.toaParameters->toaSS);
    }
}

void CPHDXMLControl::fromXML(const xml::lite::Element* antennaParamsXML,
                             AntennaParameters& params)
{
    mCommon.parsePolyXYZ(getFirstAndOnly(antennaParamsXML, "XAxisPoly"),
                         params.xAxisPoly);
    mCommon.parsePolyXYZ(getFirstAndOnly(antennaParamsXML, "YAxisPoly"),
                         params.yAxisPoly);
    parseDouble(getFirstAndOnly(antennaParamsXML, "FreqZero"),
                params.frequencyZero);

    XMLElem tmpElem = getOptional(antennaParamsXML, "EB");
    if (tmpElem)
    {
        params.electricalBoresight.reset(new ElectricalBoresight());
        mCommon.parsePoly1D(getFirstAndOnly(tmpElem, "DCXPoly"),
                            params.electricalBoresight->dcxPoly);
        mCommon.parsePoly1D(getFirstAndOnly(tmpElem, "DCYPoly"),
                            params.electricalBoresight->dcyPoly);
    }

    tmpElem = getOptional(antennaParamsXML, "HPBW");
    if (tmpElem)
    {
        params.halfPowerBeamwidths.reset(new HalfPowerBeamwidths());
        parseDouble(getFirstAndOnly(tmpElem, "DCX"),
                    params.halfPowerBeamwidths->dcx);
        parseDouble(getFirstAndOnly(tmpElem, "DCY"),
                    params.halfPowerBeamwidths->dcy);
    }

    tmpElem = getOptional(antennaParamsXML, "Array");
    if (tmpElem)
    {
        params.array.reset(new GainAndPhasePolys());
        mCommon.parsePoly2D(getFirstAndOnly(tmpElem, "GainPoly"),
                            params.array->gainPoly);
        mCommon.parsePoly2D(getFirstAndOnly(tmpElem, "PhasePoly"),
                            params.array->phasePoly);
    }

    tmpElem = getOptional(antennaParamsXML, "Elem");
    if (tmpElem)
    {
        params.element.reset(new GainAndPhasePolys());
        mCommon.parsePoly2D(getFirstAndOnly(tmpElem, "GainPoly"),
                            params.element->gainPoly);
        mCommon.parsePoly2D(getFirstAndOnly(tmpElem, "PhasePoly"),
                            params.element->phasePoly);
    }

    tmpElem = getOptional(antennaParamsXML, "GainBSPoly");
    if (tmpElem)
    {
        //optional
        mCommon.parsePoly1D(tmpElem, params.gainBSPoly);
    }

    tmpElem = getOptional(antennaParamsXML, "EBFreqShift");
    if (tmpElem)
    {
        //optional
        parseBooleanType(tmpElem, params.electricalBoresightFrequencyShift);
    }

    tmpElem = getOptional(antennaParamsXML, "MLFreqDilation");
    if (tmpElem)
    {
        //optional
        parseBooleanType(tmpElem, params.mainlobeFrequencyDilation);
    }
}

void CPHDXMLControl::fromXML(const xml::lite::Element* antennaXML, Antenna& antenna)
{
    parseInt(getFirstAndOnly(antennaXML, "NumTxAnt"),  antenna.numTxAnt);
    parseInt(getFirstAndOnly(antennaXML, "NumRcvAnt"), antenna.numRcvAnt);
    parseInt(getFirstAndOnly(antennaXML, "NumTWAnt"),  antenna.numTWAnt);

    /* Cannot have both One-Way and Two-Way Parameters
       In CPHD spec:
          "Note: Include only one-way patterns or two-way patterns."
    */

    if ((antenna.numTxAnt != 0 || antenna.numRcvAnt != 0) &&
        antenna.numTWAnt != 0)
    {
        throw except::Exception(Ctxt(
                "Antenna: cannot have both One-Way and Two-Way Antenna "
                "Parameters"));
    }

    std::vector<XMLElem> tmpXMLs;

    if (antenna.numTxAnt)
    {
        antennaXML->getElementsByTagName("Tx", tmpXMLs);
        antenna.tx.resize(tmpXMLs.size());
        for (size_t ii = 0; ii < antenna.tx.size(); ++ii)
        {
            fromXML(tmpXMLs[ii], antenna.tx[ii]);
        }
    }

    tmpXMLs.clear();
    if (antenna.numRcvAnt)
    {
        antennaXML->getElementsByTagName("Rcv", tmpXMLs);
        antenna.rcv.resize(tmpXMLs.size());
        for (size_t ii = 0; ii < antenna.rcv.size(); ++ii)
        {
            fromXML(tmpXMLs[ii], antenna.rcv[ii]);
        }
    }

    tmpXMLs.clear();
    if (antenna.numTWAnt)
    {
        antennaXML->getElementsByTagName("TwoWay", tmpXMLs);
        antenna.twoWay.resize(tmpXMLs.size());
        for (size_t ii = 0; ii < antenna.twoWay.size(); ++ii)
        {
            fromXML(tmpXMLs[ii], antenna.twoWay[ii]);
        }
    }
}
}
