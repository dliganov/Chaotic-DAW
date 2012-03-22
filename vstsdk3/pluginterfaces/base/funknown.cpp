//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : SDK Core Interfaces
// Filename    : funknown.cpp
// Created by  : Steinberg, 01/2004
// Description : Basic Interface
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

#ifndef __funknown__
#include "funknown.h"
#endif

#include <stdio.h>

#if WINDOWS
#include <objbase.h>
#endif

#if TARGET_API_MAC_CARBON
#include <CoreFoundation/CoreFoundation.h>
#include <libkern/OSAtomic.h>
#endif

namespace Steinberg {

//------------------------------------------------------------------------
#if COM_COMPATIBLE
#if WINDOWS
#define GuidStruct GUID
#else
struct GuidStruct
{
    uint32  Data1;
    uint16 Data2;
    uint16 Data3;
    uint8  Data4[8];
};
#endif
#endif

static void toString (char* string, const char* data, int32 i1, int32 i2);
static void fromString (const char* string, char* data, int32 i1, int32 i2);
static int32 makeLong (uint8 b1, uint8 b2, uint8 b3, uint8 b4);

//------------------------------------------------------------------------
//  FUnknown static members
//------------------------------------------------------------------------

static int32 gNumObjects = 0;

//------------------------------------------------------------------------
void FUnknown::addObject ()
{
	gNumObjects++;
}

//------------------------------------------------------------------------
void FUnknown::releaseObject ()
{
	gNumObjects--;
}

//------------------------------------------------------------------------
int32 FUnknown::countObjects ()
{
	return gNumObjects;
}

//------------------------------------------------------------------------
int32 FUnknown::atomicAdd (int32& var, int32 d)
{
#if WINDOWS
	return InterlockedExchangeAdd (&var, d) + d;
#elif MAC
	return OSAtomicAdd32Barrier (d, (int32_t*)&var);
#else
	var += d;
	return var;
#endif
}


//------------------------------------------------------------------------
//	FUID implementation
//------------------------------------------------------------------------

FUID::FUID ()
{
	memset (data, 0, 16);
}

//------------------------------------------------------------------------
FUID::FUID (const char* uid)
{
	memset (data, 0, 16);
	if (uid)
		memcpy (data, uid, 16);
}

//------------------------------------------------------------------------
FUID::FUID (int32 l1, int32 l2, int32 l3, int32 l4)
{
#if COM_COMPATIBLE
	data [0]  = (char)((l1 & 0x000000FF)      );
	data [1]  = (char)((l1 & 0x0000FF00) >>  8);
	data [2]  = (char)((l1 & 0x00FF0000) >> 16);
	data [3]  = (char)((l1 & 0xFF000000) >> 24);
	data [4]  = (char)((l2 & 0x00FF0000) >> 16);
	data [5]  = (char)((l2 & 0xFF000000) >> 24);
	data [6]  = (char)((l2 & 0x000000FF)      );
	data [7]  = (char)((l2 & 0x0000FF00) >>  8);
	data [8]  = (char)((l3 & 0xFF000000) >> 24);
	data [9]  = (char)((l3 & 0x00FF0000) >> 16);
	data [10] = (char)((l3 & 0x0000FF00) >>  8);
	data [11] = (char)((l3 & 0x000000FF)      );
	data [12] = (char)((l4 & 0xFF000000) >> 24);
	data [13] = (char)((l4 & 0x00FF0000) >> 16);
	data [14] = (char)((l4 & 0x0000FF00) >>  8);
	data [15] = (char)((l4 & 0x000000FF)      );
#else
	data [0]  = (char)((l1 & 0xFF000000) >> 24);
	data [1]  = (char)((l1 & 0x00FF0000) >> 16);
	data [2]  = (char)((l1 & 0x0000FF00) >>  8);
	data [3]  = (char)((l1 & 0x000000FF)      );
	data [4]  = (char)((l2 & 0xFF000000) >> 24);
	data [5]  = (char)((l2 & 0x00FF0000) >> 16);
	data [6]  = (char)((l2 & 0x0000FF00) >>  8);
	data [7]  = (char)((l2 & 0x000000FF)      );
	data [8]  = (char)((l3 & 0xFF000000) >> 24);
	data [9]  = (char)((l3 & 0x00FF0000) >> 16);
	data [10] = (char)((l3 & 0x0000FF00) >>  8);
	data [11] = (char)((l3 & 0x000000FF)      );
	data [12] = (char)((l4 & 0xFF000000) >> 24);
	data [13] = (char)((l4 & 0x00FF0000) >> 16);
	data [14] = (char)((l4 & 0x0000FF00) >>  8);
	data [15] = (char)((l4 & 0x000000FF)      );
#endif
}

//------------------------------------------------------------------------
FUID::FUID (const FUID& f)
{
	memcpy (data, f.data, 16);
}

//------------------------------------------------------------------------
FUID::~FUID ()
{}

//------------------------------------------------------------------------
bool FUID::generate ()
{
#if WINDOWS
	GUID guid;
	HRESULT hr = CoCreateGuid (&guid);
	switch (hr)
	{
		case RPC_S_OK:
			memcpy (data, (char*)&guid, 16);
			return true;

		case RPC_S_UUID_LOCAL_ONLY:
		default:
			return false;
	}

#elif TARGET_API_MAC_CARBON
	CFUUIDRef uuid = CFUUIDCreate (kCFAllocatorDefault);
	if (uuid)
	{
		CFUUIDBytes bytes = CFUUIDGetUUIDBytes (uuid);
		memcpy (data, (char*)&bytes, 16);
		CFRelease (uuid);
		return true;
	}
	return false;

#else
	// implement me!
	return false;
#endif
}

//------------------------------------------------------------------------
bool FUID::isValid () const
{
	char nulluid[16];
	memset (nulluid, 0, 16);

	return memcmp (data, nulluid, 16) != 0;
}

//------------------------------------------------------------------------
FUID& FUID::operator = (const FUID& f)
{
	memcpy (data, f.data, 16);
	return *this;
}

//------------------------------------------------------------------------
FUID& FUID::operator = (const char* uid)
{
	memcpy (data, uid, 16);
	return *this;
}

//------------------------------------------------------------------------
int32 FUID::getLong1 () const
{
#if COM_COMPATIBLE
	return makeLong (data[3],  data[2],  data [1],  data [0]);
#else
	return makeLong (data[0],  data[1],  data [2],  data [3]);
#endif
}

//------------------------------------------------------------------------
int32 FUID::getLong2 () const
{
#if COM_COMPATIBLE
	return makeLong (data[5],  data[4],  data [7],  data [6]);
#else
	return makeLong (data[4],  data[5],  data [6],  data [7]);
#endif
}

//------------------------------------------------------------------------
int32 FUID::getLong3 () const
{
#if COM_COMPATIBLE
	return makeLong (data[8],  data[9],  data [10], data [11]);
#else
	return makeLong (data[8],  data[9],  data [10], data [11]);
#endif
}

//------------------------------------------------------------------------
int32 FUID::getLong4 () const
{
#if COM_COMPATIBLE
	return makeLong (data[12], data[13], data [14], data [15]);
#else
	return makeLong (data[12], data[13], data [14], data [15]);
#endif
}

//------------------------------------------------------------------------
void FUID::toString (char* string) const
{
	if (!string)
		return;

	#if COM_COMPATIBLE
	GuidStruct* g = (GuidStruct*)data;

	char s[17];
	Steinberg::toString (s, data, 8, 16);

	sprintf (string, "%08X%04X%04X%s", g->Data1, g->Data2, g->Data3, s);
	#else
	Steinberg::toString (string, data, 0, 16);
	#endif
}

//------------------------------------------------------------------------
bool FUID::fromRegistryString (const char* string)
{
	if (!string || !*string)
		return false;
	if (strlen (string) != 38)
		return false;

	// e.g. {c200e360-38c5-11ce-ae62-08002b2b79ef}

	#if COM_COMPATIBLE
	GuidStruct g;
	char s[10];

	strncpy (s, string + 1, 8);
	s[8] = 0;
	sscanf (s, "%x", &g.Data1);
	strncpy (s, string + 10, 4);
	s[4] = 0;
	sscanf (s, "%x", &g.Data2);
	strncpy (s, string + 15, 4);
	s[4] = 0;
	sscanf (s, "%x", &g.Data3);
	memcpy (data, &g, 8);

	Steinberg::fromString (string + 20, data, 8, 10);
	Steinberg::fromString (string + 25, data, 10, 16);
	#else
	Steinberg::fromString (string + 1, data, 0, 4);
	Steinberg::fromString (string + 10, data, 4, 6);
	Steinberg::fromString (string + 15, data, 6, 8);
	Steinberg::fromString (string + 20, data, 8, 10);
	Steinberg::fromString (string + 25, data, 10, 16);
	#endif

	return true;
}

//------------------------------------------------------------------------
void FUID::toRegistryString (char* string) const
{
	// e.g. {c200e360-38c5-11ce-ae62-08002b2b79ef}

	#if COM_COMPATIBLE
	GuidStruct* g = (GuidStruct*)data;

	char s1[5];
	Steinberg::toString (s1, data, 8, 10);

	char s2[13];
	Steinberg::toString (s2, data, 10, 16);

	sprintf (string, "{%08X-%04X-%04X-%s-%s}", g->Data1, g->Data2, g->Data3, s1, s2);
	#else
	char s1[9];
	Steinberg::toString (s1, data, 0, 4);
	char s2[5];
	Steinberg::toString (s2, data, 4, 6);
	char s3[5];
	Steinberg::toString (s3, data, 6, 8);
	char s4[5];
	Steinberg::toString (s4, data, 8, 10);
	char s5[13];
	Steinberg::toString (s5, data, 10, 16);

	sprintf (string, "{%s-%s-%s-%s-%s}", s1, s2, s3, s4, s5);
	#endif
}

//------------------------------------------------------------------------
bool FUID::fromString (const char* string)
{
	if (!string || !*string)
		return false;
	if (strlen (string) != 32)
		return false;

	#if COM_COMPATIBLE
	GuidStruct g;
	char s[33];

	strcpy (s, string);
	s[8] = 0;
	sscanf (s, "%x", &g.Data1);
	strcpy (s, string + 8);
	s[4] = 0;
	sscanf (s, "%x", &g.Data2);
	strcpy (s, string + 12);
	s[4] = 0;
	sscanf (s, "%x", &g.Data3);

	memcpy (data, &g, 8);
	Steinberg::fromString (string + 16, data, 8, 16);
	#else
	Steinberg::fromString (string, data, 0, 16);
	#endif

	return true;
}

//------------------------------------------------------------------------
void FUID::print (char* string, int32 style) const
{
	if (!string) // no string: debug output
	{
		char str [128];
		print (str, style);

		#if WINDOWS
		OutputDebugStringA (str);
		OutputDebugStringA ("\n");
		#endif
		return;
	}

	int32 l1 = getLong1 ();
	int32 l2 = getLong2 ();
	int32 l3 = getLong3 ();
	int32 l4 = getLong4 ();

	switch (style)
	{
		case kINLINE_UID:
			sprintf (string, "INLINE_UID (0x%08X, 0x%08X, 0x%08X, 0x%08X)", l1, l2, l3, l4);
			break;

		case kDECLARE_UID:
			sprintf (string, "DECLARE_UID (0x%08X, 0x%08X, 0x%08X, 0x%08X)", l1, l2, l3, l4);
			break;

		case kFUID:
			sprintf (string, "FUID (0x%08X, 0x%08X, 0x%08X, 0x%08X)", l1, l2, l3, l4);
			break;

		case kCLASS_UID:
		default:
			sprintf (string, "DECLARE_CLASS_IID (Interface, 0x%08X, 0x%08X, 0x%08X, 0x%08X)", l1, l2, l3, l4);
			break;
	}
}

//------------------------------------------------------------------------
void FUID::toTUID (TUID result) const
{
	memcpy (result, data, 16);
}


//------------------------------------------------------------------------
//  helpers
//------------------------------------------------------------------------
static int32 makeLong (uint8 b1, uint8 b2, uint8 b3, uint8 b4)
{
	return (int32(b1) << 24) | (int32(b2) << 16) | (int32(b3) << 8) | int32(b4);
}

//------------------------------------------------------------------------
static void toString (char* string, const char* data, int32 i1, int32 i2)
{
	*string = 0;
	for (int32 i = i1; i < i2; i++)
	{
		char s[3];
		sprintf (s, "%02X", (uint8)data[i]);
		strcat (string, s);
	}
}

//------------------------------------------------------------------------
static void fromString (const char* string, char* data, int32 i1, int32 i2)
{
	for (int32 i = i1; i < i2; i++)
	{
		char s[3];
		s[0] = *string++;
		s[1] = *string++;
		s[2] = 0;

		int32 d = 0;
		sscanf (s, "%2x", &d);
		data[i] = (char)d;
	}
}

//------------------------------------------------------------------------
//	FVariant class
//------------------------------------------------------------------------

FVariant::FVariant (const FVariant& variant)
: type (variant.type)
{
	if ((type & kString) && variant.string)
	{
		string = new char[strlen (variant.string) + 1];
		strcpy ((char*)string, variant.string);
		type |= kOwner;
	}
	else if ((type & kObject) && variant.object)
	{
		object = variant.object;
		object->addRef ();
		type |= kOwner;
	}
	else
		intValue = variant.intValue; // copy memory
}

//------------------------------------------------------------------------
void FVariant::empty ()
{
	if (type & kOwner)
	{
		if ((type & kString) && string)
			delete [] (char *)string;

		else if ((type & kObject) && object)
			object->release ();
	}
	memset (this, 0, sizeof (FVariant));
}

//------------------------------------------------------------------------
FVariant& FVariant::operator= (const FVariant& variant)
{
	empty ();

	type = variant.type;

	if ((type & kString) && variant.string)
	{
		string = new char[strlen (variant.string) + 1];
		strcpy ((char*)string, variant.string);
		type |= kOwner;
	}
	else if ((type & kObject) && variant.object)
	{
		object = variant.object;
		object->addRef ();
		type |= kOwner;
	}
	else
		intValue = variant.intValue; // copy memory

	return *this;
}

}