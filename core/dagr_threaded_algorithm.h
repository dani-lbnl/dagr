#ifndef dagr_threaded_algorithm_h
#define dagr_threaded_algorithm_h

#include "dagr_algorithm.h"
#include "dagr_threaded_algorithm_fwd.h"
#include "dagr_dataset.h"
class dagr_metadata;
class dagr_threaded_algorithm_internals;

#include "dagr_algorithm_output_port.h"

// this is the base class defining a threaded algorithm.
// the stratgey employed is to parallelize over upstream
// data requests using a thread pool.
class dagr_threaded_algorithm : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_threaded_algorithm)
    DAGR_ALGORITHM_DELETE_COPY_ASSIGN(dagr_threaded_algorithm)
    virtual ~dagr_threaded_algorithm() noexcept;

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set/get the number of threads in the pool. setting
    // to -1 results in a thread per core factoring in all MPI
    // ranks running on the node. the default is -1.
    void set_thread_pool_size(int n_threads);
    unsigned int get_thread_pool_size() const noexcept;

    // set/get the verbosity level.
    DAGR_ALGORITHM_PROPERTY(int, verbose);

    // set/get thread affinity mode. When 0 threads are not bound
    // CPU cores, allowing for migration among all cores. This will
    // likely degrade performance. Default is 1.
    DAGR_ALGORITHM_PROPERTY(int, bind_threads);

protected:
    dagr_threaded_algorithm();

    // driver function that manages execution of the given
    // requst on the named port. each upstream request issued
    // will be executed by the thread pool.
    const_p_dagr_dataset request_data(dagr_algorithm_output_port &port,
        const dagr_metadata &request) override;

private:
    int verbose;
    int bind_threads;
    dagr_threaded_algorithm_internals *internals;
};

#endif
