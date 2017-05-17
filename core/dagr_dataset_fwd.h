#ifndef dagr_dataset_fwd_h
#define dagr_dataset_fwd_h

#include "dagr_common.h"
#include "dagr_shared_object.h"

#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_dataset)

// this is a convenience macro to be used to
// declare New and enable seamless operation
// with std C++11 shared pointer
#define DAGR_DATASET_STATIC_NEW(T)                  \
                                                    \
static p_##T New()                                  \
{                                                   \
    return p_##T(new T);                            \
}                                                   \
                                                    \
std::shared_ptr<T> shared_from_this()               \
{                                                   \
    return std::static_pointer_cast<T>(             \
        dagr_dataset::shared_from_this());          \
}                                                   \
                                                    \
std::shared_ptr<T const> shared_from_this() const   \
{                                                   \
    return std::static_pointer_cast<T const>(       \
        dagr_dataset::shared_from_this());          \
}

// convenience macro implementing new_instance method
#define DAGR_DATASET_NEW_INSTANCE()                 \
virtual p_dagr_dataset new_instance() const override\
{                                                   \
    return this->New();                             \
}

// convenience macro implementing new_copy method
#define DAGR_DATASET_NEW_COPY()                     \
virtual p_dagr_dataset new_copy() const override    \
{                                                   \
    p_dagr_dataset o = this->new_instance();        \
    o->copy(this->shared_from_this());              \
    return o;                                       \
}

// convenience macro for adding properties to dataset
// objects
#define DAGR_DATASET_PROPERTY(T, name)  \
                                        \
void set_##name(const T &val)           \
{                                       \
    this->name = val;                   \
}                                       \
                                        \
const T &get_##name() const             \
{                                       \
    return this->name;                  \
}                                       \
                                        \
T &get_##name()                         \
{                                       \
    return this->name;                  \
}

// convenience set get methods for dataset metadata
#define DAGR_DATASET_METADATA(key, T, len)          \
DAGR_DATASET_METADATA_V(T, key, len)                \
DAGR_DATASET_METADATA_A(T, key, len)                \
DAGR_DATASET_METADATA_ ## len (T, key)


#define DAGR_DATASET_METADATA_1(T, key)             \
void set_##key(const T & val_1)                     \
{                                                   \
    this->get_metadata().insert<T>(#key, val_1);    \
}                                                   \
                                                    \
int get_##key(T &val_1) const                       \
{                                                   \
    return this->get_metadata().get<T>(             \
        #key, val_1);                               \
}

#define DAGR_DATASET_METADATA_2(T, key)     \
void set_##key(const T & val_1, const T & val_2)    \
{                                                   \
    this->get_metadata().insert<T>(                 \
        #key, {val_1, val_2});                      \
}                                                   \
                                                    \
int get_##key(T &val_1, T &val_2) const             \
{                                                   \
    std::vector<T> vals;                            \
    if (this->get_metadata().get<T>(#key, vals))    \
        return -1;                                  \
    val_1 = vals[0];                                \
    val_2 = vals[1];                                \
    return 0;                                       \
}

#define DAGR_DATASET_METADATA_3(T, key)     \
void set_##key(const T & val_1, const T & val_2,    \
    const T & val_3)                                \
{                                                   \
    this->get_metadata().insert<T>(#key,            \
        {val_1, val_2, val_3});                     \
}                                                   \
                                                    \
int get_##key(T &val_1, T &val_2, T &val_3) const   \
{                                                   \
    std::vector<T> vals;                            \
    if (this->get_metadata().get<T>(#key, vals))    \
        return -1;                                  \
    val_1 = vals[0];                                \
    val_2 = vals[1];                                \
    val_3 = vals[2];                                \
    return 0;                                       \
}

#define DAGR_DATASET_METADATA_4(T, key)             \
void set_##key(const T & val_1, const T & val_2,    \
    const T & val_3, const T & val_4)               \
{                                                   \
    this->get_metadata().insert<T>(#key,            \
         {val_1, val_2, val_3, val_4});             \
}

#define DAGR_DATASET_METADATA_6(T, key)             \
void set_##key(const T & val_1, const T & val_2,    \
    const T & val_3, const T & val_4,               \
    const T & val_5, const T & val_6)               \
{                                                   \
    this->get_metadata().insert<T>(#key,            \
        {val_1, val_2, val_3,                       \
        val_4, val_5, val_6});                      \
}

#define DAGR_DATASET_METADATA_8(T, key)             \
void set_##key(const T & val_1, const T & val_2,    \
    const T & val_3, const T & val_4,               \
    const T & val_5, const T & val_6,               \
    const T & val_7, const T & val_8)               \
{                                                   \
    this->get_metadata().insert<T>(#key,            \
        {val_1, val_2, val_3, val_4, val_5,         \
         val_6, val_7, val_8});                     \
}

#define DAGR_DATASET_METADATA_V(T, key, len)            \
void set_##key(const std::vector<T> &vals)              \
{                                                       \
    if (vals.size() != len)                             \
    {                                                   \
        DAGR_ERROR(#key " requires " #len " values")    \
    }                                                   \
    this->get_metadata().insert<T>(#key, vals);         \
}                                                       \
                                                        \
int get_##key(std::vector<T> &vals) const               \
{                                                       \
    return this->get_metadata().get<T>(#key, vals);     \
}                                                       \
                                                        \
void set_##key(const p_dagr_variant_array &vals)        \
{                                                       \
    if (vals->size() != len)                            \
    {                                                   \
        DAGR_ERROR(#key " requires " #len " values")    \
    }                                                   \
    this->get_metadata().insert(#key, vals);            \
}                                                       \
                                                        \
int get_##key(p_dagr_variant_array vals) const          \
{                                                       \
    return this->get_metadata().get(#key, vals);        \
}                                                       \
                                                        \
void set_##key(const std::initializer_list<T> &l)       \
{                                                       \
    std::vector<T> vals(l);                             \
    if (vals.size() != len)                             \
    {                                                   \
        DAGR_ERROR(#key " requires " #len " values")    \
    }                                                   \
    this->get_metadata().insert<T>(#key, vals);         \
}                                                       \

#define DAGR_DATASET_METADATA_A(T, key, len)            \
void set_##key(const T *vals)                           \
{                                                       \
    this->get_metadata().insert<T>(#key, vals, len);    \
}                                                       \
                                                        \
int get_##key(T *vals) const                            \
{                                                       \
    return this->get_metadata().get<T>(                 \
        #key, vals, len);                               \
}

#endif
