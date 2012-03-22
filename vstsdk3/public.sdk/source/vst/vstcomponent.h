//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Helpers
// Filename    : vstcomponent.h
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

#ifndef __vstcomponent__
#define __vstcomponent__

#include "public.sdk/source/vst/vstcomponentbase.h"
#include "public.sdk/source/vst/vstbus.h"
#include "public.sdk/source/common/linkedlist.h"
#include "pluginterfaces/vst/ivstcomponent.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {


//------------------------------------------------------------------------
/** Default implementation for a VST3 Component.
\ingroup vstClasses 
Can be used as base class for a VST3 component implementation. */
//------------------------------------------------------------------------
class Component: public ComponentBase,
				 public IComponent
{
public:
//------------------------------------------------------------------------
	/** Constructor */
	Component ();

	//---Internal Methods-------
	/** Sets the controller Class ID associated to its component. */
	void setControllerClass (const FUID& cid) { controllerClass = cid; }

	/** Removes all Audio Busses */
	tresult removeAudioBusses ();

	/** Removes all Event Busses */
	tresult removeEventBusses ();

	//---from IComponent--------
	tresult PLUGIN_API getControllerClassId (TUID classID);
	tresult PLUGIN_API setIoMode (IoMode mode);
	int32 PLUGIN_API getBusCount (MediaType type, BusDirection dir);
	tresult PLUGIN_API getBusInfo (MediaType type, BusDirection dir, int32 index, BusInfo& info);
	tresult PLUGIN_API getRoutingInfo (RoutingInfo& inInfo, RoutingInfo& outInfo);
	tresult PLUGIN_API activateBus (MediaType type, BusDirection dir, int32 index, TBool state);
	tresult PLUGIN_API setActive (TBool state);
	tresult PLUGIN_API setState (IBStream* state);
	tresult PLUGIN_API getState (IBStream* state);

	//---from ComponentBase------
	DELEGATE_REFCOUNT (ComponentBase)
	tresult PLUGIN_API queryInterface (const char* iid, void** obj);
	tresult PLUGIN_API initialize (FUnknown* context);
	tresult PLUGIN_API terminate ();

//------------------------------------------------------------------------
protected:
	FUID controllerClass;
	BusList audioInputs;
	BusList audioOutputs;
	BusList eventInputs;
	BusList eventOutputs;

	BusList* getBusList (MediaType type, BusDirection dir);
	tresult removeAllBusses ();
};

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

#endif	// __vstcomponent__
