#pragma once
#include "vert.h"

struct geo;
struct Cur;
struct edge {
	int id = 0;
	vert* v0 = NULL;
	vert* v1 = NULL;

	vec3 o, n;
	double len = 0;
	bool dead = false;
	bool hovered = false;
	bool selected = false;
	edge(Cur& cur, vert* v0, vert* v1);
	void save(FILE* f);
	edge(Cur& cur, geo& g, FILE* f);

	vert* the_other(vert* v) { return v == v0 ? v1 : v0; }
	void Render(Cur& cur, geo& g);
	void Update(Cur& cur, geo& g);
	void PreUpdate(Cur& cur, geo& g);
};
