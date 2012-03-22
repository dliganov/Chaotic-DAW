//------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Helpers
// Filename    : vstpresetfile.cpp
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

#include "vstpresetfile.h"

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
// Preset Chunk IDs
//------------------------------------------------------------------------
static const ChunkID commonChunks[kNumPresetChunks] =
{
	{'V', 'S', 'T', '3'},	// kHeader
	{'C', 'o', 'm', 'p'},	// kComponentState
	{'C', 'o', 'n', 't'},	// kControllerState
	{'P', 'r', 'o', 'g'},	// kProgramData
	{'I', 'n', 'f', 'o'},	// kMetaInfo
	{'L', 'i', 's', 't'}	// kChunkList
};

//------------------------------------------------------------------------
// Preset Header: header id + version + class id + list offset
static const int32 kFormatVersion = 1;
static const int32 kClassIDSize = 32; // ASCII-encoded FUID
static const int32 kHeaderSize = sizeof (ChunkID) + sizeof (int32) + kClassIDSize + sizeof (TSize);
static const int32 kListOffsetPos = kHeaderSize - sizeof (TSize);

//------------------------------------------------------------------------
const ChunkID& getChunkID (ChunkType type)
{
	return commonChunks[type];
}

#ifdef verify
#undef verify
#endif

//------------------------------------------------------------------------
inline bool verify (tresult result)
{
	return result == kResultOk || result == kNotImplemented;
}

//------------------------------------------------------------------------
// PresetFile
//------------------------------------------------------------------------
bool PresetFile::savePreset (IBStream* stream, const FUID& classID, 
							 IComponent* component, IEditController* editController,
 							 const char* xmlBuffer, int32 xmlSize)
{
	PresetFile pf (stream);
	pf.setClassID (classID);
	if (!pf.writeHeader ())
		return false;

	if (!pf.storeComponentState (component))
		return false;
	
	if (editController && !pf.storeControllerState (editController))
		return false;

	if (xmlBuffer && !pf.writeMetaInfo (xmlBuffer, xmlSize))
		return false;

	return pf.writeChunkList ();
}

//------------------------------------------------------------------------
bool PresetFile::loadPreset (IBStream* stream, const FUID& classID, IComponent* component, IEditController* editController)
{
	PresetFile pf (stream);
	if (!pf.readChunkList ())
		return false;

	if (pf.getClassID () != classID)
		return false;

	if (!pf.restoreComponentState (component))
		return false;

	if (editController)
	{
		// assign component state to controller
		if (!pf.restoreComponentState (editController)) 
			return false;

		// restore controller-only state (if present)
		if (pf.contains (kControllerState) && !pf.restoreControllerState (editController))
			return false;
	}
	return true;
}

//------------------------------------------------------------------------
PresetFile::PresetFile (IBStream* stream)
: stream (stream)
, entryCount (0)
{
	memset (entries, 0, sizeof (entries));

	if (stream)
		stream->addRef ();
}

//------------------------------------------------------------------------
PresetFile::~PresetFile ()
{
	if (stream)
		stream->release ();
}

//------------------------------------------------------------------------
const PresetFile::Entry* PresetFile::getEntry (ChunkType which) const
{
	const ChunkID& id = getChunkID (which);
	for (int32 i = 0; i < entryCount; i++)
		if (isEqualID (entries[i].id, id))
			return &entries[i];
	return 0;
}

//------------------------------------------------------------------------
const PresetFile::Entry* PresetFile::getLastEntry () const 
{ 
	return entryCount > 0 ? &entries[entryCount-1] : 0; 
}

//------------------------------------------------------------------------
bool PresetFile::readID (ChunkID id)
{
	int32 numBytesRead = 0;
	stream->read (id, sizeof (ChunkID), &numBytesRead);
	return numBytesRead == sizeof (ChunkID);
}

//------------------------------------------------------------------------
bool PresetFile::writeID (const ChunkID id)
{
	int32 numBytesWritten = 0;
	stream->write ((void*)id, sizeof (ChunkID), &numBytesWritten);
	return numBytesWritten == sizeof (ChunkID);
}

//------------------------------------------------------------------------
bool PresetFile::readEqualID (const ChunkID id)
{
	ChunkID temp = {0};
	return readID (temp) && isEqualID (temp, id);
}

//------------------------------------------------------------------------
bool PresetFile::readSize (TSize& size)
{
	int32 numBytesRead = 0;
	stream->read (&size, sizeof (TSize), &numBytesRead);
#if BYTEORDER == kBigEndian
	SWAP_64 (size)
#endif	
	return numBytesRead == sizeof (TSize);
}

//------------------------------------------------------------------------
bool PresetFile::writeSize (TSize size)
{
#if BYTEORDER == kBigEndian
	SWAP_64 (size)
#endif	
	int32 numBytesWritten = 0;
	stream->write (&size, sizeof (TSize), &numBytesWritten);
	return numBytesWritten == sizeof (TSize);
}

//------------------------------------------------------------------------
bool PresetFile::readInt32 (int32& value)
{
	int32 numBytesRead = 0;
	stream->read (&value, sizeof (int32), &numBytesRead);
#if BYTEORDER == kBigEndian
	SWAP_32 (value)
#endif
	return numBytesRead == sizeof (int32);
}

//------------------------------------------------------------------------
bool PresetFile::writeInt32 (int32 value)
{
#if BYTEORDER == kBigEndian
	SWAP_32 (value)
#endif	
	int32 numBytesWritten = 0;
	stream->write (&value, sizeof (int32), &numBytesWritten);
	return numBytesWritten == sizeof (int32);
}

//------------------------------------------------------------------------
bool PresetFile::seekTo (TSize offset)
{
	int64 result = -1;
	stream->seek (offset, IBStream::kIBSeekSet, &result);
	return result == offset;
}

//------------------------------------------------------------------------
bool PresetFile::readChunkList ()
{
	seekTo (0);
	entryCount = 0;

	char classString[kClassIDSize + 1] = {0};

	// Read header
	int32 version = 0;
	TSize listOffset = 0;
	if (!(readEqualID (getChunkID (kHeader)) &&
		 readInt32 (version) &&
		 verify (stream->read (classString, kClassIDSize)) &&
		 readSize (listOffset) &&
		 listOffset > 0 &&
		 seekTo (listOffset)))
		return false;

	classID.fromString (classString);

	// Read list
	int32 count = 0;
	if (!readEqualID (getChunkID (kChunkList)))
		return false;
	if (!readInt32 (count))
		return false;

	if (count > kMaxEntries)
		count = kMaxEntries;

	for (int32 i = 0; i < count; i++)
	{
		Entry& e = entries[i];
		if (!(readID (e.id) &&
			 readSize (e.offset) &&
			 readSize (e.size)))
			break;

		entryCount++;
	}

	return entryCount > 0;
}

//------------------------------------------------------------------------
bool PresetFile::writeHeader ()
{
	// header id + version + class id + list offset (unknown yet)

	char classString[kClassIDSize + 1] = {0};
	classID.toString (classString);

	return	seekTo (0) &&
			writeID (getChunkID (kHeader)) && 
			writeInt32 (kFormatVersion) && 
			verify (stream->write (classString, kClassIDSize)) &&
			writeSize (0);
}

//------------------------------------------------------------------------
bool PresetFile::writeChunkList ()
{
	// Update list offset
	TSize pos = 0;
	stream->tell (&pos);
	if (!(seekTo (kListOffsetPos) &&
		 writeSize (pos) &&
		 seekTo (pos)))
		return false;

	// Write list
	if (!writeID (getChunkID (kChunkList)))
		return false;
	if (!writeInt32 (entryCount))
		return false;

	for (int32 i = 0; i < entryCount; i++)
	{
		Entry& e = entries[i];
		if (!(writeID (e.id) &&
			 writeSize (e.offset) &&
			 writeSize (e.size)))
			 return false;
	}
	return true;
}

//------------------------------------------------------------------------
bool PresetFile::beginChunk (Entry& e, ChunkType which)
{
	if (entryCount >= kMaxEntries)
		return false;

	const ChunkID& id = getChunkID (which);
	memcpy (e.id, &id, sizeof (ChunkID));
	stream->tell (&e.offset);
	e.size = 0;
	return true;
}

//------------------------------------------------------------------------
bool PresetFile::endChunk (Entry& e)
{
	if (entryCount >= kMaxEntries)
		return false;

	TSize pos = 0;
	stream->tell (&pos);
	e.size = pos - e.offset;
	entries[entryCount++] = e;
	return true;
}

//------------------------------------------------------------------------
bool PresetFile::readMetaInfo (char* xmlBuffer, int32& size)
{
	bool result = false;
	const Entry* e = getEntry (kMetaInfo);
	if (e)
	{
		if (xmlBuffer)
		{
			result = seekTo (e->offset) && 
					 verify (stream->read (xmlBuffer, size, &size));
		}
		else
		{
			size = (int32)e->size;
			result = size > 0;
		}
	}
	return result;
}

//------------------------------------------------------------------------
bool PresetFile::writeMetaInfo (const char* xmlBuffer, int32 size)
{
	if (contains (kMetaInfo)) // already exists!
		return false;

	if (size == -1)
		size = (int32)strlen (xmlBuffer);

	Entry e = {0};
	return	beginChunk (e, kMetaInfo) && 
			verify (stream->write ((void*)xmlBuffer, size)) && 
			endChunk (e);
}

//------------------------------------------------------------------------
bool PresetFile::prepareMetaInfoUpdate ()
{
	TSize writePos = 0;
	const Entry* e = getEntry (kMetaInfo);
	if (e)
	{
		// meta info must be the last entry!	
		if (e != getLastEntry ())
			return false;

		writePos = e->offset;
		entryCount--;
	}
	else
	{
		// entries must be sorted ascending by offset!
		e = getLastEntry ();
		writePos = e ? e->offset + e->size : kHeaderSize;
	}

	return seekTo (writePos);
}

//------------------------------------------------------------------------
bool PresetFile::writeChunk (const void* data, int32 size, ChunkType which)
{
	if (contains (which)) // already exists!
		return false;

	Entry e = {0};
	return	beginChunk (e, which) && 
			verify (stream->write ((void*)data, size)) && 
			endChunk (e);
}

//------------------------------------------------------------------------
bool PresetFile::storeComponentState (IComponent* component)
{
	if (contains (kComponentState)) // already exists!
		return false;

	Entry e = {0};
	return	beginChunk (e, kComponentState) && 
			verify (component->getState (stream)) && 
			endChunk (e);
}

//------------------------------------------------------------------------
bool PresetFile::restoreComponentState (IComponent* component)
{
	const Entry* e = getEntry (kComponentState);
	return	e && seekTo (e->offset) && 
			verify (component->setState (stream));
}

//------------------------------------------------------------------------
bool PresetFile::restoreComponentState (IEditController* editController)
{
	const Entry* e = getEntry (kComponentState);
	return	e && seekTo (e->offset) && 
			verify (editController->setComponentState (stream));
}

//------------------------------------------------------------------------
bool PresetFile::storeControllerState (IEditController* editController)
{
	if (contains (kControllerState)) // already exists!
		return false;

	Entry e = {0};
	return	beginChunk (e, kControllerState) && 
			verify (editController->getState (stream)) && 
			endChunk (e);
}

//------------------------------------------------------------------------
bool PresetFile::restoreControllerState (IEditController* editController)
{
	const Entry* e = getEntry (kControllerState);
	return	e && seekTo (e->offset) &&
			verify (editController->setState (stream));
}

//------------------------------------------------------------------------
bool PresetFile::storeProgramData (IBStream* inStream, ProgramListID listID, int32 programIndex)
{
	if (contains (kProgramData)) // already exists!
		return false;

	Entry e = {0};
	if (beginChunk (e, kProgramData))
	{
		if (writeInt32 (listID))
		{
			int8 buffer[128];
			int32 read = 0;
			int32 written = 0;
			while (inStream->read (buffer, 128, &read) == kResultTrue && read > 0)
			{
				if (stream->write (buffer, read, &written) != kResultTrue)
				{
					return false;
				}
			}
			return endChunk (e);
		}
	}
	return false;
}

//------------------------------------------------------------------------
bool PresetFile::storeProgramData (IProgramListData* unitData, ProgramListID listID, int32 programIndex)
{
	if (contains (kProgramData)) // already exists!
		return false;

	Entry e = {0};
	return	beginChunk (e, kProgramData) && 
			writeInt32 (listID) &&
			verify (unitData->getProgramData (listID, programIndex, stream)) && 
			endChunk (e);
}

//------------------------------------------------------------------------
bool PresetFile::restoreProgramData (IProgramListData* unitData, ProgramListID* listID, int32 programIndex)
{
	const Entry* e = getEntry (kProgramData);
	ProgramListID savedListID = -1;
	if (e && seekTo (e->offset))
	{
		if (readInt32 (savedListID))
		{
			if (listID && *listID != savedListID)
				return false;
			return verify (unitData->setProgramData (savedListID, programIndex, stream));
		}
	}
	return false;
}

//------------------------------------------------------------------------
bool PresetFile::restoreProgramData (IUnitInfo* unitInfo, ProgramListID listID, int32 programIndex)
{
	const Entry* e = getEntry (kProgramData);
	ProgramListID savedListID = -1;
	return	e && seekTo (e->offset) && 
			readInt32 (savedListID) &&
			listID == savedListID &&
			verify (unitInfo->setUnitProgramData (listID, programIndex, stream));
}

//------------------------------------------------------------------------
// FileStream
//------------------------------------------------------------------------
IBStream* FileStream::open (const char* filename, const char* mode)
{
	FILE* file = fopen (filename, mode);
	return file ? new FileStream (file) : 0;
}

//------------------------------------------------------------------------
FileStream::FileStream (FILE* file)
: file (file)
{
	FUNKNOWN_CTOR
}

//------------------------------------------------------------------------
FileStream::~FileStream ()
{
	fclose (file);
	FUNKNOWN_DTOR
}

//------------------------------------------------------------------------
IMPLEMENT_FUNKNOWN_METHODS (FileStream, IBStream, IBStream::iid)

//------------------------------------------------------------------------
tresult PLUGIN_API FileStream::read (void* buffer, int32 numBytes, int32* numBytesRead)
{
	size_t result = fread (buffer, 1, numBytes, file);
	if (numBytesRead)
		*numBytesRead = (int32)result;
	return result == numBytes ? kResultOk : kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FileStream::write (void* buffer, int32 numBytes, int32* numBytesWritten)
{
	size_t result = fwrite (buffer, 1, numBytes, file);
	if (numBytesWritten)
		*numBytesWritten = (int32)result;
	return result == numBytes ? kResultOk : kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FileStream::seek (int64 pos, int32 mode, int64* result)
{
	if (fseek (file, (int32)pos, mode) == 0)
	{
		if (result)
			*result = ftell (file);
		return kResultOk;
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API FileStream::tell (int64* pos)
{
	if (pos)
		*pos = ftell (file);
	return kResultOk;
}

//------------------------------------------------------------------------
}} // namespace Vst
