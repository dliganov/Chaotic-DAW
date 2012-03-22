//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : SDK Core Interfaces
// Filename    : ipluginbase.h
// Created by  : Steinberg, 01/2004
// Description : Basic Plug-In Interfaces
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

#ifndef __ipluginbase__
#define __ipluginbase__

#ifndef __funknown__
#include "funknown.h"
#endif

namespace Steinberg {

//------------------------------------------------------------------------
/**  Basic interface to a plug-in component.
\ingroup pluginBase
- [plug imp]
- initialize/terminate the plug-in component

The host uses this interface to initialize and to terminate the plug-in component.
The context that is passed to the initialize method contains any interface to the 
host that the plug-in will need to work. These interfaces can vary from category to category.
A list of supported host context interfaces should be included in the documentation 
of a specific category. */
//------------------------------------------------------------------------
class IPluginBase: public FUnknown
{
public:
//------------------------------------------------------------------------
	/** The host passes a number of interfaces as context to initialize the plug-in class. 
		@note Extensive memory allocations etc. should be performed in this method rather than in the class constructor!
		If the method does NOT return kResultOk, the object is released immediately. In this case terminate is not called! */
	virtual tresult PLUGIN_API initialize (FUnknown* context) = 0;
	
	/** This function is called, before the plug-in is unloaded and can be used for
	    cleanups. You have to release all references to any host application interfaces. */
	virtual tresult PLUGIN_API terminate () = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IPluginBase, 0x22888DDB, 0x156E45AE, 0x8358B348, 0x08190625)


//------------------------------------------------------------------------
/** Basic Information about the class factory of the plug-in.
\ingroup pluginBase
*/
//------------------------------------------------------------------------
struct PFactoryInfo
{
//------------------------------------------------------------------------
	enum FactoryFlags
	{
		kNoFlags					= 0,		///< Nothing
		kClassesDiscardable			= 1 << 0,	///< The number of exported classes can change each time the Module is loaded. If this flag is set, the host does not cache class information. This leads to a longer startup time because the host always has to load the Module to get the current class information. 
		kLicenseCheck				= 1 << 1,	///< Class IDs of components are interpreted as Syncrosoft-License (LICENCE_UID). Loaded in a Steinberg host, the module will not be loaded when the license is not valid  
		kComponentNonDiscardable	= 1 << 3,	///< Component won't be unloaded until process exit
		kUnicode                    = 1 << 4    ///< Components have entirely unicode encoded strings. (True for VST3 plug-ins so far)
	};

	enum
	{
		kURLSize = 256,
		kEmailSize = 128,
		kNameSize = 64
	};

//------------------------------------------------------------------------
	char vendor[kNameSize];		///< e.g. "Steinberg Media Technologies"
	char url[kURLSize];			///< e.g. "http://www.steinberg.de"
	char email[kEmailSize];		///< e.g. "info@steinberg.de"
	int32 flags;				///< (see above)
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
/**  Basic Information about a class provided by the plug-in.
\ingroup pluginBase
*/
//------------------------------------------------------------------------
struct PClassInfo
{
//------------------------------------------------------------------------
	enum ClassCardinality
	{
		kManyInstances = 0x7FFFFFFF
	};

	enum
	{
		kCategorySize = 32,
		kNameSize = 64
	};
//------------------------------------------------------------------------
	char cid[16];					///< Class ID 16 Byte class GUID
	int32 cardinality;				///< currently ignored, set to kManyInstances
	char category[kCategorySize];	///< class category, host uses this to categorize interfaces
	char name[kNameSize];			///< class name, visible to the user
//------------------------------------------------------------------------
};

} // namespace

//------------------------------------------------------------------------
#define LICENCE_UID(l1, l2, l3, l4) \
{ \
	(char)((l1 & 0xFF000000) >> 24), (char)((l1 & 0x00FF0000) >> 16), \
	(char)((l1 & 0x0000FF00) >>  8), (char)((l1 & 0x000000FF)      ), \
	(char)((l2 & 0xFF000000) >> 24), (char)((l2 & 0x00FF0000) >> 16), \
	(char)((l2 & 0x0000FF00) >>  8), (char)((l2 & 0x000000FF)      ), \
	(char)((l3 & 0xFF000000) >> 24), (char)((l3 & 0x00FF0000) >> 16), \
	(char)((l3 & 0x0000FF00) >>  8), (char)((l3 & 0x000000FF)      ), \
	(char)((l4 & 0xFF000000) >> 24), (char)((l4 & 0x00FF0000) >> 16), \
	(char)((l4 & 0x0000FF00) >>  8), (char)((l4 & 0x000000FF)      )  \
}

namespace Steinberg {

//------------------------------------------------------------------------
/**  Version 2 of Basic Information about a class provided by the plug-in.
\ingroup pluginBase
*/
//------------------------------------------------------------------------
struct PClassInfo2
{
	char cid[16];					///< @see PClassInfo
	int32 cardinality;				///< @see PClassInfo
	char category[PClassInfo::kCategorySize];	///< @see PClassInfo
	char name[PClassInfo::kNameSize];			///< @see PClassInfo

	enum {
		kVendorSize = 64,
		kVersionSize = 64,
		kSubCategoriesSize = 128
	};

	uint32 classFlags;				///< flags used for a specific category, must be defined where category is defined 
	char subCategories[kSubCategoriesSize];	///< module specific subcategories, can be more than one, separated by '|'
	char vendor[kVendorSize];		///< overwrite vendor information from factory info
	char version[kVersionSize];		///< Version string (e.g. "1.0.0.512" with Major.Minor.Subversion.Build)
	char sdkVersion[kVersionSize];	///< SDK version used to build this class (e.g. "VST 3.0")
};

//------------------------------------------------------------------------
//  IPluginFactory interface declaration
//------------------------------------------------------------------------
/**	Class factory that any plug-in defines for creating class instances.
\ingroup pluginBase
- [plug imp]

From the host's point of view a plug-in module is a factory which can create 
a certain kind of object(s). The interface IPluginFactory provides methods 
to get information about the classes exported by the plug-in and a 
mechanism to create instances of these classes (that usually define the IPluginBase interface).

<b> An implementation is provided in public.sdk/source/common/pluginfactory.cpp </b>
\see GetPluginFactory
*/
//------------------------------------------------------------------------
class IPluginFactory : public FUnknown
{
public:
//------------------------------------------------------------------------
	/** Fill a PFactoryInfo structure with information about the plug-in vendor. */
	virtual tresult PLUGIN_API getFactoryInfo (PFactoryInfo* info) = 0;

	/** Returns the number of exported classes by this factory. 
	If you are using the CPluginFactory implementation provided by the SDK, it returns the number of classes you registered with CPluginFactory::registerClass. */
	virtual int32   PLUGIN_API countClasses () = 0;
	
	/** Fill a PClassInfo structure with information about the class at the specified index. */
	virtual tresult PLUGIN_API getClassInfo (int32 index, PClassInfo* info) = 0;
	
	/** Create a new class instance. */
	virtual tresult PLUGIN_API createInstance (const char* cid, const char* iid, void** obj) = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IPluginFactory, 0x7A4D811C, 0x52114A1F, 0xAED9D2EE, 0x0B43BF9F)

//------------------------------------------------------------------------
//  IPluginFactory2 interface declaration
//------------------------------------------------------------------------
/**	Version 2 of class factory supporting PClassInfo2.
\ingroup pluginBase
\copydoc IPluginFactory
*/
//------------------------------------------------------------------------
class IPluginFactory2 : public IPluginFactory
{
public:
//------------------------------------------------------------------------
	/** Returns the class info (version 2) for a given index. */
	virtual tresult PLUGIN_API getClassInfo2 (int32 index, PClassInfo2* info) = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};
DECLARE_CLASS_IID (IPluginFactory2, 0x0007B650, 0xF24B4C0B, 0xA464EDB9, 0xF00B2ABB)

}

//------------------------------------------------------------------------
// GetPluginFactory
//------------------------------------------------------------------------
/**  Plug-In entrypoint. 
\ingroup pluginBase
Any plug-in must define and export this function. \n
A typical implementation of GetPluginFactory looks like this
						\code
	IPluginFactory* PLUGIN_API GetPluginFactory ()
	{
		if (!gPluginFactory)
		{
			static PFactoryInfo factoryInfo =
			{
				"My Company Name",
				"http://www.mywebpage.com",
				"mailto:myemail@address.com",
				PFactoryInfo::kNoFlags
			};

			gPluginFactory = new CPluginFactory (factoryInfo);

			static PClassInfo componentClass =
			{
				INLINE_UID (0x00000000, 0x00000000, 0x00000000, 0x00000000), // replace by a valid uid
				1,    
				"Service",    // category
				"Name"
			};

			gPluginFactory->registerClass (&componentClass, MyComponentClass::newInstance);
		}
		else
			gPluginFactory->addRef ();

		return gPluginFactory;
	}
					\endcode
\see \ref loadPlugin
*/
//------------------------------------------------------------------------
extern "C"
{
	Steinberg::IPluginFactory* PLUGIN_API GetPluginFactory ();
	typedef Steinberg::IPluginFactory* (PLUGIN_API *GetFactoryProc) ();
}

#endif	// __ipluginbase__
