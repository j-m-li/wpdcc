/* "$Id: $"
 *
 *                          12 August MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <cc/PreExpression.h>
#include <cc/PreProcessor.h>

#define P(a, b, c, d) ((a & 0xff)  | ((b << 8) & 0xff00) \
	| ((c << 16) & 0xff0000) | ((d << 24) & 0xff000000))

#define LEFT 1
#define RIGHT 0
#define NONE 2

static void compute(Ex **e__, cc__PreExpression *self, cc__PreTree *tree);

static void __init__(Ex **e__)
{
	(void)e__;

	cc__PreExpression__.super = &Object__;
	return;
}

static cc__PreTree *new_tree(Ex **e__)
{
	F("new_tree");
	cc__PreTree *tk;
	
	tk = (cc__PreTree*)Gc__malloc(e__, sizeof(cc__PreTree));
	CHECK(error);
	tk->parent = 0;
	tk->left = 0;
	tk->right = 0;
	tk->func = 0;
	tk->precedence = 0;
	tk->associativity = LEFT;
	
	return tk;
error:
	return 0;
}

static void __ctor__(Ex **e__, cc__PreExpression *self)
{
	F("__ctor__");

	self->root = new_tree(e__);
	CHECK(error);
	self->root->associativity = NONE; 
	self->current = self->root;
	self->env = 0;
error:
	return;
}


static void destroy(Ex **e__, cc__PreTree *t) 
{
	F("destroy");

	if (t == 0) return;

	destroy(e__, t->left);
	CHECK(error1);
error1:
	destroy(e__, t->right);
	CHECK(error2);
error2:
	Gc__free(e__, t);
	CHECK(error3);
error3:
	return;
}

static void __dtor__(Ex **e__, cc__PreExpression *self)
{
	F("__dtor__");

	destroy(e__, self->root);
	CHECK(error1);
error1:
	return;
}

static uint32 hash_code(Ex **e__, cc__PreExpression *self)
{
	(void)e__;

#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, cc__PreExpression *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void check_2_integer(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("check_2_integer");

	if (tree->left == 0) {
		THROW(15, "C compiler internal error.");
		CC__WARN("", tree->file, tree->line, tree->char_pos);
		CHECK(error);
	}
	if (tree->right == 0) {
		THROW(-100040, "Missing right operand.");
		CC__WARN("", tree->file, tree->line, tree->char_pos);
		CHECK(error);
	}
	if (tree->left->type != CC_NUMBER) {
		THROW(15, "C compiler internal error.");
		CC__WARN("", tree->left->file, tree->left->line,
			tree->left->char_pos);
		CHECK(error);
	}
	if (tree->right->type != CC_NUMBER) {
		THROW(15, "C compiler internal error.");
		CC__WARN("", tree->right->file, tree->right->line,
			tree->right->char_pos);
		CHECK(error);
	}
	return;
error:
	return;
}

static void check_right_integer(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("check_right_integer");
	if (tree->left != 0) {
		THROW(15, "C compiler internal error.");
		CC__WARN("", tree->file, tree->line, tree->char_pos);
		CHECK(error);
	}
	if (tree->right == 0) {
		THROW(-100044, "Missing right operand.");
		CC__WARN("", tree->file, tree->line, tree->char_pos);
		CHECK(error);
	}
	if (tree->right->type != CC_NUMBER) {
		THROW(15, "C compiler internal error.");
		CC__WARN("", tree->right->file, tree->right->line,
			tree->right->char_pos);
		CHECK(error);
	}
	return;
error:
	return;
}

static void check_left_integer(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("check_left_integer");

	if (tree->left == 0) {
		THROW(-1000047, "Missing left operand.");
		CC__WARN("", tree->file, tree->line, tree->char_pos);
		CHECK(error);
	}
	if (tree->right != 0) {
		THROW(15, "C compiler internal error.");
		CC__WARN("", tree->file, tree->line, tree->char_pos);
		CHECK(error);
	}
	if (tree->left->type != CC_NUMBER) {
		THROW(15, "C compiler internal error.");
		CC__WARN("", tree->left->file, tree->left->line,
			tree->left->char_pos);
		CHECK(error);
	}
	return;
error:
	return;
}


static void add_operator(Ex **e__, cc__PreExpression *self, cc__PreTree *tree)
{
	F("add_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->value = tree->left->value + tree->right->value;
	tree->type = CC_NUMBER;
	return;
error:
	return;
}

static void unary_plus_operator(Ex **e__, cc__PreExpression *self, 
	cc__PreTree *tree)
{
	F("unary_plus_operator");

	check_right_integer(e__, self, tree);
	CHECK(error);
	tree->value = tree->right->value;
	tree->type = tree->right->type;
	return;
error:
	return;
}

static void sub_operator(Ex **e__, cc__PreExpression *self, cc__PreTree *tree)
{
	F("sub_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->value = tree->left->value - tree->right->value;
	tree->type = CC_NUMBER;
	return;
error:
	return;
}

static void unary_minus_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("unary_minus_operator");

	check_right_integer(e__, self, tree);
	CHECK(error);
	tree->value = -tree->right->value;
	tree->type = tree->right->type;
	return;
error:
	return;
}

static void bitwise_complement_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("bitwise_complement_operator");

	check_right_integer(e__, self, tree);
	CHECK(error);
	tree->value = ~tree->right->value;
	tree->type = CC_NUMBER;
	return;
error:
	return;
}

static void logical_negation_operator(Ex **e__, cc__PreExpression *self, 
	cc__PreTree *tree)
{
	F("logical_negation_operator");

	check_right_integer(e__, self, tree);
	CHECK(error);
	tree->value = !tree->right->value;
	tree->type = CC_NUMBER;
	return;
error:
	return;
}

static void mul_operator(Ex **e__, cc__PreExpression *self, cc__PreTree *tree)
{
	F("mul_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->value = tree->left->value * tree->right->value;
	tree->type = CC_NUMBER;
	return;
error:
	return;
}

static void div_operator(Ex **e__, cc__PreExpression *self, cc__PreTree *tree)
{
	F("div_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->value = tree->left->value / tree->right->value;
	tree->type = CC_NUMBER;
	return;
error:
	return;
}

static void modulo_operator(Ex **e__, cc__PreExpression *self, 
	cc__PreTree *tree)
{
	F("modulo_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->value = tree->left->value % tree->right->value;
	tree->type = CC_NUMBER;
	return;
error:
	return;
}

static void shift_left_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("shift_left_operateur");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->value = tree->left->value << tree->right->value;
	tree->type = CC_NUMBER;
	return;
error:
	return;
}

static void shift_right_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("shift_right_operateur");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->value = tree->left->value >> tree->right->value;
	tree->type = CC_NUMBER;
	return;
error:
	return;
}

static void greater_than_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("greater_than_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->value = tree->left->value > tree->right->value;
	tree->type = CC_NUMBER;
	return;
error:
	return;
}

static void less_than_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("less_than_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->type = CC_NUMBER;
	tree->value = tree->left->value < tree->right->value;
	return;
error:
	return;
}

static void less_than_or_equal_to_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("less_than_or_equal_to_operateur");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->type = CC_NUMBER;
	tree->value = tree->left->value <= tree->right->value;
	return;
error:
	return;
}

static void greater_than_or_equal_to_operator(Ex **e__,
	cc__PreExpression *self, cc__PreTree *tree)
{
	F("greater_than_or_equal_to_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->type = CC_NUMBER;
	tree->value = tree->left->value >= tree->right->value;
	return;
error:
	return;
}

static void equal_to_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("equal_to_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->type = CC_NUMBER;
	tree->value = tree->left->value == tree->right->value;
	return;
error:
	return;
}

static void not_equal_to_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("not_equal_to_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->type = CC_NUMBER;
	tree->value = tree->left->value != tree->right->value;
	return;
error:
	return;
}

static void bitwise_and_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("bitwise_and_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->type = CC_NUMBER;
	tree->value = tree->left->value & tree->right->value;
	return;
error:
	return;
}

static void bitwise_exclusive_or_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("bitwise_exclusive_or_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->type = CC_NUMBER;
	tree->value = tree->left->value ^ tree->right->value;
	return;
error:
	return;
}

static void bitwise_inclusive_or_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("bitwise_inclusive_or_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->type = CC_NUMBER;
	tree->value = tree->left->value | tree->right->value;
	return;
error:
	return;
}


static void logical_and_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("logical_and_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->value = tree->left->value && tree->right->value;
	tree->type = CC_NUMBER;
	return;
error:
	return;
}

static void logical_or_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("logical_or_operator");

	check_2_integer(e__, self, tree);
	CHECK(error);
	tree->value = tree->left->value || tree->right->value;
	tree->type = CC_NUMBER;
	return;
error:
	return;
}

static void conditional_column_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	return;
#ifdef _MSC_VER
	e__;
	self;
	tree;
#endif
}

static void conditional_operator(Ex **e__, cc__PreExpression *self,
	cc__PreTree *tree)
{
	F("conditional_operator");

	if (tree->left == 0) {
		THROW(15, "C compiler internal error.");
		CC__WARN("", tree->file, tree->line, tree->char_pos);
		CHECK(error);
	}
	if (tree->right == 0) {
		THROW(-100041, "Missing right operand.");
		CC__WARN("", tree->file, tree->line, tree->char_pos);
		CHECK(error);
	}
	if (tree->left->type != CC_NUMBER) {
		THROW(15, "C compiler internal error.");
		CC__WARN("", tree->left->file, tree->left->line,
			tree->left->char_pos);
		CHECK(error);
	}
	if (tree->right->func != conditional_column_operator) {
		THROW(-100038, "Missing ':' operator.");
		CC__WARN("", tree->right->file, tree->right->line,
			tree->right->char_pos);
		CHECK(error);
	}

	if (tree->left->value != 0) {
		compute(e__, self, tree->right->left);
		CHECK(error);
		tree->value = tree->right->left->value;
		tree->type = tree->right->left->type;
	} else {
		compute(e__, self, tree->right->right);
		CHECK(error);
		tree->value = tree->right->right->value;
		tree->type = tree->right->right->type;
	}
	return;
error:
	return;
}


static void parenthesis(Ex **e__, cc__PreExpression *self, cc__PreTree *tree)
{
	F("parenthesis");

	check_left_integer(e__, self, tree);
	CHECK(error);
	tree->value = tree->left->value;
	tree->type = tree->left->type;
	return;
error:
	return;
}

/**
 *
 *
 *
 */
static void precedence(Ex **e__, cc__PreExpression *self,
	uint32 type, cc__PreTree *tree)
{
	F("precedence");

	tree->type = 0;

	switch(type) {
	case P('+', 0, 0, 0):
		if (self->current->precedence > 14)
		{
			tree->func = add_operator;
			tree->precedence = 11;
			tree->associativity = LEFT;
			break;
		}
		tree->func = unary_plus_operator;
		tree->precedence = 14;
		tree->associativity = RIGHT;
		break;
	case P('-', 0, 0, 0):
		if (self->current->precedence > 14) 
		{
			tree->func = sub_operator;
			tree->precedence = 11;
			tree->associativity = LEFT;
			break;
		}
		tree->func = unary_minus_operator;
		tree->precedence = 14;
		tree->associativity = RIGHT;
		break;
	case P('~', 0, 0, 0):
		tree->func = bitwise_complement_operator;
		tree->precedence = 14;
		tree->associativity = RIGHT;
		break;
	case P('!', 0, 0, 0):
		tree->func = logical_negation_operator;
		tree->precedence = 14;
		tree->associativity = RIGHT;
		break;
	case P('*', 0, 0, 0):
		tree->func = mul_operator;
		tree->precedence = 12;
		tree->associativity = LEFT;
		break;
	case P('/', 0, 0, 0):
		tree->func = div_operator;
		tree->precedence = 12;
		tree->associativity = LEFT;
		break;
	case P('%', 0, 0, 0):
		tree->func = modulo_operator;
		tree->precedence = 12;
		tree->associativity = LEFT;
		break;
	case P('<', '<', 0, 0):
		tree->func = shift_left_operator;
		tree->precedence = 10;
		tree->associativity = LEFT;
		break;
	case P('>', '>', 0, 0):
		tree->func = shift_right_operator;
		tree->precedence = 10;
		tree->associativity = LEFT;
		break;
	case P('>', 0, 0, 0):
		tree->func = greater_than_operator;
		tree->precedence = 9;
		tree->associativity = LEFT;
		break;
	case P('<', 0, 0, 0):
		tree->func = less_than_operator;
		tree->precedence = 9;
		tree->associativity = LEFT;
		break;
	case P('<', '=', 0, 0):
		tree->func = less_than_or_equal_to_operator;
		tree->precedence = 9;
		tree->associativity = LEFT;
		break;
	case P('>', '=', 0, 0):
		tree->func = greater_than_or_equal_to_operator;
		tree->precedence = 9;
		tree->associativity = LEFT;
		break;
	case P('=', '=', 0, 0):
		tree->func = equal_to_operator;
		tree->precedence = 8;
		tree->associativity = LEFT;
		break;
	case P('!', '=', 0, 0):
		tree->func = not_equal_to_operator;
		tree->precedence = 8;
		tree->associativity = LEFT;
		break;
	case P('&', 0, 0, 0):
		tree->func = bitwise_and_operator;
		tree->precedence = 7;
		tree->associativity = LEFT;
		break;
	case P('^', 0, 0, 0):
		tree->func = bitwise_exclusive_or_operator;
		tree->precedence = 6;
		tree->associativity = LEFT;
		break;
	case P('|', 0, 0, 0):
		tree->func = bitwise_inclusive_or_operator;
		tree->precedence = 5;
		tree->associativity = LEFT;
		break;
	case P('&', '&', 0, 0):
		tree->func = logical_and_operator;
		tree->precedence = 4;
		tree->associativity = LEFT;
		break;	
	case P('|', '|', 0, 0):
		tree->func = logical_or_operator;
		tree->precedence = 4;
		tree->precedence = 3;
		tree->associativity = LEFT;
		break;			
	case P('?', 0, 0, 0):
		tree->func = conditional_operator;
		tree->precedence = 2;
		tree->associativity = LEFT;
		break;
	case P(':', 0, 0, 0):
		tree->func = conditional_column_operator;
		tree->precedence = 2;
		tree->associativity = NONE;
		break;
	case P('(', 0, 0, 0):
		tree->func = parenthesis;
		tree->precedence = 0;
		tree->associativity = NONE;
		break;
	case P(')', 0, 0, 0):
		tree->func = parenthesis;
		tree->precedence = 20;
		tree->associativity = NONE;
		break;
	default:
		THROW(-100043, "Invalid C preprocessor operator.");
		CC__WARN("", tree->file, tree->line, tree->char_pos);
		CHECK(error);
	}
	return;
error:
	return;
}

static utf8 *get_dec(utf8 *name, int64 *value)
{
	*value = 0;
	while (*name != 0) {
		if (*name >= '0' && *name <= '9') {
			*value *= 10;
			*value += *name - '0';	
		} else {
			break;
		}
		name++;
	}

	return name;
}

static utf8 *get_oct(utf8 *name, int64 *value)
{
	*value = 0;
	while (*name != 0) {
		if (*name >= '0' && *name <= '7') {
			*value <<= 3;
			*value += *name - '0';
		} else {
			break;
		}
		name++;
	}

	return name;
}

static utf8 *get_hex(utf8 *name, int64 *value)
{
	*value = 0;
	while (*name != 0) {
		if (*name >= '0' && *name <= '9') {
			*value <<= 4;
			*value += *name - '0';	
		} else if (*name >= 'a' && *name <= 'f') {
			*value <<= 4;
			*value += *name - 'a' + 10;
		} else if (*name >= 'A' && *name <= 'F') {
			*value <<= 4;
			*value += *name - 'A' + 10;
		} else {
			break;
		}
		name++;
	}
	return name;
}

static int64 get_utf8(utf8 *name)
{
	return name[0];
}

static void push_number(Ex **e__, cc__PreExpression *self,
	utf8 *name, uint32 type, uint32 file, uint32 line, uint32 char_pos)
{
	F("push_number");
	cc__PreTree *tk;

	tk = new_tree(e__);
	CHECK(error1);

	tk->type = CC_NUMBER;
	tk->file = file;
	tk->line = line;
	tk->char_pos = char_pos;
	if (type == CC_CHARACTER) {
		if (name[0] == '\\') {
			switch (name[1]) {
			case '\\':
				tk->value = '\\';
				break;
			case '?':
				tk->value = '?';
				break;
			case '\'':
				tk->value = '\'';
				break;
			case '\"':
				tk->value = '"';
				break;
			case 'a':
				tk->value = '\a';
				break;
			case 'b':
				tk->value = '\b';
				break;
			case 'f':
				tk->value = '\f';
				break;
			case 'n':
				tk->value = '\n';
				break;
			case 'r':
				tk->value = '\r';
				break;
			case 't':
				tk->value = '\t';
				break;
			case 'v':
				tk->value = '\v';
				break;
			case 'x':
				get_hex(name + 2, &tk->value);
				break;
			case '0':
				get_oct(name + 1, &tk->value);
				break;
			case 'u':
			case 'U':
				get_hex(name + 2, &tk->value);
				break;

			}
		} else {
			tk->value = get_utf8(name);
		}
	} else if ((type & CC_DOUBLE) == CC_DOUBLE) {
		tk->type |= CC_DOUBLE;
	} else if ((type & CC_DECIMAL) == CC_DECIMAL) {
		get_dec(name, &tk->value);
	} else if ((type & CC_OCTAL) == CC_OCTAL) {
		get_oct(name, &tk->value);
	} else {
		get_hex(name + 2, &tk->value);
	}

	if (self->current->precedence == 20) {
		THROW(-100039, "Not expecting a number here.");
		CC__WARN("", file, line, char_pos);
		CHECK(error2);
	}

	if (self->current->left == 0 &&
		self->current->associativity == NONE) 
	{
		self->current->left = tk;
	} else if (self->current->right == 0) {
		self->current->right = tk;
	}
	tk->parent = self->current;
	self->current = tk;
	tk->associativity = NONE;
	tk->precedence = 20;
	return;
error2:
	destroy(e__, tk);
error1:
	return;
}

static void push_punctuator(Ex **e__, cc__PreExpression *self, uint32 type,
	uint32 file, uint32 line, uint32 char_pos)
{
	F("push_punctuator");
	cc__PreTree *tk;
	int8 was_right = 0;

	tk = new_tree(e__);
	CHECK(error1);

	tk->file = file;
	tk->line = line;
	tk->char_pos = char_pos;

	precedence(e__, self, type, tk);
	CHECK(error2);

	if (type == ')') {
		while (self->current->parent != 0 &&
			!(self->current->func == parenthesis &&
			self->current->precedence == 0))
		{
			self->current = self->current->parent;
		}

		if (self->current->func != parenthesis) {
			THROW(-100015, "Cannot find matching '('.");
			CC__WARN("", file, line, char_pos);
			CHECK(error2);
		}
		self->current->precedence = tk->precedence;
		Gc__free(e__, tk);
		CHECK(error2);
		return;
	} else if (type != '(') {
		while (self->current->precedence > tk->precedence) {
			if (self->current->parent->right == 
				self->current)
			{
				was_right = 1;
			} else {
				was_right = 0;
			}
			self->current = self->current->parent;
		}

		if (tk->associativity == LEFT &&
			self->current->precedence == tk->precedence) 
		{
			if (self->current->parent->right == 
				self->current)
			{
				was_right = 1;
			} else {
				was_right = 0;
			}
			self->current = self->current->parent;
		}
	} else {
		if (self->current->precedence == 20) {
			THROW(-100042, "Not expecting a '(' here.");
			CC__WARN("", file, line, char_pos);
			CHECK(error2);
		}

		if (self->current->left == 0 &&
			self->current->associativity != RIGHT) 
		{
			self->current->left = tk;
		} else if (self->current->right == 0) {
			self->current->right = tk;
		} else {
			THROW(15, "C compiler internal error.");
			CC__WARN("", file, line, char_pos);
			CHECK(error2);
		}
		tk->parent = self->current;
		self->current = tk;
		return;
	}
	
	tk->parent = self->current;

	if (tk->associativity == RIGHT && type != ':') {
		if (self->current->left == 0 && 
			self->current->associativity != RIGHT) 
		{
			self->current->left = tk;
		} else if (self->current->right == 0) {
			self->current->right = tk;
		} else {
			THROW(15, "C compiler internal error.");
			CC__WARN("", file, line, char_pos);
			CHECK(error2);
		}
		
		self->current = tk;
		return;
	}

	if (self->current->associativity == RIGHT) {
		was_right = 1;
	}
	if (was_right == 1) {
		tk->left = self->current->right;
		self->current->right = tk;
	} else {
		tk->left = self->current->left;
		self->current->left = tk;
	}

	self->current = tk;

	if (tk->left == 0 && tk->associativity == LEFT) {
		THROW(-100006, "Missing left expression.");
		CC__WARN("", file, line, char_pos);
		CHECK(error1);
	}
	
	return;
error2:
	destroy(e__, tk);
error1:
	return;
}

static void compute(Ex **e__, cc__PreExpression *self, cc__PreTree *tree)
{
	F("compute");

	if (tree == 0) return;
	compute(e__, self, tree->left);
	CHECK(error);
	compute(e__, self, tree->right);
	CHECK(error);
	if (tree->func != 0) {
		tree->func(e__, self, tree);
		CHECK(error);
	} else if (tree->left) {
		tree->value = tree->left->value;
		tree->type = tree->left->type;
	}
	if (tree->type != CC_NUMBER && 
		tree->func != conditional_column_operator)
	{
		THROW(-100009, "Expression is not an integer.");
		CC__WARN("", tree->file, tree->line,
			tree->char_pos);
	}
	return;
error:
	return;
}

static int8 solve(Ex **e__, cc__PreExpression *self)
{
	F("solve");
	
	while (self->current != self->root) {
		if (self->current->func == parenthesis &&
			self->current->precedence == 0) 
		{
			cc__PreTree *t = self->current;

			THROW(-100037, "Missing ')' in expression");
			CC__WARN("", t->file, t->line, t->char_pos);
			CHECK(error);
		}
		self->current = self->current->parent;
	}
	compute(e__, self, self->root);	
	CHECK(error);

	return (int8)self->root->value;
error:
	return 0;
}

static void set_env(Ex **e__, cc__PreExpression *self, cc__Env *env)
{
	F("set_env");

	self->env = env;
	return;
#ifdef _MSC_VER
	__func__;
	e__;
#endif
}

INIT_EXPORT cc__PreExpression__class cc__PreExpression__  = {
	// Object
	(utf8*)"cc__PreExpression",
	sizeof(cc__PreExpression),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// PreExpression
	push_number,
	push_punctuator,
	solve,
	set_env
};

