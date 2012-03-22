//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : SDK Core Interfaces
// Filename    : fplatform.h
// Created by  : Steinberg, 01/2004
// Description : Detect platform and set define
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
//------------------------------------------------------------------------------

#ifndef __fplatform__
#define __fplatform__

#define kLittleEndian 0
#define kBigEndian 1

#undef WINDOWS
#undef MAC
#undef MOTIF
#undef BEOS

#if defined (_WIN32)						// WIN32 AND WIN64
	#define WINDOWS 1
	#define BYTEORDER kLittleEndian

	// deprecated insecure calls (strcpy, memset etc)
	#pragma warning (disable : 4996)

	#if defined (_WIN64)					// WIN64 only
		#define PLATFORM_64 1
	#else
		#ifndef WIN32_LEAN_AND_MEAN			// WIN32 only
			// make windows.h smaller
			#define WIN32_LEAN_AND_MEAN 1 
		#endif
	#endif
	#ifndef WIN32
		#define WIN32	1
	#endif
#elif __UNIX__                              // SGI IRIX
	#define MOTIF 1
	#if LINUX
		#define BYTEORDER kLittleEndian
	#else
		#define BYTEORDER kBigEndian
	#endif
#elif __INTEL__                             // BeOS
	#define BEOS 1
	#define BYTEORDER kLittleEndian
#else                                       // Mac
	#define MAC 1
	#define PTHREADS 1
	#ifndef __CF_USE_FRAMEWORK_INCLUDES__
	#define __CF_USE_FRAMEWORK_INCLUDES__
	#endif
	#ifndef TARGET_API_MAC_CARBON
	#define TARGET_API_MAC_CARBON 1
	#endif
	#if __LP64__
		#define PLATFORM_64 1
	#endif
	#if __MWERKS__
		#define BYTEORDER kBigEndian
	#else
		#if defined (__BIG_ENDIAN__)
			#define BYTEORDER kBigEndian
		#else
			#define BYTEORDER kLittleEndian
		#endif
	#endif
#endif

#endif
