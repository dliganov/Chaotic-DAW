#ifndef _AWFUL_RENDERER_
#define _AWFUL_RENDERER_
#include "Awful.h"
#include "sndfile.h"

#define RNDR_MAX_FILENAME_LENGTH 300

/* Codes which client should provide */
typedef enum
{
    RNDR_Continue = 0,
    RNDR_Complete,
    RNDR_Abort
} RNDR_StreamCode;

/* Prototype of callback function */
typedef RNDR_StreamCode RndrCallback(void *sample_buff, unsigned long frameCount);

//extern
//DWORD RNDR_ThreadProc(LPVOID lpParam);


/* Internal states of the renderer module*/
typedef enum
{
	RNDR_STATUS_READY = 0,
	RNDR_STATUS_IO_READING,
	RNDR_STATUS_IO_WRITING,
	RNDR_STATUS_DATA_REQUESTED,
	RNDR_STATUS_DATA_PROCESSING,
	RNDR_STATUS_THREAD_STOPPING,
    RNDR_STATUS_PAUSED,
	RNDR_STATUS_RUNNING_IDLE,
	RNDR_STATUS_INVALID
} RNDR_STATUS_T;

/* error codes which renderer can return */
typedef enum
{
	RNDR_NO_ERROR = 0,
	RNDR_ERROR_WRITE_FILE,
	RNDR_ERROR_READ_FILE,
	RNDR_ERROR_INVALID_PARAMS,
	RNDR_ERROR_INTERNAL_FAIL,
	RNDR_ERROR_MEM_ALLOC_FAIL,
	RNDR_ERROR_API_MISUSE,
	RNDR_ERROR_OPEN_FILE_FAILED,
	RNDR_ERROR_MAX
} RNDR_ERROR_T;

typedef enum
{
    RNDR_FORMAT_WAVE,
    RNDR_FORMAT_OGG,
    RNDR_FORMAT_FLAC,
    RNDR_FORMAT_MP3
} RNDR_FORMAT_T;

enum
{
    RNDR_QUALITY_8BIT = 0,
    RNDR_QUALITY_16BIT,
    RNDR_QUALITY_24BIT,
    RNDR_QUALITY_32BIT,
    RNDR_QUALITY_FLOAT,
    RNDR_QUALITY_VORBIS_BASE,
    RNDR_QUALITY_VORBIS_0,
    RNDR_QUALITY_VORBIS_1,
    RNDR_QUALITY_VORBIS_2,
    RNDR_QUALITY_VORBIS_3,
    RNDR_QUALITY_VORBIS_4,
    RNDR_QUALITY_VORBIS_5,
    RNDR_QUALITY_VORBIS_6,
    RNDR_QUALITY_VORBIS_7,
    RNDR_QUALITY_VORBIS_8,
    RNDR_QUALITY_VORBIS_9,
    RNDR_QUALITY_VORBIS_10,
    RNDR_QUALITY_VORBIS_POOR, /* map to quality 1 */
    RNDR_QUALITY_VORBIS_GOOD, /* map to quality 4 */
    RNDR_QUALITY_VORBIS_BEST, /* map to quality 6 */
    RNDR_QUALITY_VORBIS_BRILIANT, /* map to quality 10 */
    RNDR_QUALITY_MAX
};

typedef unsigned int RNDR_QUALITY_T;

typedef struct RNDR_CONFIG_DATA_S
{
    /* Lenght of input buffer in frames. Frame is a set of sample values for each channel.
       E.g. if num_channels is 2 then frame is just a pair of float values */
    unsigned long inbuff_len;
    /* Output file format. Only wav/flac/ogg are currently suported formats */
    RNDR_FORMAT_T format;

    // Quality variables. One common and three separate for each format (applicable for saving)
    unsigned int  quality;
    int q1;
    int q2;

    /* File name (with full path) to render to */
    char          file_name[RNDR_MAX_FILENAME_LENGTH];

    String        output_dir;
} RNDR_CONFIG_DATA_T;

class Renderer
{
public:
    Renderer();
   ~Renderer();
    // API to configure renderer parameters
    RNDR_ERROR_T SetConfig(const RNDR_CONFIG_DATA_T* p_config_data);
    // API to get current renderer parameters config
    RNDR_ERROR_T GetConfig(const RNDR_CONFIG_DATA_T** pp_config_data);

    //API to set the length of the project being rendered (in amount of frames)
    RNDR_ERROR_T SetAudioLength(long ullAudioLength);

    //API to set the length of the project being rendered (in amount of frames)
    RNDR_ERROR_T GetAudioLength(long & ullAudioLength);

    //API to get current rendering position (calculated in frames)
    RNDR_ERROR_T GetCurrentPos(long & ullCurrentFramePos);

    RNDR_STATUS_T GetState();
    // as result: renderer will prepare internal variables, open a file stream and create a rendering thread
    RNDR_ERROR_T Open();
    // renderer will start a rendering thread requesting client's callback function for every portion of samples
    RNDR_ERROR_T Start(RndrCallback* p_callback_func);
    // pause a rendering thread for a while. If call Start() again, rendering will resume
    RNDR_ERROR_T Pause();
    // resume paused rendering
    RNDR_ERROR_T Renderer::Resume();
    // stop rendering thread. Calling of Start() again will start a rendering from the beginning
    RNDR_ERROR_T Stop();
    // Stop and close rendering thread, close file stream 
    RNDR_ERROR_T Close();

private:
    // semaphore handle 
    HANDLE        hMutex;

    // Callback function 
    RndrCallback* pCallBack;

    // Current state of renderer 
    RNDR_STATUS_T state;
    // Configuration record 
    RNDR_CONFIG_DATA_T config;
    //Main thread procedure
    static DWORD  RNDR_ThreadProc(LPVOID lpParam);
    HANDLE        RndrThread;
    SNDFILE*      out_file;
    float*        RENDER_BUFF;
    void*         ThreadParam;
    void          AcquireSema();
    void          ReleaseSema();

    //Current position of the renderer
    long   ullCurrentFramePos;

    //Total length of the song being rendered (in frames)
    long   ullSongFrameLength;
};

#endif /* _AWFUL_RENDERER_ */
