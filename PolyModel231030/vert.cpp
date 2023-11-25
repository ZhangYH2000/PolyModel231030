#include "vert.h"
#include "cur.h"
#include "geo.h"
#include "draw_3d.h"
#include "draw_geo.h"

#include "my_def.h"

double constexpr eps_vert = 1e-1;

vert::vert(Cur& cur, vec3 o) : o(o) { set_id; }
void vert::save(FILE* f) {
	fwt(id); fwt(o);
}
vert::vert(Cur& cur, geo& g, FILE* f) {
	frd(id); frd(o); 
	// 这里还需要 geo 绑定自己。
}

void vert::Render(Cur& cur, geo& g) {
	if (!g.selected) { return; }
	if (cur.mode == MODE_GEO) { return; }
	if (cur.mode == MODE_COL) { return; }
	if (cur.render_smooth) { return; }
	
	auto v = cm.mt * (o - cm.p);
	auto p = proj(v, cm);
	dcol c = selected ? dcol(255, 120, 40) : dcol(255);
	draw_eclipse(scr, dscr, -v.y + eps_vert, p, 4, 4, cm.vp, c);
}

void vert::Update(Cur& cur, geo& g) {
	hovered = (hvd == this);
	selected = cur.vs_sel.find(this) != cur.vs_sel.end();
	if (hovered && msc(0)) {
		bool valid =
			cur.mode == MODE_VERT && g.selected;
		if (valid) {
			if (kbd[VK_CONTROL]) { cur.vs_sel.insert(this); }
			else { cur.vs_sel = { this }; }
		}
	}
}
void vert::PreUpdate(Cur& cur, geo& g) {
	bool valid =
		cur.mode == MODE_VERT && g.selected;
	if (!valid) { return; }

	auto v = cm.mt * (o - cm.p);
	if (v.y < cm.min_d) { return; }
	auto p = (dvec)proj(v, cm);
	double dep = -v.y + eps_vert;

	bool ok = dhv <= dep &&
		(msp - p).lensqr() < 12 * 12 && insd(msp, cm.vp);
	if (ok) { dhv = dep; hvd = this; }
}
