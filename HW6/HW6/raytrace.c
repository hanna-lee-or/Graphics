/******************************************************************/
 /*          Main raytracer file, Final Project            */
/*           By:  1615057 이한나                             */
/******************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include "lowlevel.h"
#include "raytrace.h"

/* local functions */
void initScene(void);
void initCamera (int, int);
void display(void);
void init(int, int);
void traceRay(ray*,color*);
void drawScene(void);
void firstHit(ray*,point*,vector*,material**);

/* local data */
char *file_name = "background.bmp";
int k = 0;		//printf 테스트용

/* parameters defining the camera */
point* viewpoint;
GLfloat pnear;  /* distance from viewpoint to image plane */
GLfloat fovx;  /* x-angle of view frustum */
int width;     /* width of window in pixels */
int height;    /* height of window in pixels */

Light pointLight = { 0.5, {1, 1, 1}, 1, {4, -4, 0}, TRUE };

/* some geometry functions */

point* makePoint(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
  point* p;
  /* allocate memory */
  p = (point*) malloc(sizeof(point));
  /* put stuff in it */
  p->x = x; p->y = y; p->z = z; p->w = w;
  return (p);
}

sphere* makeSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r) {
  sphere* s;
  /* allocate memory */
  s = (sphere*) malloc(sizeof(sphere));

  /* put stuff in it */
  s->c = makePoint(x,y,z,1.0);   /* center */
  s->r = r;   /* radius */
  s->m = NULL;   /* material */
  return(s);
}

plane* makePlane(GLfloat x, GLfloat y, GLfloat z, vector* n) {
	plane* pl;
	/* allocate memory */
	pl = (plane*)malloc(sizeof(plane));

	/* put stuff in it */
	pl->c = makePoint(x, y, z, 1.0);   /* center */
	pl->normal.x = n->x;
	pl->normal.y = n->y;
	pl->normal.z = n->z;
	pl->m = NULL;   /* material */
	return(pl);
}

/* returns the color seen by ray r in parameter c */
void rayColor(ray* r, color* c) {
  point p1, p2;  /* first intersection point */
  vector n1, n2;
  material* m1;
  material* m2;
  color reflect_c = { 0, 0, 0 };
  int flag = 0;
  GLboolean ismap = FALSE;

  p1.w = 0.0;  /* inialize to "no intersection" */
  trace(&flag, r, &p1, &n1, &m1);		// r은 ray, p는 ray가 구에 닿은 점, n은 p에서의 normal vector, m은 구의 재질

  // ray가 물체에 닿았을 시
  if (p1.w != 0.0) {
	  // c는 색상값 받아올 변수.
	  if (flag == 4)
		  ismap = TRUE;
	  else
		  ismap = FALSE;
	  shade(ismap, TRUE, r, &p1, &n1, m1, c);  /* do the lighting calculations */
	  // 그림자 효과
	  if (p1.w == -1) {
		  c->r *= 0.3;
		  c->g *= 0.3;
		  c->b *= 0.3;
	  }
  }
  // ray가 구에 닿지 않았을 시
  else {             /* nothing was hit */
    c->r = 0.0;
    c->g = 0.0;
    c->b = 0.0;
  }

  // 반사 효과 있을 시, 한번 더 trace
  if (m1->ref > 0 && p1.w != 0) {
	  p2.w = 0.0;
	  trace(&flag, r->next, &p2, &n2, &m2);

	  // 반사 ray가 물체에 닿았을 시
	  if (p2.w != 0.0) {
		  if (flag == 4)
			  ismap = TRUE;
		  else
			  ismap = FALSE;
		  shade(ismap, FALSE, r->next, &p2, &n2, m2, &reflect_c);
		  // 그림자 효과
		  if (p2.w == -1) {
			  reflect_c.r *= 0.3;
			  reflect_c.g *= 0.3;
			  reflect_c.b *= 0.3;
		  }
	  }
	  c->r += reflect_c.r * m1->ref;
	  c->g += reflect_c.g* m1->ref;
	  c->b += reflect_c.b* m1->ref;
	  setValue(c);
  }
}

/* vector from point p to point q is returned in v */
void calculateDirection(point* p, point* q, point* v) {
  float length;

  v->x = q->x - p->x;
  v->y = q->y - p->y;
  v->z = q->z - p->z;
  /* a direction is a point at infinity */
  v->w = 0.0;
}

// 체크하고 싶은 부분 키보드 조작 이용
void KeyboardFunc(unsigned char Key, int x, int y)
{
	float gap = 1;
	
	if (Key == 27)
		exit(0);         // ESC
	
	if (Key == 'r') {
		pointLight.visable = !pointLight.visable;
		pointLight.pos_light.x = 4;
		pointLight.pos_light.y = -4;
		pointLight.pos_light.z = 0;
	}
	if (Key == 'a') {
		pointLight.pos_light.x -= gap;
	}
	if (Key == 'd') {
		pointLight.pos_light.x += gap;
	}
	if (Key == 'w') {
		pointLight.pos_light.y += gap/2;
	}
	if (Key == 's') {
		pointLight.pos_light.y -= gap/2;
	}
	if (Key == 'q') {
		pointLight.pos_light.z -= gap;
	}
	if (Key == 'e') {
		pointLight.pos_light.z += gap;
	}
	if (Key == 't') {
		printf("조명 좌표 : %f %f %f", pointLight.pos_light.x, pointLight.pos_light.y, pointLight.pos_light.z);
	}
	display();

}

/* The main program */

/* Just sets up window and display callback */
int main (int argc, char** argv) {
	int win;

  glutInit(&argc,argv);
  glutInitWindowSize(500,350);
  glutInitWindowPosition(400,200);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  win = glutCreateWindow("raytrace_1615057");
  glutSetWindow(win);
  init(500,350);
  glutDisplayFunc(display);
  glutKeyboardFunc(KeyboardFunc);
  glutMainLoop();
}

void init(int w, int h) {

  /* OpenGL setup */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  glClearColor(0.0, 0.0, 0.0, 1.0);  

  /* low-level graphics setup */
  initCanvas(w,h);

  /* raytracer setup */
  initCamera(w,h);
  initScene();

  /* read .bmp file for mapping*/
  ReadBMPFile(file_name);

}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  drawScene();  /* draws the picture in the canvas */
  flushCanvas();  /* draw the canvas to the OpenGL window */
  glFlush();
}

/* Camera placement. 
   NOTE: The placement of the image rectangle is hard-coded 
   to be parallel to the x-y plane, in the plane 
   z=(viewpoint.z = - pnear) (that is, looking down the negative
   z axis, just like OpenGL), with the viewpoint centered with 
   respect to x,y. */
void initCamera (int w, int h) {
  viewpoint = makePoint(0.0,0.0,0.0,1.0);
  pnear = -1.0;
  fovx = M_PI/6;
  /* window dimensions */
  width = w;  
  height = h;
}

// 씬에 물건 만들기
void initScene () {
	vector n1 = { 0, -0.5, 0 }, n2 = {0, 0, 0.2};
	s1 = makeSphere(0.0, 0.0, -2.5, 0.2);
	s2 = makeSphere(-0.12, -0.12, -2.0, 0.05);
	pl1 = makePlane(0, 0.3, 0, &n1);
	pl2 = makePlane(0, 0, -5, &n2);
	s1->m = makeMaterial(0.8, 0.3, 0.3, 0.5, 0.5, 0, 1, 0.5);
	s2->m = makeMaterial(0.7, 0.7, 0.3, 0.5, 0, 0, 1, 0.5);
	pl1->m = makeMaterial(0.4, 0.8, 0.4, 0.5, 0, 0, 1, 0.5);
	pl2->m = makeMaterial(0.4, 0.4, 0.8, 0.5, 0, 0, 1, 0.5);
}

void drawScene () {
  int i,j;
  GLfloat imageWidth;

  /* declare data structures on stack to avoid malloc & free */
  point worldPix;  /* current pixel in world coordinates */
  point direction; 
  ray r;
  color c;

  /* z and w are constant for all pixels */
  worldPix.w = 1.0;
  worldPix.z = pnear;

  r.start = &worldPix;
  r.end = &direction;

  imageWidth = 2*pnear*tan(fovx/2);

  /* trace a ray for every pixel */
  for (i=0; i<width; i++) {
    for (j=0; j<height; j++) {

      /* find position of pixel in world coordinates */
      /* y position = (pixel height/middle) scaled to world coords */ 
      worldPix.y = (j-(height/2))*imageWidth/width;
      /* x position = (pixel width/middle) scaled to world coords */ 
      worldPix.x = (i-(width/2))*imageWidth/width;

      /* find direction */
      /* note: direction vector is NOT NORMALIZED */
      calculateDirection(viewpoint,&worldPix,&direction);

      /* Find color for pixel */
	  r.next = NULL;
      rayColor(&r,&c);
      /* write the pixel! */
      drawPixel(i, j, c.r, c.g, c.b);
    }
  }
}
