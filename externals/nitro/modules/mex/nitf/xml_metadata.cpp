/*!
 *  This matlab function gets XML out of (almost) anywhere, and turns
 *  it into a matlab cell matrix.
 *
 *  For most typical users, you just pass in an XML file and this function
 *  will reorganize the file into a hierarchical matlab struct.
 *
 *  This function also supports reading XML in from the DES segment of
 *  a NITF.  In that case, there may be more than one XML structure coming
 *  back.
 *
 */

#include <import/nitf.hpp>
#include <import/xml/lite.h>
#include <import/sys.h>
#include <import/except.h>
#include <import/io.h>
#include <iostream>
#include <cmath>

#if defined(WIN32)
#    if _MSC_VER < 1600
         typedef wchar_t char16_t;
#    else
#        define __STDC_UTF_16__
#    endif
#endif
#include "mex.h"

using namespace sys;
using namespace xml::lite;

extern void _main();

// This is just to keep track of things
typedef std::map<std::string, int> Count_T;

// This is to get a count of all unique children of this node
static void getUniqueChildrenCount(xml::lite::Element* element, 
				   Count_T& count)
{
    int numChildren = element->getChildren().size();

    for (int i = 0; i < numChildren; i++)
    {
	xml::lite::Element* child = element->getChildren()[i];
	count[ child->getLocalName() ]++; 
    }
    return;

}

// Does the work
static mxArray* createMx(xml::lite::Element* element)
{
    // Total size is numChildren + offset
    size_t offset = 0;

    // We will use this to store data associated with this elem
    std::map<std::string, std::string> data;
    
    int i = 0;

    // This is a data structure that lists children
    Count_T childCount;

    // Get a listing of each count of children
    getUniqueChildrenCount( element, childCount );


    // The number of unique children
    size_t numChildren = childCount.size();
    //std::cout << "Num unique elements: " << numChildren << std::endl;

    // This is the tag character data
    std::string str = element->getCharacterData();
    
    // If the body is empty, no CD
    if (!str.empty())
    {	
	// Otherwise add it to our data for this element
	data[ std::string("characters") ] = str;

 	// Add a child for the data
 	offset++;
    }

    xml::lite::Attributes atts = element->getAttributes();
    for (i = 0; i < atts.getLength(); i++)
    {
	std::string attVal = atts[i].getValue();
	if (!attVal.empty())
	{
	    std::string localName = atts[i].getLocalName() + "Attr";
	    data[ localName ] = attVal;
	    offset++;
	}
    }
    
    // Our number of unique fields is going to be the unique children
    // plus the data components
    const size_t numberOfFields = numChildren + offset;

    // Attributes goes in data too
    char** names = new char*[numberOfFields];

    // Reset i
    i = 0;
    // For each unique tag name
    for (std::map<std::string, int>::const_iterator p =
	     childCount.begin(); p != childCount.end(); ++p)
    {
	// Get it
	std::string fieldName = p->first;
	// Set it
	names[i] = new char[fieldName.length() + 1];
	// Copy it
	strcpy(names[i], fieldName.c_str());
	i++;
    }

    // Also have to do the same for each data component
    // Notice we are not resetting i
    if (offset)
    {
	for (std::map<std::string, std::string>::const_iterator it =
		 data.begin(); it != data.end(); ++it)
	{
	    // Add something here
	    names[i] = new char[it->first.length() + 1];
	    strcpy(names[i], it->first.c_str());
	    i++;
	}
    }
    // Now allocate the struct, since we can
    mxArray* structArrayPtr = 
	mxCreateStructMatrix(1, 1, numberOfFields, (const char**)names);

    // Reset i
    i = 0;
    
    // Now go through each unique field
    for (std::map<std::string, int>::const_iterator p =
	     childCount.begin(); p != childCount.end(); ++p)
    {
	
	// Get it
	std::string fieldName = p->first;
	
	// Num times it happens
	int count = p->second;
	
	mxArray* childPtr = NULL;
	    // Get the vector of elements
	std::vector<xml::lite::Element*> sameElements;
	element->getElementsByTagName( fieldName, sameElements );
	    

	// If there are multiple elements, create a cell matrix
	// 1xN
	// We need to do this whole rigamorale for each child
	childPtr = mxCreateCellMatrix(count, 1);
	for (int j = 0; j < count; j++)
	{
	    mxSetCell(childPtr, j, createMx(sameElements[j]));
	}
	
 	mxSetField(structArrayPtr, 0, 
 		   names[i],
 		   childPtr);
	i++;
    }


    if (offset)
    {
	assert( data.size() + i == numberOfFields);
	assert( numberOfFields ==
	        static_cast<size_t>(mxGetNumberOfFields(structArrayPtr)) );
	for (std::map<std::string, std::string>::const_iterator it =
		 data.begin(); it != data.end(); ++it)
	{

 	    mxSetField(structArrayPtr, 0, 
 		       it->first.c_str(), mxCreateString(it->second.c_str()));

	    i++;
	}
    }
    
    for (size_t ii = 0; ii < numberOfFields; ++ii)
    {
	delete [] names[ii];
    }
    delete [] names;
    
    return structArrayPtr;
}

static mxArray* createStruct(xml::lite::Element* root)
{
    const char* fieldNames[] = { root->getLocalName().c_str() };
    mxArray* structArrayPtr = mxCreateStructMatrix(1, 1, 1, fieldNames);

    mxArray* structArrayChildPtr = createMx( root );

    mxSetField(structArrayPtr, 0, 
	       fieldNames[0],
	       structArrayChildPtr);

    return structArrayPtr;
}

static std::string getString(const mxArray* mx)
{
    if (!mxIsChar(mx))
	throw except::Exception(Ctxt("Require string arg"));

    if (mxGetM(mx) != 1)
	throw except::Exception(Ctxt("Input must be a row vector"));


     size_t len = (mxGetM(mx) * mxGetN(mx)) + 1;
     char* buf = new char[len];
     if (mxGetString(mx, buf, len) != 0)
     {
	 delete [] buf;
	 throw except::Exception(Ctxt("Not enough space!"));
     }
     std::string s(buf);
     delete [] buf;
     return s;
				
}


class IOInputStreamAdapter : public ::io::InputStream
{

    nitf::SegmentReader& mReader;
public:

    IOInputStreamAdapter(nitf::SegmentReader& reader) : mReader(reader) {}
    ~IOInputStreamAdapter() {}

    virtual sys::Off_T available()
    {
        return (sys::Off_T)mReader.getSize();
    }

protected:
    virtual sys::SSize_T readImpl(void* buffer, size_t len)
    {
        mReader.read(buffer, len);
        return len;
    }
};

void mexFunction(int nlhs, mxArray *plhs[],
		 int nrhs, const mxArray *prhs[])
{

    if (nrhs != 1 && nrhs != 5) 
    {
	mexErrMsgTxt("<file-name>");
	return;
    }
    else if (nlhs > 1) 
    {
	mexErrMsgTxt("function requires only one output (struct)");
	return;
    }
    try
    {
      
 	std::string inputFile = getString(prhs[0]);
	
	// Must not be a NITF -- maybe its an XML file
	// For now this is a feature of the function
	if (nitf::Reader::getNITFVersion(inputFile) == NITF_VER_UNKNOWN)
	{
	    xml::lite::MinidomParser parser;
	    ::io::FileInputStream fis( inputFile );
	    parser.parse( fis );
	    xml::lite::Document* doc = parser.getDocument();
	    // We are going out of scope at the end of this brace
	   
            Element* root = doc->getRootElement();
            plhs[0] = createStruct( root );

	    fis.close();
	}
	else
	{
            nitf::Reader reader;
	    nitf::IOHandle io(inputFile);
	    nitf::Record record = reader.read(io);
            nitf::List des = record.getDataExtensions();
	    uint32_t numDES = des.getSize();
            std::vector<mxArray*> structs;

            for (uint32_t i = 0; i < numDES; i++)
            {
                nitf::DESegment seg = des[i];
                nitf::DESubheader subheader = seg.getSubheader();
                std::string desid = subheader.getTypeID().toString();
                str::trim(desid);


                if (str::endsWith(desid, "XML"))
                {
                    try
                    {
                        nitf::SegmentReader deReader = reader.newDEReader(i);
                        IOInputStreamAdapter ioAdapter(deReader);
                        xml::lite::MinidomParser xmlParser;
                        xmlParser.parse(ioAdapter);
                        xml::lite::Document* doc = xmlParser.getDocument();
			Element* root = doc->getRootElement();

                        mxArray* xmlStruct = createStruct( root );
                        structs.push_back(xmlStruct);
                        
                    }
                    catch (except::Exception& e)
                    {
                        std::cout << "Failed to parse segment.  Skipping [" << desid << "]. " << e.getMessage() << std::endl;

                    }
                }
                // Now join them all together into one big LHS
                
            }
            mxArray* allPtr = mxCreateCellMatrix( structs.size(), 1);
            for (unsigned int i = 0; i < structs.size(); i++)
            {
                mxSetCell(allPtr, i, structs[i]);
            }
            plhs[0] = allPtr;
	}
	


    }
    catch (except::Exception& e)
    {
 	mexErrMsgTxt(e.getMessage().c_str());
    }
}

