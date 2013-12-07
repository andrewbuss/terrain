#include "zmath.h"
#include "specsynth.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_opengl.h>
#include <stdio.h>

#define RES 65
void gluPerspective(float FOV, float ASPECT, float NEARPLANE, float FARPLANE){
  double left, right;
  double bottom, top;
  top = tan (FOV*3.14159/360.0)*NEARPLANE;
  bottom = -top;
  left = ASPECT*bottom;
  right = ASPECT*top;
  glFrustum (left, right, bottom, top, NEARPLANE, FARPLANE);
}

int main(){
  float* terrain = (float*) malloc(RES*RES*sizeof(float));
  genterrain(terrain, RES, 40000, 2.3, true);
  al_init();
  al_install_keyboard();
  
  ALLEGRO_DISPLAY *display;
  al_set_new_display_flags(ALLEGRO_OPENGL);
  display = al_create_display(1280, 960);
  
  ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
  al_register_event_source(event_queue, al_get_display_event_source(display));
  al_register_event_source(event_queue, al_get_keyboard_event_source());
  bool quit = false;
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  float* verts = (float*) malloc(RES*RES*4*4*3);
  float* colors = (float*) malloc(RES*RES*4*4*3);
  float* vp = verts;
  float* cp = colors;
  for(int x = 0; x < RES-1; x++){
    for(int z = 0; z < RES-1; z++){
      *(vp++) = (x-RES/2.0+0.5)*1024.0/RES;
      *(vp++) = terrain[x*RES+z];
      *(vp++) = (z-RES/2.0+0.5)*1024.0/RES;

      *(cp++) = 0.5*terrain[x*RES+z];
      *(cp++) = 1-terrain[x*RES+z];
      *(cp++) = 0.5*terrain[x*RES+z];

      *(vp++) = (x-RES/2.0+1.5)*1024.0/RES;
      *(vp++) = terrain[(x+1)*RES+z];
      *(vp++) = (z-RES/2.0+0.5)*1024.0/RES;

      *(cp++) = 0.5*terrain[(x+1)*RES+z];
      *(cp++) = 1-terrain[(x+1)*RES+z];
      *(cp++) = 0.5*terrain[(x+1)*RES+z];

      *(vp++) = (x-RES/2.0+1.5)*1024.0/RES;
      *(vp++) = terrain[(x+1)*RES+z+1];
      *(vp++) = (z-RES/2.0+1.5)*1024.0/RES;

      *(cp++) = 0.5*terrain[(x+1)*RES+z+1];
      *(cp++) = 1-terrain[(x+1)*RES+z+1];
      *(cp++) = 0.5*terrain[(x+1)*RES+z+1];

      *(vp++) = (x-RES/2.0+0.5)*1024.0/RES;
      *(vp++) = terrain[x*RES+z+1];
      *(vp++) = (z-RES/2.0+1.5)*1024.0/RES;

      *(cp++) = 0.5*terrain[x*RES+z+1];
      *(cp++) = 1-terrain[x*RES+z+1];
      *(cp++) = 0.5*terrain[x*RES+z+1];
    }
  }

  glVertexPointer(3, GL_FLOAT, 0, verts);
  glColorPointer(3, GL_FLOAT, 0, colors);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  /*glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);*/
  while(!quit){
    double ms = al_current_time();
    ALLEGRO_EVENT event;
    if (al_get_next_event(event_queue, &event)){
      if (ALLEGRO_EVENT_KEY_DOWN == event.type && ALLEGRO_KEY_Q == event.keyboard.keycode ||  ALLEGRO_EVENT_DISPLAY_CLOSE == event.type){
	break;
      }
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(45, 640.0/480.0,.1,1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(30, 1, 0, 0);
    glTranslatef(0, -100, -200);
    glScalef(1,20,1);
    
    glRotatef(ms*12, 0, 1, 0);

    glPolygonMode(GL_FRONT, GL_LINES);
    glDrawArrays(GL_QUADS, 0, RES*RES*4);
    al_flip_display();
  }
  return 0;
}
