#pragma once
#include "label.h"
#include "button.h"
#include "textbox.h"
#include "dragbar.h"
#include "ctrl_list.h"
#include "float_panel.h"

struct DbXRCol : DbXLan {
	DbXRCol();
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct TbRCol : Textbox {
	TbRCol();
	void Sync(App& app) override;
	void OnDone(App& app) const override;
};

struct DbXGCol : DbXLan {
	DbXGCol();
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct TbGCol : Textbox {
	TbGCol();
	void Sync(App& app) override;
	void OnDone(App& app) const override;
};

struct DbXBCol : DbXLan {
	DbXBCol();
	void Sync(App& app) override;
	void Upload(App& app) const override;
};
struct TbBCol : Textbox {
	TbBCol();
	void Sync(App& app) override;
	void OnDone(App& app) const override;
};

struct OtCol : Control {
	dvec gap;
	double dep = 0;
	int w = 0, h = 0;

	tile chess;
	int s_chess = 0;
	dcol c_chess_a, c_chess_b;

	OtCol();
	int GetW() const override;
	int GetH() const override;
	void render(App& app);
	void Update(App& app) override;
};

struct FPnCol : FPnLan {
	ptr<DbXRCol> dbx_r;
	ptr<TbRCol> tb_r;
	ptr<CtrlListX> clx_r;
	ptr<DbXGCol> dbx_g;
	ptr<TbGCol> tb_g;
	ptr<CtrlListX> clx_g;
	ptr<DbXBCol> dbx_b;
	ptr<TbBCol> tb_b;
	ptr<CtrlListX> clx_b;
	ptr<OtCol> ot;

	ptr<CtrlListY> cly;
	FPnCol(App& app);
};
