/******************************************************************/
/*         Lighting functions, Final Project                   */
/******************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include "raytrace.h"

material* makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb, GLfloat ref, GLfloat trans) {
  material* m;
  
  /* allocate memory */
  m = (material*) malloc(sizeof(material));
  /* put stuff in it */
  m->c.r = r;
  m->c.g = g;
  m->c.b = b;
  m->amb = amb;
  m->ref = ref;
  m->trans = trans;
  return(m);
}

GLfloat calculDist(point* p, point q) {

	return (p->x - q.x)*(p->x - q.x) + (p->y - q.y)*(p->y - q.y) + (p->z - q.z)*(p->z - q.z);

}

GLfloat calculCos(vector *v1, vector *v2) {
	return (v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z);
}

int i = 0;
/* LIGHTING CALCULATIONS */
color calculLighting(ray* r, point* p, vector* n, material* m) {

	color c = { 0, 0, 0 };
	GLfloat seta = 0, beta = 0, dist = calculDist(p, L.pos_light) + 1;
	color temp;
	vector ptoi = {L.pos_light.x - p->x, L.pos_light.y - p->y , L.pos_light.z - p->z};
	Vnormalize(&ptoi);
	vector itop = {-ptoi.x, -ptoi.y, -ptoi.z};
	vector ra = {r->end->x - r->start->x, r->end->y - r->start->y , r->end->z - r->start->z};
	Vnormalize(&ra);
	seta = calculCos(&ptoi, n) * L.intensity;		// * dif = 1
	beta = calculCos(&ra, &itop) * L.intensity * 0.5;		// * spec = 0.5

	temp.r = (L.amb*m->amb + 0.1 * dist * (seta + beta))
			* (1 - m->ref - m->trans) * m->c.r;
	temp.g = (L.amb*m->amb + 0.1 * dist * (seta + beta))
			* (1 - m->ref - m->trans) * m->c.g;
	temp.b = (L.amb*m->amb + 0.1 * dist * (seta + beta))
			* (1 - m->ref - m->trans) * m->c.b;
	
	if (i == 0) {
		i++;
		printf("%f %f \n", seta, beta);
		printf("%f %f %f \n", temp.r, temp.g, temp.b);
	}

	c.r = temp.r;
	c.g = temp.g;
	c.b = temp.b;

	return c;

}

/* shade */
/* color of point p with normal vector n and material m returned in c */
void shade(ray* r, point* p,vector* n,material* m, color* c) {

  /* so far, just finds ambient component of color */
  c->r = m->amb * m->c.r;
  c->g = m->amb * m->c.g;
  c->b = m->amb * m->c.b;

  if(L.visable)
	  *c = calculLighting(r, p, n, m);

  /* clamp color values to 0.0 ~ 1.0 */
  if (c->r > 1.0) c->r = 1.0;
  else if (c->r < 0) c->r = 0;

  if (c->g > 1.0) c->g = 1.0;
  else if (c->g < 0) c->g = 0;

  if (c->b > 1.0) c->b = 1.0;
  else if (c->b < 0) c->b = 0;

}

