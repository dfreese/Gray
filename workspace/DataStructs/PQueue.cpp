/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006.
 *
 * Mathematics Subpackage (VrMath)
 *
 * Author: Peter Olcott
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
 * Bug reports: Peter Olcott, pdo@stanford.edu
 *
 */
/*

#include <math.h>
#include <assert.h>
#include "PQueue.h"
#include "ShellSort.h"

template<class T> T * PQueue<T>::Push( const T& newElt )
{
	Push();
	*TopElement = newElt;
	ShellSort(ThePQueue, SizeUsed);
	TopElement = ThePQueue+(SizeUsed-1);
	return TopElement;
}
*/
