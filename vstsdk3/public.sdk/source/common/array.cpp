//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : Common Base Classes
// Filename    : array.cpp
// Created by  : Steinberg, 01/2004
// Description : Basic array class
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

#include "array.h"

#include <stdlib.h>


//-----------------------------------------------------------------------------
// CArray
//-----------------------------------------------------------------------------
CArray::CArray (int32 delta)
: size  (0)
, count (0)
, delta (delta)
, elements (0)
{}

//-----------------------------------------------------------------------------
CArray::~CArray ()
{ 
	removeAll (); 
}

//-----------------------------------------------------------------------------
bool CArray::add (CArrayElement* e)
{
	if (count + 1 > size)
		if (!resize (count + 1))
			return false;

	elements[count++] = e;
	return true;
}

//-----------------------------------------------------------------------------
bool CArray::removeAt (int32 index)
{
	if (index < 0 || index >= count)
		return false;

	if (elements[index])
		delete elements[index];

	for (int32 i = index + 1; i < count; i++)
		elements[i - 1] = elements[i];
	count--;

	return true;
}

//-----------------------------------------------------------------------------
void CArray::removeAll ()
{
	for (int32 i = 0; i < count; i++)
		if (elements[i])
			delete elements[i];

	resize (0);
}

//-----------------------------------------------------------------------------
CArrayElement* CArray::at (int32 index) const
{
	return index >= 0 && index < count ? elements[index] : 0;
}

//-----------------------------------------------------------------------------
bool CArray::resize (int32 newSize)
{
	if (newSize <= 0)
	{
		if (elements)
			free (elements);
		elements = 0;
		size = count = 0;
		return true;
	}

	newSize = ((newSize / delta) + 1) * delta;
	
	void* newElements = 0;
	if (elements)
		newElements = realloc (elements, newSize * sizeof (CArrayElement*));
	else
		newElements = malloc (newSize * sizeof (CArrayElement*));

	if (!newElements)
		return false;

	size = newSize;
	elements = (CArrayElement**)newElements;
	return true;
}
