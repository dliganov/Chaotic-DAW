//------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Interfaces
// Filename    : ivstattributes.h
// Created by  : Steinberg, 05/2006
// Description : VST Attribute Interfaces
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

#ifndef __ivstattributes__
#define __ivstattributes__

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpush.h"
//------------------------------------------------------------------------

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Attribute list used in IMessage and IStreamAttributes.
\ingroup vstIHost
- [host imp] 

An attribute list associates values with a key (id).*/
//------------------------------------------------------------------------
class IAttributeList: public FUnknown
{
public:
//------------------------------------------------------------------------
	typedef const char* AttrID;

	/** Set integer value. */
	virtual tresult PLUGIN_API setInt (AttrID id, int64 value) = 0;
	
	/** Gets integer value. */
	virtual tresult PLUGIN_API getInt (AttrID id, int64& value) = 0;
	
	/** Set float value. */
	virtual tresult PLUGIN_API setFloat (AttrID id, double value) = 0;
	
	/** Gets float value. */
	virtual tresult PLUGIN_API getFloat (AttrID id, double& value) = 0;
	
	/** Set string value (UTF16). */
	virtual tresult PLUGIN_API setString (AttrID id, const TChar* string) = 0;
	
	/** Gets string value (UTF16). */
	virtual tresult PLUGIN_API getString (AttrID id, TChar* string, uint32 size) = 0;
	
	/** Set binary data. */
	virtual tresult PLUGIN_API setBinary (AttrID id, const void* data, uint32 size) = 0;
	
	/** Gets binary data. */
	virtual tresult PLUGIN_API getBinary (AttrID id, const void*& data, uint32& size) = 0;
//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IAttributeList, 0x1E5F0AEB, 0xCC7F4533, 0xA2544011, 0x38AD5EE4)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpop.h"
//------------------------------------------------------------------------

#endif // __ivstattributes__
