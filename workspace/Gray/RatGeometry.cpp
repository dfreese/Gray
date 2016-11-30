#include "RatGeometry.h"

void RatGeometry::Load() {
	
	int i;
	const double ScaleFactor = 1.0/400.f;
	VectorR3 offset(2.0, (1.0-0.11), 0.0);
	RigidMapR3 rtform;
	VectorR3 raxis(0.0, 1.0, 0.0);
	rtform.SetIdentity();
	rtform.SetRotationPart(raxis, PI);
	double max_x, min_x;
	double max_y, min_y;
	double max_z, min_z;
	
	int whichVertex;
	int j;
	
	FILE * frat = fopen("./rat.txt","w+");

	min_x = 10e100;
	max_x = -10e100;
	min_y = 10e100;
	max_y = -10e100;
	min_z = 10e100;
	max_z = -10e100;
	VectorR3 v[3];
	VectorR3 n[3];
	
	//g_Load3ds.SetCachedNormals("cached_normals.txt");
	g_Load3ds.Import3DS(&g_3DModel, "Labrat/Labrat.3ds");

	t3DObject * pObject;	
	for (i = 0; i < g_3DModel.numOfObjects; i++) {

  		pObject = &g_3DModel.pObject[i];
  		printf("ObjectName[%i]:%s with tex verts%i\n",i,pObject->strName,pObject->numTexVertex);
	}
	
	//g_Load3ds.Export3DS_normals(&g_3DModel, "cached_normals.txt");

	for(i = 32; i <= 35; i++)
	{
			//if (!(i>=5)&&(i<=9)) {
			// Make sure we have valid objects just in case. (size() is in the vector class)
			if(g_3DModel.pObject.size() <= 0) break;

			// Get the current object that we are displaying
			pObject = &g_3DModel.pObject[i];

			// glScalef(1/2000.f, 1/2000.f, 1/2000.f); Dont forget to scale

				// Go through all of the faces (polygons) of the object and draw them
				for(j = 0; j < pObject->numOfFaces; j++)
				{
					// Go through each corner of the triangle and draw it.

					for(whichVertex = 0; whichVertex < 3; whichVertex++)
					{
						// Get the index for each point of the face
						int index = pObject->pFaces[j].vertIndex[whichVertex];
				
						// Make sure the normals were calculated
						if(pObject->pNormals) 
						{
							// Give OpenGL the normal for this vertex.
							//glNormal3f(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);
							n[whichVertex].Set(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);
							rtform.Transform3x3(&n[whichVertex]);
						}

						// Make sure we have valid vertices
						if(pObject->pVerts) 
						{
							// Pass in the current vertex of the object (Corner of current face)
							//printf("Coords are: %f %f %f\n",pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
							//glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
							v[whichVertex].Set(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
							v[whichVertex] *= ScaleFactor;
							rtform.Transform3x3(&v[whichVertex]);
							v[whichVertex] += offset;
							//MaxMin(max_x, min_x, max_y, min_y, max_z, min_z, v[whichVertex]);
						}
					}
					//vt->AddTriangle(v[0],v[1],v[2], n[0], n[1], n[2]);
					fprintf(frat,"p 3\n");
					fprintf(frat,"%f %f %f\n",v[0].x,v[0].y,v[0].z);
					fprintf(frat,"%f %f %f\n",v[1].x,v[1].y,v[1].z);
					fprintf(frat,"%f %f %f\n",v[2].x,v[2].y,v[2].z);
					//printf("object coord:%i: %2.2f %2.2f %2.2f\n", j, v[0].x, v[0].y, v[0].z);
				}
			//}
	}
	fclose(frat);
}