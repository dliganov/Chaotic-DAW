//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : Common Base Classes
// Filename    : pluginfactory.cpp
// Created by  : Steinberg, 01/2004
// Description : Standard Plug-In Factory
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
//-----------------------------------------------------------------------------

#ifndef __pluginfactory__
#include "pluginfactory.h"
#endif

#include <stdlib.h>

namespace Steinberg {

CPluginFactory* gPluginFactory = 0;

//------------------------------------------------------------------------
//  CPluginFactory implementation
//------------------------------------------------------------------------
CPluginFactory::CPluginFactory (const PFactoryInfo& info)
: classes (0)
, classCount (0)
, maxClassCount (0)
{
	FUNKNOWN_CTOR

	factoryInfo = info;
}

//------------------------------------------------------------------------
CPluginFactory::~CPluginFactory ()
{
	if (gPluginFactory == this)
		gPluginFactory = 0;

	if (classes)
		free (classes);

	FUNKNOWN_DTOR
}

//------------------------------------------------------------------------
IMPLEMENT_REFCOUNT (CPluginFactory)

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginFactory::queryInterface (const char* iid, void** obj)
{
	QUERY_INTERFACE (iid, obj, IPluginFactory::iid, IPluginFactory)
	QUERY_INTERFACE (iid, obj, IPluginFactory2::iid, IPluginFactory2)
	QUERY_INTERFACE (iid, obj, FUnknown::iid, IPluginFactory)
	*obj = 0;
	return kNoInterface;
}

//------------------------------------------------------------------------
bool CPluginFactory::registerClass (const PClassInfo* info,
									FUnknown* (*createFunc)(void*), void* context)
{
	if (!info || !createFunc)
		return false;

	PClassInfo2 info2;
	memset (&info2, 0, sizeof (PClassInfo2));
	memcpy (&info2, info, sizeof (PClassInfo));
	return registerClass (&info2, createFunc, context);
}

//------------------------------------------------------------------------
bool CPluginFactory::registerClass (const PClassInfo2* info,
									FUnknown* (*createFunc)(void*), void* context)
{
	if (!info || !createFunc)
		return false;

	if (classCount >= maxClassCount)
	{
		if (!growClasses ())
			return false;
	}

	PClassEntry& entry = classes[classCount];
	entry.info = *info;
	entry.createFunc = createFunc;
	entry.context = context;

	classCount++;
	return true;
}

//------------------------------------------------------------------------
bool CPluginFactory::growClasses ()
{
	static const int32 delta = 10;

	size_t size = (maxClassCount + delta) * sizeof (PClassEntry);
	void* memory = classes;

	if (!memory)
		memory = malloc (size);
	else
		memory = realloc (memory, size);

	if (!memory)
		return false;

	classes = (PClassEntry*)memory;
	maxClassCount += delta;
	return true;
}

//------------------------------------------------------------------------
bool CPluginFactory::isClassRegistered (const FUID& cid)
{
	for (int32 i = 0; i < classCount; i++)
	{
		if (cid == classes[i].info.cid)
			return true;
	}
	return false;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginFactory::getFactoryInfo (PFactoryInfo* info)
{
	if (info)
		memcpy (info, &factoryInfo, sizeof (PFactoryInfo));
	return kResultOk;
}

//------------------------------------------------------------------------
int32 PLUGIN_API CPluginFactory::countClasses ()
{
	return classCount;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginFactory::getClassInfo (int32 index, PClassInfo* info)
{
	if (info && (index >= 0 && index < classCount))
	{
		memcpy (info, &classes[index].info, sizeof (PClassInfo));
		return kResultOk;
	}
	return kInvalidArgument;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginFactory::getClassInfo2 (int32 index, PClassInfo2* info)
{
	if (info && (index >= 0 && index < classCount))
	{
		memcpy (info, &classes[index].info, sizeof (PClassInfo2));
		return kResultOk;
	}
	return kInvalidArgument;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CPluginFactory::createInstance (const char* cid, const char* iid, void** obj)
{
	for (int32 i = 0; i < classCount; i++)
	{
		if (memcmp (classes[i].info.cid, cid, 16) == 0)
		{
			FUnknown* instance = classes[i].createFunc (classes[i].context);
			if (instance)
			{
				if (instance->queryInterface (iid, obj) == kResultOk)
				{
					instance->release ();
					return kResultOk;
				}
				else
					instance->release ();
			}
			break;
		}
	}

	*obj = 0;
	return kNoInterface;
}

}