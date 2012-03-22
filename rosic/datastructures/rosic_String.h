#ifndef rosic_String_h
#define rosic_String_h

#include <string>
#include <climits>
#include "../basics/rosic_FunctionTemplates.h"

#pragma warning( disable : 4996 ) // disables 'strcpy' may be unsafe warning in MSVC-2008

namespace rosic
{

  /**

  This is a class for representing strings. It is implemented as wrapper around plain old 
  C strings.

  \todo: get rid of the old-schoolish C-string stuff and manage the string as a plain array of 
  characters, maintain usedLength and allocatedLength as members, don't use the C-string 
  functions anymore, provide conversion functions from and to c-strings

  */

  class String
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Standard constructor. Creates an empty string. */
    String();  

    /** Constructor. Creates a string from a zero-terminated c-string. Note that when you pass the
    NULL macro, actually the constructor String(int) will be called with 0 as argument. */
    String(const char *initialString); 

    /** Copy constructor. Creates a (deep) copy of another string. */
    String(const String &other); 

    /** Constructor. Creates a string from an integer number. */
    String(const int intValue);

    /** Constructor. Creates a string from a single precision floating point number. */
    String(const float floatValue);

    /** Constructor. Creates a string from a double precision floating point number.
    \todo: maybe include a flag 'forceExponentialNotation' with default value false. */
    String(const double doubleValue);

    /** Destructor. */
    ~String();  

    //---------------------------------------------------------------------------------------------
    // setup:

    /** Reserves enough memory to store the given number of characters (excluding the terminating 
    zero). If already enough memory is reserved, it will do nothing, otherwise it will re-allocate
    memory of the desired size and copy the current string into it - so this function can be used 
    to make up space after the actually used memory. This can be advantageous to optimize 
    concatenations. */
    void reserveSize(int numCharactersToReserve);  

    //---------------------------------------------------------------------------------------------
    // inquiry:

    /** Returns the string as raw zero terminated c-string - this is just a pointer to the 
    internal character array which is maintained here, so DON'T attempt to DELETE it. */
    const char* getRawString() const { return cString; }

    /** Returns the number of characters in the string (excluding the terminating zero). */
    int getLength() const { return length; }

    /** Returns true when this string contains non-printable characters according to the isprint
    C-function. 
    \todo: get rid of that legacy c-stuff, treat all characters uniformly (including '0'). */
    bool containsNonPrintableCharacters() const;

    //---------------------------------------------------------------------------------------------
    // operators:

    /** Assigns one string with another one. */
    String& operator=(const String& s2)
    {
      if( reservedSize != s2.reservedSize )
      {
        delete[] cString;
        reservedSize = s2.reservedSize;
        cString      = new char[reservedSize];
      }
      length = s2.length;
      strcpy(cString, s2.cString); // use concatenateBuffers
      return *this;
    }

    /** Compares two strings of equality. Two strings are considered equal if they have the same 
    length and match character by character. The reserved memory size is irrelevant for this 
    comparison.  */
    bool operator==(const String& s2) const  
    {
      if( length != s2.length )
        return false;
      else
      {
        for(int i=0; i<length; i++)
        {
          if( cString[i] != s2.cString[i] )
            return false;
        }
      }
      return true;
    }

    /** Compares two strings of inequality. */
    bool operator!=(const String& s2) const  
    {
      return !(*this == s2);
    }

    /** Checks whether the left operand is less than the right operand (using strcmp). */
    bool operator<(const String& s2) const  
    {
      int result = strcmp(cString, s2.cString); // use compareBuffers
      return (result<0);
    }

    /** Checks whether the left operand is greater than the right operand (using strcmp). */
    bool operator>(const String& s2) const  
    {
      int result = strcmp(cString, s2.cString); // use compareBuffers
      return (result>0);
    }

    /** Checks whether the left operand is less than or equal to the right operand (using 
    strcmp). */
    bool operator<=(const String& s2) const  
    {
      int result = strcmp(cString, s2.cString);
      return (result<=0);
    }

    /** Checks whether the left operand is greater than or equal to the right operand (using 
    strcmp). */
    bool operator>=(const String& s2) const  
    {
      int result = strcmp(cString, s2.cString);
      return (result>=0);
    }

    /** Adds another string to this string by appending it and returns the result. */
    String& operator+=(const String &s2)
    {
      reserveSize(length + s2.length);
      strcat(cString, s2.cString);
      length += s2.length;
      return *this;
    }

    /** Adds two strings by concatenating them and returns the result. */
    String operator+(const String &s2)
    { 
      String result;
      result.reserveSize(length+s2.length);
      strcpy(result.cString, cString);
      strcat(result.cString, s2.cString);
      result.length = length+s2.length;
      return result;
    }

    //---------------------------------------------------------------------------------------------
    // conversions to numeric values:

    /** Returns the string as a integer number if conversion is possible, 0 otherwise. */
    int asInt() const { return atoi(cString); }

    /** Returns the string as double precision floating point number if conversion is possible,
    0.0 otherwise. */
    double asDouble() const;

    /** Returns the string as single precision floating point number if conversion is possible,
    0.0 otherwise. */
    float asFloat() const { return (float) atof(cString); }

   //---------------------------------------------------------------------------------------------
    // others:

    /** Writes the string to the standard output which is typically the console. */
    void printToStandardOutput() const;

    /** Copies the (assumed to be zero-terminated) c-string represented by the passed buffer into
    this string. */
    void readFromBuffer(char *sourceBuffer);

    /** Writes the string into the passed buffer. You should also pass the maximum number of 
    characters to write including the terminating zero - if the actual string is shorter than 
    that, the extra characters will be left as they are. If the string is longer, then you may 
    miss a terminating zero in your buffer. */
    void writeIntoBuffer(char *targetBuffer, int maxNumCharactersToWrite) const;

    //---------------------------------------------------------------------------------------------
    // static functions and data members:

    /** Returns the number of characters that are required to represent the given integer number 
    as string. */
    static int numberOfRequiredCharacters(int number);

    /** This is an empty string. */
    static const String empty;

    //=============================================================================================

  protected:

    /** Allocates memory area of size numCharactersToAllocateExcludingZero+1 and associates it with 
    our cString member, unless the allocated memory is already exactly of the right size in which 
    case nothing is done. In any event, upon return, our memory will have the right size and our
    cString pointer is associated with it. */
    void allocateMemory(int numCharactersToAllocateExcludingZero);

    char *cString;
    int  length;        // number of characters excluding the terminating zero
    int  reservedSize;  // size of the allocated memory (always greater than 'length' but not 
                        // necessarily exactly length+1)

    //=============================================================================================
   
  private:

    void initFromDoubleValue(double doubleValue); 
      // used only internally by the constructors String(double) and String(float) 

  };

} // end namespace rosic

#endif 