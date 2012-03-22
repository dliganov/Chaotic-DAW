//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : Common Base Classes
// Filename    : timer.h
// Created by  : Steinberg, 05/2007
// Description : Basic Timer class
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

#ifndef __timer__
#define __timer__

#include "pluginterfaces/base/ftypes.h"

#if MAC
#include <Carbon/Carbon.h>
#endif

#if WINDOWS
#include <windows.h>
#endif

namespace Steinberg {

class Timer;

//-----------------------------------------------------------------------------
// ITimer Interface Declaration
//-----------------------------------------------------------------------------
class ITimer
{
public:
	virtual void onTimer (Timer* timer) = 0;
};

//-----------------------------------------------------------------------------
// Timer Declaration
//! A timer class, which posts timer messages to ITimer objects
//-----------------------------------------------------------------------------
class Timer
{
public:
	Timer (ITimer* timerObject, int32 fireTime = 100);
	virtual ~Timer ();
	
	virtual bool start ();							///< starts the timer
	virtual bool stop ();							///< stops the timer, returns wheather timer was running or not

	virtual bool setFireTime (int32 newFireTime);	///< in milliseconds

//-----------------------------------------------------------------------------
protected:
	int32 fireTime;
	ITimer* timerObject;

	void* platformTimer;

#if MAC
	static pascal void timerProc (EventLoopTimerRef inTimer, void* inUserData);
#elif WINDOWS

#if (_MSC_VER >= 1300)
	static VOID CALLBACK TimerProc (HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
#else
	static VOID CALLBACK TimerProc (HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);
#endif
#endif
};

} // end namespace

using namespace Steinberg;

#endif
