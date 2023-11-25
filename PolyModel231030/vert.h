#pragma once
#include "vec3.h"

struct geo;
struct Cur;
struct vert {
	int id = 0;
	vec3 o, n;
	bool dead = false;
	bool hovered = false;
	bool selected = false;

	vert(Cur& cur, vec3 o);
	void save(FILE* f);
	vert(Cur& cur, geo& g, FILE* f);

	void Render(Cur& cur, geo& g);
	void Update(Cur& cur, geo& g);
	void PreUpdate(Cur& cur, geo& g);
};
