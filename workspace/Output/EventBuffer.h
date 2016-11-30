#ifndef EVENTBUFFER_H_
#define EVENTBUFFER_H_

#include "Event.h"
#include "../DataStructs/Array.h"
#include "../DataStructs/CLinkedList.h"
#include "../DataStructs/Stack.h"
#include "../DataStructs/PQueue.h"
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class EventBuffer
{
public:
	EventBuffer();
	~EventBuffer();
	bool AddEvent(const Photon &p, INTER_TYPE type, const MaterialBase & mat, Detector * d);
	bool WriteScatterEvents();
	bool WriteCoincidenceEvents();
	bool WriteSingleEvents();
	void Flush();
	void SetSingles(const std::string& file_name);
	void SetCoincidence(const std::string& file_name);
	void SetBufferSize(unsigned int size);
	void SetBufferFraction(float f);
	void SetLogDetector(bool val) { log_detector = val; }
private:

	void FlushCoinc();
	void PrintFlushCoinc();
	bool isCoinc;
	bool existsRed;
	bool existsBlue;
	
	bool hitDetector;
	
	void AddCoinc(Event * e);

	CLinkedList<Event> events;
	unsigned int buffer_size;
	unsigned int buffer_limit;
	ofstream singles;
	ofstream coincidence;
	
	bool log_detector;
	bool log_coincidence;
	bool log_singles;
	unsigned int num_elements;
	void ProcessSingleEvents(Event * e);
	void ProcessCoincEvents(Event * e);
	void FlushNEC();
	void FlushPrintCheckNEC();
	void PrintFlushNEC();
	
	Stack<Event*> NECb;
	Stack<Event*> cb;
};

#endif /*EVENTBUFFER_H_*/
