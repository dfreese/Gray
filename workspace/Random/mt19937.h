/*-----------------------------------------------------------
 * Util.hh
 *
 *----------------------------------------------------------*/
#ifndef UTIL_H
#define UTIL_H

/** Seed the Mersenne Twister random number generator */
void sgenrand(unsigned long seed);

/** generate the next random number */
double genrand();

/** generate a random integer from 0 .. i */
//int randint(int i);

/** randomly shuffle numbers */
//void shuffle(int n, int * list);

/** Gaussian random number with zero mean, unit variance */
//double gasrand();

#endif // Util.hh

