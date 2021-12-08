#ifndef _DUAL_DOUBLE_H_
#error "it must include by <dualdouble.h>"
#else
#ifndef _DUAL_DOUBLE_CXX_
#define _DUAL_DOUBLE_CXX_

#ifdef FAST_DF_OPERATOR
/* add */
inline dualdouble operator+(dualdouble a, double b) { return fdfadd(a, b); }

inline dualdouble operator+(dualdouble a, dualdouble b) { return fdf2add(a, b); }

inline dualdouble &operator+=(dualdouble &a, double b) {
  return a = fdfadd(a, b);
}

inline dualdouble &operator+=(dualdouble &a, dualdouble b) {
  return a = fdf2add(a, b);
}

/* sub */
inline dualdouble operator-(dualdouble a, double b) { return fdfsub(a, b); }

inline dualdouble operator-(double a, dualdouble b) { return fdfsubr(a, b); }

inline dualdouble operator-(dualdouble a, dualdouble b) { return fdf2sub(a, b); }

inline dualdouble &operator-=(dualdouble &a, double b) {
  return a = fdfsub(a, b);
}

inline dualdouble &operator-=(dualdouble &a, dualdouble b) {
  return a = fdf2sub(a, b);
}

/* mul */
inline dualdouble operator*(dualdouble a, double b) { return fdfmul(a, b); }

inline dualdouble operator*(dualdouble a, dualdouble b) { return fdf2mul(a, b); }

inline dualdouble &operator*=(dualdouble &a, double b) {
  return a = fdfmul(a, b);
}

inline double &operator*=(double &a, dualdouble b) { return a = fdfmul(b, a).hi; }

inline dualdouble &operator*=(dualdouble &a, dualdouble b) {
  return a = fdf2mul(a, b);
}

/* div */
inline dualdouble operator/(dualdouble a, double b) { return fdfdiv(a, b); }

inline dualdouble operator/(double a, dualdouble b) { return fdfdivr(a, b); }

inline dualdouble operator/(dualdouble a, dualdouble b) { return fdf2div(a, b); }

inline dualdouble &operator/=(dualdouble &a, double b) {
  return a = fdfdiv(a, b);
}

inline double &operator/=(double &a, dualdouble b) { return a = fdfdivr(a, b).hi; }

inline dualdouble &operator/=(dualdouble &a, dualdouble b) {
  return a = fdf2div(a, b);
}

#else
/* add */
inline dualdouble operator+(dualdouble a, double b) { return dfadd(a, b); }

inline dualdouble operator+(dualdouble a, dualdouble b) { return df2add(a, b); }

inline dualdouble &operator+=(dualdouble &a, double b) {
  return a = dfadd(a, b);
}

inline dualdouble &operator+=(dualdouble &a, dualdouble b) {
  return a = df2add(a, b);
}

/* sub */
inline dualdouble operator-(dualdouble a, double b) { return dfsub(a, b); }

inline dualdouble operator-(double a, dualdouble b) { return dfsubr(a, b); }

inline dualdouble operator-(dualdouble a, dualdouble b) { return df2sub(a, b); }

inline dualdouble &operator-=(dualdouble &a, double b) {
  return a = dfsub(a, b);
}

inline dualdouble &operator-=(dualdouble &a, dualdouble b) {
  return a = df2sub(a, b);
}

/* mul */
inline dualdouble operator*(dualdouble a, double b) { return dfmul(a, b); }

inline dualdouble operator*(dualdouble a, dualdouble b) { return df2mul(a, b); }

inline dualdouble &operator*=(dualdouble &a, double b) {
  return a = dfmul(a, b);
}

inline double &operator*=(double &a, dualdouble b) { return a = dfmul(b, a).hi; }

inline dualdouble &operator*=(dualdouble &a, dualdouble b) {
  return a = df2mul(a, b);
}

/* div */
inline dualdouble operator/(dualdouble a, double b) { return dfdiv(a, b); }

inline dualdouble operator/(double a, dualdouble b) { return dfdivr(a, b); }

inline dualdouble operator/(dualdouble a, dualdouble b) { return df2div(a, b); }

inline dualdouble &operator/=(dualdouble &a, double b) {
  return a = dfdiv(a, b);
}

inline double &operator/=(double &a, dualdouble b) { return a = dfdivr(a, b).hi; }

inline dualdouble &operator/=(dualdouble &a, dualdouble b) {
  return a = df2div(a, b);
}

#endif

inline dualdouble operator+(double a, dualdouble b) { return b + a; }

inline double &operator+=(double &a, dualdouble b) { return a = df1add(b, a); }

inline dualdouble operator-(dualdouble a) { return dfneg(a); }

inline double &operator-=(double &a, dualdouble b) { return a = df1subr(a, b); }

inline dualdouble operator*(double a, dualdouble b) { return b * a; }

/* cmp_eq */
inline bool operator==(const dualdouble &a, double b) {
  return (a.hi == b && a.lo == 0.0);
}

inline bool operator==(const dualdouble &a, const dualdouble &b) {
  return (a.hi == b.hi && a.lo == b.lo);
}

inline bool operator==(double a, const dualdouble &b) {
  return (a == b.hi && b.lo == 0.0);
}

/* cmp_gt */
inline bool operator>(const dualdouble &a, double b) {
  return (a.hi > b || (a.hi == b && a.lo > 0.0));
}

inline bool operator>(const dualdouble &a, const dualdouble &b) {
  return (a.hi > b.hi || (a.hi == b.hi && a.lo > b.lo));
}

inline bool operator>(double a, const dualdouble &b) {
  return (a > b.hi || (a == b.hi && b.lo < 0.0));
}

/* cmp_lt */
inline bool operator<(const dualdouble &a, double b) {
  return (a.hi < b || (a.hi == b && a.lo < 0.0));
}

inline bool operator<(const dualdouble &a, const dualdouble &b) {
  return (a.hi < b.hi || (a.hi == b.hi && a.lo < b.lo));
}

inline bool operator<(double a, const dualdouble &b) {
  return (a < b.hi || (a == b.hi && b.lo > 0.0));
}

/* cmp_gt_eq */
inline bool operator>=(const dualdouble &a, double b) {
  return (a.hi > b || (a.hi == b && a.lo >= 0.0));
}

inline bool operator>=(const dualdouble &a, const dualdouble &b) {
  return (a.hi > b.hi || (a.hi == b.hi && a.lo >= b.lo));
}

/* cmp_lt_eq */
inline bool operator<=(const dualdouble &a, double b) {
  return (a.hi < b || (a.hi == b && a.lo <= 0.0));
}

inline bool operator<=(const dualdouble &a, const dualdouble &b) {
  return (a.hi < b.hi || (a.hi == b.hi && a.lo <= b.lo));
}

/* cmp single */
inline bool operator>=(double a, const dualdouble &b) {
  return (b <= a);
}

inline bool operator<=(double a, const dualdouble &b) {
  return (b >= a);
}

/* cmp_not_eq */
inline bool operator!=(const dualdouble &a, double b) {
  return (a.hi != b || a.lo != 0.0);
}

inline bool operator!=(const dualdouble &a, const dualdouble &b) {
  return (a.hi != b.hi || a.lo != b.lo);
}

inline bool operator!=(double a, const dualdouble &b) {
  return (a != b.hi || b.lo != 0.0);
}

#endif
#endif // !_DUAL_DOUBLE_H_
