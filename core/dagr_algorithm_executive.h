#ifndef dagr_algorithm_executive_h
#define dagr_algorithm_executive_h

#include "dagr_algorithm_executive_fwd.h"
#include "dagr_metadata.h"

// base class and default implementation for executives. algorithm
// executives can control pipeline execution by providing a series
// of requests. this allows for the executive to act as a load
// balancer. the executive can for example partition requests across
// spatial data, time steps, or file names. in an MPI parallel
// setting the executive could coordinate this partitioning amongst
// the ranks. However, the only requirement of an algorithm executive
// is that it provide at least one non-empty request.
//
// the default implementation creates a single trivially non-empty
// request containing the key "__request_empty = 0". This will cause
// the pipeline to be executed once but will result in no data being
// requested. Therefore when the default implementation is used
// upstream algorithms must fill in the requests further to pull
// data as needed.
class dagr_algorithm_executive
    : public std::enable_shared_from_this<dagr_algorithm_executive>
{
public:
    static p_dagr_algorithm_executive New()
    { return p_dagr_algorithm_executive(new dagr_algorithm_executive); }

    virtual ~dagr_algorithm_executive() {}

    // initialize requests from the given metadata object.
    // this is a place where work partitioning across MPI
    // ranks can occur
    virtual int initialize(const dagr_metadata &md);

    // get the next request until all requests have been
    // processed. an empty request is returned.
    virtual dagr_metadata get_next_request();

protected:
    dagr_algorithm_executive() = default;
    dagr_algorithm_executive(const dagr_algorithm_executive &) = default;
    dagr_algorithm_executive(dagr_algorithm_executive &&) = default;
    dagr_algorithm_executive &operator=(const dagr_algorithm_executive &) = default;
    dagr_algorithm_executive &operator=(dagr_algorithm_executive &&) = default;

private:
    dagr_metadata m_md;
};

#endif
