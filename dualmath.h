#ifndef _DUAL_MATH_H_
#define _DUAL_MATH_H_

#ifndef _DUALFLOAT_BASIC_H_
typedef struct {
	float hi, lo;
} dualfloat;

typedef struct {
	double hi, lo;
} dualdouble;
#endif

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* 设置双数 */
dualfloat setdualf(float hi, float lo);

/* 设置双数 */
dualdouble setdual(double hi, double lo);

/* float相加得到dualfloat */
dualfloat _daddf(float a, float b);

/* float相减得到dualfloat */
dualfloat _dsubf(float a, float b);

/* float相乘得到dualfloat */
dualfloat _dmulf(float a, float b);

/* float相除得到商(ret.hi)和余数(ret.lo) */
dualfloat _dmdivf(float a, float b);

/* float相除得到(正确舍入)dualfloat */
dualfloat _ddivf(float a, float b);

/* double相加得到dualdouble */
dualdouble _dadd(double a, double b);

/* double相减得到dualdouble */
dualdouble _dsub(double a, double b);

/* double相乘得到dualdouble */
dualdouble _dmul(double a, double b);

/* double相除得到商(ret.hi)和余数(ret.lo) */
dualdouble _dmdiv(double a, double b);

/* double相除得到(正确舍入)dualdouble */
dualdouble _ddiv(double a, double b);


/* dualfloat与float相加得到dualfloat */
dualfloat _dfaddf(dualfloat a, float b);

/* dualfloat与float相减得到dualfloat */
dualfloat _dfsubf(dualfloat a, float b);

/* float与dualfloat相减得到dualfloat */
dualfloat _dfsubrf(float a, dualfloat b);

/* dualfloat加法 */
dualfloat _df2addf(dualfloat a, dualfloat b);

/* dualfloat减法 */
dualfloat _df2subf(dualfloat a, dualfloat b);

/* dualfloat与float相乘得到dualfloat */
dualfloat _dfmulf(dualfloat a, float b);

/* dualfloat与float相除得到dualfloat */
dualfloat _dfdivf(dualfloat a, float b);

/* float与dualfloat相除得到dualfloat */
dualfloat _dfdivrf(float a, dualfloat b);

/* dualfloat乘法 */
dualfloat _df2mulf(dualfloat a, dualfloat b);

/* dualfloat除法 */
dualfloat _df2divf(dualfloat a, dualfloat b);

/* dualfloat平方 */
dualfloat _dfsqrf(dualfloat a);

/* float倒数 */
dualfloat _drcpf(float a);

/* dualfloat倒数 */
dualfloat _dfrcpf(dualfloat a);


/* dualdouble与double相加得到dualdouble */
dualdouble _dfadd(dualdouble a, double b);

/* dualdouble与double相减得到dualdouble */
dualdouble _dfsub(dualdouble a, double b);

/* double与dualdouble相减得到dualdouble */
dualdouble _dfsubr(double a, dualdouble b);

/* dualdouble加法 */
dualdouble _df2add(dualdouble a, dualdouble b);

/* dualdouble减法 */
dualdouble _df2sub(dualdouble a, dualdouble b);

/* dualdouble与double相乘得到dualdouble */
dualdouble _dfmul(dualdouble a, double b);

/* dualdouble与double相除得到dualdouble */
dualdouble _dfdiv(dualdouble a, double b);

/* double与dualdouble相除得到dualdouble */
dualdouble _dfdivr(double a, dualdouble b);

/* dualdouble乘法 */
dualdouble _df2mul(dualdouble a, dualdouble b);

/* dualdouble除法 */
dualdouble _df2div(dualdouble a, dualdouble b);

/* dualdouble平方 */
dualdouble _dfsqr(dualdouble a);

/* double倒数 */
dualdouble _drcp(double a);

/* dualdouble倒数 */
dualdouble _dfrcp(dualdouble a);


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
