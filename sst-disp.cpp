#define GL_GLEXT_PROTOTYPES
#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <stdlib.h>
#include "sst.h"
#include <time.h>
#include <math.h>
#include "SDL/SDL_thread.h"
#include "SDL/SDL_mutex.h"
#include "SDL/SDL_audio.h"

#define XRES 720
#define YRES 480
#define smooth(x, a) a*pow(sin((3.14159265*x)/(2.0*a)),2)
#define smoothe(x, a) a*sin((3.14159265*x)/(2.0*a))

float vbopoints[RESOLUTION*RESOLUTION][3];
float colors[RESOLUTION*RESOLUTION][4];
float cx, cz, cy, ex, ey, ez, czoom, ed;
float distance[RESOLUTION*RESOLUTION];
float distance2[RESOLUTION*RESOLUTION];
float distance3[RESOLUTION*RESOLUTION];
clock_t starttime;
bool running = true;
bool colorchanged = true;
float pointsize = 0;
double* points = (double*)malloc(sizeof(double)*RESOLUTION*RESOLUTION);
SDL_mutex* lock;
float t;
int frames = 0;

void RenderNow(){
  printf("Rendering now\n");
  t = difftime(clock(),starttime)/CLOCKS_PER_SEC;
  frames++;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt((czoom*sin(t/2))*ed+cx,czoom*ey,(czoom*cos(t/2))*ed+cz,cx,cy,cz,0,1,0);
  glPointSize(pointsize);
  glDrawArrays(GL_POINTS, 0, RESOLUTION*RESOLUTION);
  SDL_GL_SwapBuffers();
}

int script(void* data){
  printf("Starting script\n");
  int lx=300;
  int rx=400;
  int bz=200;
  int tz=300;
  float zcx=((rx-lx)/2+lx-RESOLUTION/2)/(float)RESOLUTION;
  float zcz=((tz-bz)/2+bz-RESOLUTION/2)/(float)RESOLUTION;
  int lx2=360;
  int rx2=380;
  int bz2=260;
  int tz2=280;
  float zcx2=((rx2-lx2)/2+lx2-RESOLUTION/2)/(float)RESOLUTION;
  float zcz2=((tz2-bz2)/2+bz2-RESOLUTION/2)/(float)RESOLUTION;
  printf("ZCX: %f | ZCZ: %f\n",zcx,zcz);
  printf("ZCX2: %f | ZCZ2: %f\n",zcx2,zcz2);
  printf("Applying basic colors\n");
  for(int i=0; i<RESOLUTION*RESOLUTION;i++){
    float x=(i%RESOLUTION)/(float)RESOLUTION-0.5;
    float y=i/RESOLUTION/(float)RESOLUTION-0.5;
    if(points[i]>=0){
      points[i]=pow(points[i],1.5)+0.01;
      colors[i][3]=0;
      vbopoints[i][0] = 10*x;
      vbopoints[i][1] = points[i];
      vbopoints[i][2] = 10*y;
      colors[i][1] = 1.0-vbopoints[i][1]/2;
      colors[i][2] = 0.76-0.2*vbopoints[i][1];
      colors[i][0] = 0.6;
      if(vbopoints[i][1]>0.8){
	colors[i][0]=2*vbopoints[i][1]-1;
	colors[i][1]=2*vbopoints[i][1]-1;
	colors[i][2]=2*vbopoints[i][1]-1;
      }
      else if(vbopoints[i][1]<0.02){
	colors[i][2]=0.5;
	colors[i][1]=0.8;
	colors[i][0]=0.7;
      }
    }
    distance[i]=sqrt(x*x+y*y);
    distance2[i]=sqrt((zcx-x)*(zcx-x)+(zcz-y)*(zcz-y));
    distance3[i]=sqrt((zcx2-x)*(zcx2-x)+(zcz2-y)*(zcz2-y));
  }
  SDL_Delay(2000);
  printf("Turning screen on\n");
  float d=0;
  RenderNow();
  while(d<1.0){
    d+=0.01;
    float s = sin(3.141592/2.0*d)*sin(3.141592/2.0*d);
    for(int i=0; i<RESOLUTION*RESOLUTION; i++) colors[i][3] = (s-2*distance[i]);
    SDL_Delay(10);
    RenderNow();
  }
  SDL_Delay(500);
  d=0;
  printf("Applying gridlines\n");
  while(d<=4){
    d+=0.03;
    SDL_mutexP(lock);
    for(int i=0; i<RESOLUTION*RESOLUTION; i++){
      if((i%RESOLUTION)+3*(i/RESOLUTION)-8*vbopoints[i][1]<smoothe(d,4)*RESOLUTION){
	if((i%RESOLUTION)%25 == 0){
	  colors[i][1]=1;
	  colors[i][0]=1.5-vbopoints[i][1];
	  colors[i][3]=1.5;
	  if(points[i]<0){
	    vbopoints[i][0]=10*(i%RESOLUTION)/(float)RESOLUTION-5.0;
	    vbopoints[i][1]=0.001;
	    vbopoints[i][2]=10*i/(float)(RESOLUTION*RESOLUTION)-5.0;
	  }
	  if((i/RESOLUTION)%25 < 2 || (i/RESOLUTION)%25==24){
	    colors[i][1]=0;
	    colors[i][3]=1.5;
	    colors[i][2]=0;
	  }
	}
      }
      if(3*(i%RESOLUTION)+(i/RESOLUTION)-8*vbopoints[i][1]<smoothe(d,4)*RESOLUTION){
	if((i/RESOLUTION)%25 == 0){
	  colors[i][1]=1.5-vbopoints[i][1];
	  colors[i][0]=1;
	  colors[i][3]=1.5;
	  if(points[i]<0){
	    vbopoints[i][0]=10*(i%RESOLUTION)/(float)RESOLUTION-5.0;
	    vbopoints[i][1]=0.001;
	    vbopoints[i][2]=10*i/(float)(RESOLUTION*RESOLUTION)-5.0;
	  }
	  if((i%RESOLUTION)%25 < 2 || (i%RESOLUTION)%25==24){
	    colors[i][1]=0;
	    colors[i][3]=1.5;
	    colors[i][2]=0;
	  }
	}
      }
    }
    SDL_mutexV(lock);
    SDL_Delay(1);
  }
  d=0;
  printf("Screen on\nBlinking area\n");
  for(int j=0; j<6;j++){
    SDL_mutexP(lock);
    for(int i=0; i<RESOLUTION*RESOLUTION; i++){
      if(i%RESOLUTION>lx && i%RESOLUTION<rx && i/RESOLUTION<tz && i/RESOLUTION>bz) {
	  colors[i][0]=j%2;
	  colors[i][1]=0.7*(j%2);
	  colors[i][2]=0.7*(j%2);
      }
    }
    SDL_mutexV(lock);
    SDL_Delay(100);
  }
  printf("Restoring colors\n");
  for(int i=0; i<RESOLUTION*RESOLUTION; i++){
    if(i%RESOLUTION>lx && i%RESOLUTION<rx && i/RESOLUTION<tz && i/RESOLUTION>bz) {
      colors[i][1] = 1.0-vbopoints[i][1]/2;
      colors[i][2] = 0.76-0.2*vbopoints[i][1];
      colors[i][0] = 0.6;
      if(vbopoints[i][1]>0.8){
	colors[i][0]=2*vbopoints[i][1]-1;
	colors[i][1]=2*vbopoints[i][1]-1;
	colors[i][2]=2*vbopoints[i][1]-1;
      }
      else if(vbopoints[i][1]<0.02){
	colors[i][2]=0.5;
	colors[i][1]=0.8;
	colors[i][0]=0.7;
      }
    }
  }    
  d=1;
  printf("Zooming into area 1\n");
  while(d>=0){
    d -= 0.01;
    float s = pow(sin(3.141592/2.0*(1-d)),2);
    cx = s*10*zcx;
    cy = s*points[RESOLUTION*250+350]+0.2;
    cz = s*10*zcz;
    czoom = 2.0+(1-s)*4.0;
    SDL_mutexP(lock);
    for(int i=0; i<RESOLUTION*RESOLUTION; i++){
      float x=i%RESOLUTION;
      float y=i/RESOLUTION;
      if(i%RESOLUTION>lx && i%RESOLUTION<rx && i/RESOLUTION<tz && i/RESOLUTION>bz && points[i] > 0) {
	colors[i][3]+=0.3;
	vbopoints[i][1]+=0.003;
      }
      if(x==lx && y>=bz && y<=tz) colors[i][3]+=0.5;
      if(x==rx && y>=bz && y<=tz) colors[i][3]+=0.5;
      if(y==tz && x>=lx && x<=rx) colors[i][3]+=0.5;
      if(y==bz && x>=lx && x<=rx) colors[i][3]+=0.5;
    }
    SDL_mutexV(lock);
    SDL_Delay(10);
  }
  d=0;
  SDL_Delay(500);
  printf("Applying color filter 1\n");
  while(d<=1){
    d+=0.01;
    float s = pow(sin(3.141592/2.0*d),2);
    SDL_mutexP(lock);
    for(int i=0; i<RESOLUTION*RESOLUTION; i++){
      if(i%RESOLUTION>lx && i%RESOLUTION<rx && i/RESOLUTION<tz && i/RESOLUTION>bz) {
	  if(i/RESOLUTION<floor(bz+s*(tz-bz))){
	    colors[i][0]=1-s;
	    colors[i][1]=s;
	    if(points[i]<0.02) colors[i][0]=(i/RESOLUTION+1)%2/3.0;
	  }
      }
    }
    SDL_mutexV(lock);
    SDL_Delay(1);
  }
  d=0;
  printf("Applying color filter 2\n");
  while(d<1.0){
    d+=0.01;
    float s = pow(sin(3.141592/2.0*d),2);
    SDL_mutexP(lock);
    for(int i=0; i<RESOLUTION*RESOLUTION; i++){
      if(i%RESOLUTION>lx && i%RESOLUTION<rx && i/RESOLUTION<tz && i/RESOLUTION>bz) {
	  if(i%RESOLUTION<floor(lx+s*(rx-lx))){
	    colors[i][2]=points[i]*5;
	    colors[i][1]=points[i]*10;
	    if(points[i]<0.02) colors[i][1]=(i/RESOLUTION)%2/3.0;
	  }
      }
    }
    SDL_mutexV(lock);
    SDL_Delay(1);
  }
  d=0;
  printf("Applying smaller gridlies\n");
  while(d<=1){
    d+=0.02;
    float s = pow(sin(3.141592/2.0*d),2);
    SDL_mutexP(lock);
    for(int i=0; i<RESOLUTION*RESOLUTION; i++){
      if(i%RESOLUTION>lx && i%RESOLUTION<rx && i/RESOLUTION<tz && i/RESOLUTION>bz) {
	  if(i/RESOLUTION<floor(bz+s*(tz-bz))){
	    if((i/RESOLUTION)%10==0){
	      colors[i][0] = 1;
	      colors[i][2] = 0.2;
	    }
	  }
      }
    }
    SDL_mutexV(lock);
    SDL_Delay(1);
  }
  d=0;
  printf("Applying more smaller gridlies\n");
  while(d<=1){
    d+=0.02;
    float s = pow(sin(3.141592/2.0*d),2);
    SDL_mutexP(lock);
    for(int i=0; i<RESOLUTION*RESOLUTION; i++){
      if(i%RESOLUTION>lx && i%RESOLUTION<rx && i/RESOLUTION<tz && i/RESOLUTION>bz) {
	  if(i%RESOLUTION<floor(lx+s*(rx-lx))){
	    if((i%RESOLUTION)%10==0){
	      colors[i][0] = 1;
	      colors[i][2] = 0.2;
	      if((i/RESOLUTION)%10==0){
		colors[i][1] = 0;
	      }
	    }
	  }
      }
    }
    SDL_mutexV(lock);
    SDL_Delay(1);
  }
  d=0;
  printf("Highlighting area 2\n");
  for(int j=0; j<6;j++){
    SDL_mutexP(lock);
    for(int i=0; i<RESOLUTION*RESOLUTION; i++){
      if(i%RESOLUTION>lx2 && i%RESOLUTION<rx2 && i/RESOLUTION<tz2 && i/RESOLUTION>bz2) {
	  colors[i][0]=j%2;
	  colors[i][1]=0.7*(j%2);
	  colors[i][2]=0.7*(j%2);
      }
    }
    SDL_mutexV(lock);
    SDL_Delay(100);
  }
  printf("Restoring color\n");
  for(int i=0; i<RESOLUTION*RESOLUTION; i++){
    if(i%RESOLUTION>lx2 && i%RESOLUTION<rx2 && i/RESOLUTION<tz2 && i/RESOLUTION>bz2) {
	colors[i][0] = 0;
	colors[i][1] = 10*points[i];
	colors[i][2] = 5*points[i];
    }
  }
  d=1;
  printf("Zooming into area 2\n");
  while(d>=0){
    d -= 0.01;
    float s = pow(sin(3.141592/2.0*(1-d)),2);
    cx = (1-s)*10*zcx+s*10*zcx2;
    cy+= 0.003;
    ey = 1+s;
    cz = (1-s)*10*zcz+s*10*zcz2;
    czoom = 0.5+(1-s)*1.5;
    SDL_mutexP(lock);
    for(int i=0; i<RESOLUTION*RESOLUTION; i++){
      float x=i%RESOLUTION;
      float y=i/RESOLUTION;
      if(i%RESOLUTION>lx2 && i%RESOLUTION<rx2 && i/RESOLUTION<tz2 && i/RESOLUTION>bz2) {
	vbopoints[i][1]+=0.003;
      }
      else{
	colors[i][3]-=0.01;
      }
      if(x==lx2 && y>=bz2 && y<=tz2) colors[i][3]+=0.5;
      if(x==rx2 && y>=bz2 && y<=tz2) colors[i][3]+=0.5;
      if(y==tz2 && x>=lx2 && x<=rx2) colors[i][3]+=0.5;
      if(y==bz2 && x>=lx2 && x<=rx2) colors[i][3]+=0.5;
    }
    SDL_mutexV(lock);
    SDL_Delay(10);
  }
  return 0;
}

void OnEvent(SDL_Event* Event){
  switch(Event->type){
  case SDL_QUIT: running = false;
  case SDL_KEYDOWN:
    char k = SDL_GetKeyName(Event->key.keysym.sym)[0];
    if (k == 'q') running = false;
    if (k == 'i') pointsize += 0.25;
    if (k == 'k') pointsize -= 0.1;
    if (k == 'w') cz += 0.1;
    if (k == 's') cz -= 0.1;
    if (k == 'a') cx += 0.1;
    if (k == 'd') cx -= 0.1;
    if (k == 'e') cy -= 0.1;
    if (k == 'c') cy += 0.1;
    if (k == 'r') ey -= 0.1;
    if (k == 'v') ey += 0.1;
    if (k == 'z') czoom -= 0.1;
    if (k == 'x') czoom += 0.1;
    printf("CX: %f | CY: %f | CZ: %f\n",cx,cy,cz);
  }
}

int main(){
  SDL_Surface* Surf_Display = NULL;
  srand(122330);
  genterrain(points);
  double total = 0;
  double min = 10000000;
  double max =-10000000;
  printf("Preparing for normalization\n");
  for(int i=0; i<RESOLUTION*RESOLUTION; i++){
    double j=points[i];
    total+=j;
    if(j>max) max=j;
    if(j<min) min=j;
  }
  double mean = total/(RESOLUTION*RESOLUTION);
  printf("Total value: %f\n",total);
  printf("Mean value:  %f\n",mean);
  printf("Min value:   %f\n",min);
  printf("Max value:   %f\n",max);
  ey=1;
  ed=1;
  cy=0;
  cx=0;
  cz=0;
  czoom=6;
  starttime=clock();
  lock = SDL_CreateMutex();
  SDL_Thread *scriptthread=SDL_CreateThread(script,lock);
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) return false;
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,        8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,      8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,       8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,      8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,      16);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,     32);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,  8);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    8);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    8);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  4);
  if((Surf_Display = SDL_SetVideoMode(XRES, YRES, 8, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL)) == NULL) return false;
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(4, GL_FLOAT, 0, &colors);
  glVertexPointer(3, GL_FLOAT, 0, &vbopoints);
  glClearColor(0, 0, 0, 0);
  glClearDepth(1.0f);
  glViewport(0, 0, XRES, YRES);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, XRES / YRES, 0.001f, 500.0f);
  glMatrixMode(GL_MODELVIEW);
  //glEnable(GL_MULTISAMPLE);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_BLEND);
  SDL_Event Event;
  while(running) {
    while(SDL_PollEvent(&Event)) {
      OnEvent(&Event);
    }
  }
  SDL_Quit();
}
