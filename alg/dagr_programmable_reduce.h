#ifndef dagr_programmable_reduce_h
#define dagr_programmable_reduce_h

#include "dagr_programmable_reduce_fwd.h"
#include "dagr_programmable_algorithm_fwd.h"
#include "dagr_temporal_reduction.h"
#include "dagr_dataset_fwd.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

// callbacks implement a user defined reduction over time steps
/**
callbacks implement a reduction on dagr_datasets over time steps.
user provides reduce callable that takes 2 datasets and produces
a thrid reduced dataset. callbacks should be threadsafe as this is
a parallel operation. see dagr_temporal_reduction for details of
parallelization.
*/
class dagr_programmable_reduce : public dagr_temporal_reduction
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_programmable_reduce)
    ~dagr_programmable_reduce(){}

    // set the callback that initializes the output metadata during
    // report phase of the pipeline. The callback must be a callable
    // with the signature:
    //
    // dagr_metadata report_callback(unsigned int port,
    //    const std::vector<dagr_metadata> &input_md);
    //
    // the default implementation forwards downstream
    DAGR_ALGORITHM_CALLBACK_PROPERTY(report_callback_t, report_callback)

    // set the callback that initializes the upstream request.
    // The callback must be a callable with the signature:
    //
    // std::vector<dagr_metadata> request(
    //    unsigned int port, const std::vector<dagr_metadata> &input_md,
    //    const dagr_metadata &request) override;
    //
    // the default implementation forwards upstream
    DAGR_ALGORITHM_CALLBACK_PROPERTY(request_callback_t, request_callback)

    // set the callback that performs the reduction on 2 datasets
    // returning the reduced dataset. The callback must be a callable
    // with the signature:
    //
    // p_dagr_dataset reduce(const const_p_dagr_dataset &left,
    //    const const_p_dagr_dataset &right);
    //
    // the default implementation returns a nullptr
    DAGR_ALGORITHM_CALLBACK_PROPERTY(reduce_callback_t, reduce_callback)

protected:
    dagr_programmable_reduce();

    // overrides
    p_dagr_dataset reduce(const const_p_dagr_dataset &left,
        const const_p_dagr_dataset &right) override;

    std::vector<dagr_metadata> initialize_upstream_request(
        unsigned int port, const std::vector<dagr_metadata> &input_md,
        const dagr_metadata &request) override;

    dagr_metadata initialize_output_metadata(unsigned int port,
        const std::vector<dagr_metadata> &input_md) override;

private:
    reduce_callback_t reduce_callback;
    request_callback_t request_callback;
    report_callback_t report_callback;
};

#endif
