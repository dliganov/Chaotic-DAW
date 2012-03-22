//------------------------------------------------------------------------
// Project     : VST SDK
// Version     : 3.0
//
// Category    : Interfaces
// Filename    : vsttypes.h
// Created by  : Steinberg, 12/2005
// Description : common defines
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

#ifndef __vsttypes__
#define __vsttypes__

#include "pluginterfaces/base/ftypes.h"
#include "pluginterfaces/base/ustring.h"

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
#ifndef kVstVersionString
#define kVstVersionString	"VST 3.0.1"	///< SDK version for PClassInfo2
#endif

//------------------------------------------------------------------------
/** \defgroup vst3typedef VST3 Data Types
*/
/*@{*/
//------------------------------------------------------------------------
// string types
//------------------------------------------------------------------------
typedef char16 TChar;			///< UTF-16 character
typedef TChar String128[128];	///< 128 character UTF-16 string
typedef const char8* CString;	///< C-String

//------------------------------------------------------------------------
// general 
//------------------------------------------------------------------------
typedef int32 MediaType;		///< media type
typedef int32 BusDirection;		///< bus direction
typedef int32 BusType;			///< bus type
typedef int32 IoMode;			///< I/O mode
typedef int32 UnitID;			///< unit identifier
typedef double ParamValue;		///< parameter value type
typedef uint32 ParamID;			///< parameter identifier
typedef int32 ProgramListID;	///< program list identifier
typedef int16 CtrlNumber;		///< MIDI controller number

typedef double TQuarterNotes;	///< time expressed in quarter notes
typedef int64 TSamples;			///< time expressed in audio samples

//------------------------------------------------------------------------
// audio types
//------------------------------------------------------------------------
typedef float Sample32;				///< 32-bit precision audio sample
typedef double Sample64;			///< 64-bit precision audio sample
typedef double SampleRate;			///< sample rate
/*@}*/

//------------------------------------------------------------------------
/** \defgroup speakerArrangements Speaker Arrangements
\image html "vst3_speaker_types.jpg" 
\n
A SpeakerArrangement is a bitset combination of speakers. For example: 
\code
const SpeakerArrangement kStereo = kSpeakerL | kSpeakerR; // => hex: 0x03 / binary: 0011. 
\endcode*/
//------------------------------------------------------------------------
/**@{*/
typedef uint64 SpeakerArrangement;	///< Bitset of speakers 
typedef uint64 Speaker;	            ///< Bit for one speaker 
/**@}*/

//------------------------------------------------------------------------
/** Speaker Definitions.
\ingroup speakerArrangements */ 
//------------------------------------------------------------------------
/**@{*/
const Speaker kSpeakerL    = 1 << 0;		///< Left (L)
const Speaker kSpeakerR    = 1 << 1;		///< Right (R)
const Speaker kSpeakerC    = 1 << 2;		///< Center (C)
const Speaker kSpeakerLfe  = 1 << 3;		///< Subbass (Lfe)
const Speaker kSpeakerLs   = 1 << 4;		///< Left Surround (Ls)
const Speaker kSpeakerRs   = 1 << 5;		///< Right Surround (Rs)
const Speaker kSpeakerLc   = 1 << 6;		///< Left of Center (Lc)
const Speaker kSpeakerRc   = 1 << 7;		///< Right of Center (Rc)
const Speaker kSpeakerS    = 1 << 8;		///< Surround (S)
const Speaker kSpeakerCs   = kSpeakerS;	///< Center of Surround (Cs) = Surround (S)
const Speaker kSpeakerSl   = 1 << 9;		///< Side Left (Sl)
const Speaker kSpeakerSr   = 1 << 10;		///< Side Right (Sr)
const Speaker kSpeakerTm   = 1 << 11;		///< Top Middle - Center Over-head (Tm)
const Speaker kSpeakerTfl  = 1 << 12;		///< Top Front Left (Tfl)
const Speaker kSpeakerTfc  = 1 << 13;		///< Top Front Center (Tfc)
const Speaker kSpeakerTfr  = 1 << 14;		///< Top Front Right (Tfr)
const Speaker kSpeakerTrl  = 1 << 15;		///< Top Rear Left (Trl)
const Speaker kSpeakerTrc  = 1 << 16;		///< Top Rear Center (Trc)
const Speaker kSpeakerTrr  = 1 << 17;		///< Top Rear Right (Trr)
const Speaker kSpeakerLfe2 = 1 << 18;		///< Subbass 2 (Lfe2)
const Speaker kSpeakerM    = 1 << 19;		///< Mono (M)
	
const Speaker kSpeakerW	 = 1 << 20;		///< B-Format W
const Speaker kSpeakerX	 = 1 << 21;		///< B-Format X
const Speaker kSpeakerY	 = 1 << 22;		///< B-Format Y
const Speaker kSpeakerZ	 = 1 << 23;		///< B-Format Z

const Speaker kSpeakerTsl = 1 << 24;		///< Top Side Left (Tsl)
const Speaker kSpeakerTsr = 1 << 25;		///< Top Side Right (Tsr)
const Speaker kSpeakerLcs  = 1 << 26;		///< Left of Center Surround (Lcs)
const Speaker kSpeakerRcs  = 1 << 27;		///< Right of Center Surround (Rcs)
//------------------------------------------------------------------------
/** @}*/

//------------------------------------------------------------------------
/** Speaker Arrangement Definitions (SpeakerArrangement)*/ 
//------------------------------------------------------------------------
namespace SpeakerArr
{
//------------------------------------------------------------------------
/** Speaker Arrangement Definitions.
\ingroup speakerArrangements */
/*@{*/
const SpeakerArrangement kEmpty			 = 0;          ///< empty arrangement
const SpeakerArrangement kMono			 = kSpeakerM;  ///< M
const SpeakerArrangement kStereo		 = kSpeakerL  | kSpeakerR;    ///< L R
const SpeakerArrangement kStereoSurround = kSpeakerLs | kSpeakerRs;	  ///< Ls Rs
const SpeakerArrangement kStereoCenter	 = kSpeakerLc | kSpeakerRc;   ///< Lc Rc
const SpeakerArrangement kStereoSide	 = kSpeakerSl | kSpeakerSr;	  ///< Sl Sr
const SpeakerArrangement kStereoCLfe	 = kSpeakerC  | kSpeakerLfe;  ///< C Lfe
const SpeakerArrangement k30Cine		 = kSpeakerL  | kSpeakerR | kSpeakerC;  ///< L R C
const SpeakerArrangement k30Music		 = kSpeakerL  | kSpeakerR | kSpeakerS;  ///< L R S
const SpeakerArrangement k31Cine		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLfe;  ///< L R C   Lfe
const SpeakerArrangement k31Music		 = kSpeakerL  | kSpeakerR | kSpeakerLfe | kSpeakerS;    ///< L R Lfe S
const SpeakerArrangement k40Cine		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerS;    ///< L R C   S (LCRS)
const SpeakerArrangement k40Music		 = kSpeakerL  | kSpeakerR | kSpeakerLs  | kSpeakerRs;   ///< L R Ls  Rs (Quadro)
const SpeakerArrangement k41Cine		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLfe | kSpeakerS;	 ///< L R C   Lfe S (LCRS+Lfe)
const SpeakerArrangement k41Music		 = kSpeakerL  | kSpeakerR | kSpeakerLfe | kSpeakerLs  | kSpeakerRs;  ///< L R Lfe Ls Rs (Quadro+Lfe)
const SpeakerArrangement k50			 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLs  | kSpeakerRs;	 ///< L R C   Ls Rs 
const SpeakerArrangement k51			 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLfe | kSpeakerLs | kSpeakerRs;  ///< L R C  Lfe Ls Rs
const SpeakerArrangement k60Cine		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLs  | kSpeakerRs | kSpeakerCs;  ///< L R C  Ls  Rs Cs
const SpeakerArrangement k60Music		 = kSpeakerL  | kSpeakerR | kSpeakerLs  | kSpeakerRs  | kSpeakerSl | kSpeakerSr;  ///< L R Ls Rs  Sl Sr 
const SpeakerArrangement k61Cine		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLfe | kSpeakerLs | kSpeakerRs | kSpeakerCs;  ///< L R C   Lfe Ls Rs Cs
const SpeakerArrangement k61Music		 = kSpeakerL  | kSpeakerR | kSpeakerLfe | kSpeakerLs  | kSpeakerRs | kSpeakerSl | kSpeakerSr;  ///< L R Lfe Ls  Rs Sl Sr 
const SpeakerArrangement k70Cine		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLs  | kSpeakerRs | kSpeakerLc | kSpeakerRc;  ///< L R C   Ls  Rs Lc Rc 
const SpeakerArrangement k70Music		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLs  | kSpeakerRs | kSpeakerSl | kSpeakerSr;  ///< L R C   Ls  Rs Sl Sr
const SpeakerArrangement k71Cine		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLfe | kSpeakerLs | kSpeakerRs | kSpeakerLc | kSpeakerRc; 	///< L R C Lfe Ls Rs Lc Rc
const SpeakerArrangement k71Music		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLfe | kSpeakerLs | kSpeakerRs | kSpeakerSl | kSpeakerSr;	///< L R C Lfe Ls Rs Sl Sr
const SpeakerArrangement k80Cine		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLs  | kSpeakerRs | kSpeakerLc | kSpeakerRc | kSpeakerCs;  ///< L R C Ls  Rs Lc Rc Cs
const SpeakerArrangement k80Music		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLs  | kSpeakerRs | kSpeakerCs | kSpeakerSl | kSpeakerSr;	///< L R C Ls  Rs Cs Sl Sr
const SpeakerArrangement k81Cine		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLfe | kSpeakerLs | kSpeakerRs | kSpeakerLc | kSpeakerRc | kSpeakerCs;	 ///< L R C Lfe Ls Rs Lc Rc Cs
const SpeakerArrangement k81Music		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLfe | kSpeakerLs | kSpeakerRs | kSpeakerCs | kSpeakerSl | kSpeakerSr;	 ///< L R C Lfe Ls Rs Cs Sl Sr 
const SpeakerArrangement k102			 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLfe | kSpeakerLs | kSpeakerRs	| kSpeakerTfl | kSpeakerTfc | kSpeakerTfr | kSpeakerTrl | kSpeakerTrr | kSpeakerLfe2;	///< L R C Lfe Ls Rs Tfl Tfc Tfr Trl Trr Lfe2
const SpeakerArrangement k102Plus		 = kSpeakerL  | kSpeakerR | kSpeakerC   | kSpeakerLfe | kSpeakerLs | kSpeakerRs	| kSpeakerLc | kSpeakerRc | kSpeakerTfl | kSpeakerTfc | kSpeakerTfr | kSpeakerTrl | kSpeakerTrr | kSpeakerLfe2;	///< L R C Lfe Ls Rs Lc Rc Tfl Tfc Tfr Trl Trr Lfe2
const SpeakerArrangement kCube			 = kSpeakerL  | kSpeakerR | kSpeakerLs  | kSpeakerRs  | kSpeakerTfl | kSpeakerTfr | kSpeakerTrl | kSpeakerTrr;	///< L R Ls Rs Tfl Tfr Trl Trr
const SpeakerArrangement kBFormat1stOrder = kSpeakerW  | kSpeakerX | kSpeakerY   | kSpeakerZ;  ///< W X Y Z (First Order)
const SpeakerArrangement kBFormat = kBFormat1stOrder;

const SpeakerArrangement k71CineTopCenter  = kSpeakerL | kSpeakerR | kSpeakerC  | kSpeakerLfe | kSpeakerLs | kSpeakerRs | kSpeakerCs | kSpeakerTm; 		///< L R C Lfe Ls Rs Cs Tm
const SpeakerArrangement k71CineCenterHigh = kSpeakerL | kSpeakerR | kSpeakerC  | kSpeakerLfe | kSpeakerLs | kSpeakerRs | kSpeakerCs | kSpeakerTfc; 	///< L R C Lfe Ls Rs Cs Tfc
const SpeakerArrangement k71CineFrontHigh  = kSpeakerL | kSpeakerR | kSpeakerC  | kSpeakerLfe | kSpeakerLs | kSpeakerRs | kSpeakerTfl | kSpeakerTfr; 	///< L R C Lfe Ls Rs Tfl Tfl
const SpeakerArrangement k71CineSideHigh   = kSpeakerL | kSpeakerR | kSpeakerC  | kSpeakerLfe | kSpeakerLs | kSpeakerRs | kSpeakerTsl | kSpeakerTsr; 	///< L R C Lfe Ls Rs Tsl Tsl
const SpeakerArrangement k71CineSideFill   = k61Music;
const SpeakerArrangement k71CineFullRear   = kSpeakerL | kSpeakerR | kSpeakerC  | kSpeakerLfe | kSpeakerLs | kSpeakerRs | kSpeakerLcs | kSpeakerRcs; 	///< L R C Lfe Ls Rs Lcs Rcs
const SpeakerArrangement k71CineFullFront  = k71Cine;

/** Returns number of channels used in speaker arrangement. */
inline int32 getChannelCount (SpeakerArrangement arr)
{
	int32 count = 0;
	while (arr)
	{
		if (arr & SpeakerArrangement(1))
			++count;
		arr >>= 1;
	}
	return count;
}

/*@}*/

//------------------------------------------------------------------------
} // namespace SpeakerArr

} // namespace Vst
} // namespace Steinberg


#endif	// __vsttypes__
