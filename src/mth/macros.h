#define m_clamp(x, a, b) ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))
#define m_min(a, b) ((a) < (b) ? (a) : (b))
#define m_max(a, b) ((a) > (b) ? (a) : (b))
#define m_abs(x) ((x) < 0 ? (-(x)) : (x))

#define PI_F (3.1415926535f)

#define SBV(v, b) ((v) |= (b))
#define CBV(v, b) ((v) &= ~(b))
#define BTV(v, b) ((v) & (b))
