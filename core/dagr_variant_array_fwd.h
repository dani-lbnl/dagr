#ifndef dagr_variant_array_fwd_h
#define dagr_variant_array_fwd_h

#include <string>
#include "dagr_shared_object.h"

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_variant_array)
DAGR_SHARED_OBJECT_TEMPLATE_FORWARD_DECL(dagr_variant_array_impl)

#ifndef SWIG
// convenience defs for POD types
// these should not be used in API, use dagr_variant_array instead
using dagr_string_array = dagr_variant_array_impl<std::string>;
using p_dagr_string_array = std::shared_ptr<dagr_variant_array_impl<std::string>>;
using const_p_dagr_string_array = std::shared_ptr<const dagr_variant_array_impl<std::string>>;

using dagr_float_array = dagr_variant_array_impl<float>;
using p_dagr_float_array = std::shared_ptr<dagr_variant_array_impl<float>>;
using const_p_dagr_float_array = std::shared_ptr<const dagr_variant_array_impl<float>>;

using dagr_double_array = dagr_variant_array_impl<double>;
using p_dagr_double_array = std::shared_ptr<dagr_variant_array_impl<double>>;
using const_p_dagr_double_array = std::shared_ptr<const dagr_variant_array_impl<double>>;

using dagr_char_array = dagr_variant_array_impl<char>;
using p_dagr_char_array = std::shared_ptr<dagr_variant_array_impl<char>>;
using const_p_dagr_char_array = std::shared_ptr<const dagr_variant_array_impl<char>>;

using dagr_unsigned_char_array = dagr_variant_array_impl<unsigned char>;
using p_dagr_unsigned_char_array = std::shared_ptr<dagr_variant_array_impl<unsigned char>>;
using const_p_dagr_unsigned_char_array = std::shared_ptr<const dagr_variant_array_impl<unsigned char>>;

using dagr_short_array = dagr_variant_array_impl<short>;
using p_dagr_short_array = std::shared_ptr<dagr_variant_array_impl<short>>;
using const_p_dagr_short_array = std::shared_ptr<const dagr_variant_array_impl<short>>;

using dagr_unsigned_short_array = dagr_variant_array_impl<unsigned short>;
using p_dagr_unsigned_short_array = std::shared_ptr<dagr_variant_array_impl<unsigned short>>;
using const_p_dagr_unsigned_short_array = std::shared_ptr<const dagr_variant_array_impl<unsigned short>>;

using dagr_int_array = dagr_variant_array_impl<int>;
using p_dagr_int_array = std::shared_ptr<dagr_variant_array_impl<int>>;
using const_p_dagr_int_array = std::shared_ptr<const dagr_variant_array_impl<int>>;

using dagr_unsigned_int_array = dagr_variant_array_impl<unsigned int>;
using p_dagr_unsigned_int_array = std::shared_ptr<dagr_variant_array_impl<unsigned int>>;
using const_p_dagr_unsigned_int_array = std::shared_ptr<const dagr_variant_array_impl<unsigned int>>;

using dagr_long_array = dagr_variant_array_impl<long>;
using p_dagr_long_array = std::shared_ptr<dagr_variant_array_impl<long>>;
using const_p_dagr_long_array = std::shared_ptr<const dagr_variant_array_impl<long>>;

using dagr_unsigned_long_array = dagr_variant_array_impl<unsigned long>;
using p_dagr_unsigned_long_array = std::shared_ptr<dagr_variant_array_impl<unsigned long>>;
using const_p_dagr_unsigned_long_array = std::shared_ptr<const dagr_variant_array_impl<unsigned long>>;

using dagr_long_long_array = dagr_variant_array_impl<long long>;
using p_dagr_long_long_array = std::shared_ptr<dagr_variant_array_impl<long long>>;
using const_p_dagr_long_long_array = std::shared_ptr<const dagr_variant_array_impl<long long>>;

using dagr_unsigned_long_long_array = dagr_variant_array_impl<unsigned long long>;
using p_dagr_unsigned_long_long_array = std::shared_ptr<dagr_variant_array_impl<unsigned long long>>;
using const_p_dagr_unsigned_long_long_array = std::shared_ptr<const dagr_variant_array_impl<unsigned long long>>;
#endif

// this is a convenience macro to be used to declare a static
// New method that will be used to construct new objects in
// shared_ptr's. This manages the details of interoperability
// with std C++11 shared pointer
#define DAGR_VARIANT_ARRAY_STATIC_NEW(T, t)                             \
                                                                        \
static std::shared_ptr<T<t>> New()                                      \
{                                                                       \
    return std::shared_ptr<T<t>>(new T<t>);                             \
}                                                                       \
                                                                        \
static std::shared_ptr<T<t>> New(size_t n)                              \
{                                                                       \
    return std::shared_ptr<T<t>>(new T<t>(n));                          \
}                                                                       \
                                                                        \
static std::shared_ptr<T<t>> New(size_t n, const t &v)                  \
{                                                                       \
    return std::shared_ptr<T<t>>(new T<t>(n, v));                       \
}                                                                       \
                                                                        \
static std::shared_ptr<T<t>> New(const t *vals, size_t n)               \
{                                                                       \
    return std::shared_ptr<T<t>>(new T<t>(vals, n));                    \
}                                                                       \
                                                                        \
using dagr_variant_array::shared_from_this;                             \
                                                                        \
std::shared_ptr<T> shared_from_this()                                   \
{                                                                       \
    return std::static_pointer_cast<T>(shared_from_this());             \
}                                                                       \
                                                                        \
std::shared_ptr<T const> shared_from_this() const                       \
{                                                                       \
    return std::static_pointer_cast<T const>(shared_from_this());       \
}
#endif
