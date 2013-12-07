#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>

class Point3{
public:
  float x;
  float y;
  float z;
  Point3(float _x, float _y, float _z):x(_x),y(_y),z(_z){}
  Point3(Point3 a, Point3 b){
    x=a.x+b.x; y=a.y+b.y; z=a.z+b.z;
    float nf = 1/sqrt(x*x+y*y+z*z);
    x*=nf; y*=nf; z*=nf;
  }
};

class Triangle{
public:
  Point3* a;
  Point3* b;
  Point3* c;
  Triangle(Point3* _a, Point3* _b, Point3* _c){
    a=_a;
    b=_b;
    c=_c;
  }
};

const &std::vector<Triangle>(int iterations){
  float a = sqrt((5.0 + sqrt(5.0))/10.0);
  float b = sqrt(2.0/(5.0 + sqrt(5.0)));
  float c = 0.4*(5-sqrt(5.0))+0.1;
  std::cout << "Generating icosahedron\n";
  std::vector<<Point3>> pc;
  pc.push_back(new Point3(0, a, -b));
  pc.push_back(new Point3(-a, b, 0));
  pc.push_back(new Point3(a, b, 0));
  pc.push_back(new Point3(0, a, b));
  pc.push_back(new Point3(-b, 0, a));
  pc.push_back(new Point3(0, -a, b));
  pc.push_back(new Point3(b, 0, a));
  pc.push_back(new Point3(b, 0, -a));
  pc.push_back(new Point3(0, -a, -b));
  pc.push_back(new Point3(-b, 0, -a));
  pc.push_back(new Point3(-a, -b, 0));
  pc.push_back(new Point3(a, -b, 0));
  std::cout << "Generated icosahedron\nAssembling triangles\n";
  std::vector<std::vector<Triangle> > triangles;
  triangles.push_back(std::vector<Triangle>());
  for(unsigned int ii=0;ii<pc.size()-2;ii++){
    Point3* i=pc[ii];
    for(unsigned int ij=ii+1;ij<pc.size()-1;ij++){
      Point3* j=pc[ij];
      float ijdx = i->x-j->x;
      float ijdy = i->y-j->y;
      float ijdz = i->z-j->z;
      if((ijdx*ijdx+ijdy*ijdy+ijdz*ijdz) <= c){
	for(unsigned int ik=ij+1;ik<pc.size();ik++){
	  Point3* k=pc[ik];
	  float jkdx = j->x-k->x;
	  float jkdy = j->y-k->y;
	  float jkdz = j->z-k->z;
	  float kidx = k->x-i->x;
	  float kidy = k->y-i->y;
	  float kidz = k->z-i->z;
	  if((jkdx*jkdx+jkdy*jkdy+jkdz*jkdz) <= c && (kidx*kidx+kidy*kidy+kidz*kidz) <= c) triangles[0].push_back(Triangle(i,j,k));
	}
      }
    }
  }
  std::cout << "Assembled triangles\nBeginning subdivision\n";
  for(int iteration=0;iteration<iterations;iteration++){
    triangles.push_back(std::vector<Triangle>());
    std::cout << "Iteration " << iteration+1 << '\n';
    for(unsigned int t=0;t<triangles[iteration].size();t++){
      Point3* i = triangles[iteration][t].a;
      Point3* j = triangles[iteration][t].b;
      Point3* k = triangles[iteration][t].c;
      Point3* l = new Point3(*i,*j);
      Point3* m = new Point3(*j,*k);
      Point3* n = new Point3(*k,*i);
      pc.push_back(l); pc.push_back(m); pc.push_back(n);
      triangles.back().push_back(Triangle(l, i, n));
      triangles.back().push_back(Triangle(l, j, m));
      triangles.back().push_back(Triangle(n, k, m));
      triangles.back().push_back(Triangle(l, m, n));
    } 
  }
  return triangles.back()
  std::cout << "Completed subdivision\nReturning triangles\n";
}
