//------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Interfaces
// Filename    : ivsteditcontroller.h
// Created by  : Steinberg, 09/2005
// Description : VST Edit Controller Interfaces
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

#ifndef __ivsteditcontroller__
#define __ivsteditcontroller__

#include "pluginterfaces/base/ipluginbase.h"
#include "vsttypes.h"

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpush.h"
//------------------------------------------------------------------------

//------------------------------------------------------------------------
/** Class Category Name for Controller Component */
//------------------------------------------------------------------------
#ifndef kVstComponentControllerClass
#define kVstComponentControllerClass "Component Controller Class"
#endif


//------------------------------------------------------------------------
namespace Steinberg {

class IPlugView;
class IBStream;

//------------------------------------------------------------------------
namespace Vst {

//------------------------------------------------------------------------
/** GUI Parameter Info. */
//------------------------------------------------------------------------
struct ParameterInfo
{
//------------------------------------------------------------------------
	ParamID id;				///< unique identifier of this parameter
	String128 title;		///< parameter title (e.g. "Volume")
	String128 shortTitle;	///< parameter shortTitle (e.g. "Vol")
	String128 units;		///< parameter unit (e.g. "dB")
	int32 stepCount;		///< number of discrete steps (0: continuous, 1: toggle, discrete value otherwise - see \ref vst3parameterIntro)
	ParamValue defaultNormalizedValue;	///< default normalized value
	UnitID unitId;			///< id of unit this parameter belongs to (see \ref vst3UnitsIntro)
	
	int32 flags;			///< ParameterFlags (see below)
	enum ParameterFlags
	{
		kCanAutomate	 = 1 << 0,	///< parameter can be automated
		kIsReadOnly		 = 1 << 1,	///< parameter can not be changed from outside (implies that kCanAutomate is false)
		
		kIsProgramChange = 1 << 15,	///< parameter is a program change (unitId gives info about associated unit - see \ref vst3UnitPrograms)
		kIsBypass		 = 1 << 16	///< special bypass parameter (only one allowed): plug-in can handle bypass 
	};
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
/** View Types used for IEditController::createView */ 
//------------------------------------------------------------------------
namespace ViewType {
const CString kEditor = "editor";
}

//------------------------------------------------------------------------
/** Flags used for IComponentHandler::restartComponent */
//------------------------------------------------------------------------
enum RestartFlags
{
	kReloadComponent	= 1 << 0,	///< the Component should be reloaded
	kIoChanged			= 1 << 1,	///< Input and/or Output Bus configuration has changed
	kParamValuesChanged	= 1 << 2,	///< multiple parameter values have changed (as result of a program change for example) 
	kLatencyChanged		= 1 << 3,	///< latency has changed (IAudioProcessor->getLatencySamples)
	kParamTitlesChanged	= 1 << 4,	///< parameter titles have changed
	kMidiCCAssignmentChanged = 1 << 5	///< Midi Controller Assignments have changed [SDK 3.0.1]
};

//------------------------------------------------------------------------
/** Host callback interface for an edit controller. 
\ingroup vstIHost
- [host imp]
- Transfer parameter editing to component via host and support automation
- Cause the host to react on configuration changes (restartComponent)

\see IEditController */
//------------------------------------------------------------------------
class IComponentHandler: public FUnknown
{
public:
//------------------------------------------------------------------------
	/** To be called before calling a performEdit (when mouse down for example). */
	virtual tresult PLUGIN_API beginEdit (ParamID tag) = 0;
	
	/** Called between beginEdit and endEdit to inform the handler that a given parameter has a new value. */
	virtual tresult PLUGIN_API performEdit (ParamID tag, ParamValue valueNormalized) = 0;
	
	/** To be called after calling a performEdit (when mouse up for example). */
	virtual tresult PLUGIN_API endEdit (ParamID tag) = 0;

	/** Instruct host to restart the component. 
	\param flags is a combination of RestartFlags */
	virtual tresult PLUGIN_API restartComponent (int32 flags) = 0;
//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IComponentHandler, 0x93A0BEA3, 0x0BD045DB, 0x8E890B0C, 0xC1E46AC6)


//------------------------------------------------------------------------
/** Edit controller component interface. 
\ingroup vstIPlug
- [plug imp]

The GUI part of an effect or instrument.
\see IComponent::getControllerClassId, IMidiMapping
*/
//------------------------------------------------------------------------
class IEditController: public IPluginBase
{
public:
//------------------------------------------------------------------------
	/** Receive the component state. */
	virtual tresult PLUGIN_API setComponentState (IBStream* state) = 0;
	
	/** Sets the controller state. */
	virtual tresult PLUGIN_API setState (IBStream* state) = 0;
	/** Gets the controller state. */
	virtual tresult PLUGIN_API getState (IBStream* state) = 0;
	
	// parameters -------------------------
	/** Returns the number of parameter exported. */
	virtual int32 PLUGIN_API getParameterCount () = 0;
	/** Gets for a given index the parameter information. */
	virtual tresult PLUGIN_API getParameterInfo (int32 paramIndex, ParameterInfo& info /*out*/) = 0;

	/** Gets for a given paramID and normalized value its associated string representation. */
	virtual tresult PLUGIN_API getParamStringByValue (ParamID tag, ParamValue valueNormalized /*in*/, String128 string /*out*/) = 0;
	/** Gets for a given paramID and string its normalized value. */
	virtual tresult PLUGIN_API getParamValueByString (ParamID tag, TChar* string /*in*/, ParamValue& valueNormalized /*out*/) = 0;

	/** Returns for a given paramID and a normalized value its plain representation (for example 90 for 90db - see \ref vst3AutomationIntro). */
	virtual ParamValue PLUGIN_API normalizedParamToPlain (ParamID tag, ParamValue valueNormalized) = 0;
	/** Returns for a given paramID and a plain value its normalized value. (see \ref vst3AutomationIntro) */
	virtual ParamValue PLUGIN_API plainParamToNormalized (ParamID tag, ParamValue plainValue) = 0;

	/** Returns the normalized value of the parameter associated to the paramID. */
	virtual ParamValue PLUGIN_API getParamNormalized (ParamID tag) = 0;
	/** Sets the normalized value to the parameter associated to the paramID. The controller must never
	    pass this value change back to the host via the IComponentHandler. It should update the according
		GUI element(s) only!*/
	virtual tresult PLUGIN_API setParamNormalized (ParamID tag, ParamValue value) = 0;

	// handler ----------------------------
	/** Gets from host a handler. */
	virtual tresult PLUGIN_API setComponentHandler (IComponentHandler* handler) = 0;

	// view -------------------------------
	/** Create the editor view of the plug-in, currently only "editor" is supported, see \ref ViewType */
	virtual IPlugView* PLUGIN_API createView (const char* name) = 0;	

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IEditController, 0xDCD7BBE3, 0x7742448D, 0xA874AACC, 0x979C759E)


//------------------------------------------------------------------------
/** Midi Mapping Interface.
\ingroup vstIPlug
- [plug imp]
- [extends IEditController]
- [released: 3.0.1]

MIDI controllers are not transmitted directly to a VST component. MIDI as hardware protocol has 
restrictions that can be avoided in software. Controller data in particular come along with unclear 
and often ignored semantics. On top of this they can interfere with regular parameter automation and
the host is unaware of what happens in the plug-in when passing MIDI controllers directly. 

So any functionality that is to be controlled by MIDI controllers must be exported as regular parameter.
The host will transform incoming MIDI controller data using this interface and transmit them as normal
parameter change. This allows the host to automate them in the same way as other parameters. */
//------------------------------------------------------------------------
class IMidiMapping: public FUnknown
{
public:
	
	/** Gets an (preferred) associated ParamID for a given Input Event Bus index, channel and MIDI Controller. */
	virtual tresult PLUGIN_API getMidiControllerAssignment (int32 busIndex, int16 channel, CtrlNumber midiControllerNumber, ParamID& tag/*out*/) = 0;

	//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IMidiMapping, 0xDF0FF9F7, 0x49B74669, 0xB63AB732, 0x7ADBF5E5)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpop.h"
//------------------------------------------------------------------------

#endif // __ivsteditcontroller__
