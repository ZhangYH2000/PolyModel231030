#pragma once
#include "app.h"
#include "var.h"

int constexpr MODE_GEO = 0;
int constexpr MODE_VERT = 1;
int constexpr MODE_EDGE = 2;
int constexpr MODE_FACE = 3;
int constexpr MODE_COL = 4;

struct geo;
struct vert;
struct edge;
struct face;
struct Tool;

struct UI;
struct Bgr;
struct clip;
struct param;
struct Cur : App {
	ptr<UI> ui;
	ptr<Bgr> bgr;

	ptr<clip> cl0, cl1;
	clip* cl = NULL;

	double vol = 1;
	bool mute = false;
	bool paint = false;
	bool render_smooth = false;
	col3 color;
	int mode = 0;
	cam cm;
	ptr<Tool> tool;
	int cur_id = 0;
	map<int, ptr<geo>> geos;
	set<geo*> gs_sel;
	set<vert*> vs_sel;
	set<edge*> es_sel;
	set<face*> fs_sel;

	Scope gl;
	wstring dbstr, cmd, tmp_cmd;
	vector<ptr<param>> pars;

	Cur();
	void Save(wstring const& nm) const;
	void Load(wstring const& nm);
	void Reset();
	void Update() override;
	void hdl_mode(int m);
	void render_axis();

	void save_par(FILE* f) const;
	void load_par(FILE* f);
	void init_def_fun();
	void basic_update();
};
