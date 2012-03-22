//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0.1
//
// Category    : Helpers
// Filename    : vstsinglecomponenteffect.cpp
// Created by  : Steinberg, 03/2008
// Description : Basic Audio Effect Implementation
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

#include "vstsinglecomponenteffect.h"

namespace Steinberg {
namespace Vst {


//------------------------------------------------------------------------
// AudioEffect
//------------------------------------------------------------------------
SingleComponentEffect::SingleComponentEffect ()
: audioInputs (kAudio, kInput)
, audioOutputs (kAudio, kOutput)
, eventInputs (kEvent, kInput)
, eventOutputs (kEvent, kOutput)
, componentHandler (0)
, hostContext (0)
{
	FUNKNOWN_CTOR

	processSetup.maxSamplesPerBlock = 1024;
	processSetup.processMode = Vst::kRealtime;
	processSetup.sampleRate = 44100.0;
	processSetup.symbolicSampleSize = Vst::kSample32;
}

//------------------------------------------------------------------------
SingleComponentEffect::~SingleComponentEffect ()
{
	FUNKNOWN_DTOR
}

//------------------------------------------------------------------------
IMPLEMENT_REFCOUNT (SingleComponentEffect)

//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::queryInterface (const char* iid, void** obj)
{
	QUERY_INTERFACE (iid, obj, ::Steinberg::FUnknown::iid, IComponent)
	QUERY_INTERFACE (iid, obj, ::Steinberg::IPluginBase::iid, IComponent)
	QUERY_INTERFACE (iid, obj, IComponent::iid, IComponent)
	QUERY_INTERFACE (iid, obj, IAudioProcessor::iid, IAudioProcessor)
	QUERY_INTERFACE (iid, obj, IEditController::iid, IEditController)
	*obj = 0;
	return kNoInterface;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::initialize (FUnknown* context)
{
	// check if already initialized
	if (hostContext)
		return kResultFalse;

	hostContext = context;
	if (hostContext)
		hostContext->addRef ();

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::terminate ()
{
	parameters.removeAll ();

	if (componentHandler)
	{
		componentHandler->release ();
		componentHandler = 0;
	}

	// release host interfaces
	if (hostContext)
	{
		hostContext->release ();
		hostContext = 0;
	}

	return kResultOk;
}


//------------------------------------------------------------------------
int32 PLUGIN_API SingleComponentEffect::getBusCount (MediaType type, BusDirection dir)
{
	BusList* busList = getBusList (type, dir);
	return busList ? busList->total () : 0;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::getBusInfo (MediaType type, BusDirection dir, int32 index, BusInfo& info)
{
	BusList* busList = getBusList (type, dir);
	Bus* bus = busList ? (Bus*)busList->at (index) : 0;
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
tresult PLUGIN_API SingleComponentEffect::activateBus (MediaType type, BusDirection dir, int32 index, TBool state)
{
	BusList* busList = getBusList (type, dir);
	Bus* bus = busList ? (Bus*)busList->at (index) : 0;
	if (bus)
	{
		bus->setActive (state);
		return kResultTrue;
	}
	return kResultFalse;
}


//------------------------------------------------------------------------
AudioBus* SingleComponentEffect::addAudioInput (const TChar* name, SpeakerArrangement arr, 
									  BusType busType, int32 flags)
{
	AudioBus* newBus = new AudioBus (name, busType, flags, arr);
	audioInputs.append (newBus);
	return newBus;
}

//------------------------------------------------------------------------
AudioBus* SingleComponentEffect::addAudioOutput (const TChar* name, SpeakerArrangement arr, 
									   BusType busType, int32 flags)
{
	AudioBus* newBus = new AudioBus (name, busType, flags, arr);
	audioOutputs.append (newBus);
	return newBus;
}

//------------------------------------------------------------------------
EventBus* SingleComponentEffect::addEventInput (const TChar* name, int32 channels, 
									  BusType busType, int32 flags)
{
	EventBus* newBus = new EventBus (name, busType, flags, channels);
	eventInputs.append (newBus);
	return newBus;
}

//------------------------------------------------------------------------
EventBus* SingleComponentEffect::addEventOutput (const TChar* name, int32 channels, 
									   BusType busType, int32 flags)
{
	EventBus* newBus = new EventBus (name, busType, flags, channels);
	eventOutputs.append (newBus);
	return newBus;
}

//------------------------------------------------------------------------
tresult SingleComponentEffect::removeAudioBusses ()
{
	audioInputs.removeAll ();
	audioOutputs.removeAll ();

	return kResultOk;
}

//------------------------------------------------------------------------
tresult SingleComponentEffect::removeEventBusses ()
{
	eventInputs.removeAll ();
	eventOutputs.removeAll ();

	return kResultOk;
}

//------------------------------------------------------------------------
tresult SingleComponentEffect::removeAllBusses ()
{
	removeAudioBusses ();
	removeEventBusses ();

	return kResultOk;
}

// IAudioProcessor
//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::setBusArrangements (SpeakerArrangement* inputs, int32 numIns, 
													SpeakerArrangement* outputs, int32 numOuts)
{
	int32 counter = 0;
	LIST_FOREACH (AudioBus, bus, audioInputs)
		if (counter < numIns)
			bus->setArrangement (inputs[counter]);
		counter++;
	LIST_ENDFOR

	counter = 0;
	LIST_FOREACH (AudioBus, bus, audioOutputs)
		if (counter < numOuts)
			bus->setArrangement (outputs[counter]);
		counter++;
	LIST_ENDFOR

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::getBusArrangement (BusDirection dir, int32 busIndex, SpeakerArrangement& arr)
{
	BusList* busList = getBusList (kAudio, dir);
	AudioBus* audioBus = busList ? (AudioBus*)busList->at (busIndex) : 0;
	if (audioBus)
	{
		arr = audioBus->getArrangement ();
		return kResultTrue;
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::canProcessSampleSize (int32 symbolicSampleSize)
{
	return symbolicSampleSize == kSample32 ? kResultTrue : kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::setupProcessing (ProcessSetup& newSetup)
{
	if (newSetup.symbolicSampleSize != kSample32)
		return kResultFalse;

	processSetup = newSetup;
	return kResultOk;
}


//---IEditController-------
//------------------------------------------------------------------------
int32 PLUGIN_API SingleComponentEffect::getParameterCount ()
{
	return parameters.getParameterCount ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::getParameterInfo (int32 paramIndex, ParameterInfo& info)
{
	Parameter* parameter = parameters.getParameterByIndex (paramIndex);
	if (parameter)
	{
		info = parameter->getInfo ();
		return kResultTrue;
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::getParamStringByValue (ParamID tag, ParamValue valueNormalized, String128 string)
{
	Parameter* parameter = parameters.getParameter (tag);
	if (parameter)
	{
		parameter->toString (valueNormalized, string);
		return kResultTrue;
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::getParamValueByString (ParamID tag, TChar* string, ParamValue& valueNormalized)
{
	Parameter* parameter = parameters.getParameter (tag);
	if (parameter)
	{
		if (parameter->fromString (string, valueNormalized))
			return kResultTrue;
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
ParamValue PLUGIN_API SingleComponentEffect::normalizedParamToPlain (ParamID tag, ParamValue valueNormalized)
{
	Parameter* parameter = parameters.getParameter (tag);
	if (parameter)
		return parameter->toPlain (valueNormalized);
	return valueNormalized;
}

//------------------------------------------------------------------------
ParamValue PLUGIN_API SingleComponentEffect::plainParamToNormalized (ParamID tag, ParamValue plainValue)
{
	Parameter* parameter = parameters.getParameter (tag);
	if (parameter)
		return parameter->toNormalized (plainValue);
	return plainValue;
}

//------------------------------------------------------------------------
ParamValue PLUGIN_API SingleComponentEffect::getParamNormalized (ParamID tag)
{
	Parameter* parameter = parameters.getParameter (tag);
	if (parameter)
		return parameter->getNormalized ();
	return 0.0;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::setParamNormalized (ParamID tag, ParamValue value)
{
	Parameter* parameter = parameters.getParameter (tag);
	if (parameter)
	{
		parameter->setNormalized (value);
		return kResultTrue;
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SingleComponentEffect::setComponentHandler (IComponentHandler* newHandler)
{
	if (componentHandler == newHandler)
		return kResultTrue;

	if (componentHandler)
		componentHandler->release ();

	componentHandler = newHandler;
	if (componentHandler)
		componentHandler->addRef ();
		
	return kResultTrue;
}

//------------------------------------------------------------------------
tresult SingleComponentEffect::beginEdit (ParamID tag)
{
	if (componentHandler)
		return componentHandler->beginEdit (tag);
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult SingleComponentEffect::performEdit (ParamID tag, ParamValue valueNormalized)
{
	if (componentHandler)
		return componentHandler->performEdit (tag, valueNormalized);
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult SingleComponentEffect::endEdit (ParamID tag)
{
	if (componentHandler)
		return componentHandler->endEdit (tag);
	return kResultFalse;
}

//------------------------------------------------------------------------
BusList* SingleComponentEffect::getBusList (MediaType type, BusDirection dir)
{
	if (type == kAudio)
		return dir == kInput ? &audioInputs : &audioOutputs;
	else if (type == kEvent)
		return dir == kInput ? &eventInputs : &eventOutputs;
	return 0;
}


} // namespace Vst
} // namespace Steinberg
