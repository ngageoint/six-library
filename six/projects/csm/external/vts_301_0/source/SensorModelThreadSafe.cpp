//#############################################################################
//    FILENAME:   SensorModelThreadSafe.cpp
//
//    DESCRIPTION: Sensor Model Thread Safe test driver
//
//    Copyright (C) BAE Systems National Security Solutions Inc. 1989-2010
//                            ALL RIGHTS RESERVED
//    NOTES:
//    SOFTWARE HISTORY:
//     Date         Author Comment    
//      29 Jun 2011  DSL   Initial for VTS thread testing
//
//      14 Feb 2012  DSL   added ISD file types NITF_21_TYPE, NITF_20_TYPE,
//      10 Jan 2013  DSL   changes to reflect C++ reformat CRs
//      28 Jan 2013  DSL   VTS_VERSION CSM3.0.1.0
//#############################################################################

#include <iostream>
#include <fstream>

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "Plugin.h"
#include "VTSMisc.h"
#include "RasterGM.h"
#include "NitfIsd.h"
#include "SMManager.h"
#include "tinyxml.h"
#include <vector>
#include <map>

//using std::ifstream;
using namespace std;
using namespace csm;

// forward declarations
class ImageTest;
std::string initializeTestData();
std::string runThreads();

////////////////////////////////////////

std::string findAttribute(TiXmlNode* rootnode, std::string nodename, std::string attrname);
std::list<std::list<std::string> > findNodesAttributes(TiXmlNode* rootnode, std::string nname);
std::list<std::string> findNodes(TiXmlNode* rootnode, string nname);
TiXmlNode* findNode(TiXmlNode*  pnode, std::string nname);
string parseConfigFile(std::string filename);

// global variables.
int g_randSeed = 42;
int g_numberOfTestPoints = 100;
int g_numberOfTestRuns =2; //= 10;
const int MAXNUMTHREADS = 12;
int g_numThreads =4; // = 12;
bool g_oneSMPerThread = false;
std::string PluginName;
std::string SensorModelName;

	struct ImageInfo
	{
		std::string     filename;
        ISDType ISDFileType;
		int imageIndex;
	};
std::list<ImageInfo> imageList;

bool* threadStatus;
ImageTest *g_testArray = NULL;
int g_numberOfImageTests = -1;
TiXmlDocument xmldoc;
string xmlstatus;
RasterGM** g_modelList = NULL;
int g_modelListLength = 0;




// run the multi threaded sensor model test.
// the test will read the list of images from the filename,
// then go though and for each image, pick g_numberOfTestPoints
// random image points, and get the ground point.
// Then it will start g_numThreads threads, and each thread will
// go through the list of image, and recompute the ground points and
// test if it matches the precomputed points.

// There are 2 test modes:
// 1. default: threads sharing one sensor model (HART, Gridlock) 
//    this mode tests the thread-safe for the RasterGM API.
//  HART uses the same sensor model object across all its threads.  It creates
//  all of the sensor models on a single thread, then uses the sensor model to
//  create work packets that are processed by a thread pool.
// 2. -c: one sensor model per thread (NGL)
//    this mode tests the thread-safe for the SMS API.
//   Each thread creates and uses its own sensor model.

std::string testThreads(std::string configfile)
{
	std::string reply = "ok";
	reply = parseConfigFile(configfile);

	string txt;
	txt = "PluginName";
	std::cout << txt << ": " << PluginName << std::endl;

	txt = "SensorModelName";
	std::cout << txt << ": " << SensorModelName << std::endl;

	std::cout << "images: " << std::endl;

    std::list<ImageInfo>::iterator iter;
	iter = imageList.begin();

	for(int i=0;i<g_modelListLength;i++)
	{
		std::cout << (*iter).filename << " - " << ISDType2str((*iter).ISDFileType) << std::endl;
		iter++;
	}
	std::cout << std::endl;

	cout << "g_numThreads: " << g_numThreads << endl;
	if(g_oneSMPerThread)
	{
	   cout << "g_oneSMPerThread: true" << endl;
	}
	else
	{
	   cout << "g_oneSMPerThread: false" << endl;
	}

	cout << "g_numberOfTestPoints: " << g_numberOfTestPoints << endl;
	cout << "g_numberOfTestRuns: " << g_numberOfTestRuns << endl;
	cout << "g_modelListLength: " << g_modelListLength << endl;

	printf( "\n\n** Printing via xmldoc.Print **\n" );
    xmldoc.Print();

   if(reply == "ok")
   {
	   // Use VTS SMManager to load the plug-ins in a directory
	   std::string dirName;
       #ifdef _WIN32
          dirName = ".\\";
       #else
          dirName = "./";
       #endif
       SMManager::instance().loadLibraries(dirName.c_str());

	   // create the global model list
	   g_modelList = new RasterGM*[g_modelListLength];
   }

   if(reply == "ok")
   {
	   std::cerr << "Initiliaze test..." << std::endl;
	   reply = initializeTestData();
   }

   if(reply == "ok")
   {
	   std::cerr << "run test..." << std::endl;
	   reply = runThreads();
	   delete[] g_modelList;
   }

   return reply;
}

// class to keep track of the row/col to ground mapping.
class ImageGroundPoint
{
public:
   ImageGroundPoint()
   {
      _row = 0;
      _col = 0;
      _usrX = 0;
      _usrY = 0;
      _usrZ = 0;
      _line = 0;
      _sample = 0;
      _numAdjParams = 0;
      for (int i=0; i<6; i++)
         _partials[i] = 0;
   }

   ImageGroundPoint(const ImageGroundPoint &igp)
   {
      *this = igp;
   }

   double getRow() const { return _row; }
   double getCol() const { return _col; }

   double getX() const { return _usrX; }
   double getY() const { return _usrY; }
   double getZ() const { return _usrZ; }

   void setRowCol( double row, double col )
   {
      _row = row;
      _col = col;
   }

   void setLineSample( double line, double sample )
   {
      _line = line;
      _sample = sample;
   }

   void setPartials( vector<double> partials )
   {
      for (int i=0; i<6; i++)
         _partials[i] = partials[i];
   }

   void setPartials( double partials[6] )
   {
      for (int i=0; i<6; i++)
         _partials[i] = partials[i];
   }

   void setPartials(RasterGM::SensorPartials sp )
   {
        _linePartial.push_back(sp.first);
        _sampPartial.push_back(sp.second);
   }

   void setUSRmeters( double x, double y, double z )
   {
      _usrX = x;
      _usrY = y;
      _usrZ = z;
   }

   ImageGroundPoint& operator=(const ImageGroundPoint&  igp)
   {
      _row = igp._row;
      _col = igp._col;
      _usrX = igp._usrX;
      _usrY = igp._usrY;
      _usrZ = igp._usrZ;
      _line = igp._line;
      _sample = igp._sample;
	  _linePartial.clear();
      _sampPartial.clear();
      _numAdjParams = igp._numAdjParams;
      for (int i=0; i<igp._numAdjParams; i++)
      {
         _linePartial.push_back(igp._linePartial[i]);
         _sampPartial.push_back(igp._sampPartial[i]);
      }
      for (int j=0; j<6; j++)
         _partials[j] = igp._partials[j];
      return *this;
   }

   friend bool operator!=(const ImageGroundPoint& igp1, const ImageGroundPoint& igp2);
private:

   double _row, _col;
   double _usrX, _usrY, _usrZ;
   double _line, _sample;  // convert ground to image
   double _partials[6];
   int     _numAdjParams;
   vector<double> _linePartial;
   vector<double> _sampPartial;
};

bool operator!=(const ImageGroundPoint& igp1, const ImageGroundPoint& igp2)
{
   bool result1, result2;
   result1 = 
      (igp1._partials[0]    != igp2._partials[0]   ) ||
      (igp1._partials[1]    != igp2._partials[1]   ) ||
      (igp1._partials[2]    != igp2._partials[2]   ) ||
      (igp1._partials[3]    != igp2._partials[3]   ) ||
      (igp1._partials[4]    != igp2._partials[4]   ) ||
      (igp1._partials[5]    != igp2._partials[5]   );

   if (igp1._numAdjParams != igp2._numAdjParams)
      result2 = true;
   else
   {
      result2 = false;
      for (int i=0; i<igp1._numAdjParams; i++)
         result2 = result2 ||
            (igp1._linePartial[i] != igp2._linePartial[i]) ||
            (igp1._sampPartial[i] != igp2._sampPartial[i]) ;
   }

   return 
      result1 || result2 ||
      (igp1._row    != igp2._row   ) ||
      (igp1._col    != igp2._col   ) ||
      (igp1._line   != igp2._line  ) ||
      (igp1._sample != igp2._sample) ||
      (igp1._usrX   != igp2._usrX  ) ||
      (igp1._usrY   != igp2._usrY  ) ||
      (igp1._usrZ   != igp2._usrZ  );
}


// class that prepares and runs the image test
class ImageTest
{
public:
   ImageTest()
   {
      _numPoints = -1;
      _points = NULL;
   }

   ~ImageTest()
   {
      delete[] _points;
   }

   // initializes the test and return false if failed.
   // this function MUST be called before the threads are started.
   std::string intializeTestData(const ImageInfo imageInfo, int numberOfPoints, int imageTestIndex);

   // runs the test.
   bool runTest(int) const;

   int getNumberOfPoints() const { return _numPoints; }
   int getImageTestIndex() const { return _imageTestIndex; }
   const ImageGroundPoint& getPoint(int index) const { return _points[index]; }

private:

   std::string theFilename;
   int _imageIndex;
   char* _sensorName;
   ImageGroundPoint *_points;
   int _numPoints;
   int _imageTestIndex;

   ISDType _ISDFileType;
   Nitf20Isd *csmnitf20;
   Nitf21Isd *csmnitf21;
   Isd *csmfilename;
   //filenameISD *csmfilename;
   BytestreamIsd *csmbytestream;
   Isd *isd;

};


// loads the image, and gets the random image/ground points.
std::string ImageTest::intializeTestData(const ImageInfo imageInfo, int numberOfPoints, int imageTestIndex)
{
   std::string reply = "ok";

   RasterGM *model = NULL;
   EcefCoord groundPt;
   //double ref_x_usr, ref_y_usr, ref_z_usr;
   double achieved_accuracy;
   double* achieved_accuracyPtr;
   double x, y, z;
   int num_rows, num_cols;
   int row, col;
   int i;
   //Warning* csmWarn = NULL;

   WarningList warnings;

   theFilename = imageInfo.filename;
   _imageIndex = imageInfo.imageIndex;
   _ISDFileType = imageInfo.ISDFileType;
   _imageTestIndex = imageTestIndex;

   std::cerr << "theFilename = " << theFilename << std::endl;
   std::cerr << "_ISDFileType = " << ISDType2str(_ISDFileType) << std::endl;

   // Enter the plug-in name
   const Plugin* pluginPtr = Plugin::findPlugin(PluginName, &warnings);
   std::string myName("removePlugin");

   RasterGM* sensorPtr = NULL;
   csmnitf20 = NULL;
   csmnitf21 = NULL;
   csmfilename = NULL;
   isd = NULL;
   try
   {
       switch (_ISDFileType)
       {
          case NITF_20_TYPE:
                csmnitf20 = new Nitf20Isd(theFilename);
				if(_imageIndex > -1)
				{
                   initNitf20ISD(csmnitf20, theFilename.c_str(), _imageIndex, &warnings);
				}
				else
				{
                   initNitf20ISD(csmnitf20, theFilename.c_str(), -1, &warnings);
				}
                isd = csmnitf20;
                break;

          case NITF_21_TYPE:
                csmnitf21 = new Nitf21Isd(theFilename);
				if(_imageIndex > -1)
				{
                   initNitf21ISD(csmnitf21, theFilename.c_str(), _imageIndex, &warnings);
				}
				else
				{
                   initNitf21ISD(csmnitf21, theFilename.c_str(), -1, &warnings);
				}
                isd = csmnitf21;
                break;

          case BYTESTREAM_TYPE:
			    csmbytestream = new BytestreamIsd(theFilename);
                initBytestreamISD (csmbytestream,  theFilename.c_str());
                isd = csmbytestream ;
                break;

          case FILENAME_TYPE:
                csmfilename = new Isd(theFilename.c_str());
                isd = csmfilename;
                break;

          default:
				reply = "Warning: invalid file type:";
                reply += ISDType2str(_ISDFileType) + std::string(", ");
                reply += theFilename;
		}
   }
   catch (Error &ei)
   {
        reply = "ERROR: ";
        reply += theFilename + std::string(" ");
        reply += ISDType2str(_ISDFileType) + std::string(" ");
        reply += ei.getFunction() + " ";
        reply += ei.getMessage();
   }

   for (WarningList::iterator i = warnings.begin();i!=warnings.end();i++)
   {
		reply = "Warning: ";
		reply += theFilename + std::string(" #");
		reply += i->getWarning();
		reply += std::string(" ");
		reply += i->getFunction() + string(" ");
		reply += i->getMessage() + string("\n");
   }

   if(reply == "ok")
   {
       try
       {
          sensorPtr = (RasterGM*)pluginPtr->constructModelFromISD( *isd, SensorModelName, &warnings);
    	  g_modelList[imageTestIndex] = sensorPtr;
       }
       catch (Error &ei)
       {
          reply = "ERROR: ";
    	  reply += theFilename + std::string(" ");
    	  reply += ei.getFunction() + " ";
    	  reply += ei.getMessage();
       }
   }
   if(reply == "ok")
   {
	   model = g_modelList[imageTestIndex];
	   if (model == NULL)
	   {
		   reply = std::string("ERROR: ") + theFilename + std::string(" image 0 is NULL.");
		   std::cerr << "ERROR: " << theFilename << " image 0 is NULL." << std::endl;
	   }
   }
   if(reply == "ok")
   {
	   _numPoints = numberOfPoints;
	   _points = new ImageGroundPoint[numberOfPoints];

	   std::string tempStr = model->getModelName();

	   _sensorName = new char[tempStr.size()+1];
	   strcpy(_sensorName, tempStr.c_str());

	   //csmWarn = model->getReferencePoint(ref_x_usr, ref_y_usr, ref_z_usr);
       groundPt = model->getReferencePoint();

       ImageVector iv = model->getImageSize();
	   num_rows = iv.line;
	   num_cols = iv.samp;

	   double line, sample;
	   //double partials[6];
	   vector<double> partials;
	   // seed the random number, so that the same exact points are alway
	   // picked.
	   srand(g_randSeed);
	   for(i = 0;  i < _numPoints; i++)
	   {
		   row = rand() % num_rows;
		   col = rand() % num_cols;
		   _points[i].setRowCol(row,col);

	       groundPt = model->imageToGround(ImageCoord(row, col),
                        234.00,                         // hardcoded height=234 - DSL
						.001 , 
						&achieved_accuracy ,
						&warnings);

           _points[i].setUSRmeters(groundPt.x,groundPt.y,groundPt.z);

		   ImageCoord ic = model->groundToImage(groundPt,0.001,&achieved_accuracy ,&warnings);
		   //model->groundToImage(x, y, z,
			  // line, sample, achieved_accuracy);
		   _points[i].setLineSample(ic.line, ic.samp);

		   
		   partials = model->computeGroundPartials(groundPt);
		   _points[i].setPartials(partials);

		   int   numParams;
		   numParams = model->getNumParameters();
		   double *linePartial = new double [numParams];
		   double *sampPartial = new double [numParams];
		   for( int j = 0; j < numParams; j++ )
		   {
		      RasterGM::SensorPartials sp = model->computeSensorPartials(
				       j, 
				       groundPt,
				       0.001, 
				       &achieved_accuracy, 
				       &warnings);
		      _points[i].setPartials(sp);
		   }
	   }
   }

   return reply;
}

// runs the test to see if multithreaded effects anything.
bool ImageTest::runTest(int threadNumber) const
{
   bool ok = true;
   std::cout << "ImageTest::runTest: threadNumber: " << threadNumber << std::endl;
   RasterGM *model = NULL;
   RasterGM *modelList = NULL;
   ImageGroundPoint point;
   EcefCoord groundPt;
   //double ref_x_usr, ref_y_usr, ref_z_usr;
   double achieved_accuracy;
   ///double* achieved_accuracyPtr;
   double x = 0, y = 0, z = 0;
   double line, sample;
   vector<double> partials;
   //double partials[6];
   int i;
   int imageTestIndex = getImageTestIndex();
   WarningList warnings;

   // Enter the plug-in name
   const Plugin* pluginPtr = Plugin::findPlugin(PluginName, &warnings);
   RasterGM* sensorPtr = NULL;
   
   if (g_oneSMPerThread)
   {
      try
      {
        sensorPtr = (RasterGM*)pluginPtr->constructModelFromISD( *isd, SensorModelName, &warnings);
		modelList = sensorPtr;
      }
      catch (Error &ei)
      {
         std::cerr << "THREAD ERROR: createSensorModelFromFile " 
            << theFilename << std::endl
            << (ei.getFunction()).c_str() << std::endl
            << (ei.getMessage() ).c_str() << std::endl;
         ok = false;
      }

      model = modelList;
   }
   else
   {
      model = g_modelList[imageTestIndex];
   }


   if (model == NULL)
   {
      std::cerr << "THREAD ERROR: no model " << theFilename << std::endl;
	  ok = false;
   }
   std::cout << "threadNumber: " << threadNumber << std::endl;
   //model->getReferencePoint(ref_x_usr, ref_y_usr, ref_z_usr);
   groundPt = model->getReferencePoint();

   for(i = 0;  i < _numPoints; i++)
   {
	   std::cout << "point: " << i << " of thread: " << threadNumber << std::endl;
	   point = _points[i];

	   groundPt = model->imageToGround(ImageCoord(point.getRow(), point.getCol()),
                                   234.00,                         // hardcoded height=234 - DSL
								   0.001,
								   &achieved_accuracy, &warnings);

	   point.setUSRmeters(groundPt.x,groundPt.y,groundPt.z);
	   if( point != _points[i] )
	   {
		   std::cerr << "THREAD ERROR: Image to Ground points don't match " 
			   << imageTestIndex << " " << _sensorName << std::endl;
		   ok = false;
	   }

	   if(ok)
	   {
		   ImageCoord ic = model->groundToImage(groundPt,0.001,&achieved_accuracy, &warnings);
		   point.setLineSample( ic.line, ic.samp );
		   if( point != _points[i] )
		   {
			   std::cerr << "THREAD ERROR: Ground to Image points don't match " 
				   << imageTestIndex << " " << _sensorName << std::endl;
			   ok = false;
		   }
	   }

	   if(ok)
	   {
		   groundPt = EcefCoord(point.getX(), point.getY(), point.getZ());
		   partials = model->computeGroundPartials(groundPt);
		   point.setPartials( partials );
		   if( point != _points[i] )
		   {
			   std::cerr << "THREAD ERROR: ground partials don't match " 
				   << imageTestIndex << " " << _sensorName << std::endl;
			   ok = false;
		   }
	   }

	   if(ok)
	   {
		   int   numParams;
		   numParams = model->getNumParameters();
		   double *linePartial = new double [numParams];
		   double *sampPartial = new double [numParams];

		   for( int j = 0; j < numParams; j++ )
		   {
		      RasterGM::SensorPartials sp = model->computeSensorPartials(
				       j, 
				       groundPt,
				       0.001, 
				       &achieved_accuracy, 
				       &warnings);
		      point.setPartials(sp);
		   }

		   if( point != _points[i] )
		   {
			   std::cerr << "THREAD ERROR: sensor partials don't match " 
				   << imageTestIndex << " " << _sensorName << std::endl;
			   ok = false;
		   }
	   }
	   std::cout << "finished point: " << i << " of thread: " << threadNumber << std::endl;
   }
   std::cout << "finished with threadNumber: " << threadNumber << std::endl;
   return ok;
}

// make the list of test data.
std::string initializeTestData()
{
   std::string ok;
   std::list<ImageInfo>::iterator iter;

   g_numberOfImageTests = 0;
   g_testArray = new ImageTest[imageList.size()];

   iter = imageList.begin();
   while(iter != imageList.end())
   {
      ok = g_testArray[g_numberOfImageTests]
      .intializeTestData( *iter, g_numberOfTestPoints, g_numberOfImageTests );

      if(ok == "ok")
      {
         g_numberOfImageTests++;
      }
	  else
	  {
		  break;
	  }
      *iter++;
   }
   return ok;
}


// thread that does the work
#ifdef _WIN32
void runTestThread (LPVOID pvParam)
{
   bool ok = true;
   long tn = (long)pvParam;
#else
extern "C" void * runTestThread (void *pArg)
{
   long tn = (long)pArg;
   bool ok = true;
#endif
   int threadNumber = (int)tn;
   threadStatus[threadNumber] = true;
   int runNumber;
   int testNumber;

   std::cout << "runTestThread: threadNumber: " << threadNumber << std::endl;
   for( runNumber = 0; runNumber < g_numberOfTestRuns; runNumber++ )
   {
      for( testNumber = 0; testNumber < g_numberOfImageTests; testNumber++ )
      {
         ok = g_testArray[testNumber].runTest(threadNumber);
		 if(!ok)
		 {
			 threadStatus[threadNumber] = false;
			 break;
		 }
      }
	  if(!ok)
	  {
		  break;
	  }
   }
#ifndef _WIN32
   return 0;
#endif
}


// starts all of the threads, then wait for them to die.
std::string runThreads()
{
   std::string reply = "ok";
   threadStatus = new bool[g_numThreads];
#ifdef _WIN32
   int i;
   HANDLE* threads;
   threads = new HANDLE[g_numThreads];
   //HANDLE   threads[MAXNUMTHREADS];
   DWORD    threadID;

   for(i = 0; i < g_numThreads; i++)
   {
      std::cerr << "Starting Thread... " << i << std::endl;
      threads[i] = CreateThread( 0, 0,
         (LPTHREAD_START_ROUTINE) runTestThread, (void*)i, 0, &threadID);
      if ( threads[i]==0 )
	  {
         char tempStr[10] = "";
         sprintf (tempStr, "%d", i);
         reply = "Cannot create thread #";
		 reply += tempStr;
         std::cerr << "Cannot create thread #" << i 
         << " Error#: " << GetLastError() << std::endl;
		 break;
	  }
   }

   std::cerr << "Wait for all threads to finish." << std::endl;
   // wait for all of the threads to die.
   WaitForMultipleObjects( g_numThreads, threads, TRUE, INFINITE);

#else
   pthread_attr_t attr;
   int i;
   void *status;
   pthread_t threads[MAXNUMTHREADS];

   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
   for(i = 0; i < g_numThreads; i++)
   {
      std::cerr << "Starting Thread... " << i << std::endl;
	  pthread_create( &threads[i], &attr, runTestThread, (void*)i );
   }
   pthread_attr_destroy(&attr);

   // wait for all of the threads to die.
   for(i = 0; i < g_numThreads; i++)
   {
      pthread_join( threads[i], &status );
   }

#endif

   std::cerr << "All threads finished." << std::endl;

   if(reply == "ok")
   {
	   for(i = 0; i < g_numThreads; i++)
	   {
		   if(!threadStatus[i])
		   {
			   char tempStr[100] = "";
			   sprintf (tempStr, "Failure - thread # %d", i);
			   reply = tempStr;
			   break;
		   }
	   }
   }

   // clean up memory allocated in initializeTestData()
   delete[] g_testArray;
   delete[] threadStatus;

	   if(reply == "ok")
   {
	   reply = "All threads finished.";
   }
   return reply;
}

std::string findAttribute(TiXmlNode* rootnode, std::string nodename, std::string attrname)
{
	std::string ret;
	TiXmlElement* element = 0;
	TiXmlAttribute* attr = 0;

	TiXmlNode* node = findNode(rootnode, nodename);
	element = node->ToElement();
	for (attr = element->FirstAttribute(); attr; attr = attr->Next())
	{
		std::string thisAttrName = attr->Name();
		if(thisAttrName == attrname)
		{
			ret = attr->Value();
			break;
		}
	}
	return ret;
}

//return a list of nodes from siblings with nodename = nname.  The returned list will 
//   contain another list of attributes for that node
std::list<std::list<std::string> > findNodesAttributes(TiXmlNode* rootnode, std::string nname)
{
	std::string attrval = "";
	std::string nodeval = "";
	TiXmlElement* element = 0;
	TiXmlAttribute* attr = 0;
	std::list<std::list<std::string> > retlist;
	TiXmlNode* node = findNode(rootnode, nname);
	for (node;node;node = node->NextSiblingElement())
	{
		std::string thisNodeName = node->Value();
		if(thisNodeName == nname)
		{
            std::list<std::string> arglist;
            TiXmlNode* chnode = node->FirstChild();
			std::string nodeval = (char*)chnode->Value();
            arglist.push_back(nodeval);

            element = node->ToElement();
            for (attr = element->FirstAttribute(); attr; attr = attr->Next())
            {
            	std::string thisAttrName = attr->Name();
            	std::string thisAttrValue = attr->Value();
                arglist.push_back(thisAttrName);
                arglist.push_back(thisAttrValue);
            }
			retlist.push_back(arglist);
		}
	}
	return retlist;
}

//return a list of values from siblings with nodename = nname
std::list<std::string> findNodes(TiXmlNode* rootnode, std::string nname)
{
	std::list<std::string> list;
	TiXmlNode* node = findNode(rootnode, nname);
	for (node;node;node = node->NextSiblingElement())
	{
		std::string thisNodeName = node->Value();
		if(thisNodeName == nname)
		{
            TiXmlNode* chnode = node->FirstChild();
			std::string val = (char*)chnode->Value();
			list.push_back(val);
		}
	}
	return list;
}


TiXmlNode* findNode(TiXmlNode*  pnode, std::string nname)
{
	// called recursively
	// return:  0 - not found
	//         !0 - found
	TiXmlNode* node;
	bool found = false;

	for (node = pnode->FirstChild();node;node = node->NextSiblingElement())
	{
		// if this node matches nname, return node
		std::string chname = node->Value();
		if(chname == nname)
		{
			found = true;
		}
		else
		{
			// if child node matches, return child node
			if(TiXmlNode* chnode = findNode(node, nname))
			{
				node = chnode;
				found = true;
			}
		}
		if(found)
		{
			break;
		}
	}
	return node;
}

ISDType string2ISDType(std::string fileType)
{
   ISDType ISDFileType = NO_FILE_TYPE;

   if(fileType == "BYTESTREAM_TYPE")
   {
      ISDFileType = BYTESTREAM_TYPE;
   }
   else if(fileType == "NITF_20_TYPE")
   {
      ISDFileType = NITF_20_TYPE;
   }
   else if(fileType == "NITF_21_TYPE")
   {
      ISDFileType = NITF_21_TYPE;
   }
   else if(fileType == "FILENAME_TYPE")
   {
      ISDFileType = FILENAME_TYPE;
   }
   else if(fileType == "")
   {
      ISDFileType = FILENAME_TYPE;  //   default ISD file type
   }
   return ISDFileType;
}

std::string getImageInfo(std::list<std::string>attrs, std::string attrname)
{
    std::string attrval = "";
    std::string aname;
    std::list<std::string>::iterator iter;

	for(iter=attrs.begin();iter != attrs.end();iter++)
	{
       aname = *iter;
	   if(aname == attrname)
	   {
          attrval = *(++iter);
          break;
       }
	}
	return attrval;
}

std::string getTestParams(TiXmlNode* node)
{
	std::string reply = "ok";
	std::string val;
	int ival;
	std::string txt;

	txt = "PluginName";
	val = findAttribute(node, "Model", txt);
	PluginName = val;

	txt = "SensorModelName";
	val = findAttribute(node, "Model", txt);
    SensorModelName = val;

	txt = "threads";
	val = findAttribute(node, "Test", txt);
	ival = atoi(val.c_str());
	g_numThreads = ival;

	txt = "oneSMPerThread";
	val = findAttribute(node, "Test", txt);
	if(val == "true")
	{
		g_oneSMPerThread = true;
	}
	else
	{
		g_oneSMPerThread = false;
	}

	txt = "points";
	val = findAttribute(node, "Test", txt);
	ival = atoi(val.c_str());
	g_numberOfTestPoints = ival;

	txt = "runs";
	val = findAttribute(node, "Test", txt);
	ival = atoi(val.c_str());
	g_numberOfTestRuns = ival;

	std::list<std::list<std::string> > strlist = findNodesAttributes(node, "image");

	std::list<std::list<std::string> >::iterator iter1;
	std::list<std::string>::iterator iter2;

    iter1 = strlist.begin();
    while(iter1 != strlist.end())
    {
        ImageInfo imageInfo;
		iter2 = (*iter1).begin();
	    imageInfo.filename = *iter2;

        std::string fileType = getImageInfo(*iter1, "ISDFileType");
        imageInfo.ISDFileType = string2ISDType(fileType);
		if(imageInfo.ISDFileType == NO_FILE_TYPE)
		{
			reply = "Warning: invalid file type:";
                reply += fileType + std::string(", ");
                reply += imageInfo.filename;
		}

        std::string indx = getImageInfo(*iter1, "imageIndex");
        if(indx == "")
		{
			imageInfo.imageIndex = -1;
		}
		else
		{
			imageInfo.imageIndex = atoi(indx.c_str());
		}

	    imageList.push_back(imageInfo);
	    iter1++;
    }
	g_modelListLength = imageList.size();
	return reply;
}

string parseConfigFile(std::string filename)
{
	TiXmlNode* rootNode = 0;

	string reply = "ok";
	bool ok = true;
	ok = xmldoc.LoadFile(filename);
	if (ok)
	{
		rootNode = xmldoc.FirstChild( "ThreadTest" );
	}
	else
	{
		ok = false;
		reply = xmldoc.ErrorDesc();
	}
	if (ok)
	{
		reply = getTestParams(rootNode);
	}

    return reply;

}

