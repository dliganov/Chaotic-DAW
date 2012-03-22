//------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Helpers
// Filename    : vsteditcontroller.h
// Created by  : Steinberg, 04/2005
// Description : VST Edit Controller Implementation
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

#ifndef __vsteditcontroller__
#define __vsteditcontroller__

#include "public.sdk/source/vst/vstcomponentbase.h"
#include "public.sdk/source/vst/vstparameters.h"
#include "public.sdk/source/common/pluginview.h"
#include "public.sdk/source/common/array.h"

#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstunits.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

class EditorView;

//------------------------------------------------------------------------
/** Default implementation for a VST3 edit controller.
\ingroup vstClasses 
Can be used as base class for a specific controller implementation */
//------------------------------------------------------------------------
class EditController: public ComponentBase,
					  public IEditController
{
public:
//------------------------------------------------------------------------
	EditController ();

	//---from IEditController-------
	virtual tresult PLUGIN_API setComponentState (IBStream* state);
	virtual tresult PLUGIN_API setState (IBStream* state);
	virtual tresult PLUGIN_API getState (IBStream* state);
	virtual int32 PLUGIN_API getParameterCount ();
	virtual tresult PLUGIN_API getParameterInfo (int32 paramIndex, ParameterInfo& info);
	virtual tresult PLUGIN_API getParamStringByValue (ParamID tag, ParamValue valueNormalized, String128 string);
	virtual tresult PLUGIN_API getParamValueByString (ParamID tag, TChar* string, ParamValue& valueNormalized);
	virtual ParamValue PLUGIN_API normalizedParamToPlain (ParamID tag, ParamValue valueNormalized);
	virtual ParamValue PLUGIN_API plainParamToNormalized (ParamID tag, ParamValue plainValue);
	virtual ParamValue PLUGIN_API getParamNormalized (ParamID tag);
	virtual tresult PLUGIN_API setParamNormalized (ParamID tag, ParamValue value);
	virtual tresult PLUGIN_API setComponentHandler (IComponentHandler* handler);
	virtual IPlugView* PLUGIN_API createView (const char* name);

	//---from ComponentBase---------
	DELEGATE_REFCOUNT (ComponentBase)
	virtual tresult PLUGIN_API queryInterface (const char* iid, void** obj);
	virtual tresult PLUGIN_API initialize (FUnknown* context);
	virtual tresult PLUGIN_API terminate  ();

	//---Internal Methods-------
	virtual tresult beginEdit (ParamID tag);
	virtual tresult performEdit (ParamID tag, ParamValue valueNormalized);
	virtual tresult endEdit (ParamID tag);
	
	virtual void editorDestroyed (EditorView* /*editor*/) {}
	virtual void editorAttached (EditorView* /*editor*/) {}
	virtual void editorRemoved (EditorView* /*editor*/) {}

//------------------------------------------------------------------------
protected:
	IComponentHandler* componentHandler;
	ParameterContainer parameters;
};


//------------------------------------------------------------------------
/** View related to an edit controller.
\ingroup vstClasses  */
//------------------------------------------------------------------------
class EditorView: public CPluginView
{
public:
//------------------------------------------------------------------------
	EditorView (EditController* controller, ViewRect* size = 0);
	virtual ~EditorView ();

	/** Gets its controller part */
	EditController* getController () { return controller; }	

	//---from CPluginView-------------
	virtual void attachedToParent ();
	virtual void removedFromParent ();

//------------------------------------------------------------------------
protected:
	EditController* controller;
};


//------------------------------------------------------------------------
/** Unit element.
\ingroup vstClasses  */
//------------------------------------------------------------------------
class Unit: public CArrayElement
{
public:
//------------------------------------------------------------------------
	Unit ();
	Unit (const UnitInfo& unit);
	virtual ~Unit ();
	
	/** Returns its info. */
	const UnitInfo& getInfo () const {return info;}

	/** Returns its Unit ID. */
	UnitID getID () {return info.id;}

	/** Returns its Unit Name. */
	const TChar* getName () {return info.name;}

//------------------------------------------------------------------------
protected:
	UnitInfo info;
};


//------------------------------------------------------------------------
/** Advanced implementation (support IUnitInfo) for a VST3 edit controller.
\ingroup vstClasses 
- [extends EditController]
*/
//------------------------------------------------------------------------
class EditControllerEx1: public EditController, public IUnitInfo
{
public:
	EditControllerEx1 ();
	virtual ~EditControllerEx1 ();

	/** Adds a given unit. */
	bool addUnit (Unit* unit) {return units.add (unit);}

	//---from IUnitInfo------------------
	virtual int32 PLUGIN_API getUnitCount () {return units.total ();}
	virtual tresult PLUGIN_API getUnitInfo (int32 unitIndex, UnitInfo& info /*out*/);

	virtual int32 PLUGIN_API getProgramListCount () {return 0;}
	virtual tresult PLUGIN_API getProgramListInfo (int32 listIndex, ProgramListInfo& info /*out*/) {return kResultFalse;}
	virtual tresult PLUGIN_API getProgramName (ProgramListID listId, int32 programIndex, String128 name /*out*/) {return kResultFalse;}
	virtual tresult PLUGIN_API getProgramInfo (ProgramListID listId, int32 programIndex, 
		CString attributeId /*in*/, String128 attributeValue /*out*/) {return kResultFalse;}

	virtual tresult PLUGIN_API hasProgramPitchNames (ProgramListID listId, int32 programIndex) {return kResultFalse;}
	virtual tresult PLUGIN_API getProgramPitchName (ProgramListID listId, int32 programIndex,
		int16 midiPitch, String128 name /*out*/) {return kResultFalse;}

	// units selection --------------------
	virtual UnitID PLUGIN_API getSelectedUnit () {return selectedUnit;}
	virtual tresult PLUGIN_API selectUnit (UnitID unitId) {return selectedUnit = unitId;}

	virtual tresult PLUGIN_API getUnitByBus (MediaType type, BusDirection dir, int32 busIndex,
		int32 channel, UnitID& unitId /*out*/) {return kResultFalse;}
	virtual tresult PLUGIN_API setUnitProgramData (int32 listOrUnitId, int32 programIndex, IBStream* data) {return kResultFalse;}

	//-----------------------------
	DELEGATE_REFCOUNT (EditController)
	tresult PLUGIN_API queryInterface (const char* iid, void** obj);

protected:
	CArray units;
	UnitID selectedUnit;
};


//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

#endif	// __vsteditcontroller__
