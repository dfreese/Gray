#ifndef COINCIDENCE_H_
#define COINCIDENCE_H_
/*************************************************************
	Coincidence.h

	Coincidence Parser: for finding events that occur within a time window after being blurred by finite energy resolution and timing resolution of the detector system
		This is for a standard clinical imaging system that does not pair up inter-crystal scatter events

	TODO:  Merge events from the same detector (or block ID, to simulate inter-crystal-block scatter)
	TODO:  More accurate energy and time resolution blurring functions
		Energy_blurring should be a lookup table as a function of energy
		Time_resolution blurring should be a lookup table as a function of energy
		Currently modelled as a constant blurring function 2 ns FWHM Time resolution, and 10% Energy Resolution

	TODO: Add the concept of detector Id and Block Id: new data structure elements.  Block Id sum events together
	TODO: Add delayed window coincidence gating (should be simple, setup second buffer with window that has a large fixed offset)
	TODO: BackPropagate Singles Count back to the detector elements

	Algorithm:
		Insert 100 events into a priority queue
		(TODO) Merge events from same detector
		Remove first event
		TODO: (Energy Gate Event)
		Pair up all events that are within a time window of the first event
		(HACK) If Energies sum up above 480keV * 2, then register a coincidence
****************************************************************/

#include "../Graphics/MaterialBase.h"
#include "../Physics/Photon.h"
#include "../Physics/PositronDecay.h"
#include "../DataStructs/PQueue.h"
#include "Event.h"
#include "Detector.h"
#include "BinaryFormat.h"
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define ERROR_EMPTY (int)-2
#define ERROR_TRACE_DEPTH (int)-3

class Coincidence
{
public:
    Coincidence();
    ~Coincidence();
    void Set2D(unsigned int ring_diff);
    void SetTimeGate(double t);
    void SetDelayedTimeOffset(double t);
    void SetEnergyGate(double lower, double upper);
    bool AddEvent(const Photon &p, const INTER_TYPE type, const MaterialBase & mat, Detector * d);
    bool SetLogFile(char * name);
    bool SetBinning(bool b)
    {
        binEvents = b;
    };

private:
    bool MergeEventsDetector();
    bool MergeEventsBlock();
    bool DelayedWindow();
    long buffer_size;
    long buffer_limit;
    double time_window;
    double delayed_time_window_offset;
    double energy_gate_lower;
    double energy_gate_upper;
    PQueue<Event> coinc;
    ofstream output;
    bool log_coincidence;
    Event & NextEvent();
    Event & NextTop();
    bool binEvents;
    bool FBP2D_check(const Event &e1, const Event &e2, unsigned int ring);
    bool gate2D;
    bool positionCOM;
    bool blockEffect;
    unsigned int ring_difference;
};

void inline Coincidence::SetTimeGate(double t)
{
    time_window = t;
}
void inline Coincidence::SetDelayedTimeOffset(double t)
{
    delayed_time_window_offset = t;
}
void inline Coincidence::SetEnergyGate(double lower, double upper)
{
    energy_gate_lower = lower;
    energy_gate_upper = upper;
}
void inline Coincidence::Set2D(unsigned int ring_diff)
{
    ring_difference = ring_diff;
    gate2D = true;
}

#endif /*COINCIDENCE_H_*/
