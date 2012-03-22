//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Helpers
// Filename    : vstcomponentbase.h
// Created by  : Steinberg, 05/2005
// Description : Base class for Component and Edit Controller
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

#ifndef __vstcomponentbase__
#define __vstcomponentbase__

#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/ivstmessage.h"
#include "pluginterfaces/vst/ivsthostapplication.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Base class for VST3 Component and Edit Controller. 
\ingroup vstClasses */
//------------------------------------------------------------------------
class ComponentBase: public IPluginBase,
	                 public IConnectionPoint
{
public:
//------------------------------------------------------------------------
	ComponentBase ();
	virtual ~ComponentBase ();

	//--- Internal Methods------
	/** Returns the hostContext (set by the host during initialize call). */
	FUnknown* getHostContext () { return hostContext; }

	/** Returns the peer for the messaging communication. */
	IConnectionPoint* getPeer () { return peerConnection; }

	/** Allocates a message instance (don't forget to release it). */
	IMessage* allocateMessage ();
	/** Sends the given message to the peer.  */
	tresult sendMessage (IMessage* message);
	/** Sends a simple text message to the peer. */
	tresult sendTextMessage (const char* text);
	/** Receives a simple text message from the peer. */
	virtual tresult receiveText (const char* text);

	//---from FUnknown------
	DECLARE_FUNKNOWN_METHODS

	//---from IPluginBase------
	virtual tresult PLUGIN_API initialize (FUnknown* context);
	virtual tresult PLUGIN_API terminate ();

	//---from IConnectionPoint-----------
	tresult PLUGIN_API connect (IConnectionPoint* other);
	tresult PLUGIN_API disconnect (IConnectionPoint* other);
	tresult PLUGIN_API notify (IMessage* message);
//------------------------------------------------------------------------
protected:
	FUnknown* hostContext;
	IConnectionPoint* peerConnection;
};

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

#endif	// __vstcomponentbase__
