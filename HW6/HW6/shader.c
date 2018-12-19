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

material* makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb, GLfloat ref, GLfloat trans, GLfloat dif, GLfloat spec) {
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
  m->dif = dif;
  m->spec = spec;
  return(m);
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

GLfloat calculDist(point* p, point q) {

	return (p->x - q.x)*(p->x - q.x) + (p->y - q.y)*(p->y - q.y) + (p->z - q.z)*(p->z - q.z);

}

/* LIGHTING CALCULATIONS */
color calculLighting(GLboolean isFirst, ray* r, point* p, vector* n, material* m) {

	color c = { 0, 0, 0 };
	GLfloat seta = 0, beta = 0, dist = calculDist(p, pointLight.pos_light) + 1;
	color temp;
	vector ptoi = {pointLight.pos_light.x - p->x, pointLight.pos_light.y - p->y , pointLight.pos_light.z - p->z};
	Vnormalize(&ptoi);
	vector itop = {-ptoi.x, -ptoi.y, -ptoi.z};
	vector ra;
	if (isFirst) {
		ra.x = r->end->x - r->start->x;
		ra.y = r->end->y - r->start->y;
		ra.z = r->end->z - r->start->z;
	}
	else {
		ra.x = r->end->x;
		ra.y = r->end->y;
		ra.z = r->end->z;
	}
	seta = calculCross(&ptoi, n) * pointLight.intensity * m->dif;
	beta = calculCross(&ra, &itop) * pointLight.intensity * m->spec;

	temp.r = (pointLight.amb*m->amb + 0.1 * dist * (seta + beta) * pointLight.color.r)
			* (1 - m->ref - m->trans) * m->c.r;
	temp.g = (pointLight.amb*m->amb + 0.1 * dist * (seta + beta) * pointLight.color.g)
			* (1 - m->ref - m->trans) * m->c.g;
	temp.b = (pointLight.amb*m->amb + 0.1 * dist * (seta + beta) * pointLight.color.b)
			* (1 - m->ref - m->trans) * m->c.b;

	c.r = temp.r;
	c.g = temp.g;
	c.b = temp.b;

	return c;

}

// color 값이 0~1 사이 값이도록 조정
void setValue(color* c) {
  /* clamp color values to 0.0 ~ 1.0 */
  if (c->r > 1.0) c->r = 1.0;
  else if (c->r < 0) c->r = 0;

  if (c->g > 1.0) c->g = 1.0;
  else if (c->g < 0) c->g = 0;

  if (c->b > 1.0) c->b = 1.0;
  else if (c->b < 0) c->b = 0;
}

/* mapping */
void mapping(int isMap, point* p, material* m) {

	float x, y, z, rx, ry, rz;
	int index_x, index_y, index_z;

	// background mapping
	if (isMap == 1) {
		x = p->x + 1.5f;
		y = p->y + 1;
		if (x >= 0 && x < 3 && y >= 0 && y < 2) {
			rx = (float) map1_width / 3;
			ry = (float) map1_height / 2;
			index_x = roundf(x*rx);
			index_y = roundf(y*ry);
			if (index_x >= map1_width)
				index_x = map1_width - 1;
			if (index_y >= map1_height)
				index_y = map1_height - 1;
			m->c = colorMap1[index_x][index_y];
		}
	}
	else if (isMap == 2) {
		x = p->x + 2.5f;
		z = p->z + 5;
		if (x >= 0 && x <= 4 && z >= 0 && z <= 4) {
			rx = (float) map2_width / 4;
			rz = (float) map2_height / 4;
			index_x = roundf(x*rx);
			index_z = roundf(z*rz);
			if (index_x >= map2_width)
				index_x = map2_width - 1;
			if (index_z >= map2_height)
				index_z = map2_height - 1;
			m->c = colorMap2[index_x][index_z];
		}
	}

}

/* shade */
/* color of point p with normal vector n and material m returned in c */
void shade(int isMap, GLboolean isFirst, ray* r, point* p,vector* n,material* m, color* c) {

  /* so far, just finds ambient component of color */
  c->r = m->amb * m->c.r;
  c->g = m->amb * m->c.g;
  c->b = m->amb * m->c.b;

  if (isMap != 0)
	  mapping(isMap, p, m);

  if(pointLight.visable)
	  *c = calculLighting(isFirst, r, p, n, m);

  setValue(c);

}

