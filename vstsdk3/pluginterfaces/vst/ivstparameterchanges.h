//------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Interfaces
// Filename    : ivstparameterchanges.h
// Created by  : Steinberg, 09/2005
// Description : VST Parameter Change Interfaces
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

#ifndef __ivstparameterchanges__
#define __ivstparameterchanges__

#include "pluginterfaces/base/funknown.h"
#include "vsttypes.h"

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpush.h"
//------------------------------------------------------------------------

//----------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//----------------------------------------------------------------------
/** Queue of changes for a specific parameter.
\ingroup vstIHost
- [host imp]

The change queue can be interpreted as segment of an automation curve. For each 
processing block a segment in the size of the block is transmitted to the processor. 
The curve is expressed as sampling points of a linear approximation of
the original automation curve. If the original IS a linear curve it can
be transmitted precisely. A non-linear curve has to be converted to a linear
approximation by the host. Every point of the value queue defines a linear
section of the curve as a straight line from the previous point of a block to
the new one. So the plug-in can calculate the value of the curve for any sample 
position in the block. 

<b>Implicit Points:</b> \n
In order to reduce the amount of transmitted points, the first point at block 
position 0 can be omitted. When the queue does not contain an initial point, 
the processor can assumes an implicit point with the current value of the 
parameter at position 0. So when the curve has a slope of 1 no points have to 
be transmitted. If the curve has a constant slope other than 1 over the period
of several blocks, only the value for the last sample of the block has to be transmitted.

<b>Jumps:</b> \n
A jump in the automation curve has to be transmitted as two points: one with the
old value and one with the new value at the next sample position. 

\image html "automation.jpg"
\see IParameterChanges, ProcessData
*/
//----------------------------------------------------------------------
class IParamValueQueue: public FUnknown
{
public:
//------------------------------------------------------------------------
	/** Returns its associated ID. */
	virtual ParamID PLUGIN_API getParameterId () = 0;

	/** Returns count of Point in the queue. */
	virtual int32 PLUGIN_API getPointCount () = 0;
	
	/** Gets the value and offset at a given index. */
	virtual tresult PLUGIN_API getPoint (int32 index, int32& sampleOffset /*out*/, ParamValue& value /*out*/) = 0;
	
	/** Adds a new value at the end of the queue, its index is returned. */
	virtual tresult PLUGIN_API addPoint (int32 sampleOffset, ParamValue value, int32& index /*out*/) = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IParamValueQueue, 0x01263A18, 0xED074F6F, 0x98C9D356, 0x4686F9BA)


//----------------------------------------------------------------------
/** All parameter changes of a processing block.
\ingroup vstIHost
- [host imp]

This interface is used to transmit any changes that should happen to paramaters
in the current processing block. A change can be caused by GUI interaction as
well as automation. They are transmitted as a list of queues (IParamValueQueue) 
containing only queues for paramaters that actually changed.
\see IParamValueQueue, ProcessData */
//----------------------------------------------------------------------
class IParameterChanges: public FUnknown
{
public:
//------------------------------------------------------------------------
	/** Returns count of Parameter changes in the list. */
	virtual int32 PLUGIN_API getParameterCount () = 0;

	/** Returns the queue at a given index. */
	virtual IParamValueQueue* PLUGIN_API getParameterData (int32 index) = 0;

	/** Adds a new parameter queue with a given ID at the end of the list, 
	returns it and its index in the parameter changes list. */
	virtual IParamValueQueue* PLUGIN_API addParameterData (const Vst::ParamID& id, int32& index /*out*/) = 0; 

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IParameterChanges, 0xA4779663, 0x0BB64A56, 0xB44384A8, 0x466FEB9D)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpop.h"
//------------------------------------------------------------------------

#endif // __ivstparameterchanges__
