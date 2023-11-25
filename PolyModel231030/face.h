#pragma once
#include "edge.h"

struct geo;
struct Cur;
struct corner {
	vert* v = NULL;
	edge* e = NULL;
	corner() = default;
	corner(vert* v, edge* e) : v(v), e(e) {}
	void save(FILE* f);
	corner(Cur& cur, geo& g, FILE* f);
};
inline bool operator==(corner const& c0, corner const& c1) {
	return memcmp(&c0, &c1, sizeof(corner)) == 0;
}


struct face {
	int id = 0;
	vector<corner> cs;
	vec3 o, n;
	col3 c;
	double area = 0;
	bool dead = false;
	bool hovered = false;
	bool selected = false;

	face(Cur& cur);
	void save(FILE* f);
	face(Cur& cur, geo& g, FILE* f);
	void Render(Cur& cur, geo& g);
	void Update(Cur& cur, geo& g);
	void PreUpdate(Cur& cur, geo& g);
};
