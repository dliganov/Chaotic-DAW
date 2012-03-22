#ifndef __vstfxstore__
#define __vstfxstore__

#define cMagic 		'CcnK'
#define fMagic		'FxCk'
#define bankMagic	'FxBk'
#define chunkGlobalMagic	'FxCh'
#define chunkPresetMagic	'FPCh'
#define chunkBankMagic		'FBCh'

//--------------------------------------------------------------------
struct fxProgram
{
	long chunkMagic;		// 'CcnK'
	long byteSize;			// of this chunk, excl. magic + byteSize

	long fxMagic;			// 'FxCk'
	long version;
	long fxID;				// fx unique id
	long fxVersion;

	long numParams;
	char prgName[28];
	float params[1];		// variable no. of parameters	
};

//--------------------------------------------------------------------
struct fxSet
{
	long chunkMagic;		// 'CcnK'
	long byteSize;			// of this chunk, excl. magic + byteSize

	long fxMagic;			// 'FxBk'
	long version;
	long fxID;				// fx unique id
	long fxVersion;

	long numPrograms;
	char future[128];

	fxProgram programs[1];	// variable no. of programs
};

//--------------------------------------------------------------------
struct fxChunkSet
{
	long chunkMagic;		// 'CcnK'
	long byteSize;			// of this chunk, excl. magic + byteSize

	long fxMagic;			// 'FxCh', 'FPCh', or 'FBCh'
	long version;
	long fxID;				// fx unique id
	long fxVersion;

	long numPrograms;
	char future[128];

	long chunkSize;
	char chunk[8];			// variable
};

#endif
