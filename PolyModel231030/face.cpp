#include "face.h"
#include "cur.h"
#include "geo.h"
#include "draw_3d.h"

#include "my_def.h"

void corner::save(FILE* f) {
	fwt(v->id); fwt(e->id);
}
corner::corner(Cur& cur, geo& g, FILE* f) {
	// 其实 cur 在这里都没有什么用的，可以删掉的。
	int tmp = 0;
	frd(tmp); v = &*g.vs[tmp];
	frd(tmp); e = &*g.es[tmp];
}

face::face(Cur& cur) { set_id; c = 1; }
void face::save(FILE* f) {
	int sz = 0;
	fwt(id); fwt(c);
	sz = cs.size(); fwt(sz);
	for (auto& c : cs) { c.save(f); }
}
face::face(Cur& cur, geo& g, FILE* f) {
	int sz = 0;
	frd(id); frd(c);
	frd(sz);
	rep(i, 0, sz) {
		cs.push_back(corner(cur, g, f));
	}
}

void face::Render(Cur& cur, geo& g) {
	if (cur.render_smooth) {
		vec3 light = cm.mt.tsp() * vec3(0, 1, 0) * 0.9;
		double env = 0.1;
		rep(i, 1, cs.size() - 1) {
			col3 c0 = c * (max(0.0, dot(-cs[0].v->n, light)) + 0.1);
			col3 c1 = c * (max(0.0, dot(-cs[i].v->n, light)) + 0.1);
			col3 c2 = c * (max(0.0, dot(-cs[i + 1].v->n, light)) + 0.1);
			draw_3d_tri(scr, dscr, cs[0].v->o,
				cs[i].v->o, cs[i + 1].v->o, cm, c0, c1, c2);
		} return;
	}

	vec3 light = cm.mt.tsp() * vec3(0, 1, 0) * 0.4;
	col3 col = c * (max(0.0, dot(-n, light)) + 0.2);
	if (selected) { col = col3(1, 0.55, 0.28); }
	// if (cur.mode == MODE_COL) { col *= c; }

	rep(i, 1, cs.size() - 1) {
		// 有时间可以把这里改一改，不要把光源信息放里面了。
		// 不知道对性能影响大不大。
		draw_3d_tri(scr, dscr, cs[0].v->o,
			cs[i].v->o, cs[i + 1].v->o, cm, col, {}, 1);
	}
}

void face::Update(Cur& cur, geo& g) {
	hovered = (hvd == this);
	selected = cur.fs_sel.find(this) != cur.fs_sel.end();
	if (hovered && msc(0)) {
		bool valid =
			cur.mode == MODE_FACE && g.selected;
		if (valid) {
			if (kbd[VK_CONTROL]) { cur.fs_sel.insert(this); } 
			else { cur.fs_sel = { this }; }
		}

		if (cur.mode == MODE_GEO) {
			if (kbd[VK_CONTROL]) { cur.gs_sel.insert(&g); }
			else { cur.gs_sel = { &g }; }
		}

		if (cur.mode == MODE_COL) {
			cur.paint = true;
		}
	}

	if (hovered && cur.paint == true) {
		c = cur.color;
	}
}
void face::PreUpdate(Cur& cur, geo& g) {
	double dep = -DBL_MAX;
	bool hit = false; vec3 p;
	p.x = msp.x - cm.ct.x;
	p.y = cm.scl * cm.vp.h;
	p.z = -(msp.y - cm.ct.y);
	p = (cm.mt.tsp() * p).unit();

	rep(i, 1, cs.size() - 1) {
		vec3 v0 = cs[i].v->o - cs[0].v->o;
		vec3 v1 = cs[i + 1].v->o - cs[0].v->o;
		vec3 v2 = p;
		vec3 p = mat3(v0, v1, v2).inv() * (cm.p - cs[0].v->o);
		bool ok =
			p.x >= 0 && p.y >= 0 && p.x + p.y <= 1 &&
			p.z < -cm.min_d && p.z > dep;
		if (ok) { dep = p.z; hit = true; }
	}

	bool ok = dhv <= dep && hit && insd(msp, cm.vp);
	if (ok) { dhv = dep; hvd = this; }
}
