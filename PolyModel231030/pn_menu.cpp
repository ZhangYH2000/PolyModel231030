#include "pn_menu.h"
#include "ui.h"
#include "cur.h"
#include "tool.h"
#include "fpn_global.h"
#include "fpn_col.h"
#include "fpn_cmd.h"
#include "fpn_par.h"
#include "fpn_lang.h"
#include "fpn_about.h"

#define cur (*(Cur*)&app)
#include "my_def.h"
#define mkbt(nm) mkp(bt_##nm)(&*ui.fpn_##nm);

BtMute::BtMute() : Button(150) {}
void BtMute::Update(App& app) {
	txt = cur.mute ? loc(L"unmute") : loc(L"mute");
	Button::Update(app);
}
void BtMute::OnClick(App& app) { cur.mute = !cur.mute; }
BtMainMenu::BtMainMenu() : BtLan(150, L"bt_main_menu") {}
void BtMainMenu::OnClick(App& app) {
	ui.pn_menu->set_c(app, &*ui.pn_menu->cly_main);
}

BtMode::BtMode() : BtLan(150, L"bt_mode") {}
void BtMode::OnClick(App& app) {
	ui.pn_menu->set_c(app, &*ui.pn_menu->cly_mode);
}

BtModeGeo::BtModeGeo() : BtLan(150, L"bt_mode_geo") {}
void BtModeGeo::Update(App& app) {
	c_normal = cur.mode == MODE_GEO ? dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtModeGeo::OnClick(App& app) {
	cur.hdl_mode(MODE_GEO);
}

BtModeVert::BtModeVert() : BtLan(150, L"bt_mode_vert") {}
void BtModeVert::Update(App& app) {
	c_normal = cur.mode == MODE_VERT ? dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtModeVert::OnClick(App& app) {
	cur.hdl_mode(MODE_VERT);
}

BtModeEdge::BtModeEdge() : BtLan(150, L"bt_mode_edge") {}
void BtModeEdge::Update(App& app) {
	c_normal = cur.mode == MODE_EDGE ? dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtModeEdge::OnClick(App& app) {
	cur.hdl_mode(MODE_EDGE);
}

BtModeFace::BtModeFace() : BtLan(150, L"bt_mode_face") {}
void BtModeFace::Update(App& app) {
	c_normal = cur.mode == MODE_FACE ? dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtModeFace::OnClick(App& app) {
	cur.hdl_mode(MODE_FACE);
}

BtModeCol::BtModeCol() : BtLan(150, L"bt_mode_col") {}
void BtModeCol::Update(App& app) {
	c_normal = cur.mode == MODE_COL ? dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtModeCol::OnClick(App& app) {
	cur.hdl_mode(MODE_COL);
}

BtTool::BtTool() : BtLan(150, L"bt_tool") {}
void BtTool::OnClick(App& app) {
	ui.pn_menu->set_c(app, &*ui.pn_menu->cly_tool);
}

BtSelect::BtSelect() : BtLan(150, L"bt_select") {}
void BtSelect::Update(App& app) {
	c_normal = 
		cur.tool->mode == TOOL_SELECT ? 
		dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtSelect::OnClick(App& app) {
	cur.tool->mode = TOOL_SELECT;
	cur.tool->Discard(cur);
}

BtMove::BtMove() : BtLan(150, L"bt_move") {}
void BtMove::Update(App& app) {
	c_normal =
		cur.tool->mode == TOOL_MOVE ?
		dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtMove::OnClick(App& app) {
	cur.tool->mode = TOOL_MOVE;
	cur.tool->Discard(cur);
}

BtScale::BtScale() : BtLan(150, L"bt_scale") {}
void BtScale::Update(App& app) {
	c_normal =
		cur.tool->mode == TOOL_SCALE ?
		dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtScale::OnClick(App& app) {
	cur.tool->mode = TOOL_SCALE;
	cur.tool->Discard(cur);
}

BtRotate::BtRotate() : BtLan(150, L"bt_rotate") {}
void BtRotate::Update(App& app) {
	c_normal =
		cur.tool->mode == TOOL_ROTATE ?
		dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtRotate::OnClick(App& app) {
	cur.tool->mode = TOOL_ROTATE;
	cur.tool->Discard(cur);
}

BtExtrude::BtExtrude() : BtLan(150, L"bt_extrude") {}
void BtExtrude::Update(App& app) {
	c_normal =
		cur.tool->mode == TOOL_EXTRUDE ?
		dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtExtrude::OnClick(App& app) {
	cur.tool->mode = TOOL_EXTRUDE;
	cur.tool->Discard(cur);
}

BtInset::BtInset() : BtLan(150, L"bt_inset") {}
void BtInset::Update(App& app) {
	c_normal =
		cur.tool->mode == TOOL_INSET ?
		dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtInset::OnClick(App& app) {
	cur.tool->mode = TOOL_INSET;
	cur.tool->Discard(cur);
}

BtLoopCut::BtLoopCut() : BtLan(150, L"bt_loop_cut") {}
void BtLoopCut::Update(App& app) {
	c_normal =
		cur.tool->mode == TOOL_LOOP_CUT ?
		dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtLoopCut::OnClick(App& app) {
	cur.tool->mode = TOOL_LOOP_CUT;
	cur.tool->Discard(cur);
}

BtBevel::BtBevel() : BtLan(150, L"bt_bevel") {}
void BtBevel::Update(App& app) {
	c_normal =
		cur.tool->mode == TOOL_BEVEL ?
		dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtBevel::OnClick(App& app) {
	cur.tool->mode = TOOL_BEVEL;
	cur.tool->Discard(cur);
}

BtKnife::BtKnife() : BtLan(150, L"bt_knife") {}
void BtKnife::Update(App& app) {
	c_normal =
		cur.tool->mode == TOOL_KNIFE ?
		dcol(60, 150, 60) : dcol(80);
	BtLan::Update(app);
}
void BtKnife::OnClick(App& app) {
	cur.tool->mode = TOOL_KNIFE;
	cur.tool->Discard(cur);
}

PnMenu::PnMenu(App& app) : Panel(Panel::menu(app)) {
	vector<Control*> tmp;
	mkp(bt_mute)();
	mkbt(global);
	mkp(bt_mode)(); mkp(bt_tool)();
	mkbt(cmd); mkbt(par);
	mkbt(lang); mkbt(about);
	tmp = { 
		&*bt_mute,
		&*bt_global, &*bt_mode, &*bt_tool,
		&*bt_cmd, &*bt_par, &*bt_lang, &*bt_about 
	};
	mkcl(cly_main);
	mkp(bt_main_menu)(); 
	mkp(bt_mode_geo)();
	mkp(bt_mode_vert)();
	mkp(bt_mode_edge)();
	mkp(bt_mode_face)();
	mkp(bt_mode_col)();
	tmp = { &*bt_main_menu, &*bt_mode_geo, 
		&*bt_mode_vert, &*bt_mode_edge, &*bt_mode_face, &*bt_mode_col };
	mkcl(cly_mode);
	mkp(bt_select)(); mkp(bt_move)();
	mkp(bt_scale)(); mkp(bt_rotate)();
	mkp(bt_extrude)(); mkp(bt_inset)();
	mkp(bt_loop_cut)(); mkp(bt_bevel)(); mkp(bt_knife)();
	tmp = { &*bt_main_menu, &*bt_select, 
		&*bt_move, &*bt_scale, &*bt_rotate, 
		&*bt_extrude, &*bt_inset, &*bt_loop_cut, &*bt_bevel, &*bt_knife };
	mkcl(cly_tool);
	c = &*cly_main; Init(app);
}
