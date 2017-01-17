#include <Physics/InteractionList.h>
#include <Physics/GammaMaterial.h>
#include <Physics/Photon.h>
#include <Physics/Positron.h>

InteractionList::InteractionList()
{
    /* set default values for EMPTY */
    hits.Reset();
}
InteractionList::~InteractionList() {}

inline void InteractionList::AddHit(Deposit & d)
{
    hits.Push(d);
}

inline Deposit & InteractionList::NextHit()
{
    if (hits.IsEmpty()) {
        return EMPTY;
    }
    return hits.Pop();
}

ostream& operator<< (ostream &os, const InteractionList & l)
{
    for (int idx = 0; idx < l.hits.SizeUsed(); idx++) {
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
    hits.Push(hit);
}

void InteractionList::HitCompton(const Photon &p, double deposit, const GammaMaterial & mat)
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
    hit.mat_id = mat.GammaProp.GetMaterial();
    hits.Push(hit);
}
void InteractionList::HitPhotoelectric(const Photon &p, double deposit, const GammaMaterial & mat)
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
    hit.mat_id = mat.GammaProp.GetMaterial();
    hits.Push(hit);
}

void InteractionList::Reset()
{
    hits.Reset();
}
