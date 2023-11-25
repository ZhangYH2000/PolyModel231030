#pragma once
#include "vec3.h"

int constexpr TOOL_SELECT = 0;
int constexpr TOOL_MOVE = 1;
int constexpr TOOL_SCALE = 2;
int constexpr TOOL_ROTATE = 3;
int constexpr TOOL_EXTRUDE = 4;
int constexpr TOOL_INSET = 5;
int constexpr TOOL_LOOP_CUT = 6;
int constexpr TOOL_BEVEL = 7;
int constexpr TOOL_KNIFE = 8;

struct vert;
struct edge;
struct face;
struct geo;
struct v_info {
	vert* v = NULL;
	vec3 u;
	v_info(vert* v, vec3 u) : v(v), u(u) {}
};

struct Cur;
struct Tool {
	int mode = 0;
	vec3 p, n;
	double r = 0;
	bool axis = false;
	bool hovered = false;
	bool dragged = false;
	set<vert*> vs_sel;
	vector<v_info> vs_info;

	bool knife = false;
	int id_kf = 0;
	bool started_kf = false;
	int i0_kf = 0;
	vector<vec3> vs_kf;

	void Render(Cur& cur);
	void Update(Cur& cur);
	void Discard(Cur& cur);
	void PreUpdate(Cur& cur);

	void extrude(Cur& cur);
	void subdivide(Cur& cur);
	void catmull_clark(Cur& cur, geo& g);
	void subdivide(Cur& cur, geo& g, 
		set<edge*>& es_in, set<face*>& fs_in);
	void merge(Cur& cur);
	void dissolve(Cur& cur);
	void dissolve_face(Cur& cur);
	void dissolve_edge(Cur& cur);
	void loop_cut(Cur& cur);
	void bevel(Cur& cur);
	void bevel(Cur& cur, vert* v);
	void bevel_seg(Cur& cur);
	
	void update_knife(Cur& cur);
	void end_knife(Cur& cur, int i1_kf);
};
