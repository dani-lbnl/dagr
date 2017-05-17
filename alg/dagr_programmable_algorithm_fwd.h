#ifndef dagr_program_algorithm_fwd_h
#define dagr_program_algorithm_fwd_h

#include "dagr_shared_object.h"
#include "dagr_metadata.h"
#include "dagr_dataset_fwd.h"

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_programmable_algorithm)

#ifdef SWIG
typedef void* report_callback_t;
typedef void* request_callback_t;
typedef void* execute_callback_t;
#else
using report_callback_t = std::function<dagr_metadata(
        unsigned int, const std::vector<dagr_metadata>&)>;

using request_callback_t = std::function<std::vector<dagr_metadata>(
        unsigned int, const std::vector<dagr_metadata> &,
        const dagr_metadata &)>;

using execute_callback_t = std::function<const_p_dagr_dataset(
        unsigned int, const std::vector<const_p_dagr_dataset> &,
        const dagr_metadata &)>;
#endif
#endif
