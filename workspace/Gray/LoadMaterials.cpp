#include "LoadMaterials.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

const int MaxnumFiles = 256;
int numFiles =20;
char* physicalMaterial[MaxnumFiles] ;
bool phyiscalMaterialLogInteractions[MaxnumFiles] = {
    false,
    false,
    true,
    false,
    true,
    false,
    false,
    true,
    false

};

LoadMaterials myMatLoader;
bool LoadPhysicsFiles( SceneDescription& theScene )
{
    ifstream matfile;
    string matstring;
    char matfilelocation[256];
    char INCLUDE_LOCATION[256]="";
    char matname[256];
    char matchemform[256];
    float matdens;
    int matsensitive;
    int matnumber;
    //       cout << phyiscalMaterialLogInteractions[32] << endl;
    char * pPath;
    pPath = getenv ("GRAY_INCLUDE");
    if (pPath==NULL) {
        printf("No GRAY_INCLUDE variable set !\n");
        printf("Execute export GRAY_INCLUDE=yourpath\n");

    } else {
        sprintf(INCLUDE_LOCATION,"%s",pPath);
    }

    //       sprintf(matfilelocation,"%s/Gray_Materials.txt",INCLUDE_LOCATION);
    sprintf(matfilelocation,"%s/Gray_Materials.txt",INCLUDE_LOCATION);
    //       cout << INCLUDE_LOCATION <<endl;
    matfile.open(matfilelocation,ifstream::in);

    int i;
    for (i=0; i<MaxnumFiles; i++) {
        physicalMaterial[i] = new char[256];
    }

    if ( !matfile.is_open() ) {
        cout << matfilelocation << " not found. Using hard coded material table " << endl;

        sprintf(physicalMaterial[0],"air2.dat");
        sprintf(physicalMaterial[1],"al.dat");  // 1
        sprintf(physicalMaterial[2],"bgo2.dat"); // 2
        sprintf(physicalMaterial[3],"lead2.dat");// 3
        sprintf(physicalMaterial[4],"lso.dat"); // 4
        sprintf(physicalMaterial[5],"tungsten2.dat");// 5
        sprintf(physicalMaterial[6],"water.dat"); // 6
        sprintf(physicalMaterial[7],"czt.dat"); // 7
        sprintf(physicalMaterial[8],"alumina.dat"); // 8
        sprintf(physicalMaterial[9],"lyso.dat"); //9

    } else {
        numFiles=0;
        while (1) {
            matfile >> matname;
            if (matfile.eof()) {
                break;
            }
            //
            //            matstring >> matname;
            //	    cout << matname << endl;
            //       cout << "matname = " << matname << endl;
            if ( strncmp(matname,"#",1) ) {
                numFiles++;
                matfile >> matchemform;
                matfile >> matdens;
                matfile >> matsensitive;
                matfile >> matnumber;
//cout << "Material Found :: " << matname << " number : " << matnumber<< endl;
                sprintf(physicalMaterial[matnumber],"%s.dat",matname);
                phyiscalMaterialLogInteractions[matnumber]=matsensitive;
            } else {
                //                 cout << "Comment " <<endl;
                getline(matfile,matstring);
            }
        }
    }

    cout << numFiles << " Materials Found " <<endl;
    matfile.close();
    return myMatLoader.Load(numFiles, theScene );
}


bool LoadMaterials::Load(int numMaterial, SceneDescription& theScene )
{
    bool parseOk = true;
    numMaterialLoaded = 0;
    for (int i = 0; i < numMaterial; i++) {
        GammaMaterial * mat = theScene.NewMaterial();
        mat->SetName(physicalMaterial[i]);
        GammaStats * stats = &mat->GammaProp;
        stats->SetFileName(physicalMaterial[i]);
        stats->SetMaterialType( i) ;
        if (!stats->Load()) {
            parseOk = false;
        } else {
            stats->log_material = phyiscalMaterialLogInteractions[i];

            if (i == 0)
                if (i == 0) { // First material is default material
                    stats->DisableInteractions();
                }
            theScene.AddGammaStats(stats);
            numMaterialLoaded++;
        }
    }
    return parseOk;
}
