//#############################################################################
//
//    FILENAME:   tokenizer.cpp
//
//    DESCRIPTION:
//      Class functions for a string splitter
//
//#############################################################################
#include "tokenizer.h"

///////////////////////////////////////////////////////////////
//
//  Function: StringTokenizer::StringTokenizer
//
//  Description: Constructor
//
//  Inputs: _str   - String to be splitup
//          _delim - Delimter to specify where string should be split
//
//  Returns: Itself
//
///////////////////////////////////////////////////////////////
StringTokenizer::StringTokenizer(const std::string& _str,
                                 const std::string& _delim)
{
   if ((_str.length() == 0) || (_delim.length() == 0)) return;

   token_str = _str;
   delim     = _delim;

  /*
     Remove sequential delimiter
  */
   size_t curr_pos = 0;

   while(true)
   {
      if ((curr_pos = token_str.find(delim,curr_pos)) != std::string::npos)
      {
         curr_pos += delim.length();

         while(token_str.find(delim,curr_pos) == curr_pos)
         {
            token_str.erase(curr_pos,delim.length());
         }
      }
      else
       break;
   }

   /*
     Trim leading delimiter
   */
   if (token_str.find(delim,0) == 0)
   {
      token_str.erase(0,delim.length());
   }

   /*
     Trim ending delimiter
   */
   curr_pos = 0;
   if ((curr_pos = token_str.rfind(delim)) != std::string::npos)
   {
      if (curr_pos != (token_str.length() - delim.length())) return;
      token_str.erase(token_str.length() - delim.length(),delim.length());
   }

}

///////////////////////////////////////////////////////////////
//
//  Function: StringTokenizer::countTokens
//
//  Description: Determine the number of string that has been split
//
//  Inputs: None
//
//  Returns: Number of Tokens
//
///////////////////////////////////////////////////////////////
int StringTokenizer::countTokens()
{
   size_t prev_pos = 0;
   int num_tokens        = 0;

   if (token_str.length() > 0)
   {
      num_tokens = 0;

      size_t curr_pos = 0;
      while(true)
      {
         if ((curr_pos = token_str.find(delim,curr_pos)) != std::string::npos)
         {
            num_tokens++;
            prev_pos  = curr_pos;
            curr_pos += delim.length();
         }
         else
          break;
      }
      return ++num_tokens;
   }
   else
   {
      return 0;
   }
}

///////////////////////////////////////////////////////////////
//
//  Function: StringTokenizer::hasMoreTokens
//
//  Description: Are more tokens availible
//
//  Inputs: None
//
//  Returns: Boolean indicationg of tokens are availible
//
///////////////////////////////////////////////////////////////
bool StringTokenizer::hasMoreTokens()
{
   return (token_str.length() > 0);
}

///////////////////////////////////////////////////////////////
//
//  Function: StringTokenizer::nextTokens
//
//  Description: Get the next availible token
//
//  Inputs: None
//
//  Returns: The current token
//
///////////////////////////////////////////////////////////////
std::string StringTokenizer::nextToken()
{

   if (token_str.length() == 0)
     return "";

   std::string  tmp_str = "";
   size_t pos     = token_str.find(delim,0);

   if (pos != std::string::npos)
   {
      tmp_str   = token_str.substr(0,pos);
      token_str = token_str.substr(pos+delim.length(),token_str.length()-pos);
   }
   else
   {
      tmp_str   = token_str.substr(0,token_str.length());
      token_str = "";
   }

   return tmp_str;
}

///////////////////////////////////////////////////////////////
//
//  Function: StringTokenizer::nextIntTokens
//
//  Description: Get the next availible token as an integer
//
//  Inputs: None
//
//  Returns: The current token converted to an integer
//
///////////////////////////////////////////////////////////////
int StringTokenizer::nextIntToken()
{
   return atoi(nextToken().c_str());
}

///////////////////////////////////////////////////////////////
//
//  Function: StringTokenizer::nextFloatTokens
//
//  Description: Get the next availible token as a floating point num
//
//  Inputs: None
//
//  Returns: The current token converted to a double
//
///////////////////////////////////////////////////////////////
double StringTokenizer::nextFloatToken()
{
   return atof(nextToken().c_str());
}

///////////////////////////////////////////////////////////////
//
//  Function: StringTokenizer::nextTokens
//
//  Description: Get the next availible token with a new deleminator
//
//  Inputs: delimiter - The value to getermine the end of the string
//
//  Returns: The current token
//
///////////////////////////////////////////////////////////////
std::string StringTokenizer::nextToken(const std::string& delimiter)
{
   if (token_str.length() == 0)
     return "";

   std::string  tmp_str = "";
   size_t pos   = token_str.find(delimiter,0);

   if (pos != std::string::npos)
   {
      tmp_str   = token_str.substr(0,pos);
      token_str = token_str.substr(pos + delimiter.length(),token_str.length() - pos);
   }
   else
   {
      tmp_str   = token_str.substr(0,token_str.length());
      token_str = "";
   }

   return tmp_str;
}

///////////////////////////////////////////////////////////////
//
//  Function: StringTokenizer::remainingString
//
//  Description: Return what is left of the string
//
//  Inputs: None
//
//  Returns: A string of what is left in the string
//
///////////////////////////////////////////////////////////////
std::string StringTokenizer::remainingString()
{
   return token_str;
}

///////////////////////////////////////////////////////////////
//
//  Function: StringTokenizer::filterNextToken
//
//  Description: remove a string from the next Token
//
//  Inputs: filterStr - text to be removed from the next string
//
//  Returns: The next string with the requested text removes
//
///////////////////////////////////////////////////////////////
std::string StringTokenizer::filterNextToken(const std::string& filterStr)
{
   std::string  tmp_str    = nextToken();
   size_t currentPos = 0;

   while((currentPos = tmp_str.find(filterStr,currentPos)) != std::string::npos)
   {
      tmp_str.erase(currentPos,filterStr.length());
   }

   return tmp_str;
}
