//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Interfaces
// Filename    : ivstcomponent.h
// Created by  : Steinberg, 04/2005
// Description : Basic VST Interfaces
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

#ifndef __ivstcomponent__
#define __ivstcomponent__

#include "pluginterfaces/base/ipluginbase.h"
#include "vsttypes.h"

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpush.h"
//------------------------------------------------------------------------

namespace Steinberg {
class IBStream;

//------------------------------------------------------------------------
/** All VST specific interfaces are located in Vst namespace */ 
//------------------------------------------------------------------------
namespace Vst {

const int32 kDefaultFactoryFlags = PFactoryInfo::kUnicode; ///< Standard value for PFactoryInfo::flags

#define BEGIN_FACTORY_DEF(vendor,url,email) using namespace Steinberg; \
	EXPORT_FACTORY IPluginFactory* PLUGIN_API GetPluginFactory () { \
	if (!gPluginFactory) \
	{	static PFactoryInfo factoryInfo = { vendor,url,email,Vst::kDefaultFactoryFlags }; \
		gPluginFactory = new CPluginFactory (factoryInfo);

//------------------------------------------------------------------------
/** \defgroup vstBus VST Buses 
@{*/

//------------------------------------------------------------------------
/** Bus media types */
//------------------------------------------------------------------------
enum MediaTypes
{
//------------------------------------------------------------------------
	kAudio = 0,		///< audio
	kEvent,			///< events
	kNumMediaTypes
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
/** Bus directions */
//------------------------------------------------------------------------
enum BusDirections
{
//------------------------------------------------------------------------
	kInput = 0,		///< input bus
	kOutput			///< output bus
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
/** Bus types */
//------------------------------------------------------------------------
enum BusTypes
{
//------------------------------------------------------------------------
	kMain = 0,		///< main bus
	kAux			///< auxilliary bus (sidechain)
//------------------------------------------------------------------------
};


//------------------------------------------------------------------------
/** Bus Description. 
A bus can be understood as 'collection of data channels' belonging together. 
It describes a data input or a data output of the plug-in. 
A VST component can define any desired number of buses, but this number \b never must change. 
Dynamic usage of buses is handled in the host by activating and deactivating
buses. The component has to define the maximum of supported buses and it has
to define which of them are active by default. A host that can handle multiple
buses, allows the user to activate buses that were initially inactive. 

See also: IComponent::getBusInfo , IComponent::activateBus
*/
//------------------------------------------------------------------------
struct BusInfo
{
//------------------------------------------------------------------------
	MediaType mediaType;		///< Media type - has to be a value of \ref MediaTypes
	BusDirection direction;		///< input or output \ref BusDirections
	int32 channelCount;			///< number of channels
	String128 name;				///< name of the bus
	BusType busType;			///< main or Aux - has to be a value of \ref BusTypes
	uint32 flags;				///< flags - a combination of \ref BusFlags
	enum BusFlags 
	{
		kDefaultActive = 1 << 0	///< bus active per default
	};		
//------------------------------------------------------------------------
};

/*@}*/


//------------------------------------------------------------------------
/** I/O modes */
//------------------------------------------------------------------------
enum IoModes
{
	kSimple = 0,	///< 1:1 Input / Output 
	kAdvanced		///< n:n I / O
};

//------------------------------------------------------------------------
/** Routing Information. 
    When the plug-in supports multiple I/O buses, a host may want to know how the
    buses are related. The relation of an event-input-channel to an audio-output-bus 
	in particular is of interest to the host (in order to relate MIDI-tracks to audio-channels)
    \n See also: IComponent::getRoutingInfo, \ref vst3Routing */
//------------------------------------------------------------------------
struct RoutingInfo
{
	MediaType mediaType;	///< media type @see MediaTypes
	int32 busIndex;			///< bus index
	int32 channel;			///< channel (-1 for all channels)
};

//------------------------------------------------------------------------
// IComponent Interface
//------------------------------------------------------------------------
/**  Component Base Interface.
\ingroup vstIPlug
- [plug imp]
- [mandatory]

This is the basic interface for a VST component and must always be supported.
It contains the common parts of any kind of processing class. The parts that
are specific to a media type are defined in a seperate interface. An implementation
component must provide both the specific interface and IComponent. 
*/
class IComponent: public IPluginBase
{
public:
//------------------------------------------------------------------------	
	/** Called before initializing the component to get information about the controller class. */
	virtual tresult PLUGIN_API getControllerClassId (TUID classId) = 0;
	
	/** Called before 'initialize' to set the component usage (optional). */
	virtual tresult PLUGIN_API setIoMode (IoMode mode) = 0;

	/** Called after the plug-in is initialized. */
	virtual int32 PLUGIN_API getBusCount (MediaType type, BusDirection dir) = 0;

	/** Called after the plug-in is initialized. */
	virtual tresult PLUGIN_API getBusInfo (MediaType type, BusDirection dir, int32 index, BusInfo& bus /*out*/) = 0;

	/** Retrieve routing information (to be implemented when more than one regular input or output bus exists). 
	    The inInfo always refers to an input bus while the returned outInfo must refer to an output bus! */
	virtual tresult PLUGIN_API getRoutingInfo (RoutingInfo& inInfo, RoutingInfo& outInfo /*out*/) = 0;

	/** Called upon (de-)activating a bus in the host application. */
	virtual tresult PLUGIN_API activateBus (MediaType type, BusDirection dir, int32 index, TBool state) = 0;

	/** Activate / deactivate the component. */
	virtual tresult PLUGIN_API setActive (TBool state) = 0;

	/** Set complete state of component. */
	virtual tresult PLUGIN_API setState (IBStream* state) = 0;

	/** Retrieve complete state of component. */
	virtual tresult PLUGIN_API getState (IBStream* state) = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IComponent, 0xE831FF31, 0xF2D54301, 0x928EBBEE, 0x25697802)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpop.h"
//------------------------------------------------------------------------

#endif // __ivstcomponent__
