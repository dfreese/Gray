#include <Output/Event.h>

Event::Event()
{
    type = NO_INTERACTION;
    log_material = false;
    delete_event = false;
}

void Event::Init(const Photon &p, INTER_TYPE t, bool log_m, unsigned m_id, Detector * d)
{
    photon = p;
    type = t;
    log_material = log_m;
    mat_id = m_id;
    delete_event = false;
    det = d;
    flag = 0;
}

Event::Event(const Photon &p, INTER_TYPE t, bool log_m, unsigned m_id, Detector * d)
{
    Init(p, t, log_m, m_id, d);
}

ostream& operator<< ( ostream& os, const Event& e )
{
    switch(e.type) {
    case COMPTON:
        os << "1 ";
        break;
    case PHOTOELECTRIC:
        os << "3 ";
        break;
    default:
        os << "-1 ";
        break;
    }
    os << e.photon;
    //		log_file << mat.GammaProp->log_material;
    //        	log_file << " ";
    os << e.mat_id;
    os << " ";
    os << e.photon.det_id;

    // block detector id indexes
    // old singles parser doesn't set the e.photon.det
    if (e.det != NULL) {
        os << " ";
        os << e.det->idx[0];
        os << " ";
        os << e.det->idx[1];
        os << " ";
        os << e.det->idx[2];
    } else 			   {
        os << " ";
        os << -1;
        os << " ";
        os << -1;
        os << " ";
        os << -1;
    }
    os << " ";
    os << e.flag;

    return os;
}

bool Event::operator< (const Event&e)
{
    return photon < e.photon;
}
