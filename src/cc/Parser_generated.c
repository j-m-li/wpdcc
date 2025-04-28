static void init_root(Ex **e__, cc__Parser *self)
{
	String s;
	cc__Type *t;
	cc__Type *o;
	int32 i = 0;
	cc__PreToken *to;
	(void)e__;
	{
		to = new_(cc__PreToken);
		STR(s, "auto");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_AUTO;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x80000000;
		t->to_basic_block = cc__Expression__.auto_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[0] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "break");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_BREAK;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x8000000;
		t->to_basic_block = cc__Expression__.break_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[1] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "case");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_CASE;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x2000000;
		t->to_basic_block = cc__Expression__.case_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[2] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "char");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_CHAR;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__.char_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[3] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "const");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_CONST;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x20000000;
		t->to_basic_block = cc__Expression__.const_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[4] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "continue");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_CONTINUE;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x8000000;
		t->to_basic_block = cc__Expression__.continue_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[5] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "default");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_DEFAULT;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x2000000;
		t->to_basic_block = cc__Expression__.default_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[6] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "do");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_DO;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x2000000;
		t->to_basic_block = cc__Expression__.do_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[7] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "double");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_DOUBLE;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__.double_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[8] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "else");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_ELSE;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x2000000;
		t->to_basic_block = cc__Expression__.else_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[9] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "enum");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_ENUM;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x4000000;
		t->to_basic_block = cc__Expression__.enum_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[10] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "extern");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_EXTERN;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x80000000;
		t->to_basic_block = cc__Expression__.extern_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[11] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "float");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_FLOAT;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__.float_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[12] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "for");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_FOR;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x2000000;
		t->to_basic_block = cc__Expression__.for_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[13] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "goto");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_GOTO;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x8000000;
		t->to_basic_block = cc__Expression__.goto_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[14] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "if");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_IF;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x2000000;
		t->to_basic_block = cc__Expression__.if_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[15] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "inline");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_INLINE;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x80000000;
		t->to_basic_block = cc__Expression__.inline_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[16] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "int");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_INT;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__.int_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[17] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "long");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_LONG;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__.long_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[18] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "register");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_REGISTER;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x80000000;
		t->to_basic_block = cc__Expression__.register_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[19] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "restrict");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_RESTRICT;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x20000000;
		t->to_basic_block = cc__Expression__.restrict_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[20] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "return");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_RETURN;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x8000000;
		t->to_basic_block = cc__Expression__.return_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[21] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "short");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_SHORT;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__.short_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[22] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "signed");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_SIGNED;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__.signed_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[23] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "static");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_STATIC;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x80000000;
		t->to_basic_block = cc__Expression__.static_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[24] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "struct");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_STRUCT;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x4000000;
		t->to_basic_block = cc__Expression__.struct_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[25] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "switch");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_SWITCH;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x2000000;
		t->to_basic_block = cc__Expression__.switch_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[26] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "typedef");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_TYPEDEF;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x4000000;
		t->to_basic_block = cc__Expression__.typedef_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[27] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "union");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_UNION;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x4000000;
		t->to_basic_block = cc__Expression__.union_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[28] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "unsigned");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_UNSIGNED;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__.unsigned_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[29] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "void");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_VOID;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__.void_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[30] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "volatile");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_VOLATILE;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x20000000;
		t->to_basic_block = cc__Expression__.volatile_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[31] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "while");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_WHILE;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x2000000;
		t->to_basic_block = cc__Expression__.while_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[32] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "_Bool");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN__BOOL;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__._Bool_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[33] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "_Complex");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN__COMPLEX;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__._Complex_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[34] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "_Imaginary");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN__IMAGINARY;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__._Imaginary_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[35] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "/**/pointer");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_POINTER;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__./**/pointer_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[36] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "/**/object_type");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_OBJECT_TYPE;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x800000;
		t->to_basic_block = cc__Expression__./**/object_type_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[37] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "/**/function_type");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_FUNCTION_TYPE;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x800000;
		t->to_basic_block = cc__Expression__./**/function_type_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[38] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "/**/label");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_LABEL;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x800000;
		t->to_basic_block = cc__Expression__./**/label_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[39] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "/**/typedef_identifier");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_TYPEDEF_IDENTIFIER;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x40000000;
		t->to_basic_block = cc__Expression__./**/typedef_identifier_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[40] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "/**/string_constant");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_STRING_CONSTANT;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x400000;
		t->to_basic_block = cc__Expression__./**/string_constant_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[41] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "/**/character_constant");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_CHARACTER_CONSTANT;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x400000;
		t->to_basic_block = cc__Expression__./**/character_constant_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[42] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "/**/number_constant");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_NUMBER_CONSTANT;
		t->associativity = 2;
		t->precedence = 400;
		t->flags = 0x400000;
		t->to_basic_block = cc__Expression__./**/number_constant_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[43] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "paranthes");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_PARANTHES;
		t->associativity = 2;
		t->precedence = 350;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.paranthes_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[44] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "plusplus");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_PLUSPLUS;
		t->associativity = 2;
		t->precedence = 350;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.plusplus_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[45] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "minusminus");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_MINUSMINUS;
		t->associativity = 2;
		t->precedence = 350;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.minusminus_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[46] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "and");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_AND;
		t->associativity = 2;
		t->precedence = 350;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.and_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[47] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "star");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_STAR;
		t->associativity = 2;
		t->precedence = 350;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.star_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[48] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "plus");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_PLUS;
		t->associativity = 2;
		t->precedence = 350;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.plus_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[49] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "minus");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_MINUS;
		t->associativity = 2;
		t->precedence = 350;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.minus_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[50] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "colon");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_COLON;
		t->associativity = 2;
		t->precedence = 350;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.colon_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[51] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "semicolon");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_SEMICOLON;
		t->associativity = 2;
		t->precedence = 350;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.semicolon_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[52] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "bracket");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_BRACKET;
		t->associativity = 2;
		t->precedence = 350;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.bracket_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[53] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "brace");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_BRACE;
		t->associativity = 2;
		t->precedence = 300;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.brace_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[54] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "closing_brace");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_CLOSING_BRACE;
		t->associativity = 2;
		t->precedence = 300;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.closing_brace_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[55] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "closing_paranthesis");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_CLOSING_PARANTHESIS;
		t->associativity = 2;
		t->precedence = 300;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.closing_paranthesis_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[56] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "closing_bracket");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_CLOSING_BRACKET;
		t->associativity = 2;
		t->precedence = 300;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.closing_bracket_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[57] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "array_subscripting");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_ARRAY_SUBSCRIPTING;
		t->associativity = 1;
		t->precedence = 200;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.array_subscripting_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[58] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "function_calls");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_FUNCTION_CALLS;
		t->associativity = 1;
		t->precedence = 200;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.function_calls_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[59] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "structure_member");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_STRUCTURE_MEMBER;
		t->associativity = 1;
		t->precedence = 200;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.structure_member_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[60] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "pointer_structure_member");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_POINTER_STRUCTURE_MEMBER;
		t->associativity = 1;
		t->precedence = 200;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.pointer_structure_member_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[61] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "postfix_increment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_POSTFIX_INCREMENT;
		t->associativity = 1;
		t->precedence = 200;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.postfix_increment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[62] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "postfix_decrement");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_POSTFIX_DECREMENT;
		t->associativity = 1;
		t->precedence = 200;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.postfix_decrement_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[63] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "compound_literals");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_COMPOUND_LITERALS;
		t->associativity = 2;
		t->precedence = 200;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.compound_literals_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[64] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "prefix_increment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_PREFIX_INCREMENT;
		t->associativity = 0;
		t->precedence = 190;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.prefix_increment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[65] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "prefix_decrement");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_PREFIX_DECREMENT;
		t->associativity = 0;
		t->precedence = 190;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.prefix_decrement_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[66] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "address");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_ADDRESS;
		t->associativity = 0;
		t->precedence = 190;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.address_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[67] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "indirection");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_INDIRECTION;
		t->associativity = 0;
		t->precedence = 190;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.indirection_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[68] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "unary_plus");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_UNARY_PLUS;
		t->associativity = 0;
		t->precedence = 190;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.unary_plus_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[69] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "unary_minus");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_UNARY_MINUS;
		t->associativity = 0;
		t->precedence = 190;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.unary_minus_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[70] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "bitwise_complement");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_BITWISE_COMPLEMENT;
		t->associativity = 0;
		t->precedence = 190;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.bitwise_complement_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[71] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "logical_negation");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_LOGICAL_NEGATION;
		t->associativity = 0;
		t->precedence = 190;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.logical_negation_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[72] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "sizeof");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_SIZEOF;
		t->associativity = 0;
		t->precedence = 190;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.sizeof_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[73] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "cast");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_CAST;
		t->associativity = 2;
		t->precedence = 180;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.cast_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[74] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "multiply");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_MULTIPLY;
		t->associativity = 1;
		t->precedence = 170;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.multiply_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[75] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "divide");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_DIVIDE;
		t->associativity = 1;
		t->precedence = 170;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.divide_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[76] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "modulo");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_MODULO;
		t->associativity = 1;
		t->precedence = 170;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.modulo_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[77] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "addition");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_ADDITION;
		t->associativity = 1;
		t->precedence = 160;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.addition_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[78] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "substraction");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_SUBSTRACTION;
		t->associativity = 1;
		t->precedence = 160;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.substraction_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[79] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "bitwise_shift_left");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_BITWISE_SHIFT_LEFT;
		t->associativity = 1;
		t->precedence = 150;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.bitwise_shift_left_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[80] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "bitwise_shift_right");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_BITWISE_SHIFT_RIGHT;
		t->associativity = 1;
		t->precedence = 150;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.bitwise_shift_right_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[81] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "less_than");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_LESS_THAN;
		t->associativity = 1;
		t->precedence = 140;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.less_than_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[82] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "greater_than");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_GREATER_THAN;
		t->associativity = 1;
		t->precedence = 140;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.greater_than_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[83] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "less_than_or_equal_to");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_LESS_THAN_OR_EQUAL_TO;
		t->associativity = 1;
		t->precedence = 140;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.less_than_or_equal_to_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[84] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "greater_than_or_equal_to");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_GREATER_THAN_OR_EQUAL_TO;
		t->associativity = 1;
		t->precedence = 140;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.greater_than_or_equal_to_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[85] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "equal_to");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_EQUAL_TO;
		t->associativity = 1;
		t->precedence = 130;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.equal_to_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[86] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "not_equal_to");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_NOT_EQUAL_TO;
		t->associativity = 1;
		t->precedence = 130;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.not_equal_to_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[87] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "bitwise_and");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_BITWISE_AND;
		t->associativity = 1;
		t->precedence = 120;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.bitwise_and_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[88] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "bitwise_exclusive_or");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_BITWISE_EXCLUSIVE_OR;
		t->associativity = 1;
		t->precedence = 110;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.bitwise_exclusive_or_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[89] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "bitwise_inclusive_or");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_BITWISE_INCLUSIVE_OR;
		t->associativity = 1;
		t->precedence = 100;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.bitwise_inclusive_or_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[90] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "logical_and");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_LOGICAL_AND;
		t->associativity = 1;
		t->precedence = 90;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.logical_and_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[91] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "logical_or");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_LOGICAL_OR;
		t->associativity = 1;
		t->precedence = 80;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.logical_or_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[92] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "conditonal");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_CONDITONAL;
		t->associativity = 1;
		t->precedence = 70;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.conditonal_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[93] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "simple_assignment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_SIMPLE_ASSIGNMENT;
		t->associativity = 0;
		t->precedence = 60;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.simple_assignment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[94] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "multiply_assignment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_MULTIPLY_ASSIGNMENT;
		t->associativity = 0;
		t->precedence = 60;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.multiply_assignment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[95] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "divide_assignment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_DIVIDE_ASSIGNMENT;
		t->associativity = 0;
		t->precedence = 60;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.divide_assignment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[96] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "modulo_assignment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_MODULO_ASSIGNMENT;
		t->associativity = 0;
		t->precedence = 60;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.modulo_assignment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[97] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "addition_assignment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_ADDITION_ASSIGNMENT;
		t->associativity = 0;
		t->precedence = 60;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.addition_assignment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[98] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "substraction_assignment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_SUBSTRACTION_ASSIGNMENT;
		t->associativity = 0;
		t->precedence = 60;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.substraction_assignment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[99] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "shift_left_assignment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_SHIFT_LEFT_ASSIGNMENT;
		t->associativity = 0;
		t->precedence = 60;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.shift_left_assignment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[100] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "shift_right_assignment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_SHIFT_RIGHT_ASSIGNMENT;
		t->associativity = 0;
		t->precedence = 60;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.shift_right_assignment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[101] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "and_assignment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_AND_ASSIGNMENT;
		t->associativity = 0;
		t->precedence = 60;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.and_assignment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[102] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "exclusive_or_assignment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_EXCLUSIVE_OR_ASSIGNMENT;
		t->associativity = 0;
		t->precedence = 60;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.exclusive_or_assignment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[103] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "inclusive_or_assignment");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_INCLUSIVE_OR_ASSIGNMENT;
		t->associativity = 0;
		t->precedence = 60;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.inclusive_or_assignment_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[104] = t;
	{
		to = new_(cc__PreToken);
		STR(s, "comma");
		while (s.cstr[i]) {i++;}
		to->identifier = Gc__malloc(e__, i + 1); i = 0;
		while (s.cstr[i]) {to->identifier[i] =s.cstr[i]; i++;}
		to->identifier[i] = 0;
		t = self->scope->_->new_object(e__, self->scope);
		self->scope->_->name_object(e__, self->scope, to);
		t->id = CC__TOKEN_COMMA;
		t->associativity = 1;
		t->precedence = 50;
		t->flags = 0x10000000;
		t->to_basic_block = cc__Expression__.comma_;
		self->scope->_->end_object(e__, self->scope, &o);
	}
	self->types[105] = t;
}

static cc__Type *get_id_type(Ex **e__, cc__Parser *self, uint32 type)
{
	(void)e__;
	if (type < CC__TYPE_NB_TYPES) return self->types[type];
	return 0;
}

static cc__Type *get_type_of_identifier(Ex **e__, cc__Parser *self, utf8 *name)
{
	utf8 *p;
	(void)e__;
	 switch(name[0]) {
	 case 'a':
	  p = (utf8*)"uto"; name = name + 1;
	  while (*p && *name && *p == *name) {p++; name++;}
	  if (*name == 0 && *p == 0) return self->types[CC__TOKEN_AUTO];
	  break;
	 case 'b':
	  p = (utf8*)"reak"; name = name + 1;
	  while (*p && *name && *p == *name) {p++; name++;}
	  if (*name == 0 && *p == 0) return self->types[CC__TOKEN_BREAK];
	  break;
	 case 'c':
	  switch(name[1]) {
	  case 'a':
	   p = (utf8*)"se"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN_CASE];
	   break;
	  case 'h':
	   p = (utf8*)"ar"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN_CHAR];
	   break;
	  case 'o':
	   switch(name[2]) {
	   case 'n':
	    switch(name[3]) {
	    case 's':
	     p = (utf8*)"t"; name = name + 4;
	     while (*p && *name && *p == *name) {p++; name++;}
	     if (*name == 0 && *p == 0) return self->types[CC__TOKEN_CONST];
	     break;
	    case 't':
	     p = (utf8*)"inue"; name = name + 4;
	     while (*p && *name && *p == *name) {p++; name++;}
	     if (*name == 0 && *p == 0) return self->types[CC__TOKEN_CONTINUE];
	     break;
	    }
	    break;
	   }
	   break;
	  }
	  break;
	 case 'd':
	  switch(name[1]) {
	  case 'e':
	   p = (utf8*)"fault"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN_DEFAULT];
	   break;
	  case 'o':
	   switch(name[2]) {
	   case 0:
	    return self->types[CC__TOKEN_DO];
	   case 'u':
	    p = (utf8*)"ble"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_DOUBLE];
	    break;
	   }
	   break;
	  }
	  break;
	 case 'e':
	  switch(name[1]) {
	  case 'l':
	   p = (utf8*)"se"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN_ELSE];
	   break;
	  case 'n':
	   p = (utf8*)"um"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN_ENUM];
	   break;
	  case 'x':
	   p = (utf8*)"tern"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN_EXTERN];
	   break;
	  }
	  break;
	 case 'f':
	  switch(name[1]) {
	  case 'l':
	   p = (utf8*)"oat"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN_FLOAT];
	   break;
	  case 'o':
	   p = (utf8*)"r"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN_FOR];
	   break;
	  }
	  break;
	 case 'g':
	  p = (utf8*)"oto"; name = name + 1;
	  while (*p && *name && *p == *name) {p++; name++;}
	  if (*name == 0 && *p == 0) return self->types[CC__TOKEN_GOTO];
	  break;
	 case 'i':
	  switch(name[1]) {
	  case 'f':
	   p = (utf8*)""; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN_IF];
	   break;
	  case 'n':
	   switch(name[2]) {
	   case 'l':
	    p = (utf8*)"ine"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_INLINE];
	    break;
	   case 't':
	    p = (utf8*)""; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_INT];
	    break;
	   }
	   break;
	  }
	  break;
	 case 'l':
	  p = (utf8*)"ong"; name = name + 1;
	  while (*p && *name && *p == *name) {p++; name++;}
	  if (*name == 0 && *p == 0) return self->types[CC__TOKEN_LONG];
	  break;
	 case 'r':
	  switch(name[1]) {
	  case 'e':
	   switch(name[2]) {
	   case 'g':
	    p = (utf8*)"ister"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_REGISTER];
	    break;
	   case 's':
	    p = (utf8*)"trict"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_RESTRICT];
	    break;
	   case 't':
	    p = (utf8*)"urn"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_RETURN];
	    break;
	   }
	   break;
	  }
	  break;
	 case 's':
	  switch(name[1]) {
	  case 'h':
	   p = (utf8*)"ort"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN_SHORT];
	   break;
	  case 'i':
	   switch(name[2]) {
	   case 'g':
	    p = (utf8*)"ned"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_SIGNED];
	    break;
	   case 'z':
	    p = (utf8*)"eof"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_SIZEOF];
	    break;
	   }
	   break;
	  case 't':
	   switch(name[2]) {
	   case 'a':
	    p = (utf8*)"tic"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_STATIC];
	    break;
	   case 'r':
	    p = (utf8*)"uct"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_STRUCT];
	    break;
	   }
	   break;
	  case 'w':
	   p = (utf8*)"itch"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN_SWITCH];
	   break;
	  }
	  break;
	 case 't':
	  p = (utf8*)"ypedef"; name = name + 1;
	  while (*p && *name && *p == *name) {p++; name++;}
	  if (*name == 0 && *p == 0) return self->types[CC__TOKEN_TYPEDEF];
	  break;
	 case 'u':
	  switch(name[1]) {
	  case 'n':
	   switch(name[2]) {
	   case 'i':
	    p = (utf8*)"on"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_UNION];
	    break;
	   case 's':
	    p = (utf8*)"igned"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_UNSIGNED];
	    break;
	   }
	   break;
	  }
	  break;
	 case 'v':
	  switch(name[1]) {
	  case 'o':
	   switch(name[2]) {
	   case 'i':
	    p = (utf8*)"d"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_VOID];
	    break;
	   case 'l':
	    p = (utf8*)"atile"; name = name + 3;
	    while (*p && *name && *p == *name) {p++; name++;}
	    if (*name == 0 && *p == 0) return self->types[CC__TOKEN_VOLATILE];
	    break;
	   }
	   break;
	  }
	  break;
	 case 'w':
	  p = (utf8*)"hile"; name = name + 1;
	  while (*p && *name && *p == *name) {p++; name++;}
	  if (*name == 0 && *p == 0) return self->types[CC__TOKEN_WHILE];
	  break;
	 case '_':
	  switch(name[1]) {
	  case 'B':
	   p = (utf8*)"ool"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN__BOOL];
	   break;
	  case 'C':
	   p = (utf8*)"omplex"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN__COMPLEX];
	   break;
	  case 'I':
	   p = (utf8*)"maginary"; name = name + 2;
	   while (*p && *name && *p == *name) {p++; name++;}
	   if (*name == 0 && *p == 0) return self->types[CC__TOKEN__IMAGINARY];
	   break;
	  }
	  break;
	 }
	return 0;

}

static cc__Type *get_operator_type(Ex **e__, cc__Parser *self, uint32 type)
{
	(void)e__;
	switch(type) {
	case 0x28:
		return self->types[CC__TOKEN_PARANTHES];
	case 0x2b2b:
		return self->types[CC__TOKEN_PLUSPLUS];
	case 0x2d2d:
		return self->types[CC__TOKEN_MINUSMINUS];
	case 0x26:
		return self->types[CC__TOKEN_AND];
	case 0x2a:
		return self->types[CC__TOKEN_STAR];
	case 0x2b:
		return self->types[CC__TOKEN_PLUS];
	case 0x2d:
		return self->types[CC__TOKEN_MINUS];
	case 0x3a:
		return self->types[CC__TOKEN_COLON];
	case 0x3b:
		return self->types[CC__TOKEN_SEMICOLON];
	case 0x5b:
	case 0x3a3c:
		return self->types[CC__TOKEN_BRACKET];
	case 0x7b:
	case 0x253c:
		return self->types[CC__TOKEN_BRACE];
	case 0x7d:
	case 0x3e25:
		return self->types[CC__TOKEN_CLOSING_BRACE];
	case 0x29:
		return self->types[CC__TOKEN_CLOSING_PARANTHESIS];
	case 0x5d:
	case 0x3e3a:
		return self->types[CC__TOKEN_CLOSING_BRACKET];
	case 0x315b:
	case 0x313a3c:
		return self->types[CC__TOKEN_ARRAY_SUBSCRIPTING];
	case 0x3128:
		return self->types[CC__TOKEN_FUNCTION_CALLS];
	case 0x2e:
		return self->types[CC__TOKEN_STRUCTURE_MEMBER];
	case 0x3e2d:
		return self->types[CC__TOKEN_POINTER_STRUCTURE_MEMBER];
	case 0x312b2b:
		return self->types[CC__TOKEN_POSTFIX_INCREMENT];
	case 0x312d2d:
		return self->types[CC__TOKEN_POSTFIX_DECREMENT];
	case 0x3228:
		return self->types[CC__TOKEN_COMPOUND_LITERALS];
	case 0x322b2b:
		return self->types[CC__TOKEN_PREFIX_INCREMENT];
	case 0x322d2d:
		return self->types[CC__TOKEN_PREFIX_DECREMENT];
	case 0x3126:
		return self->types[CC__TOKEN_ADDRESS];
	case 0x312a:
		return self->types[CC__TOKEN_INDIRECTION];
	case 0x312b:
		return self->types[CC__TOKEN_UNARY_PLUS];
	case 0x312d:
		return self->types[CC__TOKEN_UNARY_MINUS];
	case 0x7e:
		return self->types[CC__TOKEN_BITWISE_COMPLEMENT];
	case 0x21:
		return self->types[CC__TOKEN_LOGICAL_NEGATION];
	case 0x3328:
		return self->types[CC__TOKEN_CAST];
	case 0x322a:
		return self->types[CC__TOKEN_MULTIPLY];
	case 0x2f:
		return self->types[CC__TOKEN_DIVIDE];
	case 0x25:
		return self->types[CC__TOKEN_MODULO];
	case 0x322b:
		return self->types[CC__TOKEN_ADDITION];
	case 0x322d:
		return self->types[CC__TOKEN_SUBSTRACTION];
	case 0x3c3c:
		return self->types[CC__TOKEN_BITWISE_SHIFT_LEFT];
	case 0x3e3e:
		return self->types[CC__TOKEN_BITWISE_SHIFT_RIGHT];
	case 0x3c:
		return self->types[CC__TOKEN_LESS_THAN];
	case 0x3e:
		return self->types[CC__TOKEN_GREATER_THAN];
	case 0x3d3c:
		return self->types[CC__TOKEN_LESS_THAN_OR_EQUAL_TO];
	case 0x3d3e:
		return self->types[CC__TOKEN_GREATER_THAN_OR_EQUAL_TO];
	case 0x3d3d:
		return self->types[CC__TOKEN_EQUAL_TO];
	case 0x3d21:
		return self->types[CC__TOKEN_NOT_EQUAL_TO];
	case 0x3226:
		return self->types[CC__TOKEN_BITWISE_AND];
	case 0x5e:
		return self->types[CC__TOKEN_BITWISE_EXCLUSIVE_OR];
	case 0x7c:
		return self->types[CC__TOKEN_BITWISE_INCLUSIVE_OR];
	case 0x2626:
		return self->types[CC__TOKEN_LOGICAL_AND];
	case 0x7c7c:
		return self->types[CC__TOKEN_LOGICAL_OR];
	case 0x3f:
		return self->types[CC__TOKEN_CONDITONAL];
	case 0x3d:
		return self->types[CC__TOKEN_SIMPLE_ASSIGNMENT];
	case 0x3d2a:
		return self->types[CC__TOKEN_MULTIPLY_ASSIGNMENT];
	case 0x3d2f:
		return self->types[CC__TOKEN_DIVIDE_ASSIGNMENT];
	case 0x3d25:
		return self->types[CC__TOKEN_MODULO_ASSIGNMENT];
	case 0x3d2b:
		return self->types[CC__TOKEN_ADDITION_ASSIGNMENT];
	case 0x3d2d:
		return self->types[CC__TOKEN_SUBSTRACTION_ASSIGNMENT];
	case 0x3d3c3c:
		return self->types[CC__TOKEN_SHIFT_LEFT_ASSIGNMENT];
	case 0x3d3e3e:
		return self->types[CC__TOKEN_SHIFT_RIGHT_ASSIGNMENT];
	case 0x3d26:
		return self->types[CC__TOKEN_AND_ASSIGNMENT];
	case 0x3d5e:
		return self->types[CC__TOKEN_EXCLUSIVE_OR_ASSIGNMENT];
	case 0x3d7c:
		return self->types[CC__TOKEN_INCLUSIVE_OR_ASSIGNMENT];
	case 0x2c:
		return self->types[CC__TOKEN_COMMA];
	}
	return 0;

}

