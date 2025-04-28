/* "$Id: $"
 *
 *                          5 december MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */


#include <gfx/InputText.h>

static gfx__IEventHandler__class gfx__IEventHandler__;

static void __init__(Ex **e__)
{
	(void)e__;
	gfx__InputText__.super = &gfx__Box__;
	INIT_INTERFACE(gfx__IEventHandler, gfx__InputText);
	ASSIGN_FUNC(gfx__Box, hash_code, gfx__InputText);
	ASSIGN_FUNC(gfx__Box, to_string, gfx__InputText);
	return;
}

static void __ctor__(Ex **e__, gfx__InputText *self)
{
	F("__ctor__");

	return;
}


static void __dtor__(Ex **e__, gfx__InputText *self)
{
	F("__dtor__");
	(void)e__;
	(void)self;
	(void)__func__;
	return;
}

static void gfx__i_event_handler(Ex **e__, gfx__InputText *self, 
	gfx__IEventHandler *out)
{
	(void)e__;
	out->_ = &gfx__IEventHandler__;
	out->__self__ = (Object*)self;	
}

static void init_style(Ex **e__, gfx__InputText *self, gfx__Event *evt)
{
	gfx__Style_css *s;

	s = self->style;
	
	s->border_left_style = GFX__STYLE_BORDER_STYLE_INSET;
	s->border_right_style = GFX__STYLE_BORDER_STYLE_INSET;
	s->border_top_style = GFX__STYLE_BORDER_STYLE_INSET;
	s->border_bottom_style = GFX__STYLE_BORDER_STYLE_INSET;
	s->border_left_width = 2;
	s->border_right_width = 2;
	s->border_top_width = 2;
	s->border_bottom_width = 2;
	s->width = 100;
	s->height = 100;
	s->background_color = RGBA(0xFF, 0x00, 0xFF, 0xFF);
	s->color = RGBA(0x00, 0xFF, 0x00, 0xFF);

}

static void draw(Ex **e__, gfx__InputText *self, gfx__Event *evt)
{
	String s;
	gfx__Canvas *c;
	int32 x;
	int32 y;
	static float32 g = 0.0;

	gfx__Box__draw_background_and_border(e__, 
		(gfx__Box*)self, evt);

	STRN(s, self->content, self->length);
	evt->canvas->state.fill_style = self->style->color;
	evt->canvas->_->fill_text(e__, evt->canvas, 
			&s, evt->r.x, evt->r.y + 30, 1000);

// FIXME
	c = evt->canvas;
	x = evt->r.x;
	y = evt->r.y;
	c->_->rotate(e__, c, g);
	g += 0.1;
	c->_->begin_path(e__, c);
	c->_->move_to(e__, c, x + 10.0, y + 10.0);
	c->_->line_to(e__, c, x + 70.0, y + 10.0);
	c->_->line_to(e__, c, x + 70.0, y + 70.0);
	c->_->line_to(e__, c, x + 10.0, y + 70.0);
	c->_->line_to(e__, c, x + 10.0, y + 10.0);

	c->_->move_to(e__, c, x + 20.0, y + 20.0);
	c->_->line_to(e__, c, x + 20.0, y + 60.0);
	c->_->line_to(e__, c, x + 60.0, y + 60.0);
	c->_->line_to(e__, c, x + 60.0, y + 20.0);
	c->_->line_to(e__, c, x + 20.0, y + 20.0);

	c->_->close_path(e__, c);
	c->_->fill(e__, c);
}


static void event(Ex **e__, gfx__IEventHandler *itf, gfx__Event *evt)
{
	gfx__InputText *self;

	self = (gfx__InputText*)itf->__self__;

	switch (evt->id) {
	case GFX__EVENT_INIT_STYLE:
		init_style(e__, self, evt);		
		break;
	case GFX__EVENT_DRAW:
		draw(e__, self, evt);
		break;
	default:
		gfx__Box__.event(e__, itf, evt);
		break;
	} 
}

INIT_EXPORT gfx__InputText__class gfx__InputText__  = {
	// Object
	(utf8*)"gfx__InputText",
	sizeof(gfx__InputText),
	0,
	__init__,
	__ctor__,
	__dtor__,
	0,
	0,

	// Box
	gfx__i_event_handler,
	event,
	
	// InputText
};

static gfx__IEventHandler__class gfx__IEventHandler__  = {
	// Object
	0, 0, 0, 0, 0, 0, 0, 0,

	// IEventHandler
	event
};