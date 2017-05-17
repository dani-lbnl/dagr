#ifndef dagr_algorithm_executive_fwd_h
#define dagr_algorithm_executive_fwd_h

#include "dagr_shared_object.h"

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_algorithm_executive)

// this is a convenience macro to be used to declare a static
// New method that will be used to construct new objects in
// shared_ptr's. This manages the details of interoperability
// with std C++11 shared pointer
#define DAGR_ALGORITHM_EXECUTIVE_STATIC_NEW(T)                      \
                                                                    \
static p_##T New()                                                  \
{                                                                   \
    return p_##T(new T);                                            \
}                                                                   \
                                                                    \
std::shared_ptr<T> shared_from_this()                               \
{                                                                   \
    return std::static_pointer_cast<T>(                             \
        dagr_algorithm_executive::shared_from_this());              \
}                                                                   \
                                                                    \
std::shared_ptr<T const> shared_from_this() const                   \
{                                                                   \
    return std::static_pointer_cast<T const>(                       \
        dagr_algorithm_executive::shared_from_this());              \
}

#endif
