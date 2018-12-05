/******************************************************************/
/*         Raytracer declarations, Final Project               */
/******************************************************************/


/* constants */
#define TRUE 1
#define FALSE 0

#define M_PI 3.1415926535897932384626433832795029

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
} material;

typedef struct sphere {
  point* c;  /* center */
  GLfloat r;  /* radius */
  material* m;
} sphere;

typedef struct Light {
	color c;
	GLfloat intensity;
	GLfloat amb;
	point pos_light;
	GLboolean visable;
}Light;

/* functions in tracer.c */
void trace (ray*, point*, vector*, material**);
void Vnormalize(vector*);

/* functions in shader.c */
material* makeMaterial(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void shade(ray*, point*,vector*,material*, color*);

/* global variables */
extern int width;
extern int height;

/* the scene: so far, two sphere */
sphere* s1;
sphere* s2;

