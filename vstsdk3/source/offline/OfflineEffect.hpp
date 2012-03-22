#include "AudioEffectx.h"
#include <string.h>

// -----------------------------------------------------------------------------

class OfflineEditor;

enum
{
	kType,
	kNumParams
};

// -----------------------------------------------------------------------------

class OfflineProgram
{
public:
	OfflineProgram();
	~OfflineProgram() {}

private:
	friend class OfflineEffect;
	float	fType;
	char	name[24];
};

// -----------------------------------------------------------------------------

class OfflineEffect : public AudioEffectX
{
public:
	OfflineEffect(audioMasterCallback audioMaster);
	~OfflineEffect();

	virtual void process(float **inputs, float **outputs, long sampleFrames) {}
	virtual void setProgram(long program);
	virtual void setProgramName(char *name);
	virtual void getProgramName(char *name);
	virtual void setParameter(long index, float value);
	virtual float getParameter(long index);
	virtual void getParameterLabel(long index, char *label);
	virtual void getParameterDisplay(long index, char *text);
	virtual void getParameterName(long index, char *text);
	virtual long canDo(char* function);

	// offline specific
	virtual bool offlinePrepare(VstOfflineTask* offline, long count);
	virtual bool offlineRun(VstOfflineTask* offline, long count);
	virtual bool offlineNotify(VstAudioFile* files, long count, bool start);

	// process examples
	bool	Minus6dB(VstOfflineTask* offline);
	bool	Reverse(VstOfflineTask* offline);
	bool	Reverse2(VstOfflineTask* offline);
	bool	Maximize(VstOfflineTask* offline, bool output);
	bool	ReverseThenMaximize(VstOfflineTask* offline);
	bool	Mix(VstOfflineTask* offline, long count);
	bool	CursorMarkerSelection(VstOfflineTask* offline);
	bool	ParameterRecording(VstOfflineTask* offline);
	bool	OneEcho(VstOfflineTask* offline);
	bool	HalfPitchShift(VstOfflineTask* offline);
	bool	InterleavedSwap(VstOfflineTask* offline);
	bool	UseOfTempFile(VstOfflineTask* offline);
	bool	CopyFile(VstOfflineTask* offline);
	bool	SineGenerator(VstOfflineTask* offline);

protected:
	OfflineEditor* offlineEditor;
	OfflineProgram*	programs;
	OfflineProgram	m_currProgram;
};

// -----------------------------------------------------------------------------

enum
{
	k_ofl_minus6dB = 1,
	k_ofl_reverse,
	k_ofl_reverse2,
	k_ofl_maximize,
	k_ofl_reverse_maximize,
	k_ofl_mix,
	k_ofl_cursorMarkerSelection,
	k_ofl_parameterRecording,
	k_ofl_oneEcho,
	k_ofl_halfPitchShift,
	k_ofl_interleavedSwap,
	k_ofl_useOfTempFile,
	k_ofl_copyFile,
	k_ofl_sineGenerator,

	k_ofl_numTypes,
};

// -----------------------------------------------------------------------------
