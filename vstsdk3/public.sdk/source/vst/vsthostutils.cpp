//------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Helpers
// Filename    : vsthostutils.cpp
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
//-----------------------------------------------------------------------------

#include "vsthostutils.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
// HostProcessData
//------------------------------------------------------------------------
HostProcessData::HostProcessData ()
{
	memset (this, 0, sizeof (ProcessData));
	symbolicSampleSize = kSample32;
}

//------------------------------------------------------------------------
HostProcessData::~HostProcessData ()
{
	unprepare ();
}

//------------------------------------------------------------------------
bool HostProcessData::prepare (IComponent& component)
{
	int32 newNumInputs  = component.getBusCount (kAudio, kInput);
	int32 newNumOutputs = component.getBusCount (kAudio, kOutput);

	if (inputs || outputs)
		unprepare ();

	numInputs = newNumInputs;
	if (numInputs > 0)
	{
		inputs = createBusBuffers (component, kInput, numInputs);
		if (!inputs)
			return false;
	}

	numOutputs = newNumOutputs;
	if (numOutputs > 0)
	{
		outputs = createBusBuffers (component, kOutput, numOutputs);
		if (!outputs)
			return false;
	}

	return true;
}

//------------------------------------------------------------------------
void HostProcessData::unprepare ()
{
	if (inputs)
	{
		destroyBusBuffers (inputs, numInputs);
		inputs = 0;
		numInputs = 0;
	}

	if (outputs)
	{
		destroyBusBuffers (outputs, numOutputs);
		outputs = 0;
		numOutputs = 0;
	}
}

//------------------------------------------------------------------------
AudioBusBuffers* HostProcessData::createBusBuffers (IComponent& component, BusDirection dir, int32 busCount)
{
	AudioBusBuffers* buffers = new AudioBusBuffers[busCount];
	if (!buffers)
		return 0;

	memset (buffers, 0, busCount * sizeof (AudioBusBuffers));

	for (int32 busIndex = 0; busIndex < busCount; busIndex++)
	{
		BusInfo busInfo = {0};
		tresult result = component.getBusInfo (kAudio, dir, busIndex, busInfo);
		if (result != kResultOk)
			continue;

		int32 channelCount = busInfo.channelCount;

		AudioBusBuffers& buffer = buffers[busIndex];
		buffer.numChannels = channelCount;
		buffer.silenceFlags = 0;
		if (channelCount > 0)
		{
			buffer.channelBuffers32 = new Sample32*[channelCount];
			if (buffer.channelBuffers32)
				memset (buffer.channelBuffers32, 0, channelCount * sizeof (Sample32*));
		}
	}

	return buffers;
}

//------------------------------------------------------------------------
void HostProcessData::destroyBusBuffers (AudioBusBuffers* buffers, int32 busCount)
{
	for (int32 busIndex = 0; busIndex < busCount; busIndex++)
	{
		Sample32** channelBuffers = buffers[busIndex].channelBuffers32;
		if (channelBuffers)
			delete [] channelBuffers;
	}

	delete [] buffers;
}

//------------------------------------------------------------------------
}} // namespace Vst
