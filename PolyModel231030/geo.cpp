#include "geo.h"
#include "cur.h"
#include "draw_3d.h"
#include "draw_geo.h"

#include "my_def.h"

geo::geo(Cur& cur) { set_id; }
void geo::save(FILE* f) {
	int sz = 0;
	fwt(id); fwt(o);
	sz = vs.size(); fwt(sz);
	for (auto& vp : vs) { vp.second->save(f); }
	sz = es.size(); fwt(sz);
	for (auto& ep : es) { ep.second->save(f); }
	sz = fs.size(); fwt(sz);
	for (auto& fp : fs) { fp.second->save(f); }
}
geo::geo(Cur& cur, FILE* f) {
	int sz = 0;
	frd(id); frd(o);
	frd(sz); 
	rep(i, 0, sz) {
		auto v = msh<vert>(cur, *this, f);
		vs[v->id] = v;
	}
	frd(sz);
	rep(i, 0, sz) {
		auto e = msh<edge>(cur, *this, f);
		es[e->id] = e;
	}
	frd(sz);
	rep(i, 0, sz) {
		auto fc = msh<face>(cur, *this, f);
		fs[fc->id] = fc;
	}
}

void geo::calc() {
	for (auto ep : es) {
		auto& e = *ep.second;
		auto v0 = e.v0->o;
		auto v1 = e.v1->o;
		e.o = (v0 + v1) / 2;
		e.len = (v1 - v0).len();
	}

	for (auto fp : fs) {
		auto& f = *fp.second;
		f.o = {}; f.area = 0;
		rep(i, 1, f.cs.size() - 1) {
			auto v0 = f.cs[0].v->o;
			auto v1 = f.cs[i].v->o;
			auto v2 = f.cs[i + 1].v->o;
			auto sub_a = cross(v2 - v0, v1 - v0).len() / 2;
			f.o += sub_a * (v0 + v1 + v2) / 3;
			f.area += sub_a;
		} f.o /= f.area;
	}

	for (auto vp : vs) { vp.second->n = {}; }
	for (auto ep : es) { ep.second->n = {}; }

	for (auto fp : fs) {
		auto& f = *fp.second;
		f.n = {};
		rep(i, 1, f.cs.size() - 1) {
			f.n += cross(
				f.cs[i].v->o - f.cs[0].v->o,
				f.cs[i + 1].v->o - f.cs[i].v->o);
		} f.n = f.n.unit();

		for (auto c : f.cs) { 
			c.e->n += f.n * f.area;  
			c.v->n += f.n * f.area;
		}
	}

	for (auto vp : vs) { vp.second->n = vp.second->n.unit(); }
	for (auto ep : es) { ep.second->n = ep.second->n.unit(); }
}
void geo::del_dead() {
	for (auto it = vs.begin(); it != vs.end();) {
		if (it->second->dead) { it = vs.erase(it); }
		else { ++it; }
	}
	for (auto it = es.begin(); it != es.end();) {
		if (it->second->dead) { it = es.erase(it); }
		else { ++it; }
	}
	for (auto it = fs.begin(); it != fs.end();) {
		if (it->second->dead) { it = fs.erase(it); }
		else { ++it; }
	}
}

void geo::Render(Cur& cur) {
	for (auto v : vs) { v.second->Render(cur, *this); }
	for (auto e : es) { e.second->Render(cur, *this); }
	for (auto f : fs) { f.second->Render(cur, *this); }
	if (cur.render_smooth) { return; }

	if (selected && cur.mode == MODE_GEO) {
		auto v = cm.mt * (o - cm.p);
		auto p = proj(v, cm);
		draw_eclipse(scr, dscr, 10, p, 10, 10, cm.vp, dcol(255, 120, 40));
	}
}
void geo::Update(Cur& cur) {
	selected = selected = cur.gs_sel.find(this) != cur.gs_sel.end();
	for (auto v : vs) { v.second->Update(cur, *this); }
	for (auto e : es) { e.second->Update(cur, *this); }
	for (auto f : fs) { f.second->Update(cur, *this); } calc();
}
void geo::PreUpdate(Cur& cur) {
	for (auto v : vs) { v.second->PreUpdate(cur, *this); }
	for (auto e : es) { e.second->PreUpdate(cur, *this); }
	for (auto f : fs) { f.second->PreUpdate(cur, *this); }
}

ptr<geo> MakeCube(Cur& cur) {
	auto g = msh<geo>(cur);
	ptr<vert> v000, v001, v010, v011, v100, v101, v110, v111;
	ptr<edge> ex00, ex01, ex10, ex11;
	ptr<edge> ey00, ey01, ey10, ey11;
	ptr<edge> ez00, ez01, ez10, ez11;
	ptr<face> fx0, fx1, fy0, fy1, fz0, fz1;

	mkp(v000)(cur, vec3(-1, -1, -1));
	mkp(v001)(cur, vec3(-1, -1, +1));
	mkp(v010)(cur, vec3(-1, +1, -1));
	mkp(v011)(cur, vec3(-1, +1, +1));
	mkp(v100)(cur, vec3(+1, -1, -1));
	mkp(v101)(cur, vec3(+1, -1, +1));
	mkp(v110)(cur, vec3(+1, +1, -1));
	mkp(v111)(cur, vec3(+1, +1, +1));

	mkp(ex00)(cur, &*v000, &*v100);
	mkp(ex01)(cur, &*v001, &*v101);
	mkp(ex10)(cur, &*v010, &*v110);
	mkp(ex11)(cur, &*v011, &*v111);
	mkp(ey00)(cur, &*v000, &*v010);
	mkp(ey01)(cur, &*v100, &*v110);
	mkp(ey10)(cur, &*v001, &*v011);
	mkp(ey11)(cur, &*v101, &*v111);
	mkp(ez00)(cur, &*v000, &*v001);
	mkp(ez01)(cur, &*v010, &*v011);
	mkp(ez10)(cur, &*v100, &*v101);
	mkp(ez11)(cur, &*v110, &*v111);

	corner c0, c1, c2, c3;
	c0 = corner(&*v010, &*ey00);
	c1 = corner(&*v000, &*ez00);
	c2 = corner(&*v001, &*ey10);
	c3 = corner(&*v011, &*ez01);
	mkp(fx0)(cur);
	fx0->cs = { c0, c1, c2, c3 };

	c0 = corner(&*v100, &*ey01);
	c1 = corner(&*v110, &*ez11);
	c2 = corner(&*v111, &*ey11);
	c3 = corner(&*v101, &*ez10);
	mkp(fx1)(cur);
	fx1->cs = { c0, c1, c2, c3 };

	c0 = corner(&*v000, &*ex00);
	c1 = corner(&*v100, &*ez10);
	c2 = corner(&*v101, &*ex01);
	c3 = corner(&*v001, &*ez00);
	mkp(fy0)(cur);
	fy0->cs = { c0, c1, c2, c3 };

	c0 = corner(&*v110, &*ex10);
	c1 = corner(&*v010, &*ez01);
	c2 = corner(&*v011, &*ex11);
	c3 = corner(&*v111, &*ez11);
	mkp(fy1)(cur);
	fy1->cs = { c0, c1, c2, c3 };

	c0 = corner(&*v100, &*ex00);
	c1 = corner(&*v000, &*ey00);
	c2 = corner(&*v010, &*ex10);
	c3 = corner(&*v110, &*ey01);
	mkp(fz0)(cur);
	fz0->cs = { c0, c1, c2, c3 };

	c0 = corner(&*v001, &*ex01);
	c1 = corner(&*v101, &*ey11);
	c2 = corner(&*v111, &*ex11);
	c3 = corner(&*v011, &*ey10);
	mkp(fz1)(cur);
	fz1->cs = { c0, c1, c2, c3 };

#define TMP(x) g->vs[v##x->id] = v##x;
	TMP(000); TMP(001); TMP(010); TMP(011); 
	TMP(100); TMP(101); TMP(110); TMP(111);
#undef TMP
#define TMP(x) g->es[e##x->id] = e##x;
	TMP(x00); TMP(x01); TMP(x10); TMP(x11);
	TMP(y00); TMP(y01); TMP(y10); TMP(y11);
	TMP(z00); TMP(z01); TMP(z10); TMP(z11);
#undef TMP
#define TMP(x) g->fs[f##x->id] = f##x;
	TMP(x0); TMP(x1); TMP(y0); TMP(y1); TMP(z0); TMP(z1);
#undef TMP

	return g;
}
