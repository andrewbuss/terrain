#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <complex>
#include <fftw3.h>
#define RESOLUTION 4096
#define DISTANCE_LIN 1500.0
#define DISTANCE_EXP 2.4

void tocsv(char* filename, double* array){
  std::ofstream file;
  file.open(filename);
  file.precision(10);
  int x;
  for(x=0; x<RESOLUTION; x++){
    for(int y=0; y<RESOLUTION; y++){
      int pos = x*RESOLUTION+y;
      if(array[pos]>=0) file << ' ';
      file << std::fixed << array[pos];
      if(y<RESOLUTION-1) file << ',';
    }
    file << '\n';
  }/*
  for(;x<RESOLUTION; x++){
    for(int y=0; y<RESOLUTION; y++){
      int pos = RESOLUTION*RESOLUTION-RESOLUTION*x-y-1;
      if(array[pos]>=0) file << ' ';
      file << std::fixed << array[pos];
      if(y<RESOLUTION-1) file << ',';
    }
    file << '\n';s
    }*/
  file.close();
}

int main(){
  printf("Allocating arrays\n");
  fftw_complex* noise = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * RESOLUTION * RESOLUTION);
  fftw_complex* terrain = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * RESOLUTION * RESOLUTION);
  //std::complex<double>* noise = (std::complex<double>*) fftw_malloc(sizeof(std::complex<double>) * RESOLUTION * RESOLUTION);
  //std::complex<double>* terrain = (std::complex<double>*) fftw_malloc(sizeof(std::complex<double>) * RESOLUTION * RESOLUTION);
  double* absterrain = (double*) malloc(sizeof(fftw_complex) * RESOLUTION * RESOLUTION);
  printf("Making plan\n");
  fftw_plan p = fftw_plan_dft_2d(RESOLUTION,RESOLUTION, noise, terrain, FFTW_BACKWARD, FFTW_DESTROY_INPUT || FFTW_ESTIMATE);
  printf("Filtering spectra\n");
  for(int i=0; i<RESOLUTION*RESOLUTION; i++){
    int x = i/RESOLUTION;
    int y = i%RESOLUTION;
    double nf = pow(DISTANCE_LIN*sqrt(pow((double)RESOLUTION/2-0.5-x,2)+
				      pow((double)RESOLUTION/2-0.5-y,2))/
                                      (double)RESOLUTION+1,-DISTANCE_EXP)*2;
    noise[i][0] = nf * ((double)(rand()%RAND_MAX))/((double)RAND_MAX) * (0.5-(rand()&1));
    noise[i][1] = nf * ((double)(rand()%RAND_MAX))/((double)RAND_MAX) * (0.5-(rand()&1));
  }
  printf("Executing plan\n");
  fftw_execute(p);
  fftw_destroy_plan(p);
  printf("Taking absolute value\n");
  for(int i=0; i<RESOLUTION*RESOLUTION; i++) absterrain[i]=abs(std::complex<double>(terrain[i][0],terrain[i][1]));
  double total = 0;
  double min = 10000000;
  double max =-10000000;
  printf("Preparing for normalization\n");
  for(int i=0; i<RESOLUTION*RESOLUTION; i++){
    double j=absterrain[i];
    total+=j;
    if(j>max) max=j;
    if(j<min) min=j;
  }
  double mean = total/(RESOLUTION*RESOLUTION);
  printf("Total value: %f\n",total);
  printf("Mean value:  %f\n",mean);
  printf("Min value:   %f\n",min);
  printf("Max value:   %f\n",max);
  double m=(2.0/(max-min));
  double b=1.0+(2.0*max/(min-max));
  printf("Normalizing with m = %f and b = %f\n",m,b);
  for(int i=0; i<RESOLUTION*RESOLUTION;i++) absterrain[i]=absterrain[i]*m+b;
  printf("Saving to file\n");
  tocsv("absterrain.csv",absterrain);
  fftw_free(noise); fftw_free(terrain);
  free(absterrain);
  return 0;
}
