#include "edge.h"
#include "cur.h"
#include "geo.h"
#include "draw_3d.h"
#include "draw_curve.h"
#include "draw_px_seg.h"

#include "my_def.h"

edge::edge(Cur& cur, vert* v0, vert* v1) : v0(v0), v1(v1) { set_id; }
void edge::save(FILE* f) {
	fwt(id); fwt(v0->id); fwt(v1->id);
}
edge::edge(Cur& cur, geo& g, FILE* f) {
	frd(id); int tmp = 0;
	frd(tmp); v0 = &*g.vs[tmp];
	frd(tmp); v1 = &*g.vs[tmp];
}

void edge::Render(Cur& cur, geo& g) {
	if (!g.selected) { return; }
	if (cur.render_smooth) { return; }

	dcol c = selected || cur.mode == MODE_GEO ?
		dcol(255, 120, 40) : dcol(255);
	if (cur.mode == MODE_COL) { c = {}; }
	draw_3d_seg(scr, dscr, v0->o, v1->o, cm, c);
}

void edge::Update(Cur& cur, geo& g) {
	hovered = (hvd == this);
	selected = cur.es_sel.find(this) != cur.es_sel.end();
	if (hovered && msc(0)) {
		bool valid =
			cur.mode == MODE_EDGE && g.selected;
		if (valid) {
			if (kbd[VK_CONTROL]) { cur.es_sel.insert(this); }
			else { cur.es_sel = { this }; }
		}
	}
}
void edge::PreUpdate(Cur& cur, geo& g) {
	bool valid =
		cur.mode == MODE_EDGE && g.selected;
	if (!valid) { return; }

	auto q0 = cm.mt * (v0->o - cm.p);
	if (q0.y < cm.min_d) { return; }
	auto p0 = proj(q0, cm);
	auto q1 = cm.mt * (v1->o - cm.p);
	if (q1.y < cm.min_d) { return; }
	auto p1 = proj(q1, cm);

	vec2 p = (p1 - p0).unit();
	double len = (p1 - p0).len();
	vec2 ms = ((vec2)msp - p0);
	double para = dot(ms, p);
	vec2 bias = ms - para * p;
	if (bias.lensqr() > 12 * 12 || !insd(para, 0.0, len)) { return; }

	double dep = eps_edge - len / (para / q1.y + (len - para) / q0.y);
	bool ok = dhv <= dep && insd(msp, cm.vp);
	if (ok) { dhv = dep; hvd = this; }
}
