//#############################################################################
//
//    FILENAME:   compareParam.cpp
//
//
//    DESCRIPTION:
//      This module contains the compare function to determine if the
//      Functions passed the expected results.
//
//    NOTES:
//
//
//#############################################################################
#include "csm.h"
#include "VTSMisc.h"
#include <iostream>
#include <string>
#include <math.h>
extern int termEcho;

// For sizing the act_val arrays

using namespace std;

bool compareDebugFlag=false;

///////////////////////////////////////////////////////////////
//
//  Function: compareParam
//
//  Description: Compare string results against expect values
//
//  Inputs: param_array - Vector of inputs to the instruction
//          act_val     - Vector of results from the function
//          pass_fail   - Vector of comparison results
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////
void compareParam (vector<string> param_array,
		   vector<string> act_val,
		   map<int, bool>   &pass_fail)
{
   int i,count;                        //----- Loop Variables

   //----- Initialize values
   i=0;
   count=0;
   if(compareDebugFlag)
      cout << endl;
   while(i < (int)param_array.size())
   {
      if(compareDebugFlag)
         cout << "param_array[" << i << "] = " << param_array[i] << endl;
      pass_fail[count] = true;
      if(param_array[i] == "-c~")
      {
         count++;
      }
      else if(param_array[i] == "-c")
      {
         if(param_array[i+1] != act_val[count])
            pass_fail[count] = false;
         if(compareDebugFlag)
         {
            cout << "param_array[" << i+1 << "]("
                 << param_array[i+1] << ") == ";
            cout << "act_val[" << count << "]("
                 << act_val[count] << ")";
            if( pass_fail[count] )
                cout << " ==> pass" << endl;
            else
                cout << " ==> fail" << endl;
         }
         i++;

         count++;
      }

      i++;
   }
}

///////////////////////////////////////////////////////////////
//
//  Function: compareParam
//
//  Description: Compare string results against expect values
//
//  Inputs: param_array - Vector of inputs to the instruction
//          act_val     - Vector of results from the function
//          comp_answer - Vector of computation done in the comparasion
//          pass_fail   - Vector of comparison results
//
//  Returns: Nothing
//
///////////////////////////////////////////////////////////////

void compareParam(vector<string> param_array,
                  map<int, double> act_val,
                  map<int, double> &comp_answer,
                  map<int, bool>   &pass_fail)
{
   int i,count;                        //----- Loop Variables
   double compareValue;                //----- Data Value
   double compareRange;                //----- Data Range
   double offset;                      //----- Data Difference

   //----- Initialize values
   i=0;
   count=0;
   if(compareDebugFlag)
      cout << endl;
   while(i < (int)param_array.size())
   {
      if(compareDebugFlag)
         cout << "param_array[" << i << "] = " << param_array[i] << endl;
      pass_fail[count] = true;
      if(param_array[i] == "-c~")
      {
         count++;
      }
      else if(param_array[i] == "-c")
      {
         compareValue = atof(param_array[++i].c_str());
         if(compareDebugFlag)
            cout << "-c " << param_array[i];
         compareRange = 0.0;
         if((int)param_array.size() > i+2 && param_array[i+1] == "-+")
         {
            compareRange = atof(param_array[i+2].c_str());
            comp_answer[count] = compareValue - act_val[count];
            offset = fabs(comp_answer[count]);
            if(compareDebugFlag)
               cout << " " << param_array[i+1] << " " << param_array[i+2];
            i+=2;
         }
         else if((int)param_array.size() > i+2 && param_array[i+1] == "-%")
         {
            compareRange = fabs(atof(param_array[i+2].c_str())*compareValue);
            offset = fabs(compareValue - act_val[count]);
            if(compareDebugFlag)
               cout << " " << param_array[i+1] << " " << param_array[i+2];
            i+=2;
            comp_answer[count] = act_val[count]/compareValue;
         }
         else
         {
            comp_answer[count] = compareValue - act_val[count];
         }

         if(compareRange == 0.0)
         {
            if(compareDebugFlag)
               cout << " ===> " << compareValue 
                    << " == "
                    << act_val[count];
            if(compareValue != act_val[count])
               pass_fail[count] = false;
         }
         else
         {
            if(compareDebugFlag)
               cout << " ===> " << offset << " <= " << compareRange;
            if(offset > compareRange)
               pass_fail[count] = false;
         }

         if(compareDebugFlag)
         {
            if(pass_fail[count])
               cout << " ===> pass" << endl;
            else
               cout << " ===> fail" << endl;
         }

         count++;
      }

      i++;
   }
}
