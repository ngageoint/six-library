#include <typeinfo>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

#include <sys/Path.h>

#include "VTSMisc.h"
#include "Error.h"
#include "Warning.h"

using namespace csm; 

extern int termEcho;
extern bool saveLogFile;
extern long instructionCount;
extern clock_t start_clock;
extern clock_t end_clock;
std::string serialNum;
//char serialNum[MAX_NAME_LENGTH] = "";
std::string timedata;
//char timedata[MAX_NAME_LENGTH] = "";

static void open(std::ofstream& tfile, std::string& datafile)
{
    constexpr auto mode = std::ios::app;
    tfile.open(datafile.c_str(), mode);
    if (!tfile.is_open())
    {
        // assume the name contains enviroment variables that need to be expanded
        auto pathname = sys::Path::expandEnvironmentVariables(datafile, false /*checkIfExists*/);
        tfile.open(pathname.c_str(), mode);
        if (tfile.is_open())
        {
            // Use this path from now on.  Regenerating it every time
            // could result in a new filename for ${EPOCHSECONDS}
            datafile = std::move(pathname);
        }
    }
}

// recordLog I
  /*   Variable parameter definitions used: 

       datafile- File and path where the logging file can be
       locateed/stored.  If the file exists, it will be appended, if
       it does not exist, it will be created.

       comment- Comment field to add to the log.  A string pointer value.

	   command- The name of the command that was issued.
	   
	   param_array- Array used to run the test.  This IS the command line
	   used when one of the stubs is called. (Input parameters, col 4-83 in
       spreadsheet)

       out_param_array- This is the output data to be recorded.  Size
       and breadth determined by constants MAXOUTPUTPARAMETERS*3 and
       MAX_OUTPARAM_LENGTH)

       The file data written is in the same format and sequence as
       that of the function call.  Uninitialized output parameters are
       denoted by two consecutive commas (eg ,,).  Data imported in
       this manner results in an empty cell in the spreadsheet. 

	   Global Variables:
         instructionCount
         start_clock
         end_clock
*/
// recordLog I
void recordLog(std::string& datafile,
               std::string comment,
               std::string command,         
               std::vector<std::string> param_array,
               std::vector<std::string> out_param_array)
{
  int i;

  char tmpstr[100];

  //sprintf ((char*)serialNum, "%u", (unsigned int)instructionCount);
  sprintf (tmpstr, "%u", (unsigned int)instructionCount);
  serialNum = tmpstr;
  //sprintf ((char*)timedata, "%u,%u", (unsigned int)start_clock, (unsigned int)end_clock);
  sprintf (tmpstr, "%u,%u", (unsigned int)start_clock, (unsigned int)end_clock);
  timedata = tmpstr;

  std::ofstream tfile;
  if(saveLogFile)
  {
	  open(tfile, datafile);
	  tfile << timedata << "," 
		<< serialNum << "," 
		<< comment << "," 
		<< command << "," ;
  }
  if (termEcho == 1)
    std::cout << std::endl 
	      << timedata << "," 
	      << serialNum << "," 
	      << comment << "," << command  << "," ;
	
  if (termEcho == 1)  std::cout << "\nInput Param:" << std::endl ; 

  for (i=0; i < MAXINPUTPARAMETERS; i++)
  {
    // Test if we have data to output from input
    if ( i < (int)param_array.size() )
    { 
      // Yes, do it
      if(saveLogFile)tfile << param_array[i] ;
      if (termEcho == 1)  std::cout << param_array[i] << " ";
      //if (i < MAXINPUTPARAMETERS) tfile  << ","; 
      if(saveLogFile)tfile  << ","; 
    }
    else
    {
      // no, just put the comma in
      //if (termEcho >= 1)  std::cout  << ",";
      //if (i < MAXINPUTPARAMETERS) tfile  << ",";
      if(saveLogFile)tfile  << ",";
    }
  }
  if (termEcho >= 1)  std::cout << "\nOutput Results:" << std::endl;
  for (i=0; i < (MAXOUTPUTPARAMETERS*3); i++)
  {  
    // Test if we have data to output from the output results
    if ( i < (int)out_param_array.size() )
    {
      // Yes, do it
      if(saveLogFile) tfile << out_param_array[i] ; 
      if (termEcho >= 1)  std::cout << out_param_array[i] << " ";
      //if (i < MAXOUTPUTPARAMETERS*3) tfile  << ","; 
      if(saveLogFile) tfile  << ","; 
    }
    else
    {
      // no just put in the commas
      //if (termEcho >= 1)  std::cout  << ",";
      //if (i < (MAXOUTPUTPARAMETERS*3)) tfile  << ","; 
      if(saveLogFile) tfile  << ","; 
    }
  }

  if(saveLogFile)
  {
    tfile << std::endl;
    tfile.close();
  }
  if (termEcho >= 1)
  {
	  std::cout << std::endl;
  }
  return;
}

// recordLog II
  /*   Variable parameter definitions used: 

       datafile- File and path where the logging file can be
       locateed/stored.  If the file exists, it will be appended, if
       it does not exist, it will be created.

       serialNum-Serial number arbitrarily assigned by caller.
	   This is a string pointer, so format is variable.
       
	   timedata- Time (in ASCII or String format) passed by the caller.

       comment- Comment field to add to the log.  A string pointer value.

	   command- The name of the command that was issued.  
	   
	   param_array- Array used to run the test.  This IS the command line
	   used when one of the stubs is called. (Input parameters, col 4-83 in
       spreadsheet)

       out_param_array- This is the output data to be recorded.  Size
       and breadth determined by constants MAXOUTPUTPARAMETERS*3 and
       MAX_OUTPARAM_LENGTH)

       The file data written is in the same format and sequence as
       that of the function call.  Uninitialized output parameters are
       denoted by two consecutive commas (eg ,,).  Data imported in
       this manner results in an empty cell in the spreadsheet. 

	   Global Variables:
         instructionCount
         start_clock
         end_clock
*/
// recordLog II
void recordLog(std::string& datafile,
               std::string serialNum,
               std::string timedata,
               std::string comment,
               std::string command,
               std::vector<std::string> param_array,
               std::vector<std::string> out_param_array)
{
  int i;

  std::ofstream tfile;
  if(saveLogFile)
  {
	  open(tfile, datafile);
	  tfile << timedata << "," 
		<< serialNum << "," 
		<< comment << "," 
		<< command << "," ;
  }
  if (termEcho == 1)
    std::cout << std::endl 
	      << timedata << "," 
	      << serialNum << "," 
	      << comment << "," << command  << "," ;
	
  if (termEcho == 1)  std::cout << "\nInput Param:" << std::endl ; 

  for (i=0; i < MAXINPUTPARAMETERS; i++)
  {
    // Test if we have data to output from input
    if ( i < (int)param_array.size() )
    { 
      // Yes, do it
      if(saveLogFile)tfile << param_array[i] ;
      if (termEcho == 1)  std::cout << param_array[i] << " ";
      //if (i < MAXINPUTPARAMETERS) tfile  << ","; 
      if(saveLogFile)tfile  << ","; 
    }
    else
    {
      // no, just put the comma in
      //if (termEcho >= 1)  std::cout  << ",";
      //if (i < MAXINPUTPARAMETERS) tfile  << ",";
      if(saveLogFile)tfile  << ",";
    }
  }
  if (termEcho >= 1)  std::cout << "\nOutput Results:" << std::endl;
  for (i=0; i < (MAXOUTPUTPARAMETERS*3); i++)
  {  
    // Test if we have data to output from the output results
    if ( i < (int)out_param_array.size() )
    {
      // Yes, do it
      if(saveLogFile) tfile << out_param_array[i] ; 
      if (termEcho >= 1)  std::cout << out_param_array[i] << " ";
      //if (i < MAXOUTPUTPARAMETERS*3) tfile  << ","; 
      if(saveLogFile) tfile  << ","; 
    }
    else
    {
      // no just put in the commas
      //if (termEcho >= 1)  std::cout  << ",";
      //if (i < (MAXOUTPUTPARAMETERS*3)) tfile  << ","; 
      if(saveLogFile) tfile  << ","; 
    }
  }

  if(saveLogFile)
  {
    tfile << std::endl;
    tfile.close();
  }
  if (termEcho >= 1)
  {
	  std::cout << std::endl;
  }
  return;
}

// recordLog III
  /*   Variable parameter definitions used: 

       datafile- File and path where the logging file can be
       locateed/stored.  If the file exists, it will be appended, if
       it does not exist, it will be created.

       comment- Comment field to add to the log.  A string pointer value.

	   command- The name of the command that was issued.

	   text- Free form text field.  A string pointer value.

       The file data written is in the same format and sequence as
       that of the function call.  Uninitialized output parameters are
       denoted by two consecutive commas (eg ,,).  Data imported in
       this manner results in an empty cell in the spreadsheet.

	   Global Variables:
         instructionCount
         start_clock
         end_clock
  */
// recordLog III
void recordLog(std::string& datafile,
               std::string comment,
               std::string command,
               std::string text)
{

  char tmpstr[100];

  //sprintf ((char*)serialNum, "%u", (unsigned int)instructionCount);
  sprintf (tmpstr, "%u", (unsigned int)instructionCount);
  serialNum = tmpstr;
  //sprintf ((char*)timedata, "%u,%u", (unsigned int)start_clock, (unsigned int)end_clock);
  sprintf (tmpstr, "%u,%u", (unsigned int)start_clock, (unsigned int)end_clock);
  timedata = tmpstr;
  std::ofstream tfile;
  if(saveLogFile)
  {
    open(tfile, datafile);
    tfile << timedata << "," << serialNum << "," << comment << "," 
          << command << "," << text ;
    tfile << std::endl;
    tfile.close();
  }
  
  if (termEcho == 1)
  {
    std::cout << std::endl;
    std::cout << timedata << ","  << serialNum << "," << comment << "," 
	      << command << "," << text ;
    std::cout << std::endl;
  }

  if (termEcho > 1)
  { 
   std::cout << std::endl;
   std::cout <<  comment << ",  " << command << ",  " << text ;
   std::cout << std::endl;
  }
}

// recordLog IV
  /*   Variable parameter definitions used: 

       datafile- File and path where the logging file can be
       locateed/stored.  If the file exists, it will be appended, if
       it does not exist, it will be created.

       serialNum-Serial number arbitrarily assigned by caller.
	   This is a string pointer, so format is variable.
       
	   timedata- Time (in ASCII or String format) passed by the caller.

       comment- Comment field to add to the log.  A string pointer value.

	   command- The name of the command that was issued.
	   
	   param_array- Array used to run the test.  This IS the command line
	   used when one of the stubs is called. (Input parameters, col 4-83 in
       spreadsheet)

	   text- Free form text field.  A string pointer value.

       The file data written is in the same format and sequence as
       that of the function call.  Uninitialized output parameters are
       denoted by two consecutive commas (eg ,,).  Data imported in
       this manner results in an empty cell in the spreadsheet. 

	   Global Variables:
         instructionCount
         start_clock
         end_clock
*/
// recordLog IV
void recordLog(std::string& datafile,
               std::string serialNum,
               std::string timedata,
               std::string comment,
               std::string command,
               std::vector<std::string> param_array,
               std::string text)
{
  int i;

  std::ofstream tfile;
  if(saveLogFile)
  {
    open(tfile, datafile);
    tfile << timedata << "," << serialNum << "," << comment << "," << command 
          << ",";
  }

  for (i=0; i <= MAXINPUTPARAMETERS-1; i++)
  {
    // Test if we have data to output from input
    if ( i < (int)param_array.size() )
    {
      // Yes, do it
      if(saveLogFile) tfile << param_array[i] ;
      if (termEcho == 1)  std::cout << param_array[i] << " ";
      if(saveLogFile)if (i <= MAXINPUTPARAMETERS-1) tfile  << ",";
    }
    else
    {
      // no, just put the comma in
      //if (termEcho >= 1)  std::cout  << ",";
      if(saveLogFile)if (i <= MAXINPUTPARAMETERS-1) tfile  << ",";
    }
  }

  if(saveLogFile)tfile << text ;

  // It is assumed that there is a -c and one test pram in test 
  if(saveLogFile)
  {
    for (i=1; i <= MAXINPUTPARAMETERS-1; i++) // we
    {
        if (i <= MAXINPUTPARAMETERS-1) tfile  << ",";
    }

    tfile << std::endl;
    tfile.close();
  }
 
  if (termEcho == 1)
  {
    std::cout << timedata << ","  << serialNum << "," << comment << "," 
	      << command;

    for (i=0; i <= MAXINPUTPARAMETERS-1; i++)
    {
    // Test if we have data to output from input
       if ( i < (int)param_array.size() )
       {
         // Yes, do it
         std::cout << param_array[i] ;
         if (i <= MAXINPUTPARAMETERS-1) std::cout  << ",";
       }
    }
    std::cout << text ;
    std::cout << std::endl;
  }

  if (termEcho > 1)
  {
    std::cout <<  comment << ",  " << command << ",  ";

    for (i=0; i <= MAXINPUTPARAMETERS-1; i++)
    {
    // Test if we have data to output from input
       if ( i < (int)param_array.size() )
       {
         // Yes, do it
         std::cout << param_array[i] ;
         if (i <= MAXINPUTPARAMETERS-1) std::cout  << ",";
       }
    }

    std::cout << text ;
    std::cout << std::endl;
  }

  return;
}

// recordLog V
  /*   Variable parameter definitions used: 

       datafile- File and path where the logging file can be
       locateed/stored.  If the file exists, it will be appended, if
       it does not exist, it will be created.

       comment- Comment field to add to the log.  A string pointer value.

	   text- Free form text field.  A string pointer value.

       The file data written is in the same format and sequence as
       that of the function call.  Uninitialized output parameters are
       denoted by two consecutive commas (eg ,,).  Data imported in
       this manner results in an empty cell in the spreadsheet.
*/
// recordLog V
void recordLog(std::string& datafile,
               std::string comment,
               std::string text)
{
  std::ofstream tfile;
  if(saveLogFile)
  {
    open(tfile, datafile);
    tfile <<  ",,," << comment << "," << text << std::endl; 
    tfile.close();
  }
  if (termEcho >= 1) 
    std::cout << std::endl << comment << " - " << text << std::endl;
  return;
}

// recordLog VI
  /*   Variable parameter definitions used:

       datafile- File and path where the logging file can be
       locateed/stored.  If the file exists, it will be appended, if
       it does not exist, it will be created.

       comment- Comment field to add to the log.  A string pointer value.

	   text- Free form text field.  A string pointer value.

       The file data written is in the same format and sequence as
       that of the function call.  Uninitialized output parameters are
       denoted by two consecutive commas (eg ,,).  Data imported in
       this manner results in an empty cell in the spreadsheet. */
// recordLog VI
void recordLog(std::string& datafile,
               const std::string text)
{
  std::ofstream tfile;
  if(saveLogFile)
  {
    open(tfile, datafile);
    tfile << text << std::endl;
    tfile.close();
  }
  if (termEcho >= 1) 
    std::cout << text << std::endl;
  return;
}

void echo2Term(std::string comment,
               std::string command,
               std::string text)
{
  char tmpstr[100];

  //sprintf ((char*)serialNum, "%u", (unsigned int)instructionCount);
  sprintf (tmpstr, "%u", (unsigned int)instructionCount);
  serialNum = tmpstr;
  //sprintf ((char*)timedata, "%u,%u", (unsigned int)start_clock, (unsigned int)end_clock);
  sprintf (tmpstr, "%u,%u", (unsigned int)start_clock, (unsigned int)end_clock);
  timedata = tmpstr;
	
  if (termEcho == 1)
  {
    std::cout << std::endl;
    std::cout << timedata << ","  << serialNum << "," << comment << "," 
	      << command << "," << text ;
    std::cout << std::endl;
  }

  if (termEcho > 1)
  { 
   std::cout << std::endl;
   std::cout <<  comment << ",  " << command << ",  " << text ;
   std::cout << std::endl;
  }
}

