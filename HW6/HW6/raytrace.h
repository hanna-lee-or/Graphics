/******************************************************************/
/*         Raytracer declarations, Final Project               */
/******************************************************************/

/* constants */
#define TRUE 1
#define FALSE 0

#define M_PI 3.1415926535897932384626433832795029

#include <GL/glut.h>
#include "ReadBMP.h"

/* data structures */

typedef struct point {
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat w;
} point;

/* a vector is just a point */
typedef point vector;

/* a line segment */
typedef struct _segment {
  point* start;
  point* end;
  struct _segment* next;
} segment;

/* a ray is just a segment with an endpoint at infinity */
typedef segment ray;

typedef struct color {
  GLfloat r;
  GLfloat g;
  GLfloat b; 
  /* these should be between 0 and 1 */
} color;

typedef struct material {
  /* color */
  color c;
  /* ambient reflectivity */
  GLfloat amb;
  GLfloat ref;
  GLfloat trans;
  GLfloat dif;
  GLfloat spec;
} material;

typedef struct sphere {
  point* c;  /* center */
  GLfloat r;  /* radius */
  material* m;
} sphere;

typedef struct plane {
	point* c;  /* center */
	vector normal;
	material* m;
} plane;

typedef struct Light {
	GLfloat intensity;
	color color;
	GLfloat amb;
	point pos_light;
	GLboolean visable;
}Light;

/* functions in tracer.c */
void trace (int*, ray*, point*, vector*, material**);
void Vnormalize(vector*);
GLfloat calculCross(vector *, vector *);

/* functions in shader.c */
void shade(int, GLboolean, ray*, point*, vector*, material*, color*);
void setValue(color*);
/* r, g, b, amb, ref, trans, dif, spec*/
material* makeMaterial(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);

/* global variables */
extern int width;
extern int height;
extern Light pointLight;
extern color** colorMap1;
extern color** colorMap2;
extern int map1_width;
extern int map1_height;
extern int map2_width;
extern int map2_height;

/* the scene: so far, two sphere */
sphere* s1;
sphere* s2;

plane* pl1;
plane* pl2;

