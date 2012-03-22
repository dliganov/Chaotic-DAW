//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : SDK Core Interfaces
// Filename    : funknown.h
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
#define __funknown__

#include <string.h>
#include "ftypes.h"


//------------------------------------------------------------------------
#if WINDOWS
	#define COM_COMPATIBLE 1
#else
	#define COM_COMPATIBLE 0
#endif

//------------------------------------------------------------------------
/*! \defgroup pluginBase Basic Interfaces 
*/

//------------------------------------------------------------------------
#undef PLUGIN_API
#if COM_COMPATIBLE
	#define PLUGIN_API __stdcall
#else
	#define PLUGIN_API
#endif


//------------------------------------------------------------------------
//  Unique Identifier macros
//------------------------------------------------------------------------

#if COM_COMPATIBLE
#define INLINE_UID(l1, l2, l3, l4) \
{ \
	(::Steinberg::int8)((l1 & 0x000000FF)      ), (::Steinberg::int8)((l1 & 0x0000FF00) >>  8), \
	(::Steinberg::int8)((l1 & 0x00FF0000) >> 16), (::Steinberg::int8)((l1 & 0xFF000000) >> 24), \
	(::Steinberg::int8)((l2 & 0x00FF0000) >> 16), (::Steinberg::int8)((l2 & 0xFF000000) >> 24), \
	(::Steinberg::int8)((l2 & 0x000000FF)      ), (::Steinberg::int8)((l2 & 0x0000FF00) >>  8), \
	(::Steinberg::int8)((l3 & 0xFF000000) >> 24), (::Steinberg::int8)((l3 & 0x00FF0000) >> 16), \
	(::Steinberg::int8)((l3 & 0x0000FF00) >>  8), (::Steinberg::int8)((l3 & 0x000000FF)      ), \
	(::Steinberg::int8)((l4 & 0xFF000000) >> 24), (::Steinberg::int8)((l4 & 0x00FF0000) >> 16), \
	(::Steinberg::int8)((l4 & 0x0000FF00) >>  8), (::Steinberg::int8)((l4 & 0x000000FF)      )  \
}
#else
#define INLINE_UID(l1, l2, l3, l4) \
{ \
	(::Steinberg::int8)((l1 & 0xFF000000) >> 24), (::Steinberg::int8)((l1 & 0x00FF0000) >> 16), \
	(::Steinberg::int8)((l1 & 0x0000FF00) >>  8), (::Steinberg::int8)((l1 & 0x000000FF)      ), \
	(::Steinberg::int8)((l2 & 0xFF000000) >> 24), (::Steinberg::int8)((l2 & 0x00FF0000) >> 16), \
	(::Steinberg::int8)((l2 & 0x0000FF00) >>  8), (::Steinberg::int8)((l2 & 0x000000FF)      ), \
	(::Steinberg::int8)((l3 & 0xFF000000) >> 24), (::Steinberg::int8)((l3 & 0x00FF0000) >> 16), \
	(::Steinberg::int8)((l3 & 0x0000FF00) >>  8), (::Steinberg::int8)((l3 & 0x000000FF)      ), \
	(::Steinberg::int8)((l4 & 0xFF000000) >> 24), (::Steinberg::int8)((l4 & 0x00FF0000) >> 16), \
	(::Steinberg::int8)((l4 & 0x0000FF00) >>  8), (::Steinberg::int8)((l4 & 0x000000FF)      )  \
}
#endif

//------------------------------------------------------------------------
#define DECLARE_UID(name, l1, l2, l3, l4) \
	const char name[16] = INLINE_UID (l1, l2, l3, l4);

//------------------------------------------------------------------------
#define EXTERN_UID(name) \
	extern const char name[16];

//------------------------------------------------------------------------
#ifdef INIT_CLASS_IID
#define DECLARE_CLASS_IID(ClassName, l1, l2, l3, l4) const ::Steinberg::FUID ClassName::iid (l1, l2, l3, l4);
#else
#define DECLARE_CLASS_IID(ClassName, l1, l2, l3, l4)
#endif

#define DEFINE_CLASS_IID(ClassName, l1, l2, l3, l4) const ::Steinberg::FUID ClassName::iid (l1, l2, l3, l4);

//------------------------------------------------------------------------
//  FUnknown implementation macros
//------------------------------------------------------------------------

#define DECLARE_FUNKNOWN_METHODS                                             \
public:	                                                                     \
	virtual ::Steinberg::tresult PLUGIN_API queryInterface (const char* iid, void** obj); \
	virtual ::Steinberg::uint32 PLUGIN_API addRef ();                              \
	virtual ::Steinberg::uint32 PLUGIN_API release ();                             \
protected :                                                                  \
	::Steinberg::int32 __funknownRefCount;                                        \
public:

//------------------------------------------------------------------------

#define DELEGATE_REFCOUNT(ClassName)											\
public:																			\
	virtual ::Steinberg::uint32 PLUGIN_API addRef ()  { return ClassName::addRef ();  }	\
	virtual ::Steinberg::uint32 PLUGIN_API release () { return ClassName::release (); }	\

//------------------------------------------------------------------------
#define IMPLEMENT_REFCOUNT(ClassName)          \
::Steinberg::uint32 PLUGIN_API ClassName::addRef ()  \
{                                              \
	return ::Steinberg::FUnknown::atomicAdd (__funknownRefCount, 1);               \
}                                              \
::Steinberg::uint32 PLUGIN_API ClassName::release () \
{                                              \
	if (::Steinberg::FUnknown::atomicAdd  (__funknownRefCount, -1) == 0)              \
	{                                          \
		delete this;                           \
		return 0;                              \
	}                                          \
	return __funknownRefCount;                 \
}

//------------------------------------------------------------------------
#define FUNKNOWN_CTOR	{ __funknownRefCount = 1; ::Steinberg::FUnknown::addObject (); }
#define FUNKNOWN_DTOR	{ ::Steinberg::FUnknown::releaseObject (); }

//------------------------------------------------------------------------
#define QUERY_INTERFACE(iid, obj, InterfaceIID, InterfaceName) \
if (memcmp (iid, InterfaceIID, 16) == 0)                        \
{                                                              \
	addRef ();                                                 \
	*obj = (InterfaceName*)this;                               \
	return kResultOk;                                          \
}

//------------------------------------------------------------------------
#define IMPLEMENT_QUERYINTERFACE(ClassName, InterfaceName, ClassIID)       \
::Steinberg::tresult PLUGIN_API ClassName::queryInterface (const char* iid, void** obj) \
{                                                                          \
	QUERY_INTERFACE (iid, obj, ::Steinberg::FUnknown::iid, InterfaceName)             \
	QUERY_INTERFACE (iid, obj, ClassIID, InterfaceName)                    \
	*obj = 0;                                                              \
	return kNoInterface;                                                   \
}

//------------------------------------------------------------------------
#define IMPLEMENT_FUNKNOWN_METHODS(ClassName,InterfaceName,ClassIID) \
	IMPLEMENT_REFCOUNT (ClassName)                                   \
	IMPLEMENT_QUERYINTERFACE (ClassName, InterfaceName, ClassIID)

//------------------------------------------------------------------------
//  Result Codes
//------------------------------------------------------------------------

namespace Steinberg {

//------------------------------------------------------------------------
#if COM_COMPATIBLE
#if WINDOWS
enum
{
	kNoInterface		= 0x80004002L,		// E_NOINTERFACE
	kResultOk			= 0x00000000L,		// S_OK
	kResultTrue			= kResultOk,
	kResultFalse		= 0x00000001L,		// S_FALSE
	kInvalidArgument	= 0x80070057L,		// E_INVALIDARG
	kNotImplemented		= 0x80004001L,		// E_NOTIMPL
	kInternalError		= 0x80004005L,		// E_FAIL
	kNotInitialized		= 0x8000FFFFL,		// E_UNEXPECTED
	kOutOfMemory		= 0x8007000EL		// E_OUTOFMEMORY
};
#else
enum
{
	kNoInterface		= 0x80000004L,		// E_NOINTERFACE
	kResultOk			= 0x00000000L,		// S_OK
	kResultTrue			= kResultOk,
	kResultFalse		= 0x00000001L,		// S_FALSE
	kInvalidArgument	= 0x80000003L,		// E_INVALIDARG
	kNotImplemented		= 0x80000001L,		// E_NOTIMPL
	kInternalError		= 0x80000008L,		// E_FAIL
	kNotInitialized		= 0x8000FFFFL,		// E_UNEXPECTED
	kOutOfMemory		= 0x80000002L		// E_OUTOFMEMORY
};
#endif
#else
enum
{
	kNoInterface = -1,
	kResultOk,
	kResultTrue = kResultOk,
	kResultFalse,
	kInvalidArgument,
	kNotImplemented,
	kInternalError,
	kNotInitialized,
	kOutOfMemory
};
#endif

//------------------------------------------------------------------------
typedef int64 LARGE_INT; // obsolete

//------------------------------------------------------------------------
//	FUID class declaration
//------------------------------------------------------------------------
typedef char TUID[16]; 

//------------------------------------------------------------------------
/** Handling 16 Byte Globaly Unique Identifiers.
\ingroup pluginBase
	
Each interface declares its identifier as static member inside the interface 
namespace (e.g. FUnknown::iid). 
*/
//------------------------------------------------------------------------
class FUID
{
public:
//------------------------------------------------------------------------
	FUID ();
	FUID (const char*);
	FUID (int32 l1, int32 l2, int32 l3, int32 l4);
	FUID (const FUID&);
	virtual ~FUID ();

	/** Generate a new Unique Identifier (UID). 
	    Will return true for success. If the return value is false, either no 
	    UID is generated or the UID is not guaranteed to be unique worldwide. */
	bool generate ();
	
	/** Check if the UID data is valid. 
		The default constructor initializes the memory with zeros. */
	bool isValid () const;

	FUID& operator = (const FUID& f);
	FUID& operator = (const char* uid);

	bool operator == (const FUID& f) const   { return memcmp (data, f.data, 16) == 0; }
	bool operator == (const char* uid) const { return memcmp (data, uid, 16) == 0; }

	bool operator != (const FUID& f) const   { return memcmp (data, f.data, 16) != 0; }
	bool operator != (const char* uid) const { return memcmp (data, uid, 16) != 0; }

	operator const char* () const { return data; }
	operator char* () { return data; }

	int32 getLong1 () const;
	int32 getLong2 () const;
	int32 getLong3 () const;
	int32 getLong4 () const;

	/** Convert UID to a string. 
		The string will be 32 characters int32, representing the hexadecimal values 
		of each data byte (e.g. "9127BE30160E4BB69966670AA6087880"). */
	void toString (char* string) const;

	/** Set the UID data from a string. 
		The string has to be 32 characters int32, where each character-pair is 
		the ASCII-encoded hexadecimal value of the corresponding data byte. */
	bool fromString (const char* string);

	/** Convert UID to a string in Microsoft® OLE format. 
	(e.g. "{c200e360-38c5-11ce-ae62-08002b2b79ef}") */
	void toRegistryString (char* string) const;

	/** Set the UID data from a string in Microsoft® OLE format. */
	bool fromRegistryString (const char* string);

	enum UIDPrintStyle 
	{ 
		kINLINE_UID,  ///< "INLINE_UID (0x00000000, 0x00000000, 0x00000000, 0x00000000)"
		kDECLARE_UID, ///< "DECLARE_UID (0x00000000, 0x00000000, 0x00000000, 0x00000000)"
		kFUID,        ///< "FUID (0x00000000, 0x00000000, 0x00000000, 0x00000000)"
		kCLASS_UID    ///< "DECLARE_CLASS_IID (Interface, 0x00000000, 0x00000000, 0x00000000, 0x00000000)"
	};
	/** Print the UID to a string (or debug output if string is NULL). 
		\param string is the output string if not NULL.
		\param style can be chosen from the FUID::UIDPrintStyle enumeration. */
	void print (char* string = 0, int32 style = kINLINE_UID) const;

	void toTUID (TUID result) const;
//------------------------------------------------------------------------
protected:
	TUID data;
};


//------------------------------------------------------------------------
// FUnknown
//------------------------------------------------------------------------
/**	The basic interface of all interfaces. 
\ingroup pluginBase

- The FUnknown::queryInterface method is used to retrieve pointers to other 
  interfaces of the object. 
- FUnknown::addRef and FUnknown::release manage the lifetime of the object. 
  If no more references exist, the object is destroyed in memory. 

Interfaces are identified by 16 byte Globally Unique Identifiers. 
The SDK provides a class called FUID for this purpose. 

\sa \ref howtoClass */
//------------------------------------------------------------------------
class FUnknown
{
public:
//------------------------------------------------------------------------
	/** Query for a pointer to the specified interface. 
	Returns kResultOk on success or kNoInterface if the object does not implement the interface. 
	The object has to call addRef when returning an interface. 
	\param iid : (in) 16 Byte interface identifier (-> FUID)  
	\param obj : (out) On return, *obj point to the requested interface */
	virtual tresult PLUGIN_API queryInterface (const char* iid, void** obj) = 0;
	
	/** Add a reference and return the new reference count. 
	\par Remarks:
		The initial reference count after creating an object is 1. */
	virtual uint32 PLUGIN_API addRef () = 0;

	/** Release a reference and return the new reference count. 
	If the reference count reaches zero, the object has to be destroyed in memory. */
	virtual uint32 PLUGIN_API release () = 0;

//------------------------------------------------------------------------
	static const FUID iid;

//------------------------------------------------------------------------
	// global object counting:
	static void addObject ();
	static void releaseObject ();
	static int32 countObjects ();

	static int32 atomicAdd (int32& value, int32 amount); 
//------------------------------------------------------------------------
};

DECLARE_CLASS_IID (FUnknown, 0x00000000, 0x00000000, 0xC0000000, 0x00000046)


//------------------------------------------------------------------------
// FReleaser
//------------------------------------------------------------------------
/** Release an interface using automatic object.
\ingroup pluginBase

Usage example: 
										\code
	void someFunction ()
	{
		IPath* path = pathCreateMethod ();
		FReleaser releaser (path);
		.... do something with path...
		.... path not used anymore, releaser will destroy it when leaving function scope
	}
										\endcode 
*/
//------------------------------------------------------------------------
struct FReleaser
{
	FReleaser (FUnknown* u): u (u) {}
	~FReleaser () { if (u) u->release (); }

	FUnknown* u;
};


//------------------------------------------------------------------------
// FUnknownPtr
//------------------------------------------------------------------------
/**	Smart pointer template class.
\ingroup pluginBase

- can be used as an I* pointer
- handles refCount of the interface
- tries to acquire the interface when an FUnknown* is assigned
- Usage example: 
										\code
		FUnknown* unknown = ...
		FUnknownPtr<IPath> path (unknown);
		if (path)
			path->ascend ();
										\endcode 
*/
//------------------------------------------------------------------------
template <class I>
class FUnknownPtr
{
public:
//------------------------------------------------------------------------
	inline FUnknownPtr (FUnknown* unknown);
	inline FUnknownPtr (const FUnknownPtr&);
	inline FUnknownPtr ();
	inline ~FUnknownPtr ();

	inline I* operator=(FUnknown* unknown);

	inline operator I* ()  const { return iface; }      // act as I*
	inline I* operator->() const { return iface; }      // act as I*
	inline I* getInterface () { return iface; }
//------------------------------------------------------------------------
protected:
	I* iface;
};

//------------------------------------------------------------------------
template <class I>
inline FUnknownPtr<I>::FUnknownPtr (FUnknown* unknown)
: iface (0)
{
	if (unknown && unknown->queryInterface (I::iid, (void**)&iface) != kResultOk)
		iface = 0;
}
//------------------------------------------------------------------------
template <class I>
inline FUnknownPtr<I>::FUnknownPtr (const FUnknownPtr& other)
: iface (other.iface)
{
	if (iface)
		iface->addRef ();
}

//------------------------------------------------------------------------
template <class I>
inline FUnknownPtr<I>::FUnknownPtr ()
: iface (0)
{}

//------------------------------------------------------------------------
template <class I>
inline FUnknownPtr<I>::~FUnknownPtr ()
{
	if (iface)
		iface->release ();
}

//------------------------------------------------------------------------
template <class I>
inline I* FUnknownPtr<I>::operator=(FUnknown* unknown)
{
	if (iface)
		iface->release ();
	iface = 0;
	if (unknown && unknown->queryInterface (I::iid, (void**)&iface) != kResultOk)
		iface = 0;
	return iface;
}



//------------------------------------------------------------------------
//  FVariant struct declaration
//------------------------------------------------------------------------
/** A Value of variable type.
\ingroup pluginBase
*/
class FVariant
{
//------------------------------------------------------------------------
public:
	enum
	{
		kEmpty   = 0,
		kInteger = 1<<0,
		kFloat   = 1<<1,
		kString  = 1<<2,
		kObject  = 1<<3,
		kOwner   = 1<<4
	};

//------------------------------------------------------------------------
	// ctors
	FVariant () { memset (this, 0, sizeof (FVariant)); }
	FVariant (const FVariant& variant);

	FVariant (int64 v)     : type (kInteger), intValue (v)   {}
	FVariant (double v)        : type (kFloat),   floatValue (v) {}
	FVariant (const char* str) : type (kString),  string (str)   {}
	FVariant (FUnknown* obj)   : type (kObject),  object (obj)   {}
	~FVariant () { empty (); }

//------------------------------------------------------------------------
	FVariant& operator= (const FVariant& variant);

	inline void setInt   (int64 v)    { empty (); type = kInteger; intValue = v;   }
	inline void setFloat (double v)       { empty (); type = kFloat;   floatValue = v; }
	inline void setString (const char* v) { empty (); type = kString;  string = v;     }
	inline void setObject (FUnknown* obj) { empty (); type = kObject;  object = obj;   }

	inline int64 getInt () const		{ return (type & kInteger) ? intValue : 0; }
	inline double getFloat () const	        { return (type & kFloat) ? floatValue : 0.; }
	inline double getNumber () const		{ return (type & kInteger) ? (double)intValue : (type & kFloat) ? floatValue : 0.; }
	inline const char* getString () const	{ return (type & kString) ? string : 0; }
	inline FUnknown* getObject () const	    { return (type & kObject) ? object : 0; }

	inline uint16 getType () const { return (type & 0x0F); }
	inline bool isEmpty () const           { return getType () == kEmpty; }
	inline bool isOwner () const           { return (type & kOwner) != 0; }

	void empty ();
//------------------------------------------------------------------------
	uint16 type;
	union
	{
		int64   intValue;
		double      floatValue;
		const char* string;
		FUnknown*   object;
	};
};

}

#endif
