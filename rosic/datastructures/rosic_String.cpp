#include "rosic_String.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// static data members:

const String String::empty;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

String::String()
{
  length       = 0;
  reservedSize = 1;                      // for the terminating zero
  cString      = new char[reservedSize]; // array of size 1
  strcpy(cString, "");                   // init as empty c-string
}

String::String(const char *initialString)
{
  if( initialString != NULL )
  {
    length       = strlen(initialString);
    reservedSize = length+1;
    cString      = new char[reservedSize];
    strcpy(cString, initialString);
  }
  else
  {
    length       = 0;
    reservedSize = 1; 
    cString      = new char[reservedSize];
    strcpy(cString, "");
  }
}

String::String(const String &other)
{
  length       = other.length;
  reservedSize = other.reservedSize; 
  cString      = new char[reservedSize];
  strcpy(cString, other.cString);
}
   
String::String(const int intValue)
{   
  length       = numberOfRequiredCharacters(intValue);
  reservedSize = length+1;
  cString      = new char[reservedSize];
  itoa(intValue, cString, 10);  
}

String::String(const float floatValue)
{ 
  initFromDoubleValue((double) floatValue);
}

String::String(const double doubleValue)
{ 
  initFromDoubleValue(doubleValue);
}

String::~String()
{
  delete[] cString;
}

//-------------------------------------------------------------------------------------------------
// static member functions:

int String::numberOfRequiredCharacters(int number)
{
  if( number == 0 )
    return 1;
  int numChars           = 0;
  int absValue           = abs(number);
  unsigned long long tmp = 1;             // must be able to go above INT_MAX
  while( tmp <= absValue && tmp < INT_MAX )
  {
    tmp *= 10;
    numChars++;
  }
  if(number < 0)
    numChars++; // for the minus
  return numChars;
}

//-------------------------------------------------------------------------------------------------
// setup:

void String::reserveSize(int numCharactersToReserve)
{   
  int requiredMemory = numCharactersToReserve + 1;
  if( reservedSize < requiredMemory )
  {
    char *tmpString = new char[requiredMemory];
    strcpy(tmpString, cString);
    delete[] cString;
    reservedSize = requiredMemory;
    cString      = tmpString;
  }
}

double String::asDouble() const    
{
  if( *this == String("INF") )
    return INF;
  else if( *this == String("-INF") )
    return -INF;
  else if( *this == String("NaN") )
    return NAN;
  return atof(cString); 
}

//-------------------------------------------------------------------------------------------------
// inquiry:

bool String::containsNonPrintableCharacters() const
{
  for(int i=0; i<length; i++)
  {
    if( isprint(cString[i]) == false )
      return true;
  }
  return false;
}

//-------------------------------------------------------------------------------------------------
// others:

void String::printToStandardOutput() const
{
  printf("%s", getRawString());
}

void String::readFromBuffer(char *sourceBuffer)
{
  int sourceLength = strlen(sourceBuffer);
  allocateMemory(sourceLength);
  strcpy(cString, sourceBuffer);
}

void String::writeIntoBuffer(char *targetBuffer, int maxNumCharactersToWrite) const
{
  int upperLimit = rmin(length+1, maxNumCharactersToWrite);
  for(int i=0; i<upperLimit; i++)
    targetBuffer[i] = cString[i];
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void String::allocateMemory(int numCharactersToAllocateExcludingZero)
{
  if( reservedSize-1 != numCharactersToAllocateExcludingZero )
  {
    delete[] cString;
    length       = numCharactersToAllocateExcludingZero;
    reservedSize = length+1;
    cString      = new char[reservedSize];
  }
}

void String::initFromDoubleValue(double doubleValue)
{
  length       = 0;
  reservedSize = 0;                    
  cString      = NULL;
  if( doubleValue == INF )
  {
    *this = String("INF");
    int dummy = 0;
    return;
  }
  else if( doubleValue == -INF )
  {
    *this = String("-INF");
    int dummy = 0;
    return;
  }
  else if( _isnan(doubleValue) )
  {
    *this = String("NaN");
    int dummy = 0;
    return;
  }

  // create temporary string long enough to hold all the characters, determine the actually 
  // required length and copy the repsective part of the temporary string - the c-book says, 
  // strncpy is unreliable with respect to copying the terminating zero, so we do it manually:
  char tmpString[64];
  sprintf(tmpString, "%-.17gl", doubleValue);
  length       = strspn(tmpString, "0123456789+-.eE");
  reservedSize = length+1;
  cString      = new char[reservedSize];
  for(int i=0; i<length; i++)
    cString[i] = tmpString[i];
  cString[length] = '\0';
}
