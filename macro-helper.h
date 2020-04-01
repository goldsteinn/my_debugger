#ifndef _MACRO_HELPER_H_
#define _MACRO_HELPER_H_

/* This counts the number of args */
#define NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8, _9, _10, N,...) N
#define NARGS(...) NARGS_SEQ(__VA_ARGS__, 9, 10, 8, 7, 6, 5, 4, 3, 2, 1)

/* This will let macros expand before concating them */
#define PRIMITIVE_CAT(x, y) x ## y
#define CAT(x, y) PRIMITIVE_CAT(x, y)

/* This will call a macro on each argument passed in */
#define APPLY(macro, ...) CAT(APPLY_, NARGS(__VA_ARGS__))(macro, __VA_ARGS__)
#define APPLY_1(m, x1) m(x1)
#define APPLY_2(m, x1, x2) m(x1), m(x2)
#define APPLY_3(m, x1, x2, x3) m(x1), m(x2), m(x3)
#define APPLY_4(m, x1, x2, x3, x4) m(x1), m(x2), m(x3), m(x4)
#define APPLY_5(m, x1, x2, x3, x4, x5) m(x1), m(x2), m(x3), m(x4), m(x5)
#define APPLY_6(m, x1, x2, x3, x4, x5, x6) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6)
#define APPLY_7(m, x1, x2, x3, x4, x5, x6, x7) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7)
#define APPLY_8(m, x1, x2, x3, x4, x5, x6, x7, x8) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8)
#define APPLY_9(m, x1, x2, x3, x4, x5, x6, x7, x8, x9) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9)
#define APPLY_10(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10)

#define SUM(macro, ...) CAT(SUM_, NARGS(__VA_ARGS__))(macro, __VA_ARGS__)
#define SUM_1(m, x1) m(x1)
#define SUM_2(m, x1, x2) m(x1) + m(x2)
#define SUM_3(m, x1, x2, x3) m(x1) + m(x2) + m(x3)
#define SUM_4(m, x1, x2, x3, x4) m(x1) + m(x2) + m(x3) + m(x4)
#define SUM_5(m, x1, x2, x3, x4, x5) m(x1) + m(x2) + m(x3) + m(x4) + m(x5)
#define SUM_6(m, x1, x2, x3, x4, x5, x6) m(x1) + m(x2) + m(x3) + m(x4) + m(x5) + m(x6)
#define SUM_7(m, x1, x2, x3, x4, x5, x6, x7) m(x1) + m(x2) + m(x3) + m(x4) + m(x5) + m(x6) + m(x7)
#define SUM_8(m, x1, x2, x3, x4, x5, x6, x7, x8) m(x1) + m(x2) + m(x3) + m(x4) + m(x5) + m(x6) + m(x7), m(x8)
#define SUM_9(m, x1, x2, x3, x4, x5, x6, x7, x8, x9) m(x1) + m(x2) + m(x3) + m(x4) + m(x5) + m(x6) + m(x7) + m(x8) + m(x9)
#define SUM_10(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) m(x1) + m(x2) + m(x3) + m(x4) + m(x5) + m(x6) + m(x7) + m(x8) + m(x9) + m(x10)



//////////////////////////////////////////////////////////////////////
#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define PP_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0


//////////////////////////////////////////////////////////////////////
// count arguments
#define M_NARGS(...) M_NARGS_(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define M_NARGS_(_10, _9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) N

// utility (concatenation)
#define M_CONC(A, B) M_CONC_(A, B)
#define M_CONC_(A, B) A##B

#define M_GET_ELEM(N, ...) M_CONC(M_GET_ELEM_, N)(__VA_ARGS__)
#define M_GET_ELEM_0(_0, ...) _0
#define M_GET_ELEM_1(_0, _1, ...) _1
#define M_GET_ELEM_2(_0, _1, _2, ...) _2
#define M_GET_ELEM_3(_0, _1, _2, _3, ...) _3
#define M_GET_ELEM_4(_0, _1, _2, _3, _4, ...) _4
#define M_GET_ELEM_5(_0, _1, _2, _3, _4, _5, ...) _5
#define M_GET_ELEM_6(_0, _1, _2, _3, _4, _5, _6, ...) _6
#define M_GET_ELEM_7(_0, _1, _2, _3, _4, _5, _6, _7, ...) _7
#define M_GET_ELEM_8(_0, _1, _2, _3, _4, _5, _6, _7, _8, ...) _8
#define M_GET_ELEM_9(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, ...) _9
#define M_GET_ELEM_10(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, ...) _10

// Get last argument - placeholder decrements by one
#define M_GET_LAST(...) M_GET_ELEM(M_NARGS(__VA_ARGS__), _, __VA_ARGS__ ,,,,,,,,,,,)


//////////////////////////////////////////////////////////////////////

#define APPLY_X(macro, X, ...) CAT(APPLY_X_, NARGS(__VA_ARGS__))(macro, X, __VA_ARGS__)
#define APPLY_X_1(m, X, x1) m(X, x1)
#define APPLY_X_2(m, X, x1, x2) m(X, x1), m(X, x2)
#define APPLY_X_3(m, X, x1, x2, x3) m(X, x1), m(X, x2), m(X, x3)
#define APPLY_X_4(m, X, x1, x2, x3, x4) m(X, x1), m(X, x2), m(X, x3), m(X, x4)
#define APPLY_X_5(m, X, x1, x2, x3, x4, x5) m(X, x1), m(X, x2), m(X, x3), m(X, x4), m(X, x5)
#define APPLY_X_6(m, X, x1, x2, x3, x4, x5, x6) m(X, x1), m(X, x2), m(X, x3), m(X, x4), m(X, x5), m(X, x6)
#define APPLY_X_7(m, X, x1, x2, x3, x4, x5, x6, x7) m(X, x1), m(X, x2), m(X, x3), m(X, x4), m(X, x5), m(X, x6), m(X, x7)
#define APPLY_X_8(m, X, x1, x2, x3, x4, x5, x6, x7, x8) m(X, x1), m(X, x2), m(X, x3), m(X, x4), m(X, x5), m(X, x6), m(X, x7), m(X, x8)
#define APPLY_X_9(m, X, x1, x2, x3, x4, x5, x6, x7, x8, x9) m(X, x1), m(X, x2), m(X, x3), m(X, x4), m(X, x5), m(X, x6), m(X, x7), m(X, x8), m(X, x9)
#define APPLY_X_10(m, X, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) m(X, x1), m(X, x2), m(X, x3), m(X, x4), m(X, x5), m(X, x6), m(X, x7), m(X, x8), m(X, x9), m(X, x10)

//////////////////////////////////////////////////////////////////////
#endif

