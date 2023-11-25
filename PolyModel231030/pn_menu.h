#pragma once
#include "panel.h"
#include "button.h"
#include "ctrl_list.h"

struct BtMute : Button {
	BtMute();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtMainMenu : BtLan {
	BtMainMenu();
	void OnClick(App& app) override;
};
struct BtMode : BtLan {
	BtMode();
	void OnClick(App& app) override;
};
struct BtModeGeo : BtLan {
	BtModeGeo();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtModeVert : BtLan {
	BtModeVert();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtModeEdge : BtLan {
	BtModeEdge();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtModeFace : BtLan {
	BtModeFace();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtModeCol : BtLan {
	BtModeCol();
	void Update(App& app) override;
	void OnClick(App& app) override;
};

struct BtTool : BtLan {
	BtTool();
	void OnClick(App& app) override;
};
struct BtSelect : BtLan {
	BtSelect();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtMove : BtLan {
	BtMove();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtScale : BtLan {
	BtScale();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtRotate : BtLan {
	BtRotate();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtExtrude : BtLan {
	BtExtrude();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtInset : BtLan {
	BtInset();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtLoopCut : BtLan {
	BtLoopCut();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtBevel : BtLan {
	BtBevel();
	void Update(App& app) override;
	void OnClick(App& app) override;
};
struct BtKnife : BtLan {
	BtKnife();
	void Update(App& app) override;
	void OnClick(App& app) override;
};

struct PnMenu : Panel {
	ptr<BtMute> bt_mute;
	ptr<BtFPn> bt_global;
	ptr<BtMode> bt_mode;
	ptr<BtTool> bt_tool;
	ptr<BtFPn> bt_cmd;
	ptr<BtFPn> bt_par;
	ptr<BtFPn> bt_lang;
	ptr<BtFPn> bt_about;
	ptr<CtrlListY> cly_main;

	ptr<BtMainMenu> bt_main_menu;
	ptr<BtModeGeo> bt_mode_geo;
	ptr<BtModeVert> bt_mode_vert;
	ptr<BtModeEdge> bt_mode_edge;
	ptr<BtModeFace> bt_mode_face;
	ptr<BtModeCol> bt_mode_col;
	ptr<CtrlListY> cly_mode;

	ptr<BtSelect> bt_select;
	ptr<BtMove> bt_move;
	ptr<BtScale> bt_scale;
	ptr<BtRotate> bt_rotate;
	ptr<BtExtrude> bt_extrude;
	ptr<BtInset> bt_inset;
	ptr<BtLoopCut> bt_loop_cut;
	ptr<BtBevel> bt_bevel;
	ptr<BtKnife> bt_knife;
	ptr<CtrlListY> cly_tool;

	ptr<CtrlListY> cly;
	PnMenu(App& app);
};
