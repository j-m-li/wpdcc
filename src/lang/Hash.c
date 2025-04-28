/* "$Id: $"
 *
 *                          21 June MMVIII PUBLIC DOMAIN
 *             The author disclaims copyright to this source code.
 *
 *          s = (a + b + c) / 2.0;
 *          A = sqrt(s * (s - a) * (s - b) * (s - c));    // Heron of Alexandria
 */

#include <lang/Hash.h>

static void __init__(Ex **e__)
{
	(void)e__;
	Hash__.super = &Object__;
	return;
}

static void __ctor__(Ex **e__, Hash *self)
{
	F("__ctor__");
	uint32 i;
	Hash_data **d;

	d = self->data = Gc__malloc(e__, sizeof(Hash_data*) * 256);
	CHECK(error);
	for (i = 0; i < 256; i++) {
		d[i] = 0;
	}
	self->mask = 255;
	self->nb_data = 0;
	return;
error:
	return;
}

static void __dtor__(Ex **e__, Hash *self)
{
	F("__dtor__");
	int32 i;
	Hash_data **d = self->data;
	
	for (i = self->mask; i >= 0; i--) {
		Hash_data *h = d[i];
		while (h != 0) {
			Hash_data *n = h->next;
			Gc__free(e__, h->hash);
			Gc__free(e__, h);
			CHECK(error1);
			h = n;
		}
	}
error1:
	Gc__free(e__, self->data);
	CHECK(error2);
error2:
	return;
}

static uint32 hash_code(Ex **e__, Hash *self)
{
#ifdef _MSC_VER
	return *((uint32*)&self) >> 4;
	e__;
#else 
	return (uint32)self >> 4;
#endif
} 

static void to_string(Ex **e__, Hash *self, String *out)
{
	F("to_string");
	String s;

	STR(s, self->_->__name__);
	out->_->add(e__, out, &s);
	CHECK(error);
error:
	return;
}

static void grow(Ex **e__, Hash *self)
{
	F("grow");
	int32 i;
	uint32 om;
	uint32 nm;
	Hash_data **na;
	Hash_data **oa;

	nm = self->mask * 2 + 1;
	om = self->mask;
	oa = self->data;
	na = Gc__malloc(e__, sizeof(Hash_data*) * (nm + 1));
	CHECK(error);

	for (i = nm; i >= 0; i--) {
		na[i] = 0;
	}

	for (i = om; i >= 0; i--) {
		Hash_data *h = oa[i];

		while (h != 0) {
			Hash_data *n = h->next;
			uint32 hc = h->hash_code & nm;
			Hash_data *p;

			h->next = 0;
			p = na[hc];
			if (p == 0) {
				na[hc] = h;
			} else {
				while (p->next != 0) {
					p = p->next;
				}
				p->next = h;
			}
			h = n;	
		}
	}

	self->data = na;
	self->mask = nm;
	Gc__free(e__, oa);
	CHECK(error);
	return;
error:
	return;
}

static void new_data(Ex **e__, Hash *self, utf8 *cstr, int32 size, 
	uint32 hash_code, void *in, Hash_data **out)
{
	F("new_data");
	utf8 *ptr2;
	Hash_data *p;

	(void)self;
	p = Gc__malloc(e__, sizeof(Hash_data));
	CHECK(error1);
	p->hash_code = hash_code;
	p->data = in;
	ptr2 = p->hash = Gc__malloc(e__, size + 1);
	CHECK(error2);
	ptr2[size] = 0;
	while (size > 0) {
		size--;
		ptr2[size] = cstr[size];
	}
	p->next = 0;
	*out = p;
	return;
error2:
	Gc__free(e__, p);
error1:
	return;
}

static Hash_data *set(Ex **e__, Hash *self, String *hash, void *in, void **out)
{
	F("set");
	uint32 hash_code = 0;
	utf8 *ptr;
	int32 length;
	int32 l = 0;
	Hash_data **hd = self->data;
	Hash_data *p;
	utf8 *hs;
	Hash_data *ret = 0;

	if (self->nb_data > self->mask * 8) {
		grow(e__, self);
		CHECK(error);
	}

	hs = ptr = hash->_->get_cstr(e__, hash);
	length = hash->_->get_size(e__, hash);
	CHECK(error);

	while (*ptr != 0 && l < length) {
		hash_code += *ptr;
		hash_code ^= hash_code << 0x4 | hash_code >> 28;
		ptr++;
		l++;
	}
	
	p = hd[hash_code & self->mask];
	if (p == 0) {
		if (in != 0) {
			new_data(e__, self, hs, l, hash_code, in,
				&(hd[hash_code & self->mask]));
			ret = hd[hash_code & self->mask];
			CHECK(error);
			self->nb_data++;
		}
		*out = 0;
	} else {
		Hash_data *last = 0;

		while (p != 0) {
			if (p->hash_code == hash_code) {
				int32 i = 0;
				utf8 *c = p->hash;

				while (hs[i] == c[i] && c[i] != 0) {
					i++;
				}
				if (hs[i] == c[i]) break;
			}
			last = p;
			p = p->next;
		}
		if (p != 0) {
			*out = p->data;
			if (in != 0) {
				p->data = in;
			} else {
				if (last == 0) {
					hd[hash_code & self->mask] = p->next;
				} else {
					last->next = p->next;
				}
				Gc__free(e__, p->hash);
				Gc__free(e__, p);
				CHECK(error);
				self->nb_data--;
			}
			ret = p;
		} else {
			*out = 0;
			if (in != 0) {
				new_data(e__, self, hs, l, hash_code, in,
					&(last->next));
				CHECK(error);
				ret = last->next;
				self->nb_data++;
			}
		}
	}
	return ret;
error:
	*out = 0;
	return 0;
}

static void *get(Ex **e__, Hash *self, String *hash)
{
	F("get");
	uint32 hash_code = 0;
	utf8 *ptr;
	int32 length;
	int32 l = 0;
	Hash_data **hd = self->data;
	Hash_data *p;
	utf8 *hs;

	hs = ptr = hash->_->get_cstr(e__, hash);
	length = hash->_->get_size(e__, hash);
	CHECK(error);

	while (*ptr != 0 && l < length) {
		hash_code += *ptr;
		hash_code ^= hash_code << 0x4 | hash_code >> 28;
		ptr++;
		l++;
	}
	
	p = hd[hash_code & self->mask];
	while (p != 0) {
		if (p->hash_code == hash_code) {
			int32 i = 0;
			utf8 *c = p->hash;

			while (hs[i] == c[i] && c[i] != 0) {
				i++;
			}
			if (hs[i] == c[i]) return p->data;			
		}
		p = p->next;
	}
	return 0;
error:
	return 0;
}


static uint32 get_length(Ex **e__, Hash *self)
{
	F("get_length");
	(void)__func__;
	(void)e__;
	return self->nb_data;
}

static void clear_objects(Ex **e__, Hash *self)
{
	F("clear_objects");
	Hash_data **hd = self->data;
	Hash_data *p;
	int32 i;

	for (i = self->mask; i >= 0; i--) {
		p = hd[i];
		while (p != 0) {
			Hash_data *n = p->next;
		
			delete_((Object*)p->data);
			Gc__free(e__, p->hash);
			Gc__free(e__, p);
			CHECK(error);
			p = n;
		}
		hd[i] = 0;
	}
	self->nb_data = 0;
	return;
error:
	return;
}

static void unset_first(Ex **e__, Hash *self, void **out)
{
	F("unset_first");
	Hash_data **hd = self->data;
	Hash_data *p;
	int32 i;

	for (i = self->mask; i >= 0; i--) {
		p = hd[i];
		if (p != 0) {
			*out = p->data;
			hd[i] = p->next;
			Gc__free(e__, p->hash);
			Gc__free(e__, p);
			CHECK(error);
			self->nb_data--;
			return;
		}
	}
	return;
error:
	return;
}


static void iterate(Ex **e__, Hash *self, Hash_iterate *state)
{
	F("iterate");
	Hash_data **hd = self->data;
	Hash_data *p;
	uint32 i;

	i = state->index;
	if (i >= self->mask) {
		state->data = 0;
		return;
	}

	p = hd[i];
	if (state->data == 0 && p != 0) {
		state->data = p;
		state->index = i;
		return;
	}
	while (p != 0) {
		if (p == state->data && p->next != 0) {
			state->data = p->next;
			state->index = i;
			return;
		}
		p = p->next;
	}
	i++;	

	for (; i < self->mask; i++) {
		p = hd[i];
		if (p != 0) {
			break;	
		}
	}
	state->data = p;
	state->index = i;
	return;
error:
	return;
}

INIT_EXPORT Hash__class Hash__  = {
	// Object
	(utf8*)"Hash",
	sizeof(Hash),
	0,
	__init__,
	__ctor__,
	__dtor__,
	hash_code,
	to_string,

	// Hash
	set,
	get,
	get_length,
	clear_objects,
	unset_first,
	iterate
};

