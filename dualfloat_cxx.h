#ifndef _DUAL_FLOAT_H_
#error "it must include by <dualfloat.h>"
#else
#ifndef _DUAL_FLOAT_CXX_
#define _DUAL_FLOAT_CXX_

#ifdef FAST_DF_OPERATOR
/* add */
inline dualfloat operator+(dualfloat a, float b) { return fdfaddf(a, b); }

inline dualfloat operator+(dualfloat a, dualfloat b) { return fdf2addf(a, b); }

inline dualfloat &operator+=(dualfloat &a, float b) {
  return a = fdfaddf(a, b);
}

inline dualfloat &operator+=(dualfloat &a, dualfloat b) {
  return a = fdf2addf(a, b);
}

/* sub */
inline dualfloat operator-(dualfloat a, float b) { return fdfsubf(a, b); }

inline dualfloat operator-(float a, dualfloat b) { return fdfsubrf(a, b); }

inline dualfloat operator-(dualfloat a, dualfloat b) { return fdf2subf(a, b); }

inline dualfloat &operator-=(dualfloat &a, float b) {
  return a = fdfsubf(a, b);
}

inline dualfloat &operator-=(dualfloat &a, dualfloat b) {
  return a = fdf2subf(a, b);
}

/* mul */
inline dualfloat operator*(dualfloat a, float b) { return fdfmulf(a, b); }

inline dualfloat operator*(dualfloat a, dualfloat b) { return fdf2mulf(a, b); }

inline dualfloat &operator*=(dualfloat &a, float b) {
  return a = fdfmulf(a, b);
}

inline float &operator*=(float &a, dualfloat b) { return a = fdfmulf(b, a).hi; }

inline dualfloat &operator*=(dualfloat &a, dualfloat b) {
  return a = fdf2mulf(a, b);
}

/* div */
inline dualfloat operator/(dualfloat a, float b) { return fdfdivf(a, b); }

inline dualfloat operator/(float a, dualfloat b) { return fdfdivrf(a, b); }

inline dualfloat operator/(dualfloat a, dualfloat b) { return fdf2divf(a, b); }

inline dualfloat &operator/=(dualfloat &a, float b) {
  return a = fdfdivf(a, b);
}

inline float &operator/=(float &a, dualfloat b) { return a = fdfdivrf(a, b).hi; }

inline dualfloat &operator/=(dualfloat &a, dualfloat b) {
  return a = fdf2divf(a, b);
}

#else
/* add */
inline dualfloat operator+(dualfloat a, float b) { return dfaddf(a, b); }

inline dualfloat operator+(dualfloat a, dualfloat b) { return df2addf(a, b); }

inline dualfloat &operator+=(dualfloat &a, float b) {
  return a = dfaddf(a, b);
}

inline dualfloat &operator+=(dualfloat &a, dualfloat b) {
  return a = df2addf(a, b);
}

/* sub */
inline dualfloat operator-(dualfloat a, float b) { return dfsubf(a, b); }

inline dualfloat operator-(float a, dualfloat b) { return dfsubrf(a, b); }

inline dualfloat operator-(dualfloat a, dualfloat b) { return df2subf(a, b); }

inline dualfloat &operator-=(dualfloat &a, float b) {
  return a = dfsubf(a, b);
}

inline dualfloat &operator-=(dualfloat &a, dualfloat b) {
  return a = df2subf(a, b);
}

/* mul */
inline dualfloat operator*(dualfloat a, float b) { return dfmulf(a, b); }

inline dualfloat operator*(dualfloat a, dualfloat b) { return df2mulf(a, b); }

inline dualfloat &operator*=(dualfloat &a, float b) {
  return a = dfmulf(a, b);
}

inline float &operator*=(float &a, dualfloat b) { return a = dfmulf(b, a).hi; }

inline dualfloat &operator*=(dualfloat &a, dualfloat b) {
  return a = df2mulf(a, b);
}

/* div */
inline dualfloat operator/(dualfloat a, float b) { return dfdivf(a, b); }

inline dualfloat operator/(float a, dualfloat b) { return dfdivrf(a, b); }

inline dualfloat operator/(dualfloat a, dualfloat b) { return df2divf(a, b); }

inline dualfloat &operator/=(dualfloat &a, float b) {
  return a = dfdivf(a, b);
}

inline float &operator/=(float &a, dualfloat b) { return a = dfdivrf(a, b).hi; }

inline dualfloat &operator/=(dualfloat &a, dualfloat b) {
  return a = df2divf(a, b);
}

#endif

inline dualfloat operator+(float a, dualfloat b) { return b + a; }

inline float &operator+=(float &a, dualfloat b) { return a = df1addf(b, a); }

inline dualfloat operator-(dualfloat a) { return dfnegf(a); }

inline float &operator-=(float &a, dualfloat b) { return a = df1subrf(a, b); }

inline dualfloat operator*(float a, dualfloat b) { return b * a; }

/* cmp_eq */
inline bool operator==(const dualfloat &a, double b) {
  return (a.hi == b && a.lo == 0.0f);
}

inline bool operator==(const dualfloat &a, const dualfloat &b) {
  return (a.hi == b.hi && a.lo == b.lo);
}

inline bool operator==(double a, const dualfloat &b) {
  return (a == b.hi && b.lo == 0.0f);
}

/* cmp_gt */
inline bool operator>(const dualfloat &a, double b) {
  return (a.hi > b || (a.hi == b && a.lo > 0.0f));
}

inline bool operator>(const dualfloat &a, const dualfloat &b) {
  return (a.hi > b.hi || (a.hi == b.hi && a.lo > b.lo));
}

inline bool operator>(double a, const dualfloat &b) {
  return (a > b.hi || (a == b.hi && b.lo < 0.0f));
}

/* cmp_lt */
inline bool operator<(const dualfloat &a, double b) {
  return (a.hi < b || (a.hi == b && a.lo < 0.0f));
}

inline bool operator<(const dualfloat &a, const dualfloat &b) {
  return (a.hi < b.hi || (a.hi == b.hi && a.lo < b.lo));
}

inline bool operator<(double a, const dualfloat &b) {
  return (a < b.hi || (a == b.hi && b.lo > 0.0f));
}

/* cmp_gt_eq */
inline bool operator>=(const dualfloat &a, double b) {
  return (a.hi > b || (a.hi == b && a.lo >= 0.0f));
}

inline bool operator>=(const dualfloat &a, const dualfloat &b) {
  return (a.hi > b.hi || (a.hi == b.hi && a.lo >= b.lo));
}

/* cmp_lt_eq */
inline bool operator<=(const dualfloat &a, double b) {
  return (a.hi < b || (a.hi == b && a.lo <= 0.0f));
}

inline bool operator<=(const dualfloat &a, const dualfloat &b) {
  return (a.hi < b.hi || (a.hi == b.hi && a.lo <= b.lo));
}

/* cmp single */
inline bool operator>=(double a, const dualfloat &b) {
  return (b <= a);
}

inline bool operator<=(double a, const dualfloat &b) {
  return (b >= a);
}

/* cmp_not_eq */
inline bool operator!=(const dualfloat &a, double b) {
  return (a.hi != b || a.lo != 0.0f);
}

inline bool operator!=(const dualfloat &a, const dualfloat &b) {
  return (a.hi != b.hi || a.lo != b.lo);
}

inline bool operator!=(double a, const dualfloat &b) {
  return (a != b.hi || b.lo != 0.0f);
}

#endif
#endif // !_DUAL_FLOAT_H_
