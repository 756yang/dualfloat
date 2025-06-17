#include "dualfloat.h"
#include "dualmath.h"


/* 设置双数 */
dualfloat setdualf(float hi, float lo) {
	return dfnormf(ddualf(hi,lo));
}

/* 设置双数 */
dualdouble setdual(double hi, double lo) {
	return dfnorm(ddual(hi,lo));
}

/* float相加得到dualfloat */
dualfloat _daddf(float a, float b) {
	return daddf(a,b);
}

/* float相减得到dualfloat */
dualfloat _dsubf(float a, float b) {
	return dsubf(a,b);
}

/* float相乘得到dualfloat */
dualfloat _dmulf(float a, float b) {
	return dmulf(a,b);
}

/* float相除得到商(ret.hi)和余数(ret.lo) */
dualfloat _dmdivf(float a, float b) {
	return dmdivf(a,b);
}

/* float相除得到(正确舍入)dualfloat */
dualfloat _ddivf(float a, float b) {
	return ddivf(a,b);
}

/* double相加得到dualdouble */
dualdouble _dadd(double a, double b) {
	return dadd(a,b);
}

/* double相减得到dualdouble */
dualdouble _dsub(double a, double b) {
	return dsub(a,b);
}

/* double相乘得到dualdouble */
dualdouble _dmul(double a, double b) {
	return dmul(a,b);
}

/* double相除得到商(ret.hi)和余数(ret.lo) */
dualdouble _dmdiv(double a, double b) {
	return dmdiv(a,b);
}

/* double相除得到(正确舍入)dualdouble */
dualdouble _ddiv(double a, double b) {
	return ddiv(a,b);
}


/* dualfloat与float相加得到dualfloat */
dualfloat _dfaddf(dualfloat a, float b) {
	return dfaddf(a,b);
}

/* dualfloat与float相减得到dualfloat */
dualfloat _dfsubf(dualfloat a, float b) {
	return dfsubf(a,b);
}

/* float与dualfloat相减得到dualfloat */
dualfloat _dfsubrf(float a, dualfloat b) {
	return dfsubrf(a,b);
}

/* dualfloat加法 */
dualfloat _df2addf(dualfloat a, dualfloat b) {
	return df2addf(a,b);
}

/* dualfloat减法 */
dualfloat _df2subf(dualfloat a, dualfloat b) {
	return df2subf(a,b);
}

/* dualfloat与float相乘得到dualfloat */
dualfloat _dfmulf(dualfloat a, float b) {
	return dfmulf(a,b);
}

/* dualfloat与float相除得到dualfloat */
dualfloat _dfdivf(dualfloat a, float b) {
	return dfdivf(a,b);
}

/* float与dualfloat相除得到dualfloat */
dualfloat _dfdivrf(float a, dualfloat b) {
	return dfdivrf(a,b);
}

/* dualfloat乘法 */
dualfloat _df2mulf(dualfloat a, dualfloat b) {
	return df2mulf(a,b);
}

/* dualfloat除法 */
dualfloat _df2divf(dualfloat a, dualfloat b) {
	return df2divf(a,b);
}

/* dualfloat平方 */
dualfloat _dfsqrf(dualfloat a) {
	return dfsqrf(a);
}

/* float倒数 */
dualfloat _drcpf(float a) {
	return drcpf(a);
}

/* dualfloat倒数 */
dualfloat _dfrcpf(dualfloat a) {
	return dfrcpf(a);
}


/* dualdouble与double相加得到dualdouble */
dualdouble _dfadd(dualdouble a, double b) {
	return dfadd(a,b);
}

/* dualdouble与double相减得到dualdouble */
dualdouble _dfsub(dualdouble a, double b) {
	return dfsub(a,b);
}

/* double与dualdouble相减得到dualdouble */
dualdouble _dfsubr(double a, dualdouble b) {
	return dfsubr(a,b);
}

/* dualdouble加法 */
dualdouble _df2add(dualdouble a, dualdouble b) {
	return df2add(a,b);
}

/* dualdouble减法 */
dualdouble _df2sub(dualdouble a, dualdouble b) {
	return df2sub(a,b);
}

/* dualdouble与double相乘得到dualdouble */
dualdouble _dfmul(dualdouble a, double b) {
	return dfmul(a,b);
}

/* dualdouble与double相除得到dualdouble */
dualdouble _dfdiv(dualdouble a, double b) {
	return dfdiv(a,b);
}

/* double与dualdouble相除得到dualdouble */
dualdouble _dfdivr(double a, dualdouble b) {
	return dfdivr(a,b);
}

/* dualdouble乘法 */
dualdouble _df2mul(dualdouble a, dualdouble b) {
	return df2mul(a,b);
}

/* dualdouble除法 */
dualdouble _df2div(dualdouble a, dualdouble b) {
	return df2div(a,b);
}

/* dualdouble平方 */
dualdouble _dfsqr(dualdouble a) {
	return dfsqr(a);
}

/* double倒数 */
dualdouble _drcp(double a) {
	return drcp(a);
}

/* dualdouble倒数 */
dualdouble _dfrcp(dualdouble a) {
	return dfrcp(a);
}

