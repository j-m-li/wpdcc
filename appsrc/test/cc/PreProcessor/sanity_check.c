#pragma c5_clear_tokens
#define hash_hash # ## #
#define mkstr(a) # a
#define in_between(a) mkstr(a)
#define join(c, d) in_between(c hash_hash d)
char p[] = join(x, y); // equivalent to
#pragma c5_compare_tokens char p[] = "x ## y";

#define \u0800jml 3


__FILE__
__LINE__
__DATE__
__TIME__
#if __STDC__ != 1
#error stdc
#endif
#if __STDC_HOSTED__ != 1
#error hosted
#endif
#if __STDC_VERSION__ != 199901
#error version
#endif
#if __STDC_IEC_559__ != 1
#error iec
#endif
#if __STDC_MB_MIGHT_NEQ_EC__ != 1
#error neq
#endif
#if __STDC_IEC_559_COMPLEX__ != 1
#error complex
#endif
#if __STDC_ISO_10646__ != 199712
#error iso
#endif

#if \u0800jml != 3
#error universal character 1
#else
universal character ok!
#endif

This is a ??/
single line !!

This is a \
single line too !!

trigraphs ??([ ??)] ??<{ ??>}  ??'^  ??/\ ??!| ??-~ ??=# ???3x?

%:define alternative (good)
#ifndef alternative
#error alternative doesn''t work...
#endif

# if defined(alternative)
alternative ok!
#else
#error alternative is not defined
#endif

# if ! ! ! defined ( alternative )
#error if doesnot work
#endif

# if  (0 + (1-+2 && 2))
and logical ok
#else
#error logical and doesnt work
#endif

#if 0 ? 1 : (2 - 2)
#error conditional doesnt work
#elif !(0 ? 1 : 0)
#else
#error cond0itional error
#endif

#if 0
#error float 1
#elif 10 < 10
#error float 2
#elif 10 == 9
#error float 3
#elif !(10 == 10)
#error float 4
#elif 10 <= 9
#error float 5
#elif 10 >= 11
#error float 6
#elif 100 == 100
#else
#error float 7
#endif

#if 'a' == 65
#error character 1
#elif 'A' == 65
#else
#error character 2
#endif

#if 007 != 7
#error octal
#endif

#if ~0x0 != -1
#error complement 1
#elif ~-1 != 0
#error complement 2
#endif

#if 5 * 3 != 15
#error mul
#elif 5 / 2 != 2
#error div
#elif 5 % 2 != 1
#error modulo
#elif 5 + 2 != 7
#error add
#elif 5 - 2 != 3
#error sub
#elif (4 | 1) != 5
#error or
#elif (5 & 4) != 4
#error and
#elif (5 ^ 4) != 1
#error xor
#elif 4 >> 1 != 2
#error shift right
#elif 4 << 1 != 8
#error shift left
#elif !(0 || 1)
#error logic or
#elif 0 && 1
#error logic and
#else
test passed
#endif 

#if 5 + 2 * 3 != 11
#error precedence 1
#elif 5 * 2 + 3 != 13
#error precedence 2
#endif


# keep this line with the #

#define EMPTY
EMPTY #error this shall not throw an error!!!

#define STR(a, ...) # a # __VA_ARGS__
//STR(hello)
STR(hello, hello1, hello2)
STR(,)

#define count_args(...) get_nb_args(__VA_ARGS__, 3, 2, 1)
#define get_nb_args(_1, _2, _3, n, ...) n
#define debug(...) debug_(count_args(__VA_ARGS__), __VA_ARGS__)
#define debug_(a, ...) debug##a(__VA_ARGS__)
#define debug1(a) debug_##a();
#define debug2(a, b) debug1(a) debug_##b();
#define debug3(a, b, c) debug2(a, b) debug_##c();

debug()
debug(jml)
debug(one, two, three)

debug1(
jjj jm
)

#pragma c5_clear_tokens
??=define arraycheck(a,b) a??(b??) ??!??! b??(a??)
arraycheck(A,B);
#pragma c5_compare_tokens A[B] || B[A];

#pragma c5_clear_tokens
"a//b"
#pragma c5_compare_tokens "a//b"

#pragma c5_clear_tokens
// */
#pragma c5_compare_tokens 

#pragma c5_clear_tokens
f = g/**//h
#pragma c5_compare_tokens f = g /h

#pragma c5_clear_tokens
//\
i();
#pragma c5_compare_tokens 

#pragma c5_clear_tokens
/\
/ j();
#pragma c5_compare_tokens 
 
#pragma c5_clear_tokens
/*//*/ l();
#pragma c5_compare_tokens l();

#pragma c5_clear_tokens
m = n//***/o
 + p;
#pragma c5_compare_tokens m = n + p;

 
 


#pragma c5_clear_tokens
#define hash_hash # ## #
#define mkstr(a) # a
#define in_between(a) mkstr(a)
#define join(c, d) in_between(c hash_hash d)
char p[] = join(x, y); // equivalent to
#pragma c5_compare_tokens char p[] = "x ## y";

#define x 3
#define f(a) f(x * (a))
#undef x

#define x 2

#define g f
#define z z[0]
#define h g(~

#define m(a) a(w)
#define w 0,1
#define t(a) a
#define p() int
#define q(x) x
#define r(x,y) x ## y
#define str(x) # x

#pragma c5_clear_tokens
f(y+1) + f(f(z)) % t(t(g)(0) + t)(1);
#pragma c5_compare_tokens f(2 * (y+1)) + f(2 * (f(2 * (z[0])))) % f(2 * (0)) + t(1);

#pragma c5_clear_tokens
g(x+(3,4)-w) | h 5) & m
	(f)^m(m);
#pragma c5_compare_tokens f(2 * (2+(3,4)-0,1)) | f(2 * (~ 5)) & f(2 * (0,1))^m(0,1);

#pragma c5_clear_tokens
p() i[q()] = { q(1), r(2,3), r(4,), r(,5), r(,) };
#pragma c5_compare_tokens int i[] = { 1, 23, 4, 5, };

#pragma c5_clear_tokens
char c[2][6] = { str(hello), str() };
#pragma c5_compare_tokens char c[2][6] = { "hello", "" };

#undef str
#undef debug
#undef h
#define str(s) # s
#define xstr(s) str(s)
#define debug(s, t) printf("x" # s "= %d, x" # t "= %s", \
x ## s, x ## t)
#define INCFILE(n) vers ## n
#define glue(a, b) a ## b
#define xglue(a, b) glue(a, b)
#define HIGHLOW "hello"
#define LOW LOW ", world"

#pragma c5_clear_tokens
debug(1, 2);
#pragma c5_compare_tokens printf("x" "1" "= %d, x" "2" "= %s", x1, x2);

#pragma c5_clear_tokens
fputs(str(strncmp("abc\0d", "abc", '\4') // this goes away
== 0) str(: @\n), s);
#pragma c5_compare_tokens fputs("strncmp(\"abc\\0d\", \"abc\", '\\4') == 0" ": @\n", s);

#pragma c5_clear_tokens
include xstr(INCFILE(2).h)
#pragma c5_compare_tokens include "vers2.h"

#pragma c5_clear_tokens
glue(HIGH, LOW);
#pragma c5_compare_tokens "hello";

#pragma c5_clear_tokens
xglue(HIGH, LOW)
#pragma c5_compare_tokens "hello" ", world"

#undef t
#define t(x,y,z) x ## y ## z
#pragma c5_clear_tokens
int j[] = { t(1,2,3), t(,4,5), t(6,,7), t(8,9,),
		t(10,,), t(,11,), t(,,12), t(,,) };
#pragma c5_compare_tokens int j[] = { 123, 45, 67, 89, 10, 11, 12, };

#undef debug
#undef x
#define debug(...) fprintf(stderr, __VA_ARGS__)
#define showlist(...) puts(#__VA_ARGS__)
#define report(test, ...) ((test)?puts(#test):\
	printf(__VA_ARGS__))

#pragma c5_clear_tokens
debug("Flag");
#pragma c5_compare_tokens fprintf(stderr, "Flag");

#pragma c5_clear_tokens
debug("X = %d\n", x);
#pragma c5_compare_tokens fprintf(stderr, "X = %d\n", x);

#pragma c5_clear_tokens
showlist(The first, second, and third items.);
#pragma c5_compare_tokens  puts("The first, second, and third items.");

#pragma c5_clear_tokens
report(x>y, "x is %d but y is %d", x, y);
#pragma c5_compare_tokens ((x>y)?puts("x>y"): printf( "x is %d but y is %d", x, y));

#define OBJ_LIKE (1-1)
#define OBJ_LIKE /* white space */ (1-1) /* other */
#define FUNC_LIKE(a) ( a )
#define FUNC_LIKE( a )( /* note the white space */ \
	a /* other stuff on this line
		*/ )
#line __LINE__ __FILE__
#include "test_file02.h"

