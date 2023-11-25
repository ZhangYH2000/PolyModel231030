#pragma once
#include "tile.h"

void sub_draw_tri(tile& dest, dbuf& ds,
	dvec pa, dvec pb, dvec pc, double d, dcol col, bool same_y);
void draw_tri(tile& dest, dbuf& ds, drect const& vp,
	dvec pa, dvec pb, dvec pc, double d, dcol col);
void draw_tri(tile& dest, dbuf& ds, drect const& vp,
	vec2 pa, vec2 pb, vec2 pc, double d, dcol col);

// 这里的 z 指的其实是 y。符号真的很乱。
// 透视校正插值要学清楚, 这里签名也要改的一致些。
void sub_draw_tri(tile& dest, dbuf& ds, dvec pa, dvec pb, dvec pc,
	double za, double zb, double zc, dcol col, bool same_y);
void draw_tri(tile& dest, dbuf& ds, dvec pa, dvec pb, dvec pc,
	double za, double zb, double zc, cam const& cm, dcol col);

void sub_draw_tri(tile& dest, dbuf& ds,
	dvec pa, dvec pb, dvec pc, col3 ca, col3 cb, col3 cc,
	double za, double zb, double zc, bool same_y);
void draw_tri(tile& dest, dbuf& ds, 
	dvec pa, dvec pb, dvec pc, col3 cola, col3 colb, col3 colc, 
	double za, double zb, double zc, cam const& cm);
