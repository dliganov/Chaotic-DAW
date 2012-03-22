//------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Helpers
// Filename    : vstguieditor.h
// Created by  : Steinberg, 04/2005
// Description : VSTGUI Editor
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

#include "vstguieditor.h"

#if MAC
void* VSTGUI::gBundleRef = 0;
extern CFBundleRef ghInst;
#endif

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
VSTGUIEditor::VSTGUIEditor (void* controller, ViewRect* size) 
: EditorView (static_cast<EditController*>(controller), size) 
{
#if MAC
	VSTGUI::gBundleRef = ghInst;
#endif

	// create a timer used for idle update: will call notify method
	timer = new CVSTGUITimer (dynamic_cast<CBaseObject*>(this));
}

//------------------------------------------------------------------------
VSTGUIEditor::~VSTGUIEditor ()
{
	if (timer)
		timer->forget ();
}

//------------------------------------------------------------------------
void VSTGUIEditor::setIdleRate (int millisec)
{
	if (timer)
		timer->setFireTime (millisec);
}

//------------------------------------------------------------------------
tresult PLUGIN_API VSTGUIEditor::isPlatformTypeSupported (FIDString type)
{
#if WINDOWS
	if (strcmp (type, kPlatformTypeHWND) == 0)
		return kResultTrue;

#elif MAC
	if (strcmp (type, kPlatformTypeHIView) == 0)
		return kResultTrue;

	// VSTGUI does not support NSViews yet
	if (strcmp (type, kPlatformTypeNSView) == 0)
		return kResultFalse;
#endif
	
	return kNotImplemented;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VSTGUIEditor::attached (void* parent, FIDString type)
{
	if (timer)
		timer->start ();

	if (open (parent) == true && plugFrame)
	{
		ViewRect vr (0, 0, frame->getWidth (), frame->getHeight ());
		setRect (vr);
		plugFrame->resizeView (this, &vr);
	}
	return EditorView::attached (parent, type);
}

//------------------------------------------------------------------------
tresult PLUGIN_API VSTGUIEditor::removed ()
{
	if (timer)
		timer->stop ();

	close ();
	return EditorView::removed ();
}

//------------------------------------------------------------------------
void VSTGUIEditor::beginEdit (long index)
{
	if (controller)
		controller->beginEdit (index);
}

//------------------------------------------------------------------------
void VSTGUIEditor::endEdit (long index)
{
	if (controller)
		controller->endEdit (index);
}

//------------------------------------------------------------------------
CMessageResult VSTGUIEditor::notify (CBaseObject* /*sender*/, const char* message)
{
	if (message == CVSTGUITimer::kMsgTimer)
	{
		if (frame)
 			frame->idle ();
 
 		return kMessageNotified;
 	}
 
 	return kMessageUnknown; 
}

} // namespace Vst
} // namespace Steinberg
