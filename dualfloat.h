#ifndef _DUAL_FLOAT_H_
#define _DUAL_FLOAT_H_
#include "dualfloat_basic.h"

/**
 * dualfloat的表示有49位精度
 * 实现四则运算的精度与表示精度一样，是49位精度(略微大于0.5ulps)，除了以下函数：(根据a15res-joldes.pdf)
 * fdfaddf,fdfsubf,fdfsubrf函数计算结果通常有48位精度，最坏情况有47位精度
 * fdf2addf,fdf2subf函数计算结果通常有48位精度，最坏情况有46位精度
 * sdf2addf,sdf2subf函数通常有较大误差(用于提升单数运算精度是可行的)，实际进行加法时(绝对值增大)至少有45位精度
 * fdfmulf函数计算结果通常有48位精度，最坏情况有46位精度
 * fdfdivf函数计算结果通常有48位精度，最坏情况有46位精度
 * fdfdivrf函数计算结果通常有48位精度，最坏情况有46位精度
 * fdf2mulf,fdfsqrf函数计算结果通常有47位精度，最坏情况有45位精度
 * fdf2divf函数计算结果通常有48位精度，最坏情况有46位精度
 */

/* dualfloat与float相加(精度略低但更快)得到dualfloat */
inline dualfloat fdfaddf(dualfloat a, float b) {
  dualfloat ret = daddf(a.hi, b);
  ret.lo += a.lo; // this is err come
  return dfnormf(ret);
}

/* dualfloat与float相减(精度略低但更快)得到dualfloat */
inline dualfloat fdfsubf(dualfloat a, float b) {
  dualfloat ret = dsubf(a.hi, b);
  ret.lo += a.lo;
  return dfnormf(ret);
}

/* float与dualfloat相减(精度略低但更快)得到dualfloat */
inline dualfloat fdfsubrf(float a, dualfloat b) {
  dualfloat ret = dsubf(a, b.hi);
  ret.lo -= b.lo;
  return dfnormf(ret);
}

/* dualfloat与float相加得到dualfloat */
inline dualfloat dfaddf(dualfloat a, float b) {
  dualfloat ret;
  float r0;
  uint32_t erpb = erpmarkf(b);
  if (erpb >= erpmarkf(a.hi)) {
    r0 = b;
    ret = a;
  } else {
    if (erpb <= erpmarkf(a.lo))
      ret = dfnormf(ddualf(a.lo, b));
    else
      ret = dfnormf(ddualf(b, a.lo));
    r0 = a.hi;
  }
  b = ret.lo;
  ret = dfnormf(ddualf(r0, ret.hi));
  r0 = ret.lo;
  ret = dfnormf(ddualf(ret.hi, b));
  ret.lo += r0; // this err is correct rounding
  return ret;
}

/* dualfloat与float相减得到dualfloat */
inline dualfloat dfsubf(dualfloat a, float b) {
  dualfloat ret;
  float r0;
  uint32_t erpb = erpmarkf(b);
  if (erpb >= erpmarkf(a.hi)) {
    r0 = -b;
    ret = a;
  } else {
    if (erpb <= erpmarkf(a.lo))
      ret = dfnlonormf(ddualf(a.lo, b));
    else
      ret = dfnhinormf(ddualf(b, a.lo));
    r0 = a.hi;
  }
  b = ret.lo;
  ret = dfnormf(ddualf(r0, ret.hi));
  r0 = ret.lo;
  ret = dfnormf(ddualf(ret.hi, b));
  ret.lo += r0;
  return ret;
}

/* float与dualfloat相减得到dualfloat */
inline dualfloat dfsubrf(float a, dualfloat b) {
  dualfloat ret;
  float r0;
  uint32_t erpa = erpmarkf(a);
  if (erpa >= erpmarkf(b.hi)) {
    r0 = a;
    ret = b;
  } else {
    if (erpa <= erpmarkf(b.lo))
      ret = dfnlonormf(ddualf(b.lo, a));
    else
      ret = dfnhinormf(ddualf(a, b.lo));
    r0 = -b.hi;
  }
  a = ret.lo;
  ret = dfnlonormf(ddualf(r0, ret.hi));
  r0 = ret.lo;
  ret = dfnlonormf(ddualf(ret.hi, a));
  ret.lo += r0;
  return ret;
}

/* dualfloat加法(低精度但很快,只遵循源误差) */
inline dualfloat sdf2addf(dualfloat a, dualfloat b) {
  dualfloat ret = daddf(a.hi, b.hi);
  ret.lo += (a.lo + b.lo);
  return dfnormf(ret);
}

/* dualfloat减法(低精度但很快,只遵循源误差) */
inline dualfloat sdf2subf(dualfloat a, dualfloat b) {
  dualfloat ret = dsubf(a.hi, b.hi);
  ret.lo += (a.lo - b.lo);
  return dfnormf(ret);
}

/* dualfloat加法(精度略低但更快) */
inline dualfloat fdf2addf(dualfloat a, dualfloat b) {
  dualfloat ret, tmp;
  df2reorderf(&a, &b, 2);
  ret = dfnormf(ddualf(a.hi, b.hi));
  tmp = dfnormf(ddualf(a.lo, b.lo));
  ret = dfnormf(ddualf(ret.hi, ret.lo + b.hi));
  ret = dfnormf(ddualf(ret.hi, ret.lo + b.lo));
  return ret;
}

/* dualfloat减法(精度略低但更快) */
inline dualfloat fdf2subf(dualfloat a, dualfloat b) {
  dualfloat ret, tmp;
  df2reorderf(&a, &b, 3);
  ret = dfnormf(ddualf(a.hi, b.hi));
  tmp = dfnormf(ddualf(a.lo, b.lo));
  ret = dfnormf(ddualf(ret.hi, ret.lo + b.hi));
  ret = dfnormf(ddualf(ret.hi, ret.lo + b.lo));
  return ret;
}

/* dualfloat加法 */
inline dualfloat df2addf(dualfloat a, dualfloat b) {
  dualfloat ret, tmp;
  float r0, r1, r2, r3;
  df2reorderf(&a, &b, 2);
  ret = dfnormf(ddualf(a.hi, b.hi));
  tmp = dfnormf(ddualf(a.lo, b.lo));
  r3 = tmp.lo;
  tmp = daddf(ret.lo, tmp.hi);
  r2 = tmp.lo;
  ret = dfnormf(ddualf(ret.hi, tmp.hi));
  r1 = r2 + r3;
  if (dual_likely(ret.lo != 0.0f))
    ret.lo += r1; // this branch is likely
  else {
    r0 = ret.hi;
    ret.hi += r1;
    ret.lo = ((r0 - ret.hi) + r2) + r3;
  }
  return ret;
}

/* dualfloat减法 */
inline dualfloat df2subf(dualfloat a, dualfloat b) {
  dualfloat ret, tmp;
  float r0, r1, r2, r3;
  df2reorderf(&a, &b, 3);
  ret = dfnormf(ddualf(a.hi, b.hi));
  tmp = dfnormf(ddualf(a.lo, b.lo));
  r3 = tmp.lo;
  tmp = daddf(ret.lo, tmp.hi);
  r2 = tmp.lo;
  ret = dfnormf(ddualf(ret.hi, tmp.hi));
  r1 = r2 + r3;
  if (dual_likely(ret.lo != 0.0f))
    ret.lo += r1;
  else {
    r0 = ret.hi;
    ret.hi += r1;
    ret.lo = ((r0 - ret.hi) + r2) + r3;
  }
  return ret;
}

/* dualfloat与float相乘(精度略低但更快)得到dualfloat */
inline dualfloat fdfmulf(dualfloat a, float b) {
  dualfloat ret = dmulf(a.hi, b);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMAF))
  ret.lo = fmuladdf(a.lo, b, ret.lo);
#else
  ret.lo += b * a.lo;
#endif
  return dfnormf(ret);
}

/* dualfloat与float相除(精度略低但更快)得到dualfloat */
inline dualfloat fdfdivf(dualfloat a, float b) {
  dualfloat ret;
  ret = dmdivf(a.hi, b);
  ret.lo = (ret.lo + a.lo) / b;
  return dfnormf(ret);
}

/* float与dualfloat相除(精度略低但更快)得到dualfloat */
inline dualfloat fdfdivrf(float a, dualfloat b) {
  dualfloat ret;
  ret = dmdivf(a, b.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMAF))
  ret.lo = nfmulsubf(ret.hi, b.lo, ret.lo) / b.hi;
#else
  ret.lo = (ret.lo - ret.hi * b.lo) / b.hi;
#endif
  return dfnormf(ret);
}

/* dualfloat乘法(精度略低但更快) */
inline dualfloat fdf2mulf(dualfloat a, dualfloat b) {
  dualfloat ret = dmulf(a.hi, b.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMAF))
  ret.lo += fmuladdf(a.lo, b.hi, fmuladdf(a.hi, b.lo, a.lo * b.lo));
#else
  ret.lo += a.hi * b.lo + b.hi * a.lo;
#endif
  return dfnormf(ret);
}

/* dualfloat除法(精度略低但更快) */
inline dualfloat fdf2divf(dualfloat a, dualfloat b) {
  dualfloat ret;
  ret = dmdivf(a.hi, b.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMAF))
  ret.lo = (ret.lo + nfmulsubf(ret.hi, b.lo, a.lo)) / b.hi;
#else
  ret.lo = (ret.lo + a.lo - ret.hi * b.lo) / b.hi;
#endif
  return dfnormf(ret);
}

/* dualfloat平方(精度略低但更快) */
inline dualfloat fdfsqrf(dualfloat a) {
  dualfloat ret = dsqrf(a.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMAF))
  ret.lo = fmuladdf(a.hi + a.hi, a.lo, ret.lo);
#else
  ret.lo += (a.hi + a.hi) * a.lo;
#endif
  return dfnormf(ret);
}

/* dualfloat与float相乘得到dualfloat */
inline dualfloat dfmulf(dualfloat a, float b) {
  dualfloat ret, tmp, tmp2;
  ret = dmulf(a.hi, b);
  tmp = dmulf(a.lo, b);
  if (erpmarkf(ret.lo) > erpmarkf(tmp.hi)) {
    tmp2 = dfnormf(ddualf(ret.lo, tmp.hi));
    /* 双舍入需要舍入到奇数以保证正确舍入，而默认浮点环境不保证这一点 */
    tmp2.lo += tmp.lo;
  } else {
    tmp2 = dfnormf(ddualf(tmp.hi, ret.lo));
    tmp2.lo += tmp.lo;
    tmp2 = dfnormf(tmp2);
  }
  ret = dfnormf(ddualf(ret.hi, tmp2.hi));
  ret.lo += tmp2.lo;
  return ret;
}

/* dualfloat与float相除得到dualfloat */
inline dualfloat dfdivf(dualfloat a, float b) {
  dualfloat ret, tmp;
  ret = dmdivf(a.hi, b);
  a = dfnormf(ddualf(ret.lo, a.lo));
  float rb = 1.0f / b;
  tmp.hi = a.hi * rb;
  tmp.lo = nfmulsubf_lim(tmp.hi, b, a.hi);
  tmp.lo = (tmp.lo + a.lo) * rb;
  tmp = dfnormf(tmp);
  ret = dfnormf(ddualf(ret.hi, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

/* float与dualfloat相除得到dualfloat */
inline dualfloat dfdivrf(float a, dualfloat b) {
  dualfloat ret, tmp, tmp2;
  float r0, r1, r2, r3;
  ret = dmdivf(a, b.hi);
  r0 = 1.0f / b.hi;
  r1 = ret.hi;
  tmp2 = dmulf(r1, b.lo);
  tmp = dsubf(ret.lo, tmp2.hi);
  tmp.lo -= tmp2.lo;
  r2 = tmp.hi * r0;
  r3 = nfmulsubf_lim(r2, b.hi, tmp.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMAF))
  r3 -= fmulsubf(r2, b.lo, tmp.lo);
#else
  r3 += tmp.lo - r2 * b.lo;
#endif
  r3 *= r0;
  tmp = dfnormf(ddualf(r2, r3));
  ret = dfnormf(ddualf(r1, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

/* dualfloat乘法 */
inline dualfloat df2mulf(dualfloat a, dualfloat b) {
  dualfloat ret, tmp, tmp2;
  float r0;
  r0 = a.lo * b.lo;
  tmp = dmulf(a.hi, b.lo);
  tmp2 = dmulf(a.lo, b.hi);
  ret = dmulf(a.hi, b.hi);
  r0 += tmp.lo + tmp2.lo;
  tmp = daddf(tmp.hi, tmp2.hi);
  r0 += tmp.lo;
  tmp = daddf(ret.lo, tmp.hi);
  tmp.lo += r0;
  tmp = dfnormf(tmp);
  ret = dfnormf(ddualf(ret.hi, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

/* dualfloat除法 */
inline dualfloat df2divf(dualfloat a, dualfloat b) {
  dualfloat ret, tmp, tmp2;
  float r0, r1, r2, r3;
  ret = dmdivf(a.hi, b.hi);
  r0 = 1.0f / b.hi;
  r1 = ret.hi;
  tmp2 = dfnormf(ddualf(ret.lo, a.lo));
  tmp = dmulf(r1, b.lo);
  tmp2.lo -= tmp.lo;
  tmp = dsubf(tmp2.hi, tmp.hi);
  tmp.lo += tmp2.lo;
  r2 = tmp.hi * r0;
  r3 = nfmulsubf_lim(r2, b.hi, tmp.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMAF))
  r3 -= fmulsubf(r2, b.lo, tmp.lo);
#else
  r3 += tmp.lo - r2 * b.lo;
#endif
  r3 *= r0;
  tmp = dfnormf(ddualf(r2, r3));
  ret = dfnormf(ddualf(r1, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

/* dualfloat平方 */
inline dualfloat dfsqrf(dualfloat a) {
  dualfloat ret, tmp;
  float r0;
  r0 = a.lo * a.lo;
  tmp = dmulf(a.hi, a.lo);
  tmp.hi += tmp.hi;
  tmp.lo += tmp.lo;
  ret = dsqrf(a.hi);
  r0 += tmp.lo;
  tmp = daddf(ret.lo, tmp.hi);
  tmp.lo += r0;
  tmp = dfnormf(tmp);
  ret = dfnormf(ddualf(ret.hi, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

/* float倒数 */
inline dualfloat drcpf(float a) {
  dualfloat ret, tmp;
  float r0, r1, r2, r3;
  r1 = r0 = 1.0f / a;
  r3 = r2 = nfmulsubf_lim(r0, a, 1.0f);
  r2 *= r0;
  r3 = nfmulsubf_lim(r2, a, r3);
  r3 *= r0;
  tmp = dfnormf(ddualf(r2, r3));
  ret = dfnormf(ddualf(r1, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

/* dualfloat倒数 */
inline dualfloat dfrcpf(dualfloat a) {
  dualfloat ret, tmp, tmp2;
  float r0, r1, r2, r3;
  r1 = r0 = 1.0f / a.hi;
  r2 = nfmulsubf_lim(r0, a.hi, 1.0f);
  tmp2 = dmulf(r1, a.lo);
  tmp = dsubf(r2, tmp2.hi);
  tmp.lo -= tmp2.lo;
  r2 = tmp.hi * r0;
  r3 = nfmulsubf_lim(r2, a.hi, tmp.hi);
#if FP_FMA_INTRINS == 1 || (FP_FMA_INTRINS == 2 && defined(FP_FAST_FMAF))
  r3 -= fmulsubf(r2, a.lo, tmp.lo);
#else
  r3 += tmp.lo - r2 * a.lo;
#endif
  r3 *= r0;
  tmp = dfnormf(ddualf(r2, r3));
  ret = dfnormf(ddualf(r1, tmp.hi));
  ret.lo += tmp.lo;
  return ret;
}

#if defined(__cplusplus) || defined(c_plusplus)
#include "dualfloat_cxx.h"
#endif

#endif