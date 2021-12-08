#ifndef _DUAL_DOUBLE_H_
#define _DUAL_DOUBLE_H_
#include "dualfloat_basic.h"

/**
 * dualdouble有107位精度
 * 实现四则运算的精度与表示精度一样，是107位精度(略微大于0.5ulps)，除了以下函数：(根据a15res-joldes.pdf)
 * fdfadd,fdfsub,fdfsubr函数计算结果通常有106位精度，最坏情况有105位精度
 * fdf2add,fdf2sub函数计算结果通常有106位精度，最坏情况有104位精度
 * sdf2add,sdf2sub函数通常有较大误差(用于提升单数运算精度是可行的)，实际进行加法时(绝对值增大)至少有103位精度
 * fdfmul函数计算结果通常有106位精度，最坏情况有104位精度
 * fdfdiv函数计算结果通常有106位精度，最坏情况有104位精度
 * fdfdivr函数计算结果通常有106位精度，最坏情况有104位精度
 * fdf2mul,fdfsqr函数计算结果通常有105位精度，最坏情况有103位精度
 * fdf2div函数计算结果通常有106位精度，最坏情况有104位精度
 */

/* dualdouble与double相加(精度略低但更快)得到dualdouble */
inline dualdouble fdfadd(dualdouble a, double b) {
  dualdouble ret = dadd(a.hi, b);
  ret.lo += a.lo; // this is err come
  return dfnorm(ret);
}

/* dualdouble与double相减(精度略低但更快)得到dualdouble */
inline dualdouble fdfsub(dualdouble a, double b) {
  dualdouble ret = dsub(a.hi, b);
  ret.lo += a.lo;
  return dfnorm(ret);
}

/* double与dualdouble相减(精度略低但更快)得到dualdouble */
inline dualdouble fdfsubr(double a, dualdouble b) {
  dualdouble ret = dsub(a, b.hi);
  ret.lo -= b.lo;
  return dfnorm(ret);
}

/* dualdouble与double相加得到dualdouble */
inline dualdouble dfadd(dualdouble a, double b) {
  dualdouble ret;
  double r0;
  size_t erpb = erpmark(b);
  if (erpb >= erpmark(a.hi)) {
    r0 = b;
    ret = a;
  } else {
    if (erpb <= erpmark(a.lo))
      ret = dfnorm(ddual(a.lo, b));
    else
      ret = dfnorm(ddual(b, a.lo));
    r0 = a.hi;
  }
  b = ret.lo;
  ret = dfnorm(ddual(r0, ret.hi));
  r0 = ret.lo;
  ret = dfnorm(ddual(ret.hi, b));
  ret.lo += r0; // this err is correct rounding
  return ret;
}

/* dualdouble与double相减得到dualdouble */
inline dualdouble dfsub(dualdouble a, double b) {
  dualdouble ret;
  double r0;
  size_t erpb = erpmark(b);
  if (erpb >= erpmark(a.hi)) {
    r0 = -b;
    ret = a;
  } else {
    if (erpb <= erpmark(a.lo))
      ret = dfnlonorm(ddual(a.lo, b));
    else
      ret = dfnhinorm(ddual(b, a.lo));
    r0 = a.hi;
  }
  b = ret.lo;
  ret = dfnorm(ddual(r0, ret.hi));
  r0 = ret.lo;
  ret = dfnorm(ddual(ret.hi, b));
  ret.lo += r0;
  return ret;
}

/* double与dualdouble相减得到dualdouble */
inline dualdouble dfsubr(double a, dualdouble b) {
  dualdouble ret;
  double r0;
  size_t erpa = erpmark(a);
  if (erpa >= erpmark(b.hi)) {
    r0 = a;
    ret = b;
  } else {
    if (erpa <= erpmark(b.lo))
      ret = dfnlonorm(ddual(b.lo, a));
    else
      ret = dfnhinorm(ddual(a, b.lo));
    r0 = -b.hi;
  }
  a = ret.lo;
  ret = dfnlonorm(ddual(r0, ret.hi));
  r0 = ret.lo;
  ret = dfnlonorm(ddual(ret.hi, a));
  ret.lo += r0;
  return ret;
}

/* dualdouble加法(低精度但很快,只遵循源误差) */
inline dualdouble sdf2add(dualdouble a, dualdouble b) {
  dualdouble ret = dadd(a.hi, b.hi);
  ret.lo += (a.lo + b.lo);
  return dfnorm(ret);
}

/* dualdouble减法(低精度但很快,只遵循源误差) */
inline dualdouble sdf2sub(dualdouble a, dualdouble b) {
  dualdouble ret = dsub(a.hi, b.hi);
  ret.lo += (a.lo - b.lo);
  return dfnorm(ret);
}

/* dualdouble加法(精度略低但更快) */
inline dualdouble fdf2add(dualdouble a, dualdouble b) {
  dualdouble ret, tmp;
  df2reorder(&a, &b, 2);
  ret = dfnorm(ddual(a.hi, b.hi));
  tmp = dfnorm(ddual(a.lo, b.lo));
  ret = dfnorm(ddual(ret.hi, ret.lo + b.hi));
  ret = dfnorm(ddual(ret.hi, ret.lo + b.lo));
  return ret;
}

/* dualdouble减法(精度略低但更快) */
inline dualdouble fdf2sub(dualdouble a, dualdouble b) {
  dualdouble ret, tmp;
  df2reorder(&a, &b, 3);
  ret = dfnorm(ddual(a.hi, b.hi));
  tmp = dfnorm(ddual(a.lo, b.lo));
  ret = dfnorm(ddual(ret.hi, ret.lo + b.hi));
  ret = dfnorm(ddual(ret.hi, ret.lo + b.lo));
  return ret;
}

/* dualfloat加法 */
inline dualdouble df2add(dualdouble a, dualdouble b) {
  dualdouble ret, tmp;
  double r0, r1, r2, r3;
  df2reorder(&a, &b, 2);
  ret = dfnorm(ddual(a.hi, b.hi));
  tmp = dfnorm(ddual(a.lo, b.lo));
  r3 = tmp.lo;
  tmp = dadd(ret.lo, tmp.hi);
  r2 = tmp.lo;
  ret = dfnorm(ddual(ret.hi, tmp.hi));
  r1 = r2 + r3;
  if (dual_likely(ret.lo != 0.0))
    ret.lo += r1; // this branch is likely
  else {
    r0 = ret.hi;
    ret.hi += r1;
    ret.lo = ((r0 - ret.hi) + r2) + r3;
  }
  return ret;
}

/* dualfloat减法 */
inline dualdouble df2sub(dualdouble a, dualdouble b) {
  dualdouble ret, tmp;
  double r0, r1, r2, r3;
  df2reorder(&a, &b, 3);
  ret = dfnorm(ddual(a.hi, b.hi));
  tmp = dfnorm(ddual(a.lo, b.lo));
  r3 = tmp.lo;
  tmp = dadd(ret.lo, tmp.hi);
  r2 = tmp.lo;
  ret = dfnorm(ddual(ret.hi, tmp.hi));
  r1 = r2 + r3;
  if (dual_likely(ret.lo != 0.0))
    ret.lo += r1;
  else {
    r0 = ret.hi;
    ret.hi += r1;
    ret.lo = ((r0 - ret.hi) + r2) + r3;
  }
  return ret;
}

/* dualdouble与double相乘(精度略低但更快)得到dualdouble */
inline dualdouble fdfmul(dualdouble a, double b) {
  dualdouble ret = dmul(a.hi, b);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMA))
  ret.lo = fmuladd(a.lo, b, ret.lo);
#else
  ret.lo += b * a.lo;
#endif
  return dfnorm(ret);
}

/* dualdouble与double相除(精度略低但更快)得到dualdouble */
inline dualdouble fdfdiv(dualdouble a, double b) {
  dualdouble ret;
  ret = dmdiv(a.hi, b);
  ret.lo = (ret.lo + a.lo) / b;
  return dfnorm(ret);
}

/* double与dualdouble相除(精度略低但更快)得到dualdouble */
inline dualdouble fdfdivr(double a, dualdouble b) {
  dualdouble ret;
  ret = dmdiv(a, b.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMA))
  ret.lo = nfmulsub(ret.hi, b.lo, ret.lo) / b.hi;
#else
  ret.lo = (ret.lo - ret.hi * b.lo) / b.hi;
#endif
  return dfnorm(ret);
}

/* dualdouble乘法(精度略低但更快) */
inline dualdouble fdf2mul(dualdouble a, dualdouble b) {
  dualdouble ret = dmul(a.hi, b.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMA))
  ret.lo += fmuladd(a.lo, b.hi, fmuladd(a.hi, b.lo, a.lo * b.lo));
#else
  ret.lo += a.hi * b.lo + b.hi * a.lo;
#endif
  return dfnorm(ret);
}

/* dualdouble除法(精度略低但更快) */
inline dualdouble fdf2div(dualdouble a, dualdouble b) {
  dualdouble ret;
  ret = dmdiv(a.hi, b.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMA))
  ret.lo = (ret.lo + nfmulsub(ret.hi, b.lo, a.lo)) / b.hi;
#else
  ret.lo = (ret.lo + a.lo - ret.hi * b.lo) / b.hi;
#endif
  return dfnorm(ret);
}

/* dualdouble平方(精度略低但更快) */
inline dualdouble fdfsqr(dualdouble a) {
  dualdouble ret = dsqr(a.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMA))
  ret.lo = fmuladd(a.hi + a.hi, a.lo, ret.lo);
#else
  ret.lo += (a.hi + a.hi) * a.lo;
#endif
  return dfnorm(ret);
}

/* dualdouble与double相乘得到dualdouble */
inline dualdouble dfmul(dualdouble a, double b) {
  dualdouble ret, tmp, tmp2;
  ret = dmul(a.hi, b);
  tmp = dmul(a.lo, b);
  if (erpmark(ret.lo) > erpmark(tmp.hi)) {
    tmp2 = dfnorm(ddual(ret.lo, tmp.hi));
    /* 双舍入需要舍入到奇数以保证正确舍入，而默认浮点环境不保证这一点 */
    tmp2.lo += tmp.lo;
  } else {
    tmp2 = dfnorm(ddual(tmp.hi, ret.lo));
    tmp2.lo += tmp.lo;
    tmp2 = dfnorm(tmp2);
  }
  ret = dfnorm(ddual(ret.hi, tmp2.hi));
  ret.lo += tmp2.lo;
  return ret;
}

/* dualdouble与double相除得到dualdouble */
inline dualdouble dfdiv(dualdouble a, double b) {
  dualdouble ret, tmp;
  ret = dmdiv(a.hi, b);
  a = dfnorm(ddual(ret.lo, a.lo));
  double rb = 1.0 / b;
  tmp.hi = a.hi * rb;
  tmp.lo = nfmulsub_lim(tmp.hi, b, a.hi);
  tmp.lo = (tmp.lo + a.lo) * rb;
  tmp = dfnorm(tmp);
  ret = dfnorm(ddual(ret.hi, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

/* double与dualdouble相除得到dualdouble */
inline dualdouble dfdivr(double a, dualdouble b) {
  dualdouble ret, tmp, tmp2;
  double r0, r1, r2, r3;
  ret = dmdiv(a, b.hi);
  r0 = 1.0 / b.hi;
  r1 = ret.hi;
  tmp2 = dmul(r1, b.lo);
  tmp = dsub(ret.lo, tmp2.hi);
  tmp.lo -= tmp2.lo;
  r2 = tmp.hi * r0;
  r3 = nfmulsub_lim(r2, b.hi, tmp.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMA))
  r3 -= fmulsub(r2, b.lo, tmp.lo);
#else
  r3 += tmp.lo - r2 * b.lo;
#endif
  r3 *= r0;
  tmp = dfnorm(ddual(r2, r3));
  ret = dfnorm(ddual(r1, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

/* dualdouble乘法 */
inline dualdouble df2mul(dualdouble a, dualdouble b) {
  dualdouble ret, tmp, tmp2;
  double r0;
  r0 = a.lo * b.lo;
  tmp = dmul(a.hi, b.lo);
  tmp2 = dmul(a.lo, b.hi);
  ret = dmul(a.hi, b.hi);
  r0 += tmp.lo + tmp2.lo;
  tmp = dadd(tmp.hi, tmp2.hi);
  r0 += tmp.lo;
  tmp = dadd(ret.lo, tmp.hi);
  tmp.lo += r0;
  tmp = dfnorm(tmp);
  ret = dfnorm(ddual(ret.hi, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

/* dualdouble除法 */
inline dualdouble df2div(dualdouble a, dualdouble b) {
  dualdouble ret, tmp, tmp2;
  double r0, r1, r2, r3;
  ret = dmdiv(a.hi, b.hi);
  r0 = 1.0 / b.hi;
  r1 = ret.hi;
  tmp2 = dfnorm(ddual(ret.lo, a.lo));
  tmp = dmul(r1, b.lo);
  tmp2.lo -= tmp.lo;
  tmp = dsub(tmp2.hi, tmp.hi);
  tmp.lo += tmp2.lo;
  r2 = tmp.hi * r0;
  r3 = nfmulsub_lim(r2, b.hi, tmp.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMA))
  r3 -= fmulsub(r2, b.lo, tmp.lo);
#else
  r3 += tmp.lo - r2 * b.lo;
#endif
  r3 *= r0;
  tmp = dfnorm(ddual(r2, r3));
  ret = dfnorm(ddual(r1, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

/* dualdouble平方 */
inline dualdouble dfsqr(dualdouble a) {
  dualdouble ret, tmp;
  double r0;
  r0 = a.lo * a.lo;
  tmp = dmul(a.hi, a.lo);
  tmp.hi += tmp.hi;
  tmp.lo += tmp.lo;
  ret = dsqr(a.hi);
  r0 += tmp.lo;
  tmp = dadd(ret.lo, tmp.hi);
  tmp.lo += r0;
  tmp = dfnorm(tmp);
  ret = dfnorm(ddual(ret.hi, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

/* double倒数 */
inline dualdouble drcp(double a) {
  dualdouble ret, tmp;
  double r0, r1, r2, r3;
  r1 = r0 = 1.0 / a;
  r3 = r2 = nfmulsub_lim(r0, a, 1.0);
  r2 *= r0;
  r3 = nfmulsub_lim(r2, a, r3);
  r3 *= r0;
  tmp = dfnorm(ddual(r2, r3));
  ret = dfnorm(ddual(r1, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

/* dualdouble倒数 */
inline dualdouble dfrcp(dualdouble a) {
  dualdouble ret, tmp, tmp2;
  double r0, r1, r2, r3;
  r1 = r0 = 1.0 / a.hi;
  r2 = nfmulsub_lim(r0, a.hi, 1.0);
  tmp2 = dmul(r1, a.lo);
  tmp = dsub(r2, tmp2.hi);
  tmp.lo -= tmp2.lo;
  r2 = tmp.hi * r0;
  r3 = nfmulsub_lim(r2, a.hi, tmp.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMA))
  r3 -= fmulsub(r2, a.lo, tmp.lo);
#else
  r3 += tmp.lo - r2 * a.lo;
#endif
  r3 *= r0;
  tmp = dfnorm(ddual(r2, r3));
  ret = dfnorm(ddual(r1, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

#if defined(__cplusplus) || defined(c_plusplus)
#include "dualdouble_cxx.h"
#endif

#endif