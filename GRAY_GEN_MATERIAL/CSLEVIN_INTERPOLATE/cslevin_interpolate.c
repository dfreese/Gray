#include <stdlib.h>
#include <stdio.h>

double energies[16] = { 10.000, 15.000, 20.000, 30.000, 40.000, 50.000, 60.000, 80.000, 100.000, 150.000, 200.000, 300.000, 400.000, 500.000, 511.000, 600.000};


double main(int argc, char* argv[]){
  int i;
  double input,value;
  double LUT[16] = {6.731, 6.679, 6.628, 6.532, 6.442, 6.356, 6.273, 6.121, 5.982, 5.677, 5.420, 5.006, 4.682, 4.417, 4.390, 4.194};
  input = strtod (argv[1], NULL);
  //  printf("Input Value : %lf\n", input);
  i = getindex(input);
  //  printf("i= %d\n",i);
  if (i==0)  {
    //      printf("%lf smaller then %lf\n", input,energies[i]);
    printf("%.6lf\n",LUT[0]); 
      return 0; }
  else {
    if (i==15) {
      //          printf("%lf larger then %lf\n", input,energies[i]); 
    printf("%.6lf\n",LUT[15]); 
          return 0; }
    else {
      //   printf("%lf between %lf and %lf\n", input,energies[i], energies[i+1]);  
       	double delta = energies[i] - energies[i-1];
        double alpha = (input - energies[i-1])/delta;
	value = (1.0-alpha) * LUT[i-1] + alpha*LUT[i];
        printf("%.6lf\n",value); 
         }
  }
  return 0;}

int getindex(double k){
  int i;
   for (i=0;i<16;i++){
     if (k <= energies[i]) return i ;}
   return i-1;
}
