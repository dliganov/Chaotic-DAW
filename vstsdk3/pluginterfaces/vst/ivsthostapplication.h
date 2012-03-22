//------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Interfaces
// Filename    : ivsthostapplication.h
// Created by  : Steinberg, 04/2006
// Description : VST Host Interface
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

#ifndef __ivsthostapplication__
#define __ivsthostapplication__

#include "pluginterfaces/vst/ivstmessage.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Basic Host Callback Interface.
\ingroup vstIHost
- [host imp]
- [passed as 'context' in to IPluginBase::initialize () ] 

Basic VST host application interface. */
//------------------------------------------------------------------------
class IHostApplication: public FUnknown
{
public:
//------------------------------------------------------------------------	
	/** Gets host application name. */
	virtual tresult PLUGIN_API getName (String128 name) = 0;

	/** Create host object (e.g. Vst::IMessage). */
	virtual tresult PLUGIN_API createInstance (TUID cid, TUID iid, void** obj) = 0;

//------------------------------------------------------------------------	
	static const FUID iid;
};

DECLARE_CLASS_IID (IHostApplication, 0x58E595CC, 0xDB2D4969, 0x8B6AAF8C, 0x36A664E5)

//------------------------------------------------------------------------
inline IMessage* allocateMessage (IHostApplication* host)
{
	TUID iid;
	IMessage::iid.toTUID (iid);
	IMessage* m = 0;
	if (host->createInstance (iid, iid, (void**)&m) == kResultOk)
		return m;
	return 0;
}

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

#endif // __ivsthostapplication__
