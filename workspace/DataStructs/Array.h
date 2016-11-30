/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
 *
 * Mathematics Subpackage (VrMath)
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.  Please acknowledge
 *   all use of the software in any publications or products based on it.
 *
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 */

// Array.h
//
//   A general purpose dynamically resizable array.
//   Implemented with templates.
//	Items are stored contiguously, for quick indexing.
//		However, allocation may require the array to be copied into
//		new memory locations.
//	Because of the dynamic resizing, you should be careful to understand
//		how the array code works before using it.  Care should be taken 
//		if the array might be resized during an expression evaluation.
//
// Author: Sam Buss.
// Contact: sbuss@math.ucsd.edu
// All rights reserved.  May be used for any purpose as long
//	as use is acknowledged.


#ifndef ARRAY_H
#define ARRAY_H

#include <assert.h>
#define ARRAY_MAX(a,b) (((a)>(b))?(a):(b))

template <class T> class Array {

public:
	Array();						// Constructor
	Array(long initialSize);		// Constructor
	~Array();					// Destructor

	void Reset() { ReducedSizeUsed(0); }

	// Next two routines: Main purpose to control allocation.
	//   Note: Allocation can occur without warning if you do not take care.
	//	 When allocation occurs, pointer and references become bad.
	// Use this judiciously to control when the array may be resized
	void PreallocateMore( long numAdditional ) { Resize( SizeUsed()+numAdditional ); }
	void Resize( long newMaxSize );	// Increases allocated size (will not decrease allocated size)

	// Next routines are used to update the "SizeUsed" of the stack.
	void Touch( long i );			// Makes entry i active.  Increases SizeUsed (MaxEntryPlus)
	void ReducedSizeUsed( long i ); // "i" is the number of used entries now.
	// Next four functions give the functionality of a stack.
	T& Top() { return *(TheEntries+(MaxEntryPlus-1)); }
	const T& Top() const { return *(TheEntries+(MaxEntryPlus-1)); }
	T& Pop();
	T* Push();					// Push with no argument returns pointer to top element
	T* Push( T& newElt );
	bool IsEmpty() const { return (MaxEntryPlus==0); }

	void DisallowDynamicResizing() { DynamicResizingOK = false; }
	void AllowDynamicResizing() { DynamicResizingOK = true; }

	// Access function - if i is out of range, it resizes
	T& operator[]( long i );
	const T& operator[]( long i ) const;

	// Access functions - Do not check whether i is out of range!!
	//		No resizing will occur!  Use only to get existing entries.
	T& GetEntry( long i );
	const T& GetEntry( long i ) const;

	long SizeUsed() const;			// Number of elements used (= 1+(max index));
	long AllocSize() const;			// Size allocated for the array

	bool IsFull() const { return SizeUsed()==AllocSize; }
	long SizeAvailable() const { return Allocated-MaxEntryPlus; }

private:

	long MaxEntryPlus;				// Maximum entry used, plus one (Usually same as size)
	long Allocated;					// Number of entries allocated
	T* TheEntries;					// Pointer to the array of entries

	bool DynamicResizingOK;			// If "true", array can be dynamically resized.

};

template<class T> inline Array<T>::Array()
{ 
	MaxEntryPlus = 0;
	TheEntries = 0;
	Allocated = 0;
	DynamicResizingOK = true;
	Resize( 10 );
}

template<class T> inline Array<T>::Array(long initialSize)
{
	MaxEntryPlus = 0;
	TheEntries = 0;
	Allocated = 0;
	DynamicResizingOK = true;
	Resize( initialSize );
}


template<class T> inline Array<T>::~Array<T>()
{
	delete[] TheEntries;
}

template<class T> inline void Array<T>::Resize( long newMaxSize )
{
	if ( newMaxSize <= Allocated ) {
		return;
	}
	if ( !DynamicResizingOK ) {
		assert(false);
		//exit(0);
	}
	Allocated = ARRAY_MAX(2*Allocated+1,newMaxSize);
	T* newArray = new T[Allocated];
	T* toPtr = newArray;
	T* fromPtr = TheEntries;
	for ( long i=0; i<MaxEntryPlus; i++ ) {
		*(toPtr++) = *(fromPtr++);
	}
	delete[] TheEntries;
	TheEntries = newArray;	
}

template<class T> inline void Array<T>::Touch( long i )
{
	if ( i>=Allocated ) {
		Resize( i+1 );
	}
	if ( i >= MaxEntryPlus ) {
		MaxEntryPlus = i+1;
	}
}

template<class T> inline void Array<T>::ReducedSizeUsed( long i )
{
	// "i" is the number of used entries now.
	if ( i<MaxEntryPlus ) {
		MaxEntryPlus = i;
	}
}


template<class T> inline T& Array<T>::Pop()
{
	assert( MaxEntryPlus>0 );
	MaxEntryPlus-- ;
	return *(TheEntries+MaxEntryPlus);
}

template<class T> inline T* Array<T>::Push()
{
	if ( MaxEntryPlus>=Allocated ) {
		Resize( MaxEntryPlus+1 );
	}
	T* ret = TheEntries+MaxEntryPlus;
	MaxEntryPlus++;
	return ret;
}

template<class T> inline T* Array<T>::Push( T& newElt )
{
	T* top = Push();
	*top = newElt;
	return top;
}

template<class T> inline T& Array<T>::operator[]( long i )
{
	if ( i>=Allocated ) {
		Resize( i+1 );
	}
	if ( i >= MaxEntryPlus ) {
		MaxEntryPlus = i+1;
	}
	return TheEntries[i];
}

template<class T> inline const T& Array<T>::operator[]( long i ) const
{
	if ( i>=Allocated ) {
		const_cast<Array<T>*>(this)->Resize( i+1 );
	}
	if ( i >= MaxEntryPlus ) {
		const_cast<Array<T>*>(this)->MaxEntryPlus = i+1;
	}
	return TheEntries[i];
}

template<class T> inline T& Array<T>::GetEntry( long i )
{
	assert ( i < MaxEntryPlus );
	return TheEntries[i];
}

template<class T> inline const T& Array<T>::GetEntry( long i ) const
{
	assert ( i < MaxEntryPlus );
	return TheEntries[i];
}

template<class T> inline long Array<T>::SizeUsed () const
{
	return MaxEntryPlus;
}

template<class T> inline long Array<T>::AllocSize () const
{
	return Allocated;
}


#endif // ARRAY_H
