#ifndef VECTORSOURCE_H_
#define VECTORSOURCE_H_

#include <math.h>
#include "Source.h"
#include "../VrMath/LinearR3.h"
#include "../VrMath/LinearR4.h"
#include "../VrMath/Aabb.h"
#include "../Graphics/VisiblePoint.h"
#include "../Graphics/ViewableTriangle.h"
#include "../Random/mt19937.h"
#include "../VrMath/MathMisc.h"

class VectorSource : public Source {
	public:
		VectorSource(const double act);
		VectorSource(const double act, const VectorR3& boxMin, const VectorR3& boxMax);
		void SetMin(const VectorR3 &vert);
		void SetMax(const VectorR3 &vert);
		const VectorR3 & GetMin() { return aabb.GetBoxMin(); }
		const VectorR3 & GetMax() { return aabb.GetBoxMax(); }

		void virtual Decay(unsigned int photon_number);
		bool virtual Inside(const VectorR3 & pos) const;

	private:
		AABB aabb;
		bool RejectionTest(const VectorR3 &pos);
		void UniformSphere(VectorR3 & p);
		double Random();
};

#endif /*VECTORSOURCE_H_*/
