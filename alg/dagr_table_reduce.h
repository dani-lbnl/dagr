#ifndef dagr_table_reduce_h
#define dagr_table_reduce_h

#include "dagr_shared_object.h"
#include "dagr_dataset_fwd.h"
#include "dagr_metadata.h"
#include "dagr_temporal_reduction.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_table_reduce)

// a reduction on tabular data over time steps
/**
a reduction on tabular data over time steps.
tabular data from each time step is collected and
concatenated into a big table.
*/
class dagr_table_reduce : public dagr_temporal_reduction
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_table_reduce)
    ~dagr_table_reduce(){}

protected:
    dagr_table_reduce();

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
};

#endif
