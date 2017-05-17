#ifndef dagr_type_elevate_h
#define dagr_type_elevate_h

namespace dagr_type_select
{
// given two arguments, an elevate cast, selects
// the type of or casts to the higher precision
// type. note that given a signed and unsigned
// argument, signed type is selected.
template <typename t1, typename t2>
struct elevate {};

// given two areuments, a decay cast, selects the
// type of or casts to the lower precision
// type. note that given a signed and unsigned
// argument, unsigned type is selected.
template <typename t1, typename t2>
struct decay {};

#define dagr_type_select(_class, _ret, _t1, _t2)    \
template <>                                         \
struct _class<_t1, _t2>                             \
{                                                   \
    using type = _ret;                              \
    static _ret cast(_t1 arg){ return arg; }        \
    static constexpr const char *type_name()        \
    { return #_ret; }                               \
};

#define dagr_type_elevate_case(_ret, _t1, _t2)      \
dagr_type_select(elevate, _ret, _t1, _t2)           \

#define dagr_type_elevate_sym_case(_ret, _t1, _t2)  \
dagr_type_elevate_case(_ret, _t1, _t2)              \
dagr_type_elevate_case(_ret, _t2, _t1)

#define dagr_type_decay_case(_ret, _t1, _t2)        \
dagr_type_select(decay, _ret, _t1, _t2)             \

#define dagr_type_decay_sym_case(_ret, _t1, _t2)    \
dagr_type_decay_case(_ret, _t1, _t2)                \
dagr_type_decay_case(_ret, _t2, _t1)

// elevate to double precision
dagr_type_elevate_case(double, double, double)
dagr_type_elevate_sym_case(double, double, float)
dagr_type_elevate_sym_case(double, double, char)
dagr_type_elevate_sym_case(double, double, short)
dagr_type_elevate_sym_case(double, double, int)
dagr_type_elevate_sym_case(double, double, long)
dagr_type_elevate_sym_case(double, double, long long)
dagr_type_elevate_sym_case(double, double, unsigned char)
dagr_type_elevate_sym_case(double, double, unsigned short)
dagr_type_elevate_sym_case(double, double, unsigned int)
dagr_type_elevate_sym_case(double, double, unsigned long)
dagr_type_elevate_sym_case(double, double, unsigned long long)
// elevate to single precision
dagr_type_elevate_case(float, float, float)
dagr_type_elevate_sym_case(float, float, char)
dagr_type_elevate_sym_case(float, float, short)
dagr_type_elevate_sym_case(float, float, int)
dagr_type_elevate_sym_case(float, float, long)
dagr_type_elevate_sym_case(float, float, long long)
dagr_type_elevate_sym_case(float, float, unsigned char)
dagr_type_elevate_sym_case(float, float, unsigned short)
dagr_type_elevate_sym_case(float, float, unsigned int)
dagr_type_elevate_sym_case(float, float, unsigned long)
dagr_type_elevate_sym_case(float, float, unsigned long long)
// elevate to long long
dagr_type_elevate_case(long long, long long, long long)
dagr_type_elevate_sym_case(long long, long long, char)
dagr_type_elevate_sym_case(long long, long long, short)
dagr_type_elevate_sym_case(long long, long long, int)
dagr_type_elevate_sym_case(long long, long long, long)
dagr_type_elevate_sym_case(long long, long long, unsigned char)
dagr_type_elevate_sym_case(long long, long long, unsigned short)
dagr_type_elevate_sym_case(long long, long long, unsigned int)
dagr_type_elevate_sym_case(long long, long long, unsigned long)
dagr_type_elevate_sym_case(long long, long long, unsigned long long)
// elevate to unsigned long long
dagr_type_elevate_case(unsigned long long, unsigned long long, unsigned long long)
dagr_type_elevate_sym_case(long long, unsigned long long, char)  // *
dagr_type_elevate_sym_case(long long, unsigned long long, short) // *
dagr_type_elevate_sym_case(long long, unsigned long long, int)   // *
dagr_type_elevate_sym_case(long long, unsigned long long, long)  // *
dagr_type_elevate_sym_case(unsigned long long, unsigned long long, unsigned char)
dagr_type_elevate_sym_case(unsigned long long, unsigned long long, unsigned short)
dagr_type_elevate_sym_case(unsigned long long, unsigned long long, unsigned int)
dagr_type_elevate_sym_case(unsigned long long, unsigned long long, unsigned long)
// elevate to long
dagr_type_elevate_case(long, long, long)
dagr_type_elevate_sym_case(long, long, char)
dagr_type_elevate_sym_case(long, long, short)
dagr_type_elevate_sym_case(long, long, int)
dagr_type_elevate_sym_case(long, long, unsigned char)
dagr_type_elevate_sym_case(long, long, unsigned short)
dagr_type_elevate_sym_case(long, long, unsigned int)
dagr_type_elevate_sym_case(long, long, unsigned long)
// elevate to unsigned long
dagr_type_elevate_case(unsigned long, unsigned long, unsigned long)
dagr_type_elevate_sym_case(long, unsigned long, char)  // *
dagr_type_elevate_sym_case(long, unsigned long, short) // *
dagr_type_elevate_sym_case(long, unsigned long, int)   // *
dagr_type_elevate_sym_case(unsigned long, unsigned long, unsigned char)
dagr_type_elevate_sym_case(unsigned long, unsigned long, unsigned short)
dagr_type_elevate_sym_case(unsigned long, unsigned long, unsigned int)
// elevate to int
dagr_type_elevate_case(int, int, int)
dagr_type_elevate_sym_case(int, int, char)
dagr_type_elevate_sym_case(int, int, short)
dagr_type_elevate_sym_case(int, int, unsigned char)
dagr_type_elevate_sym_case(int, int, unsigned short)
dagr_type_elevate_sym_case(int, int, unsigned int)
// elevate to unsigned int
dagr_type_elevate_case(unsigned int, unsigned int, unsigned int)
dagr_type_elevate_sym_case(int, unsigned int, char)  // *
dagr_type_elevate_sym_case(int, unsigned int, short) // *
dagr_type_elevate_sym_case(unsigned int, unsigned int, unsigned char)
dagr_type_elevate_sym_case(unsigned int, unsigned int, unsigned short)
// elevate to short
dagr_type_elevate_case(short, short, short)
dagr_type_elevate_sym_case(short, short, char)
dagr_type_elevate_sym_case(short, short, unsigned char)
dagr_type_elevate_sym_case(short, short, unsigned short)
// elevate to unsigned short
dagr_type_elevate_case(unsigned short, unsigned short, unsigned short)
dagr_type_elevate_sym_case(short, unsigned short, char)  // *
dagr_type_elevate_sym_case(unsigned short, unsigned short, unsigned char)
// elevate to char
dagr_type_elevate_case(char, char, char)
dagr_type_elevate_sym_case(char, char, unsigned char)
// elevate to unsigned char
dagr_type_elevate_case(unsigned char, unsigned char, unsigned char)

// decay from double precision
dagr_type_decay_case(double, double, double)
dagr_type_decay_sym_case(float, double, float)
dagr_type_decay_sym_case(char, double, char)
dagr_type_decay_sym_case(int, double, int)
dagr_type_decay_sym_case(long, double, long)
dagr_type_decay_sym_case(long long, double, long long)
dagr_type_decay_sym_case(unsigned char, double, unsigned char)
dagr_type_decay_sym_case(unsigned int, double, unsigned int)
dagr_type_decay_sym_case(unsigned long, double, unsigned long)
dagr_type_decay_sym_case(unsigned long long, double, unsigned long long)
// decay from single precision
dagr_type_decay_case(float, float, float)
dagr_type_decay_sym_case(char, float, char)
dagr_type_decay_sym_case(int, float, int)
dagr_type_decay_sym_case(long, float, long)
dagr_type_decay_sym_case(long long, float, long long)
dagr_type_decay_sym_case(unsigned char, float, unsigned char)
dagr_type_decay_sym_case(unsigned int, float, unsigned int)
dagr_type_decay_sym_case(unsigned long, float, unsigned long)
dagr_type_decay_sym_case(unsigned long long, float, unsigned long long)
// decay from long long
dagr_type_decay_case(long long, long long, long long)
dagr_type_decay_sym_case(char, long long, char)
dagr_type_decay_sym_case(int, long long, int)
dagr_type_decay_sym_case(long, long long, long)
dagr_type_decay_sym_case(unsigned char, long long, unsigned char)
dagr_type_decay_sym_case(unsigned int, long long, unsigned int)
dagr_type_decay_sym_case(unsigned long, long long, unsigned long)
dagr_type_decay_sym_case(unsigned long long, long long, unsigned long long)
// decay from unsigned long long
dagr_type_decay_case(unsigned long long, unsigned long long, unsigned long long)
dagr_type_decay_sym_case(unsigned char, unsigned long long, char)
dagr_type_decay_sym_case(unsigned int, unsigned long long, int)
dagr_type_decay_sym_case(unsigned long, unsigned long long, long)
dagr_type_decay_sym_case(unsigned char, unsigned long long, unsigned char)
dagr_type_decay_sym_case(unsigned int, unsigned long long, unsigned int)
dagr_type_decay_sym_case(unsigned long, unsigned long long, unsigned long)
// decay from long
dagr_type_decay_case(long, long, long)
dagr_type_decay_sym_case(char, long, char)
dagr_type_decay_sym_case(int, long, int)
dagr_type_decay_sym_case(unsigned char, long, unsigned char)
dagr_type_decay_sym_case(unsigned int, long, unsigned int)
dagr_type_decay_sym_case(unsigned long, long, unsigned long)
// decay from unsigned long
dagr_type_decay_case(unsigned long, unsigned long, unsigned long)
dagr_type_decay_sym_case(unsigned char, unsigned long, char)
dagr_type_decay_sym_case(unsigned int, unsigned long, int)
dagr_type_decay_sym_case(unsigned char, unsigned long, unsigned char)
dagr_type_decay_sym_case(unsigned int, unsigned long, unsigned int)
// decay from int
dagr_type_decay_case(int, int, int)
dagr_type_decay_sym_case(char, int, char)
dagr_type_decay_sym_case(unsigned char, int, unsigned char)
dagr_type_decay_sym_case(unsigned int, int, unsigned int)
// decay from unsigned int
dagr_type_decay_case(unsigned int, unsigned int, unsigned int)
dagr_type_decay_sym_case(unsigned char, unsigned int, char)
dagr_type_decay_sym_case(unsigned char, unsigned int, unsigned char)
// decay from char
dagr_type_decay_case(char, char, char)
dagr_type_decay_sym_case(unsigned char, char, unsigned char)
// decay from unsigned char
dagr_type_decay_case(unsigned char, unsigned char, unsigned char)
};

#endif
