//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : Common Base Classes
// Filename    : array.h
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

#ifndef __array__
#define __array__

#include "pluginterfaces/base/ftypes.h"

namespace Steinberg {

//------------------------------------------------------------------------
#define ARRAY_FOREACH(Type, obj, array) \
{ for (int32 __index = 0; __index < (array).count (); __index++) { \
  Type* obj = (Type*)(array).at (__index);

#define ARRAY_FOREACH_REVERSE(Type, obj, array) \
{ for (int32 __index = (array).count ()-1; __index >= 0; __index--) { \
  Type* obj = (Type*)(array).at (__index);

#define ARRAY_ENDFOR }}

//-----------------------------------------------------------------------------
//  CArrayElement class declaration
//-----------------------------------------------------------------------------
class CArrayElement
{
public:
//-----------------------------------------------------------------------------
	virtual ~CArrayElement () {}
//-----------------------------------------------------------------------------
};

//-----------------------------------------------------------------------------
//  CArray class declaration
//-----------------------------------------------------------------------------
class CArray
{
public:
	CArray (int32 delta = 1);
	virtual ~CArray ();

	bool add (CArrayElement* e);	///< add an element to the array
	bool removeAt (int32 idx);		///< remove an element at a given index
	void removeAll ();				///< remove all elements

	int32 total () const { return count; }	///< returns the count of element in the array
	CArrayElement* at (int32 index) const;	///< returns a pointer of a element at a given index

protected:
	int32 size;
	int32 count;
	int32 delta;
	CArrayElement** elements;

	bool resize (int32 newSize);
};

} // end namespace

using namespace Steinberg;

#endif	// __array__
