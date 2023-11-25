#include "cur.h"
#include "ui.h"
#include "bgr.h"
#include "par.h"
#include "geo.h"
#include "tool.h"
#include "clip.h"
#include "draw_3d.h"
#include "fpn_col.h"

Cur::Cur() {
	w = 1800; h = 900; set_locale();
	wv.len_block *= 4;
	print_console(L"正在加载字体...");
	App::Init();
	print_console(L"字体加载完成.", true);
	print_console(L"正在加载控件...");
	mkp(ui)(*this); ui->Init(*this);
	mkp(bgr)(*this); Reset();
	print_console(L"控件加载完成.", true);
	print_console(L"正在加载音乐...");
	mkp(cl0)(L"sound/bgm0.clip");
	mkp(cl1)(L"sound/bgm1.clip");
	print_console(L"音乐加载完成.", true);
	cl = frnd(1) < 0.5 ? &*cl0 : &*cl1;
}

#define cur (*this)
#include "my_def.h"

void Cur::Save(wstring const& nm) const {
	FILE* f = wfopen(nm, L"wb");
	if (!f) { return; }
	int sz = 0; fwtv(cmd); save_par(f);

	fwt(cur_id); cm.save(f);
	sz = geos.size(); fwt(sz);
	for (auto& gp : geos) { gp.second->save(f); }
	
	fclose(f);
}
void Cur::Load(wstring const& nm) {
	FILE* f = wfopen(nm, L"rb");
	if (!f) { return; } Reset();
	int sz = 0; frdv(cmd); 
	tmp_cmd = cmd; load_par(f);
	
	frd(cur_id); cm = cam(f);
	frd(sz);
	rep(i, 0, sz) {
		auto g = msh<geo>(cur, f);
		geos[g->id] = g;
	}

	// 这个究竟在什么时候好像也有一点讲究，不过这里不在意。
	Execute(gl, Compile(cmd)); fclose(f);
}
void Cur::Reset() {
	gl.clear(); init_def_fun(); pars.clear();

	paint = false;
	cur_id = 0;
	geos.clear();
	vs_sel.clear();
	gs_sel.clear();
	es_sel.clear();
	fs_sel.clear();
	cm = cam(bgr.vp());
	mkp(tool)();
	auto g = MakeCube(*this);
	geos[g->id] = g;
}
void Cur::Update() {
	bgr.PreUpdate(*this);
	tool->PreUpdate(*this);
	for (auto g : geos) { g.second->PreUpdate(*this); }
	ui.PreUpdate(*this);
	basic_update();

	if (cl0 && cl1) {
		if (cl->csp >= cl->n()) {
			cl->csp = 0;
			cl = (cl == &*cl0) ? &*cl1 : &*cl0;
		}
		if (!mute) { cl->play(wv.wvin); }
	}

	if (!kb) {
		if (kbc(L'R')) { render_smooth = !render_smooth; }
		if (kbc(L'V')) { hdl_mode(MODE_VERT); }
		if (kbc(L'E')) { hdl_mode(MODE_EDGE); }
		if (kbc(L'F')) { hdl_mode(MODE_FACE); }
		if (kbc(L'G')) { hdl_mode(MODE_GEO); }
	}
	if (!msd[0]) { paint = false; }

	bgr.Update(*this);
	tool->Update(*this);
	for (auto g : geos) { g.second->Update(*this); }
	tool->Render(*this);
	for (auto g : geos) { g.second->Render(*this); }
	if (!render_smooth) { render_axis(); }
	ui.Update(*this);
}

void Cur::hdl_mode(int m) {
	mode = m;
	cur.vs_sel.clear();
	cur.es_sel.clear();
	cur.fs_sel.clear();
	cur.paint = false;
	if (m == MODE_COL) { ui.fpn_col->show(cur); }
	if (m == MODE_GEO) { return; }

	if (!cur.gs_sel.empty()) { cur.gs_sel = { *cur.gs_sel.begin() }; }
	else if (!cur.geos.empty()) {
		cur.gs_sel = { &*cur.geos.begin()->second };
	} else { cur.mode = MODE_GEO; }
}
void Cur::render_axis() {
	double len = 50;
	rep(i, 1, 50) if (abs(cm.phi) > 0.15) {
		draw_3d_seg(scr, dscr, vec3(-len, +i, 0), vec3(len, +i, 0), cm, dcol(60));
		draw_3d_seg(scr, dscr, vec3(+i, -len, 0), vec3(+i, len, 0), cm, dcol(60));
		draw_3d_seg(scr, dscr, vec3(-len, -i, 0), vec3(len, -i, 0), cm, dcol(60));
		draw_3d_seg(scr, dscr, vec3(-i, -len, 0), vec3(-i, len, 0), cm, dcol(60));
	}

	draw_3d_seg(scr, dscr, vec3(-len, 0, 0), vec3(len, 0, 0), cm, dcol(255, 0, 0));
	draw_3d_seg(scr, dscr, vec3(0, -len, 0), vec3(0, len, 0), cm, dcol(0, 255, 0));
}

void Cur::save_par(FILE* f) const {
	int sz = pars.size(); fwt(sz);
	for (auto p : pars) { p->save(f); }
}
void Cur::load_par(FILE* f) {
	int sz = 0; frd(sz);
	rep(i, 0, sz) { pars.push_back(msh<param>(f)); }
}
void Cur::init_def_fun() {}
void Cur::basic_update() {
	title = loc(L"title");
	if (gl.find(L"dbstr") != gl.end()) { dbstr = gl[L"dbstr"]->str; }
	if (gl.find(L"update") != gl.end()) {
		auto f = gl[L"update"];  Execute(gl, f->procs);
	}

	pars.erase(remove_if(pars.begin(), pars.end(),
		[](ptr<param> p) { return p->del; }), pars.end());
	for (auto& p : pars) { gl[p->nm] = msh<Var>(p->val); }
}
