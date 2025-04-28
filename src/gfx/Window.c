/* "$Id: $"
 *
 *                          5 december MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */


#include <gfx/Window.h>
#include <gfx/InputText.h>

static gfx__IEventHandler__class gfx__IEventHandler__;

static void __init__(Ex **e__)
{
	(void)e__;
	gfx__Window__.super = &gfx__Box__;
	gfx__Window__.event = gfx__Box__.event;
	INIT_INTERFACE(gfx__IEventHandler, gfx__Window);
	ASSIGN_FUNC(gfx__Box, hash_code, gfx__Window);
	ASSIGN_FUNC(gfx__Box, to_string, gfx__Window);
	return;
}

static void __ctor__(Ex **e__, gfx__Window *self)
{
	F("__ctor__");

	self->style_factory = new_(gfx__Style);
	CHECK(error1);
	self->style_factory->_->create(e__, self->style_factory,
		GFX__STYLE_DISPLAY_WINDOW, (gfx__Box*)self);
	self->parent = 0;

	self->selection_start_box = (gfx__Box*)self;
	self->selection_end_box = (gfx__Box*)self;
	self->selection_start_index = 0;
	self->selection_end_index = 0;

	self->evt = new_(gfx__Event);
	self->evt->window = self;
	self->evt2 = new_(gfx__Event);
	self->evt2->window = self;

	return;	
error1:
	return;
}


static void __dtor__(Ex **e__, gfx__Window *self)
{
	F("__dtor__");
	(void)e__;
	(void)self;
	(void)__func__;
	return;
}

static void gfx__i_event_handler(Ex **e__, gfx__Window *self, 
	gfx__IEventHandler *out)
{
	(void)e__;
	out->_ = &gfx__IEventHandler__;
	out->__self__ = (Object*)self;	
}

static void insert_text(Ex **e__, gfx__Window *self, String *text)
{
	gfx__IEventHandler h;
	self->evt->text = text;
	self->evt->id = GFX__EVENT_INSERT_TEXT;
	self->selection_start_box->_->gfx__i_event_handler(e__, 
		self->selection_start_box, &h);
	h._->event(e__, &h, self->evt);
}

static void create_style(Ex **e__, gfx__Window *self, uint32 display, gfx__Box *b)
{
	gfx__IEventHandler h;

	self->style_factory->_->create(e__,
		self->style_factory,
		display, b);
	
	self->evt->id = GFX__EVENT_INIT_STYLE;
	b->_->gfx__i_event_handler(e__, b, &h);
	h._->event(e__, &h, self->evt);
}

static void create_box(Ex **e__, gfx__Window *self, uint32 display, gfx__Box **box)
{
	switch (display) {
	case GFX__STYLE_DISPLAY_INPUT_TEXT:
		*box = new_(gfx__InputText);
		break;
	default:
		*box = new_(gfx__Box);	
	}
	(*box)->parent = self->selection_start_box;
}

static gfx__Box *insert_box(Ex **e__, gfx__Window *self, uint32 display)
{
	gfx__IEventHandler h;
	gfx__Box *r = 0;
	utf8 buf[sizeof(void*) + 3];
	String s;
	int32 i;
	int32 shift = 0;


	create_box(e__, self, display, &r);
	create_style(e__, self, display, r);

	buf[0] = 0xFF;		
	for (i = 1; i <= sizeof(void*); i++) {
		buf[i] = (utf8)((long)r >> shift) & 0xFF;
		shift += 8;
	}
	buf[i] = 0xFE;
	
	STRN(s, buf, sizeof(void*) + 2);
	insert_text(e__, self, &s);
	return r;
}

static void size(Ex **e__, gfx__Window *self, int32 x, int32 y, 
	int32 w, int32 h)
{
	self->left = x;
	self->top = y;
	self->width = w;
	self->height = h;
}

void set_rectangle(Ex **e__, gfx__Window *self, gfx__Event *evt)
{
	int32 d;
	d = self->left - evt->r.x;
	if (d > 0) {
		evt->r.x = self->left;
		evt->r.w -= d;
	}
	d = self->top - evt->r.y;
	if (d > 0) {
		evt->r.y = self->top;
		evt->r.h -= d;
	}
	if (evt->r.w > self->width) {
		evt->r.w = self->width;
	}
	if (evt->r.h > self->height) {
		evt->r.h = self->height;
	}
}

static void event(Ex **e__, gfx__IEventHandler *itf, gfx__Event *evt)
{
	gfx__Window *self;
	uint32 d;
	int32 w, h;
	self = (gfx__Window*)itf->__self__;
	self->evt->window = self;

	switch (evt->id) {
	case GFX__EVENT_DRAW:
		w = self->width;
		h = self->height;
		*self->evt2 = *evt;
		self->evt2->id = GFX__EVENT_MEASURE;
		gfx__Box__.event(e__, itf, self->evt2);
		self->evt2->id = GFX__EVENT_FORMAT;
		self->evt2->r.x = 0;
		self->evt2->r.y = 0;
		self->evt2->r.w = self->width;
		self->evt2->r.h = self->height;		
		gfx__Box__.event(e__, itf, self->evt2);
		self->width = w;
		self->height = h;
		evt->handled = 0;
		set_rectangle(e__, self, evt);
		break;
	default:
		evt->handled = 0;
		break;
	} 
	if (evt->handled == 0) {
		evt->handled = 1;
		gfx__Box__.event(e__, itf, evt);
	}
}

INIT_EXPORT gfx__Window__class gfx__Window__  = {
	// Object
	(utf8*)"gfx__Window",
	sizeof(gfx__Window),
	0,
	__init__,
	__ctor__,
	__dtor__,
	0,
	0,

	// Box
	gfx__i_event_handler,
	event,
	
	// Window
	insert_box,
	insert_text,
	size,
};

static gfx__IEventHandler__class gfx__IEventHandler__  = {
	// Object
	0, 0, 0, 0, 0, 0, 0, 0,

	// IEventHandler
	event
};