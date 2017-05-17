#ifndef dagr_program_reduce_fwd_h
#define dagr_program_reduce_fwd_h

#include "dagr_shared_object.h"
#include "dagr_dataset_fwd.h"
#include <functional>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_programmable_reduce)

#ifdef SWIG
typedef void* reduce_callback_t;
#else
using reduce_callback_t = std::function<p_dagr_dataset(
    const const_p_dagr_dataset &, const const_p_dagr_dataset &)>;
#endif
#endif
