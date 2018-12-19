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
    if (*t < 0)
		return(FALSE);
    else
		return(TRUE);
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

GLfloat calculCross(vector *v1, vector *v2) {
	return (v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z);
}

// 반사 벡터 찾기
vector temp;
void findReflectionRay(vector v, vector* n, ray* R) {

	GLfloat nv = calculCross(&v, n) * 2;

	temp.x = (v.x - nv * n->x);
	temp.y = (v.y - nv * n->y);
	temp.z = (v.z - nv * n->z);
	temp.w = 1;
	Vnormalize(&temp);
	
	R->end = &temp;
	
}

// 그림자가 지는 곳인지 체크
void  isShadow(int flag, point* p) {

	double a = 0, b = 0;
	ray temp;
	point pp;

	temp.start = p;
	pp.x = pointLight.pos_light.x - p->x;
	pp.y = pointLight.pos_light.y - p->y;
	pp.z = pointLight.pos_light.z - p->z;
	temp.end = &pp;

	if (flag != 1) {
		if (raySphereIntersect(&temp, s1, &b)) {
			p->w = -1;
		}
	}

	if (flag != 2) {
		if (raySphereIntersect(&temp, s2, &a)) {
			p->w = -1;
		}
	}

}

ray refl_ray;
void pong(vector ray_v, vector* n, point* p) {
	findReflectionRay(ray_v, n, &refl_ray);
	refl_ray.start = p;
	refl_ray.next = NULL;
}

/* trace */
/* If something is hit, returns the finite intersection point p, 
   the normal vector n to the surface at that point, and the surface
   material m. If no hit, returns an infinite point (p->w = 0.0) */
int j = 0;
void trace (int* flag, ray* r, point* p, vector* n, material* *m) {
  double t_s1 = 0, t_s2 = 0, t_pl1 = 0, t_pl2 = 0;     /* parameter value at first hit */
  int hit_s1 = FALSE, hit_s2 = FALSE, hit_pl1 = FALSE, hit_pl2 = FALSE;
  int flagNum = *flag;
  vector ray_v = { r->end->x, r->end->y , r->end->z };
  //Vnormalize(&ray_v);
  
  if(flagNum != 1)
	  hit_s1 = raySphereIntersect(r, s1,&t_s1);		// 화면에 있는 구 s1 에 ray r이 닿았는지 체크
  if(flagNum != 2)
	  hit_s2 = raySphereIntersect(r, s2, &t_s2);		// 화면에 있는 구 s2 에 ray r이 닿았는지 체크
  
  // 더 가까운 곳에 닿은 걸로 처리 (Sphere)
  if (hit_s1 && hit_s2) {
	  if (t_s1 <= t_s2) {
		  hit_s2 = FALSE;
	  }
	  else {
		  hit_s1 = FALSE;
	  }
  }
  // Sphere에 안 닿았을 경우, Plane에 닿는지 처리
  else if (!hit_s1 && !hit_s2) {
	  if (flagNum != 3) {
		  t_pl1 = (pl1->c->y - r->start->y) / ray_v.y;		// 화면에 있는 plane pl1 과 ray r  체크
		  if (r->end->y > 0)
			  hit_pl1 = TRUE;
	  }
	  if (flagNum != 4) {
		  t_pl2 = (pl2->c->z - r->start->z) / ray_v.z;		// 화면에 있는 plane pl2 와 ray r  체크
		  if (r->end->z < 0)
			  hit_pl2 = TRUE;
	  }
	  // 더 가까운 곳에 닿은 걸로 처리 (Plane)
	  if (hit_pl1 && hit_pl2) {
		  if (t_pl1 <= t_pl2) {
			  hit_pl2 = FALSE;
		  }
		  else {
			  hit_pl1 = FALSE;
		  }
	  }
  }

  // 닿은 물체에 따른 처리
  if (hit_s1) {
	  *m = s1->m;
	  findPointOnRay(r, t_s1, p);		// 닿았으면 닿은 점 p 찾기
	  findSphereNormal(s1, p, n);		// 그 점에서의 normal vector n 찾기
	  if (flagNum == 0) {
		  pong(ray_v, n, p);
		  r->next = &refl_ray;
	  }
	  isShadow(1, p);							// 그림자 지는 부분인지 체크
	  *flag = 1;
  }
  else if (hit_s2) {
	  *m = s2->m;
	  findPointOnRay(r, t_s2, p);
	  findSphereNormal(s2, p, n);
	  if (flagNum == 0) {
		  pong(ray_v, n, p);
		  r->next = &refl_ray;
	  }
	  isShadow(2, p);
	  *flag = 2;
  }
  else if (hit_pl1) {
	  *m = pl1->m;
	  findPointOnRay(r, t_pl1, p);
	  *n = pl1->normal;
	  if (flagNum == 0) {
		  pong(ray_v, n, p);
		  r->next = &refl_ray;
	  }
	  isShadow(0, p);
	  *flag = 3;
  }
  else if (hit_pl2) {
	  *m = pl2->m;
	  findPointOnRay(r, t_pl2, p);
	  *n = pl2->normal;
	  if (flagNum == 0) {
		  pong(ray_v, n, p);
		  r->next = &refl_ray;
	  }
	  isShadow(0, p);
	  *flag = 4;
  }
  else {
    /* indicates nothing was hit */
    p->w = 0.0;
  }
}
