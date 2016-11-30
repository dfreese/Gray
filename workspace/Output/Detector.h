#ifndef DETECTOR_H
#define DETECTOR_H
#include "../VrMath/LinearR3.h"
#include "../VrMath/LinearR4.h"
#include "../Random/Random.h"
#include "../Physics/Photon.h"

class Detector {
	public:
		Detector();
		Detector(unsigned id, const VectorR3 & p, const VectorR3 &s, const RigidMapR3 & m, const double time_res, const double eres);
		Detector(unsigned id, const VectorR3 & p, const VectorR3 &s, const RigidMapR3 & m, const double time_res, const double eres, const unsigned x, const unsigned y, const unsigned z, const unsigned bl);
		void Init(unsigned id, const VectorR3 & p, const VectorR3 &s, const RigidMapR3 & m, const double time_res, const double eres, const unsigned x, const unsigned y, const unsigned z,const unsigned bl);
		unsigned detector_id;
		VectorR3 size;
		VectorR3 pos;
		RigidMapR3 map;
		friend ostream& operator<< ( ostream& os, const Detector& d );
		void SetTimeResolution(double r);
		void SetEnergyResolution(double r);
		void SetDecayTime(double t);
		double GetTimeResolution() const { return time_resolution; };
		double GetEnergyResolution() const { return energy_resolution; };

		double BlurTime(Photon &p );
		double BlurEnergy(Photon &p );
	
		void IncrementHit();
		void SetBlock(unsigned int bl) { block = bl; }
		unsigned long GetHitCount() const { return count; }
		double decay_time;
		unsigned idx[3];
		unsigned block;

	private:
		Random rand;
		double time_resolution;	/* specified as ns FWHM */
		double energy_resolution; /* specified as % FWHM */
		unsigned long count;
};

void inline Detector::SetTimeResolution(double r) { time_resolution = r; }
void inline Detector::SetEnergyResolution(double r) { energy_resolution = r; }
void inline Detector::SetDecayTime(double t) { decay_time = t; }
double inline Detector::BlurTime(Photon & p) { return p.time + time_resolution*FWHM_to_sigma* rand.Gaussian(); }
double inline Detector::BlurEnergy(Photon & p) { return p.energy*(1.0 + energy_resolution * FWHM_to_sigma * rand.Gaussian()); }
void inline Detector::IncrementHit() { count++; }

#endif /*DETECTOR_*/
