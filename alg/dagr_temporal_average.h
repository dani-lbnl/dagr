#ifndef dagr_temporal_average_h
#define dagr_temporal_average_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_temporal_average)

/// an algorithm that averages data in time
/**
an algorithm that averages data in time. filter_width
controls the number of time steps to average over.
all arrays in the input data are processed.
*/
class dagr_temporal_average : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_temporal_average)
    ~dagr_temporal_average();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set the number of steps to average. should be odd.
    DAGR_ALGORITHM_PROPERTY(unsigned int, filter_width)


    // select the filter stencil, default is backward
    enum {
        backward,
        centered,
        forward
    };
    DAGR_ALGORITHM_PROPERTY(int, filter_type)

protected:
    dagr_temporal_average();

private:
    std::vector<dagr_metadata> get_upstream_request(
        unsigned int port,
        const std::vector<dagr_metadata> &input_md,
        const dagr_metadata &request) override;

    const_p_dagr_dataset execute(
        unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    unsigned int filter_width;
    int filter_type;
};

#endif
