#pragma once
#include "tile.h"

// 我不知道还有那些地方要用到这个，暂且保留为外部函数。
inline vec2 proj(vec3 p, cam const& cm) {
	// 这里的 p 是与 mt 相乘之后的。
	vec2 tmp = vec2(p.x, -p.z) / p.y * cm.scl * cm.vp.h;
	return (vec2)cm.ct + tmp;
}

void draw_3d_tri(tile& dest, dbuf& ds, vec3 pa, vec3 pb, vec3 pc,
	cam const& cm, col3 col, vec3 v_light = {}, double env_light = 1);
void draw_3d_tri(tile& dest, dbuf& ds, vec3 pa, vec3 pb, vec3 pc,
	cam const& cm, col3 cola, col3 colb, col3 colc);
void draw_3d_seg(tile& dest, dbuf& ds, vec3 pa, vec3 pb, cam const& cm, dcol c);
