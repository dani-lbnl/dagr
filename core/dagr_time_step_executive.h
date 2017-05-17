#ifndef dagr_time_step_executive_h
#define dagr_time_step_executive_h

#include "dagr_shared_object.h"
#include "dagr_algorithm_executive.h"
#include "dagr_metadata.h"

#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_time_step_executive)

///
/**
An executive that generates a request for a series of
timesteps. an extent can be optionally set.
*/
class dagr_time_step_executive : public dagr_algorithm_executive
{
public:
    DAGR_ALGORITHM_EXECUTIVE_STATIC_NEW(dagr_time_step_executive)

    virtual int initialize(const dagr_metadata &md);
    virtual dagr_metadata get_next_request();

    // set the time step to process
    void set_step(long s);

    // set the first time step in the series to process.
    // default is 0.
    void set_first_step(long s);

    // set the last time step in the series to process.
    // default is -1. negative number results in the last
    // available time step being used.
    void set_last_step(long s);

    // set the stride to process time steps at. default
    // is 1
    void set_stride(long s);

    // set the extent to process. the default is the
    // whole_extent.
    void set_extent(unsigned long *ext);
    void set_extent(const std::vector<unsigned long> &ext);

    // set the list of arrays to process
    void set_arrays(const std::vector<std::string> &arrays);

protected:
    dagr_time_step_executive();

private:
    std::vector<dagr_metadata> requests;
    long first_step;
    long last_step;
    long stride;
    std::vector<unsigned long> extent;
    std::vector<std::string> arrays;
};

#endif
