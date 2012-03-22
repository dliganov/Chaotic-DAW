#ifndef _AWFUL_MIDI_DEF_H_
#define _AWFUL_MIDI_DEF_H_
/*==================================================================================================

    Module Name: awful_midi_def.h

    General Description: Set of MIDI related constant definitions.


====================================================================================================
                               Chaotic Confidential Restricted
                         (c) Copyright Chaotic 2009, All Rights Reserved

Revision History:

Author                  Date            Major Changes description
---------------------   -------------   ------------------------------------------------------------
Alexander Veprik        02/18/2009      Initial creation

/*==================================================================================================
                         TYPEDEFS AND ENUM DEFINITIONS SECTION
==================================================================================================*/
typedef enum MidiEventType_t
{
    // messages
    MidiNoteOff             = 0x80,
    MidiNoteOn              = 0x90,
    MidiKeyAftertouch       = 0xA0,
    MidiControlChange       = 0xB0,
    MidiProgramChange       = 0xC0,
    MidiChannelAftertouch   = 0xD0,
    MidiPitchBend           = 0xE0,
    // system exclusive
    MidiSysEx               = 0xF0,
    // system common - never in midi files
    MidiTimeCode            = 0xF1,
    MidiSongPosition        = 0xF2,
    MidiSongSelect          = 0xF3,
    MidiTuneRequest         = 0xF6,
    MidiEOX                 = 0xF7,
    // system real-time - never in midi files
    MidiSync                = 0xF8,
    MidiTick                = 0xF9,
    MidiStart               = 0xFA,
    MidiContinue            = 0xFB,
    MidiStop                = 0xFC,
    MidiActiveSensing       = 0xFE,
    MidiSystemReset         = 0xFF
}MidiEventType_t;

typedef enum MidiControllerCode_t
{
    MidiCBankSelect          = 0x00,
    MidiCModulation          = 0x01,
    MidiCBreath              = 0x02,
    MidiCFoot                = 0x04,
    MidiCContunuousCtrl3     = 0x03,
    MidiCPortamentoTime      = 0x05,
    MidiCDataEntry           = 0x06,
    MidiCMainVolume          = 0x07,
    MidiCBalance             = 0x08,
    MidiCPan                 = 0x0A,
    MidiCExpression          = 0x0B,
    MidiCEffect1Control      = 0x0C,
    MidiCEffect2Control      = 0x0D,
    MidiCGeneral1            = 0x10,
    MidiCGeneral2,
    MidiCGeneral3,
    MidiCGeneral4            = 0x13,
    MidiCLSB0                = 0x20,
    /* LSB 0 - 31 */
    MidiCLSB31               = 0x3F,
    MidiCDamperPedal         = 0x40,
    MidiCPortamento          = 0x41,
    MidiCSostenuto           = 0x42,
    MidiCSoftPedal           = 0x43,
    MidiCLegatoFootswitch    = 0x44,
    MidiCHold2               = 0x45,
    MidiCSoundController1    = 0x46,
    MidiCSoundController2    = 0x47,
    MidiCSoundController3    = 0x48,
    MidiCSoundController4    = 0x49,
    MidiCSoundController6    = 0x4A,
    /* 6 - 10 */
    MidiCSoundController10   = 0x4F,
    MidiCGeneral5            = 0x50,
    MidiCGeneral6,
    MidiCGeneral7,
    MidiCGeneral8            = 0x53,
    MidiCPortamentoControl   = 0x54,
    MidiCEffect1Depth        = 0x5B,
    MidiCEffect2Depth,
    MidiCEffect3Depth,
    MidiCEffect4Depth,
    MidiCEffect5Depth        = 0x5F,
    MidiCDataIncrement       = 0x60,
    MidiCDataDecrement       = 0x61,
    MidiCUnregParamNumLSB    = 0x62,
    MidiCUnregParamNumMSB    = 0x63,
    MidiCRegParamNumLSB      = 0x64,
    MidiCRepParamNumMSB      = 0x65,
    MidiCAllSoundOff         = 0x78, /* 120 */
    MidiCLocalCtrl           = 0x7A, /* 0 = off, 127 = on */
    MidiCAllNotesOff         = 0x7B, /* 123 */
    MidiCOmniOff             = 0x7C,
    MidiCOmniOn              = 0x7D
}MidiControllerCode_t;


#endif /* _AWFUL_MIDI_DEF_H_ */