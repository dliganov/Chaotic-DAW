//------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Interfaces
// Filename    : ivstevents.h
// Created by  : Steinberg, 11/2005
// Description : VST Events (Midi) Interfaces
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

#ifndef __ivstevents__
#define __ivstevents__

#include "pluginterfaces/vst/ivstprocesscontext.h"

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpush.h"
//------------------------------------------------------------------------

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

/** Note-on event specific data. Used in Event (union)*/
struct NoteOnEvent 
{
	int16 channel;			///< channel index in event bus
	int16 pitch;			///< range [0, 127] = [C-2, G8] with A3=440Hz
	float tuning;			///< 1.f = +1 cent, -1.f = -1 cent 
	float velocity;			///< range [0.0, 1.0]
	int32 length;           ///< in sample frames (optional, Note Off has to follow in any case!)
	int32 noteId;			///< note identifier
};

/** Note-off event specific data. Used in Event (union)*/
struct NoteOffEvent
{
	int16 channel;			///< channel index in event bus
	int16 pitch;			///< range [0, 127] = [C-2, G8] with A3=440Hz
	float velocity;			///< range [0.0, 1.0]
	int32 noteId;			///< note identifier
};

/** Data event specific data. Used in Event (union)*/
struct DataEvent
{
	uint32 size;			///< size of the bytes
	uint32 type;			///< type of this data block (see \ref DataTypes)
	const uint8* bytes;		///< pointer to the data block

	/** Value for DataEvent::type */
	enum DataTypes
	{
		kMidiSysEx = 0		///< for MIDI system exclusive message
	};
};

//------------------------------------------------------------------------
/** Event */
//------------------------------------------------------------------------
struct Event
{
	int32 busIndex;				///< event bus index
	int32 sampleOffset;			///< sample frames related to the current block start sample position
	TQuarterNotes ppqPosition;	///< position in project
	uint16 flags;				///< combination of \ref EventFlags
	
	/** Event Flags - used for Event::flags */
	enum EventFlags
	{
		kIsLive = 1 << 0		///< indicates that the event is played live (direct from keyboard)
	};

	/**  Event Types - used for Event::type */
	enum EventTypes
	{
		kNoteOnEvent = 0,		///< is \ref NoteOnEvent
		kNoteOffEvent,			///< is \ref NoteOffEvent
		kDataEvent				///< is \ref DataEvent
	};

	uint16 type;				///< a value from \ref EventTypes
	union
	{
		NoteOnEvent noteOn;    ///< type == kNoteOnEvent
		NoteOffEvent noteOff;  ///< type == kNoteOffEvent
		DataEvent data;        ///< type == kDataEvent
	};
};


//------------------------------------------------------------------------
/** List of events to process.
\ingroup vstIHost
- [host imp]

\see ProcessData, Event */
//------------------------------------------------------------------------
class IEventList: public FUnknown
{
public:
//------------------------------------------------------------------------
	/** Returns the count of events. */
	virtual int32 PLUGIN_API getEventCount () = 0;

	/** Gets parameter by index. */
	virtual tresult PLUGIN_API getEvent (int32 index, Event& e /*out*/) = 0;
	
	/** Adds a new event. */
	virtual tresult PLUGIN_API addEvent (Event& e /*in*/) = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IEventList, 0x3A2C4214, 0x346349FE, 0xB2C4F397, 0xB9695A44)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpop.h"
//------------------------------------------------------------------------

#endif // __ivstevents__
