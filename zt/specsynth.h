#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <complex>
#include <fftw3.h>

void genterrain(float* absterrain, int RESOLUTION, float DISTANCE_LIN, float DISTANCE_EXP, bool symmetrical){
  printf("Allocating arrays\n");
  fftwf_complex* noise = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * RESOLUTION * RESOLUTION);
  fftwf_complex* terrain = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * RESOLUTION * RESOLUTION);
  printf("Making plan\n");
  fftwf_plan p = fftwf_plan_dft_2d(RESOLUTION,RESOLUTION, noise, terrain, FFTW_BACKWARD, FFTW_DESTROY_INPUT);
  printf("Filtering spectra\n");
  for(int i=0; i<RESOLUTION*RESOLUTION/(1+symmetrical); i++){
    int x = i/RESOLUTION;
    int y = i%RESOLUTION;
    double nf = pow(DISTANCE_LIN*sqrt(pow((float)RESOLUTION/2+0.5-x,2)+
				      pow((float)RESOLUTION/2+0.5-y,2))/
                                      (float)RESOLUTION+1,-DISTANCE_EXP);
    noise[i][0] = nf * ((float)(rand()%RAND_MAX))/((float)RAND_MAX) * (0.5-(rand()&1));
    noise[i][1] = nf * ((float)(rand()%RAND_MAX))/((float)RAND_MAX) * (0.5-(rand()&1));
  }
  if(symmetrical){
    int s = 0;
    int d = RESOLUTION*RESOLUTION-1;
    while(true){
      noise[d][0]=noise[s][0];
      noise[d][1]=noise[s][1];
      d--;
      s++;
      if(d<s) break;
    }    
  }
  printf("Executing plan\n");
  fftwf_execute(p);
  fftwf_destroy_plan(p);
  printf("Taking absolute value\n");
  for(int i=0; i<RESOLUTION*RESOLUTION; i++) absterrain[i]=abs(std::complex<float>(terrain[i][0],terrain[i][1]));
  float total = 0;
  float min = 10000000;
  float max =-10000000;
  printf("Preparing for normalization\n");
  for(int i=0; i<RESOLUTION*RESOLUTION; i++){
    float j=absterrain[i];
    total+=j;
    if(j>max) max=j;
    if(j<min) min=j;
  }
  float mean = total/(RESOLUTION*RESOLUTION);
  printf("Total value: %f\n",total);
  printf("Mean value:  %f\n",mean);
  printf("Min value:   %f\n",min);
  printf("Max value:   %f\n",max);
  float d = 0.5;
  float e = -0.2;
  float c = (e*max*(max - mean)*mean - d*max*(max - min)*min - mean*min*(-mean + min))/((max - mean)*(max - min)*(mean - min));
  float b = (-(mean*mean) + e*(-(max*max) + (mean*mean)) + min*min + d*(max-min)*(max+min))/((max - mean)*(max - min)*(mean - min));
  float a = (-(d*max) + e*max + mean - e*mean + (-1 + d)*min)/((max - mean)*(max - min)*(mean - min));
  
  for(int i=0; i<RESOLUTION*RESOLUTION;i++){
    float f = absterrain[i]*absterrain[i]*a+b*absterrain[i]+c;
    if(f<0) f=0;
    absterrain[i] = pow(f,2.5);
  }
  fftwf_free(noise); fftwf_free(terrain);
}
