/*
 * SimpleNurbs.h
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 */

#ifndef CSE167_VOXELSOURCE_H
#define CSE167_VOXELSOURCE_H

#include "Source.h"
#include <iostream>
#include <fstream>

using namespace std;

class VoxelSource : public Source {
public:
	~VoxelSource();
	VoxelSource();
	VoxelSource(const VectorR3 &p, int dims[3], const VectorR3 & vox_scale, double act);
	bool Load(const char * filename);

	void virtual Decay(unsigned int photon_number);
	bool virtual Inside(const VectorR3 & pos) const;
private:
	int dimension[3];
	VectorR3 scale;
	int search(double e, int b_idx, int s_idx);
	int AddSource(double val);

	vector <double> prob;
	vector <int*> index;
};

#endif
