#include "stdafx.h"
#include "resource.h"

#include "awful.h"
#include "awful_audio.h"
#include "awful_instruments.h"
#include "awful_elements.h"
#include "awful_paramedit.h"
#include "awful_controls.h"
#include "awful_panels.h"
#include "awful_effects.h"
#include "VSTCollection.h"
#include "awful_events_triggers.h"
#include "awful_tracks_lanes.h"
#include "awful_audiohost.h"
#include "awful_renderer.h"
#include "awful_utils_common.h"
#include "awful_preview.h"
#include "awful_cursorandmouse.h"

extern void         OutsyncTriggers();
extern void         ResetProcessing(bool resetmix);
extern void         Place_Note(Instrument *instr, int note, float vol, unsigned long start_frame, unsigned long end_frame);
extern void         UpdatePerBaseFrame(Playback* pb);
extern void         ProcessSymbols4Gennote(Trigger* tg, long curr_frame, float* freq_active, bool* skip_and_continue, bool* skip);
extern PaError      PortAudio_Init();
extern PaError      PortAudio_Deinit();
extern PaError      PortAudio_Start();
extern void			PlayMain();
extern void         StopMain(bool forceresetmix);
void                MixPreviewData(float* dataL, float* dataR);
bool                IsCommandApplicable(Command* cmd, Instance* instr_instance);
extern inline float MixPans(float src_pan, float dst_pan);
inline void		    GetMonoSampleData(Sample* sample, double cursor_pos, float* dataLR);
inline void			GeStereoSampleData(Sample* sample, double cursor_pos, float* dataL, float* dataR);
inline double       Sinc(double a);
inline double       SincWindowedBlackman(double a, double b, unsigned int num);
inline float        Sinc_Interpolate(float* Yi, double dX, unsigned int num);
extern void         ResetUnstableElements();
extern void         ResetPlacedEnvelopes();

/* Main callback function called by portAudio when it needs to fill sample buffer with next chunk of data */
static int          PortAudio_StreamCallback(const void  *inputBuffer, void  *outputBuffer,
                                                    unsigned long   framesPerBuffer, const PaStreamCallbackTimeInfo*  timeInfo,
                                                    PaStreamCallbackFlags   statusFlags, void  *userData);

/* Function provided to Renderer module as callback filling in sample buffer */
static RNDR_StreamCode Render_Callback(void *sample_buff, unsigned long frameCount);

void                CommonAudioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer);


long                curr_frame;

PaStream           *Stream;					// PortAudio stream
//Parameters of active selected output audio device
PaStreamParameters  outputParameters;
//Parameters of active selected input audio device
PaStreamParameters  inputParameters;
PaTime              CurrentTime = 0;

float               databuff[MAX_BUFF_SIZE*2];

// Global mute triggers for antialiasing
bool                GlobalMute = false;
bool                MixMute = false;

//Sample buffer for sinc interpolation
static float Yi[256];

//==============================================================================
// this wraps the actual audio device
CAudioDeviceManager* audioDeviceManager = NULL;
AudioCallback*      audioCallBack = NULL;
AwfulMidiInputCallback* midiCallBack = NULL;

void AudioCallback::audioDeviceIOCallback(const float** inputChannelData, int totalNumInputChannels, float** outputChannelData, int totalNumOutputChannels, int numSamples)
{
    //Random rnd(1);
    if(totalNumOutputChannels == 2)
    {
		CommonAudioCallback(NULL, databuff, numSamples);
		for(int fc = 0; fc < numSamples; fc++)
		{
			outputChannelData[0][fc] = databuff[fc*2];
			outputChannelData[1][fc] = databuff[fc*2 + 1];
/*
			if(Playing)
			{
				outputChannelData[0][fc] = rnd.nextFloat();
				outputChannelData[1][fc] = rnd.nextFloat();
			}
			else
			{
				outputChannelData[0][fc] = 0;
				outputChannelData[1][fc] = 0;
			}
*/
		}
    }
}

void AudioCallback::audioDeviceAboutToStart(AudioIODevice * device)
{
}

void AudioCallback::audioDeviceStopped()
{
}

/*
float SincLookUp[1280] =
{

-0.0000000000, -0.0000000034, -0.0000000267, -0.0000000886, -0.0000002042, -0.0000003844, -0.0000006344, -0.0000009526, -0.0000013304, -0.0000017516,
-0.0000021933, -0.0000026259, -0.0000030145, -0.0000033205, -0.0000035030, -0.0000035213, -0.0000033366, -0.0000029148, -0.0000022285, -0.0000012594,
-0.0000000000, 0.0000015445, 0.0000033552, 0.0000053984, 0.0000076258, 0.0000099747, 0.0000123691, 0.0000147213, 0.0000169346, 0.0000189056,
0.0000205281, 0.0000216964, 0.0000223100, 0.0000222770, 0.0000215193, 0.0000199759, 0.0000176072, 0.0000143981, 0.0000103610, 0.0000055376,
0.0000000001, -0.0000061488, -0.0000127762, -0.0000197216, -0.0000267996, -0.0000338040, -0.0000405129, -0.0000466946, -0.0000521143, -0.0000565413, 
-0.0000597567, -0.0000615614, -0.0000617832, -0.0000602847, -0.0000569697, -0.0000517895, -0.0000447472, -0.0000359017, -0.0000253696, -0.0000133253, 
-0.0000000001, 0.0000143209, 0.0000293043, 0.0000445745, 0.0000597224, 0.0000743152, 0.0000879075, 0.0001000536, 0.0001103202, 0.0001183001, 
0.0001236249, 0.0001259785, 0.0001251089, 0.0001208392, 0.0001130767, 0.0001018211, 0.0000871687, 0.0000693162, 0.0000485598, 0.0000252929,
0.0000000002, -0.0000267508, -0.0000543214, -0.0000820155, -0.0001090959, -0.0001348026, -0.0001583729, -0.0001790617, -0.0001961635, -0.0002090335, 
-0.0002171084, -0.0002199257, -0.0002171417, -0.0002085460, -0.0001940745, -0.0001738174, -0.0001480246, -0.0001171065, -0.0000816299, -0.0000423105, 
-0.0000000003, 0.0000443291, 0.0000896074, 0.0001346898, 0.0001783841, 0.0002194811, 0.0002567857, 0.0002891500, 0.0003155057, 0.0003348960, 
0.0003465066, 0.0003496923, 0.0003440026, 0.0003292012, 0.0003052813, 0.0002724757, 0.0002312602, 0.0001823513, 0.0001266969, 0.0000654607, 
0.0000000005, -0.0000681603, -0.0001373660, -0.0002058669, -0.0002718618, -0.0003335430, -0.0003891443, -0.0004369882, -0.0004755337, -0.0005034221,
-0.0005195193, -0.0005229542, -0.0005131515, -0.0004898576, -0.0004531596, -0.0004034955, -0.0003416558, -0.0002687759, -0.0001863190, -0.0000960500, 
-0.0000000007, 0.0000995739, 0.0002002458, 0.0002994715, 0.0003946534, 0.0004832057, 0.0005626219, 0.0006305423, 0.0006848212, 0.0007235889, 
0.0007453104, 0.0007488362, 0.0007334457, 0.0006988802, 0.0006453654, 0.0005736217, 0.0004848624, 0.0003807792, 0.0002635138, 0.0001356179, 
0.0000000010, -0.0001401329, -0.0002813584, -0.0004201096, -0.0005527675, -0.0006757512, -0.0007856119, -0.0008791253, -0.0009533820, -0.0010058723, 
-0.0010345634, -0.0010379667, -0.0010151934, -0.0009659955, -0.0008907930, -0.0007906833, -0.0006674341, -0.0005234590, -0.0003617754, -0.0001859454, 
-0.0000000013, 0.0001916433, 0.0003842953, 0.0005730935, 0.0007531285, 0.0009195661, 0.0010677745, 0.0011934470, 0.0012927238, 0.0013623025, 
0.0013995388, 0.0014025350, 0.0013702085, 0.0013023436, 0.0011996231, 0.0010636360, 0.0008968629, 0.0007026391, 0.0004850932, 0.0002490649, 
0.0000000017, -0.0002561632, -0.0005131476, -0.0007644715, -0.0010036160, -0.0012241886, -0.0014200897, -0.0015856753, -0.0017159146, -0.0018065337, 
-0.0018541461, -0.0018563629, -0.0018118807, -0.0017205450, -0.0015833842, -0.0014026167, -0.0011816260, -0.0009249062, -0.0006379774, -0.0003272734, 
-0.0000000021, 0.0003360167, 0.0006725338, 0.0010010687, 0.0013131187, 0.0016003746, 0.0018549360, 0.0020695229, 0.0022376769, 0.0023539450, 
0.0024140466, 0.0024150091, 0.0023552794, 0.0022347993, 0.0020550441, 0.0018190256, 0.0015312547, 0.0011976645, 0.0008254969, 0.0004231516, 
0.0000000026, -0.0004338147, -0.0008676408, -0.0012905495, -0.0016916172, -0.0020601999, -0.0023862096, -0.0026603823, -0.0028745350, -0.0030217969, 
-0.0030968161, -0.0030959335, -0.0030173140, -0.0028610397, -0.0026291534, -0.0023256540, -0.0019564433, -0.0015292222, -0.0010533398, -0.0005395958, 
-0.0000000031, 0.0005524872, 0.0011042905, 0.0016415169, 0.0021503146, 0.0026172213, 0.0030295118, 0.0033755382, 0.0036450478, 0.0038294764, 
0.0039222008, 0.0039187544, 0.0038169876, 0.0036171766, 0.0033220719, 0.0029368866, 0.0024692214, 0.0019289269, 0.0013279071, 0.0006798662, 
0.0000000037, -0.0006953343, -0.0013890420, -0.0020636669, -0.0027018429, -0.0032867293, -0.0038024464, -0.0042344988, -0.0045701698, -0.0047988826, 
-0.0049125138, -0.0049056513, -0.0047757938, -0.0045234752, -0.0041523194, -0.0036690161, -0.0030832207, -0.0024073760, -0.0016564599, -0.0008476628, 
-0.0000000044, 0.0008661060, 0.0017293523, 0.0025680291, 0.0033605788, 0.0040861373, 0.0047250739, 0.0052595115, 0.0056738127, 0.0059550223, 
0.0060932497, 0.0060819834, 0.0059183259, 0.0056031416, 0.0051411176, 0.0045407247, 0.0038140842, 0.0029767412, 0.0020473439, 0.0010472420, 
0.0000000051, -0.0010691223, -0.0021338207, -0.0031673331, -0.0041431300, -0.0050355797, -0.0058206087, -0.0064763422, -0.0069836951, -0.0073269103, 
-0.0074940221, -0.0074772355, -0.0072732032, -0.0068832072, -0.0063132122, -0.0055738203, -0.0046800915, -0.0036512585, -0.0025103316, -0.0012835930, 
-0.0000000058, 0.0013094590, 0.0026125594, 0.0038765657, 0.0050690714, 0.0061588134, 0.0071164756, 0.0079154689, 0.0085326442, 0.0089489501, 
0.0091499826, 0.0091264462, 0.0088744834, 0.0083958777, 0.0076981243, 0.0067943512, 0.0057031014, 0.0044479761, 0.0030571397, 0.0015627055,
0.0000000066, -0.0015932253, -0.0031777592, -0.0047138180, -0.0061620641, -0.0074845944, -0.0086459164, -0.0096138809, -0.0103605585, -0.0108630145,
-0.0111039784, -0.0110723916, -0.0107637979, -0.0101805823, -0.0093320459, -0.0082343007, -0.0069099977, -0.0053878897, -0.0037022214, -0.0018919792,
-0.0000000074, 0.0019279920, 0.0038445480, 0.0057015745, 0.0074515631, 0.0090487795, 0.0104504414, 0.0116178496, 0.0125174252, 0.0131216487,
0.0134098632, 0.0133689195, 0.0129936449, 0.0122871213, 0.0112607647, 0.0099341962, 0.0083349012, 0.0064976942, 0.0044639818, 0.0022808532,
0.0000000083, -0.0023234491, -0.0046323230, -0.0068687163, -0.0089754639, -0.0108975749, -0.0125836469, -0.0139872134, -0.0150680011, -0.0157930441,
-0.0161376484, -0.0160861593, -0.0156325139, -0.0147805791, -0.0135442317, -0.0119471988, -0.0100226467, -0.0078125270, -0.0053666933, -0.0027417983,
-0.0000000091, 0.0027924452, 0.0055668503, 0.0082536750, 0.0107842907, 0.0130927050, 0.0151172476, 0.0168021955, 0.0180992652, 0.0189689659, 
0.0193817262, 0.0193188358, 0.0187730901, 0.0177492015, 0.0162638817, 0.0143456561, 0.0120343687, 0.0093803965, 0.0064435797, 0.0032919173, 
0.0000000099, -0.0033526700, -0.0066836728, -0.0099095702, -0.0129480157, -0.0157198142, -0.0181509554, -0.0201745518, -0.0217326302, -0.0227777548, 
-0.0232743807, -0.0231999699, -0.0225457978, -0.0213174149, -0.0195347723, -0.0172319971, -0.0144568030, -0.0112695461, -0.0077419719, -0.0039556241, 
-0.0000000108, 0.0040294821, 0.0080338521, 0.0119128758, 0.0155675802, 0.0189027246, 0.0218292400, 0.0242665578, 0.0261447765, 0.0274065901, 
0.0280089509, 0.0279244147, 0.0271421038, 0.0256682932, 0.0235265791, 0.0207576156, 0.0174184293, 0.0135813104, 0.0093323039, 0.0047693374, 
0.0000000115, -0.0048608845, -0.0096940901, -0.0143787423, -0.0187953524, -0.0228287894, -0.0263712276, -0.0293249544, -0.0316049978, -0.0331414789, 
-0.0338816233, -0.0337914154, -0.0328568071, -0.0310844351, -0.0285018906, -0.0251574181, -0.0211191420, -0.0164737534, -0.0113247475, -0.0057901731, 
-0.0000000123, 0.0059068096, 0.0117856823, 0.0174897779, 0.0228736401, 0.0277968105, 0.0321274064, 0.0357455462, 0.0385465845, 0.0404439718, 
0.0413717926, 0.0412868075, 0.0401699953, 0.0380275063, 0.0348910391, 0.0308176074, 0.0258886218, 0.0202084314, 0.0139021985, 0.0071132700, 
0.0000000129, -0.0072677629, -0.0145127513, -0.0215543639, -0.0282131005, -0.0343150087, -0.0396960713, -0.0442064852, -0.0477146432, -0.0501107797, 
-0.0513101667, -0.0512557402, -0.0499201193, -0.0473069809, -0.0434516259, -0.0384208672, -0.0323120952, -0.0252515897, -0.0173920952, -0.0089096921, 
-0.0000000135, 0.0091260737, 0.0182472840, 0.0271370802, 0.0355690867, 0.0433226489, 0.0501883402, 0.0559733845, 0.0605067573, 0.0636439025, 
0.0652709380, 0.0653081685, 0.0637129396, 0.0604815967, 0.0556505658, 0.0492965430, 0.0415356494, 0.0325216576, 0.0224432740, 0.0115204686, 
0.0000000140, -0.0118497815, -0.0237449631, -0.0353923626, -0.0464964285, -0.0567662828, -0.0659228712, -0.0737060159, -0.0798812136, -0.0842460766, 
-0.0866360292, -0.0869294927, -0.0850520432, -0.0809796229, -0.0747406632, -0.0664170906, -0.0561439656, -0.0441080704, -0.0305450857, -0.0157356095, 
-0.0000000144, 0.0163077731, 0.0328074731, 0.0491004735, 0.0647785366, 0.0794329569, 0.0926641226, 0.1040910482, 0.1133609042, 0.1201581880, 
0.1242133453, 0.1253106743, 0.1232953370, 0.1180791631, 0.1096453220, 0.0980514809, 0.0834315717, 0.0659958869, 0.0460295938, 0.0238896385, 
0.0000000147, -0.0251545217, -0.0510361716, -0.0770631433, -0.1026199013, -0.1270683557, -0.1497599483, -0.1700480878, -0.1873011142, -0.2009152472, 
-0.2103271782, -0.2150264978, -0.2145670950, -0.2085778266, -0.1967718154, -0.1789544970, -0.1550298929, -0.1250053793, -0.0889943987, -0.0472172648, 
-0.0000000148, 0.0522287525, 0.1089430526, 0.1695265621, 0.2332814932, 0.2994389832, 0.3671712577, 0.4356046319, 0.5038337708, 0.5709369779, 
0.6359910965, 0.6980877519, 0.7563483119, 0.8099397421, 0.8580884337, 0.9000939727, 0.9353414178, 0.9633120894, 0.9835928082, 0.9958829284, 
1.0000000000, 0.9958829284, 0.9835928082, 0.9633120894, 0.9353414178, 0.9000939727, 0.8580884337, 0.8099397421, 0.7563483119, 0.6980877519, 
0.6359910965, 0.5709369779, 0.5038337708, 0.4356046319, 0.3671712577, 0.2994389832, 0.2332814783, 0.1695265621, 0.1089430526, 0.0522287525, 
-0.0000000148, -0.0472172610, -0.0889943987, -0.1250053793, -0.1550298929, -0.1789544970, -0.1967718154, -0.2085778266, -0.2145670950, -0.2150264978,
 -0.2103271782, -0.2009152323, -0.1873011142, -0.1700480729, -0.1497599334, -0.1270683557, -0.1026199013, -0.0770631433, -0.0510361716, -0.0251545217, 
0.0000000147, 0.0238896385, 0.0460295901, 0.0659958869, 0.0834315643, 0.0980514735, 0.1096453145, 0.1180791631, 0.1232953370, 0.1253106743, 
0.1242133453, 0.1201581880, 0.1133608967, 0.1040910482, 0.0926641226, 0.0794329569, 0.0647785366, 0.0491004735, 0.0328074731, 0.0163077731, 
-0.0000000144, -0.0157356095, -0.0305450857, -0.0441080704, -0.0561439618, -0.0664170831, -0.0747406632, -0.0809796229, -0.0850520432, -0.0869294927,
-0.0866360292, -0.0842460692, -0.0798812136, -0.0737060085, -0.0659228712, -0.0567662828, -0.0464964248, -0.0353923589, -0.0237449612, -0.0118497806,
0.0000000140, 0.0115204686, 0.0224432722, 0.0325216576, 0.0415356494, 0.0492965393, 0.0556505620, 0.0604815930, 0.0637129396, 0.0653081611,
0.0652709305, 0.0636438951, 0.0605067573, 0.0559733845, 0.0501883402, 0.0433226489, 0.0355690867, 0.0271370783, 0.0182472840, 0.0091260737, 
-0.0000000135, -0.0089096911, -0.0173920952, -0.0252515879, -0.0323120952, -0.0384208672, -0.0434516259, -0.0473069809, -0.0499201193, -0.0512557365, 
-0.0513101630, -0.0501107797, -0.0477146395, -0.0442064814, -0.0396960713, -0.0343150049, -0.0282131005, -0.0215543639, -0.0145127513, -0.0072677629, 
0.0000000129, 0.0071132700, 0.0139021976, 0.0202084314, 0.0258886218, 0.0308176056, 0.0348910391, 0.0380275026, 0.0401699953, 0.0412868075, 
0.0413717888, 0.0404439680, 0.0385465808, 0.0357455462, 0.0321274064, 0.0277968086, 0.0228736401, 0.0174897779, 0.0117856814, 0.0059068091, 
-0.0000000123, -0.0057901726, -0.0113247465, -0.0164737534, -0.0211191401, -0.0251574162, -0.0285018887, -0.0310844332, -0.0328568034, -0.0337914117, 
-0.0338816196, -0.0331414789, -0.0316049978, -0.0293249525, -0.0263712257, -0.0228287876, -0.0187953506, -0.0143787414, -0.0096940892, -0.0048608845, 
0.0000000115, 0.0047693369, 0.0093323030, 0.0135813095, 0.0174184293, 0.0207576137, 0.0235265773, 0.0256682914, 0.0271421019, 0.0279244129, 
0.0280089490, 0.0274065882, 0.0261447746, 0.0242665578, 0.0218292382, 0.0189027227, 0.0155675793, 0.0119128749, 0.0080338512, 0.0040294817, 
-0.0000000108, -0.0039556236, -0.0077419709, -0.0112695461, -0.0144568020, -0.0172319971, -0.0195347723, -0.0213174131, -0.0225457959, -0.0231999699, 
-0.0232743789, -0.0227777530, -0.0217326302, -0.0201745499, -0.0181509536, -0.0157198124, -0.0129480148, -0.0099095693, -0.0066836723, -0.0033526698, 
0.0000000099, 0.0032919170, 0.0064435792, 0.0093803955, 0.0120343678, 0.0143456552, 0.0162638798, 0.0177492015, 0.0187730901, 0.0193188339, 
0.0193817243, 0.0189689640, 0.0180992652, 0.0168021936, 0.0151172457, 0.0130927041, 0.0107842898, 0.0082536740, 0.0055668494, 0.0027924450, 
-0.0000000091, -0.0027417981, -0.0053666928, -0.0078125270, -0.0100226458, -0.0119471978, -0.0135442298, -0.0147805782, -0.0156325120, -0.0160861574, 
-0.0161376484, -0.0157930423, -0.0150680002, -0.0139872115, -0.0125836460, -0.0108975740, -0.0089754630, -0.0068687154, -0.0046323226, -0.0023234489, 
0.0000000083, 0.0022808530, 0.0044639814, 0.0064976932, 0.0083349003, 0.0099341953, 0.0112607637, 0.0122871194, 0.0129936440, 0.0133689186, 
0.0134098614, 0.0131216468, 0.0125174243, 0.0116178486, 0.0104504405, 0.0090487786, 0.0074515622, 0.0057015740, 0.0038445476, 0.0019279917, 
-0.0000000074, -0.0018919790, -0.0037022210, -0.0053878892, -0.0069099967, -0.0082342997, -0.0093320450, -0.0101805814, -0.0107637960, -0.0110723907, 
-0.0111039774, -0.0108630136, -0.0103605576, -0.0096138800, -0.0086459154, -0.0074845934, -0.0061620637, -0.0047138175, -0.0031777588, -0.0015932252, 
0.0000000066, 0.0015627053, 0.0030571395, 0.0044479757, 0.0057031005, 0.0067943502, 0.0076981233, 0.0083958767, 0.0088744825, 0.0091264453, 
0.0091499817, 0.0089489492, 0.0085326433, 0.0079154680, 0.0071164751, 0.0061588124, 0.0050690710, 0.0038765653, 0.0026125591, 0.0013094587, 
-0.0000000058, -0.0012835928, -0.0025103313, -0.0036512581, -0.0046800906, -0.0055738194, -0.0063132113, -0.0068832063, -0.0072732023, -0.0074772346, 
-0.0074940212, -0.0073269089, -0.0069836942, -0.0064763413, -0.0058206078, -0.0050355792, -0.0041431296, -0.0031673326, -0.0021338202, -0.0010691222, 
0.0000000051, 0.0010472419, 0.0020473436, 0.0029767407, 0.0038140838, 0.0045407242, 0.0051411171, 0.0056031412, 0.0059183249, 0.0060819825, 
0.0060932487, 0.0059550214, 0.0056738118, 0.0052595106, 0.0047250735, 0.0040861368, 0.0033605783, 0.0025680286, 0.0017293521, 0.0008661058, 
-0.0000000044, -0.0008476627, -0.0016564596, -0.0024073757, -0.0030832202, -0.0036690156, -0.0041523185, -0.0045234747, -0.0047757933, -0.0049056504, 
-0.0049125128, -0.0047988822, -0.0045701694, -0.0042344984, -0.0038024459, -0.0032867286, -0.0027018425, -0.0020636667, -0.0013890418, -0.0006953342, 
0.0000000037, 0.0006798661, 0.0013279070, 0.0019289266, 0.0024692209, 0.0029368862, 0.0033220712, 0.0036171759, 0.0038169869, 0.0039187539, 
0.0039221998, 0.0038294757, 0.0036450471, 0.0033755375, 0.0030295111, 0.0026172209, 0.0021503144, 0.0016415166, 0.0011042902, 0.0005524870, 
-0.0000000031, -0.0005395957, -0.0010533396, -0.0015292220, -0.0019564428, -0.0023256536, -0.0026291530, -0.0028610390, -0.0030173135, -0.0030959330, 
-0.0030968157, -0.0030217962, -0.0028745346, -0.0026603818, -0.0023862091, -0.0020601996, -0.0016916170, -0.0012905492, -0.0008676407, -0.0004338146, 
0.0000000026, 0.0004231515, 0.0008254968, 0.0011976643, 0.0015312544, 0.0018190253, 0.0020550436, 0.0022347989, 0.0023552789, 0.0024150086, 
0.0024140461, 0.0023539446, 0.0022376764, 0.0020695226, 0.0018549357, 0.0016003742, 0.0013131184, 0.0010010685, 0.0006725336, 0.0003360166, 
-0.0000000021, -0.0003272733, -0.0006379773, -0.0009249060, -0.0011816258, -0.0014026165, -0.0015833839, -0.0017205446, -0.0018118804, -0.0018563626, 
-0.0018541457, -0.0018065333, -0.0017159141, -0.0015856749, -0.0014200894, -0.0012241884, -0.0010036158, -0.0007644714, -0.0005131475, -0.0002561631, 
0.0000000017, 0.0002490649, 0.0004850931, 0.0007026389, 0.0008968628, 0.0010636358, 0.0011996229, 0.0013023433, 0.0013702081, 0.0014025348, 
0.0013995386, 0.0013623021, 0.0012927236, 0.0011934467, 0.0010677741, 0.0009195659, 0.0007531283, 0.0005730934, 0.0003842952, 0.0001916433, 
-0.0000000013, -0.0001859453, -0.0003617753, -0.0005234589, -0.0006674339, -0.0007906830, -0.0008907928, -0.0009659953, -0.0010151931, -0.0010379665, 
-0.0010345632, -0.0010058720, -0.0009533818, -0.0008791251, -0.0007856117, -0.0006757511, -0.0005527674, -0.0004201095, -0.0002813583, -0.0001401329, 
0.0000000010, 0.0001356178, 0.0002635137, 0.0003807791, 0.0004848623, 0.0005736215, 0.0006453652, 0.0006988801, 0.0007334455, 0.0007488360, 
0.0007453102, 0.0007235887, 0.0006848210, 0.0006305421, 0.0005626217, 0.0004832056, 0.0003946533, 0.0002994715, 0.0002002458, 0.0000995738, 
-0.0000000007, -0.0000960500, -0.0001863189, -0.0002687758, -0.0003416557, -0.0004034954, -0.0004531595, -0.0004898575, -0.0005131513, -0.0005229540, 
-0.0005195191, -0.0005034219, -0.0004755335, -0.0004369880, -0.0003891442, -0.0003335429, -0.0002718617, -0.0002058668, -0.0001373660, -0.0000681603, 
0.0000000005, 0.0000654607, 0.0001266968, 0.0001823512, 0.0002312601, 0.0002724756, 0.0003052812, 0.0003292011, 0.0003440025, 0.0003496922, 
0.0003465064, 0.0003348959, 0.0003155055, 0.0002891499, 0.0002567856, 0.0002194810, 0.0001783840, 0.0001346897, 0.0000896074, 0.0000443291, 
-0.0000000003, -0.0000423105, -0.0000816298, -0.0001171064, -0.0001480246, -0.0001738173, -0.0001940744, -0.0002085459, -0.0002171416, -0.0002199256, 
-0.0002171083, -0.0002090334, -0.0001961634, -0.0001790616, -0.0001583728, -0.0001348026, -0.0001090959, -0.0000820155, -0.0000543214, -0.0000267508, 
0.0000000002, 0.0000252929, 0.0000485598, 0.0000693162, 0.0000871687, 0.0001018210, 0.0001130767, 0.0001208391, 0.0001251089, 0.0001259785, 
0.0001236248, 0.0001183000, 0.0001103202, 0.0001000535, 0.0000879075, 0.0000743152, 0.0000597224, 0.0000445745, 0.0000293043, 0.0000143209, 
-0.0000000001, -0.0000133252, -0.0000253695, -0.0000359017, -0.0000447472, -0.0000517894, -0.0000569697, -0.0000602846, -0.0000617831, -0.0000615614, 
-0.0000597567, -0.0000565412, -0.0000521142, -0.0000466946, -0.0000405129, -0.0000338040, -0.0000267996, -0.0000197216, -0.0000127762, -0.0000061488, 
0.0000000001, 0.0000055376, 0.0000103610, 0.0000143981, 0.0000176072, 0.0000199759, 0.0000215193, 0.0000222770, 0.0000223100, 0.0000216964, 
0.0000205281, 0.0000189056, 0.0000169346, 0.0000147213, 0.0000123690, 0.0000099747, 0.0000076258, 0.0000053984, 0.0000033552, 0.0000015445, 
-0.0000000000, -0.0000012594, -0.0000022285, -0.0000029148, -0.0000033366, -0.0000035213, -0.0000035030, -0.0000033205, -0.0000030145, -0.0000026259, 
-0.0000021933, -0.0000017516, -0.0000013304, -0.0000009526, -0.0000006344, -0.0000003844, -0.0000002042, -0.0000000886, -0.0000000267, -0.0000000034, 

};
*/

//
///
///////
//////////////
//////////////////
//////////////////
/////////
///////////
//////////////////////////
///////////////////
//////////
////////////8/8/8/8
//////////////*******
/////////////////////// // -9982
///////////////////////////////////
void SetSampleRate(unsigned int uiSampleRate)
{
    pVSTCollector->SetSampleRate((float)uiSampleRate);

    Eff* eff = first_eff;

    while(eff != NULL)
    {
        eff->SetSampleRate((float)uiSampleRate);
        eff = eff->next;
    }

    Instrument* instr = first_instr;

    while(instr != NULL)
    {
        if (instr->type == Instr_VSTPlugin)
        {
            ((VSTGenerator*) (instr))->pEff->SetSampleRate((float)uiSampleRate);
        }
        instr = instr->next;
    }
}
void SetBPM(float bpm)
{
    if (pVSTCollector != NULL)
    {
        pVSTCollector->SetBPM(bpm);
    }
}

bool Render_Start()
{
    long ullLastFrame;
    GetLastElementEndFrame(&ullLastFrame);

	if(ullLastFrame > 0 && Playing == false)
	{
		if(Rendering == false)
		{
			CP->PosToHome();

#ifdef USE_JUCE_AUDIO
            audioDeviceManager->removeAudioCallback(audioCallBack);
#else
			PortAudio_Stop();
#endif

			PlayMain();
			Rendering = true;
			pRenderer->SetConfig(&renderConfig);
			pRenderer->SetAudioLength(ullLastFrame);
			pRenderer->Open();
			pRenderer->Start(&Render_Callback);
		}
		else
		{
			pbkMain->UpdateQueuedEv();
			pRenderer->SetAudioLength(ullLastFrame);
			pRenderer->Resume();
		}
        return true;
	}
    else
        return false;
}

void Render_Stop()
{
    if (Rendering == true)
    {
        Rendering = false;
        pRenderer->Stop();
        pRenderer->Close();

#ifdef USE_JUCE_AUDIO
        audioDeviceManager->addAudioCallback(audioCallBack);
#else
        PortAudio_Start();
#endif
    }
    StopMain();
}

// Resets parameters being recorded to envelopes
void FinishParamsRecording()
{
    if(Recording == true)
    {
        Parameter* paramnext;
        Parameter* param = first_rec_param;
        while(param != NULL)
        {
            paramnext = param->rec_next;
            param->FinishRecording();
            param = paramnext;
        }
    }
}

// Completely stops params recording
void StopRecording()
{
    FinishParamsRecording();
    Recording = false;
    CP->Record->Release();
}

void PlayMain()
{
    if(Playing == false)  // Go playing
    {
        // "Playing selected"
        if(pbkMain->looped == true)
        {
            if(pbkMain->currFrame < pbkMain->rng_start_frame || pbkMain->currFrame > pbkMain->rng_end_frame)
            {
                pbkMain->SetCurrFrame(pbkMain->rng_start_frame);
                UpdatePerBaseFrame(pbkMain);
            }
        }
        pbkMain->UpdateQueuedEv();
        pbkMain->SetActive();
		if(aux_panel->workPt->OrigPt->autopatt == false)
		{
            pbkAux->SetInactive();
        }

        // Seamless envelopes support
        PreInitEnvelopes(pbkMain->currFrame, field_pattern, field_pattern->first_ev, true);
        PreInitSamples(pbkMain->currFrame, field_pattern, field_pattern->first_ev);

        Playing = true;
    }
    else  // Pause playing
    {
        Playing = false;
        if(Recording == true)
        {
            FinishParamsRecording();
        }

        MC->poso->stopTimer();
        pbkMain->SetInactive();
        pbkAux->SetInactive();

        // Align current frame to the last *seen* pos, not the last played.
        pbkMain->SetCurrFrame((long)pbkMain->currFrame_tsync);
		if(aux_panel->workPt->OrigPt->autopatt == false)
		{
            pbkAux->SetCurrFrame((long)pbkAux->currFrame_tsync);
        }

        ResetProcessing(false);
        ResetUnstableElements();
        CleanupAll();
    }
}

void StopMain(bool forceresetmix)
{
    bool resetmix = forceresetmix;
    if(resetmix == false && Playing == false && aux_panel->playing == false)
    {
        resetmix = true;
    }

    if(Playing == true || !aux_panel->isPlaying())
    {
        Playing = false;
        MC->poso->stopTimer();
        pbkMain->pactive = false;
        pbkMain->SetCurrFrame(0);
        currPlayX = 0;
        currPlayX_f = 0;

        if(Recording == true)
        {
            StopRecording();
        }

        Preview_StopAll();
        ResetProcessing(resetmix);
        CleanupAll();
        UpdateTime(Loc_MainGrid);
        AuxPos2MainPos();
    }
    else if(aux_panel->playing == true)
    {
        aux_panel->Stop();
    }

    // Seamless envelopes support
    PreInitEnvelopes(pbkMain->currFrame, field_pattern, field_pattern->first_ev, true);
}

void ResetPlacedEnvelopes()
{
    Command* cmd;
    Trigger* tg = first_active_command_trigger;
    while(tg != NULL)
    {
        cmd = (Command*)tg->el;
        if(cmd->cmdtype != Cmd_LocVolEnv && cmd->cmdtype != Cmd_LocPanEnv)
        {
            tg->Deactivate();
		}
        tg = tg->loc_act_next;
    }
}

// This function takes care of elements that can produce undesired effects per position change
void ResetUnstableElements()
{
    // Remove active effect-elements
    Trigger* tg = first_global_active_trigger;
    Trigger* tgnext;
    while(tg != NULL)
    {
        tgnext = tg->act_next;
        if(((tg->el->type == El_GenNote) && ((Instance*)tg->el)->instr->type == Instr_VSTPlugin)||
              tg->el->type == El_Mute ||
              tg->el->type == El_Vibrate ||
              tg->el->type == El_Slider ||
              tg->el->type == El_Transpose ||
              tg->el->type == El_SlideNote ||
              tg->el->type == El_Command) //  Envelopes will be preinited when playback restarts
        {
            tg->toberemoved = true;
        }
        tg = tgnext;
    }

    // Block instance triggers to prevent unneeded effects
    OutsyncTriggers();
}

void OutsyncTriggers()
{
    Trigger* tg = first_global_active_trigger;
    Trigger* tgnext;
    while(tg != NULL)
    {
        tgnext = tg->act_next;
        if(tg->el->IsInstance())
        {
            tg->outsync = true;
        }
        tg = tgnext;
    }
}

void ResetProcessing(bool resetmix)
{
    GlobalMute = true;
    if(resetmix)
    {
        MixMute = true;
		field_pattern->Reset();
		Trk* trk = first_trkdata;
		while(trk != NULL)
		{
			trk->fxstate.t_break = false;
			trk->fxstate.t_reversed = false;
			trk->fxstate.t_mutecount = 0;
			trk->envelopes = NULL;
			trk->fxstate.freqtrig = NULL;
			trk->fxstate.t_freq_mult = 1;

            trk = trk->next;
        }
        mAster.efftrig = NULL;
        mAster.envelopes = NULL;
        mAster.freq_mult = 1;

        Instrument* i = first_instr;
        while(i != NULL)
        {
            i->Reset();
            i = i->next;
        }
    }
}

void Place_Note(Instrument *instr, int note, float vol, unsigned long start_frame, unsigned long end_frame)
{
    aux_panel->PlaceNote(instr, note, vol, start_frame, end_frame);
}

void MidiToHost_AddNoteOn(Instrument* instr, int note, int vol)
{
    float notevol;
    if(vol <= 100)
        notevol = (float)vol/100;
    else
        notevol = float(vol - 100)/27.f*(VolRange - 1) + 1;

    // Let's handle recording stuff
    Pattern* pt = NULL;
    if(Recording)
    {
        if(Playing)
        {
            pt = field_pattern;
            if(!aux_panel->isBlank())
            {
                pt = aux_panel->workPt;
                C.SetPattern(pt, Loc_MainGrid);
            }
            else if(C.patt == field_pattern)
            {
                C.SetPos(Frame2Tick(pbkMain->currFrame), CLine);
                pt = CreateElement_Pattern(CTick, CTick + (float)ticks_per_beat, CLine, CLine, Patt_Pianoroll);
                C.SetPattern(pt, Loc_MainGrid);

                // Remove it from field
                C.type = CType_None;
                R(Refresh_KeyCursor);
                MC->ForceRefresh();
            }
            else
                pt = C.patt;

            //Place_Note(((Instance*)(PrevSlot[ic].trig.el))->instr,
            //        note, PrevSlot[ic].trig.vol_val, PrevSlot[ic].start_frame, pbMain->currFrame - pbAux->currFrame);

            if(instr->type == Instr_Sample)
            {
                // If this is a sample, then record it immediately, since it's released by itself, without
                // waiting for user to release the key on MIDI keyboard
                tframe frame1 = pbkMain->currFrame - C.patt->StartFrame();
                Grid_PutInstanceSpecific(C.patt, 
                                         instr, 
                                         note + 1, 
                                         notevol, 
                                         frame1,
                                         frame1 + 1);

                float newlength = Frame2Tick(frame1 + 4444);
                if(newlength > C.patt->tick_length)
                    C.patt->SetTickLength(newlength);

                R(Refresh_Grid);
                MC->poso->ScheduleRefresh();
                //MC->listen->CommonInputActions();
            }
        }
        else if(aux_panel->playing)
        {
            if(aux_panel->isBlank())
            {
                aux_panel->CreateNew();
            }
            pt = aux_panel->workPt;
        }
    }

    // Then preview the note
    int num = Preview_Add(NULL, instr, -1, note, pt, NULL, NULL);

    // Remember the frame and the volume, to use them after note is released
    if(num != 0xFFFF)
    {
        PrevSlot[num].ii->loc_vol->SetNormalValue(notevol);
        PrevSlot[num].start_frame = pbkMain->currFrame;
    }

    //if(Recording)
    //{
     //   Place_Note(instr, note, vol, pbMain->currFrame, pbMain->currFrame + 44100);
     //   R(Refresh_AuxGrid);
     //   MC->listen->CommonInputActions();
    //}
}

void MidiToHost_AddNoteOff(Instrument* instr, int note)
{
    Preview_Release(instr, note);
}

extern inline float MixPans(float src_pan, float dst_pan)
{
    return src_pan*(1 - abs(dst_pan)) + dst_pan;
}

inline bool IsCommandApplicable(Command* cmd, Instance* instance)
{
    if(cmd->scope->local == true)
    {
        if(cmd->patt == instance->patt)
        {
            if(cmd->scope->instr == instance->instr)
            {
                return true;
            }
        }
    }
    else if(cmd->scope->for_track == true)
    {
		if(cmd->field_trknum == instance->field_trknum)
        {
            if(cmd->scope->instr == instance->instr)
            {
                return true;
            }
        }
    }
    else if(cmd->scope->pt != NULL)
    {
        if(cmd->scope->pt == instance->patt)
        {
            if(cmd->scope->instr == instance->instr)
            {
                return true;
            }
        }
    }
    else
    {
        if(cmd->scope->instr == instance->instr)
        {
            return true;
        }
    }

    return false;
}

void StopPlacedEnvelopes()
{
    Trigger* tg = first_active_command_trigger;
    while(tg != NULL)
    {
        if(tg->el->IsPresent())
        {
           ((Envelope*)((Command*)tg->el)->paramedit)->newbuff = false;;
        }
        tg = tg->loc_act_next;
    }
}

void UpdatePerBaseFrame(Playback* pb)
{
    if(pb == pbkMain)
    {
        currPlayX_f = pbkMain->currFrame/framesPerPixel;
        currPlayX = (int)currPlayX_f;
        AuxPos2MainPos();
    }
    else if(pb == pbkAux)
    {
        aux_panel->curr_play_x_f = (pbkAux->currFrame - aux_panel->workPt->frame)/aux_panel->frames_per_pixel;
        aux_panel->curr_play_x = (int)aux_panel->curr_play_x_f;
        MainPos2AuxPos();
    }
}

INLINE float Gaussian_Interpol(float* Yi, float dX)
{
    float ret_val = 0;

//    double a = -1.0/12.0*Yi[2]-1.0/120.0*Yi[0]+1.0/24.0*Yi[1]+1.0/120.0*Yi[5]+1.0/12.0*Yi[3]-1.0/24.0*Yi[4];
//    double b = 13.0/12.0*Yi[2]+1.0/8.0*Yi[0]-7.0/12.0*Yi[1]-1.0/12.0*Yi[5]-Yi[3]+11.0/24.0*Yi[4];
//    double c = -59.0/12.0*Yi[2]-17.0/24.0*Yi[0]+71.0/24.0*Yi[1]+7.0/24.0*Yi[5]+49.0/12.0*Yi[3]-41.0/24.0*Yi[4];
//    double d = 107.0/12.0*Yi[2]+15.0/8.0*Yi[0]-77.0/12.0*Yi[1]-5.0/12.0*Yi[5]-13.0/2.0*Yi[3]+61.0/24.0*Yi[4];
//    double e = -137.0/60.0*Yi[0]+5.0*Yi[1]-5.0*Yi[2]+10.0/3.0*Yi[3]-5.0/4.0*Yi[4]+1.0/5.0*Yi[5];

    dX += 2;
    //ret_val = ((((a*dX + b)*dX + c)*dX + d)*dX + e)*dX + Yi[0];
    ret_val = (float)((((((-1.0/12.0*Yi[2]-1.0/120.0*Yi[0]+1.0/24.0*Yi[1]+1.0/120.0*Yi[5]+1.0/12.0*Yi[3]-1.0/24.0*Yi[4])*dX + \
					   (13.0/12.0*Yi[2]+1.0/8.0*Yi[0]-7.0/12.0*Yi[1]-1.0/12.0*Yi[5]-Yi[3]+11.0/24.0*Yi[4]))*dX + \
					   (-59.0/12.0*Yi[2]-17.0/24.0*Yi[0]+71.0/24.0*Yi[1]+7.0/24.0*Yi[5]+49.0/12.0*Yi[3]-41.0/24.0*Yi[4]))*dX + \
					   (107.0/12.0*Yi[2]+15.0/8.0*Yi[0]-77.0/12.0*Yi[1]-5.0/12.0*Yi[5]-13.0/2.0*Yi[3]+61.0/24.0*Yi[4]))*dX + \
					   (-137.0/60.0*Yi[0]+5.0*Yi[1]-5.0*Yi[2]+10.0/3.0*Yi[3]-5.0/4.0*Yi[4]+1.0/5.0*Yi[5]))*dX + Yi[0]);

    return ret_val;
}

INLINE float Sinc_Interpolate(float* Yi, double dX, unsigned int num)
{
    double ret_val = 0;
    unsigned char mid = num/2;
    int j = (int)(0 - mid);
    unsigned int i = 0;
//    unsigned int index1;
//    unsigned char offset;
//    float SincVal = 0.f;
//    bool interpolate = false;

//    offset = dX*20; // 0.05 = 1/20

//    if ( (float)(dX*20.f) > offset )
//    {
//        interpolate = true;
//    }

    for (; i < num; ++i)
    {
/*
        if (interpolate == true)
        {
            SincVal = Interpolate_Line(0, SincLookUp[i*20 + offset], 1, SincLookUp[i*20 + offset + 1], 0.5f);
        }
        else
        {
            SincVal = SincLookUp[i*20 + offset];
        }
*/
        //ret_val += Yi[i]*SincVal;
        //ret_val += Yi[i]*Sinc((double)(j - dX));
        ret_val += Yi[i]*SincWindowedBlackman((double)(j - dX), (double) (i - dX), num);
        ++j;
    }

    return (float)ret_val;
}

INLINE double Sinc(double a)
{
    return sin(PI*a)/(PI*a);
}

INLINE double SincWindowedBlackman(double a, double b, unsigned int num)
{
    return Sinc(a)*(0.42 - 0.5*cos((2.f*PI*b)/((float)num)) + 0.08*cos((4.f*PI*b)/((float)num)));
}

INLINE void GetMonoSampleData(Sample* sample, double cursor_pos, float* dataLR)
{
    float x1 = (float)(int)cursor_pos;
    float x2 = x1 + 1;

    float y1 = sample->sample_data[(int)x1];
    float y2;
    float  Y[6];

    if((int)x2 == sample->sample_info.frames)
    {
        y2 = 0;
    }
    else
    {
        y2 = sample->sample_data[(int)x2];
    }

    double x3 = cursor_pos;

    if (x1 == x3)
    {
        *dataLR = sample->sample_data[(int)x3];
    }
    else
    {
        unsigned int method = WorkingInterpolationMethod;
        if(Rendering == true)
            method = RenderInterpolationMethod;

        switch (method)
        {
            case Interpol_linear:
            {
               *dataLR = Interpolate_Line(x1, y1, x2, y2, x3);
            }
            break;

            case Interpol_3dHermit:
            {
                float dX = (float)cursor_pos - x1;
                float xm1 = x1 > 0 ? sample->sample_data[(int)(x1-1)] : 0;
                float _x0  = sample->sample_data[(int)x1];
                float _x1  = (x1+1) <= (sample->sample_info.frames -1) ? sample->sample_data[(int)(x1 + 1)] : sample->sample_data[(int)x1];
                float _x2  = (x1+2) <= (sample->sample_info.frames -1) ? sample->sample_data[(int)(x1 + 2)] : sample->sample_data[(int)x1];
//                float a = (3 * (_x0-_x1) - xm1 + _x2) / 2;
//                float b = 2*_x1 + xm1 - (5*_x0 + _x2) / 2;
//                float c = (_x1 - xm1) / 2;
                *dataLR = (((((3 * (_x0-_x1) - xm1 + _x2) / 2) * dX) +\
                          (2*_x1 + xm1 - (5*_x0 + _x2) / 2)) * dX + ((_x1 - xm1) / 2)) * dX + _x0;
            }
            break;

            case Interpol_6dHermit:
            {
                float dX = (float)cursor_pos - x1;
                int i = 0;
                unsigned int pX = (x1 > 1)? (unsigned int)x1-2: 0;

                for (i = 0; i<6;++i)
                {
                    if ((pX + i)< (sample->sample_info.frames -1))
                    {
                        Y[i] = sample->sample_data[(int)pX+i];
                    }
                    else
                    {
                        Y[i] = 0;
                    }
                }
                *dataLR = Gaussian_Interpol(Y, dX);
            }
            break;

            case Interpol_64Sinc:
            case Interpol_128Sinc:
            case Interpol_256Sinc:
            {
                double dX = cursor_pos - x1;
                unsigned int num = 64;
                if (method == Interpol_128Sinc)
                {
                    num = 128;
                }
                else if (method == Interpol_256Sinc)
                {
                    num = 256;
                }

                int uiLBorder;

                uiLBorder = int(x1 - num/2);

                for (unsigned int i = 0; i < num; ++i)
                {
                    if (((uiLBorder + i) >= 0) && ((uiLBorder + i) < sample->sample_info.frames))
                    {
                        Yi[i] = sample->sample_data[(int)(uiLBorder + i)];
                    }
                    else
                    {
                        Yi[i] = 0;
                    }
                }

                *dataLR = Sinc_Interpolate(Yi, dX, num);
//                char buf[100] = {0};

//                sprintf(buf, "%u %u %.08f %.08f %.08f %.08f",
//                    (int)x1, 
//                    (int)x2, 
//                    dX, sample->sample_data[(int)x1], *dataLR, sample->sample_data[(int)x2]);

//                DBG(buf);
            }
            break;

            default:
            {
            }
            break;
        }
    }
}

INLINE void GeStereoSampleData(Sample* sample, double cursor_pos, float* dataL, float* dataR)
{
    double x1, x2, x3;
	float y1, y2;
    float  Y[6];

    // process left
    x1 = (float)((int)cursor_pos);
    x2 = x1 + 1;
    x3 = cursor_pos;

    y1 = sample->sample_data[(int)x1*2];

    if((int)x2 >= sample->sample_info.frames)
    {
        y2 = 0;
    }
    else
    {
        y2 = sample->sample_data[(int)x2*2];
    }

    /* If current pos matches to sample point, don't need to interpolate (make CPU overhead) */
    if (x3 == x1)
    {
        *dataL = sample->sample_data[(int)x3*2];
    }
    else
    {
        unsigned int method = WorkingInterpolationMethod;
        if(Rendering == true)
            method = RenderInterpolationMethod;

        switch (method)
        {
            case Interpol_linear:
            {
                *dataL = Interpolate_Line(x1, y1, x2, y2, x3);
            }
            break;

            case Interpol_3dHermit:
            {
                float dX = (float)(cursor_pos - x1);
                float xm1 = x1 > 0 ? sample->sample_data[(int)(x1-1)*2] : 0;
                float _x0  = sample->sample_data[(int)x1*2];
                float _x1  = (x1+1) <= (sample->sample_info.frames -1) ? sample->sample_data[(int)(x1 + 1)*2] : sample->sample_data[(int)x1*2];
                float _x2  = (x1+2) <= (sample->sample_info.frames -1) ? sample->sample_data[(int)(x1 + 2)*2] : sample->sample_data[(int)x1*2];
//                float a = (3 * (_x0-_x1) - xm1 + _x2) / 2;
//                float b = 2*_x1 + xm1 - (5*_x0 + _x2) / 2;
//                float c = (_x1 - xm1) / 2;

                *dataL = (((((3 * (_x0-_x1) - xm1 + _x2) / 2) * dX) +\
                         (2*_x1 + xm1 - (5*_x0 + _x2) / 2)) * dX + ((_x1 - xm1) / 2)) * dX + _x0;
            }
            break;

            case Interpol_6dHermit:
            {
                float dX = (float)(cursor_pos - x1);
                int i = 0;
                unsigned int pX = (x1 > 1)? (unsigned int)x1-2: 0;

                for (i = 0; i<6;++i)
                {
                    if ((pX + i) < (sample->sample_info.frames -1))
                    {
                        Y[i] = sample->sample_data[(int)(pX+i)*2];
                    }
                    else
                    {
                        Y[i] = 0;
                    }
                }
                *dataL = Gaussian_Interpol(Y, dX);
            }
            break;

            case Interpol_64Sinc:
            case Interpol_128Sinc:
            case Interpol_256Sinc:
            {
                double dX = (cursor_pos - x1);
                unsigned int num = 64;
                if (method == Interpol_128Sinc)
                {
                    num = 128;
                }
                else if (method == Interpol_256Sinc)
                {
                    num = 256;
                }

                unsigned int uiLBorder;

                uiLBorder = int(x1 - num/2);

                for (unsigned int i = 0; i<num; ++i)
                {
                    if (((uiLBorder + i) >= 0) && ((uiLBorder + i) < sample->sample_info.frames))
                    {
                        Yi[i] = sample->sample_data[(int)(uiLBorder + i)*2];
                    }
                    else
                    {
                        Yi[i] = 0;
                    }
                }
                *dataL = Sinc_Interpolate(Yi,dX,num);
            }
            break;

            default:
            {
            }
            break;
        }
    }

    // process right
    y1 = sample->sample_data[(int)x1*2 + 1];

    if(((int)x2 + 1) >= sample->sample_info.frames)
    {
        y2 = 0;
    }
    else
    {
        y2 = sample->sample_data[(int)x2*2 + 1];
    }

    /* If current pos matches to sample point, don't need to interpolate (make CPU overhead) */
    if (x3 == x1)
    {
        *dataR = sample->sample_data[(int)x3*2 + 1];
    }
    else
    {
        unsigned int method = WorkingInterpolationMethod;
        if(Rendering == true)
            method = RenderInterpolationMethod;

        switch (method)
        {
            case Interpol_linear:
            {
                *dataR = Interpolate_Line(x1, y1, x2, y2, x3);
            }
            break;

            case Interpol_3dHermit:
            {
                float dX = (float)(cursor_pos - x1);
                float xm1 = x1 > 0 ? sample->sample_data[(int)(x1-1)*2 + 1] : 0;
                float _x0  = sample->sample_data[(int)x1*2 + 1];
                float _x1  = (x1+1) <= (sample->sample_info.frames -1) ? sample->sample_data[(int)(x1 + 1)*2 + 1] : sample->sample_data[(int)x1*2 + 1];
                float _x2  = (x1+2) <= (sample->sample_info.frames -1) ? sample->sample_data[(int)(x1 + 2)*2 + 1] : sample->sample_data[(int)x1*2 + 1];
//                float a = (3 * (_x0-_x1) - xm1 + _x2) / 2;
//                float b = 2*_x1 + xm1 - (5*_x0 + _x2) / 2;
//                float c = (_x1 - xm1) / 2;

               *dataR = (((((3 * (_x0-_x1) - xm1 + _x2) / 2) * dX) +\
                        (2*_x1 + xm1 - (5*_x0 + _x2) / 2)) * dX + ((_x1 - xm1) / 2)) * dX + _x0;
            }
            break;

            case Interpol_6dHermit:
            {
                float dX = (float)(cursor_pos - x1);
                int i = 0;
                unsigned int pX = (x1 > 1)? (unsigned int)x1-2: 0;

                for (i = 0; i<6;++i)
                {
                    if ((pX + i) < (sample->sample_info.frames -1))
                    {
                        Y[i] = sample->sample_data[(int)(pX+i)*2 + 1];
                    }
                    else
                    {
                        Y[i] = 0;
                    }
                }
                *dataR = Gaussian_Interpol(Y, dX);
            }
            break;

            case Interpol_64Sinc:
            case Interpol_128Sinc:
            case Interpol_256Sinc:
            {
                double dX = (cursor_pos - x1);
                unsigned int num = 64;
                if (method == Interpol_128Sinc)
                {
                    num = 128;
                }
                else if (method == Interpol_256Sinc)
                {
                    num = 256;
                }

                unsigned int uiLBorder;

                uiLBorder = int(x1 - num/2);

                for (unsigned int i = 0; i<num; ++i)
                {
                    if (((uiLBorder + i) >= 0) && ((uiLBorder + i) < sample->sample_info.frames))
                    {
                        Yi[i] = sample->sample_data[(int)(uiLBorder + i)*2 + 1];
                    }
                    else
                    {
                        Yi[i] = 0;
                    }
                }
                *dataR = Sinc_Interpolate(Yi, dX, num);
            }
            break;

            default:
            {
            }
            break;
        }
    }
}

void PortAudio_UpdateDevices(int out_device_idx, int in_device_idx)
{
    inputParameters.device = (PaDeviceIndex) in_device_idx;
    inputParameters.channelCount = 2;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    inputParameters.sampleFormat = paFloat32;

    if (in_device_idx != -1)
    {
        inputParameters.suggestedLatency =
            (Pa_GetDeviceInfo(inputParameters.device))->defaultLowInputLatency;
    }
    else
    {
        inputParameters.suggestedLatency = 0;
    }

    outputParameters.device = (PaDeviceIndex) out_device_idx;
    outputParameters.channelCount = 2;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = paFloat32;

    if (out_device_idx != -1)
    {
        outputParameters.suggestedLatency =
            (Pa_GetDeviceInfo(outputParameters.device))->defaultLowOutputLatency;
    }
    else
    {
        outputParameters.suggestedLatency = 0;
    }
}

PaError PortAudio_Start()
{
    PaError err = 0;
    PaStreamParameters *pInput  = NULL;
    PaStreamParameters *pOutput = NULL;

    if (outputParameters.device != -1)
    {
        pOutput = &outputParameters;

        if (inputParameters.device != -1)
        {
            pInput = &inputParameters;
        }

        /* Open an audio I/O stream. */
        err = Pa_OpenStream(&Stream,
                             pInput,
                             pOutput,
                             fSampleRate,
                             gBuffLen,
                             paNoFlag,
                             PortAudio_StreamCallback,
                             NULL);

        if(err == 0)
        {
            err = Pa_StartStream(Stream);
        }
        else
        {
            switch(err)
            {
                case paInvalidSampleRate:
                    MessageBox(0,"Selected sample rate is not supported by output device you have chosen",
                        "Config error", MB_OK|MB_ICONERROR);
                break;
                case paBadIODeviceCombination:
                    MessageBox(0,"Invalid combination of I/O devices",
                        "Config error", MB_OK|MB_ICONERROR);
                break;

                default:
                break;
            }
        }
    }
    else
    {
        err = paInvalidDevice;
    }

    return err;
}

PaError PortAudio_Stop()
{
    PaError err;

    err = Pa_AbortStream(Stream);
    err = Pa_CloseStream(Stream);

    return err;
}

PaError PortAudio_Init()
{
    PaError  err;

    // Initialize Port Audio
    err = Pa_Initialize();
    if(err == paNoError)
    {
        PortAudio_UpdateDevices((int)Pa_GetDefaultOutputDevice(), -1);
        err = PortAudio_Start();
    }
    return err;
}

PaError PortAudio_Deinit()
{
    PaError err;
    PortAudio_Stop();
    err = Pa_Terminate();

    return err;
}

void PortAudio_SetBufferSize(float BufSize)
{
#ifndef USE_JUCE_AUDIO
    if(PortAudio_Stop() == paNoError)
    {
        gBuffLen = (int)BufSize;
        pVSTCollector->SetBufferSize(BufSize);

        Eff* eff = first_eff;
        while(eff != NULL)
        {
            eff->SetBufferSize((unsigned int) BufSize);
            eff = eff->next;
        }

        Instrument* instr = first_instr;
        while(instr != NULL)
        {
            if (instr->type == Instr_VSTPlugin)
            {
                ((VSTGenerator*) (instr))->pEff->SetBufferSize((unsigned int) BufSize);
            }
            instr = instr->next;
        }

		/*
        memset(mix->m_cell.in_buff, 0, (MAX_BUFF_SIZE*2)*sizeof(float));
        for (int i = 0; i < NUM_MASTER_CELLS; ++i)
        {
            memset(mix->o_cell[i].in_buff, 0, (MAX_BUFF_SIZE*2)*sizeof(float));
        }*/

        PortAudio_Start();
    }
#endif
}

void JuceAudio_SetBufferSize(float BufSize)
{
#ifdef USE_JUCE_AUDIO
    //audioDeviceManager->stopDevice();
    gBuffLen = (int)BufSize;
    pVSTCollector->SetBufferSize(gBuffLen);

    Eff* eff = first_eff;
    while(eff != NULL)
    {
        eff->SetBufferSize(gBuffLen);
        eff = eff->next;
    }

    Instrument* instr = first_instr;
    while(instr != NULL)
    {
        if (instr->type == Instr_VSTPlugin)
        {
            ((VSTGenerator*) (instr))->pEff->SetBufferSize((unsigned int) BufSize);
        }
        instr = instr->next;
    }

    CAudioDeviceManager::AudioDeviceSetup setup;
    audioDeviceManager->getAudioDeviceSetup(setup);
    setup.bufferSize = int(BufSize);
    audioDeviceManager->setAudioDeviceSetup(setup, true);
#endif
}

RNDR_StreamCode Render_Callback(void *sample_buff, unsigned long frameCount)
{
    RNDR_StreamCode ret_val = RNDR_Continue;

    if (Playing != TRUE)
    {
        ret_val = RNDR_Complete;
    }
    else
    {
        //PortAudio_StreamCallback(NULL, sample_buff, frameCount, NULL, 0, NULL);
        CommonAudioCallback(NULL, sample_buff, frameCount);
    }

    return ret_val;
}

void CommonAudioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer)
{
    WaitForSingleObject(hAudioProcessMutex, INFINITE);

    // Cast data passed through stream to our structure.
    float          *out = (float*)outputBuffer;
    long            bc, fpb;
    const float    *in = (float*)inputBuffer;

    fpb = (long)framesPerBuffer;

    long baroffs = 0;
    long beatoffs = 0;
    // If metronome is enabled, check if we need to activate beat- or bar-sample
    if(Metronome_ON && (Playing == true || aux_panel->playing == true))
    {
        tframe frame = Playing == true ? pbkMain->currFrame : pbkAux->currFrame - pbkAux->playPatt->StartFrame();
        long fpbeat = long(frames_per_tick*ticks_per_beat);
        long fpbar = fpbeat*beats_per_bar;
        if(frame == 0 ||(frame/fpbar != (frame + fpb)/fpbar))
        {
            baroffs = (frame + fpb)/fpbar*fpbar - frame;
            barsample->ActivateTrigger(barsample->prevInst->tg_first);
        }
        else if(frame/fpbeat != (frame + fpb)/fpbeat)
        {
            beatoffs = (frame + fpb)/fpbeat*fpbeat - frame;
            beatsample->ActivateTrigger(beatsample->prevInst->tg_first);
        }
    }

    // Cleanup mixer buffers here
    aux_panel->CleanBuffers(fpb);

    jassert(NumPrevs >= 0);
    //if(NumPrevs > 0)
    {
        Preview_CheckStates(fpb);
    }

    if(aux_Pattern != pbkAux->playPatt)
    {
        pbkAux->SetPlayPatt(aux_Pattern);
        if(aux_panel->playing == true)
        {
            pbkAux->ResetPos();
            UpdatePerBaseFrame(pbkAux);
        }
    }

    if(Playing == true)
    {
        if(pbkMain->tsync_block == false)
            MC->poso->initTimer();
        UpdateTime(Loc_MainGrid);
    }
    else if(aux_panel->playing == true)
    {
        if(pbkAux->tsync_block == false)
            MC->poso->initTimer();
        UpdateTime(Loc_SmallGrid);
    }
    
    Instrument* instr;
    bool        changed = true;
    long        nc2 = 0;
    long        framestotick, nextframestotick;
    framestotick = fpb;
    if(Playing == true)
    {
        pbkMain->GetSmallestCountDown(&framestotick);
    }
    else if(aux_panel->playing == true)
    {
        pbkAux->GetSmallestCountDown(&framestotick);
    }
    
    Playback* pb = first_active_playback;
    while(pb != NULL)
    {
        pb->GetSmallestCountDown(&framestotick);
        pb = pb->next;
    }

    long    nc = 0;
    while(nc < fpb)
    {
        curr_frame = framestotick;
        nextframestotick = fpb - nc - framestotick;

        // Playback can go either on Main field or on Aux field, but not on both
        if(Playing == true)     // Main field is playing
        {
            pbkMain->TickFrame(nc, framestotick, fpb);
            pbkMain->GetSmallestCountDown(&nextframestotick);

            // Synchronize current frame in Aux, if not editing autopattern
            if(aux_panel->workPt->autopatt == false)
            {
                pbkAux->currFrame = pbkMain->currFrame;
            }

            // Stop on the last frame (disabled currently)
            //if(pbMain->currFrame >= lastFrame)
            //{
            //    const MessageManagerLock mmLock;
            //    CP->HandleButtDown(CP->play_butt);
            //    MC->listen->CommonInputActions();
            //}
        }
        else if(aux_panel->playing == true)  // Aux field is playing
        {
            pbkAux->TickFrame(nc, framestotick, fpb);
            pbkAux->GetSmallestCountDown(&nextframestotick);

            // Synchronize current frame in Main, if not editing autopattern
            if(aux_panel->workPt->autopatt == false)
            {
                pbkMain->currFrame = pbkAux->currFrame;
            }
        }

        pb = first_active_playback;
        while(pb != NULL)
        {
            pb->TickFrame(nc, framestotick, fpb);
            if(pb->dworking == false)
                DeactivatePlayback(pb);
            else
                pb->GetSmallestCountDown(&nextframestotick);
            pb = pb->next;
        }

        // Main instruments processing loop
        instr = first_instr;
        while(instr != NULL)
        {
            instr->GenerateData(framestotick, nc);
            instr = instr->next;
        }

        StopPlacedEnvelopes();

        nc += framestotick;
        framestotick = nextframestotick;
    }

    if(Metronome_ON && (Playing == true || aux_panel->playing == true))
    {
        barsample->GenerateData(fpb - baroffs, baroffs);
        beatsample->GenerateData(fpb - beatoffs, beatoffs);
    }

    // Active triggers cleanup
    Trigger* tg = first_global_active_trigger;
    Trigger* tgnext;
    while(tg != NULL)
    {
        tgnext = tg->act_next;
        if(tg->CheckRemoval())
        {
            if(tg->el->IsInstance() && // Smoothly finish playing notes
               (tg->tgstate == TgState_Sustain || tg->tgstate == TgState_Release))
            {
                tg->SoftFinish();
            }
            else
            {
                tg->Deactivate();
            }
        }
        tg = tgnext;
    }

    int fadenum = fpb >= 100 ? 100 : fpb;
    if(GlobalMute)
    {
        // Force active triggers cleanup, when need to fade out
        tg = first_global_active_trigger;
        while(tg != NULL)
        {
            if(MixMute)
                tg->Deactivate();
            else
            {
                if(tg->el->IsInstance())
                    tg->SoftFinish();
                else
                    tg->Deactivate();
            }
            tg = tg->act_next;
        }

        // Force deactivating active playbacks as well, if total muting
        if(MixMute)
        {
            pb = first_active_playback;
            while(pb != NULL)
            {
                DeactivatePlayback(pb);
                pb = pb->next;
            }
        }
        GlobalMute = false;
    }

    aux_panel->Mix(fpb);

    bool mutemixing = false;
    if(MixMute)
    {
        mutemixing = true;
        Eff* eff = first_eff;
        while(eff != NULL)
        {
            eff->Reset();
            eff = eff->next;
        }

        Instrument* instr = first_instr;
        while(instr != NULL)
        {
            if(instr->type == Instr_VSTPlugin)
            {
               ((VSTGenerator*)instr)->pEff->Reset();
            }
            instr = instr->next;
        }
    }

    Envelope* mvenv = NULL;
    Trigger* tgenv = mAster.params->vol->envelopes;
    if(tgenv != NULL && mAster.params->vol->envaffect)
    {
        mvenv = (Envelope*)((Command*)tgenv->el)->paramedit;
    }

    float vol = mAster.params->vol->outval;
    if(mAster.params->vol->lastval == -1)
    {
        mAster.params->vol->SetLastVal(mAster.params->vol->outval);
    }
    else if(mAster.params->vol->lastval != mAster.params->vol->outval)
    {
        if(mAster.rampCounterV == 0)
        {
            mAster.cfsV = float(mAster.params->vol->outval - mAster.params->vol->lastval)/RAMP_COUNT;
            vol = mAster.params->vol->lastval;
            mAster.rampCounterV = RAMP_COUNT;
        }
        else
        {
            vol = mAster.params->vol->lastval + (RAMP_COUNT - mAster.rampCounterV)*mAster.cfsV;
        }
    }
    else if(mAster.rampCounterV > 0) // (params->vol->lastval == params->vol->outval)
    {
        mAster.rampCounterV = 0;
        mAster.cfsV = 0;
    }

    float outL, outR, lMax, rMax;
    lMax = rMax = 0;
    bc = 0;
    if(in == NULL)
    {
        for(nc = 0; nc < fpb; nc++)
        {
            if(mvenv != NULL && nc >= mvenv->last_buffframe && mAster.rampCounterV == 0)
            {
                vol = mvenv->buffoutval[nc];
                if(nc == mvenv->last_buffframe_end - 1)
                {
                    mAster.params->vol->SetValueFromEnvelope(mvenv->buff[nc], mvenv);
                    mAster.params->vol->SetLastVal(mAster.params->vol->outval);
                }
            }

            if(mAster.rampCounterV > 0)
            {
                vol += mAster.cfsV;
                mAster.rampCounterV--;
                if(mAster.rampCounterV == 0)
                {
                    mAster.params->vol->SetLastVal(mAster.params->vol->outval);
                }
            }
            outL = aux_panel->masterchan.in_buff[bc++]*vol;
            outR = aux_panel->masterchan.in_buff[bc++]*vol;

            if(outL > lMax)
                lMax = outL;
            if(outR > rMax)
                rMax = outR;

           *out++ = outL;
           *out++ = outR;
        }

        CP->MVol->vu->SetLR(lMax, rMax);
        if(aux_panel->masterchan.p_vu->drawarea->isEnabled())
        {
            aux_panel->masterchan.p_vu->SetLR(lMax, rMax);
        }
    }

    if(mutemixing)
        MixMute = false;

    ReleaseMutex(hAudioProcessMutex);
}

/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int PortAudio_StreamCallback(const void   *inputBuffer,
                                          void   *outputBuffer,
                                          unsigned long    framesPerBuffer,
                                          const PaStreamCallbackTimeInfo*   timeInfo,
                                          PaStreamCallbackFlags    statusFlags,
                                          void   *userData)
{
    CommonAudioCallback(inputBuffer, outputBuffer, framesPerBuffer);

    return paContinue;
}

#define Params

/*
SoundFont::SoundFont(sfBankID bankid)
{
    bank_id = bankid;
    type = Instr_SoundFont;
}*/

