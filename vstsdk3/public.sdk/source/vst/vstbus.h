//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0.1
//
// Category    : Helpers
// Filename    : vstbus.h
// Created by  : Steinberg, 03/2008
// Description : VST Bus Implementation
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

#ifndef __vstbus__
#define __vstbus__

#include "public.sdk/source/common/linkedlist.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
/** Basic Bus object.
\ingroup vstClasses */
//------------------------------------------------------------------------
class Bus: public CListElement
{
public:
//------------------------------------------------------------------------
	/** Constructor. */
	Bus (const TChar* name, BusType busType, int32 flags);
	
	/** Returns true if the bus is active. */
	TBool isActive () const { return active; }
	
	/** Activates the bus. */
	void setActive (TBool state) { active = state; }
	
	/** Gets the BusInfo of this bus. */
	virtual bool getInfo (BusInfo&);

//------------------------------------------------------------------------
protected:
	Steinberg::UString128 name;	///< name
	BusType busType;			///< main or Aux @see BusTypes
	int32 flags;				///< flags @see BusFlags
	TBool active;				///< activation state
};

//------------------------------------------------------------------------
/** Description of an Event Bus. 
\ingroup vstClasses */
//------------------------------------------------------------------------
class EventBus: public Bus
{
public:
//------------------------------------------------------------------------
	/** Constructor of an EventBus. */
	EventBus (const TChar* name, BusType busType, int32 flags, int32 channelCount);

	//---from Bus-------
	/** Gets the BusInfo of this bus. */
	virtual bool getInfo (BusInfo& info);

//------------------------------------------------------------------------
protected:
	int32 channelCount;
};

//------------------------------------------------------------------------
/** Description of an Audio Bus. 
\ingroup vstClasses */
//------------------------------------------------------------------------
class AudioBus: public Bus
{
public:
//------------------------------------------------------------------------
	AudioBus (const TChar* name, BusType busType, int32 flags, SpeakerArrangement arr);

	/** Gets the speaker arrangement defining this Audio bus. */
	SpeakerArrangement getArrangement () const { return speakerArr; }
	/** Sets the speaker arrangement defining this Audio bus. */
	void setArrangement (const SpeakerArrangement& arr) { speakerArr = arr; }

	//---from Bus---------------------
	/** Gets the BusInfo associated to this Audio bus */
	virtual bool getInfo (BusInfo& info);
//------------------------------------------------------------------------
protected:
	SpeakerArrangement speakerArr;
};

//------------------------------------------------------------------------
/** List of Busses. 
\ingroup vstClasses */
//------------------------------------------------------------------------
class BusList: public CLinkedList
{
public:
//------------------------------------------------------------------------
	/** Constructor. */
	BusList (MediaType type, BusDirection dir);

	/** Returns the BusList Type. @see MediaType */
	MediaType getType () const { return type; }

	/** Returns the BusList direction. @see BusDirection */
	BusDirection getDirection () const { return direction; }

//------------------------------------------------------------------------
protected:
	MediaType type;
	BusDirection direction;
};


//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

#endif	// __vstbus__
