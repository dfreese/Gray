/*
 *
 * RayTrace Software Package, release 3.0.  May 3, 2006
 *
 * Author: Samuel R. Buss
 *
 * Software accompanying the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.  Please acknowledge
 *   all use of the software in any publications or products based on it.
 *
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG
 *
 */

#ifndef LOAD_OBJ_FILE_H
#define LOAD_OBJ_FILE_H

#include <string>
#include <vector>


class SceneDescription;
class VectorR2;
class VectorR3;
class VectorR4;
// ObjFileLoader are intended for future internal use.

class ObjFileLoader
{

    friend class NffFileLoader;

public:
    ObjFileLoader();

    // The "Load()" routines reads from the file and includes whatever it
    //	knows how to process into the scene.  (If the scene already includes
    //  items, they are left unchanged.)
    bool Load(const std::string & filename, SceneDescription& theScene );
    static bool LoadObjFile(const std::string & filename, SceneDescription& theScene );
    static char* ScanForSecondField( char* inbuf );
    static std::string ScanForSecondField(const std::string & inbuf);

private:
    bool ReportUnsupportedFeatures;
    bool UnsupFlagTextureDepth;
    bool UnsupFlagTooManyVerts;
    bool UnsupFlagLines;
    long FileLineNumber;

private:
    void Reset();

    static char* Preparse( char* inbuf );
    static char* ScanForNonwhite( char* inbuf );
    static char* ScanForWhite( char* inbuf );
    static char* ScanForWhiteOrSlash( char* inbuf );

    static int GetCommandNumber( char *cmd );
    static bool ReadVectorR4Hg( char* inbuf, VectorR4* theVec );
    bool ReadTexCoords( char* inbuf, VectorR2* theVec );
    bool ProcessFace( char *inbuf, SceneDescription& theScene );
    static int NextTriVertIdx( int start, int* step, int totalNum );

    void UnsupportedTextureDepth();
    void UnsupportedLines();
    void UnsupportedTooManyVerts( int maxVerts );
    void AddUnsupportedCmd(const std::string & cmd);
    void PrintCmdNotSupportedErrors( FILE* outstream );

    // Vertices in homogenous format
    std::vector<VectorR4> Vertices;
    // Texture coordinates not supported yet
    std::vector<VectorR2> TextureCoords;
    // Vertex normals not supported yet
    std::vector<VectorR3> VertexNormals;

    std::vector<std::string> UnsupportedCmds;

};

#endif // LOAD_OBJ_FILE_H
