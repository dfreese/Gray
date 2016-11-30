// PQueue.h
//
//   A general purpose dynamically resizable sorted Stack
//   Implemented with templates.
//	Items are stored contiguously, for quick accessing.
//	Templated class must implement a < operator
//	Top of queue has smallest element
//
//
// Author: Peter Olcott.
// Contact: pdo@stanford.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.


#ifndef PQUEUE_H
#define PQUEUE_H

#include <assert.h>
#include "ShellSort.h"
#define PQUEUE_MAX(a,b) (((a)>(b))?(a):(b))

template <class T> class PQueue {

public:
	PQueue();					// Constructor
	PQueue(long initialSize);			// Constructor
	~PQueue();					// Destructor

	void Reset();

	void Resize( long newMaxSize );	// Increases allocated size (will not decrease size)

	T& Top() const { return *TopElement; };
	T& Pop();

	T* Push();					// New top element is arbitrary
	T* Push( const T& newElt );			// Push newElt onto stack.

	bool IsEmpty() const { return (SizeUsed==0); }

	long Size() const { return SizeUsed; }
	long SizeAllocated() const { return Allocated; }
	void Sort();

	T& operator[]( long i );
	const T& operator[]( long i ) const;

private:

	long SizeUsed;				// Number of elements in the stack
	T* TopElement;				// Pointer to the top element of the stack
	long Allocated;				// Number of entries allocated
	T* ThePQueue;				// Pointer to the array of entries
};

template<class T> inline PQueue<T>::PQueue()
{ 
	SizeUsed = 0;
	ThePQueue = 0;
	Allocated = 0;
	Resize( 10 );
}

template<class T> inline PQueue<T>::PQueue(long initialSize)
{
	SizeUsed = 0;
	ThePQueue = 0;
	Allocated = 0;
	Resize( initialSize );
}


template<class T> inline PQueue<T>::~PQueue()
{
	delete[] ThePQueue;
}

template<class T> inline void PQueue<T>::Reset()
{
	SizeUsed = 0;
	TopElement = ThePQueue-1;
}

template<class T> inline void PQueue<T>::Resize( long newMaxSize )
{
	if ( newMaxSize <= Allocated ) {
		return;
	}
	long newSize = PQUEUE_MAX(2*Allocated+1,newMaxSize);
	T* newArray = new T[newSize];
	T* toPtr = newArray;
	T* fromPtr = ThePQueue;
	long i;
	for ( i=0; i<SizeUsed; i++ ) {
		*(toPtr++) = *(fromPtr++);
	}
	delete[] ThePQueue;
	ThePQueue = newArray;
	Allocated = newSize;
	TopElement = ThePQueue+(SizeUsed-1);
}

template<class T> inline T& PQueue<T>::Pop()
{
	T* ret = TopElement;
	assert( SizeUsed>0 );		// Should be non-empty
	SizeUsed--;
	TopElement--;
	return *ret;
}

// Enlarge the stack but do not update the top element.
template<class T> inline T* PQueue<T>::Push( )
{
	if ( SizeUsed >= Allocated ) {
		Resize(SizeUsed+1);
	}
	SizeUsed++;
	TopElement++;
	return TopElement;
}

template<class T> T * PQueue<T>::Push( const T& newElt )
{
	Push();
	*TopElement = newElt;
	return TopElement;
}

template<class T> inline void PQueue<T>::Sort()
{
	ShellSort(ThePQueue, SizeUsed);
	TopElement = ThePQueue+(SizeUsed-1);
}

template<class T> inline T& PQueue<T>::operator[]( long i )
{
	if ( i>=Allocated ) {
		Resize( i+1 );
	}
	return ThePQueue[i];
}

template<class T> inline const T& PQueue<T>::operator[]( long i ) const
{
	if ( i>=Allocated ) {
		const_cast<PQueue<T>*>(this)->Resize( i+1 );
	}
	return ThePQueue[i];
}

#endif /* PQUEUE_H */
