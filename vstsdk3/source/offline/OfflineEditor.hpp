#include "AEffEditor.hpp"

//-----------------------------------------------------------------------------

struct VstAudioFile;

class OfflineEditor : public AEffEditor
{
public:
	OfflineEditor(AudioEffectX *effect);
	virtual ~OfflineEditor();

	virtual long getRect(ERect **rect);
	virtual long open(void *ptr);
	virtual void close();
	virtual void idle();
	void OnHostChange(VstAudioFile*, long count);

	virtual void setValue (void* fader, int value);
	AudioEffectX*	getEffectX() { return effectx; }

private:
	AudioEffectX*	effectx;
	ERect			rect;

	void*			uiHandle;
};

//-----------------------------------------------------------------------------
