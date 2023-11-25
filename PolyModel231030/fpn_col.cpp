#include "fpn_col.h"
#include "ui.h"
#include "cur.h"
#include "draw_geo.h"

#define cur (*(Cur*)&app)
#include "my_def.h"

DbXRCol::DbXRCol() : DbXLan(L"dbx_r_col", 200) {};
void DbXRCol::Sync(App& app) {
	set_nx_rel(cur.color.r());
}
void DbXRCol::Upload(App& app) const {
	cur.color.r() = uval();
}
TbRCol::TbRCol() : Textbox(50) {};
void TbRCol::Sync(App& app) {
	str = tw2(cur.color.r());
}
void TbRCol::OnDone(App& app) const {
	cur.color.r() = clmp(wtof(str), 0.0, 1.0);
}

DbXGCol::DbXGCol() : DbXLan(L"dbx_g_col", 200) {};
void DbXGCol::Sync(App& app) {
	set_nx_rel(cur.color.g());
}
void DbXGCol::Upload(App& app) const {
	cur.color.g() = uval();
}
TbGCol::TbGCol() : Textbox(50) {};
void TbGCol::Sync(App& app) {
	str = tw2(cur.color.g());
}
void TbGCol::OnDone(App& app) const {
	cur.color.g() = clmp(wtof(str), 0.0, 1.0);
}

DbXBCol::DbXBCol() : DbXLan(L"dbx_b_col", 200) {};
void DbXBCol::Sync(App& app) {
	set_nx_rel(cur.color.b());
}
void DbXBCol::Upload(App& app) const {
	cur.color.b() = uval();
}
TbBCol::TbBCol() : Textbox(50) {};
void TbBCol::Sync(App& app) {
	str = tw2(cur.color.b());
}
void TbBCol::OnDone(App& app) const {
	cur.color.b() = clmp(wtof(str), 0.0, 1.0);
}

OtCol::OtCol() {
	gap = { 10, 10 }; dep = 1000;
	w = 150; h = 30; s_chess = 15;
}
int OtCol::GetW() const { return w + gap.x; }
int OtCol::GetH() const { return h + gap.y; }
void OtCol::render(App& app) {
	draw_rect_raw(scr, tl, w, h, vp, (dcol)cur.color);
}
void OtCol::Update(App& app) {
	render(app);
}


FPnCol::FPnCol(App& app) : FPnLan(app, 400, 600, L"fpn_col") {
	vector<Control*> tmp;
	mkp(dbx_r)(); mkp(tb_r)();
	tmp = { &*dbx_r, &*tb_r };
	mkcl(clx_r);
	mkp(dbx_g)(); mkp(tb_g)();
	tmp = { &*dbx_g, &*tb_g };
	mkcl(clx_g);
	mkp(dbx_b)(); mkp(tb_b)();
	tmp = { &*dbx_b, &*tb_b };
	mkcl(clx_b);
	mkp(ot)();
	tmp = { &*clx_r, &*clx_g, &*clx_b, &*ot };
	mkcl(cly); c = &*cly; Init(app);
}
