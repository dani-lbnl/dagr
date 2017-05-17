#ifndef array_time_average_h
#define array_time_average_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(array_time_average)

/**
an example implementation of a dagr_algorithm
that avergaes n timesteps
*/
class array_time_average : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(array_time_average)
    ~array_time_average();

    // set the name of the array to average
    DAGR_ALGORITHM_PROPERTY(std::string, array_name)

    // set the number of steps to average. should be odd.
    DAGR_ALGORITHM_PROPERTY(unsigned int, filter_width)

protected:
    array_time_average();

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
    std::string array_name;
    unsigned int filter_width;
};

#endif
