/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

#include <vector>

#include <import/six.h>
#include <import/six/sidd.h>
#include <import/six/sicd.h>
#include <import/xml/lite.h>
#include <import/io.h>

#include <sys/Filesystem.h>
namespace fs = std::filesystem;

/*
 *  Generate a KML file for a SICD XML to help with
 *  visualization
 *
 */

std::string generateKML(six::Data* data, const fs::path& outputDir);

/*
 *  Open the round trip product in whatever you use to view XML files
 *  (e.g., Altova XMLSpy).  This only works currently on windows
 *
 */
#if (defined(WIN32) || defined(_WIN32)) && defined(PREVIEW)
#   include <shellapi.h>
void preview(std::string outputFile)
{
    // ShellExecute might get assigned to ShellExecuteW if we arent careful
    ShellExecuteA(nullptr, "open", outputFile.c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
}
#   else
// TODO Could open this using EDITOR or html view
void preview(std::string outputFile)
{
    std::cerr << "Preview unavailable for: " << outputFile << std::endl;
}
#endif

/*
 *  Register the XMLControl handler objects for
 *  SICD and SIDD
 */
void registerHandlers()
{

    six::XMLControlFactory::getInstance(). addCreator(
                                                      six::DataType::COMPLEX,
                                                      new six::XMLControlCreatorT<
                                                              six::sicd::ComplexXMLControl>());

    six::XMLControlFactory::getInstance(). addCreator(
                                                      six::DataType::DERIVED,
                                                      new six::XMLControlCreatorT<
                                                              six::sidd::DerivedXMLControl>());

}
/*
 * Dump all files out to the local directory
 *
 */
std::vector<std::string> extractXML(std::string inputFile,
                                    const fs::path& outputDir)
{
    std::vector<std::string> allFiles;
    const std::string prefix = fs::path(inputFile).stem();

    nitf::Reader reader;
    nitf::IOHandle io(inputFile);
    nitf::Record record = reader.read(io);

    uint32_t numDES = record.getNumDataExtensions();
    for (uint32_t i = 0; i < numDES; ++i)
    {
        nitf::DESegment segment = record.getDataExtensions()[i];
        nitf::DESubheader subheader = segment.getSubheader();

        nitf::SegmentReader deReader = reader.newDEReader(i);
        nitf::Off size = deReader.getSize();

        const auto typeID = subheader.typeID();
        const std::string outputFile = FmtX("%s-%s%d.xml", prefix.c_str(), typeID.c_str(), i);
        const auto fileName = outputDir / outputFile;
        {
            std::vector<char> xml(size);
            deReader.read(xml.data(), size);

            io::StringStream oss;
            oss.write(xml.data(), size);

            xml::lite::MinidomParser parser;
            parser.parse(oss);

            xml::lite::Document* doc = parser.getDocument();

            io::FileOutputStream fos(fileName);
            doc->getRootElement()->prettyPrint(fos);
            fos.close();
        }
        allFiles.push_back(fileName);
    }
    io.close();
    return allFiles;
}

void run(std::string inputFile, std::string dataType)
{
    try
    {
        // Create an output directory if it doesnt already exist
        const auto outputDir(fs::path(inputFile).stem());
        if (!fs::exists(outputDir))
            fs::create_directory(outputDir);

        std::string xmlFile = inputFile;

        // Check if the file is a NITF - if so, extract all the parts into our outputDir
        if (nitf::Reader::getNITFVersion(inputFile) != NITF_VER_UNKNOWN)
        {
            std::vector<std::string> allFiles =
                    extractXML(inputFile, outputDir);
            if (!allFiles.size())
                throw except::Exception(
                                        Ctxt(
                                             std::string("Invalid input NITF: ")
                                                     + inputFile));
            xmlFile = allFiles[0];
        }

        // Now show the file (only if PREVIEW is defined)
        preview(xmlFile);

        // Read in the XML file from the disk location into a SICD/SIDD container
        io::FileInputStream xmlFileStream(xmlFile);
        xml::lite::MinidomParser treeBuilder;
        treeBuilder.parse(xmlFileStream);
        xmlFileStream.close();

        six::DataType dt = (dataType == "sicd") ? six::DataType::COMPLEX
                                                : six::DataType::DERIVED;

        std::unique_ptr<logging::Logger> log (new logging::NullLogger());
        six::XMLControl *control =
                six::XMLControlFactory::getInstance().newXMLControl(dt, log.get());

        six::Data *data = control->fromXML(treeBuilder.getDocument(),
                                           std::vector<std::string>());

        // Dump some core info
        std::cout << "Data Class: " << six::toString(data->getDataType())
                << std::endl;
        std::cout << "Pixel Type: " << six::toString(data->getPixelType())
                << std::endl;
        std::cout << "Num Rows  : " << six::toString(data->getNumRows())
                << std::endl;
        std::cout << "Num Cols  : " << six::toString(data->getNumCols())
                << std::endl;

        // Generate a KML in this directory
        preview(generateKML(data, outputDir));

        // Generate a Round-Trip file
        const auto outputFile = outputDir / "round-trip.xml";
        xml::lite::Document* outDom = 
            control->toXML(data, std::vector<std::string>());
        io::FileOutputStream outXML(outputFile);
        outDom->getRootElement()->prettyPrint(outXML);
        outXML.close();
        delete outDom;

        // Now show the output file if PREVIEW
        preview(outputFile);

        delete control;
        delete data;
    }
    catch (const std::exception& ex)
    {
        std::cout << "ERROR!: " << ex.what() << std::endl;
    }
}

int main(int argc, char** argv)
{

    if (argc != 3)
    {
        die_printf("Usage: %s <nitf/xml-file/nitf-dir> <sidd|sicd>\n", argv[0]);
    }

    // Is the data type SICD or SIDD
    std::string dataType = argv[2];

    // Ignore case to be safe
    str::lower(dataType);

    // And check that everything is ok
    if (dataType != "sicd" && dataType != "sidd")
        die_printf("Error - data type should be sicd or sidd");

    // Do this prior to reading any XML
    registerHandlers();

    // The input file (an XML or a NITF file)
    const fs::path inputFile(argv[1]);
    std::vector<fs::path > paths;

    if (fs::is_directory(inputFile))
    {
        const auto listing = sys::Path(inputFile).list();
        for (const auto& listing_i : listing)
        {
            auto listing_i_ = listing_i;
            str::lower(listing_i_);
            if (str::endsWith(listing_i_, "ntf")
                || str::endsWith(listing_i_, "nitf")
                || str::endsWith(listing_i_, "xml"))
            {
                paths.push_back(inputFile / listing_i);
            }
        }
    }
    else
    {
        paths.push_back(inputFile);
    }
    for (unsigned int i = 0; i < paths.size(); ++i)
    {
        std::cout << "Parsing file: " << paths[i] << std::endl;
        run(paths[i], dataType);
    }
    return 0;
}

const std::string KML_URI = "http://www.opengis.net/kml/2.2";
const std::string GX_URI = "http://www.google.com/kml/ext/2.2";
const std::string FOOTPRINT_COLOR = "ffff0000";

xml::lite::Element* createPath(const six::LatLonCorners& coords,
                               const std::string& name,
                               const std::string& envelopeType = "LineString",
                               const std::string& envelopeURI = KML_URI)
{
    std::ostringstream oss;

    // Print out all coordinates, potentially repeating the first element
    size_t end = six::LatLonCorners::NUM_CORNERS;
    if (envelopeType == "LinearRing")
    {
        ++end;
    }

    for (size_t ii = 0; ii < end; ++ii)
    {
        if (ii > 0)
        {
            oss << " ";
        }
        const size_t idx(ii % six::LatLonCorners::NUM_CORNERS);
        const six::LatLon& corner(coords.getCorner(idx));
        oss << corner.getLon() << "," << corner.getLat();
    }

    xml::lite::Element* coordsXML = new xml::lite::Element("coordinates",
                                                           envelopeURI, oss.str());
    xml::lite::Element* envelopeXML = new xml::lite::Element(envelopeType,
                                                             envelopeURI);
    envelopeXML->addChild(coordsXML);

    xml::lite::Element* placemarkXML = new xml::lite::Element("Placemark",
                                                              envelopeURI);
    placemarkXML->addChild(new xml::lite::Element("name", envelopeURI, name));
    placemarkXML->addChild(new xml::lite::Element("styleUrl", envelopeURI,
                                                  std::string("#") + name));
    placemarkXML->addChild(envelopeXML);

    return placemarkXML;
}

xml::lite::Element* createPath(const std::vector<six::LatLonAlt>& coords,
                               const std::string& name,
                               const std::string& envelopeType = "LineString",
                               const std::string& envelopeURI = KML_URI)
{
    std::ostringstream oss;

    // Print out all coordinates, potentially repeating the first element
    if (coords.empty())
    {
        throw except::Exception(Ctxt("Unexpected zero-length coordinates"));
    }

    size_t end = coords.size();
    if (envelopeType == "LinearRing")
    {
        ++end;
    }

    for (size_t ii = 0; ii < end; ++ii)
    {
        if (ii > 0)
        {
            oss << " ";
        }
        const size_t idx(ii % coords.size());
        const six::LatLonAlt& corner(coords[idx]);
        oss << corner.getLon() << "," << corner.getLat() << ","
            << corner.getAlt();
    }

    xml::lite::Element* coordsXML = new xml::lite::Element("coordinates",
                                                           envelopeURI, oss.str());
    xml::lite::Element* envelopeXML = new xml::lite::Element(envelopeType,
                                                             envelopeURI);
    envelopeXML->addChild(coordsXML);

    xml::lite::Element* placemarkXML = new xml::lite::Element("Placemark",
                                                              envelopeURI);
    placemarkXML->addChild(new xml::lite::Element("name", envelopeURI, name));
    placemarkXML->addChild(new xml::lite::Element("styleUrl", envelopeURI,
                                                  std::string("#") + name));
    placemarkXML->addChild(envelopeXML);

    return placemarkXML;
}

xml::lite::Element* createTimeSnapshot(const six::DateTime& dt)//, int seconds)
{
    xml::lite::Element* timeSnapshot = new xml::lite::Element("TimeStamp",
                                                              KML_URI);
    timeSnapshot->addChild(
                           new xml::lite::Element(
                                                  "when",
                                                  KML_URI,
                                                  six::toString<six::DateTime>(
                                                                               dt)));
    return timeSnapshot;
}

void generateKMLForSICD(xml::lite::Element* docXML,
                        six::sicd::ComplexData* data)
{
    std::vector<six::LatLonAlt> v;
    // Lets go ahead and try to add the ARP poly

    v.push_back(data->geoData->scp.llh);
    v.push_back(scene::Utilities::ecefToLatLon(data->scpcoa->arpPos));

    docXML->addChild(createPath(v, "arpToSceneCenter", "LineString"));

    // Time associated with start
    six::DateTime dateTime = data->timeline->collectStart;

    size_t durationInSeconds = (size_t) data->timeline->collectDuration;
    six::Vector3 atTimeX;
    for (size_t i = 0; i < durationInSeconds; ++i)
    {
        atTimeX = data->position->arpPoly(static_cast<double>(i));
        v[1] = scene::Utilities::ecefToLatLon(atTimeX);
        xml::lite::Element* arpPolyXML = createPath(v, "arpPoly", "LineString");
        six::DateTime dt(dateTime.getTimeInMillis() + i * 1000);
        arpPolyXML->addChild(createTimeSnapshot(dt));
        docXML->addChild(arpPolyXML);
    }

}

xml::lite::Element* createLineStyle(std::string styleID, std::string color,
                                    int width)
{
    xml::lite::Element* styleXML = new xml::lite::Element("Style", KML_URI);
    styleXML->attribute("id") = styleID;

    xml::lite::Element* lineStyleXML = new xml::lite::Element("LineStyle",
                                                              KML_URI);
    lineStyleXML->addChild(new xml::lite::Element("color", KML_URI, color));
    lineStyleXML->addChild(new xml::lite::Element("width", KML_URI,
                                                  str::toString<int>(width)));

    styleXML->addChild(lineStyleXML);
    return styleXML;
}

std::string generateKML(six::Data* data, const fs::path& outputDir)
{
    const std::string kmlFile = "visualization.kml";
    const auto kmlPath = outputDir / kmlFile;

    io::FileOutputStream fos(kmlPath);
    xml::lite::Element* root = new xml::lite::Element("kml", KML_URI);

    xml::lite::Element* docXML = new xml::lite::Element("Document", KML_URI);
    root->addChild(docXML);

    // Add name, and open the document
    docXML->addChild(new xml::lite::Element("name", KML_URI, data->getName()));
    docXML->addChild(new xml::lite::Element("open", KML_URI, "1"));

    // Now add some styles
    docXML->addChild(createLineStyle("footprint", FOOTPRINT_COLOR, 4));
    docXML->addChild(createLineStyle("arpToSceneCenter", "ff0000ff", 2));
    docXML->addChild(createLineStyle("arpPoly", "ff00007f", 2));

    // Create footprint
    const six::LatLonCorners corners(data->getImageCorners());
    docXML->addChild(createPath(corners, "footprint", "LinearRing"));

    // Specifics to SICD
    if (data->getDataType() == six::DataType::COMPLEX)
        generateKMLForSICD(docXML, (six::sicd::ComplexData*) data);

    root->prettyPrint(fos);
    delete root;
    fos.close();
    return kmlPath;
}
