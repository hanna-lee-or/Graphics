/******************************************************************/
/*         Functions to trace rays                                */
/******************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <GL/glut.h>
#include "raytrace.h"

extern Light L;

/* point on ray r parameterized by t is returned in p */
void findPointOnRay(ray* r, double t, point* p) {
  p->x = r->start->x + t * r->end->x;
  p->y = r->start->y + t * r->end->y;
  p->z = r->start->z + t * r->end->z;
  p->w = 1.0;
}

/* raySphereIntersect */
/* returns TRUE if ray r hits sphere s, with parameter value in t */
int raySphereIntersect(ray* r,sphere* s,double* t) {
  point p;   /* start of transformed ray */
  double a,b,c;  /* coefficients of quadratic equation */
  double D;    /* discriminant */
  point* v;
  
  /* transform ray so that sphere center is at origin */
  /* don't use matrix, just translate! */
  p.x = r->start->x - s->c->x;
  p.y = r->start->y - s->c->y;
  p.z = r->start->z - s->c->z;
  v = r->end; /* point to direction vector */

  a = v->x * v->x  +  v->y * v->y  +  v->z * v->z;
  b = 2*( v->x * p.x  +  v->y * p.y  +  v->z * p.z);
  c = p.x * p.x + p.y * p.y + p.z * p.z - s->r * s->r;

  D = b * b - 4 * a * c;
  
  if (D < 0) {  /* no intersection */
    return (FALSE);
  }
  else {
    D = sqrt(D);
    /* First check the root with the lower value of t: */
    /* this one, since D is positive */
    *t = (-b - D) / (2*a);
    /* ignore roots which are less than zero (behind viewpoint) */
    if (*t < 0) {
      *t = (-b + D) / (2*a);
    }
    if (*t < 0) { return(FALSE); }
    else return(TRUE);
  }
}

/* normal vector of s at p is returned in n */
/* note: dividing by radius normalizes */
void findSphereNormal(sphere* s, point* p, vector* n) {
  n->x = (p->x - s->c->x) / s->r;  
  n->y = (p->y - s->c->y) / s->r;
  n->z = (p->z - s->c->z) / s->r;
  n->w = 0.0;
}

void Vnormalize(vector *V)
{
	double d1 = sqrt(V->x * V->x + V->y * V->y + V->z * V->z);
	double d;
	if (d1 == 0)
	{
		d = 0;
	}
	else
	{
		d = (double)1.0 / d1;
	}

	V->x *= d;
	V->y *= d;
	V->z *= d;
}

GLfloat calculCross(vector *v1, vector *v2) {
	return (v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z);
}

void findReflectionRay(vector v, vector* n, vector* R) {

	GLfloat nv = calculCross(&v, n);

	R->x = v.x - 2 * nv * n->x;
	R->y = v.y - 2 * nv * n->y;
	R->z = v.x - 2 * nv * n->z;

}

vector transRtoV(ray* r) {

	vector v;

	v.x = r->end->x - r->start->x;
	v.y = r->end->y - r->start->y;
	v.z = r->end->z - r->start->z;

	return v;

}

int j = 0;
/* trace */
/* If something is hit, returns the finite intersection point p, 
   the normal vector n to the surface at that point, and the surface
   material m. If no hit, returns an infinite point (p->w = 0.0) */
void trace (ray* r, point* p, vector* n, material* *m) {
  double t1 = 0, t2 = 0;     /* parameter value at first hit */
  int hit1 = FALSE, hit2 = FALSE;
  vector v = transRtoV(r);
  vector R;
  ray temp;
  point pp;
  
  hit1 = raySphereIntersect(r, s1,&t1);		// ȭ�鿡 �ִ� �� s1 �� ray r�� ��Ҵ��� üũ
  hit2 = raySphereIntersect(r, s2, &t2);		// ȭ�鿡 �ִ� �� s2 �� ray r�� ��Ҵ��� üũ

  // �� �� ������� �� ����� ������ ���� �ɷ� ó��
  if (hit1 && hit2) {
	  if (t1 < t2)
		  hit2 = FALSE;
	  else
		  hit1 = FALSE;
  }

  if (hit1) {
    *m = s1->m;
    findPointOnRay(r, t1, p);		// ������� ���� �� p ã��
    findSphereNormal(s1, p, n);		// �� �������� normal vector n ã��
	findReflectionRay(v, n, &R);
	temp.start = p;
	pp.x = L.pos_light.x - p->x;
	pp.y = L.pos_light.y - p->y;
	pp.z = L.pos_light.z - p->z;
	temp.end = &pp;
	r->next = &temp;
	if (raySphereIntersect(r->next, s2, &t1)) {
		p->w = -1.0;
	}
  }
  else if (hit2) {
	  *m = s2->m;
	  findPointOnRay(r, t2, p);
	  findSphereNormal(s2, p, n);
	  findReflectionRay(v, n, &R);
	  /*temp.start = p;
	  pp.x = R.x * 5 + p->x;
	  pp.y = R.y * 5 + p->y;
	  pp.z = R.z * 5 + p->z;
	  temp.end = &pp;
	  r->next = &temp;
	  if (raySphereIntersect(r->next, s1, &t1)) {
		  p->w = 0.0;
	  }*/
  }
  else {
    /* indicates nothing was hit */
    p->w = 0.0;
  }
}
