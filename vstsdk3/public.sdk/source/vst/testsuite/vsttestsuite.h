//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Validator
// Filename    : vsttestsuite.h
// Created by  : Steinberg, 04/2005
// Description : VST Test Suite
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

#ifndef __vsttestsuite__
#define __vsttestsuite__

#include "pluginterfaces/test/itest.h"
#include "public.sdk/source/vst/vsthostutils.h"

// VST3 interfaces
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

typedef int32 ProcessSampleSize;

//------------------------------------------------------------------------
#define DECLARE_VSTTEST(name) \
virtual const char* getName () const { return name; }

//------------------------------------------------------------------------
// IPlugProvider
//------------------------------------------------------------------------
class IPlugProvider : public FUnknown
{
public:
//------------------------------------------------------------------------
	virtual Vst::IComponent* getComponent () = 0;
	virtual Vst::IEditController* getController () = 0;
	virtual tresult releasePlugIn (Vst::IComponent* component, Vst::IEditController* controller) = 0;
//------------------------------------------------------------------------
	static const FUID iid;
};

DECLARE_CLASS_IID (IPlugProvider, 0xDB014121, 0x09144BAA, 0x87627896, 0xBE41AF5D)

//------------------------------------------------------------------------
// VstTestBase
//------------------------------------------------------------------------
class VstTestBase: public ITest
{
public:
//------------------------------------------------------------------------
	VstTestBase (IPlugProvider* plugProvider);
	virtual ~VstTestBase ();
	
	DECLARE_VSTTEST ("VST Test Base")
	DECLARE_FUNKNOWN_METHODS

	bool PLUGIN_API setup ();
	bool PLUGIN_API run (ITestResult* testResult) {return false;}	// implement me
	bool PLUGIN_API teardown ();
//------------------------------------------------------------------------
protected:
	IPlugProvider* plugProvider;
	Vst::IComponent* vstPlug;
	Vst::IEditController* controller;

private:
	VstTestBase ();
};

//------------------------------------------------------------------------
// VstTestEnh
//------------------------------------------------------------------------
class VstTestEnh : public VstTestBase
{
public:
//------------------------------------------------------------------------
	VstTestEnh (IPlugProvider* plugProvider, ProcessSampleSize sampl);
	~VstTestEnh ();

	enum AudioDefaults
	{
		kBlockSize = 64,
		kMaxSamplesPerBlock = 8192,
		kSampleRate = 44100,
	};

	bool PLUGIN_API setup();
	bool PLUGIN_API teardown ();
//------------------------------------------------------------------------
protected:
	// interfaces
	Vst::IAudioProcessor* audioEffect;

	Vst::ProcessSetup processSetup;
};

//------------------------------------------------------------------------
// VstSuspendResumeTest
//------------------------------------------------------------------------
class VstSuspendResumeTest: public VstTestEnh
{
public:
//------------------------------------------------------------------------
	VstSuspendResumeTest (IPlugProvider* plugProvider, ProcessSampleSize sampl);

	DECLARE_VSTTEST ("Suspend/Resume")
	
	bool PLUGIN_API run (ITestResult* testResult);
//------------------------------------------------------------------------
};


//------------------------------------------------------------------------
// VstStateTransitionTest
//------------------------------------------------------------------------
class VstTerminateInitializeTest: public VstTestBase
{
public:
//------------------------------------------------------------------------
	VstTerminateInitializeTest (IPlugProvider* plugProvider);

	DECLARE_VSTTEST ("Terminate/Initialize")

	bool PLUGIN_API run (ITestResult* testResult);
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
// VstScanBussesTest
//------------------------------------------------------------------------
class VstScanBussesTest: public VstTestBase
{
public:
//------------------------------------------------------------------------
	VstScanBussesTest (IPlugProvider* plugProvider);

	DECLARE_VSTTEST ("Scan Busses")
	
	bool PLUGIN_API run (ITestResult* testResult);
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
// VstScanParametersTest
//------------------------------------------------------------------------
class VstScanParametersTest: public VstTestBase
{
public:
//------------------------------------------------------------------------
	VstScanParametersTest (IPlugProvider* plugProvider);

	DECLARE_VSTTEST ("Scan Parameters")
	
	bool PLUGIN_API run (ITestResult* testResult);
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
// VstEditorClassesTest
//------------------------------------------------------------------------
class VstEditorClassesTest: public VstTestBase
{
public:
//------------------------------------------------------------------------
	VstEditorClassesTest (IPlugProvider* plugProvider);

	DECLARE_VSTTEST ("Scan Editor Classes")
	
	bool PLUGIN_API run (ITestResult* testResult);
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
// VstUnitInfoTest
//------------------------------------------------------------------------
class VstUnitInfoTest: public VstTestBase
{
public:
//------------------------------------------------------------------------
	VstUnitInfoTest (IPlugProvider* plugProvider);

	DECLARE_VSTTEST ("Scan Units")
	
	bool PLUGIN_API run (ITestResult* testResult);
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
// VstProgramInfoTest
//------------------------------------------------------------------------
class VstProgramInfoTest: public VstTestBase
{
public:
//------------------------------------------------------------------------
	VstProgramInfoTest (IPlugProvider* plugProvider);

	DECLARE_VSTTEST ("Scan Programs")

	bool PLUGIN_API run (ITestResult* testResult);
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
// VstUnitParentTest
//------------------------------------------------------------------------
class VstUnitStructureTest: public VstTestBase
{
public:
//------------------------------------------------------------------------
	VstUnitStructureTest (IPlugProvider* plugProvider);

	DECLARE_VSTTEST ("Check Unit Structure")

	bool PLUGIN_API run (ITestResult* testResult);
//------------------------------------------------------------------------
};

//------------------------------------------------------------------------
// VstProcessTest
//------------------------------------------------------------------------
class VstProcessTest : public VstTestEnh
{
public:
//------------------------------------------------------------------------
	VstProcessTest (IPlugProvider* plugProvider, ProcessSampleSize sampl);

	DECLARE_VSTTEST ("Process Test")
	
	// ITest
	bool PLUGIN_API setup ();
	bool PLUGIN_API run (ITestResult* testResult);
	bool PLUGIN_API teardown ();

//------------------------------------------------------------------------
protected:
	virtual bool prepareProcessing ();						///< setup ProcessData and allocate buffers
	virtual bool unprepareProcessing ();					///< free dynamic memory of ProcessData
	virtual bool preProcess (ITestResult* testResult);		///< is called just before the process call
	virtual bool postProcess (ITestResult* testResult);		///< is called right after the process call

	bool setupBuffers (int32 numBusses, AudioBusBuffers* audioBuffers, BusDirection dir);
	bool setupBuffers (AudioBusBuffers& audioBuffers);
	bool freeBuffers (int32 numBuses, AudioBusBuffers* buses);
	bool canProcessSampleSize (ITestResult* testResult);	///< audioEffect has to be available

	HostProcessData processData;
};

//------------------------------------------------------------------------
// VstSpeakerArrangementTest
//------------------------------------------------------------------------
class VstSpeakerArrangementTest : public VstProcessTest
{
public:
//------------------------------------------------------------------------
	VstSpeakerArrangementTest (IPlugProvider* plugProvider, ProcessSampleSize sampl, SpeakerArrangement inSpArr, SpeakerArrangement outSpArr);

	const char* getName () const;
	static const char* getSpeakerArrangementName (SpeakerArrangement spArr);
	
	// ITest
	bool PLUGIN_API run (ITestResult* testResult);

//------------------------------------------------------------------------
protected:
	bool prepareProcessing ();
	bool verifySA (int32 numBusses, AudioBusBuffers* busses, SpeakerArrangement spArr, ITestResult* testResult);
private:
	SpeakerArrangement inSpArr;
	SpeakerArrangement outSpArr;
};

//------------------------------------------------------------------------
// VstAutomationTest
//------------------------------------------------------------------------
class ParamChanges;
class VstAutomationTest : public VstProcessTest, public IParameterChanges
{
public:
//------------------------------------------------------------------------
	VstAutomationTest (IPlugProvider* plugProvider, ProcessSampleSize sampl, int32 everyNSamples, int32 numParams, bool sampleAccuracy);
	~VstAutomationTest ();

	DECLARE_FUNKNOWN_METHODS
	const char* getName () const;
	// ITest
	bool PLUGIN_API setup ();
	bool PLUGIN_API run (ITestResult* testResult);
	bool PLUGIN_API teardown ();

	// IParameterChanges
	int32 PLUGIN_API getParameterCount ();
	IParamValueQueue* PLUGIN_API getParameterData (int32 index);
	IParamValueQueue* PLUGIN_API addParameterData (const Vst::ParamID& id, int32& index);

//------------------------------------------------------------------------
protected:
	bool preProcess (ITestResult* testResult);
	bool postProcess (ITestResult* testResult);
private:
	ParamChanges* paramChanges;
	int32 countParamChanges;
	int32 everyNSamples;
	int32 numParams;
	bool sampleAccuracy;
	bool onceExecuted;
};

//------------------------------------------------------------------------
// VstValidStateTransitionTest
//------------------------------------------------------------------------
class VstValidStateTransitionTest : public VstTestBase
{
public:
	VstValidStateTransitionTest (IPlugProvider* plugProvider);
	bool PLUGIN_API run (ITestResult* testResult);

	DECLARE_VSTTEST ("Valid State Transition")
};

//------------------------------------------------------------------------
// VstInvalidStateTransitionTest
//------------------------------------------------------------------------
class VstInvalidStateTransitionTest : public VstTestBase
{
public:
	VstInvalidStateTransitionTest (IPlugProvider* plugProvider);
	bool PLUGIN_API run (ITestResult* testResult);

	DECLARE_VSTTEST ("Invalid State Transition")
};

//------------------------------------------------------------------------
// VstRepeatIdenticalStateTransitionTest
//------------------------------------------------------------------------
class VstRepeatIdenticalStateTransitionTest : public VstTestBase
{
public:
	VstRepeatIdenticalStateTransitionTest (IPlugProvider* plugProvider);
	bool PLUGIN_API run (ITestResult* testResult);

	DECLARE_VSTTEST ("Repeat Identical State Transition")
};

//------------------------------------------------------------------------
// VstBusConsistencyTest
//------------------------------------------------------------------------
class VstBusConsistencyTest : public VstTestBase
{
public:
	VstBusConsistencyTest (IPlugProvider* plugProvider);
	bool PLUGIN_API run (ITestResult* testResult);

	DECLARE_VSTTEST ("Bus Consistency")
};

//------------------------------------------------------------------------
// VstBusInvalidIndexTest
//------------------------------------------------------------------------
class VstBusInvalidIndexTest : public VstTestBase
{
public:
	VstBusInvalidIndexTest (IPlugProvider* plugProvider);
	bool PLUGIN_API run (ITestResult* testResult);

	DECLARE_VSTTEST ("Bus Invalid Index")
};

//------------------------------------------------------------------------
// VstSilenceFlagsTest
//------------------------------------------------------------------------
class VstSilenceFlagsTest : public VstProcessTest
{
public:
	VstSilenceFlagsTest (IPlugProvider* plugProvider, ProcessSampleSize sampl);
	bool PLUGIN_API run (ITestResult* testResult);

	DECLARE_VSTTEST ("Silence Flags")
};

//------------------------------------------------------------------------
// VstFlushParamTest
//------------------------------------------------------------------------
class VstFlushParamTest : public VstAutomationTest
{
public:
	VstFlushParamTest (IPlugProvider* plugProvider, ProcessSampleSize sampl);
	bool PLUGIN_API run (ITestResult* testResult);

	DECLARE_VSTTEST ("Parameters Flush (no Buffer)")
};

//------------------------------------------------------------------------
// VstBusActivationTest
//------------------------------------------------------------------------
class VstBusActivationTest : public VstTestBase
{
public:
	VstBusActivationTest (IPlugProvider* plugProvider);
	bool PLUGIN_API run (ITestResult* testResult);

	DECLARE_VSTTEST ("Bus Activation")
};

//------------------------------------------------------------------------
// VstVariableBlockSizeTest
//------------------------------------------------------------------------
class VstVariableBlockSizeTest : public VstProcessTest
{
public:
	VstVariableBlockSizeTest (IPlugProvider* plugProvider, ProcessSampleSize sampl);
	bool PLUGIN_API run (ITestResult* testResult);

	DECLARE_VSTTEST ("Variable Block Size")
};

//------------------------------------------------------------------------
// VstProcessFormatTest
//------------------------------------------------------------------------
class VstProcessFormatTest : public VstProcessTest
{
public:
	VstProcessFormatTest (IPlugProvider* plugProvider, ProcessSampleSize sampl);
	bool PLUGIN_API run (ITestResult* testResult);

	DECLARE_VSTTEST ("Process Format")
};

//------------------------------------------------------------------------
// VstSilenceFlagsTest
//------------------------------------------------------------------------
class VstCheckAudioBusArrangementTest : public VstTestBase
{
public:
	VstCheckAudioBusArrangementTest (IPlugProvider* plugProvider);
	bool PLUGIN_API run (ITestResult* testResult);

	DECLARE_VSTTEST ("Check Audio Bus Arrangement")
};

#endif // __vsttestsuite__
