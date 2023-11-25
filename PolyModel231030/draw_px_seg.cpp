#include "draw_px_seg.h"

// �������ﾭ������������⣬��֪�ܲ��ܴ���һ�¡�
// �� int ���� long long �ˣ�Ӧ�û��а�����
bool pre_draw_px_seg(dvec& pa, dvec& pb, drect vp) {
	long long x0 = vp.left();
	long long x1 = vp.right() - 1;
	long long y0 = vp.top();
	long long y1 = vp.bottom() - 1;

	// ��������˲��ᷢ������ 0 �����⡣
#define TMP(a, b, x, y, bd, sym)\
if (p##a.x sym bd) {\
	p##a.y +=  (p##b.y - p##a.y) * (bd - p##a.x) / (p##b.x - p##a.x);\
	p##a.x = bd;\
}

	bool b0 = pa.x < x0&& pb.x < x0;
	bool b1 = pa.x > x1 && pb.x > x1;
	if (b0 || b1) { return false; }
	TMP(a, b, x, y, x0, < );
	TMP(a, b, x, y, x1, > );
	TMP(b, a, x, y, x0, < );
	TMP(b, a, x, y, x1, > );

	b0 = pa.y < y0&& pb.y < y0;
	b1 = pa.y > y1 && pb.y > y1;
	if (b0 || b1) { return false; }
	TMP(a, b, y, x, y0, < );
	TMP(a, b, y, x, y1, > );
	TMP(b, a, y, x, y0, < );
	TMP(b, a, y, x, y1, > ); return true;
#undef TMP
}
void draw_px_seg(tile& dest, dbuf& ds, 
	dvec pa, dvec pb, double d, drect vp, dcol c) {
	if (vp.w <= 0 || vp.h <= 0) { return; }
	if (!pre_draw_px_seg(pa, pb, vp)) { return; }

	bool changed = false;
	int x = pa.x; int y = pa.y;
	int dx = abs(pb.x - pa.x);
	int dy = abs(pb.y - pa.y);
	int sx = (pb.x > pa.x) ? 1 : -1;
	int sy = (pb.y > pa.y) ? 1 : -1;
	if (dy > dx) { swap(dx, dy); changed = true; }

	int e = dx;
	int id = y * dest.w + x;
	if (ds[id] <= d) { ds[id] = d; dest.cols[id] = c; }

	rep(i, 0, dx) { 
		e -= 2 * dy;
		if (changed) { y += sy; } else { x += sx; }
		if (e < 0) { 
			e += 2 * dx;
			if (changed) { x += sx; } else { y += sy; }
		} id = y * dest.w + x;
		if (ds[id] <= d) { ds[id] = d; dest.cols[id] = c; }
	}
}
void draw_px_seg(tile& dest, dbuf& ds, 
	vec2 pa, vec2 pb, double d, drect vp, dcol c) {
	draw_px_seg(dest, ds, (dvec)pa, (dvec)pb, d, vp, c);
}

bool pre_draw_px_seg
(dvec& pa, dvec& pb, double& za, double& zb, cam const& cm) {
	long long x0 = cm.vp.left();
	long long x1 = cm.vp.right() - 1;
	long long y0 = cm.vp.top();
	long long y1 = cm.vp.bottom() - 1;

	// ��������˲��ᷢ������ 0 �����⡣
	// ������˳���ܻ���Ҫ��Ȼ�Ͳ����ˡ�
#define TMP(a, b, x, y, bd, sym)\
if (p##a.x sym bd) {\
	p##a.y +=  (p##b.y - p##a.y) * (bd - p##a.x) / (p##b.x - p##a.x);\
	z##a = (p##a.x - p##b.x) / ((p##a.x - bd) / z##b + (bd - p##b.x) / z##a);\
	p##a.x = bd;\
}

	bool b0 = pa.x < x0&& pb.x < x0;
	bool b1 = pa.x > x1 && pb.x > x1;
	if (b0 || b1) { return false; }
	TMP(a, b, x, y, x0, < );
	TMP(a, b, x, y, x1, > );
	TMP(b, a, x, y, x0, < );
	TMP(b, a, x, y, x1, > );

	b0 = pa.y < y0&& pb.y < y0;
	b1 = pa.y > y1 && pb.y > y1;
	if (b0 || b1) { return false; }
	TMP(a, b, y, x, y0, < );
	TMP(a, b, y, x, y1, > );
	TMP(b, a, y, x, y0, < );
	TMP(b, a, y, x, y1, > ); return true;
#undef TMP
}
void draw_px_seg(tile& dest, dbuf& ds,
	dvec pa, dvec pb, double za, double zb, cam const& cm, dcol c) {
	if (cm.vp.w <= 0 || cm.vp.h <= 0) { return; }
	if (!pre_draw_px_seg(pa, pb, za, zb, cm)) { return; }

	bool changed = false;
	int x = pa.x; int y = pa.y;
	int dx = abs(pb.x - pa.x);
	int dy = abs(pb.y - pa.y);
	int sx = (pb.x > pa.x) ? 1 : -1;
	int sy = (pb.y > pa.y) ? 1 : -1;
	if (dy > dx) { swap(dx, dy); changed = true; }

	int e = dx;
	int id = y * dest.w + x;
	double z = za;
	double d = -z + eps_edge;
	if (ds[id] <= d) { ds[id] = d; dest.cols[id] = c; }

	rep(i, 0, dx) {
		e -= 2 * dy;
		if (changed) { y += sy; } else { x += sx; }
		if (e < 0) {
			e += 2 * dx;
			if (changed) { x += sx; } else { y += sy; }
		} id = y * dest.w + x;
		if (changed) {
			z = (pb.y - pa.y) / ((pb.y - y) / za + (y - pa.y) / zb);
		} else {
			z = (pb.x - pa.x) / ((pb.x - x) / za + (x - pa.x) / zb);
		} d = -z + eps_edge;
		if (ds[id] <= d) { ds[id] = d; dest.cols[id] = c; }
	}
}
