#include <string.h>
#include "OfflineEffect.hpp"
#include "OfflineEditor.hpp"
#include <math.h>
#define MAKE_ID(a,b,c,d)    ((d) | ((c) << 8) | ((b) << 16) | ((a) << 24))

//-----------------------------------------------------------------------------

OfflineProgram::OfflineProgram()
{
	fType = k_ofl_minus6dB;
	strcpy(name, "Init");
}

//-----------------------------------------------------------------------------

OfflineEffect::OfflineEffect(audioMasterCallback audioMaster)
	:
	AudioEffectX(audioMaster, 5, kNumParams)
{
	programs = new OfflineProgram[numPrograms];
	setProgram(0);
	setUniqueID(MAKE_ID('O','f','f','l'));

	offlineEditor = new OfflineEditor(this);
	editor = offlineEditor;
	setEditor(editor);
}

//-----------------------------------------------------------------------------

OfflineEffect::~OfflineEffect()
{
	delete[] programs;
}

//-----------------------------------------------------------------------------

void OfflineEffect::setProgram(long program)
{
	if (program < numPrograms)
	{
		programs[curProgram] = m_currProgram;
		curProgram = program;
		m_currProgram = programs[curProgram];
		if(editor)
			editor->postUpdate();
	}
}

//-----------------------------------------------------------------------------

void OfflineEffect::setProgramName(char *name)
{
	strcpy(m_currProgram.name, name);
}

//-----------------------------------------------------------------------------

void OfflineEffect::getProgramName(char *name)
{
	strcpy(name, m_currProgram.name);
}

//-----------------------------------------------------------------------------

void OfflineEffect::setParameter(long index, float value)
{
	m_currProgram.fType = value;
	if(editor)
		editor->postUpdate();
}

//-----------------------------------------------------------------------------

float OfflineEffect::getParameter(long index)
{
	float v = 0;
	switch(index)
	{
	case kType:
		v = m_currProgram.fType;
		break;
	}
	return v;
}

//-----------------------------------------------------------------------------

void OfflineEffect::getParameterName(long index, char *label)
{
	switch(index)
	{
	case kType:
		strcpy(label, "Type");
		break;
	}
}

//-----------------------------------------------------------------------------

void OfflineEffect::getParameterDisplay(long index, char *text)
{
	switch(index)
	{
	case kType:
		float2string(m_currProgram.fType, text);
		break;
	}
}

//-----------------------------------------------------------------------------

void OfflineEffect::getParameterLabel(long index, char *label)
{
	switch(index)
	{
	case kType:
		strcpy(label, " Type ");
		break;
	}
}

// -----------------------------------------------------------------------------

// This is a "pure offline" plugin, hence...

long OfflineEffect::canDo(char* function)
{
	if (stricmp(function, "offline") == 0)
		return 1;
	if (stricmp(function, "noRealTime") == 0)
		return 1;
	return AudioEffectX::canDo(function);
}

// -----------------------------------------------------------------------------

// called by the host:
// 1) whenever any parameter of VstAudioFile gets changed ("startProcess" false)
// 2) user presses the "process" button ("startProcess" true)
// The plugin should _not_ do any internal initialization at this stage

bool OfflineEffect::offlineNotify(VstAudioFile* files, long numAudioFiles, bool startProcess)
{
	if (startProcess)
	{
		switch ((int)m_currProgram.fType)
		{
		case k_ofl_minus6dB:
		{
			if (numAudioFiles)
			{
				VstAudioFile* file = &files[0];
				file->flags |= kVstOfflineCanProcessSelection;
				file->flags |= kVstOfflineWantRead;
				file->flags |= kVstOfflineWantWrite;
				return offlineStart(files, numAudioFiles, 0);
			}
			break;
		}
		case k_ofl_reverse:
		{
			if (numAudioFiles)
			{
				VstAudioFile* file = &files[0];
				file->flags |= kVstOfflineCanProcessSelection;
				file->flags |= kVstOfflineWantRead;
				file->flags |= kVstOfflineWantWrite;
				return offlineStart(files, numAudioFiles, 0);
			}
			break;
		}
		case k_ofl_reverse2:
		{
			if (numAudioFiles)
			{
				VstAudioFile* file = &files[0];
				file->flags |= kVstOfflineCanProcessSelection;
				file->flags |= kVstOfflineWantRead;
				file->flags |= kVstOfflineWantWrite;
				return offlineStart(files, numAudioFiles, 0);
			}
			break;
		}
		case k_ofl_maximize:
		{
			if (numAudioFiles)
			{
				VstAudioFile* file = &files[0];
				file->flags |= kVstOfflineCanProcessSelection;
				file->flags |= kVstOfflineWantRead;
				file->flags |= kVstOfflineWantWrite;
				return offlineStart(files, numAudioFiles, 0);
			}
			break;
		}
		case k_ofl_reverse_maximize:
		{
			if (numAudioFiles)
			{
				VstAudioFile* file = &files[0];
				file->flags |= kVstOfflineCanProcessSelection;
				file->flags |= kVstOfflineWantRead;
				file->flags |= kVstOfflineWantWrite;
				return offlineStart(files, numAudioFiles, 0);
			}
			break;
		}
		case k_ofl_mix:
		{
			if (numAudioFiles >= 2)
			{
				if (files[0].sampleRate == files[1].sampleRate)
				{
					if (files[1].numChannels == files[1].numChannels)
					{
						VstAudioFile* file = &files[0];
						file->flags |= kVstOfflineCanProcessSelection;
						file->flags |= kVstOfflineWantRead;

						file = &files[1];
						file->flags |= kVstOfflineCanProcessSelection;
						file->flags |= kVstOfflineWantRead;
						return offlineStart(files, numAudioFiles, 1);
					}
				}
			}
			break;
		}
		case k_ofl_cursorMarkerSelection:
		{
			if (numAudioFiles)
			{
				VstAudioFile* file = &files[0];
				file->flags |= kVstOfflineWantWriteMarker;
				file->flags |= kVstOfflineWantMoveCursor;
				file->flags |= kVstOfflineWantSelect;
				return offlineStart(files, numAudioFiles, 0);
			}
			break;
		}
		case k_ofl_parameterRecording:
		{
			if (numAudioFiles)
			{
				VstAudioFile* file = &files[0];
				file->flags |= kVstOfflineWantRead;
				return offlineStart(files, numAudioFiles, 0);
			}
			break;
		}
		case k_ofl_oneEcho:
		{
			if (numAudioFiles)
			{
				VstAudioFile* file = &files[0];
				file->flags |= kVstOfflineCanProcessSelection;
				file->flags |= kVstOfflineWantRead;
				file->flags |= kVstOfflineWantWrite;
				return offlineStart(files, numAudioFiles, 0);
			}
			break;
		}
		case k_ofl_halfPitchShift:
		{
			if (numAudioFiles)
			{
				VstAudioFile* file = &files[0];
				file->flags |= kVstOfflineCanProcessSelection;
				file->flags |= kVstOfflineWantRead;
				file->flags |= kVstOfflineWantWrite;
				return offlineStart(files, numAudioFiles, 0);
			}
			break;
		}
		case k_ofl_interleavedSwap:
		{
			if (numAudioFiles)
			{
				VstAudioFile* file = &files[0];
				file->flags |= kVstOfflineCanProcessSelection;
				file->flags |= kVstOfflineWantRead;
				file->flags |= kVstOfflineWantWrite;
				return offlineStart(files, numAudioFiles, 0);
			}
			break;
		}
		case k_ofl_useOfTempFile:
		{
			if (numAudioFiles)
			{
				VstAudioFile* file = &files[0];
				file->flags |= kVstOfflineCanProcessSelection;
				file->flags |= kVstOfflineWantRead;
				file->flags |= kVstOfflineWantWrite;
				return offlineStart(files, numAudioFiles, 1);
			}
			break;
		}
		case k_ofl_copyFile:
		{
			if (numAudioFiles)
			{
				VstAudioFile* file = &files[0];
				file->flags |= kVstOfflineWantRead;
				return offlineStart(files, numAudioFiles, 1);
			}
			break;
		}
		case k_ofl_sineGenerator:
		{
			return offlineStart(files, numAudioFiles, 1);
		}
		break;
		}
	}
	else
	{
		offlineEditor->OnHostChange(files, numAudioFiles);
		if (numAudioFiles)
		{
			switch ((int)m_currProgram.fType)
			{
			case k_ofl_mix:
				return (numAudioFiles >= 2);
			default:
				return true;
			}
		}
		else
		{
			if ((int)m_currProgram.fType == k_ofl_sineGenerator)
				return true;
		}
	}
	return false;
}

// -----------------------------------------------------------------------------

// called by the host so that we initialize the VstOfflineTask array
// (in many cases, only one element in the array (count == 1))
// The plugin should _not_ do any internal initialization at this stage

bool OfflineEffect::offlinePrepare(VstOfflineTask* offline, long count)
{
	if (count == 0)
		return false;
	switch ((int)m_currProgram.fType)
	{
	case k_ofl_minus6dB:
		strncpy(offline->processName, "Minus 6dB", sizeof(offline->processName) - 1);
		return true;
	case k_ofl_reverse:
		strncpy(offline->processName, "Reverse", sizeof(offline->processName) - 1);
		offline->flags |= kVstOfflineRandomWrite;
		return true;
	case k_ofl_reverse2:
		strncpy(offline->processName, "Reverse-2", sizeof(offline->processName) - 1);
		offline->flags |= kVstOfflineRandomWrite;
		return true;
	case k_ofl_maximize:
		strncpy(offline->processName, "MaximizeWindow", sizeof(offline->processName) - 1);
		return true;
	case k_ofl_reverse_maximize:
		strncpy(offline->processName, "Reverse then MaximizeWindow", sizeof(offline->processName) - 1);
		offline->flags |= kVstOfflineRandomWrite;
		return true;
	case k_ofl_mix:
		strncpy(offline->processName, "Mix", sizeof(offline->processName) - 1);
		offline[2].destinationSampleRate = offline[2].sourceSampleRate = offline[1].sourceSampleRate;
		offline[2].numDestinationChannels = offline[2].numSourceChannels = offline[1].numSourceChannels;
		return true;
	case k_ofl_cursorMarkerSelection:
		strncpy(offline->processName, "Set Cursor / Marker / Selection", sizeof(offline->processName) - 1);
		offline->flags |= kVstOfflineNoThread;	// because quick process
		return true;
	case k_ofl_parameterRecording:
		strncpy(offline->processName, "Parameter recording (invisible)", sizeof(offline->processName) - 1);
		return true;
	case k_ofl_oneEcho:
		strncpy(offline->processName, "One Echo with tail", sizeof(offline->processName) - 1);
		return true;
	case k_ofl_halfPitchShift:
		strncpy(offline->processName, "Half Pitch Shift", sizeof(offline->processName) - 1);
		offline->flags |= kVstOfflineStretch;
		return true;
	case k_ofl_interleavedSwap:
		strncpy(offline->processName, "Swap channels (Interleaved)", sizeof(offline->processName) - 1);
		offline->flags |= kVstOfflineInterleavedAudio;
		if (offline->numSourceChannels == 2)
			return true;
		else
		{	// example of an error message that will be reported by the host
			offline->flags |= kVstOfflinePlugError;
			strcpy(offline->outputText, "Needs a stereo file!");
			break;
		}
	case k_ofl_useOfTempFile:
		strncpy(offline->processName, "Use of temp file", sizeof(offline->processName) - 1);
		offline[1].flags |= kVstOfflineTempOutputFile;
		offline[1].flags |= kVstOfflineFloatOutputFile;
		// make sure to initialize this, since it's a new file! (host can't know)
		offline[1].numDestinationChannels = offline[1].numSourceChannels = offline[0].numSourceChannels;
		offline[1].destinationSampleRate = offline[1].sourceSampleRate = offline[0].sourceSampleRate;
		return true;
	case k_ofl_copyFile:
		strncpy(offline->processName, "Copy file", sizeof(offline->processName) - 1);
		// make sure to initialize this, since it's a new file! (host can't know)
		offline[1].flags |= kVstOfflineFloatOutputFile;
		offline[1].numDestinationChannels = offline[1].numSourceChannels = offline[0].numSourceChannels;
		offline[1].destinationSampleRate = offline[1].sourceSampleRate = offline[0].sourceSampleRate;
		offline[1].index = 0;	// hint for the host: generating from file source #0
		//strcpy(offline[1].outputFileName, "c:\\test.wav");
		return true;
	case k_ofl_sineGenerator:
		strncpy(offline->processName, "Sine generator", sizeof(offline->processName) - 1);
		// make sure to initialize this, since it's a new file! (host can't know)
		offline[0].numDestinationChannels = offline[0].numSourceChannels = 1;
		offline[0].destinationSampleRate = offline[0].sourceSampleRate = 44100;
		return true;
	}
	return false;
}

// -----------------------------------------------------------------------------

// called by the host to effectively execute the process

bool OfflineEffect::offlineRun(VstOfflineTask* offline, long count)
{
	switch ((int)m_currProgram.fType)
	{
	case k_ofl_minus6dB:
		return Minus6dB(offline);
	case k_ofl_reverse:
		return Reverse(offline);
	case k_ofl_reverse2:
		return Reverse2(offline);
	case k_ofl_maximize:
		return Maximize(offline, false);
	case k_ofl_reverse_maximize:
		return ReverseThenMaximize(offline);
	case k_ofl_mix:
		return Mix(offline, count);
	case k_ofl_cursorMarkerSelection:
		return CursorMarkerSelection(offline);
	case k_ofl_parameterRecording:
		return ParameterRecording(offline);
	case k_ofl_oneEcho:
		return OneEcho(offline);
	case k_ofl_halfPitchShift:
		return HalfPitchShift(offline);
	case k_ofl_interleavedSwap:
		return InterleavedSwap(offline);
	case k_ofl_useOfTempFile:
		return UseOfTempFile(offline);
	case k_ofl_copyFile:
		return CopyFile(offline);
	case k_ofl_sineGenerator:
		return SineGenerator(offline);
	}
	return false;
}

// -----------------------------------------------------------------------------

// Basic example: reads a file, reduce the gain, write back the results in place

bool OfflineEffect::Minus6dB(VstOfflineTask* offline)
{
	offline->readPosition = offline->positionToProcessFrom;
	double remainingFramesToProcess = offline->numFramesToProcess;
	strcpy(offline->progressText, "Minus 6dB");
	offline->writePosition = 0;

	while (remainingFramesToProcess > 0)
	{
		offline->progress = 1 - (remainingFramesToProcess / offline->numFramesToProcess);

		// Read samples
		offline->readCount = offline->sizeInputBuffer;
		if (remainingFramesToProcess < offline->readCount)
			offline->readCount = (long)remainingFramesToProcess;
		if (! offlineRead(offline, kVstOfflineAudio))
			return false;

		// Transform samples
		for (int iChannel = 0; iChannel < offline->numSourceChannels; iChannel++)
		{
			const float* inputSamples = ((const float**)offline->inputBuffer)[iChannel];
			float* outputSamples = ((float**)offline->outputBuffer)[iChannel];
			for (int iSample = 0; iSample < offline->readCount; iSample++)
			{
				*outputSamples = *inputSamples / 2;
				inputSamples++;
				outputSamples++;
			}
		}

		// Write samples
		offline->writeCount = offline->readCount;
		if (! offlineWrite(offline, kVstOfflineAudio))
			return false;

		remainingFramesToProcess -= offline->readCount;
	}
	return true;
}

// -----------------------------------------------------------------------------

// Reverse the file by reading backwards

bool OfflineEffect::Reverse(VstOfflineTask* offline)
{
	double topReadPosition = offline->positionToProcessFrom + offline->numFramesToProcess;
	double remainingFramesToProcess = offline->numFramesToProcess;
	strcpy(offline->progressText, "Reversing");
	offline->writePosition = 0;

	while (remainingFramesToProcess > 0)
	{
		offline->progress = 1 - (remainingFramesToProcess / offline->numFramesToProcess);

		// Read samples from top to bottom
		offline->readCount = offline->sizeInputBuffer;
		if (remainingFramesToProcess < offline->readCount)
			offline->readCount = (long)remainingFramesToProcess;

		topReadPosition -= offline->readCount;
		offline->readPosition = topReadPosition;
		if (! offlineRead(offline, kVstOfflineAudio))
			return false;

		// Transform samples
		for (int iChannel = 0; iChannel < offline->numSourceChannels; iChannel++)
		{
			const float* inputSamples = ((const float**)offline->inputBuffer)[iChannel];
			inputSamples += offline->readCount;	// read from top of buffer
			float* outputSamples = ((float**)offline->outputBuffer)[iChannel];
			for (int iSample = 0; iSample < offline->readCount; iSample++)
			{
				inputSamples--;
				*outputSamples = *inputSamples;
				outputSamples++;
			}
		}

		// Write samples
		offline->writeCount = offline->readCount;
		if (! offlineWrite(offline, kVstOfflineAudio))
			return false;

		remainingFramesToProcess -= offline->readCount;
	}
	return true;
}

// -----------------------------------------------------------------------------

// Reverse the file by writing backwards. This is not an optimized way, this is just
// to show how to do it.

bool OfflineEffect::Reverse2(VstOfflineTask* offline)
{
	double topReadPosition = offline->positionToProcessFrom + offline->numFramesToProcess;
	double remainingFramesToProcess = offline->numFramesToProcess;
	offline->readPosition = offline->positionToProcessFrom;

	while (remainingFramesToProcess > 0)
	{
		offline->progress = 1 - (remainingFramesToProcess / offline->numFramesToProcess);

		// Read samples from top to bottom
		offline->readCount = offline->sizeInputBuffer;
		if (remainingFramesToProcess < offline->readCount)
			offline->readCount = (long)remainingFramesToProcess;
		if (! offlineRead(offline, kVstOfflineAudio))
			return false;

		// Transform samples
		for (int iChannel = 0; iChannel < offline->numSourceChannels; iChannel++)
		{
			const float* inputSamples = ((const float**)offline->inputBuffer)[iChannel];
			inputSamples += offline->readCount;	// read from top of buffer
			float* outputSamples = ((float**)offline->outputBuffer)[iChannel];
			for (int iSample = 0; iSample < offline->readCount; iSample++)
			{
				inputSamples--;
				*outputSamples = *inputSamples;
				outputSamples++;
			}
		}

		// Write samples from top to bottom
		offline->writeCount = offline->readCount;
		offline->writePosition = remainingFramesToProcess - offline->writeCount;
		if (! offlineWrite(offline, kVstOfflineAudio))
			return false;

		remainingFramesToProcess -= offline->readCount;
	}
	return true;
}

// -----------------------------------------------------------------------------

// Process with two passes:
// 1) read the samples to find the peak
// 2) read and amplify the samples, then write them in place

bool OfflineEffect::Maximize(VstOfflineTask* offline, bool output)
{
	//
	// Find peak
	//
	float peak = 0;
	double remainingFramesToProcess = offline->numFramesToProcess;
	offline->readPosition = output ? 0 : offline->positionToProcessFrom;
	strcpy(offline->progressText, "Finding peak");

	while (remainingFramesToProcess > 0)
	{
		offline->progress = 1 - (remainingFramesToProcess / offline->numFramesToProcess);

		// Read samples
		offline->readCount = offline->sizeInputBuffer;
		if (remainingFramesToProcess < offline->readCount)
			offline->readCount = (long)remainingFramesToProcess;
		if (! offlineRead(offline, kVstOfflineAudio, ! output))
			return false;

		// Find peak in block
		for (int iChannel = 0; iChannel < offline->numSourceChannels; iChannel++)
		{
			const float* inputSamples = ((const float**)offline->inputBuffer)[iChannel];
			for (int iSample = 0; iSample < offline->readCount; iSample++)
			{
				if (*inputSamples > peak)
					peak = *inputSamples;
				inputSamples++;
			}
		}

		remainingFramesToProcess -= offline->readCount;
	}

	if (peak == 0.0f)
		return true;
	float factor = 1 / peak;
	//
	// MaximizeWindow
	//
	remainingFramesToProcess = offline->numFramesToProcess;
	offline->readPosition = output ? 0 : offline->positionToProcessFrom;
	offline->writePosition = 0;
	strcpy(offline->progressText, "Maximizing");

	while (remainingFramesToProcess > 0)
	{
		offline->progress = 1 - (remainingFramesToProcess / offline->numFramesToProcess);

		// Read samples
		offline->readCount = offline->sizeInputBuffer;
		if (remainingFramesToProcess < offline->readCount)
			offline->readCount = (long)remainingFramesToProcess;
		if (! offlineRead(offline, kVstOfflineAudio, ! output))
			return false;

		// Transform samples
		for (int iChannel = 0; iChannel < offline->numSourceChannels; iChannel++)
		{
			const float* inputSamples = ((const float**)offline->inputBuffer)[iChannel];
			float* outputSamples = ((float**)offline->outputBuffer)[iChannel];
			for (int iSample = 0; iSample < offline->readCount; iSample++)
			{
				*outputSamples = *inputSamples * factor;
				inputSamples++;
				outputSamples++;
			}
		}

		// Write samples
		offline->writeCount = offline->readCount;
		if (! offlineWrite(offline, kVstOfflineAudio))
			return false;

		remainingFramesToProcess -= offline->readCount;
	}
	return true;
}

// -----------------------------------------------------------------------------

// Reverse the file in place, then maximize the result.
// In this example, the maximize process (read + write) is performed
// on the file already created by the reverse process

bool OfflineEffect::ReverseThenMaximize(VstOfflineTask* offline)
{
	bool ok = Reverse(offline);
	if (ok)
	{
		ok = Maximize(offline, true);
	}
	return ok;
}

// -----------------------------------------------------------------------------

// read two files and create a new file out of them

bool OfflineEffect::Mix(VstOfflineTask* offline, long count)
{
	VstOfflineTask* inputFile1 = &offline[0];
	VstOfflineTask* inputFile2 = &offline[1];
	VstOfflineTask* outputFile = &offline[2];

	inputFile1->readPosition = inputFile1->positionToProcessFrom;
	inputFile2->readPosition = inputFile2->positionToProcessFrom;
	double remainingFramesToProcess1 = inputFile1->numFramesToProcess;
	double remainingFramesToProcess2 = inputFile2->numFramesToProcess;
	strcpy(offline->progressText, "Mixing");

	while (remainingFramesToProcess1 || remainingFramesToProcess2)
	{
		double progress;
		if (remainingFramesToProcess1 > remainingFramesToProcess2)
			progress = 1 - (remainingFramesToProcess1 / inputFile1->numFramesToProcess);
		else
			progress = 1 - (remainingFramesToProcess2 / inputFile2->numFramesToProcess);
		inputFile1->progress = progress;
		inputFile2->progress = -1;

		// Clear input buffers before reading (in case buffers won't get full)
		int iChannel;
		for (iChannel = 0; iChannel < inputFile1->numSourceChannels; iChannel++)
		{
			float* inputSamples = ((float**)inputFile1->inputBuffer)[iChannel];
			memset(inputSamples, 0, inputFile1->sizeInputBuffer * sizeof(float));
			inputSamples = ((float**)inputFile2->inputBuffer)[iChannel];
			memset(inputSamples, 0, inputFile2->sizeInputBuffer * sizeof(float));
		}

		// Read samples from file 1
		inputFile1->readCount = inputFile1->sizeInputBuffer;
		if (remainingFramesToProcess1 < inputFile1->readCount)
			inputFile1->readCount = (long)remainingFramesToProcess1;
		if (inputFile1->readCount)
			if (! offlineRead(inputFile1, kVstOfflineAudio))
				return false;
		remainingFramesToProcess1 -= inputFile1->readCount;

		// Read samples from file 2
		inputFile2->readCount = inputFile2->sizeInputBuffer;
		if (remainingFramesToProcess2 < inputFile2->readCount)
			inputFile2->readCount = (long)remainingFramesToProcess2;
		if (inputFile2->readCount)
			if (! offlineRead(inputFile2, kVstOfflineAudio))
				return false;
		remainingFramesToProcess2 -= inputFile2->readCount;

		// Mix samples
		int nFrames = inputFile1->readCount;
		if (inputFile2->readCount > nFrames)
			nFrames = inputFile2->readCount;
		for (iChannel = 0; iChannel < inputFile1->numSourceChannels; iChannel++)
		{
			const float* inputSamples1 = ((const float**)inputFile1->inputBuffer)[iChannel];
			const float* inputSamples2 = ((const float**)inputFile2->inputBuffer)[iChannel];
			float* outputSamples = ((float**)outputFile->outputBuffer)[iChannel];
			for (int iSample = 0; iSample < nFrames; iSample++)
			{
				*outputSamples = *inputSamples1 + *inputSamples2;
				inputSamples1++;
				inputSamples2++;
				outputSamples++;
			}
		}

		// Write samples
		outputFile->writeCount = nFrames;
		if (! offlineWrite(outputFile, kVstOfflineAudio))
			return false;
	}
	return true;
}

// -----------------------------------------------------------------------------

// Change the edit-cursor position
// Change the selection
// Create or move some markers
// Everytime the function is called, the result is different

bool OfflineEffect::CursorMarkerSelection(VstOfflineTask* offline)
{
	// To make the example instructive, perform various operations

	//
	// marker handling
	//
	bool createNewMarker = false;
	// read how many markers
	offline->extraBuffer = 0;
	if (offlineRead(offline, kVstOfflineMarker))	// else no marker implementation in the host
	{
		long nMarkers = offline->readCount;
		if (nMarkers >= 2)	// delete all of them
		{	// retrieve the markers
			VstAudioFileMarker* markers = new VstAudioFileMarker[nMarkers];	// allocate temp array
			offline->readCount = nMarkers;
			offline->extraBuffer = markers;
			offlineRead(offline, kVstOfflineMarker);
			// mark them to be deleted
			for (int i = 0; i < offline->readCount; i++)
				markers[i].position = -1;
			// request the host to delete them
			offline->writeCount = nMarkers;
			offlineWrite(offline, kVstOfflineMarker);
			// release temp array
			delete [] markers;
		}
		else if (nMarkers == 1)	// move it
		{
			VstAudioFileMarker marker;
			offline->extraBuffer = &marker;
			offline->readCount = 1;
			offlineRead(offline, kVstOfflineMarker);
			// request the host to move the marker
			if (marker.position != 44100)
			{
				marker.position = 44100;	// near the end
				offline->writeCount = 1;
				offlineWrite(offline, kVstOfflineMarker);
			}
			else
				createNewMarker = true;
		}
		else
			createNewMarker = true;

		if (createNewMarker)
		{	// create one marker
			VstAudioFileMarker marker;
			strcpy(marker.name, "Test");
			marker.position = 44100 / 2;
			marker.type = 1;
			marker.id = 0;	// 0 for a new marker!
			marker.reserved = 0;
			offline->extraBuffer = &marker;
			offline->writeCount = 1;
			offlineWrite(offline, kVstOfflineMarker);
		}
	}
	
	//
	// cursor handling
	//
	int channelMask = -1;
	if (offlineRead(offline, kVstOfflineCursor))	// read current position
	{
		offline->writePosition = offline->readPosition + 44100;
		if (offline->writePosition >= offline->numFramesInSourceFile)
			offline->writePosition = 0;	// wrap around
		if (offline->numSourceChannels == 2)
		{	// change the selection channels
			if ((offline->index & 3) == 3)
				channelMask  = 1;
			else if ((offline->index & 3) == 1)
				channelMask  = 2;
			else
				channelMask  = 3;
		}
		else
			channelMask = -1;	// select all channels
		offline->index = channelMask;
		offlineWrite(offline, kVstOfflineCursor);
	}
	//
	// selection handling
	//
	if (offlineRead(offline, kVstOfflineSelection))
	{
		offline->positionToProcessFrom += 44100;
		offline->numFramesToProcess = 44100;	// select 1 second
		if ((offline->positionToProcessFrom + offline->numFramesToProcess) >= offline->numFramesInSourceFile)
			offline->positionToProcessFrom = 0;	// wrap around
		offline->index = channelMask;
		offlineWrite(offline, kVstOfflineSelection);
	}
	return true;
}

// -----------------------------------------------------------------------------

// In this example, we write some dummy parameters, and read them back later

bool OfflineEffect::ParameterRecording(VstOfflineTask* offline)
{
	for (int iPass = 0; iPass < 2; iPass++)
	{	// one pass to record parameters, the other pass to read them back
		bool recordParameters = (iPass == 0);

		// we also read the file
		offline->readPosition = offline->positionToProcessFrom;
		double remainingFramesToProcess = offline->numFramesToProcess;
		strcpy(offline->progressText, "Parameter recording");

		long count = 0;
		long i = 0;
		long gap = (long)(remainingFramesToProcess / 100);	// record about 100 parameters

		double nextParameterPosition = -1;
		long nextParameterIndex = -1;
		long nextByteArraySize = 0;

		while (remainingFramesToProcess > 0)
		{
			offline->progress = 1 - (remainingFramesToProcess / offline->numFramesToProcess);

			// Read samples
			offline->readCount = offline->sizeInputBuffer;
			if (remainingFramesToProcess < offline->readCount)
				offline->readCount = (long)remainingFramesToProcess;
			if (! offlineRead(offline, kVstOfflineAudio))
				return false;

			// eg. would do any kind of analysis here
			// then record some parameters
			if (count > gap)
			{
				double position = offline->readPosition;
				count = 0;	// reset
				if (offline->readCount >= (16/sizeof(float)))	// just required for this dummy example
				{
					VstOfflineTask offlineParam = *offline;
					const float* inputSamples = ((const float**)offline->inputBuffer)[0];
					if (recordParameters)
					{
						if (i == 0)
						{
							strcpy(offlineParam.processName, "test");
							offlineParam.value = 12345;
						}
						if (i & 1)
						{	// write byte array
							offlineParam.index = 1;
							offlineParam.writeCount = 16;
							offlineParam.writePosition = position;
							offlineParam.extraBuffer = (void*)inputSamples;	// dummy test: record samples rather than actual parameters :-)
							offlineWrite(&offlineParam, kVstOfflineParameter);
						}
						else
						{	// write float value
							offlineParam.index = 0;
							offlineParam.writeCount = 0;
							offlineParam.writePosition = position;
							offlineParam.extraBuffer = (void*)inputSamples;	// dummy test: record samples rather than actual parameters :-)
							offlineWrite(&offlineParam, kVstOfflineParameter);
						}
					}
					else	// read and check
					{
						bool successfullCheck = false;
						if (i == 0)	// first time?
						{	// inquire about first parameter
							offlineParam.readPosition = 0;	// inquire at that very first position
							offlineParam.extraBuffer = 0;	// inquire mode
							if (offlineRead(&offlineParam, kVstOfflineParameter))
							{
								if (strcmp(offlineParam.processName, "test") == 0)
								{
									if (offlineParam.value == 12345)
									{
										nextParameterPosition = offlineParam.readPosition;
										nextParameterIndex = offlineParam.index;
										nextByteArraySize = offlineParam.readCount;
									}
								}
							}
						}
						if (nextParameterPosition >= 0)
						{	// start initializing for offlineRead
							offlineParam.readPosition = nextParameterPosition;
							offlineParam.index = nextParameterIndex;
							offlineParam.readCount = nextByteArraySize;
							if (nextParameterPosition == position)
							{
								if (nextByteArraySize)
								{
									float* buffer = new float[nextByteArraySize / sizeof(float)];
									offlineParam.extraBuffer = buffer;
									if (offlineRead(&offlineParam, kVstOfflineParameter))
									{
										if (memcmp(buffer, inputSamples, nextByteArraySize) == 0)
										{
											successfullCheck = true;
										}
									}
									delete [] buffer;
								}
								else
								{
									float value;
									offlineParam.extraBuffer = &value;
									if (offlineRead(&offlineParam, kVstOfflineParameter))
									{
										if (inputSamples[0] == value)
										{
											successfullCheck = true;
										}
									}
								}
								nextParameterPosition = offlineParam.readPosition;
								nextParameterIndex = offlineParam.index;
								nextByteArraySize = offlineParam.readCount;
							}
						}
						if (! successfullCheck)
						{
							// ...							
						}
					}
				}
				i++;
			}
			count += offline->readCount;
			remainingFramesToProcess -= offline->readCount;
		}
	}
	return true;
}

// -----------------------------------------------------------------------------

// Perform a single echo and mix the tail with the rest of the source file

bool OfflineEffect::OneEcho(VstOfflineTask* offline)
{
	offline->readPosition = offline->positionToProcessFrom;
	double remainingFramesToProcess = offline->numFramesToProcess;
	strcpy(offline->progressText, "Echo");
	offline->writePosition = 0;

	int delayTime = (long)offline->sourceSampleRate;	// 1 second delay
	float** buffers = new float*[offline->numSourceChannels];
	int iChannel;
	for (iChannel = 0; iChannel < offline->numSourceChannels; iChannel++)
	{
		buffers[iChannel] = new float[delayTime];
		memset(buffers[iChannel], 0, delayTime * sizeof(float));
	}
	int iHead0 = 0;
	remainingFramesToProcess += delayTime;
	bool error = false;

	while (remainingFramesToProcess > 0)
	{
		offline->progress = 1 - (remainingFramesToProcess / offline->numFramesToProcess);

		// Read samples
		offline->readCount = offline->sizeInputBuffer;
		if (remainingFramesToProcess < offline->readCount)
			offline->readCount = (long)remainingFramesToProcess;
		if (! offlineRead(offline, kVstOfflineAudio))
		{
			error = true;
			break;
		}

		// Transform samples
		int iHead;
		long nFrames = offline->readCount + offline->value;
		if (nFrames > remainingFramesToProcess)
			nFrames = (long)remainingFramesToProcess;
		for (int iChannel = 0; iChannel < offline->numSourceChannels; iChannel++)
		{
			iHead = iHead0;
			const float* inputSamples = ((const float**)offline->inputBuffer)[iChannel];
			float* outputSamples = ((float**)offline->outputBuffer)[iChannel];
			float* delayLine = buffers[iChannel];
			for (int iSample = 0; iSample < nFrames; iSample++)
			{
				delayLine[iHead] = *inputSamples;
				if (++iHead == delayTime)
					iHead = 0;
				*outputSamples = *inputSamples + delayLine[iHead];
				inputSamples++;
				outputSamples++;
			}
		}
		offline->writeCount = nFrames;
		remainingFramesToProcess -= nFrames;
		iHead0 = iHead;

		// Write samples
		if (! offlineWrite(offline, kVstOfflineAudio))
		{
			error = true;
			break;
		}
	}

	for (iChannel = 0; iChannel < offline->numSourceChannels; iChannel++)
		delete [] buffers[iChannel];
	delete [] buffers;
	return ! error;
}

// -----------------------------------------------------------------------------

// Time stretch a file (host will update the markers, is any, accordingly)

bool OfflineEffect::HalfPitchShift(VstOfflineTask* offline)
{
	offline->readPosition = offline->positionToProcessFrom;
	double remainingFramesToProcess = offline->numFramesToProcess;
	strcpy(offline->progressText, "Pitch Shift");
	offline->writePosition = 0;
	offline->writeCount = 0;

	while (remainingFramesToProcess > 0)
	{
		offline->progress = 1 - (remainingFramesToProcess / offline->numFramesToProcess);

		// Read samples
		offline->readCount = offline->sizeInputBuffer;
		if (remainingFramesToProcess < offline->readCount)
			offline->readCount = (long)remainingFramesToProcess;
		if (! offlineRead(offline, kVstOfflineAudio))
			return false;
		remainingFramesToProcess -= offline->readCount;

		// Transform samples
		bool end = false;
		for (int iSample = 0; iSample < offline->readCount; iSample++)
		{
			bool isLastFrame = (remainingFramesToProcess == 0) && (iSample == (offline->readCount - 1));
			for (int i = 0; i < 2; i++)	// two output samples for one input sample
			{
				for (int iChannel = 0; iChannel < offline->numSourceChannels; iChannel++)
				{
					float inputSample = ((const float**)offline->inputBuffer)[iChannel][iSample];
					((float**)offline->outputBuffer)[iChannel][offline->writeCount] = inputSample;
				}
				if ((++offline->writeCount == offline->sizeOutputBuffer) || isLastFrame)
				{	// Write samples
					if (! offlineWrite(offline, kVstOfflineAudio))
						return false;
					offline->writeCount = 0;
				}
			}
		}
	}
	return true;
}

// -----------------------------------------------------------------------------

// Swap the two channels of a stereo file.
// Audio interleave format is used here.

bool OfflineEffect::InterleavedSwap(VstOfflineTask* offline)
{
	offline->readPosition = offline->positionToProcessFrom;
	double remainingFramesToProcess = offline->numFramesToProcess;
	strcpy(offline->progressText, "Swap channels (interleaved)");
	offline->writePosition = 0;

	while (remainingFramesToProcess)
	{
		offline->progress = 1 - (remainingFramesToProcess / offline->numFramesToProcess);

		// Read samples
		offline->readCount = offline->sizeInputBuffer;
		if (remainingFramesToProcess < offline->readCount)
			offline->readCount = (long)remainingFramesToProcess;
		if (! offlineRead(offline, kVstOfflineAudio))
			return false;

		// Transform samples
		const float* inputSamples = (const float*)offline->inputBuffer;
		float* outputSamples = (float*)offline->outputBuffer;
		long n = offline->readCount * 2;	// function wouldn't be called if it was not stereo
		for (int iSample = 0; iSample < n; iSample += 2)
		{
			outputSamples[iSample] = inputSamples[iSample + 1];
			outputSamples[iSample + 1] = inputSamples[iSample];
		}

		// Write samples
		offline->writeCount = offline->readCount;
		if (! offlineWrite(offline, kVstOfflineAudio))
			return false;

		remainingFramesToProcess -= offline->readCount;
	}
	return true;
}
 
// -----------------------------------------------------------------------------

// Demonstration of a process using a temporary file
// The source data is copied in a temporary file, then read from that temp
// file back into the source file, while decreasing level by 6dB.

bool OfflineEffect::UseOfTempFile(VstOfflineTask* offline)
{
	VstOfflineTask* sourceFile = &offline[0];
	VstOfflineTask* tempFile = &offline[1];

	sourceFile->readPosition = sourceFile->positionToProcessFrom;
	double remainingFramesToProcess = sourceFile->numFramesToProcess;
	strcpy(sourceFile->progressText, "Use of Temp file");
	tempFile->writePosition = 0;

	// read source + write temp file
	while (remainingFramesToProcess > 0)
	{
		sourceFile->progress = 1 - (remainingFramesToProcess / sourceFile->numFramesToProcess);
		tempFile->progress = -1;

		// Read samples
		sourceFile->readCount = sourceFile->sizeInputBuffer;
		if (remainingFramesToProcess < sourceFile->readCount)
			sourceFile->readCount = (long)remainingFramesToProcess;
		if (! offlineRead(sourceFile, kVstOfflineAudio))
			return false;

		// Simply transfer samples
		for (int iChannel = 0; iChannel < offline->numSourceChannels; iChannel++)
		{
			const float* inputSamples = ((const float**)sourceFile->inputBuffer)[iChannel];
			float* outputSamples = ((float**)tempFile->outputBuffer)[iChannel];
			memcpy(outputSamples, inputSamples, sourceFile->readCount * sizeof(float));
		}

		// Write samples
		tempFile->writeCount = sourceFile->readCount;
		if (! offlineWrite(tempFile, kVstOfflineAudio))
			return false;

		remainingFramesToProcess -= sourceFile->readCount;
	}

	// read temp file + overwrite source file
	tempFile->readPosition = 0;
	sourceFile->writePosition = 0;
	remainingFramesToProcess = sourceFile->numFramesToProcess;
	while (remainingFramesToProcess > 0)
	{
		sourceFile->progress = 1 - (remainingFramesToProcess / sourceFile->numFramesToProcess);
		tempFile->progress = -1;

		// Read samples
		tempFile->readCount = tempFile->sizeInputBuffer;
		if (remainingFramesToProcess < tempFile->readCount)
			tempFile->readCount = (long)remainingFramesToProcess;
		if (! offlineRead(tempFile, kVstOfflineAudio, false))
			return false;

		// Simply transfer samples
		for (int iChannel = 0; iChannel < offline->numSourceChannels; iChannel++)
		{
			const float* inputSamples = ((const float**)tempFile->inputBuffer)[iChannel];
			float* outputSamples = ((float**)sourceFile->outputBuffer)[iChannel];
			for (int iSample = 0; iSample < tempFile->readCount; iSample++)
			{
				*outputSamples = *inputSamples / 2;
				inputSamples++;
				outputSamples++;
			}
		}

		// Write samples
		sourceFile->writeCount = tempFile->readCount;
		if (! offlineWrite(sourceFile, kVstOfflineAudio))
			return false;

		remainingFramesToProcess -= tempFile->readCount;
	}
	return true;
}

// -----------------------------------------------------------------------------

// Copy a file and its markers

bool OfflineEffect::CopyFile(VstOfflineTask* offline)
{
	VstOfflineTask* sourceFile = &offline[0];
	VstOfflineTask* destFile = &offline[1];

	sourceFile->readPosition = sourceFile->positionToProcessFrom;
	double remainingFramesToProcess = sourceFile->numFramesToProcess;
	strcpy(sourceFile->progressText, "Copy file");
	destFile->writePosition = 0;

	// read source + write temp file
	while (remainingFramesToProcess > 0)
	{
		sourceFile->progress = 1 - (remainingFramesToProcess / sourceFile->numFramesToProcess);
		destFile->progress = -1;

		// Read samples
		sourceFile->readCount = sourceFile->sizeInputBuffer;
		if (remainingFramesToProcess < sourceFile->readCount)
			sourceFile->readCount = (long)remainingFramesToProcess;
		if (! offlineRead(sourceFile, kVstOfflineAudio))
			return false;

		// Simply transfer samples
		for (int iChannel = 0; iChannel < offline->numSourceChannels; iChannel++)
		{
			const float* inputSamples = ((const float**)sourceFile->inputBuffer)[iChannel];
			float* outputSamples = ((float**)destFile->outputBuffer)[iChannel];
			memcpy(outputSamples, inputSamples, sourceFile->readCount * sizeof(float));
		}

		// Write samples
		destFile->writeCount = sourceFile->readCount;
		if (! offlineWrite(destFile, kVstOfflineAudio))
			return false;

		remainingFramesToProcess -= sourceFile->readCount;
	}
	// copy markers
	destFile->index = 0;	// index of task representing source file
	if (! offlineWrite(destFile, kVstOfflineMarker))
		return false;

	return true;
}

// -----------------------------------------------------------------------------

// Generate a sine wave. Does not read any source file

bool OfflineEffect::SineGenerator(VstOfflineTask* offline)
{
	double total = offline->destinationSampleRate;	// let's produce 1 sec worth of sines
	double remainingFramesToProduce = total;	// let's produce 1 sec worth of sines
	strcpy(offline->progressText, "Generate sine wave");
	offline->writePosition = 0;
	double phase = 0;
	double step = 2 * 3.14159265 * 220 / offline->destinationSampleRate;	// 220 Hz

	// read source + write temp file
	while (remainingFramesToProduce > 0)
	{
		offline->progress = 1 - (remainingFramesToProduce / offline->numFramesToProcess);

		// Produce samples
		offline->writeCount = offline->sizeOutputBuffer;
		if (remainingFramesToProduce < offline->writeCount)
			offline->writeCount = (long)remainingFramesToProduce;

		float* outputSamples = ((float**)offline->outputBuffer)[0];
		for (int i = 0; i < offline->writeCount; i++)
		{
			*outputSamples++ = (float)sin(phase);
			phase += step;
		}

		// Write samples
		if (! offlineWrite(offline, kVstOfflineAudio))
			return false;

		remainingFramesToProduce -= offline->writeCount;
	}
	return true;
}

// -----------------------------------------------------------------------------


