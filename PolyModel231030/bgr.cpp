#include "bgr.h"
#include "cur.h"
#include "draw_str.h"
#include "draw_tile.h"

Bgr::Bgr(Cur& cur) {
	w = 1630; h = 860;
	tl = { 170, 0 }; dep = -100000;
	black = tile(w, h, dcol{}, 255);
}

#include "my_def.h"

void Bgr::render(Cur& cur) {
	draw_tile_raw(scr, tl, scr.rect(), black, black.rect());
	draw_str(scr, dscr, 999, dbstr,
		dcol(255), ft, tl + dvec(10, 10), w - 20, bgr.vp());
}

void Bgr::set_cm(Cur& cur) {
	dvec dmsp = msp - msp_old;
	if (dragged_mid) {
		dragged_mid = msd[1];
		if (dragged_mid) {
			cm.theta += -dmsp.x * 0.005;
			cm.phi += dmsp.y * 0.005;
		}
	} else { dragged_mid = msc(1) && wheeled; }

	if (dragged_right) {
		dragged_right = msd[2];
		if (dragged_right) {
			// 说实话我不知道下面的式子有多大道理.
			cm.look = cm.mt * (cm.look - cm.p);
			cm.look.z += dmsp.y / (cm.scl * h) * cm.look.y;
			cm.look.x -= dmsp.x / (cm.scl * h) * cm.look.y;
			cm.look = cm.mt.tsp() * cm.look + cm.p;
		}
	} else { dragged_right = msc(2) && wheeled; }

	if (wheeled) { cm.scl = exp(log(cm.scl) + msw * 0.1); }
	cm.theta = modf(cm.theta, 2 * PI);
	cm.psi = modf(cm.psi, 2 * PI);
	cm.phi = clmp(cm.phi, -PI / 2, PI / 2); cm.calc();
}
void Bgr::Update(Cur& cur) {
	hovered = (hvd == this);
	wheeled = (whd == this);

	if (hovered && msc(0)) {
		cur.fs_sel.clear();
		cur.es_sel.clear();
		cur.vs_sel.clear();
		if (cur.mode == MODE_GEO) { cur.gs_sel.clear(); }
	}

	set_cm(cur); render(cur);
}
void Bgr::PreUpdate(Cur& cur) {
	bool ok = dhv <= dep && insd(msp, vp());
	if (ok) { dhv = dep; hvd = this; }

	ok = dwh <= dep && insd(msp, vp());
	if (ok) { dwh = dep; whd = this; }
}
