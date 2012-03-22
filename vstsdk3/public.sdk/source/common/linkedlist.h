//-----------------------------------------------------------------------------
// Project     : SDK Core
// Version     : 1.0
//
// Category    : Common Base Classes
// Filename    : linkedlist.h
// Created by  : Steinberg, 01/2004
// Description : Double-linked List
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

#ifndef __linkedlist__
#define __linkedlist__

#include "pluginterfaces/base/ftypes.h"

namespace Steinberg {

//------------------------------------------------------------------------
#define LIST_FOREACH(Type, obj, list) \
{ for (Type* obj = (Type*)(list).getRoot (); obj; obj = (Type*)obj->getNext ()) {

#define LIST_FOREACH_REVERSE(Type, obj, list) \
{ for (Type* obj = (Type*)(list).getLast (); obj; obj = (Type*)obj->getPrev ()) {

#define LIST_ENDFOR }}

//------------------------------------------------------------------------
//  CListElement class declaration
//------------------------------------------------------------------------
class CListElement
{
public:
//------------------------------------------------------------------------
	CListElement (): next (0), prev (0) {}
	virtual ~CListElement ()  {}

	CListElement* getNext () const { return next; }
	CListElement* getPrev () const { return prev; }
//------------------------------------------------------------------------
protected:
	friend class CLinkedList;

	CListElement* next;
	CListElement* prev;
};

//------------------------------------------------------------------------
//  CLinkedList class declaration
//------------------------------------------------------------------------
class CLinkedList
{
public:
//------------------------------------------------------------------------
	CLinkedList (): root (0), count (0) {}
	virtual ~CLinkedList () { removeAll (); }

	void append (CListElement* e);		///< append (at the end) a new element to the linked list
	void prepend (CListElement* e);		///< prepend (at the start) a new element to the linked list
	bool remove (CListElement* e);		///< remove a given element
	bool insertBefore (CListElement* before, CListElement* newElement);	///< insert an element before a given one
	void removeAll ();					///< remove all element of the list

	int32 total () const { return count; }	///< returns the count of element in the array
	CListElement* at (int32 index) const;	///< returns a pointer of a element at a given index
	bool contains (CListElement* e) const;	///< returns true if a given element is in the list
	int32 indexOf (CListElement* e) const;	///< returns the index in the list of a given element

	CListElement* getRoot () const { return root; }	///< returns the root (first) element of the list
	CListElement* getLast () const;					///< returns the last element of the list

//------------------------------------------------------------------------
protected:
	CListElement* root;
	int32 count;
};

} // end namespace

using namespace Steinberg;

#endif	// __linkedlist__
