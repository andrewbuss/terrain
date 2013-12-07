#pragma once

#include <math.h>

inline float rsqrt(float x){
  // Carmack's
  long i;
  float x2, y;
  const float threehalfs = 1.5F;
  x2 = x * 0.5F;
  y  = x;
  i  = * ( long * ) &y;
  i  = 0x5f3759df - ( i >> 1 );            
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );
}
  
class v3{
public:
  float x;
  float y;
  float z;
  v3(void){} 
  v3(float X, float Y, float Z){
    x = X;
    y = Y;
    z = X;
  }
  float length(){
    return sqrt(x * x + y * y + z * z);
  }
  inline float rlength(){
    return rsqrt(x*x+y*y+z*z);
  }
  v3 normalized(){
    float nf = rlength();
    return v3(x*nf, y*nf, z*nf);
  }
  void normalize(){
    float nf = rlength();
    x*=nf;
    y*=nf;
    z*=nf;
  }
};
