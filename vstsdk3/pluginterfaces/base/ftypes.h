//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : SDK Core Interfaces
// Filename    : ftypes.h
// Created by  : Steinberg, 01/2004
// Description : Basic data types
//
//-----------------------------------------------------------------------------
// LICENSE
// © 2008, Steinberg Media Technologies GmbH, All Rights Reserved
//-----------------------------------------------------------------------------
// This Software Development Kit may not be distributed in parts or its entirety  
// without prior written agreement by Steinberg Media Technologies GmbH. 
// This SDK must not be used to re-engineer or manipulate any technology used  
// in any Steinberg or Third-party application or software module, 
// unless permitted by law.
// Neither the name of the Steinberg Media Technologies nor the names of its
// contributors may be used to endorse or promote products derived from this 
// software without specific prior written permission.
// 
// THIS SDK IS PROVIDED BY STEINBERG MEDIA TECHNOLOGIES GMBH "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL STEINBERG MEDIA TECHNOLOGIES GMBH BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#ifndef __ftypes__
#define __ftypes__

#include "fplatform.h"

#if MAC
	#undef UNICODE
	#define UNICODE 0
#endif


namespace Steinberg
{
// intergral types
// -----------------------------------------------------------------
	typedef char int8;
	typedef unsigned char uint8;
	typedef unsigned char uchar;

	typedef short int16;
	typedef unsigned short uint16;

#if MAC && __LP64__
	typedef int int32;
	typedef unsigned int uint32;
#else
	typedef long int32;
	typedef unsigned long uint32;	
#endif

	static const int32 kMaxLong = 0x7fffffff;
	static const int32 kMinLong = (-0x7fffffff - 1);

#if WINDOWS
	typedef __int64 int64;
	typedef unsigned __int64 uint64;
	static const int64 kMaxInt64 = 9223372036854775807i64;
	static const int64 kMinInt64 = (-9223372036854775807i64 - 1);
#else
	typedef long long int64;
	typedef unsigned long long uint64;
	static const int64 kMaxInt64 = 0x7fffffffffffffffLL;
	static const int64 kMinInt64 = (-0x7fffffffffffffffLL-1);
#endif

// -----------------------------------------------------------------
// other Semantic Types		
	typedef int64 TSize;   // byte (or other) sizes
	typedef int32 tresult; // result code
// -----------------------------------------------------------------
	static const float kMaxFloat = 3.40282346638528860e+38;

#if PLATFORM_64
	typedef uint64 TPtrInt;
#else
	typedef uint32 TPtrInt;
#endif

//------------------------------------------------------------------
// boolean
	typedef uint8 TBool;
#if MOTIF 
	#ifdef NOBOOL
		typedef uint8 bool;
		static const bool false = 0; 
		static const bool true = 1; 
 	#endif
#endif

//------------------------------------------------------------------
// strings
	#if (_MSC_VER >= 1300)
	typedef __wchar_t char16;
	#else
	typedef int16 char16;
	#endif

	#if UNICODE
		typedef char16 tchar;
	#else
		typedef char tchar;
	#endif

	typedef char char8;
	typedef const char* FIDString;
}

// convert byteorder macros
//----------------------------------------------------------------------------
#define SWAP_32(l) { \
	unsigned char* p = (unsigned char*)& (l); \
	unsigned char t; \
	t = p[0]; p[0] = p[3]; p[3] = t; t = p[1]; p[1] = p[2]; p[2] = t; }

#define SWAP_16(w) { \
	unsigned char* p = (unsigned char*)& (w); \
	unsigned char t; \
	t = p[0]; p[0] = p[1]; p[1] = t; }

#define SWAP_64(i) { \
	unsigned char* p = (unsigned char*)& (i); \
	unsigned char t; \
	t = p[0]; p[0] = p[7]; p[7] = t; t = p[1]; p[1] = p[6]; p[6] = t; \
	t = p[2]; p[2] = p[5]; p[5] = t; t = p[3]; p[3] = p[4]; p[4] = t;}

#endif
