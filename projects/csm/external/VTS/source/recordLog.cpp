#include <typeinfo>
#include <iostream>
#include <fstream>
#include <string>
#include "VTSMisc.h"
#include "TSMError.h"
#include "TSMWarning.h"

extern int termEcho;

  /*   Variable parameter definitions used: 

       datafile- File and path where the logging file can be
       locateed/stored.  If the file exists, it will be appended, if
       it does not exist, it will be created.

       serialNum-Serial number arbitrarily assigned by caller.  This
       is a string pointer, so format is variable.
       
       timedata- Time (in ASCII or String format) passed by the
       caller.  

       comment- Comment field to add to the log.  Like serialNum,
       string pointer value.

       command- The name of the command that was issued.  param_array-
       Array used to run the test.  This IS the command line used when
       one of the stubs is called. (Input parameters, col 4-83 in
       spreadsheet)

       out_param_array- This is the output data to be recorded.  Size
       and breadth determined by constants MAXOUTPUTPARAMETERS*3 and
       MAX_OUTPARAM_LENGTH)

       The file data written is in the same format and sequence as
       that of the function call.  Uninitialized output parameters are
       denoted by two consecutive commas (eg ,,).  Data imported in
       this manner results in an empty cell in the spreadsheet. 
*/

// recordLog II
void recordLog(std::string datafile,
               char* serialNum,
               char* timedata,
               std::string comment,
               std::string command,         
	       std::vector<std::string> param_array,
               std::vector<std::string> out_param_array)
{
  int i;

  std::ofstream tfile(datafile.c_str(), std::ios::app);
  tfile << timedata << "," 
	<< serialNum << "," 
	<< comment << "," 
	<< command << "," ;

  if (termEcho == 1)
    std::cout << std::endl 
	      << timedata << "," 
	      << serialNum << "," 
	      << comment << "," << command  << "," ;
	
  if (termEcho == 1)  std::cout << "\nInput Param:" << std::endl ; 

  for (i=0; i < MAXINPUTPARAMETERS; i++)
  {
    // Test if we have data to output from input
    if ( i < param_array.size() )
    { 
      // Yes, do it
      tfile << param_array[i] ;
      if (termEcho == 1)  std::cout << param_array[i] << " ";
      //if (i < MAXINPUTPARAMETERS) tfile  << ","; 
      tfile  << ","; 
    }
    else
    {
      // no, just put the comma in
      //if (termEcho >= 1)  std::cout  << ",";
      //if (i < MAXINPUTPARAMETERS) tfile  << ",";
      tfile  << ",";
    }
  }
  if (termEcho >= 1)  std::cout << "\nOutput Results:" << std::endl;
  for (i=0; i < (MAXOUTPUTPARAMETERS*3); i++)
  {  
    // Test if we have data to output from the output results
    if ( i < out_param_array.size() )
    {
      // Yes, do it
      tfile << out_param_array[i] ; 
      if (termEcho >= 1)  std::cout << out_param_array[i] << " ";
      //if (i < MAXOUTPUTPARAMETERS*3) tfile  << ","; 
      tfile  << ","; 
    }
    else
    {
      // no just put in the commas
      //if (termEcho >= 1)  std::cout  << ",";
      //if (i < (MAXOUTPUTPARAMETERS*3)) tfile  << ","; 
      tfile  << ","; 
    }
  }

  tfile << std::endl;
  tfile.close();

  return;
}

  /*   Variable parameter definitions used: datafile- File and path
       where the logging file can be locateed/stored.  If the file
       exists, it will be appended, if it does not exist, it will be
       created.  serialNum-Serial number arbitrarily assigned by
       caller.  This is a string pointer, so format is variable.
       timedata- Time (in ASCII or String format) passed by the
       caller.  comment- Comment field to add to the log.  A string
       pointer value.  command- The name of the command that was
       issued.  text- Free form text field.  A string pointer value.

       The file data written is in the same format and sequence as
       that of the function call.  Uninitialized output parameters are
       denoted by two consecutive commas (eg ,,).  Data imported in
       this manner results in an empty cell in the spreadsheet. */

// recordLog IV
void recordLog(std::string datafile,
               char* serialNum,
               char* timedata,
               std::string comment,
               std::string command,
	       std::string text) 
{
  std::ofstream tfile(datafile.c_str(), std::ios::app);
  tfile << timedata << "," << serialNum << "," << comment << "," 
	<< command << "," << text ;
  tfile << std::endl;
  tfile.close();
  
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

// recordLog VI
void recordLog(std::string datafile,
               char* serialNum,
               char* timedata,
               std::string comment,
               std::string command,
               std::vector<std::string> param_array,
               std::string text)

  /*   Variable parameter definitions used: datafile- File and path
       where the logging file can be locateed/stored.  If the file
       exists, it will be appended, if it does not exist, it will be
       created.  serialNum-Serial number arbitrarily assigned by
       caller.  This is a string pointer, so format is variable.
       timedata- Time (in ASCII or String format) passed by the
       caller.  comment- Comment field to add to the log.  A string
       pointer value.  command- The name of the command that was
       issued.  text- Free form text field.  A string pointer value.

       The file data written is in the same format and sequence as
       that of the function call.  Uninitialized output parameters are
       denoted by two consecutive commas (eg ,,).  Data imported in
       this manner results in an empty cell in the spreadsheet. */

{
  int i;

  std::ofstream tfile(datafile.c_str(), std::ios::app);
  tfile << timedata << "," << serialNum << "," << comment << "," << command 
	<< ",";

  for (i=0; i <= MAXINPUTPARAMETERS-1; i++)
  {
    // Test if we have data to output from input
    if ( i < param_array.size() )
    {
      // Yes, do it
      tfile << param_array[i] ;
      if (termEcho == 1)  std::cout << param_array[i] << " ";
      if (i <= MAXINPUTPARAMETERS-1) tfile  << ",";
    }
    else
    {
      // no, just put the comma in
      //if (termEcho >= 1)  std::cout  << ",";
      if (i <= MAXINPUTPARAMETERS-1) tfile  << ",";
    }
  }

  tfile << text ;

  // It is assumed that there is a -c and one test pram in test 
  for (i=1; i <= MAXINPUTPARAMETERS-1; i++) // we
  {
      if (i <= MAXINPUTPARAMETERS-1) tfile  << ",";
  }

  tfile << std::endl;
  tfile.close();
 
  if (termEcho == 1)
  {
    std::cout << timedata << ","  << serialNum << "," << comment << "," 
	      << command;

    for (i=0; i <= MAXINPUTPARAMETERS-1; i++)
    {
    // Test if we have data to output from input
       if ( i < param_array.size() )
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
       if ( i < param_array.size() )
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

  /*   Variable parameter definitions used: datafile- File and path
       where the logging file can be locateed/stored.  If the file
       exists, it will be appended, if it does not exist, it will be
       created.  comment- Comment field to add to the log.  A string
       pointer value.  text- Free form text field.  A string pointer
       value.

       The file data written is in the same format and sequence as
       that of the function call.  Uninitialized output parameters are
       denoted by two consecutive commas (eg ,,).  Data imported in
       this manner results in an empty cell in the spreadsheet. */

// recordLog VIII
void recordLog (std::string dataFile,
		std::string comment,
		std::string text)
{
  std::ofstream tfile(dataFile.c_str(), std::ios::app);
  tfile <<  ",,," << comment << "," << text << std::endl; 
  tfile.close();
  if (termEcho >= 1) 
    std::cout << std::endl << comment << " - " << text;
  return;
}

  /*   Variable parameter definitions used: datafile- File and path
       where the logging file can be locateed/stored.  If the file
       exists, it will be appended, if it does not exist, it will be
       created.  text- Free form text field.  A string pointer value.

       The file data written is in the same format and sequence as
       that of the function call.  Uninitialized output parameters are
       denoted by two consecutive commas (eg ,,).  Data imported in
       this manner results in an empty cell in the spreadsheet. */

//recordLog X
void recordLog(std::string datafile,
               const std::string text)
{
	
  std::ofstream tfile(datafile.c_str(), std::ios::app);
  tfile << text << std::endl;

  tfile.close();
  if (termEcho >= 1) 
    std::cout << text;
  return;
}
