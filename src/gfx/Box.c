/* "$Id: $"
 *
 *                          5 december MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */


#include <gfx/Box.h>
#include <gfx/IEventHandler.h>
#include <gfx/Canvas.h>
#include <gfx/Window.h>

static gfx__IEventHandler__class gfx__IEventHandler__;


static void __init__(Ex **e__)
{
	(void)e__;
	gfx__Box__.super = &Object__;
	INIT_INTERFACE(gfx__IEventHandler, gfx__Box);

	return;
}

static void __ctor__(Ex **e__, gfx__Box *self)
{
	F("__ctor__");

	self->content = 0;
	self->length = 0;
	self->flags = 0;
	self->style = 0;
	self->lines = 0;
	self->nb_lines = 0;
	self->descent = 0;
	self->height = 0;
	self->width = 0;
	self->left = 0;
	self->top = 0;
	CHECK(error1);
	return;	
error1:
	return;
}

void delete_lines(Ex **e__, gfx__Box *self)
{
	if (self->nb_lines <= 0) return;
	while (self->nb_lines > 0) {
		self->nb_lines--;
		delete_(self->lines[self->nb_lines]);
	}
	Gc__free(e__, self->lines);
}


static void __dtor__(Ex **e__, gfx__Box *self)
{
	F("__dtor__");
	(void)e__;
	(void)self;
	(void)__func__;
	delete_lines(e__, self);
	return;
}

static uint32 hash_code(Ex **e__, gfx__Box *self)
{
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
	e__;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, gfx__Box *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void gfx__i_event_handler(Ex **e__, gfx__Box *self, 
	gfx__IEventHandler *out)
{
	(void)e__;
	out->_ = &gfx__IEventHandler__;
	out->__self__ = (Object*)self;	
}

void gfx__Box__draw_background_and_border(Ex **e__, gfx__Box *self, gfx__Event *evt)
{

	evt->canvas->_->save_state(e__, evt->canvas);
	evt->canvas->_->begin_path(e__, evt->canvas);
	evt->canvas->_->rect(e__, evt->canvas, evt->r.x, evt->r.y, evt->r.w, evt->r.h);
	evt->canvas->_->clip(e__, evt->canvas);

	if (self->style->background_color & 0xFF000000) {
		evt->canvas->state.fill_style = self->style->background_color;
		evt->canvas->_->fill_rect(e__, evt->canvas, evt->r.x, evt->r.y,
			self->width, self->height);
	}

	evt->canvas->_->restore_state(e__, evt->canvas);
}

void gfx__Box__iterate_content(Ex **e__, gfx__Box *self,
	int32 *index, String *str, gfx__Box **box, int32 max_index)
{
	int32 dx, dy;
	int32 i, l;
	utf8 *c;

	c = self->content + *index;
	l = max_index - *index;
	i = 0;
	str->size = 0;
	*box = 0;
	while (i < l) {
		if (c[i] == 0xFF) {
			if (i != 0) {
				STRN(*str, c, i);
				*index += i;
				c += i;
				l -= i;
			}
			if (l >= sizeof(void*) + 2 && 
				c[sizeof(void*) + 1] == 0xFE)
			{
				unsigned long q = 0;
				int32 j, s;
				s = 0;
				for (j = 1; j <= sizeof(void*); j++) {
					q |= ((unsigned long)c[j] << s);
					s += 8;
				}
				*box = (gfx__Box*)q;
				*index += 2 + sizeof(void*);
				return;
			} else	{
				*index -= i;
				c -= i;
				l += i;
			}
		}
		i++;
	}
	if (i != 0) {
		STRN(*str, c, i);
		*index += i;
	}
}

static void draw(Ex **e__, gfx__Box *self, gfx__Event *evt)
{
	int32 dx, dy;
	int32 i, l;
	utf8 *c;
	String s;
	gfx__Box *box;
	int32 base;
	static gfx__Image *img = 0;
	gfx__Event_rect r;
	int32 line = -1;
	int32 next_index = 0;
	gfx__Box__draw_background_and_border(e__, self, evt);

	r = evt->r;

	evt->canvas->_->save_state(e__, evt->canvas);
	evt->canvas->_->begin_path(e__, evt->canvas);
	evt->canvas->_->rect(e__, evt->canvas, evt->r.x, evt->r.y, evt->r.w, evt->r.h);
	evt->canvas->_->clip(e__, evt->canvas);

	evt->canvas->state.fill_style = self->style->color;


	i = 0;
	do {
		gfx__Font_metrics m;

		if (i == next_index) {
			if (line + 2 < self->nb_lines) {
				next_index = self->lines[line + 2]->start;
			} else {
				next_index = self->length;
			}
			line++;
			evt->r.x = r.x;
			evt->r.y = r.y + self->lines[line]->top;
			base = evt->r.y + self->lines[line]->height -
				self->lines[line]->descent;
		}
		gfx__Box__iterate_content(e__, self, &i, &s, &box, next_index);
		if (s.size != 0) {
			evt->canvas->_->measure_text(e__, evt->canvas, 
				&s, &m);
			evt->canvas->_->fill_text(e__, evt->canvas, 
				&s, evt->r.x , base, self->width);
			evt->r.x += m.width;
		}
		if (box != 0) {
			gfx__IEventHandler ieh;
			box->_->gfx__i_event_handler(e__, box, &ieh);
			ieh._->event(e__, &ieh, evt);
			evt->r.x += box->width;
		}
	} while (i < self->length);
	evt->r = r;

#if 0
	if (img == 0) {
		String nam;

		STR(nam, "test.gif");
		img = new_(gfx__Image);
		img->_->load(e__, img, &nam);
	}
	evt->canvas->_->draw_image(e__, evt->canvas, img, 0, 0, 
		img->w, img->h, x-5, y-5, img->w, img->h);


	STR(s, "A");

	evt->canvas->state.fill_style = RGBA(0xFF, 0x00, 0x00, 0xFF);//= self->style->color;
	evt->canvas->_->fill_text(e__, evt->canvas, &s, x + 10, y + 5, 1000);
	evt->canvas->_->fill_text(e__, evt->canvas, &s, x + 10, y + h + 6, 1000);	
	evt->canvas->_->fill_text(e__, evt->canvas, &s, x -5, y + h, 1000);	
	evt->canvas->_->fill_text(e__, evt->canvas, &s, x, y + h + 2, 1000);

	STR(s, "mega master mix basssssss for you and for me");
	evt->canvas->state.fill_style = self->style->color;
	evt->canvas->_->fill_text(e__, evt->canvas, &s, x - 10, y + h - 1, 1000);
	
	evt->canvas->_->draw_image(e__, evt->canvas, img, 0, 0, 
		img->w, img->h, x + w - 15, y + h - 5, img->w, img->h);
#endif

	evt->canvas->_->restore_state(e__, evt->canvas);
	evt->handled = 1;
}

static void measure_text(Ex **e__, gfx__Box *self, 
	gfx__Event *evt, String *txt)
{
	String s;
	int32 l;
	float32 spc_width = 0.0;

	STRN(s, txt->cstr, 0);
	l = txt->size;	

	do {
		gfx__Font_metrics m;

		while (s.cstr[s.size] != ' ' && l > 0) {
			s.size++;
			l--;
		}

		evt->canvas->_->measure_text(e__, evt->canvas, 
				&s, &m);
		if (m.width > self->min_width) {
			self->min_width = m.width;
		}
		self->max_width += spc_width + m.width;

		if (m.descent > self->descent) {
			self->height += m.descent - self->descent;
			self->descent = m.descent;
		}
		if (m.height > self->height) {
			self->height = m.height;
		}

		while (s.cstr[s.size] == ' ' && l > 0) {
			s.size++;
			l--;
		}
		s.cstr += s.size;

		if (l > 0 && spc_width == 0.0) {
			String spc;

			STRN(spc, " ", 1);
			evt->canvas->_->measure_text(e__, evt->canvas, 
				&spc, &m);
			spc_width = m.width;
		}
	} while (l > 0);
}

static void measure(Ex **e__, gfx__Box *self, gfx__Event *evt)
{
	int32 i;
	String s;
	gfx__Box *box;
	gfx__Event_rect r;

	delete_lines(e__, self);

	self->min_width = 0;
	self->max_width = 0;
	self->height = 0;
	self->descent = 0;
	i = 0;
	do {
		gfx__Box__iterate_content(e__, self, &i, &s, &box, self->length);
		if (s.size != 0) {
			measure_text(e__, self, evt, &s);
		}
		if (box != 0) {
			gfx__IEventHandler ieh;
			int32 min_width;
			int32 max_width;
			int32 height;
			int32 descent;

			box->_->gfx__i_event_handler(e__, box, &ieh);
			ieh._->event(e__, &ieh, evt);
			min_width = box->style->margin_left +
				box->style->margin_right + 
				box->style->border_left_width +
				box->style->border_right_width +
				box->style->padding_left +
				box->style->padding_right;
			max_width = min_width;
			height = box->style->margin_top +
				box->style->margin_bottom + 
				box->style->border_top_width +
				box->style->border_bottom_width +
				box->style->padding_top +
				box->style->padding_bottom;
			min_width += box->min_width;
			max_width += box->max_width;
			height += box->height;
			descent = box->descent;
			if (min_width > self->min_width) {
				self->min_width = min_width;
			}
			self->max_width += max_width;
			if (descent > self->descent) {
				self->height += descent - self->descent;
				self->descent = descent;
			}
			if (height > self->height) {
				self->height = height;
			}
		}
	} while (box != 0);

	if (self->style->width >= 0) {
		self->width = self->style->width;
	}
	if (self->style->height >= 0) {
		self->height = self->style->height;
	}

}



void add_line(Ex **e__, gfx__Box *self)
{
	int32 i = self->nb_lines;
	i++;

	if ((self->nb_lines & i) == 0) {
		gfx__Line **na;
		int32 j;

		na = (gfx__Line**)Gc__malloc(e__, 
			sizeof(gfx__Line*) * (i << 1));
		j = self->nb_lines;
		while (j > 0) {
			j--;
			na[j] = self->lines[j];
		}
		if (i != 1) Gc__free(e__, self->lines);
		self->lines = na;
	}
	self->lines[self->nb_lines] = new_(gfx__Line);
	self->nb_lines++;
}

static void format_text(Ex **e__, gfx__Box *self, 
	gfx__Event *evt, String *txt, gfx__Line **pline, int32 i)
{
	String s;
	int32 l;
	int32 spc = 0;
	int32 spc_width = 0;
	gfx__Line *line = *pline;

	STRN(s, txt->cstr, 0);
	l = txt->size;	

	do {
		gfx__Font_metrics m;

		while (s.cstr[s.size] != ' ' && l > 0) {
			s.size++;
			l--;
		}

		evt->canvas->_->measure_text(e__, evt->canvas, 
				&s, &m);

		m.width += 0.5;
		if (evt->r.x + m.width + spc > evt->r.w && evt->r.x != 0) {
			int32 t;

			t = line->top + line->height;
			if (line->width > self->width) {
				self->width = line->width;
			}
			add_line(e__, self);
			line = self->lines[self->nb_lines - 1];
			line->start = i;
			line->top = t;
			evt->r.x = 0;
			spc = 0;
		}
		evt->r.x += m.width + spc;
		line->width += m.width + spc;
		spc = 0;
		if (m.descent > line->descent) {
			line->height += m.descent - line->descent;
			line->descent = m.descent;
		}
		if (m.height > line->height) {
			line->height = m.height;
		}			

		if (l > 0 && spc_width == 0) {
			String spc;

			STRN(spc, " ", 1);
			evt->canvas->_->measure_text(e__, evt->canvas, 
				&spc, &m);
			spc_width = m.width + 0.5;
		}

		while (s.cstr[s.size] == ' ' && l > 0) {
			s.size++;
			l--;
			spc += spc_width;
		}
		s.cstr += s.size;
		i += s.size;
		s.size = 0;
	} while (l > 0);
	*pline = line;
}

static void format(Ex **e__, gfx__Box *self, gfx__Event *evt)
{
	int32 i;
	String s;
	gfx__Box *box;
	gfx__Line *line;

	self->width = 0;
	self->height = 0;
	self->descent = 0;

	i = 0;
	add_line(e__, self);
	line = self->lines[self->nb_lines - 1];
	line->start = 0;
	line->top = 0;
	do {
		int32 li = i;
		gfx__Box__iterate_content(e__, self, &i, &s, &box, self->length);
		if (s.size != 0) {
			format_text(e__, self, evt, &s, &line, li);
		}
		if (box != 0) {
			gfx__IEventHandler ieh;
			gfx__Event_rect r;

			r = evt->r;
			evt->r.x = 0;
			evt->r.w -= 
				box->style->margin_left +
				box->style->margin_right + 
				box->style->border_left_width +
				box->style->border_right_width +
				box->style->padding_left +
				box->style->padding_right;

			box->_->gfx__i_event_handler(e__, box, &ieh);
			ieh._->event(e__, &ieh, evt);
			if (r.x + box->width > r.w && r.x != 0) {
				int32 t;

				t = line->top + line->height;
				if (line->width > self->width) {
					self->width = line->width;
				}
				add_line(e__, self);
				line = self->lines[self->nb_lines - 1];
				line->start = i - (sizeof(void*) + 2);
				line->top = t;
				r.x = 0;
			}
			evt->r = r;
			evt->r.x += box->width;
			line->width += box->width;
			if (box->descent > line->descent) {
				line->height += box->descent - line->descent;
				line->descent = box->descent;
			}
			if (box->height > line->height) {
				line->height = box->height;
			}			
		}
	} while (box != 0);
	if (line->width > self->width) {
		self->width = line->width;
	}
	self->descent = line->descent;
	self->height = line->top + line->height;

	if (self->style->width >= 0) {
		self->width = self->style->width;
	}
	if (self->style->height >= 0) {
		self->height = self->style->height;
	}

}

static void insert_text(Ex **e__, gfx__Box *self, gfx__Event *evt)
{

	utf8 *content;
	int32 length;
	int32 i, j;
	utf8 *o;
	int32 l;
	utf8 *n;
	int32 nl;

	n = evt->text->_->get_cstr(e__, evt->text);
	nl = evt->text->_->get_size(e__, evt->text);

	length = nl + self->length;
	content = (utf8*)Gc__malloc(e__, length + 1);

	o = self->content;
	l = self->length;
	
	i = evt->window->selection_start_index;
	while (i > 0) {
		i--;
		content[i] = o[i];
	}
	i = evt->window->selection_start_index;
	j = i + nl;
	if (l == 0) {
		content[j] = 0;
	} else { 
		while (i <= l) {
			content[j] = o[i];
			j++;
			i++;
		}
	}

	i = 0;
	j = evt->window->selection_start_index;
	while (i < nl) {
		content[j] = n[i];
		j++;
		i++;
	}

	if (self->content) Gc__free(e__, self->content);
	self->length = length;
	self->content = content;
	evt->window->selection_start_index += nl;
	evt->window->selection_end_index = 
		evt->window->selection_start_index;
}


static void init_style(Ex **e__, gfx__Box *self, gfx__Event *evt)
{


}

static void default_handler(Ex **e__, gfx__Box *self, gfx__Event *evt)
{
	switch (evt->id) {
	case GFX__EVENT_DRAW:
		draw(e__, self, evt);
		break;
	case GFX__EVENT_MEASURE:
		measure(e__, self, evt);
		break;
	case GFX__EVENT_FORMAT:
		format(e__, self, evt);
		break;
	case GFX__EVENT_INSERT_TEXT:
		insert_text(e__, self, evt);
		break;
	case GFX__EVENT_INIT_STYLE:
		init_style(e__, self, evt);
		break;
	default:
		evt->handled = 0;
	}
}

static void event(Ex **e__, gfx__IEventHandler *itf, gfx__Event *evt)
{
	gfx__Box *self;
	uint32 d;

	self = (gfx__Box*)itf->__self__;
	default_handler(e__, self, evt);
}


INIT_EXPORT gfx__Box__class gfx__Box__  = {
	// Object
	(utf8*)"gfx__Box",
	sizeof(gfx__Box),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Box
	gfx__i_event_handler,
	event
};


static gfx__IEventHandler__class gfx__IEventHandler__  = {
	// Object
	0, 0, 0, 0, 0, 0, 0, 0,

	// IEventHandler
	event
};
