#include "draw_3d.h"
#include "draw_tri.h"
#include "draw_px_seg.h"

#define TMP(a, c, sgn, max_d)\
	if (p##c.y sgn cm.max_d) { return; }\
	if (p##a.y sgn cm.max_d) {\
		vec3 p0 = p##c; p0.y = cm.max_d;\
		p0.x += (p##a.x - p##c.x) * (cm.max_d - p##c.y) / (p##a.y - p##c.y);\
		p0.z += (p##a.z - p##c.z) * (cm.max_d - p##c.y) / (p##a.y - p##c.y);\
		if (pb.y sgn cm.max_d) {\
			vec3 p1 = p##c; p1.y = cm.max_d;\
			p1.x += (pb .x - p##c.x) * (cm.max_d - p##c.y) / (pb.y - p##c.y);\
			p1.z += (pb .z - p##c.z) * (cm.max_d - p##c.y) / (pb.y - p##c.y);\
			sub_draw_3d_tri(dest, ds, p##c, p0, p1, cm, col); return;\
		}\
		else {\
			vec3 p1 = pb; p1.y = cm.max_d;\
			p1.x += (p##a.x - pb.x) * (cm.max_d - pb.y) / (p##a.y - pb.y);\
			p1.z += (p##a.z - pb.z) * (cm.max_d - pb.y) / (p##a.y - pb.y);\
			sub_draw_3d_tri(dest, ds, p##c, pb, p0, cm, col);\
			sub_draw_3d_tri(dest, ds, pb, p0, p1, cm, col); return;\
		}\
	}

void sub_draw_3d_tri(tile & dest, dbuf & ds,
	vec3 pa, vec3 pb, vec3 pc, cam const& cm, dcol col) {
	// 这里的严格不等号很重要，否则会死循环。
	// 这里有点低效，重复比较了，但是总的影响不大。
	TMP(a, c, > , max_d);
	TMP(c, a, < , min_d);

#undef TMP
	dvec p0 = (dvec)proj(pa, cm);
	dvec p1 = (dvec)proj(pb, cm);
	dvec p2 = (dvec)proj(pc, cm);
	draw_tri(dest, ds, p0, p1, p2, pa.y, pb.y, pc.y, cm, col);
}
void draw_3d_tri(tile& dest, dbuf& ds, vec3 pa, vec3 pb, vec3 pc,
	cam const& cm, col3 col, vec3 v_light, double env_light) {
	auto n = cross((pb - pa), (pc - pa));
	if (dot(cm.p - pa, n) <= 0) { return; }

	pa = cm.mt * (pa - cm.p);
	pb = cm.mt * (pb - cm.p);
	pc = cm.mt * (pc - cm.p);
	double light = max(0.0, -dot(n.unit(), v_light));
	light = min(1.0, light + env_light);
	dcol c = dcol(col * light);

	if (pa.y < pb.y) { swap(pa, pb); }
	if (pa.y < pc.y) { swap(pa, pc); }
	if (pb.y < pc.y) { swap(pb, pc); }
	sub_draw_3d_tri(dest, ds, pa, pb, pc, cm, c);
}


#define TMP(a, c, sgn, max_d)\
	if (p##c.y sgn cm.max_d) { return; }\
	if (p##a.y sgn cm.max_d) {\
		vec3 p0 = p##c; p0.y = cm.max_d; col3 col0 = col##c;\
		p0.x += (p##a.x - p##c.x) * (cm.max_d - p##c.y) / (p##a.y - p##c.y);\
		p0.z += (p##a.z - p##c.z) * (cm.max_d - p##c.y) / (p##a.y - p##c.y);\
		col0 += (col##a - col##c) * (cm.max_d - p##c.y) / (p##a.y - p##c.y);\
		if (pb.y sgn cm.max_d) {\
			vec3 p1 = p##c; p1.y = cm.max_d; col3 col1 = col##c;\
			p1.x += (pb.x - p##c.x) * (cm.max_d - p##c.y) / (pb.y - p##c.y);\
			p1.z += (pb.z - p##c.z) * (cm.max_d - p##c.y) / (pb.y - p##c.y);\
			col1 += (colb - col##c) * (cm.max_d - p##c.y) / (pb.y - p##c.y);\
			sub_draw_3d_tri(dest, ds, p##c, p0, p1, cm, col##c, col0, col1); return;\
		}\
		else {\
			vec3 p1 = pb; p1.y = cm.max_d; col3 col1 = colb;\
			p1.x += (p##a.x - pb.x) * (cm.max_d - pb.y) / (p##a.y - pb.y);\
			p1.z += (p##a.z - pb.z) * (cm.max_d - pb.y) / (p##a.y - pb.y);\
			col1 += (col##a - colb) * (cm.max_d - pb.y) / (p##a.y - pb.y);\
			sub_draw_3d_tri(dest, ds, p##c, pb, p0, cm, col##c, colb, col0);\
			sub_draw_3d_tri(dest, ds, pb, p0, p1, cm, colb, col0, col1); return;\
		}\
	}

void sub_draw_3d_tri(tile& dest, dbuf& ds, vec3 pa, vec3 pb, vec3 pc, 
	cam const& cm, col3 cola, col3 colb, col3 colc) {
	// 这里的严格不等号很重要，否则会死循环。
	// 这里有点低效，重复比较了，但是总的影响不大。
	TMP(a, c, > , max_d);
	TMP(c, a, < , min_d);

#undef TMP
	dvec p0 = (dvec)proj(pa, cm);
	dvec p1 = (dvec)proj(pb, cm);
	dvec p2 = (dvec)proj(pc, cm);
	draw_tri(dest, ds, p0, p1, p2, cola, colb, colc, pa.y, pb.y, pc.y, cm);
}
void draw_3d_tri(tile& dest, dbuf& ds, vec3 pa, vec3 pb, vec3 pc,
	cam const& cm, col3 cola, col3 colb, col3 colc) {
	auto n = cross((pb - pa), (pc - pa));
	if (dot(cm.p - pa, n) <= 0) { return; }

	pa = cm.mt * (pa - cm.p);
	pb = cm.mt * (pb - cm.p);
	pc = cm.mt * (pc - cm.p);

	if (pa.y < pb.y) { swap(pa, pb); swap(cola, colb); }
	if (pa.y < pc.y) { swap(pa, pc); swap(cola, colc); }
	if (pb.y < pc.y) { swap(pb, pc); swap(colb, colc); }
	sub_draw_3d_tri(dest, ds, pa, pb, pc, cm, cola, colb, colc);
}

void draw_3d_seg(tile& dest, dbuf& ds, vec3 pa, vec3 pb, cam const& cm, dcol c) {
	pa = cm.mt * (pa - cm.p);
	pb = cm.mt * (pb - cm.p);
	if (pa.y > pb.y) { swap(pa, pb); }

#define TMP(a, b, sgn, min_d)\
	if (p##b.y sgn cm.min_d) { return; }\
	if (p##a.y sgn cm.min_d) {\
		p##a.x += (p##b.x - p##a.x) * (cm.min_d - p##a.y) / (p##b.y - p##a.y);\
		p##a.z += (p##b.z - p##a.z) * (cm.min_d - p##a.y) / (p##b.y - p##a.y);\
		p##a.y = cm.min_d;\
	}

	TMP(a, b, < , min_d);
	TMP(b, a, > , max_d);

#undef TMP
	dvec p0 = (dvec)proj(pa, cm);
	dvec p1 = (dvec)proj(pb, cm);
	draw_px_seg(dest, ds, p0, p1, pa.y, pb.y, cm, c);
}
