#pragma once
#include "tile.h"

void sub_draw_tri(tile& dest, dbuf& ds,
	dvec pa, dvec pb, dvec pc, double d, dcol col, bool same_y);
void draw_tri(tile& dest, dbuf& ds, drect const& vp,
	dvec pa, dvec pb, dvec pc, double d, dcol col);
void draw_tri(tile& dest, dbuf& ds, drect const& vp,
	vec2 pa, vec2 pb, vec2 pc, double d, dcol col);

// ����� z ָ����ʵ�� y��������ĺ��ҡ�
// ͸��У����ֵҪѧ���, ����ǩ��ҲҪ�ĵ�һ��Щ��
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
