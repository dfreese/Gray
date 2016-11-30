#ifndef LOAD_DETECTOR_H_
#define LOAD_DETECTOR_H_

#include "../DataStructs/Array.h"
#include "SceneDescription.h"
#include "../VrMath/LinearR2.h"
#include "../VrMath/LinearR3.h"
#include "../VrMath/LinearR4.h"
#include "../Graphics/TransformViewable.h"
#include "../Graphics/Material.h"
#include "../DataStructs/Stack.h"
#include "../Gray/GammaRayTrace.h"
#include "../Graphics/ViewableTriangle.h"
#include "../Graphics/CameraView.h"
#include "../Graphics/ViewableSphere.h"
#include "../Graphics/ViewableEllipsoid.h"
#include "../Sources/SphereSource.h"
#include "../Sources/VoxelSource.h"
#include "../Sources/RectSource.h"
#include "../Sources/CylinderSource.h"
#include "../Sources/AnnulusCylinderSource.h"
#include "../Sources/BeamPointSource.h"
#include "../Sources/VectorSource.h"
#include "../Sources/EllipsoidSource.h"
#include "../Sources/EllipticCylinderSource.h"
#include "../Sources/AnnulusEllipticCylinderSource.h"


//class ObjFileLoader;
//class CameraView;
#define MAX_INCLUDE 256

bool LoadDffFile( const char* filename, SceneDescription& theScene, GammaRayTrace &Gray );

class LoadDetector {
public:
	LoadDetector();
	bool Load( const char* filename, SceneDescription& theScene, GammaRayTrace &Gray );
private:
		SceneDescription* ScenePtr;
		long FileLineNumber;
		int GetCommandNumber( const char * cmd );
		bool ReadVertexR3( VectorR3& vert, FILE* infile );
		void ProcessDetector( const VectorR3& detCenter, const VectorR3& detSize, const Material* curMaterial, int id );
		void SetCameraViewInfo( CameraView& theView,
						const VectorR3& viewPos, const VectorR3& lookAtPos, 
						const VectorR3& upVector, double fovy,
						int screenWidth, int screenHeight, double nearClipping );
		void PushMatrix();
		void PopMatrix();
		void ApplyTranslation(const VectorR3&t);
		void ApplyRotation(const VectorR3& axis, double theta);
		bool ProcessFaceDFF( int numVerts, const Material* mat, FILE* infile, VectorSource * s, bool parse_VectorSource, unsigned id );
		RigidMapR3 &curMatrix();
		Stack<RigidMapR3*> MatrixStack;
		Material defaultBack;
		Material defaultSphere;
		double polygonScale;
		double actScale;
		
		double time_resolution;
		double energy_resolution;

		double time_gate;
		double energy_gate_lower;
		double energy_gate_upper;

		bool positronRange;
		bool positronRangeCusp;
		double positronC;
		double positronK1;
		double positronK2;
		double positronMaxRange;
		unsigned int block_id;

		int global_id;
	
		
};


#endif /*LOAD_DETECTOR_H_*/
