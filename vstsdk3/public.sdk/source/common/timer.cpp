//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : Common Base Classes
// Filename    : timer.cpp
// Created by  : Steinberg, 01/2004
// Description : Basic Timer class Implementation
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
#include "timer.h"
#endif

#if WINDOWS
#include <windows.h>
#include <list>
static std::list<Timer*> gTimerList;
#endif

#if MAC
#include <Carbon/Carbon.h>
#include <algorithm>
#endif

//-----------------------------------------------------------------------------
Timer::Timer (ITimer* timerObject, int32 fireTime)
: fireTime (fireTime)
, timerObject (timerObject)
, platformTimer (0)
{
}

//-----------------------------------------------------------------------------
Timer::~Timer ()
{
	stop ();
}

//-----------------------------------------------------------------------------
bool Timer::start ()
{
	if (platformTimer == 0)
	{
	#if MAC
		InstallEventLoopTimer (GetMainEventLoop (), std::max (kEventDurationMillisecond * fireTime, kEventDurationMillisecond * 400), kEventDurationMillisecond * fireTime, timerProc, this, (EventLoopTimerRef*)&platformTimer);
	#elif WINDOWS
		platformTimer = (void*)SetTimer ((HWND)NULL, (UINT_PTR)this, fireTime, TimerProc);
		if (platformTimer)
			gTimerList.push_back (this);
	#endif
	}
	return (platformTimer != 0);
}

//-----------------------------------------------------------------------------
bool Timer::stop ()
{
	if (platformTimer)
	{
	#if MAC
		RemoveEventLoopTimer ((EventLoopTimerRef)platformTimer);
	#elif WINDOWS
		KillTimer ((HWND)NULL, (UINT_PTR)platformTimer);
		std::list<Timer*>::iterator it = gTimerList.begin ();
		while (it != gTimerList.end ())
		{
			if ((*it) == this)
			{
				gTimerList.remove (*it);
				break;
			}
			it++;
		}
	#endif
		platformTimer = 0;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
bool Timer::setFireTime (int32 newFireTime)
{
	if (fireTime != newFireTime)
	{
		bool wasRunning = stop ();
		fireTime = newFireTime;
		if (wasRunning)
			return start ();
		return true;
	}
	return false;
}

#if MAC
//-----------------------------------------------------------------------------
pascal void Timer::timerProc (EventLoopTimerRef inTimer, void* inUserData)
{
	Timer* timer = (Timer*)inUserData;
	if (timer->timerObject)
		timer->timerObject->onTimer (timer);
}

#elif WINDOWS
//------------------------------------------------------------------------
#if (_MSC_VER >= 1300)
VOID CALLBACK Timer::TimerProc (HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
#else
VOID CALLBACK Timer::TimerProc (HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
#endif
{
	std::list<Timer*>::iterator it = gTimerList.begin ();
	while (it != gTimerList.end ())
	{
		if ((UINT_PTR)((*it)->platformTimer) == idEvent)
		{
			(*it)->timerObject->onTimer ((*it));
			break;
		}
		it++;
	}
}
#endif
