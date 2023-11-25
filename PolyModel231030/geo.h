#pragma once
#include "face.h"

struct Cur;
struct geo {
	int id = 0;
	map<int, ptr<vert>> vs;
	map<int, ptr<edge>> es;
	map<int, ptr<face>> fs;

	vec3 o;
	bool selected = false;

	geo(Cur& cur);
	void save(FILE* f);
	geo(Cur& cur, FILE* f);
	void calc();
	void del_dead();
	void Render(Cur& cur);
	void Update(Cur& cur);
	void PreUpdate(Cur& cur);
};

ptr<geo> MakeCube(Cur& cur);
