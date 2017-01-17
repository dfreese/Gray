#include <Output/EventBuffer.h>
#include <Physics/GammaStats.h>

EventBuffer::EventBuffer()
{
    log_coincidence = false;
    log_singles = false;
    buffer_size = 100;
    buffer_limit = 10;
    num_elements = 0;
    isCoinc = false;
    existsRed = false;
    existsBlue = false;
    hitDetector = false;
}
EventBuffer::~EventBuffer()
{
    if (log_singles) {
        singles.close();
    }
    if (log_coincidence) {
        coincidence.close();
    }
    // Garry: Added to cleanup event queue:
    for (int i = 0; i < num_elements; i++) {
        CLinkedListElt<Event>* cur = events.GetLast();
        cur->DeleteRemove();
        num_elements--;
    }
}

void EventBuffer::ProcessSingleEvents(Event * e)
{
    // if queue is empty, add the event
    if (NECb.IsEmpty()) {
        hitDetector = (e->photon.det_id != -1);
        NECb.Push(e);
    } else {
        // Add events to queue until we have a change in event id
        if (e->photon == NECb.Top()->photon) {
            hitDetector |= (e->photon.det_id != -1);
            NECb.Push(e);
        } else {
            // when event id changes, we check to see if we had coincidence
            if (hitDetector) {
                PrintFlushNEC();
            } else {
                FlushNEC();
            }
            // add this photon to top of queue
            // as if the queue was empty
            hitDetector = (e->photon.det_id != -1);
            NECb.Push(e);
        }
    }
}

void EventBuffer::ProcessCoincEvents(Event * e)
{
    // We only care about the photon if it hits the detectors
    if (e->photon.det_id != -1) {
        // if queue is empty, add the event
        if (cb.IsEmpty()) {
            existsRed = false;
            existsBlue = false;
            if (e->photon.color == P_RED) {
                existsRed = true;
            } else {
                existsBlue = true;
            }
            cb.Push(e);
        } else {
            // Add events to queue until we have a change in event id
            if (e->photon.id == cb.Top()->photon.id) {
                cb.Push(e);
                if (e->photon.color == P_RED) {
                    existsRed = true;
                } else {
                    existsBlue = true;
                }
                if (existsRed && existsBlue) {
                    isCoinc = true;
                }
            } else {
                // when event id changes, we check to see if we had coincidence
                if (isCoinc) {
                    PrintFlushCoinc();
                } else {
                    FlushCoinc();
                }
                // add this photon to top of queue
                // as if the queue was empty
                if (e->photon.color == P_RED) {
                    existsRed = true;
                } else {
                    existsBlue = true;
                }
                cb.Push(e);
            }
        }
    }
    // Garry: Added this to solve memory leak
    else {
        delete e;
    }
}

bool EventBuffer::AddEvent(const Photon &p, INTER_TYPE type, const GammaStats & mat_gamma_prop, Detector * d)
{
    if ((log_singles == false) && (log_coincidence == false)) {
        return false;
    }

    Event * e1 = new Event(p, type, mat_gamma_prop.log_material, mat_gamma_prop.GetMaterial(),d);
    events.AddHead(e1);
    num_elements++;

    if (num_elements < buffer_size) {
    }  else {
        for (int i = 0; num_elements > buffer_limit; i++) {
            CLinkedListElt<Event>* cur = events.GetLast();
            Event * e1 = cur->GetEntry();
            if (log_singles) {
                // Make new event and pass it off to singles buffer
                Event * se = new Event(e1->photon, e1->type, e1->log_material,e1->mat_id, e1->det);
                ProcessSingleEvents(se);
            }
            //if (log_coincidence && mat.GammaProp->log_material) {
            // weird bug about material properties
            if (log_coincidence&&e1->log_material) {
                // Make new event and pass it off to Coincidence buffer
                Event * ce = new Event(e1->photon, e1->type, e1->log_material, e1->mat_id, e1->det);
                ProcessCoincEvents(ce);
            }
            // WATCH OUT FOR MEMORY LEAKS!
            // 	DON'T USE EVENTS AFTER PROCESS EVENTS
            cur->DeleteRemove();
            num_elements--;
        }
    }
    if (log_singles) {
        singles.flush();
    }
    if (log_coincidence) {
        coincidence.flush();
    }
    return true;
}

bool EventBuffer::WriteScatterEvents()
{
    return false;
}
bool EventBuffer::WriteCoincidenceEvents()
{
    return false;
}
bool EventBuffer::WriteSingleEvents()
{
    return false;
}
void EventBuffer::FlushNEC()
{
    Event * e;
    while (!NECb.IsEmpty()) {
        e = NECb.Pop();
        delete e;
    }
}

void EventBuffer::PrintFlushNEC()
{
    Event * e;
    while (!NECb.IsEmpty()) {
        e = NECb.Pop();
        singles << *e;
        if (log_detector) {
            singles << " ";
            singles << e->photon.det_id;
        }
        singles << "\n";
        delete e;
    }
}

void EventBuffer::Flush()
{
    cerr << "Stub: EventBuffer:Flush(): Not implemented\n";
    exit(0);
}

void EventBuffer::SetSingles(const std::string& file_name)
{
    singles.open(file_name.c_str(), ios::out);
    if (singles.fail()) {
        cerr << "ERROR: cannot open ";
        cerr << file_name.c_str();
        cerr << " singles.\n";
        exit(0);
    }
    log_singles = true;

}
void EventBuffer::SetCoincidence(const std::string& file_name)
{
    coincidence.open(file_name.c_str(), ios::out);
    if (coincidence.fail()) {
        cerr << "ERROR: cannot open ";
        cerr << file_name.c_str();
        cerr << " coincidence.\n";
        exit(0);
    }
    log_coincidence = true;
}

void EventBuffer::SetBufferSize(unsigned int size)
{
    buffer_size = size;
}
void EventBuffer::SetBufferFraction(float f)
{
    buffer_limit = (unsigned int)((float)buffer_size * f);
}

void EventBuffer::AddCoinc(Event * e)
{
    if (e->photon.det_id != -1) {
        cb.Push(e);
    }
}

void EventBuffer::PrintFlushCoinc()
{
    isCoinc = false;
    existsRed = false;
    existsBlue = false;
    while (!cb.IsEmpty()) {
        Event * e = cb.Pop();
        coincidence << *e;
        if (log_detector) {
            coincidence << " ";
            coincidence << e->photon.det_id;
        }
        coincidence << "\n";
        delete e;
    }
}

void EventBuffer::FlushCoinc()
{
    existsRed = false;
    existsBlue = false;
    isCoinc = false;
    while (!cb.IsEmpty()) {
        Event * e = cb.Pop();
        delete e;
    }
}
