#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <stdlib.h>
#include "gensphere.h"
#include "xorshf.h"

#define XRES 1000.0
#define YRES 800.0
#define BOTTOMBAR 50.0

class App{
public:
  arandom* random;
  App(){
    running=true;
    iterate = false;
    Surf_Display = NULL;
    random = new arandom(1234);
  }
  int OnExecute(){
    if(OnInit() == false) return -1;
    SDL_Event Event;
    while(running) {
      while(SDL_PollEvent(&Event)) {
	OnEvent(&Event);
      }
      OnLoop();
      OnRender();
    }
    OnCleanup();
    return 0;
  }
  bool OnInit(){
    srand(1234);
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
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  1);
    if((Surf_Display = SDL_SetVideoMode(XRES, YRES, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL)) == NULL) return false;
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    float ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    float diffuseLight[] = { 0.5f, 0.5f, 0.5, 1.0f };
    float specularLight[] = { 0.8f, 0.8f, 0.5f, 1.0f };
    float position[] = {0.0f, 0.0f, 5.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glShadeModel(GL_SMOOTH);
    glClearColor(0, 0, 0, 0);
    glClearDepth(1.0f);
    glViewport(0, BOTTOMBAR, XRES, YRES-BOTTOMBAR);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, XRES / YRES, 1.0f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
    std::vector<std::vector<Triangle> > alltriangles;
    makesphere(&alltriangles, 5);
    triangles=alltriangles.back();
    return true;
  }
  void OnEvent(SDL_Event* Event){
    switch(Event->type){
    case SDL_QUIT: running = false;
    case SDL_KEYUP:
      char k = SDL_GetKeyName(Event->key.keysym.sym)[0];
      if (k == 'q') running = false;
      if (k == 'i') iterate = true;
    } 
  }
  void OnLoop(){}
  void OnRender(){
    static float angle = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f,0.0f,-3.0f);
    glRotatef(angle,0.0f,1.0f,0.0f);
    float mcolor[] = { 0.5, 0.5, 0.5, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mcolor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mcolor);
    glBegin(GL_TRIANGLES);
    if(iterate){
      float a = 2.0*((float)rand()/(float)RAND_MAX)-1.0;
      float b = 2.0*((float)rand()/(float)RAND_MAX)-1.0;
      std::cout << "A: " << a << '\n';
      std::cout << "B: " << b << '\n';
      Point3 n(sin(a)*cos(b), sin(a)*sin(b), cos(a));
      for(int t=0; t<triangles.size(); t++){
	if(n.x*triangles[t].a->x+n.y*triangles[t].a->y+n.z*triangles[t].a->z > 0){
	  triangles[t].a->x*=1.001;
	  triangles[t].a->y*=1.001;
	  triangles[t].a->z*=1.001;
	}else{
	  triangles[t].a->x*=0.999;
	  triangles[t].a->y*=0.999;
	  triangles[t].a->z*=0.999;
	}
	if(n.x*triangles[t].b->x+n.y*triangles[t].b->y+n.z*triangles[t].b->z > 0){
	  triangles[t].b->x*=1.001;
	  triangles[t].b->y*=1.001;
	  triangles[t].b->z*=1.001;
	}else{
	  triangles[t].b->x*=0.999;
	  triangles[t].b->y*=0.999;
	  triangles[t].b->z*=0.999;
	}
	if(n.x*triangles[t].c->x+n.y*triangles[t].c->y+n.z*triangles[t].c->z > 0){
	  triangles[t].c->x*=1.001;
	  triangles[t].c->y*=1.001;
	  triangles[t].c->z*=1.001;
	}else{
	  triangles[t].c->x*=0.999;
	  triangles[t].c->y*=0.999;
	  triangles[t].c->z*=0.999;
	}
	iterate = false;
      }
    }
    for(int t=0; t<triangles.size(); t++){
      glNormal3f(triangles[t].a->x,triangles[t].a->y,triangles[t].a->z);
      glVertex3f(triangles[t].b->x,triangles[t].b->y,triangles[t].b->z);
      glNormal3f(triangles[t].c->x,triangles[t].c->y,triangles[t].c->z);
      glVertex3f(triangles[t].a->x,triangles[t].a->y,triangles[t].a->z);
      glNormal3f(triangles[t].b->x,triangles[t].b->y,triangles[t].b->z);
      glVertex3f(triangles[t].c->x,triangles[t].c->y,triangles[t].c->z);
    }
    glEnd();
    angle+=0.2f;
    SDL_GL_SwapBuffers();
  }
  void OnCleanup(){
    SDL_Quit();
  }
private:
  bool running;
  bool iterate;
  SDL_Surface* Surf_Display;
  std::vector<Triangle> triangles;
};

int main(){ 
  App theApp;
  return theApp.OnExecute();
}
