#ifndef BINARYFORMAT_H
#define BINARYFORMAT_H

#include <stdint.h>


//struct GRAY_BINARY {
/*		Type of event:
 * 		1 -- compton
 * 		2 -- x-ray
 * 		3 -- photoelectric
 */
  // uint8_t t;
/*		Event Index */
  //  uint32_t i;
/*		Red/Blue */
//	uint8_t rnb;
/* be careful of floating point formats */
//	double time;
  //	float energy;
  //	float x,y,z;
/* detector id */
//	uint32_t det_id;
/* material id */
//	uint8_t m;
  //};

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


#endif /*BINARYFORMAT_*/
