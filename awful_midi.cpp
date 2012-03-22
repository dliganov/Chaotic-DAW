/*==================================================================================================

    Module Name: awful_midi.cpp

    General Description: Helper MIDI host implementation. Midi host class is responsible for
                         interaction with system MIDI devices.


====================================================================================================
                               Chaotic Confidential Restricted
                         (c) Copyright Chaotic 2009, All Rights Reserved

Revision History:

Author                  Date            Major Changes description
---------------------   -------------   ------------------------------------------------------------
Alexander Veprik        02/18/2009      Initial creation

====================================================================================================
                              INCLUDE FILES
==================================================================================================*/
#include "awful_midi.h"
#include "Awful_audio.h"
#include "Awful_preview.h"
//#include "Awful_utils_common.h"


/*==================================================================================================
                          FUNCTIONS AND CLASS MEMBER IMPLEMENTATION SECTION
==================================================================================================*/

/*==================================================================================================
BRIEF:
    This is the main MIDI in callback routine. The function is got called by midi-in devices for
    all non SysEx midi messages.

PARAMETERS:
[in]
    source  - pointer to a midi source called callback
    message - the message to handle

[out]
    none

OUTPUT:
    none

==================================================================================================*/
void AwfulMidiInputCallback::handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message)
{
    unsigned char*  midi_data = message.getRawData();
    MidiEventType_t evt       = (MidiEventType_t)(midi_data[0] & 0xf0);

    /* Some stupid lousy Synths like casio CTK-481, which doesn't even have pitch wheel, 
       they don't send NoteOff when a key is released. They send note-on with 0-velosity instead. */
    if ((evt == MidiNoteOn) && (message.getFloatVelocity() == 0))
    {
        evt = MidiNoteOff;
    }

    switch (evt)
    {
        case MidiNoteOn:
        {

            //go through the all instruments of the source and send note evt to 'em
            unsigned char src_idx = owner->GetSrcIndex(source); // find the source in the array

            MidiToHost_AddNoteOn(current_instr,
                message.getNoteNumber(), message.getVelocity());

			/*
            for(unsigned char i = 0; i < MAX_NUM_INSTR_PER_MIDI_IN_SRC; ++i)
            {
                if (owner->m_InstrSlots[src_idx][i] != NULL)
                {
                    MidiToHost_AddNoteOn(owner->m_InstrSlots[src_idx][i],
                        message.getNoteNumber(), message.getFloatVelocity());
                }
            }
			*/ 

        }break;

        case MidiNoteOff:
        {
            //TODO: we got to create fake samplent/longnote of the length equal to key preassure time

            //go through the all instruments of the source and send note evt to 'em
            unsigned char src_idx = owner->GetSrcIndex(source); // find the source in the array

            MidiToHost_AddNoteOff(current_instr, message.getNoteNumber());
			/*
            for(unsigned char i = 0; i < MAX_NUM_INSTR_PER_MIDI_IN_SRC; ++i)
            {
                if (owner->m_InstrSlots[src_idx][i] != NULL)
                {
                    MidiToHost_AddNoteOff(owner->m_InstrSlots[src_idx][i], message.getNoteNumber());
                }
            }
			*/ 
        }break;

        case MidiKeyAftertouch:
        {
            unsigned char src_idx = owner->GetSrcIndex(source); // find the source in the array

            for(unsigned char i = 0; i < MAX_NUM_INSTR_PER_MIDI_IN_SRC; ++i)
            {
                if (owner->m_InstrSlots[src_idx][i] != NULL)
                {
                    //Change the volume of the particular note
                    Preview_SetNoteVolume(message.getNoteNumber(), message.getFloatVelocity());
                }
            }
        }break;

        case MidiChannelAftertouch:
        {
            unsigned char src_idx = owner->GetSrcIndex(source); // find the source in the array

            for(unsigned char i = 0; i < MAX_NUM_INSTR_PER_MIDI_IN_SRC; ++i)
            {
                if (owner->m_InstrSlots[src_idx][i] != NULL)
                {
                    //Change the volume of the particular note
                    Preview_SetVolume(message.getFloatVelocity());
                }
            }
        }break;

        case MidiControlChange:
        {
            unsigned int num = message.getControllerNumber();
            unsigned int val = message.getControllerValue();

            switch(num)
            {
                case MidiCAllSoundOff:
                case MidiCAllNotesOff:
                {
                    //MidiToHost_StopAllNotes();
                }break;

                case MidiCPan:
                default:
                    break;
            }

        }break;

        case MidiPitchBend:
        {
            //MidiToHost_PitchBend();
        }break;

        case MidiProgramChange:
        default:
            break;
    }
}

/*==================================================================================================
BRIEF:
    This is the MIDI in callback routine for System Exclusive messages. The function is got called 
    by midi-in devices for SysEx midi messages.

PARAMETERS:
[in]
    source        - pointer to a midi source called callback
    messageData   - pointer to a data field of the message
    numBytesSoFar - num bytes received so far
    timestamp     - message timestamp

[out]
    none

OUTPUT:
    none

==================================================================================================*/
void AwfulMidiInputCallback::handlePartialSysexMessage (MidiInput* source,
                                                        const uint8* messageData,
                                                        const int numBytesSoFar,
                                                        const double timestamp)
{
    //just a stub. Don't think we really need to support SysEx messages in our sequensor
}



AwfulMidiWrapper::AwfulMidiWrapper()
{
    unsigned char index = 0;
    unsigned char j = 0;

    m_MidiInputCallback = new AwfulMidiInputCallback(this);

    for (index = 0; index < MAX_NUM_MIDI_IN_SRC; ++index)
    {
        m_MidiInputs[index].source = NULL;
        m_MidiInputs[index].index = -1;

        m_DevList[index].index = -1;
        m_DevList[index].name = NULL;

        for (j = 0; j < MAX_NUM_INSTR_PER_MIDI_IN_SRC; ++j)
        {
            m_InstrSlots[index][j] = NULL;
        }

        m_InstrCount[index] = 0;
    }

    /* Now fill the device list. Re-convert juce structures to old good C-based buffers with pointers.
       Yeah, I know, I'm of old school programmer ;) */
    StringArray devs = MidiInput::getDevices();
    String str;

    for (index = 0; index < devs.size(); ++index)
    {
        m_DevList[index].index = index;
        str = devs[index];
        m_DevList[index].name = (char*) malloc((str.length() + 1) * sizeof(char));

        if (m_DevList[index].name != NULL)
        {
            memset(m_DevList[index].name, 0, str.length() + 1);
            str.copyToBuffer(m_DevList[index].name, str.length());
        }
    }

    m_DeviceCount = devs.size();
}

AwfulMidiWrapper::~AwfulMidiWrapper()
{
    unsigned char index = 0;

    if (m_MidiInputCallback != NULL)
    {
        delete m_MidiInputCallback;
        m_MidiInputCallback = NULL;
    }

    for (index = 0; index < MAX_NUM_MIDI_IN_SRC; ++index)
    {
        if (m_MidiInputs[index].source != NULL)
        {
            this->CloseMidiSource(index);
        }

        if (m_DevList[index].name != NULL)
        {
            free(m_DevList[index].name);
            m_DevList[index].name = NULL;
        }
    }
}

unsigned char AwfulMidiWrapper::GetDeviceCount() const
{
    return m_DeviceCount;
}

const MIDEVICE_DESCR_T* AwfulMidiWrapper::GetDeviceDescr(unsigned char index) const
{
    return &(m_DevList[index]);
}

unsigned char AwfulMidiWrapper::Attach(Instrument* instr, unsigned char juce_index)
{
    unsigned int ret_val = -1;
    char         i = 0, j = 0, pos = -1, index = -1;

    /* First of all, convert juce_index to an index of the source in internal array */
    for (i = 0; i < MAX_NUM_MIDI_IN_SRC; ++i)
    {
        if (m_MidiInputs[i].index == juce_index)
        {
            index = i;
            break;
        }
        else if ((m_MidiInputs[i].index == -1) &&
                 (index == -1))
        {
            index = i;
        }
    }

    if (index == -1)
    {
        /* there are no free slots. MAX_NUM_MIDI_IN_SRC is not enough */
        return index;
    }

    /* first of all, go through the all slots and detach the instrument if any */
    for (i = 0; i < MAX_NUM_MIDI_IN_SRC; ++i)
    {
        /* exclude from the search the source being attached */
        if (i != index)
        {
            for(j = 0; j < MAX_NUM_INSTR_PER_MIDI_IN_SRC; ++j)
            {
                if (m_InstrSlots[i][j] == instr)
                {
                    m_InstrSlots[i][j] = NULL;
                    --(m_InstrCount[i]);

                    if (m_InstrCount[i] <= 0)
                    {
                        this->CloseMidiSource(index);
                    }
                    break;
                }
            }
        }
    }

    for (j = 0; j < MAX_NUM_INSTR_PER_MIDI_IN_SRC; ++j)
    {
        if (m_InstrSlots[index][j] == instr)
        {
            /* If the instrument already attached to the source, just return the index */
            return j;
        }
        else if ((m_InstrSlots[index][j] == NULL) &&
                 (pos == -1))
        {
            pos = j;
        }
    }

    if (pos != -1)
    {
        /* went through the slots and didn't find any match, attach then */
        if (true == this->OpenMidiSource(juce_index))
        {
            m_InstrSlots[index][pos] = instr;
            ++(m_InstrCount[index]);
        }
        else
        {
            /* Error happened. Can't open MidiSource */
            pos = -1;
        }
    }

    return pos;
}

void AwfulMidiWrapper::Detach(Instrument* instr)
{
    unsigned char i,j;

    for (i = 0; i < MAX_NUM_MIDI_IN_SRC; ++i)
    {
        for(j = 0; j < MAX_NUM_INSTR_PER_MIDI_IN_SRC; ++j)
        {
            if (m_InstrSlots[i][j] == instr)
            {
                m_InstrSlots[i][j] = NULL;
                --(m_InstrCount[i]);

                if (m_InstrCount[i] <= 0)
                {
                    this->CloseMidiSource(i);
                }
                break;
            }
        }
    }
}

bool AwfulMidiWrapper::OpenMidiSource(unsigned char juce_index)
{
    bool ret_val = true;
    bool open = false;
    char index = -1, i = 0;

    /* First of all, convert juce_index to an index of the source in internal array */
    for (i = 0; i < MAX_NUM_MIDI_IN_SRC; ++i)
    {
        if (m_MidiInputs[i].index == juce_index)
        {
            index = i;
            open = true;
            break;
        }
        else if ((m_MidiInputs[i].index == -1) &&
                 (index == -1))
        {
            index = i;
        }
    }

    if (index != -1)
    {
        if (open == false)
        {
            if (NULL != (m_MidiInputs[index].source = MidiInput::openDevice(juce_index, m_MidiInputCallback)))
            {
                m_MidiInputs[index].index = juce_index;
                m_MidiInputs[index].source->start();
            }
            else
            {
                ret_val = false;
            }
        }
    }
    else
    {
        ret_val = false;
    }

    return ret_val;
}

void AwfulMidiWrapper::CloseMidiSource(unsigned char index)
{
    if (m_MidiInputs[index].source != NULL)
    {
        m_MidiInputs[index].source->stop();
        delete m_MidiInputs[index].source;
        m_MidiInputs[index].source = NULL;
        m_MidiInputs[index].index = -1;
    }
}

