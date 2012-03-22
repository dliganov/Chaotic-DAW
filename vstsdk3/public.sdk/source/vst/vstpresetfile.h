//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Helpers
// Filename    : vstpresetfile.h
// Created by  : Steinberg, 03/2006
// Description : VST3 Preset File Format
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
// contributors may be used to endorse or promote products derived from this software without specific prior written permission.
// 
// THIS SDK IS PROVIDED BY STEINBERG MEDIA TECHNOLOGIES GMBH "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//
// IN NO EVENT SHALL STEINBERG MEDIA TECHNOLOGIES GMBH BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef __vstpresetfile__
#define __vstpresetfile__

#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstunits.h"

#include "pluginterfaces/base/ibstream.h"

#include <cstdio>

//------------------------------------------------------------------------
/** \page vst3presetFileFormat VST3 Preset File Format Definition
\code

   VST3 Preset File Format Definition
   ==================================

0   +---------------------------+
    | HEADER                    |
    | header id ('VST3')        |       4 Bytes
    | version                   |       4 Bytes (int32)
    | ASCII-encoded class id    |       32 Bytes 
 +--| offset to chunk list      |       8 Bytes (int64)
 |  +---------------------------+
 |  | DATA AREA                 |<-+
 |  | data of chunks 1..n       |  |
 |  ...                       ...  |
 |  |                           |  |
 +->+---------------------------+  |
    | CHUNK LIST                |  |
    | list id ('List')          |  |    4 Bytes
    | entry count               |  |    4 Bytes (int32)
    +---------------------------+  |
    |  1..n                     |  |
    |  +----------------------+ |  |
    |  | chunk id             | |  |    4 Bytes
    |  | offset to chunk data |----+    8 Bytes (int64)
    |  | size of chunk data   | |       8 Bytes (int64)
    |  +----------------------+ |
EOF +---------------------------+

\endcode
\see \ref Steinberg::Vst::PresetFile
*/
//------------------------------------------------------------------------

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
typedef char ChunkID[4];

//------------------------------------------------------------------------
enum ChunkType
{
	kHeader,
	kComponentState,
	kControllerState,
	kProgramData,
	kMetaInfo,
	kChunkList,
	kNumPresetChunks
};

//------------------------------------------------------------------------
extern const ChunkID& getChunkID (ChunkType type);

//------------------------------------------------------------------------
inline bool isEqualID (const ChunkID id1, const ChunkID id2)
{
	return memcmp (id1, id2, sizeof(ChunkID)) == 0;
}

//------------------------------------------------------------------------
/** Handeler for a VST3 Preset File.
\ingroup vstClasses 
\see \ref vst3presetFileFormat */
//------------------------------------------------------------------------
class PresetFile
{
public:
//------------------------------------------------------------------------
	PresetFile (IBStream* stream);
	virtual ~PresetFile ();

	struct Entry
	{
		ChunkID id;
		TSize offset;
		TSize size;
	};

	IBStream* getStream () { return stream; }

	const FUID& getClassID () const { return classID; }
	void setClassID (const FUID& uid) { classID = uid; }

	const Entry* getEntry (ChunkType which) const;
	const Entry* getLastEntry () const;
	int32 getEntryCount () const { return entryCount; }
	const Entry& at (int32 index) const { return entries[index]; }
	bool contains (ChunkType which) const { return getEntry (which) != 0; }

	bool readChunkList ();
	bool writeHeader ();
	bool writeChunkList ();

	bool readMetaInfo (char* xmlBuffer, int32& size);
	bool writeMetaInfo (const char* xmlBuffer, int32 size = -1); // -1 means null-terminated
	bool prepareMetaInfoUpdate ();

	// use if data already exists (e.g. convert from other formats)
	bool writeChunk (const void* data, int32 size, ChunkType which = kComponentState); 

	bool storeComponentState (IComponent* component);
	bool restoreComponentState (IComponent* component);
	bool restoreComponentState (IEditController* editController);

	bool storeControllerState (IEditController* editController);
	bool restoreControllerState (IEditController* editController);

	bool storeProgramData (IBStream* stream, ProgramListID listID, int32 programIndex);
	bool storeProgramData (IProgramListData* unitData, ProgramListID listID, int32 programIndex);
	bool restoreProgramData (IProgramListData* unitData, ProgramListID* listID = 0, int32 programIndex = 0);
	bool restoreProgramData (IUnitInfo* unitInfo, ProgramListID listID, int32 programIndex);

//------------------------------------------------------------------------
	// shortcut to create preset from component/controller state
	static bool savePreset (IBStream* stream, 
							const FUID& classID, 
							IComponent* component, 
							IEditController* editController = 0,
							const char* xmlBuffer = 0,
							int32 xmlSize = -1);

	// shortcut to load preset with component/controller state
	static bool loadPreset (IBStream* stream, 
							const FUID& classID, 
							IComponent* component, 
							IEditController* editController = 0);
//------------------------------------------------------------------------
protected:
	IBStream* stream;
	FUID classID;
	enum { kMaxEntries = 128 };
	Entry entries[kMaxEntries];
	int32 entryCount;

	bool readID (ChunkID id);
	bool writeID (const ChunkID id);
	bool readEqualID (const ChunkID id);
	bool readSize (TSize& size);
	bool writeSize (TSize size);
	bool readInt32 (int32& value);
	bool writeInt32 (int32 value);
	bool seekTo (TSize offset);
	bool beginChunk (Entry& e, ChunkType which);
	bool endChunk (Entry& e);
};

//------------------------------------------------------------------------
/** Stream implementation for a file using stdio. */
//------------------------------------------------------------------------
class FileStream: public IBStream
{
public:
//------------------------------------------------------------------------
	static IBStream* open (const char* filename, const char* mode);

	// FUnknown
	DECLARE_FUNKNOWN_METHODS

	// IBStream
	tresult PLUGIN_API read (void* buffer, int32 numBytes, int32* numBytesRead = 0);
	tresult PLUGIN_API write (void* buffer, int32 numBytes, int32* numBytesWritten = 0);
	tresult PLUGIN_API seek (int64 pos, int32 mode, int64* result = 0);
	tresult PLUGIN_API tell (int64* pos);
//------------------------------------------------------------------------
protected:
	FileStream (FILE* file);
	virtual ~FileStream ();

	FILE* file;
};

//------------------------------------------------------------------------
}} // namespace Vst

#endif	// __vstpresetfile__
