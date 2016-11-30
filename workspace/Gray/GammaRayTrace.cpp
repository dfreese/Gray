#include "GammaRayTrace.h"

GammaRayTrace::GammaRayTrace() {
	defaultMat = NULL;
	FileNameOutputFile = NULL;
	
	// Default simulation time is 1 second
	simulationTime = 1.0;
	SetLogPositron(false);
	SetLogAll(false);
	
}

// Garry: Added destructor for proper cleanup
GammaRayTrace::~GammaRayTrace() {
 
}


void GammaRayTrace::SetFileNameOutput(char * name) {
	FileNameOutputFile = name;
}

void GammaRayTrace::SetDefaultMaterial(MaterialBase * mat) {
	defaultMat = mat;
}

INTER_TYPE GammaRayTrace::GRayTrace(Interaction &interaction, 
	VisiblePoint &visPoint, int TraceDepth, Photon &photon, 
	MaterialStack& MatStack, InteractionList &i, Output &o, long avoidK=-1) 
{
	double hitDist;
	MaterialBase * curMaterial = MatStack.curMaterial();
	
	if (curMaterial == ERROR_MATERIAL) {
	  o.LogError(photon, ERROR_EMPTY,  0);
		return ERROR;
	}

	MaterialBase * nextMaterial;

	if (TraceDepth <= 0) {
	  o.LogError(photon, ERROR_TRACE_DEPTH, curMaterial->GammaProp->GetMaterial() );
	   return NO_INTERACTION;
	}

	int intersectNum = SeekIntersectionKd(photon.pos,photon.dir,
								&hitDist,visPoint,avoidK );

	if ( intersectNum<0 ) {
		return NO_INTERACTION;
	//	cout << "== No Intersection ==\n";
	} else {
		INTER_TYPE inter_type;
		// set detector id in photon
		switch(interaction.GammaInteraction(photon, hitDist, *curMaterial, i, o)) {
			case PHOTOELECTRIC:
				return PHOTOELECTRIC;
			break;
			case XRAY_ESCAPE:
			case COMPTON:
				return GRayTrace(interaction, visPoint, TraceDepth - 1, photon,MatStack, i, o, avoidK);
				return COMPTON;
			break;
			case NO_INTERACTION:
			{
				// If not interaction, recursively traverse the in the direction the photon was travelling
				if (visPoint.IsFrontFacing()) {	
					// to enter a detector, we must first go into it, then out
					switch (visPoint.GetObject().GetViewableType()) {
						case ViewableBase::Viewable_Triangle:
							// This detector id will be used to determine if we scatter in a detector
							// or inside a phantom
							photon.det_id = ((const ViewableTriangle&)(visPoint.GetObject())).GetDetectorId();
						break;
						default:
							photon.det_id = -1;
						break;
					}
					nextMaterial = const_cast<MaterialBase*>(&visPoint.GetMaterial());
					MatStack.PushMaterial(nextMaterial);
				} else if (visPoint.IsBackFacing()) {
					photon.det_id = -1;
					MaterialBase * mat = MatStack.PopMaterial();
				} else {
					cout << "ERROR: material has no face\n";
					exit(1);
				}
				// calculate the time taken to travel distance of the non-interaction
				photon.time += (hitDist * s1_SOL);
				
				// Make sure not to hit same place in kdtree
				photon.pos = visPoint.GetPosition() + photon.dir * Epsilon;
				inter_type = GRayTrace(interaction, visPoint, TraceDepth - 1, photon, MatStack, i, o, avoidK);
				switch(inter_type) {
					case PHOTOELECTRIC:
					case COMPTON:
					case ESCAPE_INTERACTION:
						return inter_type;
					break;
					default:
						return NO_INTERACTION;
					break;
				}
			}
			break;
			default:
				cout << "ERROR: Interaction not specified\n";
				return NO_INTERACTION;
			break;
		}
	}
}

void GammaRayTrace::SetLogPositron(bool val) {
	logPositron = val;
}

void GammaRayTrace::SetLogAll(bool val) {
	logAll = val;
}

void GammaRayTrace::GRayTraceSources(void) {
	
	//rat.Load();
	const char escape = 0x1B;
	const int num_chars = 70;
	int char_state = 0;
	int tick_mark = 0;
	
	int num_rays = 0;
	
	// calculate the number of positrons to throw
	// TODO: need to fix the number of rays because of negative sources
	// FIXME: time should not increase when Inside() of a negative source
	num_rays = sources.GetTotalEvents(simulationTime);
	
	// create a tick mark so that we fill up 40 chars
	tick_mark = (int)(num_rays / num_chars);
	
	// done for PET Benchmark
	//num_rays = 34076000;
	//num_rays = 2938374;
	
	MaterialStack MatStack;
	MatStack.SetDefault(defaultMat);
	MatStack.PushMaterial(defaultMat);

	VisiblePoint visPoint;
	int TraceDepth;
	
	//TODO: Make a photon stack, then race trace it
	Interaction interaction;
//	Output output;

	output.SetLogfile(FileNameOutputFile);
	output.SetLogAll(logAll);
	
	// text graphics preamble
	cout << "|";
	
	Isotope * isotope = NULL;
	Photon photon;

	for (int i = 1; i < num_rays; i++) {
		isotope = sources.Decay();

		if (logPositron == true) output.LogNuclearDecay(((Positron*)isotope)->GetPositron());
				
		if ((i % 10000)==0) {
		  	switch(char_state) {
				case 0:
					cout << "\033[1m|\033[0m";
					break;
				case 1:
					cout << "\033[1m/\033[0m";
					break;
				case 2:
					cout << "\033[1m=\033[0m";
					break;
				case 3:
					cout << "\033[1m\\\033[0m";
					break;
				default:
					cout << "\033[1m|\033[0m";
				break;
			}
			
			char_state++;
			if (char_state == 4) char_state = 0;
	
			cout << escape;
			cout << "[1D";
		
			cout.flush();
		}

		// Fun ANSI graphics to do while waiting for simulation
		// this is a simple spinner code
		if ((i % tick_mark) == 0) {
			cout << "\033[32m=\033[0m";
			cout.flush();
		}

		interactions.Reset();
		interactions.HitPositron((Positron&)*isotope,((Positron*)isotope)->GetEnergy());
		while(!isotope->IsEmpty()) {
			if (isotope == NULL) {
				cerr << "Empty Decay: ERROR\n";
				continue;
			}

			photon = isotope->NextPhoton();

			TraceDepth = 100;
			MatStack.PushMaterial(isotope->GetMaterial());
			if (GRayTrace( interaction, visPoint, TraceDepth, photon, MatStack, interactions, output ) == ERROR) {
				cout << "ERROR\n";
			}
			MatStack.ResetMaterial();
		}
		//cout << "DEBUG INTERACTIONS\n";
		//cout << interactions;
	}
	//cout << "=|Done.\n";
}

void GammaRayTrace::AddSource(Source & s) {
	sources.AddSource(s);
}
