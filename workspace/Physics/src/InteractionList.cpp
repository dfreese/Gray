#include <Physics/InteractionList.h>
#include <Physics/GammaStats.h>
#include <Physics/Photon.h>
#include <Physics/Positron.h>

InteractionList::InteractionList()
{
    /* set default values for EMPTY */
    hits.clear();
}
InteractionList::~InteractionList() {}

bool InteractionList::isEmpty() const
{
    return hits.empty();
}

inline void InteractionList::AddHit(Deposit & d)
{
    hits.push_back(d);
}

inline Deposit InteractionList::NextHit()
{
    if (hits.empty()) {
        return EMPTY;
    }
    Deposit deposit = hits.back();
    hits.pop_back();
    return deposit;
}

ostream& operator<< (ostream &os, const InteractionList & l)
{
    for (int idx = 0; idx < l.hits.size(); idx++) {
        os << "[" << idx << "]:";
        os << l.hits[idx];
        os << "\n";
    }
    return os;
}

void InteractionList::HitPositron(const Positron &p, double deposit)
{
    hit.Reset();
    // TODO: Calculate the energy that is deposited based on the beta energy of the positron
    hit.pos = p.GetPosition();
    hit.id = p.GetId();
    hit.time = p.time;
    hit.energy = deposit;
    hit.type = I_POSITRON;
    hit.src_id = p.source_num;
    hit.mat_id = -1;
    hits.push_back(hit);
}

void InteractionList::HitCompton(const Photon &p, double deposit, const GammaStats & mat_gamma_prop)
{
    hit.Reset();
    hit.pos = p.pos;
    hit.id = p.id;
    hit.det_id = p.det_id;
    hit.color = p.color;
    hit.time = p.time;
    hit.energy = deposit;
    hit.type = I_COMPTON;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hits.push_back(hit);
}
void InteractionList::HitPhotoelectric(const Photon &p, double deposit, const GammaStats & mat_gamma_prop)
{
    hit.Reset();
    hit.pos = p.pos;
    hit.id = p.id;
    hit.det_id = p.det_id;
    hit.color = p.color;
    hit.time = p.time;
    hit.energy = deposit;
    hit.type = I_PHOTOELECTRIC;
    hit.src_id = p.GetSrc();
    hit.mat_id = mat_gamma_prop.GetMaterial();
    hits.push_back(hit);
}

void InteractionList::Reset()
{
    hits.clear();
}
