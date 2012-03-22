//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Helpers
// Filename    : vsteditcontroller.cpp
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

#include "vsteditcontroller.h"

#include <stdio.h>

namespace Steinberg {
namespace Vst {


//------------------------------------------------------------------------
// EditController Implementation
//------------------------------------------------------------------------
EditController::EditController ()
: componentHandler (0)
{}

//------------------------------------------------------------------------
tresult PLUGIN_API EditController::queryInterface (const char* iid, void** obj)
{
	QUERY_INTERFACE (iid, obj, IEditController::iid, IEditController)
	return ComponentBase::queryInterface (iid, obj);
}

//------------------------------------------------------------------------
tresult PLUGIN_API EditController::initialize (FUnknown* context)
{
	return ComponentBase::initialize (context);
}

//------------------------------------------------------------------------
tresult PLUGIN_API EditController::terminate ()
{
	parameters.removeAll ();

	if (componentHandler)
	{
		componentHandler->release ();
		componentHandler = 0;
	}

	return ComponentBase::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API EditController::setComponentState (IBStream* state)
{
	return kNotImplemented;
}

//------------------------------------------------------------------------
tresult PLUGIN_API EditController::setState (IBStream* state)
{
	return kNotImplemented;
}

//------------------------------------------------------------------------
tresult PLUGIN_API EditController::getState (IBStream* state)
{
	return kNotImplemented;
}

//------------------------------------------------------------------------
int32 PLUGIN_API EditController::getParameterCount ()
{
	return parameters.getParameterCount ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API EditController::getParameterInfo (int32 paramIndex, ParameterInfo& info)
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
tresult PLUGIN_API EditController::getParamStringByValue (ParamID tag, ParamValue valueNormalized, String128 string)
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
tresult PLUGIN_API EditController::getParamValueByString (ParamID tag, TChar* string, ParamValue& valueNormalized)
{
	Parameter* parameter = parameters.getParameter (tag);
	if (parameter)
	{
		if (parameter->fromString (string, valueNormalized))
		{
			return kResultTrue;
		}
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
ParamValue PLUGIN_API EditController::normalizedParamToPlain (ParamID tag, ParamValue valueNormalized)
{
	Parameter* parameter = parameters.getParameter (tag);
	if (parameter)
	{
		return parameter->toPlain (valueNormalized);
	}
	return valueNormalized;
}

//------------------------------------------------------------------------
ParamValue PLUGIN_API EditController::plainParamToNormalized (ParamID tag, ParamValue plainValue)
{
	Parameter* parameter = parameters.getParameter (tag);
	if (parameter)
	{
		return parameter->toNormalized (plainValue);
	}
	return plainValue;
}

//------------------------------------------------------------------------
ParamValue PLUGIN_API EditController::getParamNormalized (ParamID tag)
{
	Parameter* parameter = parameters.getParameter (tag);
	if (parameter)
	{
		return parameter->getNormalized ();
	}
	return 0.0;
}

//------------------------------------------------------------------------
tresult PLUGIN_API EditController::setParamNormalized (ParamID tag, ParamValue value)
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
tresult PLUGIN_API EditController::setComponentHandler (IComponentHandler* newHandler)
{
	if (componentHandler == newHandler)
	{
		return kResultTrue;
	}

	if (componentHandler)
	{
		componentHandler->release ();
	}

	componentHandler = newHandler;
	if (componentHandler)
	{
		componentHandler->addRef ();
	}
		
	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API EditController::createView (const char* name)
{
	return 0;
}

//------------------------------------------------------------------------
tresult EditController::beginEdit (ParamID tag)
{
	if (componentHandler)
	{
		return componentHandler->beginEdit (tag);
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult EditController::performEdit (ParamID tag, ParamValue valueNormalized)
{
	if (componentHandler)
	{
		return componentHandler->performEdit (tag, valueNormalized);
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult EditController::endEdit (ParamID tag)
{
	if (componentHandler)
	{
		return componentHandler->endEdit (tag);
	}
	return kResultFalse;
}


//------------------------------------------------------------------------
// EditorView Implementation
//------------------------------------------------------------------------
EditorView::EditorView (EditController* controller, ViewRect* size)
: CPluginView (size)
, controller (controller)
{
	if (controller)
	{
		controller->addRef ();
	}
}

//------------------------------------------------------------------------
EditorView::~EditorView ()
{
	if (controller)
	{
		controller->editorDestroyed (this);
		controller->release ();
	}
}

//------------------------------------------------------------------------
void EditorView::attachedToParent ()
{
	if (controller)
	{
		controller->editorAttached (this);
	}
}

//------------------------------------------------------------------------
void EditorView::removedFromParent ()
{
	if (controller)
	{
		controller->editorRemoved (this);
	}
}


//------------------------------------------------------------------------
// EditControllerEx1 implementation
//------------------------------------------------------------------------
EditControllerEx1::EditControllerEx1 ()
: selectedUnit (kRootUnitId)
{
}

//------------------------------------------------------------------------
EditControllerEx1::~EditControllerEx1 ()
{
	units.removeAll ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API EditControllerEx1::queryInterface (const char* iid, void** obj)
{
	QUERY_INTERFACE (iid, obj, IUnitInfo::iid, IUnitInfo)
	return EditController::queryInterface (iid, obj);
}

//------------------------------------------------------------------------
tresult PLUGIN_API EditControllerEx1::getUnitInfo (int32 unitIndex, UnitInfo& info /*out*/)
{
	Unit* unit = static_cast<Unit*> (units.at (unitIndex));
	if (unit)
	{
		info = unit->getInfo ();
		return kResultTrue;
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
// Unit implementation
//------------------------------------------------------------------------
Unit::Unit ()
{
	memset (&info, 0, sizeof (UnitInfo));
}

//------------------------------------------------------------------------
Unit::Unit (const UnitInfo& info)
: info (info)
{}

//------------------------------------------------------------------------
Unit::~Unit ()
{}

}} // namespace
