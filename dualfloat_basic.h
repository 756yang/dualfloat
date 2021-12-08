#ifndef _DUALFLOAT_BASIC_H_
#define _DUALFLOAT_BASIC_H_

/**
 * 双数浮点运算库
 * dual floating point arithmetic library (only headers)
 * by 杨玉军, 2021/11/20.
 */

/* __FMA__宏,适用于x86-CPU启用融合乘加指令 */
#define __FMA__

/*
 * FP_FMA_INTRINS值1则使用FMA指令(x86)非x86平台则尝试math库的fma函数,
 * 其他值则使用math库的fma函数,未定义则不使用FMA加速
 */
#define FP_FMA_INTRINS 1

/* USE_DF_SPLIT_FMA宏,在无FMA加速情况下是否使用分割浮点算法,不推荐启用 */
#define USE_DF_SPLIT_FMA 0

/* SUBNORM_NO_DAZ宏,渐下溢非规格化数是否不视为0以启用渐下溢支持,推荐关闭 */
//#define SUBNORM_NO_DAZ

/*
 * FAST_DF_OPERATOR宏,定义则运算符重载快速版本的双数运算(精度略低),
 * 未定义则重载达到精度要求的版本(误差等于或略大于0.5ulps)
 */
#define FAST_DF_OPERATOR

/*
 * USE_BRANCH_DADD宏,定义则使用分支计算带余加减法,
 * 否则不使用分支计算之,推荐定义(分支应该比两次浮点加法略快)
 */
#define USE_BRANCH_DADD

#include <stdbool.h>
#include <stdint.h>
#if (defined(__x86_64__) || defined(_M_X64) || defined(i386) ||                \
     defined(__i386__) || defined(__i386) || defined(_M_IX86)) &&              \
    defined(__FMA__)
#include <immintrin.h>
#elif defined(FP_FMA_INTRINS)
#include <math.h>
#undef FP_FMA_INTRINS
#define FP_FMA_INTRINS 2
#endif

/* define byte order macro */
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN ||                   \
    defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) ||   \
    defined(__AARCH64EB__) || defined(_MIBSEB) || defined(__MIBSEB) ||         \
    defined(__MIBSEB__)
// It's a big-endian target architecture
#ifndef __BIG_ENDIAN__
#define __BIG_ENDIAN__
#endif
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN ||              \
    defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) ||                        \
    defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) ||      \
    defined(__MIPSEL) || defined(__MIPSEL__) ||                                \
    (defined(__x86_64__) || defined(_M_X64) || defined(i386) ||                \
     defined(__i386__) || defined(__i386) || defined(_M_IX86))
// It's a little-endian target architecture
#ifndef __LITTLE_ENDIAN__
#define __LITTLE_ENDIAN__
#endif
#else
#error "I don't know what architecture this is!"
#endif

typedef struct dualfloat {
#ifdef __LITTLE_ENDIAN__
  float hi;
  float lo;
#elif defined(__BIG_ENDIAN__)
  /* I don't know that big-endian simd order. */
  float hi; // should load in simd registers low bits.
  float lo;
#endif
#if defined(__cplusplus) || defined(c_plusplus)
  operator float() { return hi + lo; }
  dualfloat &operator=(float a) {
    hi = a;
    lo = 0.0f;
    return *this;
  }
#endif
} dualfloat;

typedef struct dualdouble {
#ifdef __LITTLE_ENDIAN__
  double hi;
  double lo;
#elif defined(__BIG_ENDIAN__)
  /* I don't know that big-endian simd order. */
  double hi; // should load in simd registers low bits.
  double lo;
#endif
#if defined(__cplusplus) || defined(c_plusplus)
  operator double() { return hi + lo; }
  dualdouble &operator=(double a) {
    hi = a;
    lo = 0.0;
    return *this;
  }
#endif
} dualdouble;

typedef dualdouble quadfloat;

inline long dual_likely(long x) {
#if defined(__GNUC__) && defined(__has_builtin)
#if __has_builtin(__builtin_expect)
  return __builtin_expect(!!(x), 1);
#endif
#endif
#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(likely)
  if (x)
    [[likely]] return 1;
  else
    return 0;
#endif
#endif
  return !!x;
}

/* 构造双数 */
inline dualfloat ddualf(float hi, float lo) {
  dualfloat ret;
  ret.hi = hi;
  ret.lo = lo;
  return ret;
}

/* 构造双数 */
inline dualdouble ddual(double hi, double lo) {
  dualdouble ret;
  ret.hi = hi;
  ret.lo = lo;
  return ret;
}

/* 取反 */
inline dualfloat dfnegf(dualfloat x) { return ddualf(-x.hi, -x.lo); }

/* 取反 */
inline dualdouble dfneg(dualdouble x) { return ddual(-x.hi, -x.lo); }

/*
 * 返回能比较浮点的erp(相当于ulp,最小精度单位)的标志,
 * 如:erpmarkf(a)>erpmarkf(b),在erp(a)<erp(b)时值为false.
 */
inline uint32_t erpmarkf(float a) {
  uint32_t ia = *(uint32_t *)&a;
  return ia + ia;
}

/*
 * 返回能比较浮点的erp(相当于ulp,最小精度单位)的标志,
 * 如:erpmark(a)>erpmark(b),在erp(a)<erp(b)时值为false.
 */
inline size_t erpmark(double a) {
  size_t ia;
  if (sizeof(ia) == 4)
#ifdef __LITTLE_ENDIAN__
    ia = ((uint32_t *)&a)[1];
#elif defined(__BIG_ENDIAN__)
    ia = ((uint32_t *)&a)[0];
#endif
  else if (sizeof(ia) == 8)
    ia = *(uint64_t *)&a;
  // else assert(0);
  return ia + ia;
}

/* 规格化dualfloat */
inline dualfloat dfnormf(dualfloat x) {
  dualfloat ret;
  ret.hi = x.hi + x.lo;
  ret.lo = x.lo + (x.hi - ret.hi);
  return ret;
}

/* 低位部分取反并规格化dualfloat */
inline dualfloat dfnlonormf(dualfloat x) {
  dualfloat ret;
  ret.hi = x.hi - x.lo;
  ret.lo = (x.hi - ret.hi) - x.lo;
  return ret;
}

/* 高位部分取反并规格化dualfloat */
inline dualfloat dfnhinormf(dualfloat x) {
  dualfloat ret;
  ret.hi = x.lo - x.hi;
  ret.lo = x.lo - (x.hi + ret.hi);
  return ret;
}

/* 规格化dualdouble */
inline dualdouble dfnorm(dualdouble x) {
  dualdouble ret;
  ret.hi = x.hi + x.lo;
  ret.lo = x.lo + (x.hi - ret.hi);
  return ret;
}

/* 低位部分取反并规格化dualdouble */
inline dualdouble dfnlonorm(dualdouble x) {
  dualdouble ret;
  ret.hi = x.hi - x.lo;
  ret.lo = (x.hi - ret.hi) - x.lo;
  return ret;
}

/* 高位部分取反并规格化dualdouble */
inline dualdouble dfnhinorm(dualdouble x) {
  dualdouble ret;
  ret.hi = x.lo - x.hi;
  ret.lo = x.lo - (x.hi + ret.hi);
  return ret;
}

/* 分割float以进行无损乘法 */
inline dualfloat df_split_float(float a) {
  dualfloat ret;
  int32_t ix = *(int32_t *)&a;
  ix &= (-1 << 12); // 12=(23+2)/2
  ret.lo = a - *(float *)&ix;
  ret.hi = a - ret.lo; // prevent DAZ
  return ret;
}

/* 分割double以进行无损乘法 */
inline dualdouble df_split_double(double a) {
  dualdouble ret;
  static const int64_t cvt_const = 0x3fa8000000000000LL;
  int64_t ix = *(int64_t *)&a;
  int64_t ie = ix & (-1LL << 52);        // 取阶码和符号
  int32_t it = (int32_t)ix << (32 - 27); // 27=(52+2)/2
  if (it < 0) {                          // 如果高位非0,需要进位取反
    ie ^= (1LL << 63);
    it = -it; // 即使it为最小负数也没问题
  }
  ix = cvt_const | it; // 进行转换,不会溢出
  ret.lo = *(double *)&ie * (*(double *)&ix - *(double *)&cvt_const); // 2^(-57)
  ret.hi = a - ret.lo; // ret.lo可能会下溢
  return ret;
}

/**
 * 计算a*b-c并只进行一次舍入,要求c近似于a*b且正负同号(误差2^-17)
 * 可以用来计算浮点乘法的余数,例如:fmulsubf_lim(a,b,a*b)
 * 正确处理了溢出但不适用非规格化数
 */
inline float fmulsubf_lim(float a, float b, float c) {
#if FP_FMA_INTRINS == 1
  __m128 ret = _mm_fmsub_ss(_mm_set_ss(a), _mm_set_ss(b), _mm_set_ss(c));
  return _mm_cvtss_f32(ret);
#elif FP_FMA_INTRINS == 2
  return fmaf(a, b, -c);
#elif USE_DF_SPLIT_FMA
  dualfloat da, db;
  float ret;
  da = df_split_float(a);
  db = df_split_float(b);
  ret = (((da.hi * db.hi - c) + da.hi * db.lo) + da.lo * db.hi) + da.lo * db.lo;
  return ret;
#else
  float ra, rc; // o=17~20
  int32_t ap, bp, cp, am, bm, cm;
  int shift;
  ap = *(int32_t *)&a;
  bp = *(int32_t *)&b;
  cp = *(int32_t *)&c;
  /* 取阶码和符号 */
  am = ap & 0xff800000;
  bm = bp & 0xff800000;
  cm = cp & 0xff800000;
  rc = *(float *)&cm;
  /* 取尾数 */
  ap &= 0x7fffff;
  bp &= 0x7fffff;
#ifdef SUBNORM_NO_DAZ
  /* 先判断c的值,以防规格化处理死循环 */
  if (!dual_likely(rc != 0))
    return rc;
  if (!dual_likely(c == c))
    return c;
  /* 规格化处理 */
  if (dual_likely(am << 1))
    ap |= 0x800000;
  else { // 处理非规格化数
    while ((ap <<= 1) < 0x800000)
      am -= 0x800000;
  }
  if (dual_likely(bm << 1))
    bp |= 0x800000;
  else {
    while ((bp <<= 1) < 0x800000)
      bm -= 0x800000;
  }
#else
  ap |= 0x800000;
  bp |= 0x800000;
#endif
  /* 计算移位 */
  am = am + bm - cm;
  shift = 150 - (am >> 23);
  cp <<= shift;
  am += 0xe9000000;
  rc *= *(float *)&am;
  /* 计算误差 */
  ap = ap * bp - cp;
  ra = (float)ap;
  /* 浮点乘法上阶码,能正确处理溢出 */
  ra *= rc; //上阶码,防止溢出
  return ra;
#endif
}

/**
 * 计算-(a*b-c)并只进行一次舍入,要求c近似于a*b且正负同号(误差2^-17)
 * 可以用来计算浮点乘法的余数,例如:nfmulsubf_lim(a,b,a*b)
 * 正确处理了溢出但不适用非规格化数
 */
inline float nfmulsubf_lim(float a, float b, float c) {
#if FP_FMA_INTRINS == 1
  __m128 ret = _mm_fnmadd_ss(_mm_set_ss(a), _mm_set_ss(b), _mm_set_ss(c));
  return _mm_cvtss_f32(ret);
#elif FP_FMA_INTRINS == 2
  return fmaf(-a, b, c);
#elif USE_DF_SPLIT_FMA
  dualfloat da, db;
  float ret;
  da = df_split_float(a);
  db = df_split_float(b);
  ret = (((c - da.hi * db.hi) - da.hi * db.lo) - da.lo * db.hi) - da.lo * db.lo;
  return ret;
#else
  float ra, rc; // o=17~20
  int32_t ap, bp, cp, am, bm, cm;
  int shift;
  ap = *(int32_t *)&a;
  bp = *(int32_t *)&b;
  cp = *(int32_t *)&c;
  /* 取阶码和符号 */
  am = ap & 0xff800000;
  bm = bp & 0xff800000;
  cm = cp & 0xff800000;
  rc = *(float *)&cm;
  /* 取尾数 */
  ap &= 0x7fffff;
  bp &= 0x7fffff;
#ifdef SUBNORM_NO_DAZ
  if (!dual_likely(rc != 0))
    return rc;
  if (!dual_likely(c == c))
    return c;
  if (dual_likely(am << 1))
    ap |= 0x800000;
  else { //处理非规格化数
    while ((ap <<= 1) < 0x800000)
      am -= 0x800000;
  }
  if (dual_likely(bm << 1))
    bp |= 0x800000;
  else { //处理非规格化数
    while ((bp <<= 1) < 0x800000)
      bm -= 0x800000;
  }
#else
  ap |= 0x800000;
  bp |= 0x800000;
#endif
  /* 计算移位 */
  am = am + bm - cm;
  shift = 150 - (am >> 23);
  cp <<= shift;
  am += 0xe9000000;
  rc *= *(float *)&am;
  /* 计算误差 */
  ap = cp - ap * bp;
  ra = (float)ap;
  /* 浮点乘法上阶码,能正确处理溢出 */
  ra *= rc; //上阶码,防止溢出
  return ra;
#endif
}

/**
 * 计算a*b-c并只进行一次舍入,要求c近似于a*b且正负同号(误差2^-43)
 * 可以用来计算浮点乘法的余数,例如:fmulsub_lim(a,b,a*b)
 * 正确处理了溢出但不适用非规格化数
 */
inline double fmulsub_lim(double a, double b, double c) {
#if FP_FMA_INTRINS == 1
  __m128d ret = _mm_fmsub_sd(_mm_set_sd(a), _mm_set_sd(b), _mm_set_sd(c));
  return _mm_cvtsd_f64(ret);
#elif FP_FMA_INTRINS == 2
  return fma(a, b, -c);
#elif USE_DF_SPLIT_FMA
  dualdouble da, db;
  double ret;
  da = df_split_double(a);
  db = df_split_double(b);
  ret = (((da.hi * db.hi - c) + da.hi * db.lo) + da.lo * db.hi) + da.lo * db.lo;
  return ret;
#else
  double ra, rc; // o=17~20
  int64_t ap, bp, cp, am, bm, cm;
  int shift;
  ap = *(int64_t *)&a;
  bp = *(int64_t *)&b;
  cp = *(int64_t *)&c;
  /* 取阶码和符号 */
  am = ap & 0xfff0000000000000LL;
  bm = bp & 0xfff0000000000000LL;
  cm = cp & 0xfff0000000000000LL;
  rc = *(double *)&cm;
  /* 取尾数 */
  ap &= 0xfffffffffffffLL;
  bp &= 0xfffffffffffffLL;
#ifdef SUBNORM_NO_DAZ
  if (!dual_likely(rc != 0))
    return rc;
  if (!dual_likely(c == c))
    return c;
  if (dual_likely(am << 1))
    ap |= 0x10000000000000LL;
  else { //处理非规格化数
    while ((ap <<= 1) < 0x10000000000000LL)
      am -= 0x10000000000000LL;
  }
  if (dual_likely(bm << 1))
    bp |= 0x10000000000000LL;
  else { //处理非规格化数
    while ((bp <<= 1) < 0x10000000000000LL)
      bm -= 0x10000000000000LL;
  }
#else
  ap |= 0x10000000000000LL;
  bp |= 0x10000000000000LL;
#endif
  /* 计算移位 */
  am = am + bm - cm;
  shift = 1075 - (am >> 52);
  am += 0xf980000000000000LL;
  rc *= *(double *)&am;
  cp <<= shift;
  /* 计算误差 */
  ap = ap * bp - cp;
  ra = (double)ap;
  /* 浮点乘法上阶码,能正确处理溢出 */
  ra *= rc; //上阶码,防止溢出
  return ra;
#endif
}

/**
 * 计算-(a*b-c)并只进行一次舍入,要求c近似于a*b且正负同号(误差2^-43)
 * 可以用来计算浮点乘法的余数,例如:fmulsub_lim(a,b,a*b)
 * 正确处理了溢出但不适用非规格化数
 */
inline double nfmulsub_lim(double a, double b, double c) {
#if FP_FMA_INTRINS == 1
  __m128d ret = _mm_fnmadd_sd(_mm_set_sd(a), _mm_set_sd(b), _mm_set_sd(c));
  return _mm_cvtsd_f64(ret);
#elif FP_FMA_INTRINS == 2
  return fma(-a, b, c);
#elif USE_DF_SPLIT_FMA
  dualdouble da, db;
  double ret;
  da = df_split_double(a);
  db = df_split_double(b);
  ret = (((c - da.hi * db.hi) - da.hi * db.lo) - da.lo * db.hi) - da.lo * db.lo;
  return ret;
#else
  double ra, rc; // o=17~20
  int64_t ap, bp, cp, am, bm, cm;
  int shift;
  ap = *(int64_t *)&a;
  bp = *(int64_t *)&b;
  cp = *(int64_t *)&c;
  /* 取阶码和符号 */
  am = ap & 0xfff0000000000000LL;
  bm = bp & 0xfff0000000000000LL;
  cm = cp & 0xfff0000000000000LL;
  rc = *(double *)&cm;
  /* 取尾数 */
  ap &= 0xfffffffffffffLL;
  bp &= 0xfffffffffffffLL;
#ifdef SUBNORM_NO_DAZ
  if (!dual_likely(rc != 0))
    return rc;
  if (!dual_likely(c == c))
    return c;
  if (dual_likely(am << 1))
    ap |= 0x10000000000000LL;
  else { //处理非规格化数
    while ((ap <<= 1) < 0x10000000000000LL)
      am -= 0x10000000000000LL;
  }
  if (dual_likely(bm << 1))
    bp |= 0x10000000000000LL;
  else { //处理非规格化数
    while ((bp <<= 1) < 0x10000000000000LL)
      bm -= 0x10000000000000LL;
  }
#else
  ap |= 0x10000000000000LL;
  bp |= 0x10000000000000LL;
#endif
  /* 计算移位 */
  am = am + bm - cm;
  shift = 1075 - (am >> 52);
  am += 0xf980000000000000LL;
  rc *= *(double *)&am;
  cp <<= shift;
  /* 计算误差 */
  ap = cp - ap * bp;
  ra = (double)ap;
  /* 浮点乘法上阶码,能正确处理溢出 */
  ra *= rc; //上阶码,防止溢出
  return ra;
#endif
}

/**
 * 计算a*a-c并只进行一次舍入,要求c近似于a*a且正负同号(误差2^-17)
 * 可以用来计算浮点乘法的余数,例如:fsqrsubf_lim(a,a*a)
 * 正确处理了溢出但不适用非规格化数
 */
inline float fsqrsubf_lim(float a, float c) {
#if FP_FMA_INTRINS == 1
  __m128 ret = _mm_fmsub_ss(_mm_set_ss(a), _mm_set_ss(a), _mm_set_ss(c));
  return _mm_cvtss_f32(ret);
#elif FP_FMA_INTRINS == 2
  return fmaf(a, a, -c);
#elif USE_DF_SPLIT_FMA
  dualfloat da;
  float ret;
  da = df_split_float(a);
  ret = ((da.hi * da.hi - c) + da.hi * da.lo * 2) + da.lo * da.lo;
  return ret;
#else
  float ra, rc; // o=17~20
  int32_t ap, cp, am, cm;
  int shift;
  ap = *(int32_t *)&a;
  cp = *(int32_t *)&c;
  /* 取阶码和符号 */
  am = ap & 0xff800000;
  cm = cp & 0xff800000;
  rc = *(float *)&cm;
  /* 取尾数 */
  ap &= 0x7fffff;
#ifdef SUBNORM_NO_DAZ
  /* 先判断c的值,以防规格化处理死循环 */
  if (!dual_likely(rc != 0))
    return rc;
  if (!dual_likely(c == c))
    return c;
  /* 规格化处理 */
  if (dual_likely(am << 1))
    ap |= 0x800000;
  else { //处理非规格化数
    while ((ap <<= 1) < 0x800000)
      am -= 0x800000;
  }
#else
  ap |= 0x800000;
#endif
  /* 计算移位 */
  am = am + am - cm;
  shift = 150 - (am >> 23);
  cp <<= shift;
  am += 0xe9000000;
  rc *= *(float *)&am;
  /* 计算误差 */
  ap = ap * ap - cp;
  ra = (float)ap;
  /* 浮点乘法上阶码,能正确处理溢出 */
  ra *= rc; //上阶码,防止溢出
  return ra;
#endif
}

/**
 * 计算-(a*a-c)并只进行一次舍入,要求c近似于a*a且正负同号(误差2^-17)
 * 可以用来计算浮点乘法的余数,例如:nfsqrsubf_lim(a,a*a)
 * 正确处理了溢出但不适用非规格化数
 */
inline float nfsqrsubf_lim(float a, float c) {
#if FP_FMA_INTRINS == 1
  __m128 ret = _mm_fnmadd_ss(_mm_set_ss(a), _mm_set_ss(a), _mm_set_ss(c));
  return _mm_cvtss_f32(ret);
#elif FP_FMA_INTRINS == 2
  return fmaf(-a, a, c);
#elif USE_DF_SPLIT_FMA
  dualfloat da;
  float ret;
  da = df_split_float(a);
  ret = ((c - da.hi * da.hi) - da.hi * da.lo * 2) - da.lo * da.lo;
  return ret;
#else
  float ra, rc; // o=17~20
  int32_t ap, cp, am, cm;
  int shift;
  ap = *(int32_t *)&a;
  cp = *(int32_t *)&c;
  /* 取阶码和符号 */
  am = ap & 0xff800000;
  cm = cp & 0xff800000;
  rc = *(float *)&cm;
  /* 取尾数 */
  ap &= 0x7fffff;
#ifdef SUBNORM_NO_DAZ
  /* 先判断c的值,以防规格化处理死循环 */
  if (!dual_likely(rc != 0))
    return rc;
  if (!dual_likely(c == c))
    return c;
  /* 规格化处理 */
  if (dual_likely(am << 1))
    ap |= 0x800000;
  else { //处理非规格化数
    while ((ap <<= 1) < 0x800000)
      am -= 0x800000;
  }
#else
  ap |= 0x800000;
#endif
  /* 计算移位 */
  am = am + am - cm;
  shift = 150 - (am >> 23);
  cp <<= shift;
  am += 0xe9000000;
  rc *= *(float *)&am;
  /* 计算误差 */
  ap = cp - ap * ap;
  ra = (float)ap;
  /* 浮点乘法上阶码,能正确处理溢出 */
  ra *= rc; //上阶码,防止溢出
  return ra;
#endif
}

/**
 * 计算a*a-c并只进行一次舍入,要求c近似于a*a且正负同号(误差2^-43)
 * 可以用来计算浮点乘法的余数,例如:fsqrsub_lim(a,a*a)
 * 正确处理了溢出但不适用非规格化数
 */
inline double fsqrsub_lim(double a, double c) {
#if FP_FMA_INTRINS == 1
  __m128d ret = _mm_fmsub_sd(_mm_set_sd(a), _mm_set_sd(a), _mm_set_sd(c));
  return _mm_cvtsd_f64(ret);
#elif FP_FMA_INTRINS == 2
  return fma(a, a, -c);
#elif USE_DF_SPLIT_FMA
  dualdouble da;
  double ret;
  da = df_split_double(a);
  ret = ((da.hi * da.hi - c) + da.hi * da.lo * 2) + da.lo * da.lo;
  return ret;
#else
  double ra, rc; // o=17~20
  int64_t ap, cp, am, cm;
  int shift;
  ap = *(int64_t *)&a;
  cp = *(int64_t *)&c;
  /* 取阶码和符号 */
  am = ap & 0xfff0000000000000LL;
  cm = cp & 0xfff0000000000000LL;
  rc = *(double *)&cm;
  /* 取尾数 */
  ap &= 0xfffffffffffffLL;
#ifdef SUBNORM_NO_DAZ
  if (!dual_likely(rc != 0))
    return rc;
  if (!dual_likely(c == c))
    return c;
  if (dual_likely(am << 1))
    ap |= 0x10000000000000LL;
  else { //处理非规格化数
    while ((ap <<= 1) < 0x10000000000000LL)
      am -= 0x10000000000000LL;
  }
#else
  ap |= 0x10000000000000LL;
#endif
  /* 计算移位 */
  am = am + am - cm;
  shift = 1075 - (am >> 52);
  am += 0xf980000000000000LL;
  rc *= *(double *)&am;
  cp <<= shift;
  /* 计算误差 */
  ap = ap * ap - cp;
  ra = (double)ap;
  /* 浮点乘法上阶码,能正确处理溢出 */
  ra *= rc; //上阶码,防止溢出
  return ra;
#endif
}

/**
 * 计算-(a*a-c)并只进行一次舍入,要求c近似于a*a且正负同号(误差2^-43)
 * 可以用来计算浮点乘法的余数,例如:fsqrsub_lim(a,a*a)
 * 正确处理了溢出但不适用非规格化数
 */
inline double nfsqrsub_lim(double a, double c) {
#if FP_FMA_INTRINS == 1
  __m128d ret = _mm_fnmadd_sd(_mm_set_sd(a), _mm_set_sd(a), _mm_set_sd(c));
  return _mm_cvtsd_f64(ret);
#elif FP_FMA_INTRINS == 2
  return fma(-a, a, c);
#elif USE_DF_SPLIT_FMA
  dualdouble da;
  double ret;
  da = df_split_double(a);
  ret = ((c - da.hi * da.hi) - da.hi * da.lo * 2) - da.lo * da.lo;
  return ret;
#else
  double ra, rc; // o=17~20
  int64_t ap, cp, am, cm;
  int shift;
  ap = *(int64_t *)&a;
  cp = *(int64_t *)&c;
  /* 取阶码和符号 */
  am = ap & 0xfff0000000000000LL;
  cm = cp & 0xfff0000000000000LL;
  rc = *(double *)&cm;
  /* 取尾数 */
  ap &= 0xfffffffffffffLL;
#ifdef SUBNORM_NO_DAZ
  if (!dual_likely(rc != 0))
    return rc;
  if (!dual_likely(c == c))
    return c;
  if (dual_likely(am << 1))
    ap |= 0x10000000000000LL;
  else { //处理非规格化数
    while ((ap <<= 1) < 0x10000000000000LL)
      am -= 0x10000000000000LL;
  }
#else
  ap |= 0x10000000000000LL;
#endif
  /* 计算移位 */
  am = am + am - cm;
  shift = 1075 - (am >> 52);
  am += 0xf980000000000000LL;
  rc *= *(double *)&am;
  cp <<= shift;
  /* 计算误差 */
  ap = cp - ap * ap;
  ra = (double)ap;
  /* 浮点乘法上阶码,能正确处理溢出 */
  ra *= rc; //上阶码,防止溢出
  return ra;
#endif
}

/* float相加得到dualfloat */
inline dualfloat daddf(float a, float b) {
#ifdef USE_BRANCH_DADD
  if (erpmarkf(a) >= erpmarkf(b))
    return dfnormf(ddualf(a, b));
  else
    return dfnormf(ddualf(b, a));
#else
  dualfloat ret;
  float z;
  ret.hi = a + b;
  z = ret.hi - a;
  ret.lo = (a - (ret.hi - z)) + (b - z);
  return ret;
#endif
}

/* float相减得到dualfloat */
inline dualfloat dsubf(float a, float b) {
#ifdef USE_BRANCH_DADD
  if (erpmarkf(a) >= erpmarkf(b))
    return dfnlonormf(ddualf(a, b));
  else
    return dfnhinormf(ddualf(b, a));
#else
  dualfloat ret;
  float z;
  ret.hi = a - b;
  z = ret.hi - a;
  ret.lo = (a - (ret.hi - z)) - (b + z);
  return ret;
#endif
}

/* double相加得到dualdouble */
inline dualdouble dadd(double a, double b) {
#ifdef USE_BRANCH_DADD
  if (erpmark(a) >= erpmark(b))
    return dfnorm(ddual(a, b));
  else
    return dfnorm(ddual(b, a));
#else
  dualdouble ret;
  double z;
  ret.hi = a + b;
  z = ret.hi - a;
  ret.lo = (a - (ret.hi - z)) + (b - z);
  return ret;
#endif
}

/* double相减得到dualdouble */
inline dualdouble dsub(double a, double b) {
#ifdef USE_BRANCH_DADD
  if (erpmark(a) >= erpmark(b))
    return dfnlonorm(ddual(a, b));
  else
    return dfnhinorm(ddual(b, a));
#else
  dualdouble ret;
  double z;
  ret.hi = a - b;
  z = ret.hi - a;
  ret.lo = (a - (ret.hi - z)) - (b + z);
  return ret;
#endif
}

/* dualfloat与float相加得到float */
inline float df1addf(dualfloat a, float b) {
  dualfloat ret = daddf(a.hi, b);
  return ret.hi + (ret.lo + a.lo);
}

/* dualfloat与float相减得到float */
inline float df1subf(dualfloat a, float b) {
  dualfloat ret = dsubf(a.hi, b);
  return ret.hi + (ret.lo + a.lo);
}

/* float与dualfloat相减得到float */
inline float df1subrf(float a, dualfloat b) {
  dualfloat ret = dsubf(a, b.hi);
  return ret.hi + (ret.lo - b.lo);
}

/* dualdouble与double相加得到double */
inline double df1add(dualdouble a, double b) {
  dualdouble ret = dadd(a.hi, b);
  return ret.hi + (ret.lo + a.lo);
}

/* dualdouble与double相减得到double */
inline double df1sub(dualdouble a, double b) {
  dualdouble ret = dsub(a.hi, b);
  return ret.hi + (ret.lo + a.lo);
}

/* double与dualdouble相减得到double */
inline double df1subr(double a, dualdouble b) {
  dualdouble ret = dsub(a, b.hi);
  return ret.hi + (ret.lo - b.lo);
}

/* 计算a*b+c并只进行一次舍入 */
inline float fmuladdf(float a, float b, float c) {
#if FP_FMA_INTRINS == 1
  __m128 ret = _mm_fmadd_ss(_mm_set_ss(a), _mm_set_ss(b), _mm_set_ss(c));
  return _mm_cvtss_f32(ret);
#elif FP_FMA_INTRINS == 2
  return fmaf(a, b, c);
#else
  float ab = a * b;
  float rab = fmulsubf_lim(a, b, ab);
  return df1addf(ddualf(ab, rab), c);
#endif
}

/* 计算a*b+c并只进行一次舍入 */
inline double fmuladd(double a, double b, double c) {
#if FP_FMA_INTRINS == 1
  __m128d ret = _mm_fmadd_sd(_mm_set_sd(a), _mm_set_sd(b), _mm_set_sd(c));
  return _mm_cvtsd_f64(ret);
#elif FP_FMA_INTRINS == 2
  return fma(a, b, c);
#else
  double ab = a * b;
  double rab = fmulsub_lim(a, b, ab);
  return df1add(ddual(ab, rab), c);
#endif
}

/* 计算a*b-c并只进行一次舍入 */
inline float fmulsubf(float a, float b, float c) {
#if FP_FMA_INTRINS == 1
  __m128 ret = _mm_fmsub_ss(_mm_set_ss(a), _mm_set_ss(b), _mm_set_ss(c));
  return _mm_cvtss_f32(ret);
#elif FP_FMA_INTRINS == 2
  return fmaf(a, b, -c);
#else
  float ab = a * b;
  float rab = fmulsubf_lim(a, b, ab);
  return df1subf(ddualf(ab, rab), c);
#endif
}

/* 计算a*b-c并只进行一次舍入 */
inline double fmulsub(double a, double b, double c) {
#if FP_FMA_INTRINS == 1
  __m128d ret = _mm_fmsub_sd(_mm_set_sd(a), _mm_set_sd(b), _mm_set_sd(c));
  return _mm_cvtsd_f64(ret);
#elif FP_FMA_INTRINS == 2
  return fma(a, b, -c);
#else
  double ab = a * b;
  double rab = fmulsub_lim(a, b, ab);
  return df1sub(ddual(ab, rab), c);
#endif
}

/* 计算-(a*b+c)并只进行一次舍入 */
inline float nfmuladdf(float a, float b, float c) {
#if FP_FMA_INTRINS == 1
  __m128 ret = _mm_fnmsub_ss(_mm_set_ss(a), _mm_set_ss(b), _mm_set_ss(c));
  return _mm_cvtss_f32(ret);
#elif FP_FMA_INTRINS == 2
  return -fmaf(a, b, c);
#else
  float ab = a * b;
  float rab = fmulsubf_lim(a, b, ab);
  return -df1addf(ddualf(ab, rab), c);
#endif
}

/* 计算-(a*b+c)并只进行一次舍入 */
inline double nfmuladd(double a, double b, double c) {
#if FP_FMA_INTRINS == 1
  __m128d ret = _mm_fnmsub_sd(_mm_set_sd(a), _mm_set_sd(b), _mm_set_sd(c));
  return _mm_cvtsd_f64(ret);
#elif FP_FMA_INTRINS == 2
  return -fma(a, b, c);
#else
  double ab = a * b;
  double rab = fmulsub_lim(a, b, ab);
  return -df1add(ddual(ab, rab), c);
#endif
}

/* 计算-(a*b-c)并只进行一次舍入 */
inline float nfmulsubf(float a, float b, float c) {
#if FP_FMA_INTRINS == 1
  __m128 ret = _mm_fnmadd_ss(_mm_set_ss(a), _mm_set_ss(b), _mm_set_ss(c));
  return _mm_cvtss_f32(ret);
#elif FP_FMA_INTRINS == 2
  return fmaf(-a, b, c);
#else
  float ab = a * b;
  float rab = fmulsubf_lim(a, b, ab);
  return df1subrf(c, ddualf(ab, rab));
#endif
}

/* 计算-(a*b-c)并只进行一次舍入 */
inline double nfmulsub(double a, double b, double c) {
#if FP_FMA_INTRINS == 1
  __m128d ret = _mm_fnmadd_sd(_mm_set_sd(a), _mm_set_sd(b), _mm_set_sd(c));
  return _mm_cvtsd_f64(ret);
#elif FP_FMA_INTRINS == 2
  return fma(-a, b, c);
#else
  double ab = a * b;
  double rab = fmulsub_lim(a, b, ab);
  return df1subr(c, ddual(ab, rab));
#endif
}

/* float自乘得到dualfloat */
inline dualfloat dsqrf(float a) {
  dualfloat ret;
  ret.hi = a * a;
  ret.lo = fsqrsubf_lim(a, ret.hi);
  return ret;
}

/* float相乘得到dualfloat */
inline dualfloat dmulf(float a, float b) {
  dualfloat ret;
  ret.hi = a * b;
  ret.lo = fmulsubf_lim(a, b, ret.hi);
  return ret;
}

/* float相除得到商(ret.hi)和余数(ret.lo) */
inline dualfloat dmdivf(float a, float b) {
  dualfloat ret;
  ret.hi = a / b;
  ret.lo = nfmulsubf_lim(ret.hi, b, a);
  return ret;
}

/* double自乘得到dualdouble */
inline dualdouble dsqr(double a) {
  dualdouble ret;
  ret.hi = a * a;
  ret.lo = fsqrsub_lim(a, ret.hi);
  return ret;
}

/* double相乘得到dualdouble */
inline dualdouble dmul(double a, double b) {
  dualdouble ret;
  ret.hi = a * b;
  ret.lo = fmulsub_lim(a, b, ret.hi);
  return ret;
}

/* double相除得到商(ret.hi)和余数(ret.lo) */
inline dualdouble dmdiv(double a, double b) {
  dualdouble ret;
  ret.hi = a / b;
  ret.lo = nfmulsub_lim(ret.hi, b, a);
  return ret;
}

/* float相除得到(正确舍入)dualfloat */
inline dualfloat ddivf(float a, float b) {
  dualfloat ret = dmdivf(a, b);
  ret.lo /= b;
  return ret;
}

/* double相除得到(正确舍入)dualdouble */
inline dualdouble ddiv(double a, double b) {
  dualdouble ret = dmdiv(a, b);
  ret.lo /= b;
  return ret;
}

/* 将{x,y}的数据按erp重排,若(mode&1)则先对y取反,若(mode&2)则进行不完全排序 */
inline void df2reorderf(dualfloat *x, dualfloat *y, const int mode) {
#if FP_FMA_INTRINS == 1
  __m128 mask = _mm_set1_ps(-0.0f); // 绝对值掩码
  __m128 mx = _mm_set_ps(0, 0, x->lo, x->hi);
  __m128 my = _mm_set_ps(0, 0, y->lo, y->hi);
  if (mode & 1)
    my = _mm_xor_ps(my, mask);
  mask = _mm_cmpgt_ps(_mm_andnot_ps(mx, mask),
                      _mm_andnot_ps(my, mask));            // 大于比较
  _mm_storel_pi((__m64 *)&x, _mm_blendv_ps(my, mx, mask)); // 大于
  _mm_storel_pi((__m64 *)&y, _mm_blendv_ps(mx, my, mask)); // 小于
  if (mode & 2)
    return;
  uint32_t r1 = *(uint32_t *)&x->lo, r2 = *(uint32_t *)&y->hi;
  if (!dual_likely(r1 + r1 < r2 + r2))
    return; // 再次比较
  *(uint32_t *)&x->lo = r2;
  *(uint32_t *)&y->hi = r1;
#else
  uint32_t erpxh, erpxl, erpyh, erpyl;
  float tmp;
  erpxh = erpmarkf(x->hi);
  erpyh = erpmarkf(y->hi);
  if (mode & 1)
    *y = dfnegf(*y);
  if (mode & 2) {
    if (erpxh < erpyh) {
      tmp = x->hi;
      x->hi = y->hi;
      y->hi = tmp;
    }
    erpxl = erpmarkf(x->lo);
    erpyl = erpmarkf(y->lo);
    if (erpxl < erpyl) {
      tmp = x->lo;
      x->lo = y->lo;
      y->lo = tmp;
    }
    return;
  }
  if (erpxh >= erpyh) {
    erpxl = erpmarkf(x->lo);
    if (erpxl < erpyh) {
      erpyl = erpmarkf(y->lo);
      tmp = x->lo;
      x->lo = y->hi;
      if (erpxl >= erpyl) {
        y->hi = tmp;
      } else {
        y->hi = y->lo;
        y->lo = tmp;
      }
    }
  } else {
    erpyl = erpmarkf(y->lo);
    tmp = x->hi;
    x->hi = y->hi;
    y->hi = tmp;
    if (erpyl >= erpxh) {
      tmp = x->lo;
      x->lo = y->lo;
      y->lo = tmp;
    } else {
      erpxl = erpmarkf(x->lo);
      tmp = x->lo;
      x->lo = y->hi;
      if (erpxl >= erpyl) {
        y->hi = tmp;
      } else {
        y->hi = y->lo;
        y->lo = tmp;
      }
    }
  }
#endif
}

/* 将{x,y}的数据按erp重排,若(mode&1)则先对y取反,若(mode&2)则进行不完全排序 */
inline void df2reorder(dualdouble *x, dualdouble *y, const int mode) {
#if FP_FMA_INTRINS == 1
  __m128d mask = _mm_set1_pd(-0.0); // 绝对值掩码
  __m128d mx = _mm_set_pd(x->lo, x->hi);
  __m128d my = _mm_set_pd(y->lo, y->hi);
  if (mode & 1)
    my = _mm_xor_pd(my, mask);
  mask = _mm_cmpgt_pd(_mm_andnot_pd(mx, mask),
                      _mm_andnot_pd(my, mask));           // 大于比较
  _mm_store_pd((double *)x, _mm_blendv_pd(my, mx, mask)); // 大于
  _mm_store_pd((double *)y, _mm_blendv_pd(mx, my, mask)); // 小于
  if (mode & 2)
    return;
  uint64_t r1 = *(uint64_t *)&x->lo, r2 = *(uint64_t *)&y->hi;
  if (!dual_likely(r1 + r1 < r2 + r2))
    return; // 再次比较
  *(uint64_t *)&x->lo = r2;
  *(uint64_t *)&y->hi = r1;
#else
  size_t erpxh, erpxl, erpyh, erpyl;
  double tmp;
  erpxh = erpmark(x->hi);
  erpyh = erpmark(y->hi);
  if (mode & 1)
    *y = dfneg(*y);
  if (mode & 2) {
    if (erpxh < erpyh) {
      tmp = x->hi;
      x->hi = y->hi;
      y->hi = tmp;
    }
    erpxl = erpmark(x->lo);
    erpyl = erpmark(y->lo);
    if (erpxl < erpyl) {
      tmp = x->lo;
      x->lo = y->lo;
      y->lo = tmp;
    }
    return;
  }
  if (erpxh >= erpyh) {
    erpxl = erpmark(x->lo);
    if (erpxl < erpyh) {
      erpyl = erpmark(y->lo);
      tmp = x->lo;
      x->lo = y->hi;
      if (erpxl >= erpyl) {
        y->hi = tmp;
      } else {
        y->hi = y->lo;
        y->lo = tmp;
      }
    }
  } else {
    erpyl = erpmark(y->lo);
    tmp = x->hi;
    x->hi = y->hi;
    y->hi = tmp;
    if (erpyl >= erpxh) {
      tmp = x->lo;
      x->lo = y->lo;
      y->lo = tmp;
    } else {
      erpxl = erpmark(x->lo);
      tmp = x->lo;
      x->lo = y->hi;
      if (erpxl >= erpyl) {
        y->hi = tmp;
      } else {
        y->hi = y->lo;
        y->lo = tmp;
      }
    }
  }
#endif
}

#include "dualdouble.h"
#include "dualfloat.h"

#endif