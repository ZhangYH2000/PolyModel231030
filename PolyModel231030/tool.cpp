#include "tool.h"
#include "cur.h"
#include "geo.h"
#include "draw_3d.h"
#include "draw_geo.h"
#include "draw_curve.h"
#include "draw_px_seg.h"

#include "my_def.h"

double constexpr dep = 20;

// shared_ptr 的复制开销是比较大，下面很多地方没有考虑到这一点，以后要更正。
void Tool::Render(Cur& cur) {
	if (started_kf) {
		auto v0 = (*cur.fs_sel.begin())->cs[i0_kf].v->o;
		auto p0 = proj(cm.mt * (v0 - cm.p), cm);
		draw_eclipse(scr, dscr, 100, p0, 12, 12, cm.vp, dcol(0, 150, 0));
		draw_eclipse_frame(scr, dscr, 100, p0, 12, 12, cm.vp, dcol(255), 50);

		for (auto &v1 : vs_kf) {
			auto p1 = proj(cm.mt * (v1 - cm.p), cm);
			draw_eclipse(scr, dscr, 100, p1, 12, 12, cm.vp, dcol(0, 150, 0));
			draw_eclipse_frame(scr, dscr, 100, p1, 12, 12, cm.vp, dcol(255), 50);
			draw_px_seg(scr, dscr, p0, p1, 99, cm.vp, dcol(0, 255, 0));
			v0 = v1; p0 = p1;
		}
	}
	if (!r) { return; }
	auto v = proj(cm.mt * (p - cm.p), cm);
	draw_eclipse_frame(scr, dscr, dep, v, r, r, cm.vp, dcol(255), 50);
}
void Tool::Update(Cur& cur) {
	hovered = (hvd == this);

	auto old_p = p;
	vs_sel.clear();
	r = 0; n = {}; p = {};
	bool valid = false;
	
	// 这一部分也需要抽象和简化了。 
	if (cur.mode == MODE_GEO && !cur.gs_sel.empty()) {
		valid = true; double sum = 0;
		for (auto g : cur.gs_sel) {
			sum += 1; p += g->o;
			for (auto &v : g->vs) { vs_sel.insert(&*v.second); }
		} p /= sum;
	}
	if (cur.mode == MODE_FACE && !cur.fs_sel.empty()) {
		valid = true; double sum = 0;
		for (auto f : cur.fs_sel) {
			sum += f->area; 
			p += f->o * f->area;
			n += f->n * f->area;
			for (auto &c : f->cs) { vs_sel.insert(c.v); }
		} p /= sum; n = n.unit();
	}
	// 这里线的平均法向量与 blender 不同，不知道怎么考虑的。
	if (cur.mode == MODE_EDGE && !cur.es_sel.empty()) {
		valid = true; double sum = 0;
		for (auto e : cur.es_sel) {
			sum += e->len; 
			p += e->o * e->len;
			n += e->n * e->len;
			vs_sel.insert(e->v0); vs_sel.insert(e->v1);
		} p /= sum; n = n.unit();
	}
	if (cur.mode == MODE_VERT && !cur.vs_sel.empty()) {
		valid = true; double sum = 0;
		for (auto v : cur.vs_sel) {
			sum += 1; 
			p += v->o;
			n += v->n;
			vs_sel.insert(v);
		} p /= sum; n = n.unit();
	}
	if (dragged && mode == TOOL_BEVEL) { p = old_p; }
	if (dragged && mode == TOOL_INSET) { p = old_p; }
	if (dragged && mode == TOOL_LOOP_CUT) {
		if (!vs_info.empty()) {
			auto& v = vs_info.front(); p = v.v->o;
		}
	}
	valid = valid || dragged;
	valid = valid && mode != TOOL_SELECT && mode != TOOL_KNIFE;
	if (valid) {
		bool tiny =
			mode == TOOL_MOVE || mode == TOOL_LOOP_CUT;
		r = tiny ? 15 : 120;
	}

	auto dmsp = msp - msp_old;
	auto o = p;
	o = cm.mt * (o - cm.p);
	o.z -= dmsp.y / (cm.scl * cm.vp.h) * o.y;
	o.x += dmsp.x / (cm.scl * cm.vp.h) * o.y;
	o = cm.mt.tsp() * o + cm.p;

	double theta = -dmsp.x * 0.005;
	double phi = dmsp.y * 0.005;
	auto tsf =
		mat3::rot(cm.mt.tsp().v2(), -theta) *
		mat3::rot(cm.mt.tsp().v0(), phi);

	auto q = cm.mt * (p - cm.p);
	auto qp = proj(q, cm);
	vec2 q1 = qp - (vec2)msp;
	vec2 q0 = qp - (vec2)msp_old;
	double d_ratio = q1.len() / q0.len();
	double d_dif = (q1.len() - q0.len()) / (cm.scl * cm.vp.h) * q.y;

	axis = false;
	if (!kb) {
		if (kbd[L'N']) { axis = true; }
		if (kbd[L'X']) { axis = true; n = vec3(1, 0, 0); }
		if (kbd[L'Y']) { axis = true; n = vec3(0, 1, 0); }
		if (kbd[L'Z']) { axis = true; n = vec3(0, 0, 1); }
		if (kbc(L'B')) {
			if (cur.mode == MODE_GEO) {
				auto g = MakeCube(cur); 
				cur.geos[g->id] = g;
				g->o += cm.look;
				for (auto &v : g->vs) { v.second->o += cm.look; }
				cur.gs_sel = { &*g };
			}
		}
		if (kbc(VK_DELETE)) {
			if (cur.mode == MODE_GEO) {
				// 不知道是否可以有更有效率的算法。
				for (auto g : cur.gs_sel) 
				for (auto it = cur.geos.begin(); it != cur.geos.end(); ++it) {
					if (&*it->second == g) { cur.geos.erase(it); break; }
				} cur.gs_sel.clear();
			}
		}
		if (kbc(L'S')) { subdivide(cur); }
		if (kbc(L'M')) { merge(cur); }
		if (kbc(L'D')) { dissolve(cur); }
		if (kbc(L'C')) {
			if (cur.mode == MODE_GEO) {
				for (auto g : cur.gs_sel) { catmull_clark(cur, *g); }
			}
		}
	}

	if (mode == TOOL_KNIFE) { update_knife(cur); }

	// 感觉这里顺序有点乱了。
	if (dragged && mode == TOOL_LOOP_CUT) {
		if (!vs_info.empty()) {
			auto& v = vs_info.front();
			n = v.u; axis = true;
		}
	}

	if (axis) {
		auto v = cm.mt.tsp() * vec3(0, 1, 0);
		if (abs(dot(v, n)) > 0.95 || n.zero()) { o = p; }
		else {
			auto perp = (n - dot(n, v) * v).unit();
			o = p + dot(o - p, perp) * n / cross(v, n).len();
		}

		tsf = mat3::rot(n, atan2(q1.y, q1.x) - atan2(q0.y, q0.x));
		// 不知道这个考虑有没有必要。
		if (n.zero()) { tsf = mat3::id(); }
	}

	if (!dragged) {
		// 感觉有点乱，就是会不会造成不好的情形。
		dragged = hovered && 
			msc(0) && mode != TOOL_SELECT && mode != TOOL_KNIFE;
		if (dragged) {
			if (mode == TOOL_EXTRUDE) { extrude(cur); }
			if (mode == TOOL_INSET) { extrude(cur); }
			if (mode == TOOL_LOOP_CUT) { loop_cut(cur); }
			if (mode == TOOL_BEVEL) { bevel(cur); }
		}
	
	} else {
		dragged = msd[0];
		bool move =
			mode == TOOL_MOVE ||
			mode == TOOL_EXTRUDE && cur.mode == MODE_FACE;
		if (move) {
			if (cur.mode == MODE_GEO)
			for (auto g : cur.gs_sel) { g->o += o - p; }
			for (auto v : vs_sel) { v->o += o - p; }
		} else if (mode == TOOL_SCALE) {
			if (axis) {
				for (auto v : vs_sel) {
					v->o += dot(v->o - p, n) * n * (d_ratio - 1);
				}
			} else {
				for (auto v : vs_sel) { v->o = p + (v->o - p) * d_ratio; }
			}
		} else if (mode == TOOL_ROTATE) {
			for (auto v : vs_sel) { v->o = p + tsf * (v->o - p); }
		} else if (mode == TOOL_INSET || mode == TOOL_BEVEL) {
			for (auto& v : vs_info) { v.v->o += v.u * d_dif; }
		} else if (mode == TOOL_LOOP_CUT) {
			for (auto& v : vs_info) { v.v->o += v.u * dot(o - p, n); }
		}
	}
}
void Tool::Discard(Cur& cur) {
	hovered = dragged = false;
	vs_info.clear(); vs_sel.clear();

	knife = false; id_kf = 0; 
	started_kf = false; i0_kf = 0; vs_kf.clear();
}
void Tool::PreUpdate(Cur& cur) {
	if (mode == TOOL_KNIFE) {
		// 怎么处理这个 knife 是个问题，不知道专门搞一个类来做这个如何。
		// 为什么要着重验证 knife 操作的合理，就是说中途相关的面是否删除之类。
		// 因为如果这里出问题了，是真的会有严重错误的。
		bool ok = dhv <= 100 && knife && insd(msp, cm.vp);
		if (ok) { dhv = dep; hvd = this; }
		return;
	}

	if (!r) { return; }
	auto v = proj(cm.mt * (p - cm.p), cm);
	bool ok = dhv <= dep &&
		(v - (vec2)msp).lensqr() < r * r && insd(msp, cm.vp);
	if (ok) { dhv = dep; hvd = this; }
}

struct ex_v {
	ptr<vert> v;
	bool in = false;
	bool out = false;
	ptr<edge> e;
	vec3 n, u;
	bool bd() { return in && out; }
};
struct ex_e {
	ptr<edge> e;
	vert* start = NULL;
	bool in = false;
	bool out = false;
	bool bd() { return in && out;  }
};
void Tool::extrude(Cur& cur) {
	vs_info.clear();
	if (cur.mode != MODE_FACE) { return; }

	map<vert*, ex_v> vs; map<edge*, ex_e> es;
	auto& g = **cur.gs_sel.begin();

	// 我不知道如何评价这个方法的效率。
	for (auto& v : g.vs) { vs[&*v.second] = {}; }
	for (auto& e : g.es) { es[&*e.second] = {}; }
	for (auto &f : g.fs) {
		auto in = cur.fs_sel.find(&*f.second) != cur.fs_sel.end();
		for (auto &c : f.second->cs) {
			auto& v = vs[c.v]; 
			auto& e = es[c.e];
			if (in) { 
				v.n += f.second->n;
				// 这里其实可以不用 start，直接调换顶点顺序就行了。
				// 暂时不改这一点。
				v.in = true; e.in = true; e.start = c.v; 
			} else { v.out = true; e.out = true;  }
		}
	}

	for (auto &vp : vs) {
		auto& v = *vp.first; auto& ve = vp.second;
		// ve.n = ve.n.unit();
		if (!ve.bd()) { continue; }
		mkp(ve.v)(cur, v.o); g.vs[ve.v->id] = ve.v;
		mkp(ve.e)(cur, &v, &*ve.v); g.es[ve.e->id] = ve.e;
	}
	for (auto& ep : es) {
		auto& e = *ep.first; auto& ee = ep.second;
		auto& ve0 = vs[e.v0];
		auto& ve1 = vs[e.v1];
		auto u = (e.v1->o - e.v0->o).unit();
		if (ee.start != e.v0) { u = -u; }
		if (ee.in && !ee.out) {
			if (ve0.bd()) { e.v0 = &*ve0.v; }
			if (ve1.bd()) { e.v1 = &*ve1.v; }
		}
		if (!ee.bd()) { continue; }
		ve0.u += u;
		ve1.u += u;
		mkp(ee.e)(cur, &*vs[e.v0].v, &*vs[e.v1].v);
		g.es[ee.e->id] = ee.e;
	}

	for (auto f : cur.fs_sel) {
		for (auto& c : f->cs) {
			auto& ve = vs[c.v]; 
			auto& ee = es[c.e];
			if (ve.bd()) { c.v = &*ve.v; }
			if (ee.bd()) { c.e = &*ee.e; }
		}
	}
	for (auto& ep : es) {
		auto& e = *ep.first; auto& ee = ep.second;
		if (!ee.bd()) { continue; }
		// 感觉流形结构在这里有用，要不然会很奇怪。
		auto v0 = ee.start;
		auto v1 = e.the_other(v0);
		auto f = msh<face>(cur);
		// 侧边统一是四边形了。
		f->cs.push_back(corner(v0, &e));
		f->cs.push_back(corner(v1, &*vs[v1].e));
		f->cs.push_back(corner(&*vs[v1].v, &*es[&e].e));
		f->cs.push_back(corner(&*vs[v0].v, &*vs[v0].e));
		g.fs[f->id] = f;
	}

	for (auto& vp : vs) {
		auto& v = *vp.first; auto& ve = vp.second;
		if (!ve.bd()) { continue; }
		// 这么搞对正方形还行，但不知道跟 blender 是否一样。
		auto u = cross(ve.n.unit(), ve.u);
		u = -1 / u.len() * u.unit();
		vs_info.push_back(v_info(&*ve.v, u));
	}
}

struct sd_v {
	int n = 0;
	vec3 sum_f;
	vec3 sum_e;
};
struct sd_e {
	ptr<vert> v;
	ptr<edge> e0, e1;
	vec3 avg_v;
	vec3 sum_f;
};
void Tool::subdivide(Cur& cur) {
	if (cur.mode == MODE_COL) { return; }
	if (cur.mode == MODE_VERT) { return; }
	if (cur.mode == MODE_GEO) {
		for (auto g : cur.gs_sel) {
			set<edge*> es_in; set<face*> fs_in;
			for (auto& e : g->es) { es_in.insert(&*e.second); }
			for (auto& f : g->fs) { fs_in.insert(&*f.second); }
			subdivide(cur, *g, es_in, fs_in);
		} return;
	}

	auto& g = **cur.gs_sel.begin();
	set<edge*> es_in; set<face*> fs_in;

	if (cur.mode == MODE_FACE) {
		for (auto f : cur.fs_sel) for (auto& c : f->cs) {
			es_in.insert(c.e);
		} fs_in = cur.fs_sel;
	} else if (cur.mode == MODE_EDGE) {
		es_in = cur.es_sel;
	}

	subdivide(cur, g, es_in, fs_in);
	if (cur.mode == MODE_EDGE) { cur.es_sel = es_in; }
	if (cur.mode == MODE_FACE) { cur.fs_sel = fs_in; }
}
void Tool::subdivide(Cur& cur, geo& g, 
	set<edge*>& es_in, set<face*>& fs_in) {
	map<edge*, sd_e> es;

	set<edge*> eos;
	for (auto e : es_in) {
		e->dead = true;
		sd_e ei;
		auto v0 = e->v0;
		auto v1 = e->v1;
		mkp(ei.v)(cur, (v0->o + v1->o) / 2);
		mkp(ei.e0)(cur, v0, &*ei.v);
		mkp(ei.e1)(cur, v1, &*ei.v);
		g.vs[ei.v->id] = ei.v;
		g.es[ei.e0->id] = ei.e0;
		g.es[ei.e1->id] = ei.e1; es[e] = ei;
		eos.insert(&*ei.e0);
		eos.insert(&*ei.e1);
	}
	for (auto fp : g.fs) {
		auto& f = *fp.second;
		vector<corner> cs;
		for (auto& c : f.cs) {
			bool in = es.find(c.e) != es.end();
			if (!in) { cs.push_back(c); continue; }
			auto& ei = es[c.e];
			if (c.e->v0 == c.v) {
				cs.push_back(corner(&*c.v, &*ei.e0));
				cs.push_back(corner(&*ei.v, &*ei.e1));
			} else {
				cs.push_back(corner(&*c.v, &*ei.e1));
				cs.push_back(corner(&*ei.v, &*ei.e0));
			}
		} f.cs = cs;
	}

	set<face*> fos;
	for (auto f : fs_in) {
		f->dead = true;
		auto v = msh<vert>(cur, f->o);
		g.vs[v->id] = v;
		int m = f->cs.size() / 2;
		vector<ptr<edge>> es;
		rep(i, 0, m) {
			int a = 2 * i + 1;
			auto e = msh<edge>(cur, f->cs[a].v, &*v);
			es.push_back(e);
			g.es[e->id] = e;
		}

		rep(i, 0, m) {
			int j = (i + 1) % m;
			int a = 2 * i + 1;
			int b = (a + 1) % f->cs.size();
			int c = (a + 2) % f->cs.size();
			auto fo = msh<face>(cur);
			fo->c = f->c;
			fo->cs.push_back(corner(&*v, &*es[i]));
			fo->cs.push_back(f->cs[a]);
			fo->cs.push_back(f->cs[b]);
			fo->cs.push_back(corner(f->cs[c].v, &*es[j]));
			g.fs[fo->id] = fo; fos.insert(&*fo);
		}
	} g.del_dead();
	es_in = eos; fs_in = fos;
}
void Tool::catmull_clark(Cur& cur, geo& g) {
	map<vert*, sd_v> vs; map<edge*, sd_e> es;
	for (auto v : g.vs) { vs[&*v.second] = {}; }

	set<ptr<edge>> eos;
	for (auto &ep : g.es) {
		auto e = &*ep.second;
		e->dead = true;
		sd_e ei;
		auto v0 = e->v0;
		auto v1 = e->v1;
		ei.avg_v = (v0->o + v1->o) / 2;
		vs[v0].sum_e += ei.avg_v;
		vs[v1].sum_e += ei.avg_v;
		mkp(ei.v)(cur, (v0->o + v1->o) / 2);
		mkp(ei.e0)(cur, v0, &*ei.v);
		mkp(ei.e1)(cur, v1, &*ei.v);
		g.vs[ei.v->id] = ei.v;
		es[e] = ei;
		eos.insert(ei.e0);
		eos.insert(ei.e1);
	}
	for (auto fp : g.fs) {
		auto& f = *fp.second;
		vector<corner> cs;
		for (auto& c : f.cs) {
			auto& vi = vs[c.v];
			auto& ei = es[c.e];
			vi.n++;
			vi.sum_f += f.o;
			ei.sum_f += f.o;
			if (c.e->v0 == c.v) {
				cs.push_back(corner(&*c.v, &*ei.e0));
				cs.push_back(corner(&*ei.v, &*ei.e1));
			} else {
				cs.push_back(corner(&*c.v, &*ei.e1));
				cs.push_back(corner(&*ei.v, &*ei.e0));
			}
		} f.cs = cs;
	}

	set<ptr<face>> fos;
	for (auto& fp : g.fs) {
		auto f = &*fp.second;
		f->dead = true;
		auto v = msh<vert>(cur, f->o);
		g.vs[v->id] = v;
		int m = f->cs.size() / 2;
		vector<ptr<edge>> es;
		rep(i, 0, m) {
			int a = 2 * i + 1;
			auto e = msh<edge>(cur, f->cs[a].v, &*v);
			es.push_back(e);
			g.es[e->id] = e;
		}

		rep(i, 0, m) {
			int j = (i + 1) % m;
			int a = 2 * i + 1;
			int b = (a + 1) % f->cs.size();
			int c = (a + 2) % f->cs.size();
			auto fo = msh<face>(cur);
			fo->c = f->c;
			fo->cs.push_back(corner(&*v, &*es[i]));
			fo->cs.push_back(f->cs[a]);
			fo->cs.push_back(f->cs[b]);
			fo->cs.push_back(corner(f->cs[c].v, &*es[j]));
			fos.insert(fo);
		}
	} g.del_dead();

	for (auto e : eos) { g.es[e->id] = e; }
	for (auto f : fos) { g.fs[f->id] = f; }
	
	for (auto& vp : vs) {
		auto& v = *vp.first; auto& vi = vp.second;
		v.o = ((vi.sum_f + 2 * vi.sum_e) / vi.n + (vi.n - 3) * v.o) / vi.n;
	}
	for (auto& ep : es) {
		auto& e = *ep.first; auto& ei = ep.second;
		ei.v->o = (ei.avg_v + ei.sum_f / 2) / 2;
	}
}

struct mg_v {
	bool in = false;
	set<edge*> es;
};
struct mg_e {
	edge* beg = NULL;
	set<edge*> es;
};
void Tool::merge(Cur& cur) {
	if (vs_sel.empty()) { return; }
	if (cur.mode == MODE_GEO) { return; }
	if (cur.mode == MODE_COL) { return; }

	auto v_beg = *vs_sel.begin();
	for (auto v : vs_sel) if (v != v_beg) { v->dead = true; }
	map<vert*, mg_v> vs;
	auto& g = **cur.gs_sel.begin();
	for (auto& vp : g.vs) { 
		auto& v = *vp.second;
		vs[&v].in = vs_sel.find(&v) != vs_sel.end();
	}

	for (auto& ep : g.es) {
		auto& e = *ep.second;
		auto& vi0 = vs[e.v0];
		auto& vi1 = vs[e.v1];
		if (vi0.in && vi1.in) { e.dead = true; }
		if (vi0.in && !vi1.in) { vi1.es.insert(&e); e.v0 = v_beg; }
		if (vi1.in && !vi0.in) { vi0.es.insert(&e); e.v1 = v_beg; }
	}

	vector<mg_e> es;
	for (auto& vp : vs) {
		auto& v = *vp.first; auto& vi = vp.second;
		if (vi.es.size() >= 2) {
			mg_e ei;
			ei.beg = *vi.es.begin();
			ei.es = vi.es; es.push_back(ei);
			for (auto& e : ei.es) if (e != ei.beg) { e->dead = true; }
		}
	}

	// 面点替换, 创建新面
	set<ptr<face>> fos;
	for (auto& fp : g.fs) {
		auto& f = *fp.second;
		bool ok = true; int i0 = 0;

		rep(i, 0, f.cs.size()) {
			auto& c = f.cs[i];
			if (vs[c.v].in) { 
				c.v = v_beg; 
				ok = false; i0 = i;
			}
			for (auto& ei : es) {
				bool in = ei.es.find(c.e) != ei.es.end();
				if (in) { c.e = ei.beg; }
			}
		}
		if (ok) { continue; }

		f.dead = true;
		vector<corner> cs = { f.cs[i0] };
		rep(i, 1, f.cs.size() + 1) {
			auto& c = f.cs[(i0 + i) % f.cs.size()];
			if (c.v == v_beg) {
				if (cs.size() > 2) {
					auto fo = msh<face>(cur);
					fo->cs = cs; fos.insert(fo);
				} cs = { c };
			} else { cs.push_back(c); }
		}
	}

	v_beg->o = p;
	for (auto f : fos) { g.fs[f->id] = f; }
	cur.fs_sel.clear(); 
	cur.es_sel.clear(); 
	cur.vs_sel.clear();
	g.del_dead();
}

void Tool::dissolve(Cur& cur) {
	if (cur.mode == MODE_FACE) { dissolve_face(cur); }
	else if (cur.mode == MODE_EDGE) { dissolve_edge(cur); }
}

void Tool::dissolve_face(Cur& cur) {
	map<vert*, ex_v> vs; map<edge*, ex_e> es;
	auto& g = **cur.gs_sel.begin();

	for (auto& v : g.vs) { vs[&*v.second] = {}; }
	for (auto& e : g.es) { es[&*e.second] = {}; }
	for (auto& f : g.fs) {
		auto in = cur.fs_sel.find(&*f.second) != cur.fs_sel.end();
		for (auto& c : f.second->cs) {
			auto& v = vs[c.v];
			auto& e = es[c.e];
			if (in) { 
				v.in = true; e.in = true;
				if (c.e->v0 != c.v) { swap(c.e->v0, c.e->v1); }
			} else { v.out = true; e.out = true; }
		}
	}

	set<edge*> es_bd;
	for (auto& ep : es) 
	if (ep.second.bd()) { es_bd.insert(ep.first); }

	if (es_bd.size() <= 2) { return; }
	auto& e0 = **es_bd.begin();
	vector<corner> cs = { corner(e0.v0, &e0) };
	while (1) {
		auto v = cs.back().e->v1;
		if (v == e0.v0) { break; }
		auto it = find_if(es_bd.begin(), es_bd.end(),
			[v](edge* e) { return e->v0 == v; });
		if (it == es_bd.end()) { return; }
		cs.push_back(corner(v, *it));
	}
	if (cs.size() < es_bd.size()) { return; }

	for (auto f : cur.fs_sel) { f->dead = true; };
	for (auto e : es) 
	if (e.second.in && !e.second.out) { e.first->dead = true; }
	for (auto v : vs)
	if (v.second.in && !v.second.out) { v.first->dead = true; }
	
	auto f = msh<face>(cur); 
	f->cs = cs; g.fs[f->id] = f;
	g.del_dead(); cur.fs_sel = { &*f };
}

struct ds_v {
	int n = 0;
	edge* e = NULL;
	bool out = false;
};
void Tool:: dissolve_edge(Cur& cur) {
	auto& g = **cur.gs_sel.begin();
	map<vert*, ds_v> vs;
	for (auto &vp : g.vs) { vs[&*vp.second] = {}; }

	for (auto& ep : g.es) {
		auto& e = *ep.second;
		bool in = cur.es_sel.find(&e) != cur.es_sel.end();
		if (in) {
			vs[e.v0].n++; vs[e.v0].e = &e;
			vs[e.v1].n++; vs[e.v1].e = &e;
		} else {
			vs[e.v0].out = true;
			vs[e.v1].out = true;
		}
	}

	int n_end = 0;
	corner c0, c1;
	for (auto v : vs_sel) {
		auto& vi = vs[v];
		if (vi.n == 1) {
			if (n_end == 2) { return; }
			if (n_end) { c1 = corner(v, vi.e); }
			else { c0 = corner(v, vi.e); } ++n_end;
		} else if (vi.out || vi.n != 2) { return; }
	} 
	// 如果这个大边本来就存在，会有奇异的情况，不想处理这个。
	for (auto& ep : g.es) {
		auto& e = *ep.second;
		if (e.v0 == c0.v && e.v1 == c1.v) { return; }
		if (e.v1 == c0.v && e.v0 == c1.v) { return; }
	}

	auto e = msh<edge>(cur, c0.v, c1.v);
	g.es[e->id] = e;
	for (auto& vp : vs) {
		auto& v = *vp.first; auto& vi = vp.second;
		if (vi.n == 2) { v.dead = true; }
	}
	for (auto e : cur.es_sel) { e->dead = true; }

	for (auto& fp : g.fs) {
		auto& f = *fp.second;
		bool ok = false;
		int i0 = 0; bool end0 = false;
		rep(i, 0, f.cs.size()) {
			if (f.cs[i] == c0) {
				ok = true; i0 = i; 
				end0 = true; break;
			}
			if (f.cs[i] == c1) {
				ok = true; i0 = i; 
				end0 = false; break;
			}
		}

		if (!ok) { continue; }
		int i1 = (i0 + cur.es_sel.size()) % f.cs.size();
		auto c = corner(end0 ? c0.v : c1.v, &*e);
		vector<corner> cs = { c };
		rep(i, 0, f.cs.size() - cur.es_sel.size()) {
			cs.push_back(f.cs[(i1 + i) % f.cs.size()]);
		} f.cs = cs;
	}

	g.del_dead(); cur.es_sel = { &*e };
}

struct lc_e {
	bool bad = false;
	face* f0 = NULL;
	face* f1 = NULL;
	int i0 = 0, i1 = 0;
	ptr<edge> e0, e1;
	ptr<vert> v;
};
void Tool::loop_cut(Cur& cur) {
	vs_info.clear();
	if (cur.es_sel.size() != 1) { return; }

	auto& g = **cur.gs_sel.begin();
	map<edge*, lc_e> es;
	for (auto& e : g.es) { es[&*e.second] = {}; }
	for (auto& fp : g.fs) {
		auto& f = *fp.second;
		rep(i, 0, f.cs.size()) {
			auto& c = f.cs[i];
			auto& ei = es[c.e];
			bool is_f1 = c.v == c.e->v0;
			if (f.cs.size() == 4) {
				if (ei.f0 && ei.f1) { ei.bad = true; }
				else if (ei.f0) { 
					ei.f1 = &f;  ei.i1 = i;
					if (!is_f1) { ei.bad = true; }
				} else {
					ei.f0 = &f; ei.i0 = i;
					if (is_f1) { swap(c.e->v0, c.e->v1); }
				}
			} else { ei.bad = true; }
		}
	}

	vector<edge*> loop = { *cur.es_sel.begin() };
	auto e_beg = loop.front();
	set<face*> fs;
	while (1) {
		// 我感觉某些判断是否可以移到底部。
		auto& e0 = *loop.back();
		auto& ei0 = es[&e0];
		if (ei0.bad) { return; }
		if (fs.find(ei0.f1) != fs.end()) { return; }
		fs.insert(ei0.f1);

		int i = (ei0.i1 + 2) % 4;
		auto& c = ei0.f1->cs[i];
		auto& e1 = *c.e;
		bool rev = e1.v1 != c.v;
		if (&e1 == e_beg) {
			if (rev) { return; }
			break;
		}
		if (rev) {
			// 这里求了两次，可以优化。
			auto& ei1 = es[&e1];
			swap(e1.v0, e1.v1);
			swap(ei1.f0, ei1.f1);
			swap(ei1.i0, ei1.i1);
		} loop.push_back(&e1);
	}

	for (auto e : loop) {
		e->dead = true;
		auto& ei = es[e];
		mkp(ei.v)(cur, (e->v0->o + e->v1->o) / 2);
		mkp(ei.e0)(cur, e->v0, &*ei.v);
		mkp(ei.e1)(cur, e->v1, &*ei.v);
		g.vs[ei.v->id] = ei.v;
		g.es[ei.e0->id] = ei.e0;
		g.es[ei.e1->id] = ei.e1;
	}

	rep(i, 0, loop.size()) {
		auto& e0 = *loop[i];
		auto& e1 = *loop[(i + 1) % loop.size()];
		auto& ei0 = es[&e0];
		auto& ei1 = es[&e1];
		auto& f = *ei0.f1; f.dead = true;
		auto f0 = msh<face>(cur);
		auto f1 = msh<face>(cur);
		auto e = msh<edge>(cur, &*ei0.v, &*ei1.v);
		int i0 = ei0.i1;
		int i1 = (i0 + 1) % 4;
		int i3 = (i0 + 3) % 4;
		f0->cs.push_back(corner(e0.v0, &*ei0.e0));
		f0->cs.push_back(corner(&*ei0.v, &*e));
		f0->cs.push_back(corner(&*ei1.v, &*ei1.e0));
		f0->cs.push_back(f.cs[i3]);
		f1->cs.push_back(corner(&*ei0.v, &*ei0.e1));
		f1->cs.push_back(f.cs[i1]);
		f1->cs.push_back(corner(e1.v1, &*ei1.e1));
		f1->cs.push_back(corner(&*ei1.v, &*e));
		g.es[e->id] = e;
		g.fs[f0->id] = f0; g.fs[f1->id] = f1;
	} 
	
	for (auto e : loop) {
		auto& ei = es[e];
		vs_info.push_back(v_info(&*ei.v, e->v1->o - e->v0->o));
	} 
	auto len = vs_info.front().u.len();
	// 之前很多设计到除法的操作好像也没有考虑到溢出，以后要注意。
	if (len > 1e-2) {
		for (auto& v : vs_info) { v.u /= len; }
	} else { vs_info.clear(); }

	g.del_dead(); cur.es_sel.clear();
}

// 现在还在考虑没有东西选中，却 dragged 的情况，不知道能不能改进。
void Tool::bevel(Cur& cur) {
	vs_info.clear();
	if (cur.mode == MODE_VERT) {
		for (auto v : vs_sel) { bevel(cur, v); }
		cur.vs_sel.clear();
	}
	if (cur.mode == MODE_EDGE) {
		bevel_seg(cur);
	}
}

struct bv_c {
	vec3 u;
	ptr<vert> v;
	ptr<edge> e;
	face* f = NULL;
	// 不会有两个相同的面很重要，要不然 i 会变。
	int i = 0;
	bool s0 = false, s1 = false;

	corner& in() { return f->cs[mod(i - 1, f->cs.size())]; }
	corner& out() { return f->cs[i]; }
};
struct bv_v {
	int n_vs = 0;
	vector<ptr<bv_c>> cs;

	int prev(int i) {
		while (true) {
			i = mod(i - 1, cs.size());
			auto& c = *cs[i];
			if (c.s0 && c.s1 || !c.s1) { return i; }
			
		}
	}
	int next(int i) {
		while (true) {
			auto& c = *cs[i];
			if (c.s0 && c.s1 || !c.s1) { return i; }
			i = mod(i + 1, cs.size());
		}
	}
};
struct bv_e {
	bool ok = false;
	int i0 = 0, i1 = 0;
	vert* v0 = NULL;
	vert* v1 = NULL;
};

void Tool::bevel(Cur& cur, vert* v) {
	// 如果是点模式的话，记得取消选定。
	vector<ptr<bv_c>> cs;
	auto& g = **cur.gs_sel.begin();
	for (auto& fp : g.fs) {
		auto& f = *fp.second;
		rep(i, 0, f.cs.size()) {
			auto& c = f.cs[i];
			if (c.v == v) {
				auto co = msh<bv_c>();
				co->f = &f; co->i = i;
				cs.push_back(co);
			}
		}
	}

	// 这里不想考虑退化的情况了。
	if (cs.size() <= 2) { return; }
	vector<ptr<bv_c>> tmp = { cs.front() };
	auto& c_beg = tmp.front();
	while (1) {
		auto& c0 = tmp.back();
		auto it = find_if(cs.begin(), cs.end(),
			[&c0](ptr<bv_c> c) { return c->in().e == c0->out().e; });
		if (it == cs.end()) { return; }
		auto& c1 = *it;
		it = find(tmp.begin(), tmp.end(), c1);
		if (it != tmp.end()) {
			if (it != tmp.begin()) { return; }
			if (tmp.size() != cs.size()) { return; }
			break;
		} tmp.push_back(c1);
	} cs = tmp;

	for (auto c : cs) {
		auto& e = *c->out().e;
		auto& v0 = *v;
		auto& v1 = e.v0 == v ? *e.v1 : *e.v0;
		c->u = (v1.o - v0.o).unit();

		mkp(c->v)(cur, v->o);
		g.vs[c->v->id] = c->v;
		c->out().v = &*c->v;
		if (e.v0 == v) { e.v0 = &*c->v; }
		else { e.v1 = &*c->v; }
	}

	rep(i, 0, cs.size()) {
		int h = mod(i - 1, cs.size());
		auto& c = *cs[i];
		mkp(c.e)(cur, &*cs[h]->v, &*c.v);
		g.es[c.e->id] = c.e;
		vector<corner> cos;
		rep(i, 0, c.i) { cos.push_back(c.f->cs[i]); }
		cos.push_back(corner(&*cs[h]->v, &*c.e));
		rep(i, c.i, c.f->cs.size()) { cos.push_back(c.f->cs[i]); }
		c.f->cs = cos;
	}

	auto fo = msh<face>(cur);
	for (int i = cs.size() - 1; i >= 0; --i) {
		auto& c = *cs[i];
		fo->cs.push_back(corner(&*c.v, &*c.e));
	}
	g.fs[fo->id] = fo; 

	for (auto c : cs) { vs_info.push_back(v_info(&*c->v, c->u)); }
	v->dead = true; g.del_dead();
}

void Tool::bevel_seg(Cur& cur) {
	map<vert*, bv_v> vs; map<edge*, bv_e> es;
	auto& g = **cur.gs_sel.begin();
	for (auto v : vs_sel) { vs[v] = {}; }
	for (auto e : cur.es_sel) { es[e] = {}; }
	for (auto& fp : g.fs) {
		auto& f = *fp.second;
		rep(i, 0, f.cs.size()) {
			auto& c = f.cs[i];
			for (auto& vp : vs) {
				auto& v = *vp.first; auto& vi = vp.second;
				if (c.v == &v) {
					auto co = msh<bv_c>();
					co->f = &f; co->i = i;
					vi.cs.push_back(co);
				}
			}
			if (es.find(c.e) != es.end()) { es[c.e].ok = true; }
		}
	}

	for (auto& ep : es) if (!ep.second.ok) { return; }
	
	for (auto& vp : vs) {
		auto& v = *vp.first; auto& vi = vp.second;
		//现在也先不考虑两条边的情况，其实有一种子情况是可以允许的。
		if (vi.cs.size() <= 2) { return; }
		vector<ptr<bv_c>> tmp = { vi.cs.front() };
		auto& c_beg = tmp.front();
		while (1) {
			auto& c0 = tmp.back();
			auto it = find_if(vi.cs.begin(), vi.cs.end(),
				[&c0](ptr<bv_c> c) { return c->in().e == c0->out().e; });
			if (it == vi.cs.end()) { return; }
			auto& c1 = *it;
			it = find(tmp.begin(), tmp.end(), c1);
			if (it != tmp.end()) {
				if (it != tmp.begin()) { return; }
				if (tmp.size() != vi.cs.size()) { return; }
				break;
			} tmp.push_back(c1);
		} vi.cs = tmp;

		for (auto c : vi.cs) {
			int h = mod(c->i - 1, c->f->cs.size());
			auto e0 = c->f->cs[h].e;
			auto e1 = c->f->cs[c->i].e;
			if (es.find(e0) != es.end()) { c->s0 = true; }
			if (es.find(e1) != es.end()) { c->s1 = true; }
		}
	}


	for (auto& ep : es) {
		auto& e = *ep.first; auto& ei = ep.second;
		auto& vi0 = vs[e.v0];
		auto& vi1 = vs[e.v1];
		// 这是因为 e 本身在之后会发生改变。
		ei.v0 = e.v0;
		ei.v1 = e.v1;

		rep(i, 0, vi0.cs.size()) {
			auto& c = *vi0.cs[i];
			if (c.f->cs[c.i].e == &e) { ei.i0 = i; }
		}
		rep(i, 0, vi1.cs.size()) {
			auto& c = *vi1.cs[i];
			if (c.f->cs[c.i].e == &e) { ei.i1 = i; }
		}
	}
	

	for (auto& vp : vs) {
		auto& v = *vp.first; auto& vi = vp.second;
		for (auto& c : vi.cs) {
			auto h = mod(c->i - 1, c->f->cs.size());
			auto& e0 = *c->f->cs[h].e;
			auto& e1 = *c->f->cs[c->i].e;
			vert& v_prev = e0.v0 == &v ? *e0.v1 : *e0.v0;
			vert& v_next = e1.v0 == &v ? *e1.v1 : *e1.v0;

			// 这个偏移量怎么怎么定是个大难点。
			// 我这里只适合方方正正或者一条线的情况。blender 不是这样的。
			if (c->s0 && c->s1) {
				++vi.n_vs;
				mkp(c->v)(cur, v.o);
				g.vs[c->v->id] = c->v;
				auto u = (v_next.o - v.o).unit() + (v.o - v_prev.o).unit();
				u = 2 / u.len() * u.unit();
				c->u = cross(c->f->n, u);
			} 
			if (!c->s1) {
				++vi.n_vs;
				mkp(c->v)(cur, v.o);
				g.vs[c->v->id] = c->v;
				c->u = (v_next.o - v.o).unit();
			}
		}
	}

	for (auto& vp : vs) {
		auto& v = *vp.first; auto& vi = vp.second;
		
		if (vi.n_vs > 2) {
			rep(i, 0, vi.cs.size()) {
				int i0 = vi.prev(i);
				int i1 = vi.next(i);
				auto& v0 = *vi.cs[i0]->v;
				auto& v1 = *vi.cs[i1]->v;
				auto& c = *vi.cs[i];
				if (c.s0 && c.s1 || !c.s1) { 
					// 这里的顺序是约定好的。
					mkp(c.e)(cur, &v1, &v0); 
					g.es[c.e->id] = c.e;
				}
			}
		} else {
			auto i0 = vi.next(0);
			auto i1 = vi.next(i0 + 1);
			auto& v0 = *vi.cs[i0]->v;
			auto& v1 = *vi.cs[i1]->v;
			auto e = msh<edge>(cur, &v0, &v1);
			g.es[e->id] = e;
			for (auto c : vi.cs) 
			if (c->s0 && c->s1 || !c->s1) { c->e = e; }
		}
	}

	for (auto& vp : vs) {
		auto& v = *vp.first; auto& vi = vp.second;
		v.dead = true;
		rep(i, 0, vi.cs.size()) {
			// 感觉这里的很多东西都是没有必要的。
			auto i0 = vi.prev(i);
			auto i1 = vi.next(i);
			auto& c = *vi.cs[i];
			auto& f = *c.f;
			rep(i, 0, f.cs.size()) if (f.cs[i].v == &v) { c.i = i; }
			int h = mod(c.i - 1, f.cs.size());
			
			// 我觉得这里 prev， next 的设置非常不方便。
			auto& vc = *vi.cs[vi.prev(i + 1)]->v; 
			f.cs[c.i].v = &vc;
			auto& e1 = *f.cs[c.i].e;
		
			if (e1.v0 == &v) { e1.v0 = &vc; }
			else { e1.v1 = &vc; }

			if (c.s0) {
				auto eo = msh<edge>(cur, f.cs[h].v, &vc);
				g.es[eo->id] = eo;
				f.cs[h].e = &*eo;
			}
			if (!c.s0 && !c.s1) {
				auto& v0 = *vi.cs[i0]->v;
				vector<corner> cs;
				rep(i, 0, c.i) { cs.push_back(f.cs[i]); }
				cs.push_back(corner(&v0, &*c.e));
				rep(i, c.i, f.cs.size()) { cs.push_back(f.cs[i]); }
				f.cs = cs;
			}
		}
	}

	for (auto& vp : vs) {
		auto& v = *vp.first; auto& vi = vp.second;
		for (auto& c : vi.cs) {
			auto& f = *c->f;
			rep(i, 0, f.cs.size()) if (f.cs[i].v == &*c->v) { c->i = i; }
		}

		if (vi.n_vs <= 2) { continue; }
		auto fo = msh<face>(cur);
		g.fs[fo->id] = fo;
		for (int i = vi.cs.size() - 1; i >= 0; --i) {
			auto& c = vi.cs[i];
			if (c->e) {
				fo->cs.push_back(corner(c->e->v0, &*c->e));
			}
		}
	}
	
	for (auto& ep : es) {
		auto& e = *ep.first; auto& ei = ep.second;
		e.dead = true;
		// 噢噢，原来这条边没有以任何形式留存下来，所以必须删掉。
		auto& vi0 = vs[ei.v0];
		auto& vi1 = vs[ei.v1];
		auto fo = msh<face>(cur);
		g.fs[fo->id] = fo;

		auto& c00 = *vi0.cs[ei.i0];
		auto& c10 = *vi1.cs[ei.i1];
		auto& c01 = *vi0.cs[(ei.i0 + 1) % vi0.cs.size()];
		auto& c11 = *vi1.cs[(ei.i1 + 1) % vi1.cs.size()];
		fo->cs.push_back(corner(&*c11.v, c00.f->cs[c00.i].e));
		fo->cs.push_back(corner(c00.f->cs[c00.i].v, &*c01.e));
		fo->cs.push_back(corner(&*c01.v, c10.f->cs[c10.i].e));
		fo->cs.push_back(corner(c10.f->cs[c10.i].v, &*c11.e));
	}

	for (auto& vp : vs) for (auto c : vp.second.cs) {
		if (c->v) { vs_info.push_back(v_info(&*c->v, c->u)); }
	}

	g.del_dead();
	cur.es_sel.clear();
}

void Tool::update_knife(Cur& cur) {
	if (cur.fs_sel.size() != 1 || 
		knife && (*cur.fs_sel.begin())->id != id_kf) {
		Discard(cur); return;
	}

	if (!knife) {
		knife = true;
		id_kf = (*cur.fs_sel.begin())->id;
	}

	auto& f = **cur.fs_sel.begin();
	if (hovered && msc(0)) {;
		rep(i, 0, f.cs.size()) {
			auto& o = f.cs[i].v->o;
			auto v = cm.mt * (o - cm.p);
			// 这里懒得比较深度了，但是这个做法真的可以推广吗。
			// 推广的话就是要算上所有面了，感觉有很多重复的东西。
			if (v.y >= cm.min_d) {
				auto p = (dvec)proj(v, cm);
				if ((msp - p).lensqr() < 12 * 12) {
					if (started_kf) { end_knife(cur, i); }
					else { i0_kf = i; started_kf = true; }
					return;
				}
			}
		}

		vec3 p;
		p.x = msp.x - cm.ct.x;
		p.y = cm.scl * cm.vp.h;
		p.z = -(msp.y - cm.ct.y);
		p = (cm.mt.tsp() * p).unit();

		rep(i, 1, f.cs.size() - 1) {
			vec3 v0 = f.cs[i].v->o - f.cs[0].v->o;
			vec3 v1 = f.cs[i + 1].v->o - f.cs[0].v->o;
			vec3 v2 = p;
			vec3 p = mat3(v0, v1, v2).inv() * (cm.p - f.cs[0].v->o);
			bool ok =
				p.x >= 0 && p.y >= 0 && p.x + p.y <= 1 && p.z < -cm.min_d;
			if (ok) {
				auto v = 
					f.cs[i].v->o * p.x + 
					f.cs[i + 1].v->o * p.y +  
					f.cs[0].v->o * (1 - p.x - p.y);
				vs_kf.push_back(v); return;
			}
		}
	}
} 

struct kf_e {
	ptr<edge> e;
	vert* v0 = NULL;
	vert* v1 = NULL;
};
void Tool::end_knife(Cur& cur, int i1_kf) {
	if (i1_kf == i0_kf) { Discard(cur); return; }
	auto& g = **cur.gs_sel.begin();
	auto& f = **cur.fs_sel.begin();
	f.dead = true;

	vector<vert*> vs = { f.cs[i0_kf].v };
	for (auto &o : vs_kf) {
		auto v = msh<vert>(cur, o);
		g.vs[v->id] = v;
		vs.push_back(&*v);
	}
	vs.push_back(f.cs[i1_kf].v);
	vector<edge *> es;
	rep(i, 0, vs.size() - 1) {
		auto e = msh<edge>(cur, vs[i], vs[i + 1]);
		g.es[e->id] = e; es.push_back(&*e);
	}

	auto f1 = msh<face>(cur);
	rep(i, 0, es.size()) { 
		f1->cs.push_back(corner(es[i]->v0, es[i])); 
	}
	auto f0 = msh<face>(cur);
	for (int i = es.size() - 1; i >= 0; --i) {
		f0->cs.push_back(corner(es[i]->v1, es[i]));
	}
	for (int i = i0_kf; i != i1_kf; i = (i + 1) % f.cs.size()) {
		f0->cs.push_back(f.cs[i]);
	}
	for (int i = i1_kf; i != i0_kf; i = (i + 1) % f.cs.size()) {
		f1->cs.push_back(f.cs[i]);
	}
	g.fs[f0->id] = f0; g.fs[f1->id] = f1;

	g.del_dead(); cur.fs_sel.clear();
	Discard(cur);
}
