//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Helpers
// Filename    : vsthostutils.h
// Created by  : Steinberg, 10/2005
// Description : VST Hosting Utilities
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

#ifndef __vsthostutils__
#define __vsthostutils__

#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
// HostProcessData
//------------------------------------------------------------------------
class HostProcessData: public ProcessData
{
public:
//------------------------------------------------------------------------
	HostProcessData ();
	~HostProcessData ();

	/** Prepare buffer containers for all busses. */
	bool prepare (IComponent& component);

	/** Remove bus buffers. */
	void unprepare ();

	/** Sets one sample buffer for all channels. */
	void setChannelBuffers (BusDirection dir, int32 busIndex, Sample32* sampleBuffer);

	/** Sets individual sample buffers per channel. */
	void setChannelBuffers (BusDirection dir, int32 busIndex, Sample32* sampleBuffers[], int32 bufferCount);
//------------------------------------------------------------------------
protected:
	AudioBusBuffers* createBusBuffers (IComponent& component, BusDirection dir, int32 busCount);
	void destroyBusBuffers (AudioBusBuffers* buffers, int32 busCount);
};

//------------------------------------------------------------------------
// inline
//------------------------------------------------------------------------
inline void HostProcessData::setChannelBuffers (BusDirection dir, int32 busIndex, Sample32* sampleBuffer)
{
	if (dir == kInput && (!inputs || busIndex >= numInputs))
		return;
	if (dir == kOutput && (!outputs || busIndex >= numOutputs))
		return;

	AudioBusBuffers& busBuffers = dir == kInput ? inputs[busIndex] : outputs[busIndex];
	for (int32 i = 0; i < busBuffers.numChannels; i++)
		busBuffers.channelBuffers32[i] = sampleBuffer;
}

//------------------------------------------------------------------------
inline void HostProcessData::setChannelBuffers (BusDirection dir, int32 busIndex, Sample32* sampleBuffers[], int32 bufferCount)
{
	if (dir == kInput && (!inputs || busIndex >= numInputs))
		return;
	if (dir == kOutput && (!outputs || busIndex >= numOutputs))
		return;

	AudioBusBuffers& busBuffers = dir == kInput ? inputs[busIndex] : outputs[busIndex];
	int32 count = bufferCount < busBuffers.numChannels ? bufferCount : busBuffers.numChannels;
	for (int32 i = 0; i < count; i++)
		busBuffers.channelBuffers32[i] = sampleBuffers ? sampleBuffers[i] : 0;
}
//------------------------------------------------------------------------

}} // namespace Vst

#endif	// __vsthostutils__
