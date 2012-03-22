//-----------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Helpers
// Filename    : vsttestsuite.cpp
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

#include "vsttestsuite.h"

//------------------------------------------------------------------------
#define INIT_CLASS_IID
// This macro definition modifies the behavior of DECLARE_CLASS_IID (funknown.h)
// and produces the actual symbols for all interface identifiers.
// It must be defined before including the interface headers and
// in only one source file!
//------------------------------------------------------------------------

#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"
#include "pluginterfaces/vst/ivstmessage.h"
#include "pluginterfaces/vst/ivstunits.h"
#include "pluginterfaces/vst/vstpresetkeys.h"

using namespace Vst;

extern FUnknown* gStandardPluginContext;

#include <cstdio>
#include <stdlib.h>

#define NUM_ITERATIONS 20
#define DEFAULT_SAMPLE_RATE 44100
#define DEFAULT_BLOCK_SIZE 64
#define MAX_BLOCK_SIZE 8192
#define BUFFERS_ARE_EQUAL 0
#define NUM_AUDIO_BLOCKS_TO_PROCESS 3
#define CHANNEL_IS_SILENT 1

#define TOUGHTESTS 0

//------------------------------------------------------------------------
template<class T> struct ArrayDeleter
{	
	ArrayDeleter (T* array): array (array) {}
	~ArrayDeleter () { if (array) delete[] array; }
	
	T* array;
};

//------------------------------------------------------------------------
// VstTestBase
//------------------------------------------------------------------------
VstTestBase::VstTestBase (IPlugProvider* plugProvider)
: plugProvider (plugProvider)
, controller (0)
, vstPlug (0)
{
	FUNKNOWN_CTOR 

	if (plugProvider)
		plugProvider->addRef ();
}

//------------------------------------------------------------------------
VstTestBase::VstTestBase ()
: plugProvider (0)
, controller (0)
, vstPlug (0)
{
	FUNKNOWN_CTOR 
}

//------------------------------------------------------------------------
VstTestBase::~VstTestBase ()
{
	FUNKNOWN_DTOR

	if (plugProvider)
		plugProvider->release ();
}

//------------------------------------------------------------------------
IMPLEMENT_FUNKNOWN_METHODS (VstTestBase, ITest, ITest::iid)

//------------------------------------------------------------------------
bool VstTestBase::setup ()
{
	if (plugProvider)
	{
		vstPlug = plugProvider->getComponent ();
		controller = plugProvider->getController ();
		if (vstPlug)
			return true;
	}
	return false;
}

//------------------------------------------------------------------------
bool VstTestBase::teardown ()
{
	if (vstPlug /*&& controller*/)
		plugProvider->releasePlugIn (vstPlug, controller);
	return true;
}

//------------------------------------------------------------------------
// Component Initialize / Terminate 
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// VstTestEnh
//------------------------------------------------------------------------
VstTestEnh::VstTestEnh (IPlugProvider* plugProvider, ProcessSampleSize sampl)
: VstTestBase (plugProvider)
, audioEffect (0)
{
	// process setup defaults
	memset (&processSetup, 0, sizeof (ProcessSetup));

	processSetup.processMode = kRealtime;
	processSetup.symbolicSampleSize = sampl;
	processSetup.maxSamplesPerBlock = kMaxSamplesPerBlock;
	processSetup.sampleRate = kSampleRate;
}

//------------------------------------------------------------------------
VstTestEnh::~VstTestEnh ()
{
}

//------------------------------------------------------------------------
bool VstTestEnh::setup ()
{
	bool res = VstTestBase::setup ();

	if (vstPlug)
	{
		tresult check = vstPlug->queryInterface (IAudioProcessor::iid, (void**)&audioEffect);
		if (check != kResultTrue)
			return false;
	}

	return (res && audioEffect);
}

//------------------------------------------------------------------------
bool VstTestEnh::teardown ()
{
	if (audioEffect)
		audioEffect->release ();
	
	bool res = VstTestBase::teardown ();
	
	return res && audioEffect;
}


//------------------------------------------------------------------------
// VstSuspendResumeTest
//------------------------------------------------------------------------
VstSuspendResumeTest::VstSuspendResumeTest (IPlugProvider* plugProvider, ProcessSampleSize sampl)
: VstTestEnh (plugProvider, sampl)
{}

//------------------------------------------------------------------------
bool PLUGIN_API VstSuspendResumeTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	for (int32 i = 0; i < 3; i++)
	{
		if (audioEffect)
		{
			if (audioEffect->canProcessSampleSize (kSample32) == kResultOk)
				processSetup.symbolicSampleSize = kSample32;
			else if (audioEffect->canProcessSampleSize (kSample64) == kResultOk)
				processSetup.symbolicSampleSize = kSample64;
			else
			{
				testResult->addErrorMessage ("No appropriate symbolic sample size supported!");
				return false;
			}
			
			if (audioEffect->setupProcessing (processSetup) != kResultOk)
			{
				testResult->addErrorMessage ("Process setup failed!");
				return false;
			}
			
		}
		tresult result = vstPlug->setActive (true);
		if (result != kResultOk)
			return false;

		result = vstPlug->setActive (false);
		if (result != kResultOk)
			return false;
	}
	return true;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
// VstTerminateInitializeTest
//------------------------------------------------------------------------
VstTerminateInitializeTest::VstTerminateInitializeTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{}

//------------------------------------------------------------------------
bool VstTerminateInitializeTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	bool result = true;
	if (vstPlug->terminate () != kResultTrue)
	{
		testResult->addErrorMessage ("IPluginBase::terminate () failed.");
		result = false;
	}
	if (vstPlug->initialize (gStandardPluginContext) != kResultTrue)
	{
		testResult->addErrorMessage ("IPluginBase::initialize (..) failed.");
		result = false;
	}
	return result;
}

//------------------------------------------------------------------------
// VstScanBussesTest
//------------------------------------------------------------------------
VstScanBussesTest::VstScanBussesTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{}

//------------------------------------------------------------------------
bool PLUGIN_API VstScanBussesTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	int32 numBusses = 0;

	for (MediaType mediaType = kAudio; mediaType < kNumMediaTypes; mediaType++)
	{
		int32 numInputs = vstPlug->getBusCount (mediaType, kInput);
		int32 numOutputs = vstPlug->getBusCount (mediaType, kOutput);
		
		numBusses += (numInputs + numOutputs);
		
		if ((mediaType == (kNumMediaTypes - 1)) && (numBusses == 0))
		{
			testResult->addErrorMessage ("This component exports no busses!!!");
			return false;
		}

		sprintf (text, "*** %s Busses: ***", mediaType == kAudio ? "Audio" : "Midi");
		testResult->addMessage (text);

		sprintf (text, " %d In / %d Out", numInputs, numOutputs);
		testResult->addMessage (text);

		for (int32 i = 0; i < numInputs + numOutputs; i++)
		{
			BusDirection busDirection = i < numInputs ? kInput : kOutput;
			int32 busIndex = busDirection == kInput ? i : i - numInputs;

			BusInfo busInfo = {0};
			if (vstPlug->getBusInfo (mediaType, busDirection, busIndex, busInfo) == kResultTrue)
			{
				char busName[256] = {0};
				UString (busInfo.name, USTRINGSIZE (busInfo.name)).toAscii (busName, 256);

				if (busName[0] == 0)
				{
					sprintf (text, "Bus %d has no name!!!", numInputs, numOutputs);
					testResult->addErrorMessage (text);
					return false;
				}

				sprintf (text, " %s %d: \"%s\" (%s-%s) ", 
					busDirection == kInput ? "IN " : "OUT",
					busIndex, 
					busName, 
					busInfo.busType == kMain ? "Main" : "Aux",
					busInfo.kDefaultActive ? "Default Active" : "Default Inactive");
	
				testResult->addMessage (text);
			}
			else
				return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------
// VstScanParametersTest
//------------------------------------------------------------------------
VstScanParametersTest::VstScanParametersTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{}

//------------------------------------------------------------------------
bool PLUGIN_API VstScanParametersTest::run (ITestResult* testResult)
{
	if (!testResult || !vstPlug)
		return false;
	
	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);
	
	if (!controller)
	{
		testResult->addMessage ("No Edit Controller supplied!");
		return true;
	}
		
	int32 numParameters = controller->getParameterCount ();
	if (numParameters <= 0)
	{
		testResult->addMessage ("This component exports no parameters!");
		return true;
	}

	sprintf (text, "This component exports %d parameter(s)", numParameters);
	testResult->addMessage (text);

	FUnknownPtr<IUnitInfo> iUnitInfo2 (controller);
	if (!iUnitInfo2 && numParameters > 20)
	{
		testResult->addMessage ("Note: it could be better to use UnitInfo in order to sort Parameters (>20).");
	}

	// used for ID check
	int32* paramIds = new int32[numParameters];

	bool foundBypass = false;
	for (int32 i = 0; i < numParameters; i++)
	{
		ParameterInfo paramInfo = {0};
	
		tresult result = controller->getParameterInfo (i, paramInfo);
		if (result != kResultOk)
		{
			sprintf (text, "Param %03d: is missing!!!", i);
			testResult->addErrorMessage (text);
			return false;
		}

		int32 paramId = paramInfo.id;
		paramIds[i] = paramId;
		if (paramId < 0)
		{
			sprintf (text, "Param %03d: Invalid Id!!!", i);
			testResult->addErrorMessage (text);
			return false;			
		}

		// check if ID is already used by another parameter
		for (int32 idIndex = 0; idIndex < i; idIndex++)
		{
			if (paramIds[idIndex] == paramIds[i])
			{
				sprintf (text, "Param %03d: ID already used (by %03d)!!!", i, idIndex);
				testResult->addErrorMessage (text);
				return false;
			}
		}

		const char* paramType = "";
		if (paramInfo.stepCount < 0)
		{
			sprintf (text, "Param %03d: invalid stepcount!!!", i);
			testResult->addErrorMessage (text);
			return false;
		}
		if (paramInfo.stepCount == 0)
			paramType = "Float";
		else if (paramInfo.stepCount == 1)
			paramType = "Toggle";
		else
			paramType = "Discrete";

		char paramTitle[256] = {0};
		char paramUnits[256] = {0};
		UString (paramInfo.title, USTRINGSIZE (paramInfo.title)).toAscii (paramTitle, 256);
		UString (paramInfo.units, USTRINGSIZE (paramInfo.units)).toAscii (paramUnits, 256);

		if (paramTitle[0] == 0)
		{
			sprintf (text, "Param %03d: has no title!!!", i);
			testResult->addErrorMessage (text);
			return false;
		}

		if (paramInfo.defaultNormalizedValue != -1.f && (paramInfo.defaultNormalizedValue < 0. || paramInfo.defaultNormalizedValue > 1.))
		{
			sprintf (text, "Param %03d: defaultValue is not normalized!!!", i);
			testResult->addErrorMessage (text);
			return false;
		}

		int32 unitId = paramInfo.unitId;
		if (unitId < -1)
		{
			sprintf (text, "Param %03d: No appropriate unit ID!!!", i);
			testResult->addErrorMessage (text);
			return false;			
		}
		if (unitId >= -1)
		{
			FUnknownPtr<IUnitInfo> iUnitInfo (controller);
			if (!iUnitInfo && unitId != 0)
			{
				sprintf (text, "IUnitInfo interface is missing, but ParameterInfo::unitID is not %03d (kRootUnitId).", kRootUnitId);
				testResult->addMessage (text);
				//return false;
			}
			else if (iUnitInfo)
			{
				bool found = false;
				int32 uc = iUnitInfo->getUnitCount ();
				for (int32 ui = 0; ui < uc; ui++)
				{
					UnitInfo uinfo = {0};
					if (iUnitInfo->getUnitInfo (ui, uinfo) != kResultTrue)
					{
						testResult->addErrorMessage ("IUnitInfo::getUnitInfo (..) failed.");
						return false;
					}
					if (uinfo.id == unitId)
						found = true;
				}
				if (!found && unitId != kRootUnitId)
				{
					testResult->addErrorMessage ("Parameter has a UnitID, which isn't defined in IUnitInfo.");
					return false;
				}
			}
		}
		if (((paramInfo.flags & ParameterInfo::kCanAutomate) != 0) && ((paramInfo.flags & ParameterInfo::kIsReadOnly) != 0))
		{
			testResult->addErrorMessage ("Parameter must not be kCanAutomate and kReadOnly at the same time.");
			return false;
		}

		if ((paramInfo.flags & ParameterInfo::kIsBypass) != 0)
		{
			if (!foundBypass)
				foundBypass = true;
			else
			{
				testResult->addErrorMessage ("There can only be one bypass (kIsBypass).");
				return false;
			}
		}

		sprintf (text, "Param %03d(ID = %d): \"%s\" [%s] (type = %s, default = %lf, unit = %d)", 
				 i, paramId, paramTitle, paramUnits, paramType, paramInfo.defaultNormalizedValue, unitId);

		testResult->addMessage (text);
	}

	if (paramIds)
		delete [] paramIds;

	return true;
}

//------------------------------------------------------------------------
// VstEditorClassesTest
//------------------------------------------------------------------------
VstEditorClassesTest::VstEditorClassesTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{}

//------------------------------------------------------------------------
bool PLUGIN_API VstEditorClassesTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	// no controller is allowed...
	FUID controllerClassUID;
	if (vstPlug->getControllerClassId (controllerClassUID) != kResultOk)
	{
		testResult->addMessage ("This component does not export an edit controller class ID!!!");
		return true;
	}
	if (controllerClassUID.isValid () == false)
	{
		testResult->addErrorMessage ("The edit controller class has no valid UID!!!");
		return false;
	}

	testResult->addMessage ("This component has an edit controller class");

	char cidString[50];
	controllerClassUID.toRegistryString (cidString);
	sprintf (text, "Controller CID: %s", cidString);
	testResult->addMessage (text);

	return true;
}


//------------------------------------------------------------------------
// VstUnitInfoTest
//------------------------------------------------------------------------
VstUnitInfoTest::VstUnitInfoTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{
}

//------------------------------------------------------------------------
bool VstUnitInfoTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	FUnknownPtr<IUnitInfo> iUnitInfo (controller);
	if (iUnitInfo)
	{
		int32 unitCount = iUnitInfo->getUnitCount ();
		if (unitCount <= 0)
		{
			testResult->addErrorMessage ("No units found!!!");
			return false;
		}
		else
		{
			sprintf (text, "This component has %d unit(s).", unitCount);
			testResult->addMessage (text);
		}
		
		int32* unitIds = new int32[unitCount];
		ArrayDeleter<int32> unitIdDeleter (unitIds);

		for (int32 unitIndex = 0; unitIndex < unitCount; unitIndex++)
		{
			UnitInfo unitInfo = {0};

			if (iUnitInfo->getUnitInfo (unitIndex, unitInfo) == kResultOk)
			{
				int32 unitId = unitInfo.id;
				unitIds[unitIndex] = unitId;
				if (unitId < 0)
				{
					sprintf (text, "Unit %03d: Invalid ID!", unitIndex);
					testResult->addErrorMessage (text);
					return false;
				}

				// check if ID is already used by another unit
				for (int32 idIndex = 0; idIndex < unitIndex; idIndex++)
				{
					if (unitIds[idIndex] == unitIds[unitIndex])
					{
						sprintf (text, "Unit %03d: ID already used!!!", unitIndex);
						testResult->addErrorMessage (text);
						return false;
					}
				}

				char unitName[256];
				UString (unitInfo.name, USTRINGSIZE (unitInfo.name)).toAscii (unitName, 256);
				if (unitName [0] == 0)
				{
					sprintf (text, "Unit %03d: No name!", unitIndex);
					testResult->addErrorMessage (text);
					return false;				
				}

				int32 parentUnitId = unitInfo.parentUnitId;
				if (parentUnitId < -1)
				{
					sprintf (text, "Unit %03d: Invalid parent ID!", unitIndex);
					testResult->addErrorMessage (text);
					return false;	
				}
				else if (parentUnitId == unitId)
				{
					sprintf (text, "Unit %03d: Parent ID is equal to Unit ID!", unitIndex);
					testResult->addErrorMessage (text);
					return false;	
				}

				int32 unitProgramListId = unitInfo.programListId;
				if (unitProgramListId < -1)
				{
					sprintf (text, "Unit %03d: Invalid programlist ID!", unitIndex);
					testResult->addErrorMessage (text);
					return false;	
				}

				sprintf (text, "Unit%03d (ID = %d): \"%s\" (parent ID = %d, programlist ID = %d)", 
					 unitIndex, unitId, unitName, parentUnitId, unitProgramListId);
				
				testResult->addMessage (text);

				// test select Unit
				if (iUnitInfo->selectUnit (unitIndex) == kResultTrue)
				{
					UnitID newSelected = iUnitInfo->getSelectedUnit ();
					if (newSelected != unitIndex)
					{
						sprintf (text, "The host has selected Unit ID = %d but getSelectedUnit returns ID = %d!!!", 
							 unitIndex, newSelected);
						testResult->addMessage (text);
					}
				}
			}
			else
			{
				sprintf (text, "Unit%03d: No unit info!", unitIndex);
			}
		}	
	}
	else
	{
		testResult->addMessage ("This component has no units.");
	}

	return true;
}


//------------------------------------------------------------------------
// VstUnitStructureTest
//------------------------------------------------------------------------
VstUnitStructureTest::VstUnitStructureTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{
}

//------------------------------------------------------------------------
bool VstUnitStructureTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	FUnknownPtr<IUnitInfo> iUnitInfo (controller);
	if (iUnitInfo)
	{
		int32 unitCount = iUnitInfo->getUnitCount ();
		if (unitCount <= 0)
		{
			testResult->addErrorMessage ("No units found!!!");
			return false;
		}

		UnitInfo unitInfo = {0};
		UnitInfo tmpInfo = {0};
		bool rootFound = false;
		for (int32 unitIndex = 0; unitIndex < unitCount; unitIndex++)
		{
			if (iUnitInfo->getUnitInfo (unitIndex, unitInfo) == kResultOk)
			{
				// check parent Id
				if (unitInfo.parentUnitId != kNoParentUnitId) //-1: connected to root
				{
					bool noParent = true;
					for (int32 i = 0; i < unitCount; i++)
					{
						if (iUnitInfo->getUnitInfo (i, tmpInfo) == kResultOk)
						{
							if (unitInfo.parentUnitId == tmpInfo.id)
							{
								noParent = false;
								break;
							}
						}
					}
					if (noParent && unitInfo.parentUnitId != kRootUnitId)
					{
						sprintf (text, "Unit %03d: Parent does not exist!!", unitInfo.id);
						testResult->addErrorMessage (text);
						return false;
					}
				}
				else if (!rootFound)
				{
					// root Unit have always the rootID
					if (unitInfo.id != kRootUnitId)
					{
						// we should have a root unit id
						sprintf (text, "Unit %03d: Should be the Root Unit => id should be %03d!!", unitInfo.id, kRootUnitId);
						testResult->addErrorMessage (text);
						return false;
					}
					rootFound = true;
				}
				else
				{
					sprintf (text, "Unit %03d: Has no parent, but there is already a root.", unitInfo.id);
					testResult->addErrorMessage (text);
					return false;
				}
			}
			else
			{
				sprintf (text, "Unit %03d: No unit info.", unitInfo.id);
				testResult->addErrorMessage (text);
				return false;
			}
		}
		testResult->addMessage ("All units have valid parent IDs.");
	}
	else
	{
		testResult->addMessage ("This component does not support IUnitInfo!");
	}
	return true;
}


//------------------------------------------------------------------------
// VstProgramInfoTest
//------------------------------------------------------------------------
VstProgramInfoTest::VstProgramInfoTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{
}

//------------------------------------------------------------------------
bool VstProgramInfoTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	FUnknownPtr<IUnitInfo> iUnitInfo (controller);
	if (iUnitInfo)
	{
		int32 programListCount = iUnitInfo->getProgramListCount ();
		if (programListCount == 0)
		{
			testResult->addMessage ("This component does not export any programs.");
			return true;
		}
		else if (programListCount < 0)
		{
			testResult->addErrorMessage ("IUnitInfo::getProgramListCount () returned a negative number.");
			return false;
		}

		// used to check double IDs
		int32* programListIds = new int32 [programListCount];
		ArrayDeleter<int32> idDeleter (programListIds);

		for (int32 programListIndex = 0; programListIndex < programListCount; programListIndex++)
		{
			// get programm list info 
			ProgramListInfo programListInfo;
			if (iUnitInfo->getProgramListInfo (programListIndex, programListInfo) == kResultOk)
			{
				int32 programListId = programListInfo.id;
				programListIds [programListIndex] = programListId;
				if (programListId < 0)
				{
					sprintf (text, "Programlist%03d: Invalid ID!!!", programListIndex);
					testResult->addErrorMessage (text);
					return false;
				}

				// check if ID is already used by another parameter
				for (int32 idIndex = 0; idIndex < programListIndex; idIndex++)
				{
					if (programListIds[idIndex] == programListIds[programListIndex])
					{
						sprintf (text, "Programlist%03d: ID already used!!!", programListIndex);
						testResult->addErrorMessage (text);
						return false;
					}
				}

				char programListName[256];
				UString (programListInfo.name, USTRINGSIZE (programListInfo.name)).toAscii (programListName, 256);
				if (programListName[0] == 0)
				{
					sprintf (text, "Programlist%03d (ID = %d): No name!!!", programListIndex, programListId);
					testResult->addErrorMessage (text);
					return false;
				}

				int32 programCount = programListInfo.programCount;
				if (programCount <= 0)
				{
					sprintf (text, "Programlist%03d (ID = %d): \"%s\" No programs!!!", programListIndex, programListId, programListName);
					testResult->addErrorMessage (text);
					return false;
				}
				
				sprintf (text, "Programlist%03d (ID = %d):  \"%s\" (%d programs).", programListIndex, programListId, programListName, programCount);
				testResult->addMessage (text);

				for (int32 programIndex = 0; programIndex < programCount; programIndex++)
				{	
					Vst::TChar programName[256];
					if (iUnitInfo->getProgramName (programListId, programIndex, programName) == kResultOk)
					{
						if (programName[0] == 0)
						{
							sprintf (text, "Programlist%03d-Program%03d: has no name!!!", programListIndex, programIndex);
							testResult->addErrorMessage (text);
							return false;
						}

						char asciiString[256] = {0};
						UString (programName, USTRINGSIZE (programName)).toAscii (asciiString, sizeof(asciiString));
						sprintf (text, "Programlist%03d-Program%03d: \"%s\"", programListIndex, programIndex, asciiString);
	
						String128 programInfo = {0};
						if (iUnitInfo->getProgramInfo (programListId, programIndex, PresetAttributes::kInstrument, programInfo) == kResultOk)
						{
							UString (programInfo, USTRINGSIZE (programInfo)).toAscii (asciiString, sizeof(asciiString));
							strcat (text, " (instrument = \"");
							strcat (text, asciiString);
							strcat (text, "\")");
						}
					
						testResult->addMessage (text);
					}
				}
			}
		}
	}
	else
		testResult->addMessage ("This component does not export any programs.");

	return true;
}

//------------------------------------------------------------------------
// VstValidStateTransitionTest
//------------------------------------------------------------------------
VstValidStateTransitionTest::VstValidStateTransitionTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{}

//------------------------------------------------------------------------
bool PLUGIN_API VstValidStateTransitionTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	for (int32 i = 0; i < 3; i++)
	{
		tresult result = vstPlug->setActive (true);
		if (result != kResultTrue)
			return false;

		result = vstPlug->setActive (false);
		if (result != kResultTrue)
			return false;

		result = vstPlug->terminate ();
		if (result != kResultTrue)
			return false;

		result = vstPlug->initialize (gStandardPluginContext);
		if (result != kResultTrue)
			return false;
	}
	return true;
}

//------------------------------------------------------------------------
// VstInvalidStateTransitionTest
//------------------------------------------------------------------------
VstInvalidStateTransitionTest::VstInvalidStateTransitionTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{}

//------------------------------------------------------------------------
bool PLUGIN_API VstInvalidStateTransitionTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	// created
	tresult result = vstPlug->initialize (gStandardPluginContext);
	if (result == kResultFalse)
			return false;

	// initialized
	result = vstPlug->setActive (false);
	if (result == kResultOk)
			return false;

	result = vstPlug->setActive (true);
	if (result == kResultFalse)
			return false;

	// allocated
	result = vstPlug->initialize (gStandardPluginContext);
	if (result == kResultOk)
			return false;

	result = vstPlug->setActive (false);
	if (result == kResultFalse)
			return false;

	// deallocated (initialized)
	result = vstPlug->initialize (gStandardPluginContext);
	if (result == kResultOk)
		return false;

	result = vstPlug->terminate ();
	if (result == kResultFalse)
			return false;

	// terminated (created)
	result = vstPlug->setActive (false);
	if (result == kResultOk)
			return false;

	result = vstPlug->terminate ();
	if (result == kResultOk)
			return false;

	return true;
}

//------------------------------------------------------------------------
// VstRepeatIdenticalStateTransitionTest
//------------------------------------------------------------------------
VstRepeatIdenticalStateTransitionTest::VstRepeatIdenticalStateTransitionTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{}

//------------------------------------------------------------------------
bool VstRepeatIdenticalStateTransitionTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	tresult result = vstPlug->initialize (gStandardPluginContext);
	if (result != kResultFalse)
		return false;

	result = vstPlug->setActive (true);
	if (result != kResultOk)
		return false;

	result = vstPlug->setActive (true);
	if (result != kResultFalse)
		return false;

	result = vstPlug->setActive (false);
	if (result != kResultOk)
		return false;

	result = vstPlug->setActive (false);
	if (result == kResultOk)
		return false;

	result = vstPlug->terminate ();
	if (result != kResultOk)
		return false;

	result = vstPlug->terminate ();
	if (result == kResultOk)
		return false;

	result = vstPlug->initialize (gStandardPluginContext);
	if (result != kResultOk)
		return false;

	return true;
}

//------------------------------------------------------------------------
// VstBusConsistencyTest
//------------------------------------------------------------------------
VstBusConsistencyTest::VstBusConsistencyTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{
}

//------------------------------------------------------------------------
bool PLUGIN_API VstBusConsistencyTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	bool failed = false;
	int32 numFalseDescQueries = 0;

	for (MediaType mediaType = kAudio; mediaType < kNumMediaTypes; mediaType++)
	{
		for (BusDirection dir = kInput; dir <= kOutput; dir++)
		{
			int32 numBusses = vstPlug->getBusCount (mediaType, dir);	
			if (numBusses > 0)
			{
				BusInfo* busArray = new BusInfo[numBusses];
				if (busArray)
				{
					// get all bus descriptions and save them in an array
					int32 busIndex;
					for (busIndex = 0; busIndex < numBusses; busIndex++)
					{
						memset (&busArray[busIndex], 0, sizeof (BusInfo));
						vstPlug->getBusInfo (mediaType, dir, busIndex, busArray[busIndex]);
					}	
					
					// test by getting descriptions randomly and comparing with saved ones
					int32 randIndex = 0;
					BusInfo info = {0};

					for (busIndex = 0; busIndex <= numBusses * NUM_ITERATIONS; busIndex++)
					{
						randIndex = rand () % (numBusses);

						memset (&info, 0, sizeof (BusInfo));
						
						tresult result = vstPlug->getBusInfo (mediaType, dir, randIndex, info);
						if (memcmp ((void*)&busArray[randIndex], (void*)&info, sizeof (BusInfo)) != BUFFERS_ARE_EQUAL)
						{
							failed |= true;
							numFalseDescQueries++;
						}
					}
					delete [] busArray;
				}
			}
		}
	}

	if (numFalseDescQueries > 0)
	{
		sprintf (text, "The component returned %i inconsistent busses! (getBusInfo () returns sometime different info for the same bus!", numFalseDescQueries);
		testResult->addErrorMessage (text);
	}

	return failed == false;
}

//------------------------------------------------------------------------
// VstBusInvalidIndexTest
//------------------------------------------------------------------------
VstBusInvalidIndexTest::VstBusInvalidIndexTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{
}

//------------------------------------------------------------------------
bool PLUGIN_API VstBusInvalidIndexTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	bool failed = false;
	int32 numInvalidDesc = 0;

	for (MediaType mediaType = kAudio; mediaType < kNumMediaTypes; mediaType++)
	{
		int32 numBusses = vstPlug->getBusCount (mediaType, kInput) + vstPlug->getBusCount (mediaType, kOutput);
		for (BusDirection dir = kInput; dir <= kOutput; dir++)
		{
			BusInfo descBefore = {0};
			BusInfo descAfter = {0};
	
			int32 randIndex = 0;

			// todo: rand with negative numbers
			for (int32 i = 0; i <= numBusses * NUM_ITERATIONS; i++)
			{
				randIndex = rand ();
				if (0 > randIndex || randIndex > numBusses)
				{
					tresult result = vstPlug->getBusInfo (mediaType, dir, randIndex, descAfter);
					
					if (memcmp ((void*)&descBefore, (void*)&descAfter, sizeof (BusInfo)) != 0)
					{
						failed |= true;
						numInvalidDesc++;
					}
				}
			}
		}
	}

	if (numInvalidDesc > 0)
	{
		sprintf (text, "The component returned %i busses queried with an invalid index!", numInvalidDesc);
		testResult->addErrorMessage (text);
	}

	return failed == false;
}

//------------------------------------------------------------------------
// VstProcessTest
//------------------------------------------------------------------------
VstProcessTest::VstProcessTest (IPlugProvider* plugProvider, ProcessSampleSize sampl)
: VstTestEnh (plugProvider, sampl)
{
	//memset (&processData, 0, sizeof (HostProcessData));
	processData.numSamples = DEFAULT_BLOCK_SIZE;
	processData.symbolicSampleSize = sampl;

	processSetup.processMode = kRealtime;
	processSetup.symbolicSampleSize = sampl;
	processSetup.maxSamplesPerBlock = MAX_BLOCK_SIZE;
	processSetup.sampleRate = DEFAULT_SAMPLE_RATE;
}

//------------------------------------------------------------------------
bool PLUGIN_API VstProcessTest::setup ()
{
	if (!VstTestEnh::setup ())
		return false;
	if (!vstPlug || !audioEffect)
		return false;
	if (processSetup.symbolicSampleSize != processData.symbolicSampleSize)
		return false;
	if (audioEffect->canProcessSampleSize (processSetup.symbolicSampleSize) != kResultOk)
		return true;	// this fails in run (..)

	prepareProcessing ();

	if (vstPlug->setActive (true) != kResultTrue)
		return false;
	return true;
}

//------------------------------------------------------------------------
bool PLUGIN_API VstProcessTest::run (ITestResult* testResult)
{
	if (!testResult || !audioEffect)
		return false;
	if (processSetup.symbolicSampleSize != processData.symbolicSampleSize)
		return false;
	if (!canProcessSampleSize (testResult))
		return true;

	audioEffect->setProcessing (true);

	for (int32 i = 0; i < NUM_AUDIO_BLOCKS_TO_PROCESS; i++)
	{
		if (!preProcess (testResult))
			return false;
		tresult result = audioEffect->process (processData);
		if (result != kResultOk)
		{
			testResult->addErrorMessage ("IAudioProcessor::process (..) failed.");
			audioEffect->setProcessing (false);
			return false;
		}
		if (!postProcess (testResult))
		{
			audioEffect->setProcessing (false);
			return false;
		}
	}
	audioEffect->setProcessing (false);
	return true;
}

//------------------------------------------------------------------------
bool VstProcessTest::preProcess (ITestResult* testResult)
{
	return true;
}

//------------------------------------------------------------------------
bool VstProcessTest::postProcess (ITestResult* testResult)
{
	return true;
}

//------------------------------------------------------------------------
bool VstProcessTest::canProcessSampleSize (ITestResult* testResult)
{
	if (!testResult || !audioEffect)
		return false;
	if (processSetup.symbolicSampleSize != processData.symbolicSampleSize)
		return false;
	if (audioEffect->canProcessSampleSize (processSetup.symbolicSampleSize) != kResultOk)
	{
		if (processSetup.symbolicSampleSize == kSample32)
			testResult->addMessage ("32bit Audio Processing not supported.");
		else
			testResult->addMessage ("64bit Audio Processing not supported.");
		return false;
	}
	return true;
}

//------------------------------------------------------------------------
bool PLUGIN_API VstProcessTest::teardown ()
{
	unprepareProcessing ();
	if (!vstPlug || (vstPlug->setActive (false) != kResultOk))
		return false;
	return VstTestEnh::teardown ();
}

//------------------------------------------------------------------------
bool VstProcessTest::prepareProcessing ()
{
	if (!vstPlug || !audioEffect)
		return false;

	if (audioEffect->setupProcessing (processSetup) == kResultOk)
	{
		processData.prepare (*vstPlug);

		for (BusDirection dir = kInput; dir <= kOutput; dir++)
		{
			int32 numBusses = vstPlug->getBusCount (kAudio, dir);
			AudioBusBuffers* audioBuffers = dir == kInput ? processData.inputs : processData.outputs; //new AudioBusBuffers [numBusses];
			if (!setupBuffers (numBusses, audioBuffers, dir))
				return false;

			if (dir == kInput)
			{
				processData.numInputs = numBusses;
				processData.inputs = audioBuffers;
			}
			else
			{
				processData.numOutputs = numBusses;
				processData.outputs = audioBuffers;
			}
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
bool VstProcessTest::setupBuffers (int32 numBusses, AudioBusBuffers* audioBuffers, BusDirection dir)
{
	if (((numBusses > 0) && !audioBuffers) || !vstPlug)
		return false;
	for (int32 busIndex = 0; busIndex < numBusses; busIndex++) // busses
	{
		BusInfo busInfo;
		if (vstPlug->getBusInfo (kAudio, dir, busIndex, busInfo) == kResultTrue)
		{
			if (!setupBuffers (audioBuffers[busIndex]))
				return false;

			if ((busInfo.flags & BusInfo::kDefaultActive) != 0)
			{
				for (int32 channelIndex = 0; channelIndex < busInfo.channelCount; channelIndex++) // channels per bus
					audioBuffers[busIndex].silenceFlags |= (uint64)CHANNEL_IS_SILENT << channelIndex;
			}
		}
		else
			return false;
	}
	return true;
}

//------------------------------------------------------------------------
bool VstProcessTest::setupBuffers (AudioBusBuffers& audioBuffers)
{
	if (processSetup.symbolicSampleSize != processData.symbolicSampleSize)
		return false;
	audioBuffers.silenceFlags = 0;
	for (int32 channelIndex = 0; channelIndex < audioBuffers.numChannels; channelIndex++)
	{
		if (processSetup.symbolicSampleSize == kSample32)
		{
			if (audioBuffers.channelBuffers32)
			{
				audioBuffers.channelBuffers32[channelIndex] = new Sample32[processSetup.maxSamplesPerBlock];
				if (audioBuffers.channelBuffers32[channelIndex])
					memset (audioBuffers.channelBuffers32[channelIndex], 0, processSetup.maxSamplesPerBlock * sizeof (Sample32));
				else
					return false;
			}
			else
				return false;
		}
		else if (processSetup.symbolicSampleSize == kSample64)
		{
			if (audioBuffers.channelBuffers64)
			{
				audioBuffers.channelBuffers64[channelIndex] = new Sample64[processSetup.maxSamplesPerBlock];
				if (audioBuffers.channelBuffers64[channelIndex])
					memset (audioBuffers.channelBuffers64[channelIndex], 0, processSetup.maxSamplesPerBlock * sizeof (Sample64));
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	return true;
}

//------------------------------------------------------------------------
bool VstProcessTest::unprepareProcessing ()
{
	bool ret = true;
	ret &= freeBuffers (processData.numInputs, processData.inputs);
	ret &= freeBuffers (processData.numOutputs, processData.outputs);
	processData.unprepare ();
	return ret;
}

//------------------------------------------------------------------------
bool VstProcessTest::freeBuffers (int32 numBuses, AudioBusBuffers* buses)
{
	if (processSetup.symbolicSampleSize != processData.symbolicSampleSize)
		return false;
	for (int32 busIndex = 0; busIndex < numBuses; busIndex++)
	{
		for (int32 channelIndex = 0; channelIndex < buses[busIndex].numChannels; channelIndex++)
		{
			if (processSetup.symbolicSampleSize == kSample32)
				delete [] buses[busIndex].channelBuffers32[channelIndex];
			else if (processSetup.symbolicSampleSize == kSample64)
				delete [] buses[busIndex].channelBuffers64[channelIndex];
			else
				return false;
		}
	}
	return true;
}


//------------------------------------------------------------------------
// VstSpeakerArrangementTest
//------------------------------------------------------------------------
VstSpeakerArrangementTest::VstSpeakerArrangementTest (IPlugProvider* plugProvider, ProcessSampleSize sampl, 
													  SpeakerArrangement inSpArr, SpeakerArrangement outSpArr)
: VstProcessTest (plugProvider, sampl)
, inSpArr (inSpArr)
, outSpArr (outSpArr)
{
}

//------------------------------------------------------------------------
const char* VstSpeakerArrangementTest::getSpeakerArrangementName (SpeakerArrangement spArr)
{
	const char* saName = 0;
	switch (spArr)
	{
		case SpeakerArr::kMono:				saName = "Mono"; break;
		case SpeakerArr::kStereo:			saName = "Stereo"; break;
		case SpeakerArr::kStereoSurround:	saName = "StereoSurround"; break;
		case SpeakerArr::kStereoCenter:		saName = "StereoCenter"; break;
		case SpeakerArr::kStereoSide:		saName = "StereoSide"; break;
		case SpeakerArr::kStereoCLfe:		saName = "StereoCLfe"; break;
		case SpeakerArr::k30Cine:			saName = "30Cine"; break;
		case SpeakerArr::k30Music:			saName = "30Music"; break;
		case SpeakerArr::k31Cine:			saName = "31Cine"; break;
		case SpeakerArr::k31Music:			saName = "31Music"; break;
		case SpeakerArr::k40Cine:			saName = "40Cine"; break;
		case SpeakerArr::k40Music:			saName = "40Music"; break;
		case SpeakerArr::k41Cine:			saName = "41Cine"; break;
		case SpeakerArr::k41Music:			saName = "41Music"; break;
		case SpeakerArr::k50:				saName = "50"; break;
		case SpeakerArr::k51:				saName = "51"; break;
		case SpeakerArr::k60Cine:			saName = "60Cine"; break;
		case SpeakerArr::k60Music:			saName = "60Music"; break;
		case SpeakerArr::k61Cine:			saName = "61Cine"; break;
		case SpeakerArr::k61Music:			saName = "61Music"; break;
		case SpeakerArr::k70Cine:			saName = "70Cine"; break;
		case SpeakerArr::k70Music:			saName = "70Music"; break;
		case SpeakerArr::k71Cine:			saName = "71Cine"; break;
		case SpeakerArr::k71Music:			saName = "71Music"; break;
		case SpeakerArr::k80Cine:			saName = "80Cine"; break;
		case SpeakerArr::k80Music:			saName = "80Music"; break;
		case SpeakerArr::k81Cine:			saName = "81Cine"; break;
		case SpeakerArr::k81Music:			saName = "81Music"; break;
		case SpeakerArr::k102:				saName = "102"; break;
		case SpeakerArr::k102Plus:			saName = "102Plus"; break;
		case SpeakerArr::kCube:				saName = "Cube"; break;
		case SpeakerArr::kBFormat:			saName = "BFormat"; break;
		case SpeakerArr::kEmpty:			saName = "Empty"; break;
		default:							saName = "Unknown"; break;
	}
	return saName;
}

//------------------------------------------------------------------------
const char* VstSpeakerArrangementTest::getName () const
{
	const char* inSaName = getSpeakerArrangementName (inSpArr);
	const char* outSaName = getSpeakerArrangementName (outSpArr);
	if (inSaName && outSaName)
	{
		static char text[256];
		sprintf (text, "In: %s: %d Channels, Out: %s: %d Channels", inSaName, SpeakerArr::getChannelCount (inSpArr), outSaName, SpeakerArr::getChannelCount (outSpArr));
		return text;
	}
	return "error";
}

//------------------------------------------------------------------------
bool VstSpeakerArrangementTest::prepareProcessing ()
{
	if (!vstPlug || !audioEffect)
		return false;
	
	bool ret = true;
	int32 is = vstPlug->getBusCount (kAudio, kInput);
	SpeakerArrangement* inSpArrs = new SpeakerArrangement[is];
	for (int32 i = 0; i < is; i++)
		inSpArrs[i] = inSpArr;

	int32 os = vstPlug->getBusCount (kAudio, kOutput);
	SpeakerArrangement* outSpArrs = new SpeakerArrangement[os];
	for (int32 o = 0; o < os; o++)
		outSpArrs[o] = outSpArr;
	
	if (audioEffect->setBusArrangements (inSpArrs, is, outSpArrs, os) != kResultTrue)
		ret = false;
	
	ret &= VstProcessTest::prepareProcessing ();
	
	delete [] inSpArrs;
	delete [] outSpArrs;
	
	return ret;
}

//------------------------------------------------------------------------
bool VstSpeakerArrangementTest::run (ITestResult* testResult)
{
	if (!testResult || !audioEffect || !vstPlug)
		return false;
	
	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	SpeakerArrangement spArr = SpeakerArr::kEmpty;
	SpeakerArrangement compareSpArr = SpeakerArr::kEmpty;
	BusDirections bd = kInput;
	BusInfo busInfo = {0};
	int32 count = 0;
	do
	{
		count++;
		int32 numBusses = 0;
		if (bd == kInput)
		{
			numBusses = processData.numInputs;
			compareSpArr = inSpArr;
		}
		else
		{
			numBusses = processData.numOutputs;
			compareSpArr = outSpArr;
		}
		for (int32 i = 0; i < numBusses; i++)
		{
			if (audioEffect->getBusArrangement (bd, i, spArr) != kResultTrue)
			{
				testResult->addErrorMessage ("IAudioProcessor::getBusArrangement (..) failed.");
				return false;
			}
			if (spArr != compareSpArr)
			{
				char msg[255] = {0};
				sprintf (msg, "%s %sSpeakerArrangement is not supported. Plug-in suggests: %s.", 
								getSpeakerArrangementName (compareSpArr), 
								bd == kInput ? "Input-" : "Output-", 
								getSpeakerArrangementName (spArr));
				testResult->addMessage (msg);
			}
			if (vstPlug->getBusInfo (kAudio, bd, i, busInfo) != kResultTrue)
			{
				testResult->addErrorMessage ("IComponent::getBusInfo (..) failed.");
				return false;
			}
			if (SpeakerArr::getChannelCount (spArr) != busInfo.channelCount)
			{
				testResult->addErrorMessage ("SpeakerArrangement mismatch (BusInfo::channelCount inconsistency).");
				return false;
			}
		}
		bd = kOutput;
	} while (count < 2);
	
	bool ret = true;
	
	// not a Pb ret &= verifySA (processData.numInputs, processData.inputs, inSpArr, testResult);
	// not a Pb ret &= verifySA (processData.numOutputs, processData.outputs, outSpArr, testResult);
	ret &= VstProcessTest::run (testResult);
	
	return ret;
}

//------------------------------------------------------------------------
bool VstSpeakerArrangementTest::verifySA (int32 numBusses, AudioBusBuffers* busses, SpeakerArrangement spArr, ITestResult* testResult)
{
	if (!testResult || !busses)
		return false;
	for (int32 i = 0; i < numBusses; i++)
	{
		if (busses[i].numChannels != SpeakerArr::getChannelCount (spArr))
		{
			testResult->addErrorMessage ("ChannelCount is not matching SpeakerArrangement.");
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------
// VstAutomationTest helper classes
//------------------------------------------------------------------------
class ParamPoint
{
public:
	ParamPoint () : offsetSamples (-1), value (0.), read (false) {}
	void set (int32 offsetSamples, double value) {this->offsetSamples = offsetSamples; this->value = value;}
	void get (int32& offsetSamples, double& value) {offsetSamples = this->offsetSamples; value = this->value; read = true;}
	bool wasRead () const {return read;}
private:
	int32 offsetSamples;
	double value;
	bool read;
};

//------------------------------------------------------------------------
class ParamChanges : public IParamValueQueue
{
public:
	ParamChanges () : id (-1), numPoints (0), points (0), numUsedPoints (0), processedFrames (0) {FUNKNOWN_CTOR}
	~ParamChanges () {if (points) delete [] points; FUNKNOWN_DTOR}

	DECLARE_FUNKNOWN_METHODS

	void init (ParamID id, int32 numPoints)
	{
		this->id = id;
		this->numPoints = numPoints;
		numUsedPoints = 0;
		points = new ParamPoint [numPoints];
		processedFrames = 0;
	}
	bool setPoint (int32 index, int32 offsetSamples, double value)
	{
		if (points && (index >= 0) && (index == numUsedPoints) && (index < numPoints))
		{
			points[index].set (offsetSamples, value);
			numUsedPoints++;
			return true;
		}
		if (!points)
			return true;
		return false;
	}
	void resetPoints ()	{numUsedPoints = 0; processedFrames = 0;}
	int32 getProcessedFrames () const {return processedFrames;}
	void setProcessedFrames (int32 amount) {processedFrames = amount;}
	bool havePointsBeenRead (bool atAll)
	{
		bool all = true;
		for (int32 i = 0; i < getPointCount (); i++)
		{
			if (points[i].wasRead ())
			{
				if (atAll)
					return true;
			}
			else if (!atAll)
				return false;
		}
		return !atAll;
	}

	//---for IParamValueQueue-------------------------
	ParamID PLUGIN_API getParameterId () { return id; }
	int32 PLUGIN_API getPointCount () { return numUsedPoints; }
	tresult PLUGIN_API getPoint (int32 index, int32& offsetSamples, double& value)
	{
		if (points && (index < numUsedPoints) && (index >= 0))
		{
			points[index].get (offsetSamples, value);
			return kResultTrue;
		}
		return kResultFalse;
	}
	tresult PLUGIN_API addPoint (int32 offsetSamples, double value, int32& index)
	{
		return kResultFalse;
	}
	//---------------------------------------------------------
		
private:
	ParamID id;
	int32 numPoints;
	int32 numUsedPoints;
	ParamPoint* points;
	int32 processedFrames;
};

IMPLEMENT_FUNKNOWN_METHODS (ParamChanges, IParamValueQueue, IParamValueQueue::iid)

//------------------------------------------------------------------------
// VstAutomationTest
//------------------------------------------------------------------------
IMPLEMENT_FUNKNOWN_METHODS (VstAutomationTest, IParameterChanges, IParameterChanges::iid)

//------------------------------------------------------------------------
VstAutomationTest::VstAutomationTest (IPlugProvider* plugProvider, ProcessSampleSize sampl, int32 everyNSamples, int32 numParams, bool sampleAccuracy)
: VstProcessTest (plugProvider, sampl)
, paramChanges (0)
, countParamChanges (0)
, numParams (numParams)
, everyNSamples (everyNSamples)
, sampleAccuracy (sampleAccuracy)
, onceExecuted (false)
{
	FUNKNOWN_CTOR
}

//------------------------------------------------------------------------
VstAutomationTest::~VstAutomationTest ()
{
	FUNKNOWN_DTOR
}

//------------------------------------------------------------------------
const char* VstAutomationTest::getName () const
{
	static char text[256] = {0};
	const char* accTxt = "Sample";
	if (!sampleAccuracy)
		accTxt = "Block";
	if (numParams < 1)
		sprintf (text, "Accuracy: %s, All Parameters, Change every %d Samples", accTxt, everyNSamples);
	else
		sprintf (text, "Accuracy: %s, %d Parameters, Change every %d Samples", accTxt, numParams, everyNSamples);
	return text;
}

//------------------------------------------------------------------------
bool VstAutomationTest::setup ()
{
	onceExecuted = false;
	if (!VstProcessTest::setup ())
		return false;

	if (!controller)
		return false;
	if ((numParams < 1) || (numParams > controller->getParameterCount ()))
		numParams = controller->getParameterCount ();
	if (audioEffect && (numParams > 0))
	{
		paramChanges = new ParamChanges [numParams];
		ParameterInfo inf = {0};
		for (int32 i = 0; i < numParams; i++)
		{
			tresult r = controller->getParameterInfo (i, inf);
			if (r != kResultTrue)
				return false;
			if ((inf.flags & inf.kCanAutomate) != 0)
				paramChanges[i].init (inf.id, processSetup.maxSamplesPerBlock);
		}
		return true;
	}
	return numParams == 0;
}

//------------------------------------------------------------------------
bool VstAutomationTest::run (ITestResult* testResult)
{
	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	if (!testResult)
		return false;
	if (numParams == 0)
		testResult->addMessage ("No Parameters present.");
	bool ret = VstProcessTest::run (testResult);
	return ret;
}

//------------------------------------------------------------------------
bool VstAutomationTest::teardown ()
{
	if (paramChanges)
	{
		delete [] paramChanges;
		paramChanges = 0;
	}
	return VstProcessTest::teardown ();
}

//------------------------------------------------------------------------
bool VstAutomationTest::preProcess (ITestResult* testResult)
{
	if (!testResult)
		return false;
	if (!paramChanges)
		return numParams == 0;
	bool check = true;
	for (int32 i = 0; i < numParams; i++)
	{
		paramChanges[i].resetPoints ();
		int32 point = 0;
		for (int32 pos = 0; pos < processData.numSamples; pos++)
		{
			bool add = (rand () % everyNSamples) == 0;
			if (!onceExecuted)
			{
				if (pos == 0)
				{
					add = true;
					if (!sampleAccuracy)
						onceExecuted = true;
				}
				else if ((pos == 1) && sampleAccuracy)
				{
					add = true;
					onceExecuted = true;
				}
			}
			if (add)
				check &= paramChanges[i].setPoint (point++, pos, ((float)(rand () % 1000000000)) / 1000000000.0);
		}
		if (check)
			processData.inputParameterChanges = this;
	}
	return check;
}

//------------------------------------------------------------------------
bool VstAutomationTest::postProcess (ITestResult* testResult)
{
	if (!testResult)
		return false;
	if (!paramChanges)
		return numParams == 0;
	
	for (int32 i = 0; i < numParams; i++)
	{
		if ((paramChanges[i].getPointCount () > 0) && !paramChanges[i].havePointsBeenRead (!sampleAccuracy))
		{
			if (sampleAccuracy)
				testResult->addMessage ("Not all points have been read via IParameterChanges");
			else
				testResult->addMessage ("No point at all has been read via IParameterChanges");
			return true;// should not be a problem
		}
	}
	return true;
}

//------------------------------------------------------------------------
int32 VstAutomationTest::getParameterCount ()
{
	if (paramChanges)
		return numParams;
	return 0;
}

//------------------------------------------------------------------------
IParamValueQueue* VstAutomationTest::getParameterData (int32 index)
{
	if (paramChanges && (index >= 0) && (index < getParameterCount ()))
		return &paramChanges[index];
	return 0;
}

//------------------------------------------------------------------------
IParamValueQueue* VstAutomationTest::addParameterData (const Vst::ParamID& id, int32& index)
{
	return 0;
}

//------------------------------------------------------------------------
// VstFlushParamTest
//------------------------------------------------------------------------
VstFlushParamTest::VstFlushParamTest (IPlugProvider* plugProvider, ProcessSampleSize sampl)
: VstAutomationTest (plugProvider, sampl, 100, 1, false)
{
}

//------------------------------------------------------------------------
bool PLUGIN_API VstFlushParamTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult || !audioEffect)
		return false;
	if (!canProcessSampleSize (testResult))
		return true;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	unprepareProcessing ();

	processData.numSamples = 0;
	processData.numInputs = 0;
	processData.numOutputs = 0;
	processData.inputs = 0;
	processData.outputs = 0;

	audioEffect->setProcessing (true);
	
	preProcess (testResult);
	
	tresult result = audioEffect->process (processData);
	if (result != kResultOk)
	{
		char text[256];
		sprintf (text, "The component failed to process without audio buffers!");
		testResult->addErrorMessage (text);
		audioEffect->setProcessing (false);
		return false;
	}
	
	postProcess (testResult);
		
	audioEffect->setProcessing (false);

	return true;
}


//------------------------------------------------------------------------
// VstSilenceFlagsTest
//------------------------------------------------------------------------
VstSilenceFlagsTest::VstSilenceFlagsTest (IPlugProvider* plugProvider, ProcessSampleSize sampl)
: VstProcessTest (plugProvider, sampl)
{
}

//------------------------------------------------------------------------
bool PLUGIN_API VstSilenceFlagsTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult || !audioEffect)
		return false;
	if (!canProcessSampleSize (testResult))
		return true;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	if (processData.inputs != 0)
	{
		audioEffect->setProcessing (true);
	
		for (int32 inputsIndex = 0; inputsIndex < processData.numInputs; inputsIndex++)
		{
			int32 numSilenceFlagsCombinations = (1 << processData.inputs[inputsIndex].numChannels) - 1;
			for (int32 flagCombination = 0; flagCombination <= numSilenceFlagsCombinations; flagCombination++)
			{
				processData.inputs[inputsIndex].silenceFlags = flagCombination;
				tresult result = audioEffect->process (processData);
				if (result != kResultOk)
				{
					char text[256];
					sprintf (text, "The component failed to process bus %i with silence flag combination %x!", inputsIndex, flagCombination);
					testResult->addErrorMessage (text);
					audioEffect->setProcessing (false);
					return false;
				}
			}
		}
	}
	else if (processData.numInputs > 0)
	{
		testResult->addErrorMessage ("ProcessData::inputs are 0 but ProcessData::numInputs are nonzero.");
		return false;
	}
	
	audioEffect->setProcessing (false);

	return true;
}

//------------------------------------------------------------------------
// VstVariableBlockSizeTest
//------------------------------------------------------------------------
VstVariableBlockSizeTest::VstVariableBlockSizeTest (IPlugProvider* plugProvider, ProcessSampleSize sampl)
: VstProcessTest (plugProvider, sampl)
{
}

//------------------------------------------------------------------------
bool PLUGIN_API VstVariableBlockSizeTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult || !audioEffect)
		return false;
	if (!canProcessSampleSize (testResult))
		return true;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	audioEffect->setProcessing (true);

	for (int32 i = 0; i <= NUM_ITERATIONS; i++)
	{
		int32 sampleFrames = rand () % processSetup.maxSamplesPerBlock;
		processData.numSamples = sampleFrames;
		if (i == 0)
			processData.numSamples = 0;
		#if TOUGHTESTS
		else if (i == 1)
			processData.numSamples = -50000;
		else if (i == 2)
			processData.numSamples = processSetup.maxSamplesPerBlock * 2;
		#endif
		tresult result = audioEffect->process (processData);
		if ((result != kResultOk) 
			#if TOUGHTESTS
			&& (i > 1)
			#else
			&& (i > 0)
			#endif
			)
		{
			char text[256];
			sprintf (text, "The component failed to process an audioblock of size %i", sampleFrames);
			testResult->addErrorMessage (text);
			
			audioEffect->setProcessing (false);
			return false;
		}
	}
	
	audioEffect->setProcessing (false);

	return true;
}


//------------------------------------------------------------------------
// VstProcessFormatTest
//------------------------------------------------------------------------
VstProcessFormatTest::VstProcessFormatTest (IPlugProvider* plugProvider, ProcessSampleSize sampl)
: VstProcessTest (plugProvider, sampl)
{}

//------------------------------------------------------------------------
bool PLUGIN_API VstProcessFormatTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult || !audioEffect)
		return false;
	if (!canProcessSampleSize (testResult))
		return true;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	int32 numFails = 0;
	const int32 numRates = 11;
	SampleRate sampleRateFormats [numRates] = {22050., 32000., 44100., 48000., 88200., 96000., 192000.,
		1234.5678, 12345.678, 123456.78, 1234567.8};
  	
	tresult result = vstPlug->setActive (false);
	if (result != kResultOk)
	{
		testResult->addErrorMessage ("IComponent::setActive (false) failed.");
		return false;
	}

	sprintf (text, "***Tested Sample Rates***");
	testResult->addMessage (text);

	for (int32 i = 0; i < numRates; i++)
	{
		processSetup.sampleRate = sampleRateFormats [i];
		result = audioEffect->setupProcessing (processSetup);
		if (result == kResultOk)
		{
			result = vstPlug->setActive (true);
			if (result != kResultOk)
			{
				testResult->addErrorMessage ("IComponent::setActive (true) failed.");
				return false;
			}
		
			audioEffect->setProcessing (true);
			result = audioEffect->process (processData);
			audioEffect->setProcessing (false);

			if (result == kResultOk)
			{
				sprintf (text, " %10.10G Hz - processed successfully!", sampleRateFormats [i]);
				testResult->addMessage (text);
			}
			else
			{
				sprintf (text, " %10.10G Hz - failed to process!", sampleRateFormats [i]);
				numFails++;
				testResult->addErrorMessage (text);
			}

			result = vstPlug->setActive (false);
			if (result != kResultOk)
			{
				testResult->addErrorMessage ("IComponent::setActive (false) failed.");
				return false;
			}
		}
		else if (sampleRateFormats [i] > 0.)
		{
			testResult->addErrorMessage ("IAudioProcessor::setupProcessing (..) failed.");
			return false;
		}
	}

	result = vstPlug->setActive (true);
	if (result != kResultOk)
		return false;

	return true;
}

//------------------------------------------------------------------------
// VstBusActivationTest
//------------------------------------------------------------------------
VstBusActivationTest::VstBusActivationTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{}

//------------------------------------------------------------------------
bool PLUGIN_API VstBusActivationTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	int32 numTotalBusses = 0;
	int32 numFailedActivations = 0;

	for (MediaType type = kAudio; type < kNumMediaTypes; type++)
	{
		int32 numInputs = vstPlug->getBusCount (type, kInput);
		int32 numOutputs = vstPlug->getBusCount (type, kOutput);

		numTotalBusses += (numInputs + numOutputs);

		for (int32 i = 0; i < numInputs + numOutputs; i++)
		{
			BusDirection busDirection = i < numInputs ? kInput : kOutput;
			int32 busIndex = busDirection == kInput ? i : i - numInputs;
			
			BusInfo busInfo = {0};
			if (vstPlug->getBusInfo (type, busDirection, busIndex, busInfo) != kResultTrue)
			{
				testResult->addErrorMessage ("IComponent::getBusInfo (..) failed.");
				return false;
			}

			sprintf (text, "Bus Activation: %s %s Bus (%d) (%s)", 
				busDirection == kInput ? "Input" : "Output", 
				type == kAudio ? "Audio" : "Event", busIndex,
				busInfo.busType == kMain ? "kMain" : "kAux");
			testResult->addMessage (text);

			if ((busInfo.flags & BusInfo::kDefaultActive) == false)
			{
				if (vstPlug->activateBus (type, busDirection, busIndex, true) != kResultOk)
					numFailedActivations++;
				if (vstPlug->activateBus (type, busDirection, busIndex, false) != kResultOk)
					numFailedActivations++;
			}
			else if ((busInfo.flags & BusInfo::kDefaultActive) == true)
			{
				if (vstPlug->activateBus (type, busDirection, busIndex, false) != kResultOk)
					numFailedActivations++;
				if (vstPlug->activateBus (type, busDirection, busIndex, true) != kResultOk)
					numFailedActivations++;
			}
		}
	}

	if (numFailedActivations > 0)
		testResult->addErrorMessage ("Bus activation failed!");

	return (numFailedActivations == 0);
}


//------------------------------------------------------------------------
// VstCheckAudioBusArrangementTest
//------------------------------------------------------------------------
VstCheckAudioBusArrangementTest::VstCheckAudioBusArrangementTest (IPlugProvider* plugProvider)
: VstTestBase (plugProvider)
{
}

//------------------------------------------------------------------------
bool VstCheckAudioBusArrangementTest::run (ITestResult* testResult)
{
	if (!vstPlug || !testResult)
		return false;

	char text[256];
	sprintf (text, "===%s ====================================", getName ());
	testResult->addMessage (text);

	int32 numInputs = vstPlug->getBusCount (kAudio, kInput);
	int32 numOutputs = vstPlug->getBusCount (kAudio, kOutput);
	int32 arrangementMismatchs = 0;
	
	FUnknownPtr <IAudioProcessor> audioEffect (vstPlug);
	if (audioEffect)
	{
		for (int32 i = 0; i < numInputs + numOutputs; i++)
		{
			BusDirection dir = i < numInputs ? kInput : kOutput;
			int32 busIndex = dir == kInput ? i : i - numInputs;
			
			sprintf (text, "Check %s Audio Bus Arrangement (%d)", dir == kInput ? "Input" : "Output", busIndex);
			testResult->addMessage (text);

			BusInfo busInfo = {0};
			if (vstPlug->getBusInfo (kAudio, dir, busIndex, busInfo) == kResultTrue)
			{
				SpeakerArrangement arrangement;
				if (audioEffect->getBusArrangement (dir, busIndex, arrangement) == kResultTrue)
				{
					if (busInfo.channelCount != SpeakerArr::getChannelCount (arrangement))
					{
						arrangementMismatchs++;
						testResult->addErrorMessage ("channelCount is inconsistent!");
					}
				}
				else
				{
					testResult->addErrorMessage ("IAudioProcessor::getBusArrangement (..) failed!");
					return false;
				}
			}
			else
			{
				testResult->addErrorMessage ("IComponent::getBusInfo (..) failed!");
				return false;
			}
		}
	}
	return (arrangementMismatchs == 0);
}
