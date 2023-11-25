#pragma once
#include "vec3.h"
#include "tile.h"

double constexpr eps_edge = 4e-2;

bool pre_draw_px_seg(dvec& pa, dvec& pb, drect vp);
void draw_px_seg(tile& dest, dbuf& ds, 
	dvec pa, dvec pb, double d, drect vp, dcol c);
void draw_px_seg(tile& dest, dbuf& ds, 
	vec2 pa, vec2 pb, double d, drect vp, dcol c);

bool pre_draw_px_seg
(dvec& pa, dvec& pb, double& za, double& zb, cam const& cm);
void draw_px_seg(tile& dest, dbuf& ds,
	dvec pa, dvec pb, double za, double zb, cam const& cm, dcol c);
