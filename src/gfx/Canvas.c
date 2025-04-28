/* "$Id: $"
 *
 *                          5 december MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */


#include <gfx/Canvas.h>
#include <gfx/IRaster.h>
#include <math.h>

#include "stb_truetype.h"


#define TR_X(x, y) ((int32)((x * self->state.m11 + \
	y * self->state.m21 + self->state.dx)) * 16.0)
#define TR_Y(x, y) ((int32)((x * self->state.m12 + \
	y * self->state.m22 + self->state.dy)) * 16.0)
#define TR_W(w, h) ((int32)(0.5 + w * self->state.m11 + h * self->state.m21))
#define TR_H(w, h) ((int32)(0.5 + w * self->state.m12 + h * self->state.m22))
#define TR_RX(r) ((int32)(0.5 + r * (self->state.m21 ? \
	sqrt(self->state.m11 * self->state.m11 + \
		self->state.m21 * self->state.m21) : \
	fabs(self->state.m11))))
#define TR_RY(r) ((int32)(0.5 + r * (self->state.m12 ? \
	sqrt(self->state.m12 * self->state.m12 + \
		self->state.m22 * self->state.m22) : \
	fabs(self->state.m22))))

#define FLAGS_SWAPPED 1

#define GAMMA 2.0

static uint16 gamma_to_linear[0x100];
static uint8 linear_to_gamma[0x10001];

static void init_gamma(void);

static void set_transform(Ex **e__, gfx__Canvas *self, float32 m11, 
	float32 m12, float32 m21, float32 m22, float32 dx, float32 dy);

static void __init__(Ex **e__)
{
	(void)e__;
	gfx__Canvas__.super = &Object__;
	
	init_gamma();

	return;
}

gfx__Font *default_font = 0;

static void __ctor__(Ex **e__, gfx__Canvas *self)
{
	F("__ctor__");	
	self->path.first = 0;
	self->path.last = 0;
	self->state.previous = 0;

	set_transform(e__, self, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
 
	if (!default_font) {
		String n;
		STR(n,"Tuffy");
		default_font = new_(gfx__Font);
		default_font->_->load(e__, default_font, &n);
	}

	return;
}



static void __dtor__(Ex **e__, gfx__Canvas *self)
{
	F("__dtor__");

	return;
}

static uint32 hash_code(Ex **e__, gfx__Canvas *self)
{
	(void)e__;
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, gfx__Canvas *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void init_gamma(void)
{
	int32 i;
	
	for (i = 0; i < 0x100; i++) {
		gamma_to_linear[i] = (uint16)(pow((float32)i / 0xFF, GAMMA) * (0x10000 / 2) + 0.5);
	}
	for (i = 0; i < 0x10001; i++) {
		linear_to_gamma[i] = (uint8)(pow((float32)i / 0x10000, 1.0 / GAMMA) * 0xFF + 0.5);
	}

}

static void set_context(Ex **e__, gfx__Canvas *self, 
		struct gfx__IRaster *raster,
		int32 x, int32 y, int32 w, int32 h)
{
	raster->_->get_info(e__, raster, x, y, w, h, &self->raster);
	self->state.clipping_region.left = x;
	self->state.clipping_region.right = x + w;
	self->state.clipping_region.top = y;
	self->state.clipping_region.bottom = y + h;

}

static void save_state(Ex **e__, gfx__Canvas *self)
{
	gfx__Canvas_state *p;

	p = Gc__malloc(e__, sizeof(gfx__Canvas_state));
	*p = self->state;
	self->state.previous = p;
}
	
static void restore_state(Ex **e__, gfx__Canvas *self)
{
	gfx__Canvas_state *p = self->state.previous;
	if (!p) return;
	self->state = *p;
	Gc__free(e__, p);
}

static void transform(Ex **e__, gfx__Canvas *self, float32 m11, 
	float32 m12, float32 m21, float32 m22, float32 dx, float32 dy)
{
	float32 om11, om12, om21, om22, odx, ody;

	om11 = self->state.m11;
	om12 = self->state.m12;
	om21 = self->state.m21;
	om22 = self->state.m22;
	odx = self->state.dx;
	ody = self->state.dy;
	
	self->state.m11 = m11 * om11 + m12 * om21;
	self->state.m12 = m11 * om12 + m12 * om22;
	self->state.m21 = m21 * om11 + m22 * om21;
	self->state.m22 = m21 * om12 + m22 * om22;
	self->state.dx = dx * om11 + dy * m21 + odx;
	self->state.dy = dy * om22 + dx * m12 + ody;
}


static void scale(Ex **e__, gfx__Canvas *self, float32 x, float32 y)
{
	transform(e__, self, x, 0.0, 0.0, y, 0.0, 0.0);
}
	
static void rotate(Ex **e__, gfx__Canvas *self, float32 angle)
{
	float32 s, c;

	s = (float32)sin(angle); 
	c = (float32)cos(angle);
	transform(e__, self, c, -s, s, c, 0.0, 0.0);	
}

static void translate(Ex **e__, gfx__Canvas *self, float32 x, float32 y)
{
	transform(e__, self, 1.0, 0.0, 0.0, 1.0, x, y);

}

static void set_transform(Ex **e__, gfx__Canvas *self, float32 m11, 
	float32 m12, float32 m21, float32 m22, float32 dx, float32 dy)
{
	self->state.m11 = m11;
	self->state.m12 = m12;
	self->state.m21 = m21;
	self->state.m22 = m22;
	self->state.dx = dx;
	self->state.dy = dy;
}
	
static void clear_rect(Ex **e__, gfx__Canvas *self, float32 x, float32 y,
		float32 w, float32 h)
{
	int32 i, j;
	int32 mx, my, hh;
	uint8 *rgba = self->raster.data;

	mx = x + w;
	my = y + h;
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (mx > self->w) mx = self->w;
	if (my > self->h) my = self->h;
	hh = self->h - 1;

	for (i = y; i < my; i++) {
		int32 l;

		l = self->raster.scanline_width * (hh - i);
		l += x * 4;
		for (j = x; j < mx; j++) {
			rgba[l++] = 0x0;
			rgba[l++] = 0x0;
			rgba[l++] = 0x0;
			rgba[l++] = 0x0;
		}
	}

}


static void clip_dimension_f(Ex **e__, gfx__Canvas *self, float32 *x, float32 *y,
		float32 *w, float32 *h)
{
	gfx__Canvas_path *p;
	p = &self->state.clipping_region;
	if (*x + *w > p->right) {
		*w = p->right - *x;
		if (*w < 0.0) *w = 0;
	}
	if (*y + *h > p->bottom) {
		*h = p->bottom - *y;
		if (*h < 0.0) *h = 0;
	}
	if (*x < p->left) *x = p->left;
	if (*y < p->top) *y = p->top;

}

static void clip_dimension(Ex **e__, gfx__Canvas *self, int32 *x, int32 *y,
		int32 *w, int32 *h)
{
	gfx__Canvas_path *p;
	p = &self->state.clipping_region;
	if (*x + *w > p->right) {
		*w = p->right - *x;
		if (*w < 0) *w = 0;
	}
	if (*y + *h > p->bottom) {
		*h = p->bottom - *y;
		if (*h < 0) *h = 0;
	}
	if (*x < p->left) *x = p->left;
	if (*y < p->top) *y = p->top;

}

static void fill_rect(Ex **e__, gfx__Canvas *self, float32 x, float32 y,
		float32 w, float32 h)
{
	int32 i, j;
	int32 mx, my, hh;
	uint8 r, g, b;
	uint32 a;
	uint8 *rgba = self->raster.data;

	clip_dimension_f(e__, self, &x, &y, &w, &h);
	mx = x + w;
	my = y + h;
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (mx > self->raster.w) mx = self->raster.w;
	if (my > self->raster.h) my = self->raster.h;
	hh = self->raster.h - 1;

	r = self->state.fill_style & 0xFF;
	g = (self->state.fill_style >> 8) & 0xFF;
	b = (self->state.fill_style >> 16) & 0xFF;
	a = (self->state.fill_style >> 24) & 0xFF;
	if (a == 0xFF) {
		for (i = y; i < my; i++) {
			int32 l;

			l = self->raster.scanline_width * (hh - i);
			l += x * 4;
			for (j = x; j < mx; j++) {
				rgba[l++] = r;
				rgba[l++] = g;
				rgba[l++] = b;
				rgba[l++] = 0xFF;
			}
		}
	} else {
		uint32 na = 0xFF - a;

		for (i = y; i < my; i++) {
			int32 l;

			l = self->raster.scanline_width * (hh - i);
			l += x * 4;
			for (j = x; j < mx; j++) {
				rgba[l] = 
					(rgba[l] * na + r * a) / 0xFF;
				l++;
				rgba[l] = 
					(rgba[l] * na + g * a) / 0xFF;
				l++;
				rgba[l] = 
					(rgba[l] * na + b * a) / 0xFF;
				l++;
				rgba[l] = 
					(rgba[l] * na + a * a) / 0xFF;

			}
		}
	}
}

static void stroke_rect(Ex **e__, gfx__Canvas *self, float32 x, float32 y,
		float32 w, float32 h)
{
}

static void delete_vertex(Ex **e__, gfx__Canvas *self, gfx__Canvas_vertex *vx)
{
	F("delete_vertex");
	Gc__free(e__, vx);
}
	
static void delete_subpath(Ex **e__, gfx__Canvas *self, gfx__Canvas_subpath *sp)
{
	F("delete_subpath");
	gfx__Canvas_vertex *vx;

	vx = sp->first;
	while (vx != 0) {
		gfx__Canvas_vertex *v;
		v = vx->next;
		delete_vertex(e__, self, vx);
		vx = v;
	}
	Gc__free(e__, sp);
}


static void new_subpath(Ex **e__, gfx__Canvas *self)
{
	F("new_subpath");
	gfx__Canvas_subpath *sp;
	gfx__Canvas_path *pa;
	
	sp = (gfx__Canvas_subpath*)Gc__malloc(e__, sizeof(*sp));
	sp->first = 0;
	sp->last = 0;
	sp->next = 0;

	pa = &self->path;
	if (pa->first == 0) {
		pa->first = sp;
	} else {
		pa->last->next = sp;
	}
	pa->last = sp;
}

static void add_vertex(Ex **e__, gfx__Canvas *self, int32 x, int32 y)
{
	F("add_vertex");
	gfx__Canvas_vertex *vx;
	gfx__Canvas_subpath *sp;

	vx = (gfx__Canvas_vertex*)Gc__malloc(e__, sizeof(*vx));

	vx->next = 0;
	vx->x = x;
	vx->y = y;

	sp = self->path.last;
	if (sp->first == 0) {
		sp->first = vx;
	} else {
		sp->last->next = vx;
	}
	sp->last = vx;
}

static void begin_path(Ex **e__, gfx__Canvas *self)
{
	gfx__Canvas_subpath *sp;

	sp = self->path.first;
	while (sp != 0) {
		gfx__Canvas_subpath *s;
		s = sp->next;
		delete_subpath(e__, self, sp);
		sp = s;
	}
	self->path.first = 0;
	self->path.last = 0;
}

static void close_path(Ex **e__, gfx__Canvas *self)
{
	gfx__Canvas_subpath *sp;

	if (self->path.last == 0) return;
	sp = self->path.last;
	if (sp->first == 0) return;
	if (sp->first->x != sp->last->x ||
		sp->first->y != sp->last->y)
	{
		add_vertex(e__, self, sp->first->x, 
			sp->first->y);
	}
	new_subpath(e__, self);
	add_vertex(e__, self, sp->first->x, 
		sp->first->y);

}

static void move_to(Ex **e__, gfx__Canvas *self, float32 x, float32 y)
{
	new_subpath(e__, self);
	add_vertex(e__, self, TR_X(x, y), TR_Y(x, y));
}

static void line_to(Ex **e__, gfx__Canvas *self, float32 x, float32 y)
{
	if (self->path.first == 0) {
		new_subpath(e__, self);
	}
	add_vertex(e__, self, TR_X(x, y), TR_Y(x, y));
}

static void quadratic_curve_to(Ex **e__, gfx__Canvas *self, 
		float32 cpx, float32 cpy, float32 x, float32 y)
{
}

static void bezier_curve_to(Ex **e__, gfx__Canvas *self, 
		float32 cp1x, float32 cp1y, float32 cp2x, float32 cp2y, 
		float32 x, float32 y)
{
}

static void arc_to(Ex **e__, gfx__Canvas *self, 
	float32 x1, float32 y1, float32 x2, float32 y2, float32 radius)
{
}

static void rect(Ex **e__, gfx__Canvas *self, float32 x, float32 y,
		float32 w, float32 h)
{
	gfx__Canvas_path *p = &self->path;
	p->left = x;
	p->right = x + w;
	p->top = y;
	p->bottom = y + h;
}

static void arc(Ex **e__, gfx__Canvas *self, float32 x, float32 y,
		float32 radius, float32 start_angle, float32 end_angle,
		uint8 anticlockwise)
{
}

static gfx__Canvas_sorted_edges *new_sorted_edges(
	Ex **e__, gfx__Canvas *self)
{
	gfx__Canvas_sorted_edges *se;

	se = Gc__malloc(e__, sizeof(*se));
	return se;
}

static void fill_horizontal(Ex **e__, gfx__Canvas *self,
	int32 x0, int32 y, int32 x1, uint8 stroke)
{
	uint32 c;
	uint32 r;
	uint32 g;
	uint32 b;
	uint32 a;
	uint32 s;
	uint8 *p;
	int32 lw;
	int32 xx0;
	int32 xx1;
	uint32 q;
	uint32 o;

	lw = self->raster.scanline_width;
	p = &self->raster.data[0];

	xx0 = x0;
	xx1 = x1;
	x0 >>= 4;
	x1 >>= 4;	
	if (x1 > self->raster.w) x1 = self->raster.w;

	y = self->raster.h - 1 - y;

	p = p + y * lw;
	p = p + x0 * 4;
	if (stroke != 0) {
		c = self->state.stroke_style;
	} else {
		c = self->state.fill_style;
	}
	
	r = c & 0xFF;
	g = (c >> 8) & 0xFF;
	b = (c >> 16) & 0xFF;
	a = (c >> 24) & 0xFF;
	s = 0xFF - a;

	if (x0 == x1) {
		q = xx1 - xx0;
		q *= a;
		o = 0xFF - (q >> 4);
		p[0] = (p[0] * o + (r * q) >> 8);
		p[1] = (p[1] * o + (g * q) >> 8);
		p[2] = (p[2] * o + (b * q) >> 8);
		p[3] = (p[3] * o + (a * q) >> 8);
		return;
	} else if ((xx0 & 0x0F) != 0) {
		q = a * (xx0 & 0x0F);
		o = 0xFF - (q >> 4);
		p[0] = (p[0] * o + ((r * q) >> 4));
		p[1] = (p[1] * o + ((g * q) >> 4));
		p[2] = (p[2] * o + ((b * q) >> 4));
		p[3] = (p[3] * o + ((a * q) >> 4));
		p += 4;
		x0++;
	}

	while (x0 < x1) {
		p[0] = (p[0] * s + (r * a) >> 4) >> 4;
		p[1] = (p[1] * s + (g * a) >> 4) >> 4;
		p[2] = (p[2] * s + (b * a) >> 4) >> 4;
		p[3] = (p[3] * s + (a * a) >> 4) >> 4;
		p += 4;
		x0++;
	}

	q = a * (xx1 & 0x0F);
	o = 0xFF - (q >> 4);
	p[0] = (p[0] * o + (r * q) >> 4);
	p[1] = (p[1] * o + (g * q) >> 4);
	p[2] = (p[2] * o + (b * q) >> 4);
	p[3] = (p[3] * o + (a * q) >> 4);
}		

static void delete_edge(
	Ex **e__, gfx__Canvas *self, gfx__Canvas_edge *edge)
{

	while (edge) {
		gfx__Canvas_edge *n;

		n = edge->next;
		Gc__free(e__, edge);
		edge = n;
	}
}


static void sort_edge(
	Ex **e__, gfx__Canvas *self, gfx__Canvas_sorted_edges *edges, int32 y)
{
	gfx__Canvas_edge *ed;
	gfx__Canvas_edge *nx;
	int32 x;

	ed = edges->first;
	while (ed != 0) {
		if (ed->bottom <= y) {
			nx = ed->next;
			if (ed == edges->first) {
				edges->first = nx;
			} else {
				ed->prev->next = nx;
				if (nx) nx->prev = ed->prev;
				
			}
			ed->next = 0;
			delete_edge(e__, self, ed);			
			ed = nx;
		} else {
			ed = ed->next;
		}
	}

	if (edges->first == 0) return;

	ed = edges->first;
	while (ed != 0 && ed->next) {
		nx = ed->next;
		if (nx->x < ed->x) {
			if (ed == edges->first) {
				edges->first = nx;
				ed->next = nx->next;
				if (nx->next != 0) nx->next->prev = ed;
				nx->next = ed;
				ed->prev = nx;
			} else {
				ed->prev->next = nx;
				ed->next = nx->next;
				if (ed->next != 0) ed->next->prev = ed;
				nx->prev = ed->prev;				
				ed->prev = nx;
				nx->next = ed;
			}
		}			
		ed = nx;
	}

}

static void merge_edge(Ex **e__, gfx__Canvas *self, 
	gfx__Canvas_sorted_edges *edges, gfx__Canvas_sorted_edges *add)
{
	gfx__Canvas_edge *ed;
	gfx__Canvas_edge *ea;
	gfx__Canvas_edge *nx;
	gfx__Canvas_edge *px;

	ed = edges->first;
	ea = add->first;
	add->first = 0;
	
	if (ea == 0) return;

	if (ed != 0) {
		if (ea->x < ed->x) {
			edges->first = ea;		
			while (ea->next != 0 && ea->next->x < ed->x) {
				ea = ea->next;
			}
			nx = ea->next;
			ea->next = ed;
			ed->prev = ea;

			ea = nx;
		}
		ed = ed->next;
	}
	while (ed != 0 && ea != 0) {
		if (ea->x < ed->x) {
			ed->prev->next = ea;
			ea->prev = ed->prev;		
			while (ea->next != 0 && ea->next->x < ed->x) {
				ea = ea->next;
			}
			nx = ea->next;
			ea->next = ed;
			ed->prev = ea;

			ea = nx;
		}

		ed = ed->next;
	}
	if (ea != 0) {
		ed = edges->first;
		if (ed != 0) {
			while (ed->next != 0) {
				ed = ed->next;
			}
			ed->next = ea;
		} else {
			edges->first = ea;
		}
		ea->prev = ed;
	}
}

static void line_mask(Ex **e__, gfx__Canvas *self, int32 x0, int32 x1)
{
	uint8 *p;
	int32 x00;
	int32 x11;

	p = self->raster.mask;
	x00 = x0 >> 4;
	x11 = x1 >> 4;
	if (x00 == x11) {
		p[x00] += x1 - x0;
		return;
	}
	p[x00] += 0x0F - (x0 & 0x0F);
	p[x11] += (x1 & 0x0F);
	x00++;
	while (x00 < x11) {
		p[x00] += 16;
		if (p[x00] < 16) p[x00] = 0xFF;
		x00++;
	}
}

static void clear_mask(Ex **e__, gfx__Canvas *self)
{
	int32 i;
	uint8 *p;

	p = self->raster.mask;
	i = self->raster.w;
	while (i > 0) {
		i--;
		p[i] = 0;
	}	
}

static void fill_mask(Ex **e__, gfx__Canvas *self,
	 int32 y, int32 x0, int32 x1, uint8 stroke)
{
	uint32 c;
	uint32 r;
	uint32 g;
	uint32 b;
	uint32 a;
	uint32 s;
	uint8 *p;
	uint8 *m;
	int32 lw;
	uint32 q;
	uint32 o;
	
	lw = self->raster.scanline_width;
	p = &self->raster.data[0];
	m = &self->raster.mask[0];

	y = self->raster.h - 1 - y;

	p = p + y * lw;
	p = p + x0 * 4;
	if (stroke != 0) {
		c = self->state.stroke_style;
	} else {
		c = self->state.fill_style;
	}
	
	r = c & 0xFF;
	g = (c >> 8) & 0xFF;
	b = (c >> 16) & 0xFF;
	a = (c >> 24) & 0xFF;

	while (x0 < x1) {
		uint32 aa;
		uint32 t;

		aa = ((a + 1) * m[x0]) >> 8;
		s = ((0x100 - aa) * (p[3])) >> 8;

		t = (((gamma_to_linear[p[0]] * s) >> 7) + (r * aa));
		//if (t > 0xFFFF) t = 0xFFFF;
		p[0] = linear_to_gamma[t];
		t = (((gamma_to_linear[p[1]] * s) >> 7) + (g * aa));
		//if (t > 0xFFFF) t = 0xFFFF;
		p[1] = linear_to_gamma[t];
		t = (((gamma_to_linear[p[2]] * s) >> 7) + (b * aa));
		//if (t > 0xFFFF) t = 0xFFFF;
		p[2] = linear_to_gamma[t];
		t = s + aa;
		if (t > 0xFF) t = 0xFF;
		p[3] = t;
		p += 4;
		x0++;
	}
}

static void fill_edges(
	Ex **e__, gfx__Canvas *self, gfx__Canvas_sorted_edges *edges,
	uint8 stroke)
{
	int32 y;
	int32 x0 = 0;
	int32 w;
	int32 q;
	int32 max_h;
	int32 max_w;
	int32 xmin;
	int32 xmax;
	gfx__Canvas_sorted_edges *next;
	gfx__Canvas_edge *ed;

	y = edges->y;
	next = edges->next;

	max_h = self->raster.h << 4;
	max_w = self->raster.w << 4;
	xmin = self->raster.w << 4;
	xmax = 0;
	clear_mask(e__, self);

	do {
		ed = edges->first;
		w = 0;
		while (ed != 0) {
			ed->inc += ed->grad_num;
			q = ed->inc / ed->grad_den;
			ed->x += q;
			ed->inc -= q * ed->grad_den;

			if (w == 0) {
				x0 = ed->x;
				w += ed->winding;
				while (ed->next && (w + ed->next->winding) != 0)
				{
					ed = ed->next;
		
					ed->inc += ed->grad_num;
					q = ed->inc / ed->grad_den;
					ed->x += q;
					ed->inc -= q * ed->grad_den;

					w += ed->winding;
				}
			} else {
				int32 x1;

				w += ed->winding;
				x1 = ed->x;
				if (w == 0 && x0 != x1 && y >= 0 && x1 >= 0) {
					if (x0 < 0) x0 = 0;
					if (x1 > max_w) x1 = max_w;
					if (x0 < xmin) xmin = x0;
					if (x1 > xmax) xmax = x1;
					line_mask(e__, self, x0, x1);		
				}	
			}
			ed = ed->next;	
		}
		y++;
		if ((y & 0x0F) == 0) {
			if ((xmax & 0x0F) != 0) xmax += 16;
			fill_mask(e__, self, (y >> 4) - 1, xmin >> 4, xmax >> 4, stroke);
			if (y >= max_h) return;
			xmin = self->raster.w << 4;
			xmax = 0;
			clear_mask(e__, self);
		}
		 
		sort_edge(e__, self, edges, y);
		if (next && next->y == y) {
			merge_edge(e__, self, edges, next);
			next = next->next;
		}
	} while (edges->first != 0 || next);
}


static void delete_sorted_edges(
	Ex **e__, gfx__Canvas *self, gfx__Canvas_sorted_edges *edges)
{

	while (edges) {
		gfx__Canvas_sorted_edges *n;
		gfx__Canvas_edge *ed;

		n = edges->next;
		delete_edge(e__, self, edges->first);

		Gc__free(e__, edges);
		edges = n;
	}
}


static gfx__Canvas_edge *new_edge(
	Ex **e__, gfx__Canvas *self, gfx__Canvas_vertex *from,
	gfx__Canvas_vertex *to, int32 winding)
{
	gfx__Canvas_edge *ed;

	ed = (gfx__Canvas_edge*)Gc__malloc(e__, sizeof(*ed));
	ed->x = from->x;
	ed->top = from->y;
	ed->bottom = to->y;
	ed->inc = 0;
	ed->grad_num = to->x - from->x;
	ed->grad_den = to->y - from->y;
	ed->winding = winding;
	return ed;
}

static void add_edge(Ex **e__, gfx__Canvas *self, 
	gfx__Canvas_sorted_edges **edges, gfx__Canvas_vertex *from,
	gfx__Canvas_vertex *to)
{
	int32 winding = 1;
	gfx__Canvas_sorted_edges *ce;
	gfx__Canvas_sorted_edges *ns;
	int32 y;
	gfx__Canvas_edge *ne;

	ce = *edges;
	if (from->y == to->y) return;
	if (from->y > to->y) {
		gfx__Canvas_vertex *t;

		winding = -1;
		t = to;
		to = from;
		from = t;
	}
	y = from->y;

	ne = new_edge(e__, self, from, to, winding);

	if (ce == 0) {
		ce = new_sorted_edges(e__, self);
		ce->first = ne;
		ce->prev = 0;
		ce->next = 0;
		ce->y = y;
		ne->prev = 0;
		ne->next = 0;
	} else {
		while (ce->prev != 0 && ce->y > y) {
			ce = ce->prev;
		}
		while (ce->next != 0 && ce->y < y) {
			ce = ce->next;
		}
		if (ce->y == y) {
			int32 x;
			gfx__Canvas_edge *ed;

			x = from->x;
			ed = ce->first;
			while (ed->next != 0 && ed->x < x) {
				ed = ed->next;
			}

			if (ed->x <= x) {
				ne->prev = ed;
				ne->next = ed->next;
				ed->next = ne;			
			} else {
				ne->next = ed;
				ne->prev = ed->prev;
				ed->prev = ne;
				if (ed == ce->first) {
					ce->first = ne;
				}
			}
		} else if (ce->y < y) {
			ns = new_sorted_edges(e__, self);
			ns->y = y;
			ns->prev = ce;
			ns->next = ce->next;
			if (ns->next) ns->next->prev = ns;
			ce->next = ns;
			ce = ns;
			ns->first = ne;
			ne->next = 0;
			ne->prev = 0;
		} else {
			ns = new_sorted_edges(e__, self);
			ns->y = y;
			ns->next = ce;
			ns->prev = ce->prev;
			if (ns->prev) ns->prev->next = ns;
			ce->prev = ns;
			ce = ns;
			ns->first = ne;
			ne->next = 0;
			ne->prev = 0;
		}
	}
	*edges = ce;
}


static void fill(Ex **e__, gfx__Canvas *self)
{	
	gfx__Canvas_sorted_edges *ce = 0;
	gfx__Canvas_sorted_edges *se;
	gfx__Canvas_vertex *v;
	gfx__Canvas_vertex *f;
	gfx__Canvas_vertex *l;
	gfx__Canvas_subpath *p;

	p = self->path.first;
	while (p != 0) {
		if (p->first == 0) {
			p = p->next;
		} else {
			if (p->first != p->last)
			{
				v = p->first;
				break;
			}
			p = p->next;
		}
	}
	if (v == 0) return;

	f = v;
	l = v;
	while (v != 0) {
		add_edge(e__, self, &ce, l, v);
		if (v->next == 0 && (v->x != f->x || v->y != f->y))
		{
			add_edge(e__, self, &ce, v, f);
		}
		l = v;
		v = v->next;
		while (v == 0 && p->next) 
		{
			p = p->next;
			f = p->first;
			if (f != 0) {
				v = f->next;
			}
			l = f;
		}
	}
	if (ce == 0) return;
	while (ce->prev != 0) {
		ce = ce->prev;
	}
	se = ce;
	fill_edges(e__, self, se, 0);
	delete_sorted_edges(e__, self, se);
}

static void stroke(Ex **e__, gfx__Canvas *self)
{
}

static void clip(Ex **e__, gfx__Canvas *self)
{
	 {
		gfx__Canvas_path *c = &self->state.clipping_region;
		gfx__Canvas_path *n = &self->path;

		if (c->left < n->left) {
			c->left = n->left;
		}
		if (c->top < n->top) {
			c->top = n->top;
		}
		if (c->right > n->right) {
			c->right = n->right;
		}
		if (c->bottom > n->bottom) {
			c->bottom = n->bottom;
		}
	}
}

static uint8 is_point_in_path(Ex **e__, gfx__Canvas *self,
		float32 x, float32 y)
{
	return 0;
}

static void fill_text(Ex **e__, gfx__Canvas *self, String *text,
		float32 x, float32 y, float32 max_width)
{	
	uint8 r, g, b;
	uint32 a;
	uint8 *p;
	int32 lw;
	int32 X, Y, W, H;
	int32 rx, ry;
	gfx__Font_glyph gl;
	gfx__Font_metrics m;
	String n;
	int32 xx, yy;
	int32 i, l;
	utf8 *bf;

	x += 0.5;
	y += 0.5;
	lw = self->raster.scanline_width;
	p = &self->raster.data[0];

	r = self->state.fill_style & 0xFF;
	g = (self->state.fill_style >> 8) & 0xFF;
	b = (self->state.fill_style >> 16) & 0xFF;
	a = (self->state.fill_style >> 24) & 0xFF;
	
	l = text->_->get_size(e__, text);
	bf = text->_->get_cstr(e__, text);
	i = 0;

	default_font->_->get_metrics(e__, default_font, ' ', 
		default_font->size, &m);
	if (m.height > (int32)m.height) m.height += 1.0;
	if (m.descent > (int32)m.descent) m.descent += 1.0;
	y = (int32)((y + (m.descent) - (m.height)));
	H = m.height;
	W = max_width;
	X = x;
	Y = y;
	
	clip_dimension(e__, self, &X, &Y, &W, &H);

	while (i < l) {
		uint32 na = 0xFF - a;
		int32 bmp_x;
		int32 bmp_y;
		int32 len;
		int32 ucs;
		int32 height;

		len = String__utf2ucs(bf + i, l - i, &ucs);

		if (len > 0) {
			i += len;
		} else {
			i++;
		}

		default_font->_->get_glyph(e__, default_font, 
			ucs, default_font->size, &gl);
		bmp_x = (x + gl.left_side_bearing) + 0.5;
		bmp_y = y + gl.y0 + (int32)m.height - (int32)m.descent;
				
		ry = self->raster.h - bmp_y;
		height = gl.height;
		for (yy = 0; yy < height; yy++) {
			int32 xxx = yy * gl.width;
			if (bmp_y + yy <= Y || bmp_y + yy > Y + H) {
				continue;
			}
			rx = bmp_x * 4 + (ry - yy) * lw;
			for (xx = 0; xx < gl.width; xx++) {
				uint8 q = gl.data[xxx];
				uint32 o;
				uint32 no;

				if (q == 0 && bmp_x + xx < X || bmp_x + xx >= X + W) {
					rx += 4;
					xxx++;
					continue;
				}
				if (q > 0xA0) q = 0xFF;

				o = a * q / 0xFF;
				no = 0xFF - o;
	
				p[rx] = (p[rx] * no + r * o) / 0xff;
				rx++;
				p[rx] = (p[rx] * no + g * o) / 0xff;
				rx++;
				p[rx] = (p[rx] * no + b * o) / 0xff;
				rx++;
				p[rx] = (p[rx] * no + a * o) / 0xff;
				rx++;
				xxx++;
			}
		}
		default_font->_->release_glyph(e__, default_font, &gl);
		//i++;
		if (String__nonspacing(ucs) == 0) {
			x += gl.advance;
		}
	} 
}

static void stroke_text(Ex **e__, gfx__Canvas *self, String *text,
		float32 x, float32 y, float32 max_width)
{
}

static void measure_text(Ex **e__, gfx__Canvas *self, String *text,
		gfx__Font_metrics *m)
{
	uint8 r, g, b;
	uint32 a;
	uint8 *p;
	int32 lw;
	int32 X, Y, W, H;
	int32 rx, ry;
	gfx__Font_glyph gl;
	String n;
	int32 xx, yy;
	int32 i, l;
	utf8 *bf;
	STR(n,"Tuffy");

	l = text->_->get_size(e__, text);
	bf = text->_->get_cstr(e__, text);
	i = 0;

	default_font->_->get_metrics(e__, default_font, ' ', 
		default_font->size, m);

	m->width = 0;
	while (i < l) {
		int32 len;
		int32 ucs;

		len = String__utf2ucs(bf + i, l - i, &ucs);

		if (len > 0) {
			i += len;
		} else {
			i++;
		}

		default_font->_->get_glyph(e__, default_font, ucs, 
			default_font->size, &gl);
		//i++;
		if (String__nonspacing(ucs) == 0) {
			m->width += gl.advance;
		}
	} 
}

static void draw_image(Ex **e__, gfx__Canvas *self,
		gfx__Image *source,
		float32 sx, float32 sy, float32 sw, float32 sh,
		float32 dx, float32 dy, float32 dw, float32 dh)
{
	uint8 *p;
	int32 lw;
	int32 rx, ry;
	int32 xx, yy;
	int32 i, l;
	int32 X = dx;
	int32 Y = dy;
	int32 W = dw;
	int32 H = dh;
	int32 bmp_x = dx;
	int32 bmp_y = dy;
	lw = self->raster.scanline_width;
	p = &self->raster.data[0];

	
	i = 0;
	clip_dimension(e__, self, &X, &Y, &W, &H);
	
	ry = self->raster.h - dy;
		for (yy = 0; yy < source->h; yy++) {
			int32 xxx = yy * source->w * 4;
			if (bmp_y + yy <= Y || bmp_y + yy > Y + H) {
				continue;
			}
			rx = dx;
			rx = rx * 4 + (ry - yy) * lw;
			for (xx = 0; xx < source->w; xx++) {
				uint8 r = source->rgba[xxx++];
				uint8 g = source->rgba[xxx++];
				uint8 b = source->rgba[xxx++];
				uint32 a = source->rgba[xxx++];
				uint32 na = 0xFF - a;

				if (bmp_x + xx < X || bmp_x + xx >= X + W) {
					rx += 4;
					continue;
				}
				p[rx] = (p[rx] * na + r * a) / 
					(0xFF);
				rx++;
				p[rx] = (p[rx] * na + g * a) / 
					(0xFF);
				rx++;
				p[rx] = (p[rx] * na  + b * a) / 
					(0xFF);
				rx++;
				p[rx] = (p[rx] * na + a * a) / 
					(0xFF);
				rx++;
			}
		}
		

}

static void get_image(Ex **e__, gfx__Canvas *self,
		float32 sx, float32 sy, float32 sw, float32 sh,
		gfx__Image **dest)
{
}

INIT_EXPORT gfx__Canvas__class gfx__Canvas__  = {
	// Object
	(utf8*)"gfx__Canvas",
	sizeof(gfx__Canvas),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Canvas
	set_context,

	save_state,
	restore_state,

	scale,
	rotate,
	translate,
	transform,
	set_transform,
	
	clear_rect,
	fill_rect,
	stroke_rect,
	
	begin_path,
	close_path,
	move_to,
	line_to,
	quadratic_curve_to,
	bezier_curve_to,
	arc_to,
	rect,
	arc,
	fill,
	stroke,
	clip,
	is_point_in_path,
	fill_text,
	stroke_text,
	measure_text,
	draw_image,
	get_image,

};
