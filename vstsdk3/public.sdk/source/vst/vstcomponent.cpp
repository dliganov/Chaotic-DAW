//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Helpers
// Filename    : vstcomponent.cpp
// Created by  : Steinberg, 04/2005
// Description : Basic VST Plug-In Implementation
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
//----------------------------------------------------------------------------------

#include "vstcomponent.h"

namespace Steinberg {
namespace Vst {


//------------------------------------------------------------------------
// Component
//------------------------------------------------------------------------
Component::Component ()
: audioInputs (kAudio, kInput)
, audioOutputs (kAudio, kOutput)
, eventInputs (kEvent, kInput)
, eventOutputs (kEvent, kOutput)
{}

//------------------------------------------------------------------------
tresult PLUGIN_API Component::queryInterface (const char* iid, void** obj)
{
	QUERY_INTERFACE (iid, obj, IComponent::iid, IComponent)
	return ComponentBase::queryInterface (iid, obj);
}

//------------------------------------------------------------------------
tresult PLUGIN_API Component::initialize (FUnknown* context)
{
	return ComponentBase::initialize (context);
}

//------------------------------------------------------------------------
tresult PLUGIN_API Component::terminate ()
{
	// remove all busses
	removeAllBusses ();

	return ComponentBase::terminate ();
}

//------------------------------------------------------------------------
BusList* Component::getBusList (MediaType type, BusDirection dir)
{
	if (type == kAudio)
		return dir == kInput ? &audioInputs : &audioOutputs;
	else if (type == kEvent)
		return dir == kInput ? &eventInputs : &eventOutputs;
	return 0;
}

//------------------------------------------------------------------------
tresult Component::removeAudioBusses ()
{
	audioInputs.removeAll ();
	audioOutputs.removeAll ();

	return kResultOk;
}

//------------------------------------------------------------------------
tresult Component::removeEventBusses ()
{
	eventInputs.removeAll ();
	eventOutputs.removeAll ();

	return kResultOk;
}

//------------------------------------------------------------------------
tresult Component::removeAllBusses ()
{
	removeAudioBusses ();
	removeEventBusses ();

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Component::getControllerClassId (TUID classID)
{
	if (controllerClass.isValid ())
	{
		controllerClass.toTUID (classID);
		return kResultTrue;
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Component::setIoMode (IoMode mode)
{
	return kNotImplemented;
}

//------------------------------------------------------------------------
int32 PLUGIN_API Component::getBusCount (MediaType type, BusDirection dir)
{
	BusList* busList = getBusList (type, dir);
	return busList ? busList->total () : 0;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Component::getBusInfo (MediaType type, BusDirection dir, int32 index, BusInfo& info)
{
	BusList* busList = getBusList (type, dir);
	Bus* bus = busList ? static_cast<Bus*> (busList->at (index)) : 0;
	if (bus)
	{
		info.mediaType = type;
		info.direction = dir;
		if (bus->getInfo (info))
			return kResultTrue;
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Component::getRoutingInfo (RoutingInfo& inInfo, RoutingInfo& outInfo)
{
	return kNotImplemented;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Component::activateBus (MediaType type, BusDirection dir, int32 index, TBool state)
{
	BusList* busList = getBusList (type, dir);
	Bus* bus = busList ? static_cast<Bus*> (busList->at (index)) : 0;
	if (bus)
	{
		bus->setActive (state);
		return kResultTrue;
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Component::setActive (TBool state)
{
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Component::setState (IBStream* state)
{
	return kNotImplemented;
}

//------------------------------------------------------------------------
tresult PLUGIN_API Component::getState (IBStream* state)
{
	return kNotImplemented;
}

} // namespace Vst
} // namespace Steinberg
