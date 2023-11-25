#include "draw_tri.h"

void sub_draw_tri(tile& dest, dbuf& ds,
	dvec pa, dvec pb, dvec pc, double d, dcol col, bool same_y) {
	// 假设 pa, pb, pc  已经交换过了。
	// 用它画斜的长方形有的时候会出现裂痕，不知道能不能修复。
	if (pa.y == pb.y) { return; }
	int sy = pb.y > pa.y ? 1 : -1;
	int sx = pc.x > pb.x ? 1 : -1;
	for (int y = pa.y; y != pb.y + sy; y += sy) {
		int xb = pa.x * (pb.y - y) + pb.x * (y - pa.y); xb /= (pb.y - pa.y);
		int xc = pa.x * (pb.y - y) + pc.x * (y - pa.y); xc /= (pb.y - pa.y);
		for (int x = xb; x != xc + sx; x += sx) {
			int dp = same_y ?
				y * dest.w + x : x * dest.w + y;
			if (ds[dp] <= d) { ds[dp] = d; dest.cols[dp] = col; }
		}
	}
}
void draw_tri(tile& dest, dbuf& ds, drect const& vp,
	dvec pa, dvec pb, dvec pc, double d, dcol col) {
	// 如果可以让这里写得好看有条理，容易修改且不容易出 bug 就好了。

#define TMP(a, c, x, y, sgn, bd)\
if (p##c.y sgn bd) { return; }\
if (p##a.y sgn bd) {\
	dvec p0 = p##c; p0.y = bd;\
	p0.x += (p##a.x - p##c.x) * (bd - p##c.y) / (p##a.y - p##c.y);\
	if (pb.y sgn bd) {\
		dvec p1 = p##c; p1.y = bd;\
		p1.x += (pb.x - p##c.x) * (bd - p##c.y) / (p##b.y - p##c.y);\
		draw_tri(dest, ds, vp, p0, p1, p##c, d, col); return;\
	}\
	else {\
		dvec p1 = pb; p1.y = bd;\
		p1.x += (p##a.x - pb.x) * (bd - pb.y) / (p##a.y - pb.y);\
		draw_tri(dest, ds, vp, p0, pb, p##c, d, col);\
		draw_tri(dest, ds, vp, p0, pb, p1, d, col); return;\
	}\
}

	// 这里有点低效，重复比较了，但是总的影响不大。
	long long x0 = vp.left();
	long long x1 = vp.right() - 1;
	long long y0 = vp.top();
	long long y1 = vp.bottom() - 1;
	// 不要直接把表达式放在 TMP 里面。

	if (pa.x < pb.x) { swap(pa, pb); }
	if (pa.x < pc.x) { swap(pa, pc); }
	if (pb.x < pc.x) { swap(pb, pc); }
	if (pa.x == pc.x) { return; }
	int dx = pa.x - pc.x;
	TMP(a, c, y, x, > , x1);
	TMP(c, a, y, x, < , x0);

	if (pa.y < pb.y) { swap(pa, pb); }
	if (pa.y < pc.y) { swap(pa, pc); }
	if (pb.y < pc.y) { swap(pb, pc); }
	if (pa.y == pc.y) { return; }
	int dy = pa.y - pc.y;
	TMP(a, c, x, y, > , y1);
	TMP(c, a, x, y, < , y0);

#undef TMP
	// 这里是出于数值稳定性的考虑交换一下方向，否则如果三角形很扁的话会奇怪。
	bool y_big = dy > dx;
	if (!y_big) {
		if (pa.x < pb.x) { swap(pa, pb); }
		if (pa.x < pc.x) { swap(pa, pc); }
		if (pb.x < pc.x) { swap(pb, pc); }
		swap(pa.x, pa.y); swap(pb.x, pb.y); swap(pc.x, pc.y);
	}
	dvec pd = pb;
	pd.x = pa.x * (pb.y - pc.y) + pc.x * (pa.y - pb.y); pd.x /= pa.y - pc.y;
	sub_draw_tri(dest, ds, pa, pb, pd, d, col, y_big);
	sub_draw_tri(dest, ds, pc, pb, pd, d, col, y_big);
}
void draw_tri(tile& dest, dbuf& ds, drect const& vp,
	vec2 pa, vec2 pb, vec2 pc, double d, dcol col) {
	draw_tri(dest, ds, vp, (dvec)pa, (dvec)pb, (dvec)pc, d, col);
}

#define TMP(a, c, x, y, sgn, bd)\
if (p##c.y sgn bd) { return; }\
if (p##a.y sgn bd) {\
	dvec p0 = p##c; p0.y = bd; double z0;\
	p0.x += (p##a.x - p##c.x) * (bd - p##c.y) / (p##a.y - p##c.y);\
	z0 = (p##a.y - p##c.y) / ((bd - p##c.y) / z##a + (p##a.y - bd) / z##c);\
	if (pb.y sgn bd) {\
		dvec p1 = p##c; p1.y = bd; double z1;\
		p1.x += (pb.x - p##c.x) * (bd - p##c.y) / (pb.y - p##c.y);\
		z1 = (pb.y - p##c.y) / ((bd - p##c.y) / zb + (pb.y - bd) / z##c);\
		draw_tri(dest, ds, p0, p1, p##c, z0, z1, z##c, cm, col); return;\
	}\
	else {\
		dvec p1 = pb; p1.y = bd; double z1;\
		p1.x += (p##a.x - pb.x) * (bd - pb.y) / (p##a.y - pb.y);\
		z1 = (p##a.y - pb.y) / ((bd - pb.y) / z##a + (p##a.y - bd) / zb);\
		draw_tri(dest, ds, p0, pb, p##c, z0, zb, z##c, cm, col);\
		draw_tri(dest, ds, p0, pb, p1, z0, zb, z1, cm, col); return;\
	}\
}

void sub_draw_tri(tile& dest, dbuf& ds, dvec pa, dvec pb, dvec pc,
	double za, double zb, double zc, dcol col, bool same_y) {
	if (pa.y == pb.y) { return; }
	int sy = pb.y > pa.y ? 1 : -1;
	int sx = pc.x > pb.x ? 1 : -1;
	for (int y = pa.y; y != pb.y + sy; y += sy) {
		int xb = pa.x * (pb.y - y) + pb.x * (y - pa.y); xb /= (pb.y - pa.y);
		int xc = pa.x * (pb.y - y) + pc.x * (y - pa.y); xc /= (pb.y - pa.y);
		double z0 = (pb.y - pa.y) / ((pb.y - y) / za + (y - pa.y) / zb);
		double z1 = (pb.y - pa.y) / ((pb.y - y) / za + (y - pa.y) / zc);
		for (int x = xb; x != xc + sx; x += sx) {
			int dp = same_y ?
				y * dest.w + x : x * dest.w + y;
			double z = (xc - xb) / ((x - xb) / z1 + (xc - x) / z0);
			if (ds[dp] <= -z) { ds[dp] = -z; dest.cols[dp] = col; }
		}
	}
}
void draw_tri(tile& dest, dbuf& ds, dvec pa, dvec pb, dvec pc,
	double za, double zb, double zc, cam const& cm, dcol col) {
	// 如果可以让这里写得好看有条理，容易修改且不容易出 bug 就好了。
	// 这里有点低效，重复比较了，但是总的影响不大。
	long long x0 = cm.vp.left();
	long long x1 = cm.vp.right() - 1;
	long long y0 = cm.vp.top();
	long long y1 = cm.vp.bottom() - 1;
	// 不要直接把表达式放在 TMP 里面。

	if (pa.x < pb.x) { swap(pa, pb); swap(za, zb); }
	if (pa.x < pc.x) { swap(pa, pc); swap(za, zc); }
	if (pb.x < pc.x) { swap(pb, pc); swap(zb, zc); }
	if (pa.x == pc.x) { return; }
	int dx = pa.x - pc.x;
	TMP(a, c, y, x, > , x1);
	TMP(c, a, y, x, < , x0);

	if (pa.y < pb.y) { swap(pa, pb); swap(za, zb); }
	if (pa.y < pc.y) { swap(pa, pc); swap(za, zc); }
	if (pb.y < pc.y) { swap(pb, pc); swap(zb, zc); }
	if (pa.y == pc.y) { return; }
	int dy = pa.y - pc.y;
	TMP(a, c, x, y, > , y1);
	TMP(c, a, x, y, < , y0);

#undef TMP
	// 这里是出于数值稳定性的考虑交换一下方向，否则如果三角形很扁的话会奇怪。
	bool y_big = dy > dx;
	if (!y_big) {
		if (pa.x < pb.x) { swap(pa, pb); swap(za, zb); }
		if (pa.x < pc.x) { swap(pa, pc); swap(za, zc); }
		if (pb.x < pc.x) { swap(pb, pc); swap(zb, zc); }
		swap(pa.x, pa.y); swap(pb.x, pb.y); swap(pc.x, pc.y);
	}
	dvec pd = pb; double zd;
	pd.x = pa.x * (pb.y - pc.y) + pc.x * (pa.y - pb.y); pd.x /= pa.y - pc.y;
	zd = (pa.y - pc.y) / ((pb.y - pc.y) / za + (pa.y - pb.y) / zc);
	sub_draw_tri(dest, ds, pa, pb, pd, za, zb, zd, col, y_big);
	sub_draw_tri(dest, ds, pc, pb, pd, zc, zb, zd, col, y_big);
}

#define TMP(a, c, x, y, sgn, bd)\
if (p##c.y sgn bd) { return; }\
if (p##a.y sgn bd) {\
	dvec p0 = p##c; p0.y = bd; double z0; col3 col0;\
	p0.x += (p##a.x - p##c.x) * (bd - p##c.y) / (p##a.y - p##c.y);\
	z0 = (p##a.y - p##c.y) / ((bd - p##c.y) / z##a + (p##a.y - bd) / z##c);\
	col0 = ((bd - p##c.y) * col##a / z##a + (p##a.y - bd) * col##c / z##c)\
		/ (p##a.y - p##c.y) * z0;\
	if (pb.y sgn bd) {\
		dvec p1 = p##c; p1.y = bd; double z1; col3 col1;\
		p1.x += (pb.x - p##c.x) * (bd - p##c.y) / (pb.y - p##c.y);\
		z1 = (pb.y - p##c.y) / ((bd - p##c.y) / zb + (pb.y - bd) / z##c);\
		col1 = ((bd - p##c.y) * colb / zb + (pb.y - bd) * col##c / z##c) \
			/ (pb.y - p##c.y) * z1;\
		draw_tri(dest, ds, p0, p1, p##c, col0, col1, col##c, z0, z1, z##c, cm); return;\
	}\
	else {\
		dvec p1 = pb; p1.y = bd; double z1; col3 col1;\
		p1.x += (p##a.x - pb.x) * (bd - pb.y) / (p##a.y - pb.y);\
		z1 = (p##a.y - pb.y) / ((bd - pb.y) / z##a + (p##a.y - bd) / zb);\
		col1 = ((bd - pb.y) * col##a / z##a + (p##a.y - bd) * colb / zb)\
			/ (p##a.y - pb.y) * z1;\
		draw_tri(dest, ds, p0, pb, p##c, col0, colb, col##c, z0, zb, z##c, cm);\
		draw_tri(dest, ds, p0, pb, p1, col0, colb, col1, z0, zb, z1, cm); return;\
	}\
}

void sub_draw_tri(tile& dest, dbuf& ds,
	dvec pa, dvec pb, dvec pc, col3 ca, col3 cb, col3 cc,
	double za, double zb, double zc, bool same_y) {
	// 可能退化的情况真的要认真对待。
	if (pa.y == pb.y) { return; }
	int sy = pb.y > pa.y ? 1 : -1;
	int sx = pc.x > pb.x ? 1 : -1;
	
	//if (same_y) {
	//	pa.y = clmp(pa.y - sy, 0, dest.h - 1);
	//	pb.y = pc.y = clmp(pb.y + sy, 0, dest.h - 1);
	//	pb.x = clmp(pb.x - sx, 0, dest.w - 1);
	//	pc.x = clmp(pc.x + sx, 0, dest.w - 1);
	//} else {
	//	pa.y = clmp(pa.y - sy, 0, dest.w - 1);
	//	pb.y = pc.y = clmp(pb.y + sy, 0, dest.w - 1);
	//	pb.x = clmp(pb.x - sx, 0, dest.h - 1);
	//	pc.x = clmp(pc.x + sx, 0, dest.h - 1);
	//}
	// 这样是为了避免出现缝隙。clmp 是为了避免超出范围。
	// 非常不优美，不知道还有啥解决方法。

	for (int y = pa.y; y != pb.y + sy; y += sy) {
		int xb = pa.x * (pb.y - y) + pb.x * (y - pa.y); xb /= (pb.y - pa.y);
		int xc = pa.x * (pb.y - y) + pc.x * (y - pa.y); xc /= (pb.y - pa.y);
		double z0 = (pb.y - pa.y) / ((pb.y - y) / za + (y - pa.y) / zb);
		double z1 = (pb.y - pa.y) / ((pb.y - y) / za + (y - pa.y) / zc);
		col3 c0 = ((pb.y - y) * ca / za + (y - pa.y) * cb / zb) / (pb.y - pa.y) * z0;
		col3 c1 = ((pb.y - y) * ca / za + (y - pa.y) * cc / zc) / (pb.y - pa.y) * z1;

		for (int x = xb; x != xc + sx; x += sx) {
			int dp = same_y ?
				y * dest.w + x : x * dest.w + y;
			double z = (xc - xb) / ((x - xb) / z1 + (xc - x) / z0);
			col3 c = ((x - xb) * c1 / z1 + (xc - x) * c0 / z0) / (xc - xb) * z;
			if (ds[dp] <= -z) { ds[dp] = -z; dest.cols[dp] = (dcol)c; }
		}
	}
}
void draw_tri(tile& dest, dbuf& ds,
	dvec pa, dvec pb, dvec pc, col3 cola, col3 colb, col3 colc,
	double za, double zb, double zc, cam const& cm) {
	// 如果可以让这里写得好看有条理，容易修改且不容易出 bug 就好了。
	// 这里有点低效，重复比较了，但是总的影响不大。
	long long x0 = cm.vp.left();
	long long x1 = cm.vp.right() - 1;
	long long y0 = cm.vp.top();
	long long y1 = cm.vp.bottom() - 1;
	// 不要直接把表达式放在 TMP 里面。

	if (pa.x < pb.x) { swap(pa, pb); swap(za, zb); swap(cola, colb); }
	if (pa.x < pc.x) { swap(pa, pc); swap(za, zc); swap(cola, colc); }
	if (pb.x < pc.x) { swap(pb, pc); swap(zb, zc); swap(colb, colc); }
	if (pa.x == pc.x) { return; }
	int dx = pa.x - pc.x;
	TMP(a, c, y, x, > , x1);
	TMP(c, a, y, x, < , x0);

	if (pa.y < pb.y) { swap(pa, pb); swap(za, zb); swap(cola, colb); }
	if (pa.y < pc.y) { swap(pa, pc); swap(za, zc); swap(cola, colc); }
	if (pb.y < pc.y) { swap(pb, pc); swap(zb, zc); swap(colb, colc); }
	if (pa.y == pc.y) { return; }
	int dy = pa.y - pc.y;
	TMP(a, c, x, y, > , y1);
	TMP(c, a, x, y, < , y0);

	// 这里是出于数值稳定性的考虑交换一下方向，否则如果三角形很扁的话会奇怪。
	bool y_big = dy > dx;
	if (!y_big) {
		if (pa.x < pb.x) { swap(pa, pb); swap(za, zb); swap(cola, colb); }
		if (pa.x < pc.x) { swap(pa, pc); swap(za, zc); swap(cola, colc); }
		if (pb.x < pc.x) { swap(pb, pc); swap(zb, zc); swap(colb, colc); }
		swap(pa.x, pa.y); swap(pb.x, pb.y); swap(pc.x, pc.y);
	}
	dvec pd = pb; double zd;
	pd.x = pa.x * (pb.y - pc.y) + pc.x * (pa.y - pb.y); pd.x /= pa.y - pc.y;
	zd = (pa.y - pc.y) / ((pb.y - pc.y) / za + (pa.y - pb.y) / zc);
	col3 cold = ((pb.y - pc.y) * cola / za + (pa.y - pb.y) * colc / zc) 
		/ (pa.y - pc.y) * zd;
	sub_draw_tri(dest, ds, pa, pb, pd, cola, colb, cold, za, zb, zd, y_big);
	sub_draw_tri(dest, ds, pc, pb, pd, colc, colb, cold, zc, zb, zd, y_big);
}
