/* "$Id: $"
 *
 *                          28 october MMX PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Object.h>

#ifndef GFX__STYLE_H
#define GFX__STYLE_H

#define GFX__STYLE_DISPLAY_NONE			0
#define GFX__STYLE_DISPLAY_INHERIT		0xFFFFFFFF
#define GFX__STYLE_DISPLAY_INLINE		1
#define GFX__STYLE_DISPLAY_BLOCK		2
#define GFX__STYLE_DISPLAY_LIST_ITEM		3
#define GFX__STYLE_DISPLAY_RUN_IN		4
#define GFX__STYLE_DISPLAY_COMPACT		5
#define GFX__STYLE_DISPLAY_MARKER		6
#define GFX__STYLE_DISPLAY_TABLE		7
#define GFX__STYLE_DISPLAY_INLINE_TABLE		8
#define GFX__STYLE_DISPLAY_TABLE_ROW_GROUP	9
#define GFX__STYLE_DISPLAY_TABLE_HEADER_GROUP	10
#define GFX__STYLE_DISPLAY_TABLE_FOOTER_GROUP	11
#define GFX__STYLE_DISPLAY_TABLE_ROW		12
#define GFX__STYLE_DISPLAY_TABLE_COLUMN_GROUP	13
#define GFX__STYLE_DISPLAY_TABLE_COLUMN		14
#define GFX__STYLE_DISPLAY_TABLE_CELL		15
#define GFX__STYLE_DISPLAY_TABLE_CAPTION	16
#define GFX__STYLE_DISPLAY_WINDOW		17
#define GFX__STYLE_DISPLAY_FRAMESET		18
#define GFX__STYLE_DISPLAY_FRAME		19
#define GFX__STYLE_DISPLAY_NOFRAMES		20
#define GFX__STYLE_DISPLAY_IFRAME		21
#define GFX__STYLE_DISPLAY_LINK			22
#define GFX__STYLE_DISPLAY_UNORDERED_LIST	23
#define GFX__STYLE_DISPLAY_ORDERED_LIST		24
#define GFX__STYLE_DISPLAY_DEFINITION_LIST	25
#define GFX__STYLE_DISPLAY_DEFINITION_TERM	26
#define GFX__STYLE_DISPLAY_DEFINITION_DESCRIPTION	27
#define GFX__STYLE_DISPLAY_FORM			28
#define GFX__STYLE_DISPLAY_INPUT_TEXT		29
#define GFX__STYLE_DISPLAY_INPUT_PASSWORD	30
#define GFX__STYLE_DISPLAY_INPUT_CHECKBOX	31
#define GFX__STYLE_DISPLAY_INPUT_RADIO		32
#define GFX__STYLE_DISPLAY_INPUT_SUBMIT		33
#define GFX__STYLE_DISPLAY_INPUT_RESET		34
#define GFX__STYLE_DISPLAY_INPUT_FILE		35
#define GFX__STYLE_DISPLAY_INPUT_HIDDEN		36
#define GFX__STYLE_DISPLAY_INPUT_IMAGE		37
#define GFX__STYLE_DISPLAY_INPUT_BUTTON		38
#define GFX__STYLE_DISPLAY_BUTTON		39
#define GFX__STYLE_DISPLAY_SELECT		40
#define GFX__STYLE_DISPLAY_OPTGROUP		41
#define GFX__STYLE_DISPLAY_OPTION		42
#define GFX__STYLE_DISPLAY_TEXTAREA		43
#define GFX__STYLE_DISPLAY_LABEL		44
#define GFX__STYLE_DISPLAY_OBJECT		45
#define GFX__STYLE_DISPLAY_IMAGE		46
#define GFX__STYLE_DISPLAY_APPLET		47
#define GFX__STYLE_DISPLAY_MAX_STYLE		48

#define GFX__STYLE_PERCENTAGE_MIN	0.0000000000000000000000000000000000000000000014012984643248171
#define GFX__STYLE_PERCENTAGE_MAX	0.00000000000000000000000000000000000000018367099231598242
#define GFX__STYLE_NONE			0.0
#define GFX__STYLE_INHERIT		0.0000000000000000000000000000000000000014693679385278594


#define GFX__STYLE_BORDER_STYLE_NONE		0
#define GFX__STYLE_BORDER_STYLE_INHERIT		0xFFFFFFFF
#define GFX__STYLE_BORDER_STYLE_HIDDEN		2
#define GFX__STYLE_BORDER_STYLE_DOTTED		3
#define GFX__STYLE_BORDER_STYLE_DASHED		4
#define GFX__STYLE_BORDER_STYLE_SOLID		5
#define GFX__STYLE_BORDER_STYLE_DOUBLE		6
#define GFX__STYLE_BORDER_STYLE_GROOVE		7
#define GFX__STYLE_BORDER_STYLE_RIDGE		8
#define GFX__STYLE_BORDER_STYLE_INSET		9
#define GFX__STYLE_BORDER_STYLE_OUTSET		10


typedef struct gfx__Style_css {
	struct gfx__Style_css *parent;
	struct gfx__Style_css *next;
 
	float32 left;
	float32 top;
	float32 right;
	float32 bottom;
	float32 width;
	float32 height;
	float32 max_width;
	float32 max_height;
	float32 min_width;
	float32 min_height;

	float32 azimuth;
	uint8 background;
	uint8 background_attachment;
	uint32 background_color;
	utf8 *background_image;
	float32 background_position_horizontal;
	float32 background_position_vertical;
	uint8 background_repeat;

	int32 padding_left;
	int32 padding_top;
	int32 padding_right;
	int32 padding_bottom;

	uint8 border;
	uint8 border_collapse;
	int32 border_spacing_horizontal;
	int32 border_spacing_vertical;
	int32 border_left_width;
	int32 border_top_width;
	int32 border_right_width;
	int32 border_bottom_width;
	uint32 border_left_color;
	uint32 border_top_color;
	uint32 border_right_color;
	uint32 border_bottom_color;
	uint8 border_left_style;
	uint8 border_top_style;
	uint8 border_right_style;
	uint8 border_bottom_style;
	int32 margin_left;
	int32 margin_top;
	int32 margin_right;
	int32 margin_bottom;

	uint8 caption_side;
	uint8 clear;
	int32 clip_left;
	int32 clip_top;
	int32 clip_right;
	int32 clip_bottom;
	
	uint32 color;
	uint8 content;
	utf8 *content_string;
	utf8 *counter_identifier;
	int32 counter_increment;
	utf8 *counter_reset_identifier;
	int32 counter_reset_increment;
	uint8 cue;
	utf8* cue_after;
	utf8* cue_before;
	uint32 cursor;
	utf8* cursor_uri;

	uint8 direction;
	uint32 display;
	float32 elevation;
	uint8 empty_cells;
	uint8 float_;
	uint8 font;
	utf8 *font_family;
	float32 font_size;
	float32 font_size_adjust;
	uint8 font_stretch;
	uint8 font_style;
	uint8 font_variant;
	uint8 font_weight;
	
	float32 letter_spacing;
	float32 line_height;
	uint8 line_style;
	utf8* line_style_image;
	uint8 list_style_position;
	uint8 list_style_type;
	
	float32 marker_offset;
	uint8 marks;
	
	int32 orphans;
	uint8 outline;
	uint32 outline_color;
	uint8 outline_style;
	float32 outline_width;
	uint8 overflow;
	
	utf8 *page;
	uint8 page_break_after;
	uint8 page_break_before;
	uint8 page_break_inside;
	float32 pause_after;
	float32 pause_before;
	float32 pitch;
	float32 pitch_range;
	utf8 *play_during;
	uint8 position;
	
	utf8 *quotes_before;
	utf8 *quotes_after;
	
	int32 richness;
	float32 size_width;
	float32 size_height;
	uint8 speak;
	uint8 speak_header;
	uint8 speak_numeral;
	uint8 speak_punctuation;
	float32 speech_rate;
	float32 stress;

	uint8 table_layout;
	uint8 text_align;
	utf8* text_align_string;
	uint8 text_decoration;
	float32 text_indent;
	uint32 text_shadow_color;
	float32 text_shadow_horizontal;
	float32 text_shadow_vertical;
	float32 text_shadow_blur;
	uint8 text_transform;
	uint8 unicode_bidi;
	float32 vertical_align;
	uint8 visibility;
	utf8 *voice_familiy;
	float32 volume;
	uint8 white_space;
	int32 windows;
	float32 word_spacing;
	int32 z_index;
} gfx__Style_css;

typedef struct gfx__Style {
	// Object
	struct gfx__Style__class *_;

	// Style
	gfx__Style_css *root;
} gfx__Style;

struct gfx__Box;

typedef struct gfx__Style__class {
	// Object
	utf8 *__name__;
	uint32 __size__;
	struct Object__class *super;
	void (*__init__)(Ex **e__);
	void (*__ctor__)(Ex **e__, gfx__Style *self);
	void (*__dtor__)(Ex **e__, gfx__Style *self);
	uint32 (*hash_code)(Ex **e__, gfx__Style *self); 
	void (*to_string)(Ex **e__, gfx__Style *self,
		struct String *out);

	// Style
	void (*create)(Ex **e__, gfx__Style *self,
		uint32 display, struct gfx__Box *box);
} gfx__Style__class; 

EXPORT gfx__Style__class gfx__Style__;

#endif // GFX__STYLE_H

