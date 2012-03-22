//------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0.1
//
// Category    : Helpers
// Filename    : vstparameters.h
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

#ifndef __vstparameters__
#define __vstparameters__

#include "public.sdk/source/common/array.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstunits.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {


//------------------------------------------------------------------------
/** Description of a Parameter.
\ingroup vstClasses */
//------------------------------------------------------------------------
class Parameter: public CArrayElement
{
public:
//------------------------------------------------------------------------
	Parameter ();
	Parameter (const ParameterInfo&);
	virtual ~Parameter ();
	
	/** Returns its info. */
	const ParameterInfo& getInfo () const {return info;}
	
	/** Sets its associated UnitId */
	void setUnitID (UnitID id) {info.unitId = id;}
	/** Gets its associated UnitId */
	UnitID getUnitID () {return info.unitId;}

	/** Gets its normalized value [0.0, 1.0] */
	ParamValue getNormalized () const { return valueNormalized; }
	/** Sets its normalized value [0.0, 1.0] */
	bool setNormalized (ParamValue v);

	/** Converts a normalized value to a string */
	virtual void toString (ParamValue valueNormalized, String128 string) const;
	/** Converts a string to a normalized value */
	virtual bool fromString (const TChar* string, ParamValue& valueNormalized) const;
	
	/** Converts a normalized value to plain value (ex. 0.5 to 10000.0Hz) */
	virtual ParamValue toPlain (ParamValue valueNormalized) const;
	/** Converts a plain value to a normalized value (ex. 10000 to 0.5) */
	virtual ParamValue toNormalized (ParamValue plainValue) const;

//------------------------------------------------------------------------
protected:
	ParameterInfo info;
	ParamValue valueNormalized;
};

//------------------------------------------------------------------------
/** Collection of parameters.
\ingroup vstClasses */
//------------------------------------------------------------------------
class ParameterContainer
{
public:
//------------------------------------------------------------------------
	ParameterContainer ();

	/** Creates and adds a new parameter from a ParameterInfo. */
	Parameter* addParameter (const ParameterInfo& info);

	/** Creates and adds a new parameter with given properties. */
	Parameter* addParameter (const TChar* title, const TChar* units = 0, 
							 int32 stepCount = 0, 
							 ParamValue defaultValueNormalized = 0., 
							 int32 flags = ParameterInfo::kCanAutomate, 
							 int32 tag = -1,
							 UnitID unitID = kRootUnitId);
	
	/** Adds a given parameter. */
	bool addParameter (Parameter* p) { return params.add (p); }

	/** Returns the count of parameters. */
	int32 getParameterCount () const { return params.total (); }
	
	/** Gets parameter by index. */
	Parameter* getParameterByIndex (int32 index) { return (Parameter*)params.at (index); }

	/** Removes all parameters. */
	void removeAll () { params.removeAll (); }

	/** Gets parameter by ID. */
	Parameter* getParameter (ParamID tag);

//------------------------------------------------------------------------
protected:
	CArray params;
};


//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

#endif	// __vsteditcontroller__
