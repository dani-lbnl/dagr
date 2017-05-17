#ifndef array_temporal_stats_h
#define array_temporal_stats_h

#include "dagr_shared_object.h"
#include "array_fwd.h"

#include "dagr_temporal_reduction.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(array_temporal_stats)

/** example demonstarting a temporal reduction. min, average
 and max are computed over time steps for the named array.
*/
class array_temporal_stats : public dagr_temporal_reduction
{
public:
    DAGR_ALGORITHM_STATIC_NEW(array_temporal_stats)
    ~array_temporal_stats(){}

    // set the array to process
    DAGR_ALGORITHM_PROPERTY(std::string, array_name)

private:
    // helpers
    p_array new_stats_array();
    p_array new_stats_array(const_p_array input);
    p_array new_stats_array(const_p_array l_input, const_p_array r_input);

protected:
    array_temporal_stats();

    // overrides
    p_dagr_dataset reduce(
        const const_p_dagr_dataset &left,
        const const_p_dagr_dataset &right) override;

    std::vector<dagr_metadata> initialize_upstream_request(
        unsigned int port,
        const std::vector<dagr_metadata> &input_md,
        const dagr_metadata &request) override;

    dagr_metadata initialize_output_metadata(
        unsigned int port,
        const std::vector<dagr_metadata> &input_md) override;

private:
    std::string array_name;
};

#endif
