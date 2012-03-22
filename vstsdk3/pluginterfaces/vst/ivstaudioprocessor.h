//------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Interfaces
// Filename    : ivstaudioprocessor.h
// Created by  : Steinberg, 10/2005
// Description : VST Audio Processing Interfaces
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

#ifndef __ivstaudioprocessor__
#define __ivstaudioprocessor__

#include "ivstcomponent.h"

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpush.h"
//------------------------------------------------------------------------

//------------------------------------------------------------------------
/** Class Category Name for Audio Processor Component */
//------------------------------------------------------------------------
#ifndef kVstAudioEffectClass
#define kVstAudioEffectClass "Audio Module Class"
#endif


//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

class IEventList;
class IParameterChanges;
struct ProcessContext;

//------------------------------------------------------------------------
/** Component Types used as subCategories in PClassInfo2 */ 
//------------------------------------------------------------------------
namespace PlugType
{
/**
\defgroup plugType Plug-in Type used for subCategories */
/*@{*/
//------------------------------------------------------------------------
const CString kAnalyzer				= "Fx|Analyzer";	///< Scope, FFT-Display,...
const CString kDelay				= "Fx|Delay";		///< Delay, Multi-tap Delay,...
const CString kDistortion			= "Fx|Distortion";	///< Amp Simulator, Sub-Harmonic,...
const CString kDynamics				= "Fx|Dynamics";	///< Compressor, Expander, Limiter, Tape Simulator,...
const CString kEQ					= "Fx|EQ";			///< Equalization,...
const CString kFilter				= "Fx|Filter";
const CString kGenerator			= "Fx|Generator";
const CString kMastering			= "Fx|Mastering";	///< Dither, Noise Shaping,...
const CString kModulation			= "Fx|Modulation";	///< Phaser, Flanger, Chorus,...
const CString kReverb				= "Fx|Reverb";		///< Reverberation, Room Simulation,...
const CString kRestoration			= "Fx|Restoration";
const CString kPitchShift			= "Fx|PitchShift";	///< Pitch Processing
const CString kSurround				= "Fx|Surround";	///< dedicated to surround processing: LFE Splitter,...
const CString kFxInstrument			= "Fx|Instrument";	///< Fx which could be loaded as instrument too

const CString kInstrumentDrum		= "Instrument|Drum";
const CString kInstrumentSampler	= "Instrument|Sampler";	///< Instrument based on Samples
const CString kInstrumentSynth		= "Instrument|Synth";

const CString kSpatial				= "Spatial";		///< used for SurroundPanner
const CString kOnlyRealTime			= "OnlyRT";			///< indicates that it supports only realtime process call, no offline
//------------------------------------------------------------------------
/*@}*/
}

//------------------------------------------------------------------------
/** Component Flags used as classFlags in PClassInfo2 */
//------------------------------------------------------------------------
enum ComponentFlags
{
//------------------------------------------------------------------------
	kDistributable			= 1 << 0,	///< Component can be run on remote computer
	kSimpleModeSupported	= 1 << 1	///< Component supports simple IO mode (or works in simple mode anyway)
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
/** Symbolic sample size.
\see ProcessSetup, ProcessData */
//------------------------------------------------------------------------
enum SymbolicSampleSizes
{
	kSample32,		///< 32-bit precision
	kSample64		///< 64-bit precision
};

//------------------------------------------------------------------------
/** Processing mode.
\see ProcessSetup, ProcessData */
//------------------------------------------------------------------------
enum ProcessModes
{
	kRealtime,		///< realtime
	kPrefetch,		///< prefetch (could be called faster than realtime, same quality level than realtime)
	kOffline		///< offline (higher quality than realtime could be used)
};

//------------------------------------------------------------------------
/** Audio processing setup. 
\see IAudioProcessor::setupProcessing*/
//------------------------------------------------------------------------
struct ProcessSetup
{
//------------------------------------------------------------------------
	int32 processMode;			///< \ref ProcessModes
	int32 symbolicSampleSize;	///< \ref SymbolicSampleSizes
	int32 maxSamplesPerBlock;	///< maximum number of samples per audio block
	SampleRate sampleRate;		///< sample rate
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
/** Processing buffers of an audio bus.
This structure contains the processing buffer for each channel of an audio bus.
- The number of channels (numChannels) must always match the current bus arrangement.
- The size of the channel buffer array must always match the number of channels. So the host 
  must always supply an array for the channel buffers, regardless if the
  bus is active or not. However, if an audio bus is currently inactive, the actual sample 
  buffer addresses are safe to be null.
- The silent flag is set when every sample of the according buffer has the value '0'. It is
  intended to be used as help for optimizations allowing a plug-in to reduce processing activities.
  But even if this flag is set for a channel, the channel buffers must still point to valid memory!
  This flag is optional. A host is free to support it or not. 
.
\see ProcessData*/
//------------------------------------------------------------------------
struct AudioBusBuffers
{
//------------------------------------------------------------------------
	int32 numChannels;		///< number of audio channels in bus
	uint64 silenceFlags;	///< Bitset of silence state per channel
	union
	{
		Sample32** channelBuffers32;	///< sample buffers to process with 32-bit precision
		Sample64** channelBuffers64;	///< sample buffers to process with 64-bit precision
	};
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
/** Any data needed in audio processing. 
	The host prepares AudioBusBuffers for each input/output bus,
	regardless of the bus activation state. Bus buffer indices always match 
	with bus indices used in IComponent::getBusInfo of media type kAudio. 
    \see AudioBusBuffers, IParameterChanges, IEventList, ProcessContext */
//------------------------------------------------------------------------
struct ProcessData
{
//------------------------------------------------------------------------
	int32 processMode;			///< processing mode - value of \ref ProcessModes
	int32 symbolicSampleSize;   ///< sample size - value of \ref SymbolicSampleSizes
	int32 numSamples;			///< number of samples to process
	int32 numInputs;			///< number of audio input busses
	int32 numOutputs;			///< number of audio output busses
	AudioBusBuffers* inputs;	///< buffers of input busses
	AudioBusBuffers* outputs;	///< buffers of output busses

	IParameterChanges* inputParameterChanges;	///< incoming parameter changes for this block 
	IParameterChanges* outputParameterChanges;	///< outgoing parameter changes for this block (optional)
	IEventList* inputEvents;				///< incoming events for this block (optional)
	IEventList* outputEvents;				///< outgoing events for this block (optional)
	ProcessContext* processContext;			///< processing context (optional, but most welcome)
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
/** Audio Processing Interface.
\ingroup vstIPlug
- [plug imp]
- [extends IComponent]
- [mandatory]

This interface must always be supported by audio processing plug-ins. */
//------------------------------------------------------------------------
class IAudioProcessor: public FUnknown
{
public:
//------------------------------------------------------------------------
	//** Try to set (from host) a predefined arrangement for inputs and outputs. */
	virtual tresult PLUGIN_API setBusArrangements (SpeakerArrangement* inputs, int32 numIns, 
												   SpeakerArrangement* outputs, int32 numOuts) = 0;
	
	//** Gets the bus arrangement for a given direction (input/output) and index. */
	virtual tresult PLUGIN_API getBusArrangement (BusDirection dir, int32 index, SpeakerArrangement& arr) = 0;
		
	//** Ask if a given sample size is supported (@see ProcessSampleSizes). */
	virtual tresult PLUGIN_API canProcessSampleSize (int32 symbolicSampleSize) = 0;
	
	//** Gets the current Latency in samples. */
	virtual uint32  PLUGIN_API getLatencySamples () = 0; 

	//** Called in disable state (not active) before processing will begin. */
	virtual tresult PLUGIN_API setupProcessing (ProcessSetup& setup) = 0;
	
	/** Inform the plug-in about the processing state. This will be called before process calls (one or more) start with true and after with false. */
	virtual tresult PLUGIN_API setProcessing (TBool state) = 0;

	//** The Process call, where all information (parameter changes, event, audio buffer) are passed. */
	virtual tresult PLUGIN_API process (ProcessData& data) = 0;

	//** Gets tail size in samples (for example the reverb time of a reverb plug-in). */
	virtual uint32 PLUGIN_API getTailSamples () = 0;

//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IAudioProcessor, 0x42043F99, 0xB7DA453C, 0xA569E79D, 0x9AAEC33D)

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

//------------------------------------------------------------------------
#include "pluginterfaces/base/falignpop.h"
//------------------------------------------------------------------------

#endif // __ivstaudioprocessor__
