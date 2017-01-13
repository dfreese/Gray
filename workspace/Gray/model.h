
#ifndef LOGO3D_MODEL_H
#define LOGO3D_MODEL_H

//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <vector>
//#include <GL\gl.h>										// Header File For The OpenGL32 Library
//#include <GL\glu.h>										// Header File For The GLu32 Library
//#include <GL\glaux.h>
//#include <crtdbg.h>
using namespace std;

#define MAX_TEXTURES 100								// The maximum amount of textures to load

// This is our 3D point class.  This will be used to store the vertices of our model.
class CVector3
{
public:
    float x, y, z;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
////////// *** CHANGE *** /////// *** CHANGE *** /////////// *** CHANGE *** //////////////////

    // These operator overloads were added so that this type
    // could be treated more like an object.

    const CVector3 &operator=(const CVector3& Right)
    {
        x = (float)Right.x;
        y = (float)Right.y;
        z = (float)Right.z;
        return *this;
    }

    const CVector3 &operator+=(const CVector3& Right)
    {
        x += (float)Right.x;
        y += (float)Right.y;
        z += (float)Right.z;
        return *this;
    }

    const CVector3 &operator-=(const CVector3& Right)
    {
        x -= (float)Right.x;
        y -= (float)Right.y;
        z -= (float)Right.z;
        return *this;
    }

    CVector3 operator+(const CVector3& Right)
    {
        CVector3 temp;
        temp.x = x + Right.x;
        temp.y = y + Right.y;
        temp.z = z + Right.z;
        return temp;
    }

    CVector3 operator-(const CVector3& Right)
    {
        CVector3 temp;
        temp.x = x - Right.x;
        temp.y = y - Right.y;
        temp.z = z - Right.z;
        return temp;
    }
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
////////// *** CHANGE *** /////// *** CHANGE *** /////////// *** CHANGE *** //////////////////
};

// This is our 2D point class.  This will be used to store the UV coordinates.
class CVector2
{
public:
    float x, y;
};

// This is our face structure.  This is is used for indexing into the vertex
// and texture coordinate arrays.  From this information we know which vertices
// from our vertex array go to which face, along with the correct texture coordinates.
struct tFace {
    int vertIndex[3];			// indicies for the verts that make up this triangle
    int coordIndex[3];			// indicies for the tex coords to texture this face
};

// This holds the information for a material.  It may be a texture map of a color.
// Some of these are not used, but I left them because you will want to eventually
// read in the UV tile ratio and the UV tile offset for some models.
struct tMaterialInfo {
    char  strName[255];			// The texture name
    char  strFile[255];			// The texture file name (If this is set it's a texture map)
    char  color[3];				// The color of the object (R, G, B)
    int   texureId;				// the texture ID
    float uTile;				// u tiling of texture  (Currently not used)
    float vTile;				// v tiling of texture	(Currently not used)
    float uOffset;			    // u offset of texture	(Currently not used)
    float vOffset;				// v offset of texture	(Currently not used)
} ;

// This holds all the information for our model/scene.
// You should eventually turn into a robust class that
// has loading/drawing/querying functions like:
// LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject {
    int  numOfVerts;			// The number of verts in the model
    int  numOfFaces;			// The number of faces in the model
    int  numTexVertex;			// The number of texture coordinates
    int  materialID;			// The texture ID to use, which is the index into our texture array
    bool bHasTexture;			// This is TRUE if there is a texture map for this object
    char strName[255];			// The name of the object

    int  positionFrames;		// The number of key frames for the position
    int  rotationFrames;		// The number of key frames for the rotations
    int  scaleFrames;			// The number of key frames for the scaling

    CVector3 vPivot;			// The object pivot point (The local axis it rotates around)

    vector<CVector3> vPosition;	// The object's current position list
    vector<CVector3> vRotation;	// The object's current rotation list
    vector<CVector3> vScale;	// The object's current scale list

    vector<float> vRotDegree;	// The object's current rotation degree (associated with vRotation)

    CVector3  *pVerts;			// The object's vertices
    CVector3  *pNormals;		// The object's normals
    CVector2  *pTexVerts;		// The texture's UV coordinates
    tFace *pFaces;				// The faces information of the object

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
////////// *** CHANGE *** /////// *** CHANGE *** /////////// *** CHANGE *** //////////////////

    // These are the variables that make the object hierarchy possible
    unsigned short m_NodeID;	// The primary key for each animation node
    t3DObject* m_ParentNode;	// Points to the parent node of the object
    bool m_GroupToggle;			// This indicates whether the object is a group
    bool m_LinkedObjectToggle; // Indicates a linked object hierachy

    // Altered the default values for the object hierarchy variables
    //constructor
    t3DObject()
    {
        m_NodeID = 65535;
        m_ParentNode = NULL;
        m_GroupToggle = 0;
        m_LinkedObjectToggle = 0;
    }
    //destructor
    ~t3DObject()
    {
        m_NodeID = 65535;
        m_ParentNode = NULL; // don't want to delete this reference pointer
        m_GroupToggle = 0;
        m_LinkedObjectToggle = 0;
    }
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
////////// *** CHANGE *** /////// *** CHANGE *** /////////// *** CHANGE *** //////////////////
};

// This holds our model information.  This should also turn into a robust class.
// We use STL's (Standard Template Library) vector class to ease our link list burdens. :)
struct t3DModel {
    int numOfObjects;			// The number of objects in the model
    int numOfMaterials;			// The number of materials for the model

    int numberOfFrames;			// The number of frames of animation this model (at least 1)
    int currentFrame;			// The current frame of animation that the model is in

    vector<tMaterialInfo> pMaterials;	// The list of material information (Textures and colors)
    vector<t3DObject> pObject;			// The object list for our model

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
////////// *** CHANGE *** /////// *** CHANGE *** /////////// *** CHANGE *** //////////////////

    // Holds the path and file name of the model
    char *m_strFileName;

    // The master scale value that should be applied to all
    // the scale track values for each frame of the animation.
    float MasterScale;

    // This is a hash array that allows nodes to be referred by node ids
    // [ NodeID ] == Object Index
    vector<unsigned short> m_NodeHash;

    // This holds the number of animation nodes
    unsigned short m_KeyNumberOfNodes;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
////////// *** CHANGE *** /////// *** CHANGE *** /////////// *** CHANGE *** //////////////////
};

#endif
