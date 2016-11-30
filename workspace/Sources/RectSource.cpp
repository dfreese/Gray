#include "RectSource.h"

RectSource::RectSource() {
	position.SetZero();
}

RectSource::RectSource(const VectorR3 &p, const VectorR3 &sz, double act) {
	SetPosition(p);
	SetSize(sz);
	SetActivity(act);
}

void RectSource::Decay(unsigned int photon_number) {
	if (isotope == NULL) return;
	VectorR3 pos;
	pos.x = (0.5 - Random());
	pos.y = (0.5 - Random());
	pos.z = (0.5 - Random());
	pos.ArrayProd(size);
	pos += position;
	isotope->SetPosition(pos);
	isotope->SetMaterial(GetMaterial());
	isotope->Decay(photon_number);
}

void RectSource::SetSize(const VectorR3 &sz) {
	size = sz;
}

bool RectSource::Inside(const VectorR3 & pos) const {

	// FIXME: Source rotation does not work
	// (translation works)
	if (isotope == NULL) return false;

	VectorR3 dist;
	dist = pos;
	dist -= position;

	if (fabs(dist.x) > size.x/2.0) return false;
	if (fabs(dist.y) > size.y/2.0) return false;
	if (fabs(dist.z) > size.z/2.0) return false;

	return true;
}
