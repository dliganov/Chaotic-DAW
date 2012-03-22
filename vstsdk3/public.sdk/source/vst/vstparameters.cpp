//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0.1
//
// Category    : Helpers
// Filename    : vstparameters.cpp
// Created by  : Steinberg, 03/2008
// Description : VST Parameter Implementation
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

#include "vstparameters.h"

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
// Parameter Implementation
//------------------------------------------------------------------------
Parameter::Parameter ()
: valueNormalized (0.)
{
	memset (&info, 0, sizeof (ParameterInfo));
}

//------------------------------------------------------------------------
Parameter::Parameter (const ParameterInfo& info)
: info (info)
, valueNormalized (info.defaultNormalizedValue)
{}

//------------------------------------------------------------------------
Parameter::~Parameter ()
{}
	
//------------------------------------------------------------------------
bool Parameter::setNormalized (ParamValue normValue)
{
	if (normValue > 1.0)
	{
		normValue = 1.0;
	}
	else if (normValue < 0.)
	{
		normValue = 0.;
	}
	
	if (normValue != valueNormalized)
	{
		valueNormalized = normValue;
		return true;	
	}
	return false;
}

//------------------------------------------------------------------------
void Parameter::toString (ParamValue normValue, String128 string) const
{
	Steinberg::UString wrapper (string, USTRINGSIZE (String128));
	if (info.stepCount == 1)
	{
		if (valueNormalized > 0.5)
		{
			wrapper.fromAscii ("On");
		}
		else
		{
			wrapper.fromAscii ("Off");
		}
	}
	else
	{
		wrapper.printFloat (normValue);
	}
}

//------------------------------------------------------------------------
bool Parameter::fromString (const TChar* string, ParamValue& normValue) const
{
	Steinberg::UString wrapper ((TChar*)string, -1); // don't know buffer size here!
	return wrapper.scanFloat (normValue);
}

//------------------------------------------------------------------------
ParamValue Parameter::toPlain (ParamValue normValue) const
{
	return normValue;
}

//------------------------------------------------------------------------
ParamValue Parameter::toNormalized (ParamValue plainValue) const
{
	return plainValue;
}

//------------------------------------------------------------------------
// ParameterContainer Implementation
//------------------------------------------------------------------------
ParameterContainer::ParameterContainer ()
: params (10)
{}

//------------------------------------------------------------------------
Parameter* ParameterContainer::addParameter (const ParameterInfo& info)
{
	Parameter* p = new Parameter (info);
	params.add (p);
	return p;
}

//------------------------------------------------------------------------
Parameter* ParameterContainer::getParameter (ParamID tag)
{
	for (int32 i = 0; i < params.total (); i++)
	{
		Parameter* param = static_cast<Parameter*> (params.at (i));
		if (param)
		{
			ParameterInfo info = param->getInfo ();
			if (info.id == tag)
			{
				return param;
			}
		}
	}
	return 0;
}

//------------------------------------------------------------------------
Parameter* ParameterContainer::addParameter (const TChar* title, const TChar* units,
											 int32 stepCount, ParamValue defaultNormalizedValue,
											 int32 flags, int32 tag, UnitID unitID)
{
	if (!title)
	{
		return 0;
	}

	ParameterInfo info = {0};

	Steinberg::UString (info.title, USTRINGSIZE (info.title)).assign (title);
	Steinberg::UString uUnits (info.units, USTRINGSIZE (info.units));
	if (units)
	{
		uUnits.assign (units);
	}

	info.stepCount = stepCount;
	info.defaultNormalizedValue = defaultNormalizedValue;
	info.flags = flags;
	info.id = (tag >= 0) ? tag : getParameterCount ();
	info.unitId = unitID;
	
	return addParameter (info);
}


}} // namespace
