#ifndef EVENT_H_
#define EVENT_H_
#include "../Graphics/Material.h"
#include "../Physics/Photon.h"
#include "Detector.h"

#define	E_ERROR		-1
#define	E_SINGLE	0
#define	E_PROMPT	1
#define	E_SCATTER	2
#define	E_RANDOM	3
#define	E_MULTIPLE	4

class Event {
	public:
		Event();
		Event(const Photon &p, INTER_TYPE t, bool log_m, unsigned m_id, Detector * d);
		//Event(const Photon &p, INTER_TYPE t, bool log_m, unsigned m_id);
		void SetPrompt();
		void SetScatter();
		void SetRandom();
		void SetMultiple();
		void BinEvent();
		friend ostream& operator<< ( ostream& os, const Event& e );
		bool operator< (const Event&e);
		void BlurEvent(Detector& d);
		void BlurEvent();
		INTER_TYPE type;
		Photon photon;
		bool log_material;
		unsigned mat_id;
		bool delete_event;
		Detector * det;
		int flag;		// Coincidence processor flag
					// -1 error
					// 0 Single
					// 1 Prompt
					// 2 Prompt (scatter)
					// 3 Delayed Window Coincidence
					// 4 Multiple

	private:
		void Init(const Photon &p, INTER_TYPE t, bool log_m, unsigned m_id, Detector * d);
};

void inline Event::BlurEvent(Detector& d) {
	photon.time = d.BlurTime(photon);
	photon.energy = d.BlurEnergy(photon);
}

void inline Event::BlurEvent() {
	photon.time = det->BlurTime(photon);
	photon.energy = det->BlurEnergy(photon);
}

void inline Event::SetPrompt() { 
	if (photon.CheckScatter()) flag = E_SCATTER;
	else flag = E_PROMPT; 
}
void inline Event::SetScatter() { flag = E_SCATTER; }
void inline Event::SetRandom() { flag = E_RANDOM; }
void inline Event::SetMultiple() { flag = E_MULTIPLE; }
void inline Event::BinEvent() { photon.pos = det->pos; }

#endif /*EVENT_H_*/
