/* "$Id: $"
 *
 *                          5 december MMIX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */


#include <lang/Object.h>

#ifndef GFX__CANVAS_H
#define GFX__CANVAS_H

#include <gfx/Image.h>
#include <gfx/Font.h>
#include <gfx/IRaster.h>

struct gfx__Image;

#define RGBA(r, g, b, a) (a << 24 | b << 16 | g << 8 | r)

//stroke_style fill_style shadow_color
#define GFX__CANVAS_BLACK 0xFF000000
#define GFX__CANVAS_TRANSPARENT 0x00000000

// line_cap
#define GFX__CANVAS_BUTT 0
#define GFX__CANVAS_ROUND 1
#define GFX__CANVAS_SQUARE 2

// line_join
#define GFX__CANVAS_MITER 0
#define GFX__CANVAS_ROUND 1
#define GFX__CANVAS_BEVEL 2

// text_align
#define GFX__CANVAS_START 0
#define GFX__CANVAS_END 1
#define GFX__CANVAS_LEFT 2
#define GFX__CANVAS_RIGHT 3
#define GFX__CANVAS_CENTER 4

// text_base_line
#define GFX__CANVAS_ALPHABETIC 0
#define GFX__CANVAS_TOP 1
#define GFX__CANVAS_HANGING 2
#define GFX__CANVAS_MIDDLE 3
#define GFX__CANVAS_IDEOGRAPHIC 4
#define GFX__CANVAS_BOTTOM 5


typedef struct gfx__Canvas_edge {
	int32 x;
	int32 top;
	int32 bottom;
	int32 inc;
	int32 grad_num;
	int32 grad_den;
	int32 winding;
	struct gfx__Canvas_edge *next;	
	struct gfx__Canvas_edge *prev;	
} gfx__Canvas_edge;

typedef struct gfx__Canvas_sorted_edges {
	int32 y;
	gfx__Canvas_edge *first;
	struct gfx__Canvas_sorted_edges *next;	
	struct gfx__Canvas_sorted_edges *prev;	
} gfx__Canvas_sorted_edges;


typedef struct gfx__Canvas_vertex {
	int32 x;
	int32 y;
	struct gfx__Canvas_vertex *next;	
} gfx__Canvas_vertex;

typedef struct gfx__Canvas_subpath {
	struct gfx__Canvas_subpath *next;
	gfx__Canvas_vertex *first;
	gfx__Canvas_vertex *last;
} gfx__Canvas_subpath;

typedef struct gfx__Canvas_path {
	float32 left;
	float32 top;
	float32 right;
	float32 bottom;
	gfx__Canvas_subpath *first;
	gfx__Canvas_subpath *last;	
} gfx__Canvas_path;

typedef struct gfx__Canvas_state {
	float32 global_alpha;
	
	uint32 stroke_style;
	uint32 fill_style;
	float32 line_width;
	uint8 line_cap;
	uint8 line_join;
	float32 mitter_limit;
	
	float32 shadow_offset_x;
	float32 shadow_offset_y;
	float32 shadow_blur;
	uint32 shadow_color;

	gfx__Font *font;
	uint8 text_align;
	uint8 text_baseline;

	float32 m11;
	float32 m12;
	float32 m21;
	float32 m22;
	float32 dx;
	float32 dy;

	gfx__Canvas_path clipping_region;
	struct gfx__Canvas_state *previous;
} gfx__Canvas_state;


typedef struct gfx__Canvas {
	// Object
	struct gfx__Canvas__class *_;

	// Canvas
	int32 w;
	int32 h;

	uint32 *rgba_line;
	int32 line_size;
	uint32 *fill_mask;
	uint32 *clip_mask;

	gfx__Canvas_state state;
	gfx__Canvas_path path;
	gfx__IRaster_info raster;
} gfx__Canvas;

typedef struct gfx__Canvas__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__Canvas *self);
	void (*__dtor__)(Ex **e__, gfx__Canvas *self);
	uint32 (*hash_code)(Ex **e__, gfx__Canvas *self); 
	void (*to_string)(Ex **e__, gfx__Canvas *self,
		struct String *out);

	// Canvas
	void (*set_context)(Ex **e__, gfx__Canvas *self, 
		struct gfx__IRaster *raster,
		int32 x, int32 y, int32 w, int32 h);

	void (*save_state)(Ex **e__, gfx__Canvas *self);
	void (*restore_state)(Ex **e__, gfx__Canvas *self);

	void (*scale)(Ex **e__, gfx__Canvas *self, float32 x, float32 y);
	void (*rotate)(Ex **e__, gfx__Canvas *self, float32 angle);
	void (*translate)(Ex **e__, gfx__Canvas *self, float32 x, float32 y);
	void (*transform)(Ex **e__, gfx__Canvas *self, float32 m11, 
		float32 m12, float32 m21, float32 m22, float32 dx, float32 dy);
	void (*set_transform)(Ex **e__, gfx__Canvas *self, float32 m11, 
		float32 m12, float32 m21, float32 m22, float32 dx, float32 dy);
	
	void (*clear_rect)(Ex **e__, gfx__Canvas *self, float32 x, float32 y,
		float32 w, float32 h);
	void (*fill_rect)(Ex **e__, gfx__Canvas *self, float32 x, float32 y,
		float32 w, float32 h);
	void (*stroke_rect)(Ex **e__, gfx__Canvas *self, float32 x, float32 y,
		float32 w, float32 h);
	
	void (*begin_path)(Ex **e__, gfx__Canvas *self);
	void (*close_path)(Ex **e__, gfx__Canvas *self);
	void (*move_to)(Ex **e__, gfx__Canvas *self, float32 x, float32 y);
	void (*line_to)(Ex **e__, gfx__Canvas *self, float32 x, float32 y);
	void (*quadratic_curve_to)(Ex **e__, gfx__Canvas *self, 
		float32 cpx, float32 cpy, float32 x, float32 y);
	void (*bezier_curve_to)(Ex **e__, gfx__Canvas *self, 
		float32 cp1x, float32 cp1y, float32 cp2x, float32 cp2y, 
		float32 x, float32 y);
	void (*arc_to)(Ex **e__, gfx__Canvas *self, 
		float32 x1, float32 y1, float32 x2, float32 y2, float32 radius);
	void (*rect)(Ex **e__, gfx__Canvas *self, float32 x, float32 y,
		float32 w, float32 h);
	void (*arc)(Ex **e__, gfx__Canvas *self, float32 x, float32 y,
			float32 radius, float32 start_angle, float32 end_angle,
			uint8 anticlockwise);
	void (*fill)(Ex **e__, gfx__Canvas *self);
	void (*stroke)(Ex **e__, gfx__Canvas *self);
	void (*clip)(Ex **e__, gfx__Canvas *self);
	uint8 (*is_point_in_path)(Ex **e__, gfx__Canvas *self,
		float32 x, float32 y);

	void (*fill_text)(Ex **e__, gfx__Canvas *self, String *text,
		float32 x, float32 y, float32 max_width);
	void (*stroke_text)(Ex **e__, gfx__Canvas *self, String *text,
		float32 x, float32 y, float32 max_width);
	void (*measure_text)(Ex **e__, gfx__Canvas *self, String *text,
		gfx__Font_metrics *text_metrics);

	void (*draw_image)(Ex **e__, gfx__Canvas *self,
		struct gfx__Image *source,
		float32 sx, float32 sy, float32 sw, float32 sh,
		float32 dx, float32 dy, float32 dw, float32 dh);
	void (*get_image)(Ex **e__, gfx__Canvas *self,
		float32 sx, float32 sy, float32 sw, float32 sh,
		struct gfx__Image **dest);
} gfx__Canvas__class; 

EXPORT gfx__Canvas__class gfx__Canvas__;

#endif // GFX__CANVAS_H

