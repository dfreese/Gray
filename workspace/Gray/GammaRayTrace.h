#ifndef GAMMARAYTRACE_H
#define GAMMARAYTRACE_H

#include "../Physics/Interaction.h"
#include "../Physics/MaterialStack.h"
#include "../Physics/Photon.h"
#include "../Physics/Positron.h"
#include "../Physics/GammaDecay.h"
#include "../Physics/InteractionList.h"
#include "../Sources/SphereSource.h"
#include "../Sources/RectSource.h"
#include "../Sources/SourceList.h"
#include "../Output/Output.h"
#include "RatGeometry.h"

extern long SeekIntersectionKd(const VectorR3& startPos, const VectorR3& direction,
										double *hitDist, VisiblePoint& returnedPoint,
										long avoidK = -1);
										
const double Epsilon = 1e-10;
const double s1_SOL = (1.0 / 29979245800.0); 
									
class GammaRayTrace {
	public:
		GammaRayTrace();
		~GammaRayTrace();
		void GRayTraceSources(void);
		void AddSource(Source & s);
		void SetDefaultMaterial(MaterialBase * mat);
		void SetFileNameOutput(char * name);
		void SetSimulationTime(double time) { simulationTime = time; };
		void SetLogPositron(bool val);
		void SetLogAll(bool val);
		Output output;
		InteractionList interactions;
		SourceList sources;
	private:
		INTER_TYPE GRayTrace(Interaction &interaction, VisiblePoint &visPoint, int TraceDepth, Photon &photon, 
			  MaterialStack& MatStack, InteractionList & i, Output &o, long avoidK ) ;
		VectorR3 positionCenter;

		MaterialBase * defaultMat;
		char * FileNameOutputFile;
		double simulationTime;
		RatGeometry rat;
		bool logPositron;
		bool logAll;

};

#endif /*GAMMARAYTRACE_H*/
