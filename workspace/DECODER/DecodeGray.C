#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define FILENAMELENGTH 120

#pragma pack(push, 1)

struct GRAY_BINARY {
   double time;
   float energy;
   float x,y,z; 
   int log;
  int i;
  int det_id;
 };
#pragma pack(pop)

int DecodeLogWord(int word, int &interaction, int &color, int &scatter, int &mat_id, int &src_id);
void Usage(void);


int main(int argc, char *argv[]){
  struct GRAY_BINARY p;
  char filename[FILENAMELENGTH]="";
char outfilename[FILENAMELENGTH+6]="";
  int verbose=0;
  int ix,i,fileSize,nrlines;
 int p_int, p_col, p_scat, p_mat, p_src; 
 FILE *f,*ff;
	char str[256];

 //Reading input arguments
 for ( ix=1;ix< argc;ix++){
    //    cout << argv[ix] << endl;
    /* Verbose  '-v' */
    if (strncmp(argv[ix], "-v", 2) == 0) {
      verbose = 1;
    }
    /* filename '-f' */
    if (strncmp(argv[ix], "-f", 2) == 0){
      if (strlen(argv[ix+1])<FILENAMELENGTH) {  
        sprintf( filename,"%s", argv[ix+1]);}
      else  {
        cout << "Filename " << argv[ix+1] << " too long !" <<endl;
        cout << "Exiting.." <<endl;
        return -99;}
    }
     if (strncmp(argv[ix], "-h", 2) == 0){
       cout << " Usage .. " << endl;
       cout << "  -v             :: verbose mode " << endl;
       cout << "  -f [filename]  :: translation vector " << endl;
     }
} 

 if (strcmp(filename,"") == 0) {
   cout << "Please specify input file !" << endl;
   Usage();
   return -98;}

 f=fopen(filename,"r");  

// obtain file size:
  fseek (f , 0 , SEEK_END);
  fileSize = ftell (f);
  rewind (f);
  nrlines = fileSize/sizeof(p);
  if  ( fileSize % (sizeof(p)) != 0 ) { cout << " something's wrong ! " << endl;
    cout << " fileSize = " << fileSize << " is not a multiple of the data struct size " << sizeof(p) <<" : " << fileSize << "/" << sizeof(p) << " = " << fileSize/sizeof(p) << endl; return -1;}
  cout << " sizeof(p) = " << sizeof(p) << endl;
  cout << " fileSize = " << fileSize << endl;
  cout << " nrlines = " << nrlines << endl;

 sprintf(outfilename,"%s.ascii",filename);
 ff=fopen(outfilename,"w");


 for (i=0;i<nrlines;i++) {
 fread(&p,sizeof(p),1,f);
 // printf("0x%x",p.log) ;
 DecodeLogWord(p.log, p_int, p_col, p_scat, p_mat,  p_src);
 fprintf(ff," %d %d %d %23.16e %12.6e %15.8e %15.8e %15.8e ",p_int, p.i, p_col, p.time, p.energy, p.x, p.y, p.z);
 if ( p_int ){ fprintf(ff,"%2d  %d  %d  %d", p_src, p_scat, p_mat, p.det_id );fprintf(ff,"  %14.8e %14.8e %14.8e\n",0.0f, 0.0f, 0.0f);}
 else fprintf(ff," %d\n", p.det_id);
 }

 fclose(f);
 fclose(ff);

 return 0;}

int DecodeLogWord(int word, int &interaction, int &color, int &scatter, int &mat_id, int &src_id){
  int errorbit=0;

  src_id = word & 0xFFF;
  mat_id = ( word >> 12) & 0XFFF;
  scatter = (word >> 24 ) & 0x1;
  color = ( word >> 26 ) & 0x3;
  interaction = ( word >> 28 ) & 0x7;
  errorbit = ( word >> 31) & 0x1;
  //  cout << " " << mat_id << " " << src_id << endl;
  if ( errorbit ) interaction *= -1;
  return 0;}
 
// return (  errorbit | ( ( interaction << 28) & (0x70000000))   | ( (color << 26 ) & 0xC000000 )| ((scatter << 24) &0x1000000 )| (( mat_id << 12 ) & (0xFFF000)) | ( src_id & 0xFFF ) );    

void Usage(void){
       cout << " :::::: Usage :::::. " << endl;
       cout << "  -v             :: verbose mode " << endl;
       cout << "  -h             :: help " << endl;
       cout << "  -f [filename]  :: inputfilenam " << endl;
       cout << " A file [filename].ascii will be created " << endl;
  return;}
