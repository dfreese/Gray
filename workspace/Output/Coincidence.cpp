#include "Coincidence.h"

Coincidence::Coincidence()
{
    buffer_size = 150;
    buffer_limit = 20;
    time_window = 10e-9; /* default time window is 10 ns */
    delayed_time_window_offset = 100e-9; /* default time window is 10 ns */
    log_coincidence = false;
    energy_gate_lower = 0.390;
    energy_gate_upper = 0.600;
    binEvents = false;

    // Sets the maximum ring difference between events in the Z axis
    ring_difference = 2;
    gate2D = false;
    positionCOM = true;
    blockEffect = false;
}

Coincidence::~Coincidence()
{
    long sizeCoinc = coinc.Size();
    if (log_coincidence) {
        output.close();
    }
}

bool Coincidence::AddEvent(const Photon &p, const INTER_TYPE type, const GammaMaterial & mat, Detector * d)
{
    // Coincidence processor doesn't deal with logging scattered photons: see other parsers
    // or materials that are not logged

    if (!mat.GammaProp.log_material) {
        return false;
    }
    if (p.det_id == -1) {
        return false;
    }

    Event eadd(p,type,mat.GammaProp.log_material, mat.GammaProp.GetMaterial(),d);
    // output << "B: " << *eadd << endl;
    eadd.BlurEvent(*d);
    //output << "A: " << *eadd << endl;
    coinc.Push(eadd);

    if (coinc.Size() < buffer_size) {
    } else {

        MergeEventsDetector();
        if (blockEffect) {
            MergeEventsBlock();
        }
        // TODO: Introduce concept of block id
        // TODO: Add Delayed Window

        // Coincidence Processing Algorithm
        for (long i; coinc.Size() > buffer_limit; i++) {
            double select_window = 0.0;
            double select_del_window_start = 0.0;
            double select_del_window_end = 0.0;
            bool output_e1;
            Event e1 = NextEvent();
            if (!((e1.photon.energy > energy_gate_lower) && (e1.photon.energy < energy_gate_upper))) {
                continue;
            }

            Event e2;

            output_e1 = true;
            select_window = e1.photon.time + time_window;

            // delayed coincidence window same as select window
            // may want to make this larger for better statistics
            select_del_window_start = e1.photon.time + delayed_time_window_offset;
            select_del_window_end = e1.photon.time + delayed_time_window_offset + time_window;

            do {
                if (coinc.IsEmpty()) {
                    break;
                }
                e2 = NextTop();

                // Open up coincidence window, and collect all events in the window
                if (e2.photon.time < select_window) {
                    e2 = NextEvent();
                    if ((e2.photon.energy > energy_gate_lower) && (e2.photon.energy < energy_gate_upper)) {
                        if (output_e1) {
                            // Only process the FIRST coincidence in the window
                            e1.SetPrompt();
                            e2.SetPrompt();
                            if (binEvents) {
                                e1.BinEvent();
                                e2.BinEvent();
                            }
                            // Reduce data and only bin for 2D FBP
                            // TODO: Add flag in datafile to enable 2D FBP
                            if (FBP2D_check(e1,e2,ring_difference)) {
                                output << e2 << endl;
                                output << e1 << endl;
                            }
                            output_e1 = false;
                        } else {  	// We have multiple events within a window
                            //TODO: Process Multiple
                        }
                        break;
                    } else {
                        // TODO: Process multi-detector events
                        //output << "[" << coinc.Size() << "]C2: " << e2 << " ESUMC DELTA_T: " << (e2.photon.time - e1.photon.time) << endl;
                    }
                } else {
                    break;
                }
            } while (true);
        }
    }
    return true;
}

bool Coincidence::SetLogFile(char * name)
{
    output.open(name, ios::out);
    if (output.fail()) {
        cerr << "ERROR: cannot open ";
        cerr << name;
        cerr << " log file.\n";
        log_coincidence = false;
        return false;
    } else {
        log_coincidence = true;
        return true;
    }
}

bool Coincidence::MergeEventsDetector()
{
    double merge_window;  			// Events are merged together in a single detector_id based on the merge window
    // This window is based on the 40 ns decay time of LSO
    //TODO: Lookup decay time of scintillator from Gray_Material.txt
    unsigned start_idx = 0;
    unsigned scan_idx = 1;
    long det_id = -1;
    double cur_time = 0.0;
    double scan_time = 0.0;
    //for (unsigned long i = 0; i < coinc.Size(); i++) {
    //}
    coinc.Sort();

    // Mininum event starts at Size-1
    start_idx = coinc.Size()-1;
    while (start_idx > 0) {
        cur_time = coinc[start_idx].photon.time;
        det_id = coinc[start_idx].photon.det_id;
        merge_window = coinc[start_idx].det->decay_time;
        scan_idx = start_idx-1;
        do {
            if (scan_idx == 0) {
                break;
            }
            scan_time = coinc[scan_idx].photon.time;
            if (det_id == coinc[scan_idx].photon.det_id) {
                //output << "M[" << cur_time << "][" << start_idx << "][" << scan_time << "][" << scan_idx << "][" << det_id << "]\n";
                // Add energy together
                // Keep timing for the first event
                coinc[start_idx].photon.energy += coinc[scan_idx].photon.energy;
                // Delete flag be deleted on Pop
                coinc[scan_idx].delete_event = true;
            }
            scan_idx--;
        } while ((cur_time + merge_window) > scan_time);
        start_idx--;
    }
    return false;
}

bool Coincidence::MergeEventsBlock()
{

    // ASSUMES EVENTS ARE SORTED

    double merge_window;  			// Events are merged together in a single detector_id based on the merge window
    // This window is based on the 40 ns decay time of LSO
    //TODO: Lookup decay time of scintillator from Gray_Material.txt
    unsigned start_idx = 0;
    unsigned scan_idx = 1;
    //long det_id = -1;
    long block_id = -1;
    double cur_time = 0.0;
    double scan_time = 0.0;

    // Mininum event starts at Size-1
    start_idx = coinc.Size()-1;
    while (start_idx > 0) {
        if (coinc[start_idx].delete_event == false) {
            cur_time = coinc[start_idx].photon.time;
            //det_id = coinc[start_idx].photon.det_id;
            // Merge Events based on block id, not detector id
            block_id = coinc[start_idx].det->block;

            merge_window = coinc[start_idx].det->decay_time;
            //cout << "Decay Time:" << coinc[start_idx].det->decay_time << "\n";
            scan_idx = start_idx-1;
            do {
                if (scan_idx == 0) {
                    break;
                }
                scan_time = coinc[scan_idx].photon.time;
                if (coinc[scan_idx].delete_event == false) {
                    if (block_id == coinc[scan_idx].det->block) {
                        // TODO: Change the detector id to reflect COM positioning
                        if (positionCOM) {
                            VectorR3 com_pos1, com_pos2;
                            double e_sum;
                            e_sum = coinc[start_idx].photon.energy + coinc[scan_idx].photon.energy;
                            com_pos1 = coinc[start_idx].photon.pos;
                            com_pos1 *= coinc[start_idx].photon.energy/e_sum;

                            com_pos2 = coinc[scan_idx].photon.pos;
                            com_pos2 *= coinc[scan_idx].photon.energy/e_sum;

                            com_pos1 += com_pos2;

                            coinc[start_idx].photon.pos = com_pos1;
                        }
                        // Add energy together
                        // Keep timing for the first event
                        coinc[start_idx].photon.energy += coinc[scan_idx].photon.energy;
                        // Delete flag be deleted on Pop
                        coinc[scan_idx].delete_event = true;
                    }
                }
                scan_idx--;
            } while ((cur_time + merge_window) > scan_time);
        }
        start_idx--;
    }
    return false;
}

Event& Coincidence::NextEvent()
{
    while (coinc.Top().delete_event == true) {
        //output << "D:" << coinc.Pop() << endl;
        coinc.Pop();
    }
    return coinc.Pop();
}
Event& Coincidence::NextTop()
{
    while (coinc.Top().delete_event == true) {
        //output << "D:" << coinc.Pop() << endl;
        coinc.Pop();
    }
    return coinc.Top();
}

bool Coincidence::DelayedWindow()
{
    return false;
}

bool Coincidence::FBP2D_check(const Event &e1, const Event &e2, unsigned int ring)
{
    if (!gate2D) {
        return true;
    }
    // Assumptions:
    //  1) scanner is oriented in z
    //  2) scanner has been defined by a single repeat command, or no inter ring coincidence
    //		TODO: Allow for inter repeat coincidence between rings
    if ((unsigned int)abs((int)e1.det->idx[2] - (int)e2.det->idx[2]) <= (unsigned int)ring) {
        return true;
    } else {
        return false;
    }
}
